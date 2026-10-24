"""直接运行测试时复用工具目录的正式编译器定位规则。"""
from pathlib import Path
import runpy

compiler_path = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'tools/compiler_path.py'))['compiler_path']
