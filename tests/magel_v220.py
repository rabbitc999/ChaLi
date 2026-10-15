"""验证 0.14.0 的显式类型擦除、安全提取及二进制中间产物契约。"""
from pathlib import Path
import argparse
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--magel', type=Path, default=ROOT / ('magel.exe' if os.name == 'nt' else 'magel'))
    compiler = parser.parse_args().magel.resolve()
    checks = 0
    with tempfile.TemporaryDirectory(dir=ROOT / 'build', prefix='v220-') as directory:
        work = Path(directory)

        def run_source(name, code, expected_error=None):
            nonlocal checks
            source = work / (name + '.mgl')
            source.write_text(code, encoding='utf-8')
            result = subprocess.run([str(compiler), 'run', str(source), 'out', str(work / name)],
                                    cwd=ROOT, capture_output=True, encoding='utf-8', errors='replace', timeout=120,
                                    env=dict(os.environ, MAGEL_LANG='zh'))
            if expected_error:
                assert result.returncode != 0 and expected_error in result.stderr, (name, result)
            else:
                assert result.returncode == 0 and not result.stderr, (name, result)
            checks += 1

        for name, code, diagnostic in [
            ('old_object', 'int64 main(){Object box;return 0;}', 'Object'),
            ('old_nameof', 'int64 main(){return nameof(int64);}', 'nameof'),
            ('old_name', 'int64 main(){Any box;return box.name();}', 'name'),
            ('missing_generic', 'struct ErasurePair<A,B>{A a;B b;};int64 main(){ErasurePair<int64> p;return 0;}', '缺少类型实参 B'),
            ('private_check', 'int64 main(){Any box;box.__isTypeTag(1);return 0;}', '仅供核心库'),
            ('readonly_mut', 'int64 g;int64 main(){const Any box=&g;tryGetMut<int64>(&box);return 0;}', 'std::lang::Any const&'),
        ]:
            run_source(name, code, diagnostic)
        # 1.0.0（原 2.5.3）把 `__typeTag(T)` 从核心库内部放开到用户代码：下面的写法原先
        # 会报「内部类型标签仅供核心库与反射桥接使用」，现在用户代码能取任意类型的编译期身份。
        run_source('public_tag', '''struct Cell { int64 value; };
int64 main() {
    assert(__typeTag(Cell)==__typeTag(Cell));
    return 0;
}''')
        run_source('explicit_default', '''struct ErasurePair<A,B=Any>{A a;B b;};
int64 main(){ErasurePair<int64> pair;pair.a=9;assert(pair.a==9);return 0;}''')
        run_source('safe_refs', '''int64 global=7;
int64 main(){int64 const& readonly=&global;Any box=&readonly;
assert(!tryGetMut<int64>(&box).isValue());assert(tryGetReference<int64>(&box).isValue());
box=(int64)9;assert(tryGet<int64>(&box).unwrap()==9);box="中文";
assert(tryGet<String>(&box).unwrap()=="中文");return 0;}''')
        run_source('empty_interface', 'interface Unused{int64 get();};int64 main(){Any box;assert(!tryGetReference<Unused>(&box).isValue());return 0;}')
        run_source('negative_int64', 'int64 main(){int64 x=-62135596800;assert(x < -2147483648);assert(x+62135596800==0);return 0;}')
        prefix = work / 'artifacts'
        source = work / 'artifacts.mgl'
        source.write_text('int64 main(){println("binary artifacts");return 0;}', encoding='utf-8')
        result = subprocess.run([str(compiler), 'emit', str(source), 'out', str(prefix)], cwd=ROOT,
                                capture_output=True, encoding='utf-8', timeout=120)
        assert result.returncode == 0, result.stderr
        assert prefix.with_suffix('.quad').read_bytes().startswith(b'MGQI\x01')
        assert prefix.with_suffix('.layout').read_bytes().startswith(b'MGLY\x01')
        assert prefix.with_suffix('.data.bin').is_file()
        assert not prefix.with_suffix('.data').exists() and not Path(str(prefix)+'.data.h').exists()
        checks += 1
        # 合并六个模块的真实样例，验证 AOT 与解释执行拥有相同的 ABI 和资源释放行为。
        imports, bodies, expected, calls = [], [], '', []
        for name in ('std_encoding', 'std_csv', 'std_datetime', 'std_cli', 'std_utf8', 'std_io_extended'):
            sample = (ROOT / 'example/samples' / (name+'.mgl')).read_text(encoding='utf-8')
            lines = sample.splitlines()
            imports.extend(line for line in lines if line.startswith('import '))
            bodies.append('\n'.join(line for line in lines if not line.startswith('import ')).replace('int64 main()', 'int64 '+name+'()'))
            expected += (ROOT / 'example/expected' / (name+'.txt')).read_text(encoding='utf-8')
            calls.append('assert('+name+'()==0);')
        source = work / 'all_modules.mgl'
        source.write_text('\n'.join(imports+bodies)+ '\nint64 main(){'+''.join(calls)+'return 0;}', encoding='utf-8')
        target = work / ('all_modules.exe' if os.name == 'nt' else 'all_modules')
        result = subprocess.run([str(compiler), 'aot', str(source), 'out', str(target), 'opt', '0'],
                                cwd=ROOT, capture_output=True, encoding='utf-8', errors='replace', timeout=300)
        assert result.returncode == 0, result.stderr
        result = subprocess.run([str(target)], cwd=ROOT, capture_output=True, encoding='utf-8', errors='replace', timeout=60)
        assert result.returncode == 0 and result.stdout == expected and not result.stderr, result
        checks += 1
    print(f'0.14.0 类型与产物契约通过：{checks} 项')


if __name__ == '__main__':
    main()
