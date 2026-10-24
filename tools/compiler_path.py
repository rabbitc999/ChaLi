"""定位当前版本的编译器，供构建与回归统一选择实际产物。"""
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def default_compiler():
    """返回当前平台和 LLVM 配置对应的正式构建文件名。"""
    if os.name != 'nt':
        return ROOT / 'chali'
    version = (ROOT / 'VERSION').read_text(encoding='utf-8').strip()
    variant = 'static' if os.environ.get('CHALI_STATIC_LLVM') == '1' else 'external'
    return ROOT / f'chali-{version}-llvm-{variant}-windows-x64.exe'


def compiler_path():
    """优先使用显式指定的编译器，默认定位正式命名的当前版本产物。"""
    selected = os.environ.get('CHALI_COMPILER') or os.environ.get('CHALI_BIN')
    return Path(selected).resolve() if selected else default_compiler()


if __name__ == '__main__':
    print(compiler_path().as_posix())
