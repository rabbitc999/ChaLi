"""性能脚本共用项目临时目录策略，不使用系统或上级目录。"""
from pathlib import Path
import runpy

tempfile = runpy.run_path(str(Path(__file__).resolve().parents[2] / 'tools/project_temp.py'))['tempfile']
