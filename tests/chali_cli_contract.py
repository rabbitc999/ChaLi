"""跨平台、无需发行编译器的命令与文档漂移门禁。"""
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
TEXT = {'.cpp', '.hpp', '.h', '.c', '.chl', '.gram', '.tsv', '.md', '.toml', '.js', '.json', '.py', '.sh', '.ps1'}
GENERATED = {'source/chali_stdlib_embedded.cpp', 'tools/chali_manual.json'}


def main():
    """检查当前代码与指南；历史记录和负面测试不冒充当前语法。"""
    failures = []
    names = subprocess.check_output(['git', '-c', 'core.quotepath=false', 'ls-files'], cwd=ROOT, text=True, encoding='utf-8').splitlines()
    for name in names:
        path = ROOT / name
        if path.suffix not in TEXT or name in GENERATED or not path.is_file():
            continue
        code = name.startswith('source/') and path.suffix != '.md'
        guide = name.startswith(('doc/', 'vscode/skills/')) or name in {'README.md', 'README.cn.md', 'README.en.md'}
        configuration = path.suffix == '.toml' and not name.startswith(('benchmarks/results/', 'tools/history/'))
        text = path.read_text(encoding='utf-8-sig')
        for number, line in enumerate(text.splitlines(), 1):
            reason = ''
            if code and re.search(r'\bVM\b|虚拟机|\.svmb', line, re.I):
                reason = '旧解释执行表述或字节码提示'
            elif guide and re.search(r'\b(?:chali|ChaLi)(?:\.exe)? aot\b|原生\s*AOT\s*调试|Native AOT debugging|00-vision\.md', line, re.I):
                reason = '已删除命令或旧调试标题'
            elif configuration and re.match(r'\s*\[aot\]', line):
                reason = '旧构建配置表'
            if reason:
                failures.append(f'{name}:{number}: {reason}')
        if guide:
            for block in re.findall(r'```(?:toml|ini)\s*\n(.*?)```', text, re.S):
                if re.search(r'^\s*\[aot\]', block, re.M):
                    failures.append(f'{name}: TOML 示例仍使用旧构建配置表')
    driver = (ROOT / 'source/chali_driver.cpp').read_text(encoding='utf-8')
    if 'first == "build"' not in driver or 'mode == "build"' not in driver:
        failures.append('缺少 build 子命令或 --mode build 入口')
    if 'first == "aot"' in driver or 'mode == "aot"' in driver:
        failures.append('已删除的命令入口回流')
    package = (ROOT / 'source/chali_package.cpp').read_text(encoding='utf-8')
    if 'name == "build"' not in package or 'name == "aot"' in package:
        failures.append('项目配置表未统一')
    adapter = (ROOT / 'vscode/src/nativeDebugAdapter.js').read_text(encoding='utf-8')
    if "['build', config.program" not in adapter:
        failures.append('调试适配器未使用构建命令')
    if failures:
        print('\n'.join(failures), file=sys.stderr)
        return 1
    print('CLI contract passed: build, [build], build debugging, and legacy runtime text')
    return 0


if __name__ == '__main__':
    sys.exit(main())
