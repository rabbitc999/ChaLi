"""独立验证语言服务语义预算、优先级、排队快照上限与关闭。"""
from pathlib import Path
import argparse
import subprocess
import shutil
import os
from project_temp import tempfile


def main():
    """只链接标准线程库，验证真实调度器头文件的并发契约。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', help='模块门禁兼容参数')
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--sysroot', type=Path, help='与 C++ 工具链匹配的系统头文件及库根目录')
    options = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    compiler = Path(shutil.which(options.cxx) or options.cxx).resolve()
    sdk = root / 'build/dependencies/llvm/static-sdk'
    if options.sysroot is None and compiler.parent == (sdk / 'bin').resolve():
        options.sysroot = sdk
    environment = dict(os.environ, PATH=str(compiler.parent) + os.pathsep + os.environ.get('PATH', ''))
    with tempfile.TemporaryDirectory(prefix='lsp-scheduler-') as directory:
        binary = Path(directory) / ('scheduler.exe' if os.name == 'nt' else 'scheduler')
        toolchain = ['--sysroot=' + str(options.sysroot.resolve())] if options.sysroot else []
        subprocess.run([str(compiler), *toolchain, '-std=c++17', '-O2', '-pthread', '-Itools',
                        'tests/chali_lsp_scheduler.cpp', '-o', str(binary)], cwd=root, env=environment, check=True, timeout=90)
        subprocess.run([str(binary)], cwd=root, env=environment, check=True, timeout=15)
    print('LSP scheduler: bounded processes, interactive priority, cancellation, queue bytes and shutdown passed')


if __name__ == '__main__':
    main()
