"""运行 DAP 协议、真实 GDB 和构建取消的完整调试专项。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """用指定编译器验证调试器全部现行协议与原生执行用例。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    env = dict(os.environ, CHALI_BIN=str(args.chali.resolve()))
    for script in ['debugConfiguration.js', 'debugIntegration.js', 'debugProtocol.js', 'debugMetadata.js', 'debugMemory.js', 'debugTypes.js', 'debugSources.js', 'debugMemoryNative.js', 'debugValue.js', 'debugValueNative.js', 'debugBreakpoints.js', 'debugLaunch.js', 'debugLifecycle.js', 'nativeDebug.js', 'debugFailure.js', 'debugFrames.js', 'debugDisassemble.js', 'debugObjectMemory.js', 'debugContainers.js', 'debugContainerMetadata.js', 'debugContainersNative.js', 'debugCallbackContext.js', 'debugDynamicValues.js', 'debugInterfacesNative.js', 'debugClosureValues.js', 'debugClosuresNative.js', 'debugExpression.js', 'debugExpressionNative.js', 'debugAssignment.js', 'debugAssignmentNative.js', 'debugThreads.js', 'debugThreadsNative.js']:
        result = subprocess.run(['node', str(root / 'vscode/test' / script)], cwd=root,
                                env=env, text=True, encoding='utf-8', errors='replace',
                                capture_output=True, timeout=300)
        print(result.stdout, end='')
        if result.returncode:
            print(result.stderr, end='')
            return result.returncode
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
