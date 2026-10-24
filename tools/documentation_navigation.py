"""为受维护 Markdown 生成语言导航，并按文档语言选择实际存在的相对链接。"""
from functools import lru_cache
import os
from pathlib import Path
import re
import subprocess
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
BEGIN = '<!-- LANGUAGE NAVIGATION -->'
END = '<!-- END LANGUAGE NAVIGATION -->'


@lru_cache(maxsize=1)
def documents():
    """枚举版本管理文件及新增 Markdown，不处理构建和临时目录。"""
    names = subprocess.check_output(['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard'], cwd=ROOT).decode().split('\0')
    return tuple(sorted({ROOT / name for name in names if name.endswith('.md') and (ROOT / name).is_file()}))


def language(path, text=None):
    """优先按目录及文件名确定语言，其余文档按首个标题辨认。"""
    relative = path.relative_to(ROOT)
    if path == ROOT / 'README.md' or 'en' in relative.parts or path.name.endswith('.en.md'):
        return 'en'
    if path.name.endswith(('.cn.md', '.zh.md')):
        return 'zh'
    content = path.read_text(encoding='utf-8') if text is None else text
    heading = re.search(r'(?m)^#\s+(.+)', content)
    sample = heading[1] if heading else content[:300]
    return 'zh' if re.search(r'[\u3400-\u9fff]', sample) else 'en'


def counterpart(path, wanted):
    """查找同名语言目录、README 后缀或同编号书稿，不凭空构造目标。"""
    if path in (ROOT / 'README.md', ROOT / 'README.cn.md', ROOT / 'README.en.md'):
        return ROOT / ('README.md' if wanted == 'en' else 'README.cn.md')
    relative = path.relative_to(ROOT)
    candidates = []
    if wanted == 'en':
        candidates += [path.parent / 'en' / path.name, path.with_name(path.stem + '.en.md')]
        if relative.parts[0] == 'doc':
            candidates.append(ROOT / 'doc/en' / Path(*relative.parts[1:]))
        if path.parent == ROOT / 'doc/book':
            prefix = path.stem.split('-')[0]
            candidates += [item for item in documents() if item.parent == ROOT / 'doc/book/en' and item.stem.split('-')[0] == prefix]
    else:
        if 'en' in relative.parts:
            parts = list(relative.parts)
            parts.remove('en')
            candidates.append(ROOT / Path(*parts))
        if path.name.endswith('.en.md'):
            candidates += [path.with_name(path.name[:-6] + '.cn.md'), path.with_name(path.name[:-6] + '.md')]
        if path.parent == ROOT / 'doc/book/en':
            prefix = path.stem.split('-')[0]
            candidates += [item for item in documents() if item.parent == ROOT / 'doc/book' and item.stem.split('-')[0] == prefix]
    for candidate in candidates:
        if candidate.is_file() and language(candidate) == wanted:
            return candidate
    return None


def relative_link(source, target):
    """生成从当前 Markdown 到目标文件的正斜杠相对路径。"""
    return os.path.relpath(target, source.parent).replace('\\', '/')


def navigation(path, text):
    """生成明确标注当前语言及缺失译文回退的导航。"""
    if language(path, text) == 'en':
        chinese = counterpart(path, 'zh')
        label = '简体中文' if chinese else '简体中文入口'
        bar = '**English** · [' + label + '](' + relative_link(path, chinese or ROOT / 'README.cn.md') + ')'
    else:
        english = counterpart(path, 'en')
        label = 'English' if english else 'English（中文原文）'
        bar = '[' + label + '](' + relative_link(path, english or path) + ') · **简体中文**'
    return BEGIN + '\n' + bar + '\n' + END


def localize_links(path, text):
    """只改普通 Markdown 相对文档链接，保留代码块、行内代码和显式语言切换。"""
    wanted = language(path, text)
    inside = False
    fence = None
    output = []
    for line in text.splitlines(keepends=True):
        marker = re.match(r'^\s*(`{3,}|~{3,})', line)
        if marker:
            if fence is None:
                fence = marker[1][0]
            elif marker[1][0] == fence:
                fence = None
            output.append(line)
            continue
        if line.strip() == BEGIN:
            inside = True
        if fence or inside:
            output.append(line)
            if line.strip() == END:
                inside = False
            continue

        def rewrite(match):
            """保留链接标签、片段与标题，仅替换有现存译文的 Markdown 目标。"""
            label, href = match[1], match[2]
            explicit_chinese = re.search(r'\bChinese\b|中文原文|中文 README', label, re.I)
            if not explicit_chinese and re.search(r'English|简体中文', label, re.I):
                return match[0]
            parsed = urlsplit(href)
            if parsed.scheme or parsed.netloc or not parsed.path.endswith('.md') or parsed.query:
                return match[0]
            target = (path.parent / unquote(parsed.path)).resolve()
            if not target.is_relative_to(ROOT) or not target.is_file():
                return match[0]
            target_language = 'zh' if explicit_chinese else wanted
            selected = target if language(target) == target_language else counterpart(target, target_language)
            if selected is None:
                return match[0]
            # 译文标题的锚点通常不同；切换目标时定位到译文页面，避免沿用失效锚点。
            fragment = parsed.fragment if selected == target else ''
            value = relative_link(path, selected) + ('#' + fragment if fragment else '')
            return '[' + label + '](' + value + ')'

        parts = re.split(r'(`+[^`\n]*`+)', line)
        for index in range(0, len(parts), 2):
            parts[index] = re.sub(r'(?<!!)\[([^\]]+)\]\(([^\s)]+)\)', rewrite, parts[index])
        output.append(''.join(parts))
    return ''.join(output)


def decorate(path, text):
    """幂等更新文档导航并删除指定日文口号。"""
    text = re.sub(r'\n*' + re.escape(BEGIN) + r'.*?' + re.escape(END) + r'\n*', '\n\n', text, flags=re.S)
    text = text.replace(' · 一度書けば、どこでも動く。', '').replace('· 一度書けば、どこでも動く。', '')
    lines = text.splitlines()
    cleaned = []
    for line in lines:
        if '·' in line and re.fullmatch(r'\s*>?\s*(?:\*\*[^*]+\*\*|\[[^\]]+\]\([^)]+\))\s*·\s*(?:\*\*[^*]+\*\*|\[[^\]]+\]\([^)]+\))\s*', line):
            if ('English' in line or '简体中文' in line) and ('中文' in line or 'Chinese' in line):
                continue
        cleaned.append(line)
    text = '\n'.join(cleaned).rstrip() + '\n'
    heading = re.search(r'(?m)^# .*(?:\n|$)', text)
    if heading:
        at = heading.end()
        text = text[:at].rstrip() + '\n\n' + navigation(path, text) + '\n\n' + text[at:].lstrip('\n')
    else:
        frontmatter = re.match(r'^---\n.*?\n---\n', text, re.S)
        at = frontmatter.end() if frontmatter else 0
        text = text[:at].rstrip() + ('\n\n' if at else '') + navigation(path, text) + '\n\n' + text[at:].lstrip('\n')
    return localize_links(path, text)


def main():
    """更新受维护文档，不调用测试、编译器或门禁。"""
    changed = 0
    for path in documents():
        old = path.read_text(encoding='utf-8')
        new = decorate(path, old)
        if old != new:
            path.write_text(new, encoding='utf-8')
            changed += 1
    print(f'文档语言导航：{len(documents())} 页，更新 {changed} 页。')


if __name__ == '__main__':
    main()
