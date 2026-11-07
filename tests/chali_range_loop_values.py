"""验证区间、三段式 for、集合字面量和循环值表达式的普通源文件样例。"""
from compiler_path import compiler_path
import argparse
from pathlib import Path
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """执行 checked 原生样例与非法语法、协议负例，文件操作限制在临时目录。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', default=compiler_path())
    compiler = Path(parser.parse_args().chali).resolve()
    fixtures = ROOT / 'example/feature_tests/range_loop_values'
    negatives = sorted((fixtures / 'errors').glob('*.chl'))
    assert len(negatives) == 18
    with tempfile.TemporaryDirectory(prefix='chali-range-loop-') as directory:
        work = Path(directory)

        def invoke(*arguments):
            """运行真实工具命令并拒绝泄漏提示，所有运行期文件写入本轮临时目录。"""
            result = subprocess.run([str(compiler), *map(str, arguments)], cwd=work,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=360)
            assert result.returncode == 0, result.stdout + result.stderr
            assert '程序结束时' not in result.stderr, result.stderr
            return result.stdout

        assert 'range and loop values ok' in invoke('run', fixtures / 'values.chl', '--out', work / 'values')
        assert 'loop ownership ok' in invoke('run', fixtures / 'ownership.chl', '--out', work / 'ownership')
        counted = invoke('run', ROOT / 'example/samples/syntax_for_minimal.chl', '--out', work / 'counted')
        assert counted.strip() == (ROOT / 'example/expected/syntax_for_minimal.txt').read_text(encoding='utf-8').strip(), counted
        for source in negatives:
            result = subprocess.run([str(compiler), 'check', str(source)], cwd=work,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=60)
            assert result.returncode != 0, f'{source.name}: invalid source was accepted'
    print(f'range and loop values: 3 native samples and {len(negatives)} rejection samples passed')


if __name__ == '__main__':
    main()
