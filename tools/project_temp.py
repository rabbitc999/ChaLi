"""项目临时目录策略：所有隔离临时文件夹只创建在仓库 tmp 内。"""
import os
from pathlib import Path
import tempfile as _tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
workspace = ROOT
# 交叉构建的源码副本位于仓库 build/ 下，共用原始仓库的临时目录。
if ROOT.parent.name == 'build' and (ROOT.parent.parent / '.git').exists():
    workspace = ROOT.parent.parent
TEMP_ROOT = (workspace / 'tmp').resolve()
if TEMP_ROOT.parent != workspace.resolve():
    raise RuntimeError('项目 tmp 不能重定向到仓库外部')
TEMP_ROOT.mkdir(parents=True, exist_ok=True)
_tempfile.tempdir = str(TEMP_ROOT)
for _name in ('TMPDIR', 'TMP', 'TEMP'):
    os.environ[_name] = str(TEMP_ROOT)


def _directory(value):
    """只保留位于项目 tmp 内的指定子目录，其余临时目录归入统一根。"""
    if value is not None:
        path = Path(value).resolve()
        if path == TEMP_ROOT or TEMP_ROOT in path.parents:
            path.mkdir(parents=True, exist_ok=True)
            return str(path)
    return str(TEMP_ROOT)


class ProjectTemporaryDirectory(_tempfile.TemporaryDirectory):
    """清理项目子目录时，有界等待 Windows 释放刚结束的可执行映像。"""

    def cleanup(self):
        """仅重试短暂权限错误；越界路径和持续失败均向调用方报告。"""
        path = Path(self.name).resolve()
        if TEMP_ROOT not in path.parents:
            raise RuntimeError('拒绝清理项目 tmp 子目录以外的路径')
        deadline = time.monotonic() + 5
        while True:
            try:
                return super().cleanup()
            except PermissionError:
                if os.name != 'nt' or time.monotonic() >= deadline:
                    raise
                time.sleep(0.1)


class ProjectTempfile:
    """沿用标准 tempfile 文件操作，仅约束创建临时文件夹的位置。"""

    def __getattr__(self, name):
        """透传标准库文件接口，保留原子写入指定输出目录的能力。"""
        return getattr(_tempfile, name)

    def TemporaryDirectory(self, suffix=None, prefix=None, dir=None, **kwargs):
        """在项目 tmp 中创建可自动清理的独立子目录。"""
        return ProjectTemporaryDirectory(suffix=suffix, prefix=prefix, dir=_directory(dir), **kwargs)

    def mkdtemp(self, suffix=None, prefix=None, dir=None):
        """在项目 tmp 中创建由调用方管理生命周期的独立子目录。"""
        return _tempfile.mkdtemp(suffix=suffix, prefix=prefix, dir=_directory(dir))


tempfile = ProjectTempfile()
