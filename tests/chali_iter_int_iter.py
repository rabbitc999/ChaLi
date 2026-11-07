"""实际验证整数迭代工厂的新名称、步长、核心区间共存与旧名删除。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """以实际原生产物验证两种调用形式和错误边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='int-iter-') as directory:
        work = Path(directory)
        source = work/'main.chl'
        source.write_text('''import std::iter;
int64 add(int64 a, int64 b) { return a + b; }
void main() {
    assert(intIter(1, 4).fold(0, add) == 6);
    assert(std::iter::intIter(5, 0, -2).fold(0, add) == 9);
    assert(intIter(3, 3).count() == 0);
    assert(intIter(5, 1).count() == 0);
    int64 total = 0;
    for (int64 item in range(1, 4)) { total += item; }
    assert(total == 6);
    println("intIter OK");
}
''', encoding='utf-8')
        result = subprocess.run([str(compiler), 'run', str(source), '--out', str(work/'main')],
                                cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert result.returncode == 0 and result.stdout == 'intIter OK\n', result
        for name, text, mode in (
            ('old', 'import std::iter; void main() { let items = std::iter::range(0, 3); }', 'check'),
            ('zero', 'import std::iter; void main() { let items = intIter(0, 3, 0); }', 'run'),
        ):
            source = work/(name+'.chl'); source.write_text(text, encoding='utf-8')
            arguments = [str(compiler), mode, str(source)]
            if mode == 'run': arguments += ['--out', str(work/name)]
            result = subprocess.run(arguments,
                                    cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=180)
            assert result.returncode != 0, result
            if name == 'zero': assert '迭代器区间步长不能为零' in result.stderr, result.stderr
        print('intIter：短名/限定名、正负步长、空区间、核心 range 共存及旧名拒绝通过')


if __name__ == '__main__':
    main()
