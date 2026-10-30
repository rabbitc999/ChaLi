"""验证带类别变参的单次求值、语法角色、嵌套重复和空捕获。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行真实程序，区分 value 缓存与 expr 插入，并核对错误类别。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument("--case", action="append", default=[])
    options=parser.parse_args()
    definitions='''
int64 calls=0;int64 next(){calls+=1;return calls;}
$macro SUM(...xs:value) -> expr
int64 total=0;
$for value in xs
total+=value+value;
$end_for
total
$end_macro
$macro EXPRESSIONS(...xs:expr) -> expr
int64 total=0;
$for value in xs
total+=value+value;
$end_for
total
$end_macro
$macro IGNORE(...xs:value) -> expr
0
$end_macro
$macro FORWARD(...xs:value) -> expr
$SUM(xs)
$end_macro
$macro NEST(...xs:value) -> expr
int64 total=0;
$for left in xs
$for right in xs
total+=left+right;
$end_for
$end_for
total
$end_macro
$macro NAMES(...names:ident) -> stmt
$for name in names
int64 name=7;
$end_for
$end_macro
$macro TYPES(...types:type) -> expr
int64 total=0;
$for T in types
T local=cast<T>(3);
total+=cast<int64>(local);
$end_for
total
$end_macro
$macro STEPS(...steps:stmt) -> stmt
$for step in steps
step;
$end_for
$end_macro
$macro BLOCKS(...blocks:block) -> stmt
$for body in blocks
body;
$end_for
$end_macro
'''
    cases=[('values',definitions+'''void main(){calls=0;assert($SUM(next(),next())==6 && calls==2);assert($SUM()==0);calls=0;assert($IGNORE(next(),next())==0 && calls==2);}''',None),
('expressions',definitions+'void main(){calls=0;assert($EXPRESSIONS(next(),next())==10 && calls==4);}',None),
('forward',definitions+'void main(){calls=0;assert($FORWARD(next(),next())==6 && calls==2);}',None),
('nested',definitions+'void main(){calls=0;assert($NEST(next(),next())==12 && calls==2);}',None),
('names',definitions+'void main(){$NAMES(first,second);assert(first+second==14);}',None),
('types',definitions+'void main(){assert($TYPES(int8,int16,int32,uint64)==12);}',None),
('statements',definitions+'void main(){int64 x=0;$STEPS(x+=1;,x+=2;);$BLOCKS({x+=3;},{x+=4;});assert(x==10);}',None),
('copy', '''
$macro CHANGE(...values:value) -> stmt
$for value in values
value.push(7);
$end_for
$end_macro
void main(){List<int64> a;a.push(1);List<int64> b;b.push(2);$CHANGE(a,b);assert(a.len()==1 && b.len()==1);}
''',None)]
    for name,call in [('value','$SUM(int64);'),('expr','$EXPRESSIONS(int64);'),('ident','$NAMES(1+2);'),('type','$TYPES(1+2);'),('stmt','$STEPS(int64);'),('block','$BLOCKS(1+2);')]:
        cases.append((name,definitions+'void main(){'+call+'}','形参类别不匹配'))
    if options.case:
        cases = [case for case in cases if case[0] in options.case]
        assert cases, options.case
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-variadics-') as directory:
        work=Path(directory)
        for name,source,diagnostic in cases:
            path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostic else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=(result.returncode!=0 and diagnostic in result.stderr) if diagnostic else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result))
    assert not failures,failures
    print(f'宏类别变参通过：{len(cases)} 项')


if __name__=='__main__':
    main()
