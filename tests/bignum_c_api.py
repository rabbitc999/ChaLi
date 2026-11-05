"""独立数值测试共用的 C11 构建和原生布局，不依赖 C++ 适配层。"""
import ctypes as C
import os
from pathlib import Path
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[1]


class Allocator(C.Structure):
    """与 ChaliCAllocator 对齐的三指针布局。"""
    _fields_ = [('context', C.c_void_p), ('allocate', C.c_void_p), ('release', C.c_void_p)]


class Buffer(C.Structure):
    """拥有元素大小和分配所有者的连续缓冲。"""
    _fields_ = [('data', C.c_void_p), ('length', C.c_size_t), ('capacity', C.c_size_t),
                ('element_size', C.c_size_t), ('allocator', Allocator)]


class BigInt(C.Structure):
    """符号和小端三十二位分块。"""
    _fields_ = [('sign', C.c_int), ('limbs', Buffer)]


class BigFloat(C.Structure):
    """C 大整数尾数与十进制指数。"""
    _fields_ = [('mantissa', BigInt), ('exponent10', C.c_int)]


class BigString(C.Structure):
    """C 数值服务返回的独占文本。"""
    _fields_ = [('data', C.c_void_p), ('length', C.c_size_t), ('capacity', C.c_size_t), ('allocator', Allocator)]


class MathError(C.Structure):
    """调用拥有的错误类别及静态消息指针。"""
    _fields_ = [('kind', C.c_int), ('message', C.c_char_p)]


def build_shared(output, *, floating=False, portable=False, testing=False):
    """以真实 C11 编译内核，Windows 默认使用仓库的静态 SDK。"""
    output = Path(output).resolve()
    assert output.is_relative_to((ROOT / 'tmp').resolve())
    output.parent.mkdir(parents=True, exist_ok=True)
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    flags = shlex.split(os.environ.get('CFLAGS', ''))
    env = dict(os.environ, TMP=str(ROOT / 'tmp'), TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'))
    compiler = os.environ.get('CC')
    if not compiler and os.name == 'nt' and (sdk / 'bin/gcc.exe').is_file():
        compiler = str(sdk / 'bin/gcc.exe')
        flags.append('--sysroot=' + sdk.as_posix())
        env['PATH'] = str(sdk / 'bin') + os.pathsep + env['PATH']
    compiler = compiler or 'gcc'
    sources = sorted((ROOT / 'source/bignum').glob('chali_bigint*.c'))
    if floating:
        sources += sorted((ROOT / 'source/bignum').glob('chali_bigfloat*.c'))
    sources += [ROOT / 'source/runtime_c/chali_memory.c', ROOT / 'source/runtime_c/chali_buffer.c']
    subprocess.run([compiler, '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-shared',
                    *([] if os.name == 'nt' else ['-fPIC']), *flags,
                    *(['-DBIGINT_FORCE_PORTABLE_MULTIPLY'] if portable else []),
                    *(['-DCHALI_BIGINT_TESTING'] if testing else []), *map(str, sources),
                    '-o', str(output), *([] if os.name == 'nt' else ['-lm'])],
                   cwd=ROOT, env=env, check=True)
    return C.CDLL(str(output))


def bind(library, name, arguments, result=C.c_int):
    """固定 ctypes 原型，避免平台默认整数宽度掩盖 ABI 错误。"""
    function = getattr(library, name)
    function.argtypes = arguments
    function.restype = result
    return function


def integer_value(value):
    """直接读出有效小端分块，由 Python 任意精度整数独立重建。"""
    if not value.sign:
        return 0
    return value.sign * int.from_bytes(C.string_at(value.limbs.data, value.limbs.length * 4), 'little')
