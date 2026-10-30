"""执行书本中标记的新增完整程序，并核对 LaTeX 生成内容。"""
from compiler_path import compiler_path
from pathlib import Path
import os
import re
import subprocess
import sys
import json
from project_temp import tempfile
from concurrent.futures import ThreadPoolExecutor

ROOT=Path(__file__).resolve().parents[1]
COMPILER=Path(os.environ.get('CHALI_BIN',str(compiler_path())))
sys.path.insert(0,str(ROOT/'doc/book/latex/tools'))
from md2tex import Converter, FILES, CHAPTERS, main as generate_chapters
IMPORTS={c["zh"]:c.get("imports",[]) for c in CHAPTERS}
EXTRAS=json.loads((ROOT/"tests/book_extras_manifest.json").read_text(encoding="utf-8"))["sha256"]


def main():
    """新章节示例须产生所列输出，生成章节须与 Markdown 一致。"""
    # 生成章节不入库：全新检出先生成，已有章节仍逐字核对是否过期。
    if any(not (ROOT/'doc/book/latex/chapters'/target).exists() for _,target in FILES):
        generate_chapters()
    book=(ROOT/'doc/book/latex/book.tex').read_text(encoding='utf-8')
    assert not re.search(r'^\s*\\part\*?\{',book,re.M), '书稿直接按章节排列，不使用 part 分组'
    # 章节数跟随 md2tex 的映射表，加章时只改那一处，不用再改这里的常数。
    chapterCount = sum(1 for _, name in FILES if name.startswith('ch'))
    assert len(re.findall(r'\\input\{chapters/ch\d+\}',book))==chapterCount
    for name in (name for _, name in FILES if name.startswith('ch')):
        assert re.search(r'\\input\{chapters/%s\}' % name[:-4], book), name
    headings=Converter().convert('## 3.7 字面量\n\n### 数字后缀\n\n说明。')
    assert r'\subsection*{数字后缀}' in headings and r'\subsection{数字后缀}' not in headings
    directive=Converter().convert('#borrow 描述借用来源。\n\n## 22.3 借用\n\n正文。')
    assert r'\#borrow' in directive and r'\chapter{' not in directive, directive
    numbered=Converter().convert('## 15.7 捕获\n\n### 15.7.1 借用\n\n正文。\n\n### 15.7.2 拥有\n\n正文。')
    assert r'\subsection{借用}' in numbered and r'\subsection{拥有}' in numbered
    try:
        Converter().convert('## 3.7 字面量\n\n### 3.7.1 唯一子节\n\n正文。')
    except ValueError as error:
        assert '孤立编号子节' in str(error)
    else:
        raise AssertionError('转换器不能重新引入孤立编号子节')
    for image in ('chali-compilation.png', 'chali-aot.png'):
        assert (ROOT/'doc/book/images'/image).read_bytes()==(ROOT/'doc/images'/image).read_bytes(),image
    fixture = "1. 先声明：\n\n   ```chali\n   int64 value = 1;\n   ```\n\n2. 再读取：\n\n   ```chali\n   println(\"{}\", value);\n   ```\n\n- 无序说明：\n\n  ```text\n  output\n  ```"
    rendered = Converter().convert(fixture)
    assert r'\begin{enumerate}[start=2]' in rendered
    assert 'minipage' not in rendered and 'samepage' not in rendered
    environments=[]
    for tag, name in re.findall(r'\\(begin|end)\{([^}]+)\}', rendered):
        if tag == 'begin':
            if name == 'lstlisting':
                assert not any(item in ('enumerate', 'itemize') for item in environments)
            environments.append(name)
        else:
            assert environments.pop() == name
    assert not environments
    try:
        Converter().convert('| chapter | title |\n| 24 | project |\n| 25 | Linux |')
    except ValueError as error:
        assert '分隔行' in str(error)
    else:
        raise AssertionError('没有表头分隔行的表格不能静默丢行')
    # 双反引号代码段要整体识别，拆成单反引号会在 PDF 里漏出裸反引号。
    double = Converter().convert('| 指令 | 作用 |\n| --- | --- |\n| `` $macro N((a)) `` | 说明 |')
    assert '`' not in double and r'\texttt{ \$macro N(' in double, double
    operators = Converter().convert('| 用途 | 操作符 |\n| --- | --- |\n| 逻辑 | \\|\\| |')
    assert '逻辑 & ||' in operators, operators
    preface=(ROOT/'doc/book/latex/chapters/preface.tex').read_text(encoding='utf-8')
    for chapter in range(1, chapterCount + 1):
        assert preface.count(r'\hyperref[ch:%02d]{%02d} &' % (chapter, chapter)) == 1, chapter
    count=0
    programs=[]
    with tempfile.TemporaryDirectory(prefix='chali-book-') as folder:
        work=Path(folder)
        for source,target in FILES:
            path=ROOT/'doc/book'/source
            text=path.read_text(encoding='utf-8')
            if re.match(r'^\d\d-', source):
                assert '参考解答与验收要点' in text, source
                assert len(re.findall(r'^\d+\. ', text[text.index('参考解答与验收要点'):], re.M)) >= 4, source
                if source in EXTRAS:
                    assert '番外：' in re.findall(r'^## (.+)$', text, re.M)[-1], source
            mode='preface' if source=='README.md' else 'appendix' if source.startswith(('A-','B-','C-')) else 'chapter'
            expected=Converter(mode).convert(text)
            if mode == 'chapter':
                assert expected.count(r'\chapter{') == 1, ('正文被误识别为章标题', source)
            assert (ROOT/'doc/book/latex/chapters'/target).read_text(encoding='utf-8')==expected,target
            assert 'minipage' not in expected and 'samepage' not in expected, target
            list_depth=0
            for match in re.finditer(r'\\(begin|end)\{(enumerate|itemize|lstlisting)\}', expected):
                direction, environment=match.groups()
                if environment == 'lstlisting':
                    assert list_depth == 0, (target, match.start())
                else:
                    list_depth += 1 if direction == 'begin' else -1
            assert list_depth == 0, target
            for input_json,code,output in re.findall(r'<!-- verify-book(?:-input:\s*("(?:\\.|[^"\\])*"))? -->\s*```chali\n(.*?)```\s*```text\n(.*?)```',text,re.S):
                count+=1;file=work/f'example{count}.chl';file.write_text(code,encoding='utf-8')
                programs.append((source,file,work/f'output{count}',output,json.loads(input_json) if input_json else None))
        def execute(program):
            """在独立产物路径并行验证一个书稿完整程序。"""
            source,file,prefix,output,stdin=program
            environment=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1')
            if stdin is not None:
                environment.pop('CHALI_NO_STDIN_INPUT',None)
            includes=[arg for directory in IMPORTS.get(source,[]) for arg in ('--include-path',str(ROOT/directory))]
            result=subprocess.run([str(COMPILER),'run',str(file),'--out',str(prefix),*includes],cwd=work,
                env=environment,input=stdin,capture_output=True,encoding='utf-8',timeout=180)
            assert result.returncode==0 and result.stdout.replace('\r\n','\n')==output,(source,result.stdout,result.stderr,output)
        with ThreadPoolExecutor(max_workers=4) as pool:
            list(pool.map(execute, programs))
        chapter=(ROOT/'doc/book/22-预处理与宏.md').read_text(encoding='utf-8')
        invalid=re.search(r'<!-- verify-book-error: temp -->\s*```chali\n(.*?)```',chapter,re.S)[1]
        bad=work/'invalid_macro.chl';bad.write_text(invalid,encoding='utf-8')
        result=subprocess.run([str(COMPILER),'check',str(bad)],cwd=work,capture_output=True,encoding='utf-8',timeout=60)
        assert result.returncode!=0 and 'temp' in result.stderr and '分支结果类型必须一致' not in result.stderr,result
        kong=(ROOT/'doc/book/19-函数值与闭包.md').read_text(encoding='utf-8')
        assert '鲁镇的码栈' in kong and '违约金零头' in kong and not re.search(r'^\[\d+\]',kong,re.M)
    # 附录 A 的 <!-- fail --> 示例逐个重跑：必须失败，且文档记载的首条消息真实出现。
    with tempfile.TemporaryDirectory(prefix='chali-book-fail-') as failFolder:
        failWork=Path(failFolder)
        appendix=(ROOT/'doc/book/A-常见错误速查.md').read_text(encoding='utf-8')
        failBlock=re.compile(r'<!-- fail[^>]*-->\s*```chali\n(.*?)```\s*```text\n(.*?)```',re.S)
        position=re.compile(r'\S+ 第 \d+ 行(第 \d+ 列)?：')
        fails=failBlock.findall(appendix)
        assert fails,'附录 A 缺少 fail 示例'
        for index,(code,expected) in enumerate(fails):
            file=failWork/f'appendix-fail{index}.chl';file.write_text(code,encoding='utf-8')
            result=subprocess.run([str(COMPILER),'run',str(file),'--out',failWork/f'appendix-fail{index}'],cwd=failWork,
                env=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1'),capture_output=True,encoding='utf-8',timeout=180)
            actual=result.stderr.replace('\r\n','\n')
            lines=actual.splitlines()
            first=expected.strip().splitlines()[0] if expected.strip() else ''
            documented=position.sub('',first).strip()
            assert result.returncode!=0,(index,actual)
            assert lines and (position.sub('',lines[0]).strip()==documented or documented in actual),(index,first,lines[0] if lines else '')
    assert count>=22,count
    print(f'新增 {count} 个书本完整程序输出、{len(fails)} 个附录失败示例与全部 LaTeX 生成章节一致。')


if __name__=='__main__':main()
