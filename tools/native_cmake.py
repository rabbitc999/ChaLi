"""为原生扩展选择当前工具链可用的 CMake 构建程序。"""
import os
from pathlib import Path
import shutil


def generator_args():
    """优先使用 Ninja，Windows 缺少 Ninja 时使用 GCC 配套的 MinGW Make。"""
    ninja = shutil.which('ninja')
    if ninja:
        return ['-G', 'Ninja', '-DCMAKE_MAKE_PROGRAM=' + Path(ninja).as_posix()]
    if os.name == 'nt':
        compiler = shutil.which(os.environ.get('CC', 'gcc'))
        make = Path(compiler).parent / 'mingw32-make.exe' if compiler else None
        if make is None or not make.is_file():
            found = shutil.which('mingw32-make')
            make = Path(found) if found else None
        if make is not None:
            return ['-G', 'MinGW Makefiles', '-DCMAKE_MAKE_PROGRAM=' + make.as_posix()]
    raise RuntimeError('原生扩展构建需要 Ninja；Windows GCC 工具链也可提供 mingw32-make.exe')
