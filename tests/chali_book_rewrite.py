"""核对教程重写保留番外原文，双语例程一致，并清除指定宣传表述。"""
import argparse
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PROGRAM = re.compile(r'<!-- verify-book(?:-input:\s*("(?:\\.|[^"\\])*"))? -->\s*```chali\n(.*?)```\s*```text\n(.*?)```', re.S)


def main():
    """按冻结摘要检查番外，并检查完整例程的双语对应。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chapters', help='仅核对指定逗号分隔章号；番外摘要仍核对全部章节')
    args = parser.parse_args()
    selected = set(map(int, args.chapters.split(','))) if args.chapters else set(range(1, 26))
    fixture = json.loads((ROOT / 'tests/book_extras_manifest.json').read_text(encoding='utf-8'))
    for name, digest in fixture['sha256'].items():
        text = (ROOT / 'doc/book' / name).read_text(encoding='utf-8')
        match = re.search(r'^## [^\n]*番外：[^\n]*\n', text, re.M)
        assert match, name
        tail = re.sub(r'^## \d+\.\d+ ', '## ', text[match.start():], count=1).rstrip() + '\n'
        assert hashlib.sha256(tail.encode()).hexdigest() == digest, ('番外原文改变', name)
        if int(name[:2]) not in selected:
            continue
        en = re.search(r'\[English\]\(en/([^)]*)\)', text)[1]
        translated = (ROOT / 'doc/book/en' / en).read_text(encoding='utf-8')
        assert PROGRAM.findall(text[:match.start()]) == PROGRAM.findall(translated), ('双语例程不同', name)
        prose = re.sub(r'```.*?```', '', text[:match.start()], flags=re.S)
        for phrase in ('绝不共享内存', '深到底', '零开销', '永远是对的', '一定能', '万无一失', '每段代码都实测运行过', 'ChaLi 没有这回事'):
            assert phrase not in prose, (name, phrase)
    print(f'番外原文 {len(fixture["sha256"])} 章一致；重写教程 {len(selected)} 章双语例程与表述检查通过')


if __name__ == '__main__':
    main()
