"""new 三个语法糖（heap T; / new T(args); / heap new T(args);）的回归。

只做语法层等价降级，不引入新语义；旧写法（new; / new(实参); / T::new(实参);）必须继续
合法、行为不变。VM 与 AOT 双后端结果一致。红线：原本非法的写法（堆分配给值、构造函数/
工厂不匹配）仍须报错。

验收：
  1. 下列样例编译并运行全部通过：
     new_test_old.mgl / new_test_heap.mgl / new_test_ctor.mgl /
     new_test_heap_ctor.mgl / struct_usage.mgl
  2. 下列写法必须报错：
     Point p = heap Point;            （堆分配只能给引用，不能给值）
     Counter c = new Counter(7, 8);   （无匹配的 2 参构造函数）
     Point q = Point::new();          （工厂 0 参重载不存在）
     int64 x = heap int64;            （heap 后必须是类型名，int64 不是标识符）
  3. 新旧写法在同一程序里混用合法且行为一致（见 struct_usage.mgl）。
"""
from pathlib import Path
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMPILER = Path(os.environ.get('MAGEL_TEST_COMPILER', str(ROOT / ('magel.exe' if os.name == 'nt' else 'magel'))))
FIXTURES = ROOT / 'tests' / 'fixtures'

# 5 个验收样例（磁盘上的独立程序）。
SAMPLES = [
    'new_test_old.mgl',
    'new_test_heap.mgl',
    'new_test_ctor.mgl',
    'new_test_heap_ctor.mgl',
    'struct_usage.mgl',
]

# 4 个必须报错的写法（内联源 + 期望出现在诊断里的片段）。
ERRORS = {
    'heap_into_value': (
        'struct Point { int64 x; int64 y; Point(int64 x, int64 y) { self.x = x; self.y = y; } '
        'static Point new(int64 x, int64 y) { self.x = x; self.y = y; } };\n'
        'int64 main() { Point p = heap Point; return 0; }',
        '只能赋给引用',
    ),
    'ctor_mismatch': (
        'struct Counter { int64 a; Counter(int64 a) { self.a = a; } };\n'
        'int64 main() { Counter c = new Counter(7, 8); return 0; }',
        '没有匹配的构造函数',
    ),
    'factory_mismatch': (
        'struct Point { int64 x; int64 y; Point(int64 x, int64 y) { self.x = x; self.y = y; } '
        'static Point new(int64 x, int64 y) { self.x = x; self.y = y; } };\n'
        'int64 main() { Point q = Point::new(); return 0; }',
        '找不到匹配的重载',
    ),
    'heap_builtin': (
        'int64 main() { int64 x = heap int64; return 0; }',
        '语法错误',
    ),
}


def run_sample(path, folder, vm_only):
    env = dict(os.environ, MAGEL_LANG='zh', MAGEL_NO_STDIN_INPUT='1')
    vm = subprocess.run([str(COMPILER), 'run', str(path), 'out', str(folder / (path.stem + '-vm'))],
                        capture_output=True, text=True, encoding='utf-8', env=env, timeout=90)
    # 真实编译/运行错误会落在 stdout（见 `magel：` 诊断）并以非零退出码返回；stderr 只承载
    # 堆泄漏之类的**提示性警告**，不代表失败，这里不纳入判定。
    assert vm.returncode == 0 and '错误' not in vm.stdout, (path.name, vm.stdout, vm.stderr)
    if vm_only:
        print(path.name + ': VM 通过', flush=True)
        return
    executable = folder / (path.stem + ('.exe' if os.name == 'nt' else ''))
    aot = subprocess.run([str(COMPILER), 'aot', str(path), 'out', str(executable), 'opt', '0'],
                         capture_output=True, text=True, encoding='utf-8', env=env, timeout=180)
    assert aot.returncode == 0 and '错误' not in aot.stdout, (path.name, aot.stdout, aot.stderr)
    native = subprocess.run([str(executable)], capture_output=True, text=True, encoding='utf-8', timeout=30)
    assert native.returncode == 0 and native.stdout == vm.stdout, \
        (path.name, native.stdout, native.stderr, vm.stdout)
    print(path.name + ': VM / AOT 通过', flush=True)


def main():
    assert COMPILER.exists(), COMPILER
    vm_only = bool(os.environ.get('MAGEL_NEW_VM_ONLY'))
    with tempfile.TemporaryDirectory(prefix='magel-new-') as directory:
        folder = Path(directory)
        for name in SAMPLES:
            selected = os.environ.get('MAGEL_NEW_CASE')
            if selected and selected != name:
                continue
            run_sample(FIXTURES / name, folder, vm_only)
        if not os.environ.get('MAGEL_NEW_CASE'):
            env = dict(os.environ, MAGEL_LANG='zh', MAGEL_NO_STDIN_INPUT='1')
            for label, (source, fragment) in ERRORS.items():
                path = folder / (label + '.mgl')
                path.write_text(source, encoding='utf-8')
                result = subprocess.run([str(COMPILER), 'check', str(path)],
                                        capture_output=True, text=True, encoding='utf-8', env=env, timeout=90)
                assert result.returncode != 0 and fragment in result.stdout + result.stderr, \
                    (label, result.stdout, result.stderr)
            print('new 语法糖：5 个样例 VM/AOT 通过，4 个错误用例诊断到位。')


if __name__ == '__main__':
    main()
