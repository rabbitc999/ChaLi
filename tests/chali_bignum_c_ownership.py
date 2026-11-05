"""以 C11 编译数值服务，检查分配失败、跨所有者释放及结果回滚。"""
import argparse
import os
from pathlib import Path
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """逐项执行完整 C 分配失败回归，可选地址及未定义行为检查器。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--kind', choices=['integer', 'float', 'all'], default='all')
    parser.add_argument('--portable', action='store_true')
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    directory = ROOT / 'tmp' / ('numeric-c-ownership-portable' if args.portable else 'numeric-c-ownership')
    directory.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, TMP=str(ROOT / 'tmp'), TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'))
    compiler = env.get('CC')
    flags = shlex.split(env.get('CFLAGS', ''))
    if not compiler and os.name == 'nt':
        compiler = str(sdk / 'bin/gcc.exe')
        flags.append('--sysroot=' + sdk.as_posix())
        env['PATH'] = str(sdk / 'bin') + os.pathsep + env['PATH']
    compiler = compiler or 'gcc'
    if args.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-g']
        env['ASAN_OPTIONS'] = 'detect_leaks=1:halt_on_error=1'
        env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    if args.portable:
        flags.append('-DBIGINT_FORCE_PORTABLE_MULTIPLY')
    for kind in (['integer', 'float'] if args.kind == 'all' else [args.kind]):
        name = 'chali_c_bigint_allocator' if kind == 'integer' else 'chali_c_bigfloat_allocator'
        sources = sorted((ROOT / 'source/bignum').glob('chali_bigint*.c'))
        if kind == 'float':
            sources += sorted((ROOT / 'source/bignum').glob('chali_bigfloat*.c'))
        sources += [ROOT / 'source/runtime_c/chali_memory.c', ROOT / 'source/runtime_c/chali_buffer.c',
                    ROOT / 'tests' / (name + '.c')]
        output = directory / (name + ('.exe' if os.name == 'nt' else ''))
        subprocess.run([compiler, '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-DCHALI_BIGINT_TESTING',
                        '-I' + str(ROOT / 'source/bignum'), *flags, *map(str, sources), '-o', str(output), '-lm'],
                       cwd=ROOT, env=env, check=True)
        subprocess.run([str(output)], cwd=ROOT, env=env, check=True)
        print(f'{kind}: ownership and allocation failures passed; portable={args.portable}, sanitize={args.sanitize}', flush=True)


if __name__ == '__main__':
    main()
