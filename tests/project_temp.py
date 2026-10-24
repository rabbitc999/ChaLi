"""让直接执行的测试共用 tools/project_temp.py 的仓库临时目录策略。"""
from pathlib import Path
import runpy

tempfile = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'tools/project_temp.py'))['tempfile']
