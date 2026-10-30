from pathlib import Path
import sys
sys.path.insert(0,str(Path('doc/book/latex/tools').resolve()))
from md2tex import Converter,FILES
for source,target in FILES:
    mode='preface' if source=='README.md' else 'appendix' if source.startswith(('A-','B-')) else 'chapter'
    actual=Converter(mode).convert((Path('doc/book')/source).read_text(encoding='utf-8'))
    assert (Path('doc/book/latex/chapters')/target).read_text(encoding='utf-8')==actual,source
    if mode=='chapter':assert actual.count('\\chapter{')==1,source
assert '\\chapter{' not in Converter().convert('#borrow 是正文。\n\n## 22.3 借用\n\n正文。')
print('25 章、2 附录与前言的最终 LaTeX 同步，章标题边界正确。')
