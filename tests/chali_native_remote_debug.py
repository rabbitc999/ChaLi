"""在真实 Linux 宿主、GDB 与 TCP gdbserver 上验证远程调试。"""
import argparse
import os
from pathlib import Path
import subprocess
import sys
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """远程专项需要真实 Linux 环境，缺少环境明确失败而非跳过。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    if not sys.platform.startswith('linux'):
        print('真实远程专项要求 Linux 宿主和 Linux gdbserver', file=sys.stderr)
        return 1
    root = Path(__file__).resolve().parents[1]
    env = dict(os.environ, CHALI_BIN=str(args.chali.resolve()))
    return subprocess.run(['node', str(root / 'vscode/test/debugRemoteNative.js')],
                          cwd=root, env=env, timeout=600).returncode


if __name__ == '__main__':
    raise SystemExit(main())
