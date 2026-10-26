"""从权威文法、语言参考、书稿与 AI SKILL 生成内嵌手册数据。"""
import argparse
import json
from pathlib import Path
import re
from embedded_asset_cache import AssetCache

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / 'tools/chali_manual.json'


def generate():
    """收集实际 Markdown 与关键字对应章节，不在运行时访问源码目录。"""
    book = []
    for path in sorted((ROOT / 'doc/book').glob('*.md')):
        text = path.read_text(encoding='utf-8')
        identifier = path.stem.split('-')[0] if '-' in path.stem else 'index'
        book.append(dict(id=identifier, title=text.splitlines()[0].lstrip('# '),
                         path=path.relative_to(ROOT).as_posix(), content=text))
    english_book = []
    for path in sorted((ROOT / 'doc/book/en').glob('*.md')):
        text = path.read_text(encoding='utf-8')
        identifier = path.stem.split('-')[0] if '-' in path.stem else 'index'
        english_book.append(dict(id=identifier, title=text.splitlines()[0].lstrip('# '),
                                 path=path.relative_to(ROOT).as_posix(), content=text))
    assert {chapter['id'] for chapter in english_book} == {chapter['id'] for chapter in book}, '中英文书稿章节编号必须对应'
    assert len(english_book) == len(book) == len({chapter['id'] for chapter in book}), '每个书稿章节编号只能出现一次'
    reference = (ROOT / 'doc/04-language-reference.md').read_text(encoding='utf-8')
    sections = re.split(r'(?=^## )', reference, flags=re.M)
    groups = {
        '2.': 'int8 int16 int32 int64 uint8 uint16 uint32 uint64 float32 float64 bool char void true false',
        '3.': 'let const static type', '4.': 'return new move export', '5.': 'cast',
        '6.': 'struct', '7.': 'enum match _', '8.': 'interface',
        '10.': 'if elif else while do for loop continue in',
        '12.': 'module import inline', '13.': 'mut',
        '15.': '@annotation', '16.': 'abi opcode #NonCopyable', 'fn 捕获': 'fn copy move',
        '返回控制表达式': 'return', '具名函数声明': 'fn mut copy move', '#derive': '#derive', '#borrow 与 #static 生命周期契约': '#borrow #static #ret',
        'for-in 整数': 'for let in', '带值退出': 'break yield',
        '指定层数退出': 'break', '块表达式': 'break',
    }
    token_source = (ROOT / 'source/chali_token.gram').read_text(encoding='utf-8')
    grammar = (ROOT / 'source/chali.gram').read_text(encoding='utf-8')
    productions = re.findall(r'^\w+\s*:=.*?\s;(?=\s*(?:/\*|\w+\s*:=|$))', grammar, re.M | re.S)
    keywords = {}
    for token, word in re.findall(r'^%(?:token|define)\s+(\w+)\s+"([@#]?[A-Za-z_][A-Za-z_0-9]*)"', token_source, re.M):
        if word.startswith('__chali_') or word == '__typeTag':
            continue
        selected = [section for prefix, names in groups.items() if word in names.split()
                    for section in sections if section.startswith('## ' + prefix)]
        source = 'doc/04-language-reference.md'
        if token.startswith('MACRO_'):
            source = 'doc/24-literals-operators-macros.md'
            selected = [(ROOT / source).read_text(encoding='utf-8')]
        if word in ('defer', 'scope', 'drop'):
            source = 'doc/book/08-资源与作用域.md'
            selected = [(ROOT / source).read_text(encoding='utf-8')]
        if word == 'impl':
            source = 'doc/book/21-与C互操作.md'
            selected = [(ROOT / source).read_text(encoding='utf-8')]
        assert selected, f'关键字缺少明确的文档映射：{word}'
        if word in ('return', 'fn', 'copy', 'move', 'mut', '#derive'):
            selected.append((ROOT / 'doc/28-declaration-sugars.md').read_text(encoding='utf-8'))
        if word in ('uint8', 'uint16', 'uint32', 'uint64', 'float32', 'float64'):
            chapter = (ROOT / 'doc/book/03-变量与类型.md').read_text(encoding='utf-8')
            selected.extend(part for part in re.split(r'(?=^## )', chapter, flags=re.M) if '1.0f' in part and '1u' in part)
        syntax = '\n\n'.join(part.strip() for part in productions if re.search(r'\b' + re.escape(token) + r'\b', part))
        keywords[word] = dict(name=word, token=token, syntax=syntax,
                              documentation='\n\n'.join(selected), source=source)
    macro_source = 'doc/24-literals-operators-macros.md'
    macro_sections = re.split(r'(?=^## )', (ROOT / macro_source).read_text(encoding='utf-8'), flags=re.M)
    keywords['macro'] = dict(name='macro', token='$macro', syntax='$macro NAME<T:arg>[..Attrs](arg, ..Args){Body}\n...\n$end_macro',
                             documentation='\n\n'.join(section for section in macro_sections if section.startswith(('## 编译期卫生', '## 参数组', '## 宏泛型', '## 五类', '## 正则'))), source=macro_source)
    assert 'break' in keywords and 'struct' in keywords and len(book) >= 25
    images = [dict(path='images/' + path.name, hex=path.read_bytes().hex())
              for path in sorted((ROOT / 'doc/book/images').glob('*')) if path.is_file()]
    api_pages = {path.name: path.read_text(encoding='utf-8') for path in sorted((ROOT / 'doc/library').glob('*.md'))}
    return json.dumps(dict(schemaVersion=1, sourceLanguage='zh', apiPages=api_pages, book=book, bookEn=english_book, images=images, keywords=keywords,
                           skill=(ROOT / 'skills/chali-programming/SKILL.md').read_text(encoding='utf-8')),
                      ensure_ascii=False, indent=2) + '\n'


def ensure_generated(check=False):
    """先核对文档、图片和生成脚本摘要，缓存命中时直接使用已有手册。"""
    inputs = {path.relative_to(ROOT).as_posix(): path for pattern in (
        'doc/book/*.md', 'doc/book/en/*.md', 'doc/book/images/*', 'doc/library/*.md',
        'doc/04-language-reference.md', 'doc/24-literals-operators-macros.md',
        'doc/28-declaration-sugars.md', 'skills/chali-programming/SKILL.md',
        'source/chali.gram', 'source/chali_token.gram', 'tools/embed_manual.py',
    ) for path in ROOT.glob(pattern) if path.is_file()}
    cache = AssetCache('manual', inputs)
    if cache.valid([OUTPUT]):
        print('内嵌手册 SHA256 相同，跳过生成。')
        return
    text = generate()
    if check:
        assert OUTPUT.read_text(encoding='utf-8') == text, '内嵌书稿、关键字或 SKILL 数据需要重新生成'
    elif not OUTPUT.exists() or OUTPUT.read_text(encoding='utf-8') != text:
        OUTPUT.write_text(text, encoding='utf-8')
    cache.save([OUTPUT])


def main():
    """生成数据或检查已生成数据是否与文档一致。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    options = parser.parse_args()
    ensure_generated(options.check)
    print('内嵌书稿、关键字与 SKILL 数据同步。')


if __name__ == '__main__':
    main()
