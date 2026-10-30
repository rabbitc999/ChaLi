"""验证模式宏的类别、联合分支、载荷绑定和卫生隔离。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行模式正反例并核对普通匹配语义。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument("--case", action="append", default=[])
    options = parser.parse_args()
    definition = '''
$macro SELECT(value:value, p:pattern, body:block) -> expr
match(value){p => {yield body;} _ => {0}}
$end_macro
'''
    cases = [
        ('literal', definition+'void main(){assert($SELECT(2,2,{yield 7;})==7);assert($SELECT(3,2,{yield 7;})==0);}', None),
        ('union', definition+'void main(){assert($SELECT(2,1 | 2,{yield 9;})==9);}', None),
        ('payload', definition+'enum Box{Value(int64 value),Empty} void main(){int64 x=100;Box b=Box::Value(7);assert($SELECT(b,Box::Value(x),{yield x+1;})==8);assert(x==100);}', None),
        ('multiple', definition+'enum Box{Value(int64 first,int64 second),Empty} void main(){Box b=Box::Value(7,8);assert($SELECT(b,Box::Value(x,y),{yield x+y;})==15);}', None),
        ('wildcard', '$macro ANY(p:pattern) -> expr\nmatch(1){p=>{3}}\n$end_macro\nvoid main(){assert($ANY(_)==3);}', None),
        ('forward', definition+'$macro FORWARD(v:value,p:pattern,b:block) -> expr\n$SELECT(v,p,b)\n$end_macro\nvoid main(){assert($FORWARD(2,1 | 2,{yield 4;})==4);}', None),
        ('position', '$macro BAD(p:pattern) -> expr\np+1\n$end_macro\nvoid main(){$BAD(1);}', 'pattern'),
        ('category', definition+'void main(){$SELECT(1,int64,{yield 0;});}', '类别不匹配'),
    ]
    cases.extend([
        ('variadic', '$macro CHECK(v:value,mut &out:value,...ps:pattern) -> stmt\n$for p in ps\nmatch(v){p=>{out+=1;} _=>{}}\n$end_for\n$end_macro\nvoid main(){int64 out=0;$CHECK(2,out,1,2,2 | 3);assert(out==2);}\n',None),
        ('nested_payload', 'enum Box{Value(int64 value),Empty}\n$macro TAKE(v:value,p:pattern,b:block) -> expr\nmatch(v){Box::Value(p)=>{yield b;} _=>{0}}\n$end_macro\nvoid main(){Box box=Box::Value(4);assert($TAKE(box,x,{yield x+1;})==5);}\n',None),
        ('nested_union', 'enum Box{Value(int64 value),Empty}\n$macro TAKE(v:value,p:pattern) -> expr\nmatch(v){Box::Value(p)=>{1} _=>{0}}\n$end_macro\nvoid main(){Box box=Box::Value(4);$TAKE(box,1 | 2);}\n','多个备选模式'),
    ])
    interface = "interface Base{int64 value();};struct E:Base{int64 value(){return 3;}};struct F:Base{int64 value(){return 4;}};"
    cases.extend([
        ('interface',definition+interface+'void main(){E e;Base a=e;assert($SELECT(a,E(found),{yield found.value();})==3);}',None),
        ('interface_sugar',definition+interface+'void main(){E e;Result<int64,Base> r=Result<int64,Base>::Err(e);assert($SELECT(r,Err::E(found),{yield found.value();})==3);}',None),
        ('typed_multiple',definition+interface+'enum Multi{Three(Base a,Base b,String text),Empty};void main(){E e;F f;Multi v=Multi::Three(e,f,"text");assert($SELECT(v,Three(E a,F b,text),{assert(text=="text");yield a.value()+b.value();})==7);}',None),
    ])
    if options.case:
        cases = [case for case in cases if case[0] in options.case]
        assert cases, options.case
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-patterns-') as directory:
        work=Path(directory)
        for name, source, diagnostic in cases:
            path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostic else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=(result.returncode!=0 and diagnostic in result.stderr) if diagnostic else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
    assert not failures, failures
    print(f'宏模式通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
