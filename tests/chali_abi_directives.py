"""验证逐函数 ABI 指令、导入函数值、原生导出和纯值聚合往返。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import ctypes
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """构建真实 C 库，在 VM/AOT 中调用，并由宿主按 C ABI 调回导出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='abi-directives-'))
    env = dict(os.environ, CHALI_LLVM_CLANG=str(ROOT / 'library/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang'))

    def invoke(command, name):
        """保存每个实际调用的输出。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=600)
        (work / (name + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
        assert result.returncode == 0 and not result.stderr, (command, result)
        return result.stdout

    native = work / 'native.c'
    native.write_text('''#include <stdint.h>
#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API __attribute__((visibility("default")))
#endif
typedef struct { int32_t f0; double f1; int16_t f2; } Packet;
/** @brief 增加一个标量。 @param x 输入。 @return 输入加一。 */
API int32_t calc(int32_t x) { return x + 1; }
/** @brief 缩放单精度数。 @param x 输入。 @return 两倍输入。 */
API float scale(float x) { return x * 2; }
/** @brief 调整聚合字段。 @param p 输入。 @return 调整后的独立值。 */
API Packet packet(Packet p) { p.f0 += 1; p.f1 *= 2; p.f2 += 3; return p; }
/** @brief 同步调用回调。 @param f 回调。 @param x 输入。 @return 回调结果。 */
API int32_t callback(int32_t (*f)(int32_t), int32_t x) { return f(x); }
''', encoding='utf-8')
    library = work / ('native.dll' if os.name == 'nt' else 'native.so')
    invoke(['gcc', '-shared', *([] if os.name == 'nt' else ['-fPIC']), native, '-o', library], 'native-build')
    source = work / 'import.chl'
    declarations = f'''struct Packet {{ int32 a; float64 b; int16 c; }};
#abi("C", "{library.as_posix()}", "calc") int32 calc(int32 x);
#abi("C", "{library.as_posix()}", "scale") float32 scale(float32 x);
#abi("C", "{library.as_posix()}", "packet") Packet packet(Packet p);
#abi("C", "{library.as_posix()}", "callback") int32 callback(int32 fn(int32) f, int32 x);
'''
    source.write_text(declarations + '''void main() {
    assert(calc((int32)41) == (int32)42);
    int32 fn(int32) f = calc;
    assert(f((int32)41) == (int32)42);
    float32 fn(float32) g = scale;
    assert(g((float32)1.25) == (float32)2.5);
    Packet p; p.a = (int32)4; p.b = 1.25; p.c = (int16)7;
    Packet fn(Packet) h = packet;
    Packet q = h(p);
    assert(q.a == (int32)5 && q.b == 2.5 && q.c == (int16)10);
    assert(p.a == (int32)4 && p.b == 1.25 && p.c == (int16)7);
    assert(callback(f, (int32)41) == (int32)42);
    println("ABI directives OK");
}
''', encoding='utf-8')
    for backend in ['run', 'aot']:
        target = work / ('import-' + backend)
        output = invoke([compiler, backend, source, '--out', target], 'import-' + backend + '-build')
        if backend == 'aot':
            output = invoke([str(target) + ('.exe' if os.name == 'nt' else '')], 'import-aot-run')
        assert output == 'ABI directives OK\n', output
    exported = work / 'export.chl'
    exported.write_text('''struct Packet { int32 a; float64 b; int16 c; };
#export("calc") int32 calc(int32 x) { return (int32)(x + (int32)1); }
#export("scale") float32 scale(float32 x) { return (float32)(x * (float32)2); }
#export("packet") Packet packet(Packet p) { p.a = (int32)(p.a + (int32)1); p.b = p.b * 2.0; p.c = (int16)(p.c + (int16)3); return p; }
''', encoding='utf-8')
    prefix = work / 'export'
    target = Path(str(prefix) + ('.dll' if os.name == 'nt' else '.so'))
    invoke([compiler, 'aot', exported, '--shared', '--out', prefix], 'export-build')

    class Packet(ctypes.Structure):
        """平台 C ABI 中的聚合布局。"""
        _fields_ = [('a', ctypes.c_int32), ('b', ctypes.c_double), ('c', ctypes.c_int16)]

    module = ctypes.CDLL(str(target))
    module.chali_init.argtypes = []
    module.chali_finish.argtypes = []
    module.calc.argtypes = [ctypes.c_int32]
    module.calc.restype = ctypes.c_int32
    module.scale.argtypes = [ctypes.c_float]
    module.scale.restype = ctypes.c_float
    module.packet.argtypes = [Packet]
    module.packet.restype = Packet
    module.chali_init()
    try:
        assert module.calc(41) == 42
        assert module.scale(1.25) == 2.5
        p = Packet(4, 1.25, 7)
        q = module.packet(p)
        assert (q.a, q.b, q.c) == (5, 2.5, 10)
        assert (p.a, p.b, p.c) == (4, 1.25, 7)
    finally:
        module.chali_finish()
    header = target.with_suffix('.h')
    assert header.exists(), list(work.glob('*'))
    header_check = work / 'header_check.c'
    header_check.write_text('#include "' + header.name + '"\n', encoding='utf-8')
    invoke(['gcc', '-fsyntax-only', header_check], 'header-check')
    print('ABI directives VM/AOT and native export passed; logs:', work)


if __name__ == '__main__':
    main()
