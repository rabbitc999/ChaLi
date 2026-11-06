"""为独立数值回归分别编译 C 内核与 C++ 适配层。"""
from pathlib import Path
import os
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def compile_c_kernel(directory, portable=False, extra_flags=()):
    """按 C11 编译内核，返回供 C++ 链接的对象路径。"""
    directory = Path(directory)
    directory.mkdir(parents=True, exist_ok=True)
    flags = [*shlex.split(os.environ.get('CFLAGS', '')), *extra_flags]
    sources = sorted((ROOT / 'source/bignum').glob('chali_big*.c'))
    sources += [ROOT / 'source/runtime_c/chali_memory.c', ROOT / 'source/runtime_c/chali_buffer.c']
    objects = []
    for source in sources:
        target = directory / (source.stem + '.o')
        subprocess.run([os.environ.get('CC', 'gcc'), '-std=c11', '-O2', '-Wall', '-Wextra',
                        *flags, *(['-DBIGINT_FORCE_PORTABLE_MULTIPLY'] if portable else []),
                        '-c', str(source), '-o', str(target)], cwd=ROOT, check=True)
        objects.append(str(target))
    return objects
