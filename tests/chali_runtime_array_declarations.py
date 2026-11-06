"""验证类型侧与名称侧运行时数组声明的传参、复制、清理和错误边界。"""
import argparse
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]

PROGRAM = '''int64 drops = 0;
struct Item : Drop {
    String text;
    void drop() { drops += 1; }
}
int64 readTypeSide(int64[] values) { return values[0]; }
int64 readNameSide(int64 values[]) { return values[0]; }
void changeCopy(String values[]) { values[0] = "copy"; }
void changeBorrow(String[] mut& values) { values[0] = "borrow"; }
String[] make() {
    return { String[] local = new[1]; local[0] = "result"; yield local; };
}
void owned() {
    Item[] first = new[2];
    first[0].text = "alpha";
    Item second[] = first;
    second[0].text = "beta";
    assert(first[0].text == "alpha" && second[0].text == "beta");
    program::Item[] qualified = new[1];
    qualified[0].text = "qualified";
    assert(qualified[0].text == "qualified");
}
void main() {
    int64[] first = new[2], second = new[1];
    first[0] = 7;
    second[0] = 8;
    int64 third[] = first;
    third[0] = 9;
    assert(first[0] == 7 && third[0] == 9);
    assert(readTypeSide(first) == 7 && readNameSide(first) == 7);
    assert(readTypeSide(third) == 9 && readNameSide(third) == 9);
    assert(readTypeSide(second) == 8);
    const int64[] frozen = first;
    assert(frozen[0] == 7);
    String[] words = new[1];
    words[0] = "original";
    changeCopy(words);
    assert(words[0] == "original");
    changeBorrow(&words);
    assert(words[0] == "borrow");
    String[] mut& borrowed = &words;
    borrowed[0] = "referenced";
    assert(words[0] == "referenced");
    String returned[] = make();
    assert(returned[0] == "result");
    const String[] fixedWords = words;
    assert(fixedWords[0] == "referenced");
    List<String>[] nested = new[1];
    nested[0].push("generic");
    assert(nested[0][0] == "generic");
    (int64, String)[] pairs = new[1];
    pairs[0] = (3, "tuple");
    assert(pairs[0][0] == 3 && pairs[0][1] == "tuple");
    int64[] empty = new[0];
    assert(len(empty) == 0);
    owned();
    assert(drops == 5);
    println("runtime array declarations passed");
}
'''


def main():
    """用实际编译器验证两种声明走相同所有权路径，非法借用仍被拒绝。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='runtime-array-declarations-') as directory:
        work = Path(directory)
        source = work / 'declarations.chl'
        source.write_text(PROGRAM, encoding='utf-8')
        for backend in ('run', 'build'):
            output = work / (backend + ('.exe' if os.name == 'nt' else ''))
            result = subprocess.run([str(compiler), backend, str(source), '--out', str(output)],
                                    cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=300)
            assert result.returncode == 0, result
            if backend == 'build':
                result = subprocess.run([str(output)], cwd=work, env=env, capture_output=True,
                                        text=True, encoding='utf-8', timeout=60)
            assert (result.returncode, result.stdout, result.stderr) == (0, 'runtime array declarations passed\n', ''), result
        failures = (
            ('void main(){int64[] values;}', '运行时数组必须初始化'),
            ('void main(){int64[] values=7;}', '运行时数组初始化需要'),
            ('void main(){const int64[] values=new[1];values[0]=1;}', None),
            ('int64[] const& bad(){int64[] values=new[1];return &values;} void main(){}', 'CHL1204'),
            ('void main(){int64[] values=new[1];let taken=move values;}', None),
        )
        for index, (code, expected) in enumerate(failures):
            source.write_text(code, encoding='utf-8')
            result = subprocess.run([str(compiler), 'check', str(source)], cwd=ROOT, env=env,
                                    capture_output=True, text=True, encoding='utf-8', timeout=120)
            assert result.returncode != 0, (index, result)
            if expected:
                assert expected in result.stderr, (index, result)
        source.write_text('void f(int64[] values){} void f(int64 values[]){} void main(){}', encoding='utf-8')
        duplicate = subprocess.run([str(compiler), 'check', str(source)], cwd=ROOT, env=env,
                                   capture_output=True, text=True, encoding='utf-8', timeout=120)
        assert duplicate.returncode == 0 and '函数 f 重复定义' in duplicate.stderr, duplicate
    print('运行时数组声明：checked run、unchecked AOT、双形参等价、独立复制、清理与五项拒绝边界通过')


if __name__ == '__main__':
    main()
