"""从实际词法声明生成书稿使用的 ChaLi listings 语言定义。"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
DESTINATION = ROOT / 'doc/book/latex/chali_language.tex'


def render():
    """保留词法关键字与库名称的区别，宏和指令使用各自的颜色组。"""
    source = (ROOT/'source/chali_token.gram').read_text(encoding='utf-8')
    literals = set(re.findall(r'^%token\s+\w+\s+"([^"\\]+)"', source, re.M))
    types = {'int8','int16','int32','int64','uint8','uint16','uint32','uint64','float32','float64','bool','char','void'}
    words = {word for word in literals if re.fullmatch(r'[A-Za-z_]\w*', word)}
    # 宏片段类别为上下文名称；与普通流程控制分开着色。
    categories = {'expr','ident','block','stmt','pattern','is'}
    controls = {word for word in words-types-categories-{'true','false'} if not word.startswith('__chali_')}
    directives = {word for word in literals if word.startswith(('$','#')) or word == '@annotation'}
    directives |= {'$fields','$member','$compile_assert','$compile_error','$stringify','$concat','$line','$file','$module','$count','$argc','$typecheck'}
    directives |= {'%if','%ifdef','%ifndef','%elif','%else','%endif','%define','%undefine','%symbol','%unsymbol','%warning','%error'}
    library = {'String','List','Map','Set','Option','Result','Value','None','Ok','Err','Any','Drop','Closeable','Copy','Error','self'}
    def group(index,values):
        escaped = [word.replace('%', r'\%').replace('#', r'\#') for word in sorted(values)]
        return '  morekeywords='+('' if index==1 else f'[{index}]')+'{'+','.join(escaped)+'},\n'
    return ('% 由 tools/gen_book_syntax.py 生成；从词法声明维护，库名称单独登记。\n'
        '\\lstdefinelanguage{ChaLi}{\n'
        '  sensitive=true,\n'
        '  alsoletter={\\$\\#@\\%},\n'
        +group(1,controls)+group(2,types|library)+group(3,directives)+group(4,categories|{'value','item','member','true','false'})+
        '  morecomment=[l]{//},\n  morecomment=[s]{/*}{*/},\n'
        '  morestring=[b]",\n  morestring=[b]\'\n}\n')


def main():
    """确定性写入语言定义，由书稿导言加载。"""
    DESTINATION.write_text(render(),encoding='utf-8',newline='\n')
    print(DESTINATION.relative_to(ROOT))


if __name__ == '__main__':
    main()
