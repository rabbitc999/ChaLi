"""验证字段静态遍历、常量分支、成员访问及成组重复。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行编译期控制的运行结果及拒绝用例。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument("--case", action="append", default=[])
    options=parser.parse_args()
    fields='''
struct Data{int64 a;int32 b;}
$macro SUM<T>(v:value) -> expr
int64 total=0;
$for field in $fields<T>()
total+=cast<int64>($member(v,field));
$end_for
total
$end_macro
'''
    conditions='''
$macro CHOOSE(flag:expr) -> expr
$if flag
7
$else
missing(undeclared)
$end_if
$end_macro
'''
    zipdef='''
$macro ZIP(...left:value)[...right:value] -> expr
int64 total=0;
$for (a,b) in (left,right)
total+=a*b;
$end_for
total
$end_macro
'''
    cases=[
        ('fields',fields+'void main(){Data d;d.a=4;d.b=5;assert($SUM<Data>(d)==9);}',None),
        ('empty',fields+'struct Empty{} void main(){Empty e;assert($SUM<Empty>(e)==0);}',None),
        ('generic',fields+'struct Box<T>{T a;T b;} void main(){Box<int64> b;b.a=2;b.b=3;assert($SUM<Box<int64>>(b)==5);}',None),
        ('condition',conditions+'void main(){assert($CHOOSE(true)==7);}',None),
        ('condition_false',conditions.replace('7\n$else\nmissing(undeclared)','missing(undeclared)\n$else\n8')+'void main(){assert($CHOOSE(false)==8);}',None),
        ('field_condition','''
struct Data{int64 a;String b;int64 c;}
$macro SUM<T>(v:value) -> expr
int64 total=0;
$for field in $fields<T>()
$if field.type.name == "int64"
int64 local=$member(v,field);total+=local;
$end_if
$end_for
total
$end_macro
void main(){Data d;d.a=3;d.b="text";d.c=4;assert($SUM<Data>(d)==7);}
''',None),
        ('zip',zipdef+'void main(){assert($ZIP(2,3)[4,5]==23);assert($ZIP()[]==0);}',None),
        ('zip_eval',zipdef+'int64 count=0;int64 next(){count+=1;return count;}void main(){assert($ZIP(next(),next())[next(),next()]==11 && count==4);}',None),
        ('length',zipdef+'void main(){$ZIP(1,2)[3];}','长度必须一致'),
        ('arity',zipdef.replace('(a,b) in','(a) in')+'void main(){$ZIP(1)[2];}','数量必须一致'),
        ('dynamic',conditions+'bool flag(){return true;}void main(){$CHOOSE(flag());}','编译期'),
        ('source','$macro BAD\n$for f in $fields<int64>()\n0;\n$end_for\n$end_macro\nvoid main(){$BAD;}','普通结构体'),
    ]
    cases.append(('optional', '\n$macro OPTIONAL(...xs:value) -> expr\n$if $count(xs) == 0\n10\n$else\n20\n$end_if\n$end_macro\nvoid main(){assert($OPTIONAL()==10);assert($OPTIONAL(1)==20);}\n',None))
    cases.extend([
        ('deferred_tail', '$macro LOCAL<T>() -> expr\nT total=cast<T>(3);\ntotal\n$end_macro\nvoid main(){assert($LOCAL<int64>()==3);}\n',None),
        ('deferred_reference', '$macro INC<T>(mut &x:value) -> expr\nx+=cast<T>(1);\nx\n$end_macro\nvoid main(){int64 x=2;assert($INC<int64>(x)==3 && x==3);}\n',None),
        ('field_type', 'struct Data{int8 a;int32 b;}\n$macro ONE(T:type) -> expr\ncast<int64>(cast<T>(1))\n$end_macro\n$macro COUNT<T>() -> expr\nint64 total=0;\n$for field in $fields<T>()\ntotal+=$ONE(field.type);\n$end_for\ntotal\n$end_macro\nvoid main(){assert($COUNT<Data>()==2);}\n',None),
        ('field_write', 'struct Data{int64 a;int64 b;}\n$macro RESET<T>(mut &v:value) -> stmt\n$for f in $fields<T>()\n$member(v,f)=0;\n$end_for\n$end_macro\nvoid main(){Data d;d.a=4;d.b=5;$RESET<Data>(d);assert(d.a==0 && d.b==0);}\n',None),
        ('field_owner', 'struct First{int64 a;}struct Second{int64 a;}\nvoid main(){Second d;int64 x=$member(d,($fields<First>())[0]);}\n','具体类型不一致'),
        ('nested_fields', 'struct Data{int64 a;int64 b;}\n$macro NEST<T>() -> expr\nint64 total=0;\n$for outer in $fields<T>()\n$for inner in $fields<T>()\ntotal+=cast<int64>(outer.index*10+inner.index);\n$end_for\n$end_for\ntotal\n$end_macro\nvoid main(){assert($NEST<Data>()==22);}\n',None),
    ])
    cases.append(('outside', 'void main(){\n$if true\nint64 x=1;\n$end_if\n}', '宏模板内部'))
    cases.append(('field_shadow', 'struct Data{int64 a;int64 b;}\n$macro SHADOW<T>() -> expr\nint64 total=0;\n$for field in $fields<T>()\n{int64 field=7;total+=field;}\ntotal+=cast<int64>(field.index);\nint64 field=3;total+=field;\n$end_for\ntotal\n$end_macro\nvoid main(){assert($SHADOW<Data>()==21);}\n',None))
    cases.append(('field_pattern_shadow', 'struct Data{int64 a;int64 b;}enum Box{Value(int64 value),Empty}\n$macro SHADOW<T>(box:value) -> expr\nint64 total=0;\n$for field in $fields<T>()\nmatch(box){Value(field)=>{total+=field;} Empty=>{}}\ntotal+=cast<int64>(field.index);\n$end_for\ntotal\n$end_macro\nvoid main(){Box box=Box::Value(3);assert($SHADOW<Data>(box)==7);}\n',None))
    cases.append(('field_foreach_shadow', 'struct Data{int64 a;int64 b;}\n$macro SHADOW<T>(items:value) -> expr\nint64 total=0;\n$for field in $fields<T>()\nfor(field in items){total+=field;}\ntotal+=cast<int64>(field.index);\n$end_for\ntotal\n$end_macro\nvoid main(){List<int64> items;items.push(1);items.push(2);assert($SHADOW<Data>(items)==7);}\n',None))
    if options.case:
        cases = [case for case in cases if case[0] in options.case]
        assert cases, options.case
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-controls-') as directory:
        work=Path(directory)
        for name,source,diagnostic in cases:
            path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostic else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=(result.returncode!=0 and diagnostic in result.stderr) if diagnostic else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
    assert not failures,failures
    print(f'宏字段与结构化控制通过：{len(cases)} 项')


if __name__=='__main__':
    main()
