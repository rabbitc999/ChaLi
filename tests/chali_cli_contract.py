"""跨平台、无需发行编译器的命令与文档漂移门禁。"""
from pathlib import Path
import re
import ast
import xml.etree.ElementTree as ET
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
TEXT = {'.cpp', '.hpp', '.h', '.c', '.chl', '.gram', '.tsv', '.md', '.toml', '.js', '.json', '.py', '.sh', '.ps1', '.txt'}
GENERATED = {'source/chali_stdlib_embedded.cpp', 'tools/chali_manual.json'}


def main():
    """检查当前代码与指南；历史记录和负面测试不冒充当前语法。"""
    failures = []
    names = subprocess.check_output(['git', '-c', 'core.quotepath=false', 'ls-files'], cwd=ROOT, text=True, encoding='utf-8').splitlines()
    for name in names:
        path = ROOT / name
        if path.suffix not in TEXT or name in GENERATED or not path.is_file():
            continue
        if path.suffix == '.txt' and not name.startswith(('example/expected/', 'example/expected-error/')):
            continue
        code = (name.startswith('source/') and path.suffix != '.md') or name in {
            'tools/library_reference_data.py', 'tools/library_reference_native.py',
        }
        guide = name.startswith(('doc/', 'vscode/skills/')) or name in {'README.md', 'README.cn.md', 'README.en.md'}
        configuration = path.suffix == '.toml' and not name.startswith(('benchmarks/results/', 'tools/history/'))
        text = path.read_text(encoding='utf-8-sig')
        for number, line in enumerate(text.splitlines(), 1):
            reason = ''
            if name.startswith(('example/expected/', 'example/expected-error/')) and 'chali aot:' in line:
                reason = '当前样例仍期待旧运行时错误前缀'
            elif code and re.search(r'\bVM\b|虚拟机|\.svmb', line, re.I):
                reason = '旧解释执行表述或字节码提示'
            elif code and re.search(r'(?:使用|use)\s+aot\b', line):
                reason = '诊断仍推荐已删除命令'
            elif guide and re.search(r'\b(?:chali|ChaLi)(?:\.exe)? aot\b|原生\s*AOT\s*调试|Native AOT debugging|00-vision\.md|99-chali-in-blocks\.md', line, re.I):
                reason = '已删除命令或旧调试标题'
            elif configuration and re.match(r'\s*\[aot\]', line):
                reason = '旧构建配置表'
            if reason:
                failures.append(f'{name}:{number}: {reason}')
        if guide:
            for block in re.findall(r'```(?:toml|ini)\s*\n(.*?)```', text, re.S):
                if re.search(r'^\s*\[aot\]', block, re.M):
                    failures.append(f'{name}: TOML 示例仍使用旧构建配置表')
    # 只检测直接调用旧命令的正例；拒绝旧命令的反例继续保留。
    for path in sorted((ROOT / 'tests').glob('*.py')):
        tree = ast.parse(path.read_text(encoding='utf-8-sig'), filename=str(path))
        for node in ast.walk(tree):
            if isinstance(node, ast.Call) and node.args:
                method = node.func.id if isinstance(node.func, ast.Name) else getattr(node.func, 'attr', '')
                old_command = isinstance(node.args[0], ast.Constant) and node.args[0].value == 'aot'
                rejected = any(isinstance(k.value, ast.Constant) and (
                    (k.arg in ('ok', 'success') and k.value.value is False) or
                    (k.arg in ('expected', 'code') and isinstance(k.value.value, int) and k.value.value != 0)
                ) for k in node.keywords)
                if method in ('run', 'run_cli', 'invoke', 'chali') and old_command and not rejected:
                    failures.append(f'{path.relative_to(ROOT).as_posix()}:{node.lineno}: 正例仍调用已删除命令')
            elif isinstance(node, ast.For) and isinstance(node.iter, (ast.List, ast.Tuple)):
                values = [n.value for n in node.iter.elts if isinstance(n, ast.Constant)]
                if 'run' in values and 'aot' in values:
                    failures.append(f'{path.relative_to(ROOT).as_posix()}:{node.lineno}: 回归循环仍使用已删除命令')
    for path in sorted((ROOT / 'doc/images').glob('*.svg')):
        root = ET.parse(path).getroot()
        if re.search(r'\bAOT\b', ' '.join(root.itertext())):
            failures.append(f'{path.relative_to(ROOT).as_posix()}: 图中文字仍使用旧名称')
        for node in root.iter('{http://www.w3.org/2000/svg}text'):
            if node.get('font-weight') != '700':
                continue
            text = re.sub(r'^\d+[a-z]?\s+', '', ''.join(node.itertext()))
            if any(not word[0].isupper() for word in re.findall(r'[A-Za-z]+', text)):
                failures.append(f'{path.relative_to(ROOT).as_posix()}: 英文标题各单词首字母须大写：{text}')
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
    print('CLI contract passed: build, [build], current command tests, API text, fixture prefixes and diagram headings')
    return 0


if __name__ == '__main__':
    sys.exit(main())
