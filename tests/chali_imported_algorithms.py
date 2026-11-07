"""实际执行算法短名、显式类型实参和自动推导，并检查作用域与错误边界。"""
from pathlib import Path
import argparse
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """使用当前构件验证短名与限定名产生相同结果。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='imported-algorithms-') as directory:
        work = Path(directory)
        for index, expression in enumerate(('foldValue(items, 0, add)',
                                           'foldValue<int64>(items, 0, add)',
                                           'std::algorithms::foldValue(items, 0, add)',
                                           'std::algorithms::foldValue<int64>(items, 0, add)')):
            source = work / f'case{index}.chl'
            source.write_text('import std::algorithms;\n'
                'int64 add(int64 a, int64 b) { return a + b; }\n'
                'void main() { List<int64> items; items.push(7); items.push(5); '
                f'assert({expression} == 12); '
                'assert(sortValue(items)[0] == 5); println("OK"); }', encoding='utf-8')
            result = subprocess.run([str(args.chali.resolve()), 'run', str(source), '--out', str(work / f'case{index}')],
                                    cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=180)
            assert result.returncode == 0 and 'OK' in result.stdout, (expression, result.stdout, result.stderr)
        negative = [
            ('void main() { List<int64> items; foldValue(items, 0, add); }', ''),
            ('import std::algorithms; void main() { List<int64> items; foldValue(items, 0, wrong); }',
             'float64 wrong(float64 a, float64 b) { return a + b; }'),
        ]
        for index, (body, extra) in enumerate(negative):
            source = work / f'negative{index}.chl'
            source.write_text('int64 add(int64 a, int64 b) { return a + b; }\n' + extra + '\n' + body, encoding='utf-8')
            result = subprocess.run([str(args.chali.resolve()), 'check', str(source)],
                                    cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=180)
            assert result.returncode != 0, (source, result.stdout, result.stderr)
        print('算法短名：4 组实际执行及 2 组未导入/类型冲突反例通过')


if __name__ == '__main__':
    main()
