"""测试共用仓库的 LSP 计时预算读取器。"""
from pathlib import Path
import runpy

timing_budget = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'tools/cpu_budget.py'))['timing_budget']
