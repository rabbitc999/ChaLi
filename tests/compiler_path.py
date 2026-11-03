"""直接运行测试时复用工具目录的正式编译器定位规则。"""
from pathlib import Path
import runpy
import os

# 旧业务回归比较程序输出；完成提示由独立专项显式撤销静默设置后验证。
os.environ.setdefault('CHALI_QUIET', '1')

compiler_path = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'tools/compiler_path.py'))['compiler_path']
