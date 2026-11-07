"""直接运行测试时复用工具目录的正式编译器定位规则。"""
from pathlib import Path
import runpy
import os
import json
import subprocess
from functools import lru_cache

# 旧业务回归比较程序输出；完成提示由独立专项显式撤销静默设置后验证。
os.environ.setdefault('CHALI_QUIET', '1')

compiler_path = runpy.run_path(str(Path(__file__).resolve().parents[1] / 'tools/compiler_path.py'))['compiler_path']


@lru_cache(maxsize=8)
def compiler_build_info(compiler):
    """读取实际编译器变体，避免静态回归误传仅外置版支持的工具路径。"""
    result=subprocess.run([str(compiler),'--build-info'],capture_output=True,text=True,encoding='utf-8',timeout=30)
    assert result.returncode==0,result
    return json.loads(result.stdout)
