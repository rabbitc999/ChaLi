"""对照正则 C 帧迁移前输出，核对拥有值、限制、错误及四种原生配置。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """保持字节位置、捕获组缩窄及空匹配的既有结果。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    expected = json.loads((ROOT / 'tests/fixtures/regex_compatibility.json').read_text(encoding='utf-8'))['output']
    with tempfile.TemporaryDirectory(prefix='regex-c-api-') as directory:
        work = Path(directory)

        def run(command, success=True):
            result = subprocess.run(list(map(str, command)), cwd=ROOT,
                                    env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                    text=True, encoding='utf-8', errors='replace', timeout=300)
            assert (result.returncode == 0) == success, (command, result.returncode, result.stdout, result.stderr)
            return result

        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                target = work / f'{allocator}-{checks}.exe'
                run([compiler, 'build', ROOT / 'tests/fixtures/regex_compatibility.chl',
                     '--allocator', allocator, '--' + checks, '--out', target])
                result = run([target])
                assert result.stdout == expected and not result.stderr, (allocator, checks, result)
                print(f'{allocator}/{checks}: frozen matches, captures, replacements and owned copies agree', flush=True)
        for sample in ('regex_limits', 'integer_radix_regex'):
            result = run([compiler, 'run', ROOT / f'example/samples/{sample}.chl', '--out', work / sample])
            assert result.stdout == (ROOT / f'example/expected/{sample}.txt').read_text(encoding='utf-8') and not result.stderr, result
        invalid = work / 'invalid.chl'
        invalid.write_text('import std::regex;\nvoid main(){std::regex::Regex rule;rule.contains("x");}', encoding='utf-8')
        result = run([compiler, 'run', invalid, '--out', work / 'invalid'], False)
        assert '正则表达式无效；请先调用 std::regex::Regex::compile' in result.stderr.splitlines()[0], result.stderr
        print('long patterns, 300 compiled patterns, lifetime and invalid receiver diagnostics passed')


if __name__ == '__main__':
    main()
