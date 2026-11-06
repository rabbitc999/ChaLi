"""检测更改记录以外的 Markdown 中的历史表述与虚拟机时代残留。"""
from pathlib import Path
import re

"""1.0.8 起 run 使用 checked AOT、aot 默认 unchecked 导出，编译器与内嵌资产
不含 VM 解释执行核；本书与文档不得再把运行期描述成「解释执行」或「组装 VM
指令」。历史版本的真实措辞只保留在 CHANGELOG（更改记录）、benchmarks（历史
测量）与 tools/history（历史清理映射）里，扫描时整体跳过这三处。"""

ROOT = Path(__file__).resolve().parents[1]

EXCLUDED_PARTS = (
    '.git', 'build', 'tmp', 'dist', 'benchmarks', 'tools/history',
    'node_modules', 'doc/book/latex',
)

# （正则，说明）。命中即失败；例外行必须整行包含对应豁免子串。
RULES = (
    (r'\bv?[0-9]+\[[0-9]+\]\.[0-9]+\b', '版本号保留点号分隔，不得被元组下标迁移改成中括号', ()),
    (r'\b(?:pair|pairs|tuple|info|value|hint)\.[0-9]+\b', '元组按位置访问使用编译期常量下标 [0]、[1]，不使用点号序号', ()),
    (r'解释执行', '运行已改为 checked AOT 编译执行，不得再写「解释执行」',
     ('解释执行器', '不再提供解释执行', '不依赖 VM 断点或解释执行', '不含 VM 解释执行核')),
    (r'虚拟机', '统一称四元式 IR 与 LLVM AOT，不得再写「虚拟机」', ()),
    (r'组装 VM', '执行路径不再组装 VM 指令', ()),
    (r'惰性求值', '惰性只用于描述迭代器；闭包是调用时才执行的延迟执行', ()),
    (r'不是\s*BUG', '特性按语言规则陈述，不得写「不是 BUG」', ()),
    (r'不是 a bug|not a bug', '特性按语言规则陈述，不得写「not a bug」', ()),
    (r'运行期错误：', '运行期错误首行是 `chali aot: `，旧前缀已废弃', ()),
    (r'调用栈（外 → 内）', '调用栈方向为（内 → 外）', ()),
    (r'chali-execution\.png', '虚拟机流程图已删除，不得再引用', ()),
    (r'0\.21\.1|chali-1\.0\.2-llvm', '版本以 VERSION 为准，不得引用过期版本号', ()),
    (r'break\s+level\b', '跨层退出使用 break 标签;，不得保留旧层数退出语法', ()),
    (r'break\s+all\b.*退出.*(?:全部|所有).*循环', 'all 是普通循环标签，不表示退出全部循环', ()),
    (r'宏调用名.*只认小写', '宏名允许 ASCII 大小写字母并区分大小写', ()),
    (r'分配一律写出类型', 'new[n] 的数组元素类型由上下文推导', ()),
    (r'IR（四元式）和 VM 使用最高可用优化|maximum VM and quadruple optimization',
     '四元式不做内部优化，原生代码优化由 LLVM 执行', ()),
    (r'\$SQUARE\(next\(\)\).*实参展开两次', '普通宏值参数只求值一次，expr 片段按插入位置执行', ()),
    (r'以及 `Any`、未定型.*一律拒绝', 'Any 支持从可写位置移动，不能与未定型占位一并拒绝', ()),
)


def markdownFiles():
    """收集参与扫描的 Markdown 文件，跳过更改记录与历史测量目录。"""
    for path in ROOT.rglob('*.md'):
        relative = path.relative_to(ROOT).as_posix()
        if relative == 'CHANGELOG.md':
            continue
        if any(relative == part or relative.startswith(part + '/') for part in EXCLUDED_PARTS):
            continue
        yield relative, path


def main():
    """逐文件匹配规则；出现豁免子串的行视为已说明「已移除」的合法语境。"""
    violations = []
    for relative, path in markdownFiles():
        for number, line in enumerate(path.read_text(encoding='utf-8').splitlines(), 1):
            for pattern, reason, exemptions in RULES:
                if not re.search(pattern, line):
                    continue
                if any(item in line for item in exemptions):
                    continue
                violations.append(f'{relative}:{number}: {reason}\n    {line.strip()[:120]}')
    if violations:
        print(f'检测到 {len(violations)} 处历史表述残留（更改记录以外）：')
        for item in violations:
            print(' -', item)
        raise SystemExit(1)
    skill = ROOT / 'skills/chali-programming/SKILL.md'
    bundledSkill = ROOT / 'vscode/skills/chali-programming/SKILL.md'
    assert skill.read_text(encoding='utf-8') == bundledSkill.read_text(encoding='utf-8'), '仓库与 VS Code 的编程 SKILL 不一致'
    print('历史表述门禁通过：当前 Markdown 无已登记的旧语法与执行方式残留，两份编程 SKILL 一致。')


if __name__ == '__main__':
    main()
