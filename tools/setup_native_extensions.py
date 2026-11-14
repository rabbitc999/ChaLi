"""为当前目标构建随 ChaLi 编译器内嵌的 GUI、音频、XML 与密码学静态归档。"""
import argparse
import os
from pathlib import Path
import subprocess
import sys

import project_temp  # 子进程统一继承仓库临时目录。

ROOT = Path(__file__).resolve().parents[1]


def main():
    """用同一 C 工具链构建四个服务，归档放到选定编译目录。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-root', type=Path, default=Path(os.environ.get('CHALI_BUILD_ROOT', 'build')))
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--cache', type=Path)
    parser.add_argument('--expat-archive', type=Path)
    parser.add_argument('--crypto-archive', type=Path)
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--cc', help='用于 CMake 的 C 编译器')
    parser.add_argument('--sysroot', type=Path)
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs 必须为正整数')
    if args.offline and not args.expat_archive:
        parser.error('--offline 需要 --expat-archive 提供锁定的 Expat 源码归档')
    output = args.build_root.resolve() / 'native-extensions'
    common = ['--static', '--out', str(output), '--jobs', str(args.jobs)]
    gui = ['--audio']
    if args.cache:
        gui += ['--cache', str(args.cache.resolve())]
    if args.offline:
        gui += ['--offline']
    xml = ['--archive', str(args.expat_archive.resolve())] if args.expat_archive else []
    crypto = ['--archive', str(args.crypto_archive.resolve())] if args.crypto_archive else []
    if args.offline:
        crypto += ['--offline']
    env = os.environ.copy()
    if args.cc:
        compiler = Path(args.cc)
        env['CC'] = str(compiler.resolve()) if compiler.is_file() else args.cc
        if compiler.is_file():
            env['PATH'] = str(compiler.resolve().parent) + os.pathsep + env.get('PATH', '')
    if args.sysroot:
        env['CFLAGS'] = (env.get('CFLAGS', '') + ' --sysroot="' + args.sysroot.resolve().as_posix() + '"').strip()
    for script, flags in [('setup_desktop_gui.py', gui), ('setup_xml.py', xml), ('setup_crypto.py', crypto)]:
        if script == 'setup_crypto.py' and args.cc:
            flags += ['--cc', env['CC']]
        subprocess.run([sys.executable, str(ROOT / 'tools' / script), *common, *flags], cwd=ROOT, env=env, check=True)


if __name__ == '__main__':
    main()
