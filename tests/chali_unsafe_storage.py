"""验证 unsafe 权限隔离、原始槽位状态和资源所有权，覆盖 VM 与 LLVM AOT。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]

RESOURCE = '''import std::memory;
int64 closed = 0;
struct Owned: Closeable {
    String text;
    int64 id;
    void close() { closed = closed + 1; self.id = 0; }
};
void main() {
    std::lang::RawStorage<Owned> slots = std::lang::RawStorage<Owned>::allocate(20);
    assert(closed == 0);
    Owned value;
    value.id = 7; value.text = "resource";
    unsafe {
        std::lang::initAt<Owned>(slots.pointer(0), move(value));
        assert(closed == 0);
        std::lang::relocateNonOverlapping<Owned>(slots.pointer(10), slots.pointer(0), 1);
        assert(closed == 0);
        Owned moved = std::lang::takeAt<Owned>(slots.pointer(10));
        assert(moved.text == "resource" && moved.id == 7 && closed == 0);
        std::lang::initAt<Owned>(slots.pointer(3), move(moved));
        assert(closed == 0);
    }
    slots.close();
    assert(closed == 1);
    println("raw resource OK");
}
'''


def main():
    """对静态拒绝检查诊断，对运行时失败检查槽位错误并保留日志。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--aot', action='store_true')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='unsafe-storage-'))
    env = dict(os.environ, CHALI_LLVM_CLANG=str(ROOT / 'library/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang'))

    def invoke(command, label):
        """保存原始诊断及状态。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=600)
        (work / (label + '.log')).write_text(f'exit={result.returncode}\n' + result.stdout + result.stderr, encoding='utf-8')
        return result

    negatives = {
        'call': 'unsafe int64 raw(){return 7;}void main(){println("{}",raw());}',
        'function_value': 'unsafe int64 raw(){return 7;}void main(){unsafe{int64 fn() f=raw;}}',
        'closure': 'unsafe int64 raw(){return 7;}void main(){unsafe{let f=int64 fn(){return raw();};}}',
        'layout': 'void main(){println("{}",__rawSize(int64));}',
        'integer_pointer': 'void main(){int64 mut& p=(int64 mut&)16;}',
        'native_call': 'import std::memory;\nvoid main(){std::lang::__rawStorage(4,0u,0,0,0,1);}',
        'bare_dereference': 'import std::memory;\nvoid main(){std::lang::BarePtr<int64> p;std::lang::dereference<int64>(p);}',
        'bare_cast': 'void main(){BarePtr<int64> p;BarePtr<uint8> q=p.castTo<uint8>();}',
        'bare_copy': 'import std::memory;\nvoid main(){std::lang::BarePtr<uint8> p;std::lang::memcpy<uint8,uint8>(p,p,0);}',
        'bare_set': 'import std::memory;\nvoid main(){std::lang::BarePtr<uint8> p;std::lang::memset<uint8>(p,(uint8)0,0);}',
        'reference_copy': 'void main(){int64 x=0;int64 y=1;std::lang::memcpy(&x,&y,8);}',
        'legacy_abi': 'abi "none.dll" {int32 f(int32 x);}void main(){}',
        'abi_mode': '#abi("invalid","none.dll","f")int32 f(int32 x);void main(){}',
        'empty_export': '#export("")int32 f(int32 x){return x;}void main(){}',
        'duplicate_export': '#export("same")int32 f(int32 x){return x;}#export("same")int32 g(int32 x){return x;}void main(){}',
    }
    for name, source in negatives.items():
        path = work / (name + '.chl')
        path.write_text(source, encoding='utf-8')
        result = invoke([compiler, 'check', path], name)
        assert result.returncode != 0 and result.returncode not in (-1073741819, 3221225477), (name, result)
        assert result.stdout or result.stderr, name
    positive = [('resource', RESOURCE, 'raw resource OK\n'),
                ('raw_storage', (ROOT / 'example/samples/raw_storage.chl').read_text(encoding='utf-8'), 'raw storage OK\n'),
                ('raw_pointer', (ROOT / 'example/samples/raw_pointer.chl').read_text(encoding='utf-8'), 'raw pointer OK\n'),
                ('raw_containers', (ROOT / 'benchmarks/perf/raw_containers.chl').read_text(encoding='utf-8') + '\n' + (ROOT / 'benchmarks/perf/raw_containers_check.chl').read_text(encoding='utf-8'), 'raw containers OK\n'),
                ('raw_resources', (ROOT / 'benchmarks/perf/raw_containers.chl').read_text(encoding='utf-8') + '\n' + (ROOT / 'benchmarks/perf/raw_resources_check.chl').read_text(encoding='utf-8'), 'raw resource containers OK\n'),
                ('string', (ROOT / 'benchmarks/perf/bootstrap_string.chl').read_text(encoding='utf-8') + '\n' + (ROOT / 'benchmarks/perf/bootstrap_string_check.chl').read_text(encoding='utf-8'), 'bootstrap String OK\n'),
                ('string_assignment_regression', 'void main(){String text="aaa";String needle="b";int64 sum=0;int64 i=0;while(i<32){sum=sum+text.indexOf(needle);i=i+1;}assert(sum==0-32);println("String assignment OK");}', 'String assignment OK\n')]
    failures = {
        'uninitialized': 'std::lang::readAt<int64>(slots.pointer(0));',
        'double_init': 'std::lang::initAt<int64>(slots.pointer(0),1);std::lang::initAt<int64>(slots.pointer(0),2);',
        'double_take': 'std::lang::initAt<int64>(slots.pointer(0),1);std::lang::takeAt<int64>(slots.pointer(0));std::lang::takeAt<int64>(slots.pointer(0));',
        'bounds': 'std::lang::readAt<int64>(slots.pointer(2));',
        'overlap': 'std::lang::initAt<int64>(slots.pointer(0),1);std::lang::relocateNonOverlapping<int64>(slots.pointer(0),slots.pointer(0),1);',
        'stale': 'std::lang::RawPtr<int64> p=slots.pointer(0);slots.close();std::lang::readAt<int64>(p);',
        'byte_overlap': 'std::lang::BarePtr<int64> p=std::lang::barePointer<int64>(slots.pointer(0));std::lang::memcpy<int64,int64>(p,p,1);',
        'byte_partial_overlap': 'std::lang::BarePtr<int64> p=std::lang::barePointer<int64>(slots.pointer(0));std::lang::memcpy<int64,int64>(p+1,p,9);',
        'byte_bounds': 'std::lang::BarePtr<int64> p=std::lang::barePointer<int64>(slots.pointer(0));std::lang::memset<int64>(p,(uint8)0,17);',
        'byte_negative': 'std::lang::BarePtr<int64> p;std::lang::memset<int64>(p,(uint8)0,0-1);',
        'byte_source_bounds': 'std::lang::BarePtr<int64> p=std::lang::barePointer<int64>(slots.pointer(0));std::lang::RawStorage<int64> fresh=std::lang::RawStorage<int64>::allocate(4);std::lang::memcpy<int64,int64>(std::lang::barePointer<int64>(fresh.pointer(0)),p,17);',
        'byte_stale': 'std::lang::BarePtr<int64> p=std::lang::barePointer<int64>(slots.pointer(0));slots.close();std::lang::memset<int64>(p,(uint8)0,1);',
    }
    for backend in (['run', 'aot'] if args.aot else ['run']):
        for name, source, expected in positive:
            path = work / (name + '.chl')
            path.write_text(source, encoding='utf-8')
            target = work / (name + '-' + backend)
            result = invoke([compiler, backend, path, '--out', target], name + '-' + backend + '-build')
            assert result.returncode == 0, (name, backend, result)
            if backend == 'aot':
                result = invoke([str(target) + ('.exe' if os.name == 'nt' else '')], name + '-aot-run')
            assert result.returncode == 0 and result.stdout == expected and not result.stderr, (name, backend, result)
        for name, operation in failures.items():
            path = work / ('failure-' + name + '.chl')
            path.write_text('import std::memory;\nvoid main(){std::lang::RawStorage<int64> slots=std::lang::RawStorage<int64>::allocate(2);unsafe{' + operation + '}}', encoding='utf-8')
            target = work / ('failure-' + name + '-' + backend)
            result = invoke([compiler, backend, path, '--out', target], 'failure-' + name + '-' + backend + '-build')
            if backend == 'aot':
                assert result.returncode == 0, result
                result = invoke([str(target) + ('.exe' if os.name == 'nt' else '')], 'failure-' + name + '-aot-run')
            assert result.returncode != 0 and any(message in result.stderr for message in ('原始', 'assert', 'memcpy')), (name, backend, result)
    print('unsafe storage checks passed; logs:', work)


if __name__ == '__main__':
    main()
