"""验证全部数值类型求解、数组互转、列迭代及真实 checked／unchecked AOT。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
BAD = (
    ('a.set(0,0,0.0);', '奇异或近奇异矩阵'),
    ('a.set(0,0,0.000000000001);', '奇异或近奇异矩阵'),
    ('a.rows=2;', '维度不匹配'),
    ('a.rows=-1;a.cols=-1;', '维度不匹配'),
    ('a.values.clear();', '维度不匹配'),
    ('let col=a[-1];', '列索引越界'),
    ('let col=a[1];', '列索引越界'),
    ('float64 value=rhs[1];', '越界'),
    ('float64 raw[2];let bad=std::linear_algebra::Matrix<float64>::fromArray(raw,2,2,2);', '维度不匹配'),
)


def main():
    """不裁掉任何整数类型；临时文件由项目临时目录统一管理。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='linalg-solve-') as directory:
        work = Path(directory)

        def run(command, success=True):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'),
                                    capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=300)
            assert (result.returncode == 0) == success, (command, result.returncode, result.stdout, result.stderr)
            return result

        expected = (ROOT / 'example/expected/std_linalg_solve_glue.txt').read_text(encoding='utf-8')
        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                target = work / f'{allocator}-{checks}.exe'
                run([compiler, 'build', ROOT / 'example/samples/std_linalg_solve_glue.chl',
                     '--allocator', allocator, '--' + checks, '--out', target])
                result = run([target])
                assert result.stdout == expected and not result.stderr, result
                print(f'{allocator}/{checks}: all ten scalar types, arrays, columns and ownership passed', flush=True)
        for index, (setup, diagnostic) in enumerate(BAD):
            path = work / f'bad-{index}.chl'
            path.write_text('import std::linear_algebra;\nvoid main(){'
                            'let a=std::linear_algebra::Matrix<float64>::identity(1);'
                            'let rhs=std::linear_algebra::Vector<float64>::zeros(1);' + setup +
                            'let result=a.solve(rhs);}', encoding='utf-8')
            result = run([compiler, 'run', path, '--out', work / f'bad-{index}'], False)
            assert diagnostic in result.stdout + result.stderr, result
        print('nine singular, shape, column, array and index rejection cases passed')


if __name__ == '__main__':
    main()
