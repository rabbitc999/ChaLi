"""核对核心内存 API 的限定／省略前缀、泛型推导、专门指令与 LLVM intrinsic。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from svmb_format import read_raw, FEATURE_BULK_MEMORY
SOURCE = '''int64 sizeOfCopy(int64 n) { return n; }
void main() {
    @P@RawStorage<int64> values = @P@RawStorage<int64>::allocate(4);
    @P@RawStorage<uint8> bytes = @P@RawStorage<uint8>::allocate(8);
    unsafe {
        @P@BarePtr<int64> p = @P@barePointer<int64>(values.pointer(0));
        @P@BarePtr<uint8> q = @P@barePointer<uint8>(bytes.pointer(0));
        assert((p + 2) - p == 2);
        assert((2 + p).address == (p + 2).address);
        int64 count = sizeOfCopy(8);
        @P@memset@S64@(p, (uint8)0, 32);
        @P@memset@S8@(q, (uint8)42, count);
        @P@memcpy@C@(p, q, count);
        @P@BarePtr<uint8> first = p.castTo<uint8>();
        assert(first.address == p.address && first.castTo<int64>().address == p.address);
        int64 i = 0;
        while (i < count) {
            uint8 mut& value = @P@dereference<uint8>(first + i);
            assert(*value == (uint8)42);
            i = i + 1;
        }
        @P@memcpy(p + 1, q, count);
        @P@memset(p + 2, 0, count);
        @P@memset(q, (uint8)'A', count);
        uint8 mut& letter = @P@dereference<uint8>(q);
        assert(*letter == (uint8)65);
        @P@RawPtr<int64> slot = values.pointer(1);
        @P@initAt<int64>(slot, 99);
        assert(@P@readAt<int64>(slot) == 99);
        int64 moved = @P@takeAt<int64>(slot);
        assert(moved == 99 && !values.isInitialized(1));
        int64 original = 123;
        int64 copied = 0;
        @P@memcpy(&copied, &original, count);
        assert(copied == 123 && original == 123);
        @P@BarePtr<uint8> empty;
        @P@memcpy<uint8,uint8>(empty, empty, 0);
        @P@memset<uint8>(empty, (uint8)0, 0);
    }
    values.close(); bytes.close();
    println("memory intrinsics OK");
}
'''


def main():
    """先核对结果，再检查程序主体的 IR，避免把运行时自带 intrinsic 当作证据。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    clang = os.environ.get('CHALI_LLVM_CLANG', str(ROOT / 'library/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else 'clang')
    work = Path(tempfile.mkdtemp(prefix='memory-intrinsics-'))
    env = dict(os.environ, CHALI_LLVM_CLANG=clang)
    results = []

    def invoke(command, label, allow_stderr=False):
        """执行并保留诊断，不忽略失败或警告。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=900)
        (work / (label + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
        assert result.returncode == 0 and (allow_stderr or not result.stderr), (command, result)
        return result.stdout

    for qualified in [True, False]:
        for explicit in [True, False]:
            name = ('qualified' if qualified else 'implicit-namespace') + ('-explicit-types' if explicit else '-inferred-types')
            text = SOURCE.replace('@P@', 'std::lang::' if qualified else '')
            text = text.replace('@S64@', '<int64>' if explicit else '').replace('@S8@', '<uint8>' if explicit else '').replace('@C@', '<int64,uint8>' if explicit else '')
            source = work / (name + '.chl')
            source.write_text(text, encoding='utf-8')
            assert invoke([compiler, 'run', source, '--out', work / (name + '-vm')], name + '-vm') == 'memory intrinsics OK\n'
            target = work / (name + '-aot')
            invoke([compiler, 'aot', source, '--out', target], name + '-aot-build')
            assert invoke([str(target) + ('.exe' if os.name == 'nt' else '')], name + '-aot-run') == 'memory intrinsics OK\n'
            llvm = target.with_suffix('.ll').read_text(encoding='utf-8')
            body = re.search(r'^define[^\n]*@f_main\([^\n]*\).*?^}', llvm, re.M | re.S)
            assert body, 'main LLVM body missing'
            main_body = body.group(0)
            assert '@llvm.memcpy.p0.p0.i64' in main_body and '@llvm.memset.p0.i64' in main_body
            assert '@chali_host_memory_prepare' in main_body
            assert not re.search(r'\bcall[^\n]*@f_[^\s(]*(?:memcpy__|memset__|dereference__)', llvm), name
            assert not re.search(r'@native_name[^\n]*std::lang::__rawStorage', llvm), name
            optimized = work / (name + '-optimized.ll')
            invoke([clang, '-O3', '-S', '-emit-llvm', target.with_suffix('.ll'), '-o', optimized, '-Wno-override-module'], name + '-optimize')
            optimized_text = optimized.read_text(encoding='utf-8')
            pointer_calls = re.findall(r'\bcall[^\n]*@f_[^\s(]*(?:BarePtr__[^\s(]*[_.](?:leftAdd|leftSub|rightAdd)|barePointer__|readAt__|takeAt__|initAt__)', optimized_text)
            results.append(dict(case=name, vm='passed', aot='passed', llvm_byte_intrinsics='passed',
                                remaining_optimized_pointer_calls=pointer_calls))
            if not qualified and not explicit:
                bytecode = work / 'memory-bytecode'
                invoke([compiler, 'svmb', source, '--out', bytecode], 'bytecode-build', allow_stderr=True)
                product = Path(str(bytecode) + '.svmb')
                metadata = read_raw(product.read_bytes())
                assert metadata['features'] & FEATURE_BULK_MEMORY
                assert {54, 55} <= {instruction['op'] for instruction in metadata['code']}
                vm = work / ('chali-stackvm.exe' if os.name == 'nt' else 'chali-stackvm')
                invoke(['g++', '-std=c++17', '-O2', '-DCHALI_CUSTOM_ALLOCATOR=1', '-DCHALI_WITH_REFLECTION=1',
                        '-DSTACKVM_WITH_REFLECTION=1', '-pthread', '-Isource', '-Ilibrary', '-Isource/stackvm', '-Isource/bignum',
                        '-Isource/allocator', ROOT / 'source/stackvm/main.cpp', ROOT / 'build/chali-stackvm-core/libstackvm.a',
                        ROOT / 'build/chali-host-records/librecords.a', *(['-lwinhttp'] if os.name == 'nt' else []), '-o', vm], 'vm-host-build')
                output = invoke([vm, 'run', product, '--data', bytecode.with_suffix('.data.bin'),
                                 '--layout', bytecode.with_suffix('.layout')], 'bytecode-run')
                assert [line for line in output.splitlines() if line and not line.startswith('; ')] == ['memory intrinsics OK']
            print(name, 'VM/AOT and LLVM intrinsic passed', flush=True)
    (work / 'results.json').write_text(json.dumps(results, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('namespace, memory intrinsic and bytecode checks passed; logs:', work)


if __name__ == '__main__':
    main()
