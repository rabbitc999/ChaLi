"""核对自举矩阵列的全部已有标量类型、独立性、迭代及原始诊断。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
BAD = (
    ('a.column(-1);', 'Matrix.column 列索引越界或维度无效'),
    ('a.column(1);', 'Matrix.column 列索引越界或维度无效'),
    ('a.rows=-1;a.column(0);', 'Matrix.column 列索引越界或维度无效'),
    ('a.cols=0;a.column(0);', 'Matrix.column 列索引越界或维度无效'),
    ('a.rows=2;a.column(0);', 'Matrix.column 存储长度与维度不匹配'),
    ('a.values.clear();a.column(0);', 'Matrix.column 存储长度与维度不匹配'),
)


def main():
    """对照迁移前冻结的诊断，并运行四种原生配置。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--record-baseline', action='store_true')
    options = parser.parse_args()
    compiler = options.chali.resolve()
    baseline = ROOT / 'tests/fixtures/linalg_column_diagnostics.json'
    with tempfile.TemporaryDirectory(prefix='linalg-column-') as directory:
        work = Path(directory)

        def run(command, success=True):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'),
                                    capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=300)
            assert (result.returncode == 0) == success, (command, result.returncode, result.stdout, result.stderr)
            return result

        if options.record_baseline:
            import hashlib
            digest = hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert digest == '4e7a8a0639e3968eaa5f9271bbaa1c5a0e0748e666d3f09380e54bd6850c4aab'
            result = run([compiler, 'run', ROOT / 'tests/fixtures/linalg_column_compatibility.chl', '--out', work / 'positive'])
            assert result.stdout == 'ten scalar columns, iteration and independent storage OK\n' and not result.stderr, result
        expected = [] if options.record_baseline else json.loads(baseline.read_text(encoding='utf-8'))['diagnostics']
        if not options.record_baseline:
            for allocator in ('malloc', 'pool'):
                for checks in ('checked', 'unchecked'):
                    target = work / f'{allocator}-{checks}.exe'
                    run([compiler, 'aot', ROOT / 'tests/fixtures/linalg_column_compatibility.chl',
                         '--allocator', allocator, '--' + checks, '--out', target])
                    result = run([target])
                    assert result.stdout == 'ten scalar columns, iteration and independent storage OK\n' and not result.stderr, result
                    print(f'{allocator}/{checks}: ten scalar columns, uint64 maximum, iteration and independent copies passed', flush=True)
        for index, (setup, diagnostic) in enumerate(BAD):
            path = work / f'bad-{index}.chl'
            path.write_text('import std::linalg;\nvoid main(){let a=std::linalg::Matrix<float64>::identity(1);' + setup + '}', encoding='utf-8')
            result = run([compiler, 'run', path, '--out', work / f'bad-{index}'], False)
            first = result.stderr.splitlines()[0]
            assert diagnostic in first, result
            if options.record_baseline:
                expected.append(first)
            else:
                assert first == expected[index], (first, expected[index])
        if options.record_baseline:
            baseline.write_text(json.dumps(dict(source_commit='0df13e4d', compiler_sha256=digest, diagnostics=expected), ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            print('old ten-scalar columns and six original diagnostics verified')
        else:
            print('six invalid key, dimension and storage diagnostics match frozen version')


if __name__ == '__main__':
    main()
