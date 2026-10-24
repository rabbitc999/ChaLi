"""微基准与回归共用当前版本编译器的产物命名规则。"""
from pathlib import Path
import runpy

compiler_path = runpy.run_path(str(Path(__file__).resolve().parents[2] / 'tools/compiler_path.py'))['compiler_path']
