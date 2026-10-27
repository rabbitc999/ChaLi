"""AOT 下验证集合传参、返回值与复制语义，并确认标准集合胶水随产物静态链接。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import subprocess
from project_temp import tempfile

SOURCE = """List<int64> transform(List<int64> values, int64 fn(int64) mapper)
{
    for (int64 index in std::lang::range(0, values.len())) { values[index] = mapper(values[index]); }
    return values;
}
Map<int64, int64> copyMap(Map<int64, int64> values) { return values; }
Set<int64> copySet(Set<int64> values) { return values; }

int64 doubleValue(int64 value) { return value * 2; }

void main()
{
    List<int64> source = [1, 2, 3];
    List<int64> transformed = transform(source, doubleValue);
    assert(source[0] == 1 && transformed[0] == 2 && transformed[2] == 6);
    transformed.push(8);
    List<int64> copy = transformed;
    transformed.clear();
    assert(copy.len() == 4 && copy[3] == 8);
    Map<int64, int64> inputMap = {2: 20, 1: 10};
    Map<int64, int64> outputMap = copyMap(inputMap);
    inputMap.clear();
    assert(outputMap.len() == 2 && outputMap[2] == 20 && outputMap[1] == 10);
    Set<int64> inputSet = {3, 1, 3};
    Set<int64> outputSet = copySet(inputSet);
    inputSet.clear();
    assert(outputSet.len() == 2 && outputSet.has(3) != 0 && outputSet.has(1) != 0);
    println("collection values OK");
}
"""

EXPECTED = 'collection values OK\n'


def main():
    """编译同一份源码的 checked / unchecked 两个产物，比较输出与集合语义。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', default=compiler_path())
    options = parser.parse_args()
    compiler = Path(options.chali).resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')

    def run(*arguments):
        """执行构建或产物，失败时保留完整诊断。"""
        result = subprocess.run([str(value) for value in arguments], env=env, capture_output=True,
                                text=True, encoding='utf-8', errors='replace', timeout=600)
        assert result.returncode == 0, result.stdout + result.stderr
        return result

    with tempfile.TemporaryDirectory(prefix='chali-collection-glue-') as folder:
        work = Path(folder)
        source = work / 'collections.chl'
        source.write_text(SOURCE, encoding='utf-8')
        product = Path(str(work / 'program') + ('.exe' if os.name == 'nt' else ''))
        run(compiler, 'aot', source, '--out', work / 'program')
        result = run(product)
        assert result.stdout == EXPECTED, result.stdout
        assert not result.stderr, result.stderr
        checked = run(compiler, 'run', source, '--out', work / 'checked')
        assert checked.stdout == EXPECTED, checked.stdout
    print('AOT collection values: parameters, returns and copies passed')


if __name__ == '__main__':
    main()
