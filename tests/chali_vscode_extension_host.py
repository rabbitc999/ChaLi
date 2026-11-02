"""使用已安装的 VSCode 运行真实扩展宿主调试及进程清理验收。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path


def main():
    """传入本阶段编译器；缺少 VSCode 或图形宿主时明确失败，不静默跳过。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    return subprocess.run(['node', str(root / 'vscode/tools/test-extension-host.js')], cwd=root,
                          env=dict(os.environ, CHALI_BIN=str(args.chali.resolve())), timeout=260).returncode


if __name__ == '__main__':
    raise SystemExit(main())
