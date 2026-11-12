"""使用普通源文件验证目标宏、布尔标志、工程清单与缓存隔离。"""
import argparse
import os
import re
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行源文件样例，并验证无效用法与目标选择。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_QUIET='1', CHALI_LANG='zh')
    fixture = ROOT / 'example/feature_tests/build_options'
    with tempfile.TemporaryDirectory(prefix='target-options-') as directory:
        output = Path(directory) / 'program'

        def invoke(arguments, cwd=ROOT, success=True):
            result = subprocess.run([str(compiler), *map(str, arguments)], cwd=cwd, env=env,
                                    encoding='utf-8', errors='replace', capture_output=True, timeout=180)
            assert (result.returncode == 0) == success, (arguments, result.returncode, result.stdout, result.stderr)
            return result

        source = ROOT / 'example/samples/macro_build_options.chl'
        for flags, expected in [([], 'A=0, B=0'), (['-B', 'A'], 'A=1, B=0'),
                                (['-BA', '-BB', '-BA'], 'A=1, B=1'), ([], 'A=0, B=0')]:
            result = invoke(['run', source, '--incremental', '--out', output, *flags])
            assert result.stdout.strip() == expected + ', absent=0', result
        invoke(['check'], cwd=fixture)
        invoke(['check'], cwd=fixture / 'check_alias')
        for flags, expected in [([], 'A=1, B=0'), (['-BB'], 'A=1, B=1')]:
            result = invoke(['run', '--out', output, *flags], cwd=fixture)
            assert result.stdout.strip() == expected + ', absent=0', result
        for flags in [[], ['-BFEATURE_A']]:
            result = invoke(['run', ROOT / 'example/samples/macro_target_environment.chl', '--out', output, *flags])
            assert result.stdout.strip() == 'target macros ok', result
        source = ROOT / 'example/samples/source_compile_if.chl'
        for flags, expected in [([], '1'), (['-BFEATURE_A'], '2'), ([], '1')]:
            result = invoke(['run', source, '--incremental', '--out', output, *flags])
            assert result.stdout.strip() == expected, result
        source = ROOT / 'example/samples/source_compile_if_nested.chl'
        for flags, expected in [([], '0'), (['-BA'], '2'), (['-BB'], '1'),
                                (['-BA', '-BB'], '3'), ([], '0')]:
            result = invoke(['run', source, '--incremental', '--out', output, *flags])
            assert result.stdout.strip() == expected, result
        book_project = fixture / 'book_project'
        book_output = Path(directory) / 'book'
        invoke(['check'], cwd=book_project)
        result = invoke(['run', '--out', book_output], cwd=book_project)
        assert result.stdout.strip() == 'book toml ok', result
        invoke(['build', '--emit-llvm', '--out', book_output], cwd=book_project)
        assert book_output.with_suffix('.ll').is_file()
        for name in ['A=false', '', 'A B']:
            invoke(['check', source, '-B', name], success=False)
        for source in sorted((fixture / 'errors').glob('*.chl')):
            invoke(['check', source], success=False)
        # LLVM 导出无需目标系统链接器，可直接核对目标元数据常量。
        for target, system, cpu in [('windows-x64', 'win', 'amd64'), ('linux-x64', 'linux', 'amd64'),
                                    ('linux-arm64', 'linux', 'arm64'), ('linux-x86', 'linux', 'x86'),
                                    ('linux-riscv32', 'linux', 'riscv32'), ('linux-riscv64', 'linux', 'riscv64'), ('linux-mipsel', 'linux', 'mipsel'),
                                    ('linux-arm32', 'linux', 'arm32'), ('linux-mips64el', 'linux', 'mips64el')]:
            source = ROOT / 'example/feature_tests/build_options/target.chl'
            invoke(['build', source, '--target', target, '--emit-llvm', '--out', output])
            text = output.with_suffix('.ll').read_text(encoding='utf-8')
            literals = [bytes.fromhex(value.replace('\\', '')).rstrip(b'\0').decode('utf-8', errors='replace')
                        for value in re.findall(r'c"((?:\\[0-9A-Fa-f]{2})+)"', text)]
            strings = {part for literal in literals for part in literal.split(chr(0))}
            assert system in strings and cpu in strings, (target, system, cpu, strings)
    print('目标宏、-B、build_option、拒绝用法与增量缓存样例通过。')


if __name__ == '__main__':
    main()
