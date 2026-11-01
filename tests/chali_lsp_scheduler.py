"""独立验证语言服务语义预算、优先级、排队快照上限与关闭。"""
from pathlib import Path
import argparse
import subprocess
import os
from project_temp import tempfile


def main():
    """只链接标准线程库，验证真实调度器头文件的并发契约。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', help='模块门禁兼容参数')
    parser.add_argument('--cxx', default='g++')
    options = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    with tempfile.TemporaryDirectory(prefix='lsp-scheduler-') as directory:
        binary = Path(directory) / ('scheduler.exe' if os.name == 'nt' else 'scheduler')
        subprocess.run([options.cxx, '-std=c++17', '-O2', '-pthread', '-Itools',
                        'tests/chali_lsp_scheduler.cpp', '-o', str(binary)], cwd=root, check=True, timeout=90)
        subprocess.run([str(binary)], cwd=root, check=True, timeout=15)
    print('LSP scheduler: bounded processes, interactive priority, cancellation, queue bytes and shutdown passed')


if __name__ == '__main__':
    main()
