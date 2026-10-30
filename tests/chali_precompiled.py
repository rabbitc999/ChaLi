"""核对标准库核心内存装载、源码回退及无磁盘标准库缓存。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """在隔离目录运行实际编译器，对照启用与关闭内嵌单元的行为。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='precompiled-export-') as directory:
        build = Path(directory)
        units = []
        for name, text in (('short', 'void main(){}\n'),
                           ('long', 'module other;\n' + '\n' * 1000 + 'void main(){List<String> xs;xs.push("input");}\n')):
            source = build / (name + '.chl')
            source.write_text(text, encoding='utf-8')
            output = build / (name + '.unit')
            exported = subprocess.run([str(compiler), 'check', str(source), '--build-stdlib-unit', str(output)],
                                      cwd=build, capture_output=True, encoding='utf-8', timeout=180)
            assert exported.returncode == 0, exported
            units.append(output.read_bytes())
        assert units[0] == units[1], '标准库核心二进制不能随用户路径、模块、类型或输入行数改变'
    with tempfile.TemporaryDirectory(prefix='precompiled-test-') as directory:
        work = Path(directory)
        isolated = work / compiler.name
        shutil.copy2(compiler, isolated)
        env = dict(os.environ, CHALI_LANG='zh', CHALI_LLVM_CLANG=str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe'))
        env.pop('CHALI_NO_STDLIB_CACHE', None)
        # 占据旧默认缓存目录的位置，确认内嵌路径不需要在这里建立目录。
        (work / '.chali').write_text('blocked cache directory', encoding='utf-8')
        cases = [
            'void main(){println("embedded standard unit");}',
            'void main(){List<int64> a;a.push(4);a.push(5);assert(a[0]+a[1]==9);}',
            'import std::parse;\nvoid main(){assert(std::parse::Parse::parseInt64("42").unwrap()==42);}',
            '$macro twice(n)\nn+n\n$end_macro\nint64 calls=0;int64 next(){calls=calls+1;return 3;}void main(){assert($twice(next())==6 && calls==1);}',
        ]
        for index, text in enumerate(cases):
            source = work / f'case{index}.chl'
            source.write_text(text, encoding='utf-8')
            results = []
            for disable in (False, True):
                command = [str(isolated), 'run', str(source), '--out', str(work / f'out{index}-{disable}'), '--stats']
                if disable:
                    command.append('--no-stdlib-cache')
                result = subprocess.run(command, cwd=work, env=env, capture_output=True,
                                        encoding='utf-8', timeout=180)
                assert result.returncode == 0, (index, disable, result)
                if not disable:
                    assert '标准库预编译：命中' in result.stderr, (index, result.stderr)
                results.append(result.stdout)
            assert results[0] == results[1], (index, results)
        for module in sorted((ROOT / 'source/std').glob('*.chl')):
            if module.stem == 'lang':
                continue
            source = work / 'import.chl'
            source.write_text('import std::' + module.stem + ';\nvoid main(){}\n', encoding='utf-8')
            checked = subprocess.run([str(isolated), 'check', str(source), '--stats'], cwd=work,
                                     env=env, capture_output=True, encoding='utf-8', timeout=180)
            assert checked.returncode == 0 and '标准库预编译：命中' in checked.stderr, (module.name, checked)
        for statement in ('int64 value="wrong";', 'std::lang::__std_eprint("private");'):
            source.write_text('void main(){' + statement + '}\n', encoding='utf-8')
            failures = []
            for extra in ([], ['--no-stdlib-cache']):
                rejected = subprocess.run([str(isolated), 'check', str(source), *extra], cwd=work,
                                          env=env, capture_output=True, encoding='utf-8', timeout=180)
                assert rejected.returncode != 0, rejected
                failures.append(rejected.stderr)
            assert failures[0] == failures[1], failures
        assert not list(work.rglob('*.unit'))
        assert not list(work.rglob('chali-stdlib-cache'))
        assert (work / '.chali').read_text(encoding='utf-8') == 'blocked cache directory'
        removed = subprocess.run([str(isolated), 'check', str(source), '--stdlib-cache-dir', str(work / 'old')],
                                 cwd=work, env=env, capture_output=True, encoding='utf-8', timeout=30)
        assert removed.returncode != 0 and '已删除' in removed.stderr and not (work / 'old').exists(), removed
    print('embedded standard core: isolated compiler, imports, generics, macro hygiene and zero disk cache passed')


if __name__ == '__main__':
    main()
