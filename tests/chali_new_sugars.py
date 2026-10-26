"""验证显式类型构造、new 堆分配与数组分配，以及不支持形式的诊断。"""
from compiler_path import compiler_path
from pathlib import Path
import os
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]
COMPILER = Path(os.environ.get('CHALI_TEST_COMPILER', str(compiler_path())))
CASES = {'heap_sugar': '\nstruct Point { int64 x; int64 y; }\nvoid main() {\n    Point mut& p = new Point;\n    p.x = 3; p.y = 4;\n    assert(p.x == 3 && p.y == 4);\n    println("heap sugar ok");\n    return;\n}\n', 'new_ctor_sugar': '\nstruct Point { int64 x; int64 y; static Point create(int64 x, int64 y) { self.x = x; self.y = y; } }\nvoid main() {\n    Point mut& p = new Point(3, 4);\n    assert(p.x == 3 && p.y == 4);\n    Point q = Point::create(5, 6);\n    assert(q.x == 5 && q.y == 6);\n    println("new ctor sugar ok");\n    return;\n}\n', 'heap_ctor': '\nstruct Point { int64 x; int64 y; static Point create(int64 x, int64 y) { self.x = x; self.y = y; } }\nvoid main() {\n    Point mut& p = new Point(3, 4);\n    assert(p.x == 3 && p.y == 4);\n    println("heap sugar ok");\n    return;\n}\n', 'array_sugar': '\nvoid main() {\n    int64 a[] = new [5];\n    a[0] = 7;\n    assert(a[0] == 7);\n    println("new array ok");\n    return;\n}\n'}
ERRORS = {'old_bare_new': ('struct Point { int64 x; int64 y; }\nvoid main() { Point mut& p = new; return; }', '分号'), 'old_bare_new_args': ('struct Point { int64 x; int64 y; }\nvoid main() { Point mut& p = new(7); return; }', '左圆括号'), 'old_factory_new': ('struct Point { int64 x; int64 y; }\nvoid main() { Point q = Point::new(3, 4); return; }', '找不到匹配的重载')}

ERRORS['heap_new'] = ('struct Point { int64 x; }; void main() { Point mut& p = heap new Point(); }', 'new')

ERRORS['removed_heap'] = ('struct Point { int64 x; }; void main() { Point mut& p = heap Point; }', '')
ERRORS['removed_heap_ctor'] = ('struct Point { int64 x; }; void main() { Point mut& p = heap Point(1); }', '')
CASES['heap_identifier'] = 'void main() { int64 heap = 7; assert(heap == 7); }'

def main():
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    with tempfile.TemporaryDirectory(prefix='chali-new-') as directory:
        folder = Path(directory)
        for name, source in CASES.items():
            selected = os.environ.get('CHALI_NEW_CASE')
            if selected and selected != name:
                continue
            path = folder / (name + '.chl')
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(COMPILER), 'run', str(path), 'out', str(folder / (name + '-vm'))], capture_output=True, text=True, encoding='utf-8', env=env, timeout=90)
            assert result.returncode == 0, (name, result.stdout, result.stderr)
            if os.environ.get('CHALI_NEW_VM_ONLY'):
                print(name + ': VM 通过', flush=True)
                continue
            executable = folder / (name + ('.exe' if os.name == 'nt' else ''))
        if not os.environ.get('CHALI_NEW_CASE'):
            for name, (source, fragment) in ERRORS.items():
                path = folder / (name + '.chl')
                path.write_text(source, encoding='utf-8')
                result = subprocess.run([str(COMPILER), 'check', str(path)], capture_output=True, text=True, encoding='utf-8', env=env, timeout=90)
                assert result.returncode != 0 and fragment in result.stdout + result.stderr, (name, result.stdout, result.stderr)
if __name__ == '__main__':
    main()
