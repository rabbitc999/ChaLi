"""验证同名接口与成员宏配对的自定义派生、卫生和冲突。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行真实派生程序，拒绝不合法的定义、布局变化和成员覆盖。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    parser.add_argument('--preview',action='store_true',help='选择用例时也核对真实编辑器派生预览')
    args=parser.parse_args()
    describe='''interface Describe{String describe();}
$macro Describe<T>() -> member
String describe(){return "derived";}
$end_macro
'''
    count='''interface Count{int64 count();}
$macro Count<T>() -> member
int64 count(){int64 result=0;
$for field in $fields<T>()
result+=1;
$end_for
return result;}
$end_macro
'''
    cases=[
        ('basic',describe+'''#derive(Describe)
struct Data{int64 value;}
void main(){Data value;Describe contract=value;assert(value.describe()=="derived"&&contract.describe()=="derived");}''',None),
        ('imported','''import derive_tools;
#derive(derive_tools::Describe)
struct Data{int64 value;}
void main(){Data value;derive_tools::Describe contract=value;assert(contract.describe()=="imported");}''',None),
        ('overloads','''interface Read{int64 read();}
$macro Read<T>() -> member
int64 read(){return 7;}
int64 read(int64 value){return value+1;}
$end_macro
#derive(Read)
struct Data{}
void main(){Data value;assert(value.read()==7&&value.read(8)==9);}''',None),
        ('duplicate',describe+'''#derive(Describe,Describe)
struct Data{}void main(){}''',['重复请求接口']),
        ('qualified_drop','#derive(std::lang::Drop)\nstruct Data{}void main(){}',['禁止生成 Drop']),
        ('qualified_generic_drop','#derive(std::lang::Closeable)\nstruct Data<T>{T value;}void main(){Data<int64> value;}',['禁止生成 Drop']),
        ('wrong_category','''interface Read{int64 read();}
$macro Read<T>() -> expr
7
$end_macro
#derive(Read)
struct Data{}void main(){}''',['member']),
        ('recursive','''interface Read{int64 read();}
$macro Read<T>() -> member
$Read<T>();
$end_macro
#derive(Read)
struct Data{}void main(){}''',['循环依赖']),
        ('fields',count+'''#derive(Count)
struct Data{int64 left;String right;}
void main(){Data value;assert(value.count()==2);}''',None),
        ('imported_field_types','''import std::random;
import std::regex;
'''+count+'''#derive(Count)
struct Data{Random generator;Regex pattern;}
void main(){Data value;assert(value.count()==2);}''',None),
        ('existing_member',count+'''$macro FIELD(name:ident) -> member
int64 name;
$end_macro
#derive(Count)
struct Data{$FIELD(value);}
void main(){Data value;assert(value.count()==1);}''',None),
        ('generic_owner',count+'''#derive(Count)
struct Data<T>{T value;}
void main(){Data<int16> a;Data<String> b;assert(a.count()==1&&b.count()==1);}''',None),
        ('constraint',count+'''#derive(Count)
struct Data{int64 value;}
int64 total<T:Count>(T value){return value.count();}
void main(){Data value;assert(total(value)==1);}''',None),
        ('late_metadata', '''struct Other<T>{T value;}
interface Count{int64 count();}
$macro Count<T>() -> member
int64 count(){int64 result=0;
$for field in $fields<Other<int16>>()
result+=1;
$end_for
return result;}
$end_macro
#derive(Count)
struct Data{}
void main(){Data value;assert(value.count()==1);}''',None),
        ('multiple_retry', count+'''struct Other<T>{T value;}
interface Read{int64 read();}
$macro Read<T>() -> member
int64 read(){int64 result=0;
$for field in $fields<Other<int16>>()
result+=1;
$end_for
return result;}
$end_macro
#derive(Count,Read)
struct Data{int64 value;}
void main(){Data value;assert(value.count()==1&&value.read()==1);}''',None),
        ('generic_contract','''interface Describe<T>{String describe(T value);}
$macro Describe<T>() -> member
String describe(T value){return "generic";}
$end_macro
#derive(Describe)
struct Data{int64 value;}
void main(){Data value;assert(value.describe(value)=="generic");}''',None),
        ('helper','''interface Read{int64 read();}
$macro Read<T>() -> member
int64 helper(){return 7;}
int64 read(){return self.helper();}
$end_macro
#derive(Read)
struct Data{}
void main(){Data value;assert(value.read()==7);}''',None),
        ('inherited','''interface Base{int64 read();}
interface Read:Base{int64 extra(){return 2;}}
$macro Read<T>() -> member
int64 read(){return 7;}
$end_macro
#derive(Read)
struct Data{}
void main(){Data value;Read contract=value;assert(contract.read()==7&&contract.extra()==2);}''',None),
        ('associated','''interface Read{type Item;Item read();}
$macro Read<T>() -> member
int64 read(){return 7;}
$end_macro
#derive(Read)
struct Data{type Item=int64;}
void main(){Data value;assert(value.read()==7);}''',None),
        ('enum_payloads',describe+'''#derive(Describe)
enum State{None,Pair(int64 value,String text),Many(int8 a,int16 b,int32 c,int64 d)};
void main(){State a=State::Pair(7,"owned");State b=State::Many(1,2,3,4);State c=a;assert(a.describe()=="derived"&&b.describe()=="derived"&&c.describe()=="derived");}''',None),
        ('helper_hidden','''interface Read{int64 read();}
$macro Read<T>() -> member
int64 helper(){return 7;}
int64 read(){return self.helper();}
$end_macro
#derive(Read)
struct Data{}
void main(){Data value;value.helper();}''',['没有方法']),
        ('conflict',describe+'''#derive(Describe)
struct Data{String describe(){return "manual";}}
void main(){}''',['不覆盖已有成员']),
        ('missing_macro','interface Describe{String describe();}\n#derive(Describe)\nstruct Data{}void main(){}',['未定义宏']),
        ('missing_interface','''$macro Missing<T>() -> member
int64 value(){return 1;}
$end_macro
#derive(Missing)
struct Data{}void main(){}''',['同名接口']),
        ('field_rejected','''interface Read{int64 read();}
$macro Read<T>() -> member
int64 extra;
int64 read(){return 7;}
$end_macro
#derive(Read)
struct Data{}void main(){}''',['只能生成方法']),
        ('wrong_signature','''interface Read{int64 read();}
$macro Read<T>() -> member
String read(){return "wrong";}
$end_macro
#derive(Read)
struct Data{}void main(){}''',['read']),
        ('enum',describe+'''#derive(Describe)
enum State{None,Value(int64 value)};
void main(){State value=State::Value(7);assert(value.describe()=="derived");}''',None),
    ]
    if args.case:
        cases=[case for case in cases if case[0] in args.case]
        assert len(cases)==len(set(args.case))
    failures=[]
    with tempfile.TemporaryDirectory(prefix='custom-derive-') as directory:
        work=Path(directory)
        (work/'derive_tools.chl').write_text('''module derive_tools;
interface Describe{String describe();}
$macro Describe<T>() -> member
String describe(){return "imported";}
$end_macro
''',encoding='utf-8')
        for name,source,diagnostics in cases:
            path=work/(name+'.chl')
            path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(args.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=result.returncode!=0 and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
            print(name, '通过' if valid else ('失败 '+repr(failures[-1])), flush=True)
        assert not failures,failures
        if not args.case or args.preview:
            server=Transport(args.chali.resolve(),str(work))
            server.query('initialize',dict(rootUri=work.as_uri()))
            try:
                previews=[(describe+'#derive(Describe)\nstruct Data{int64 value;}\nvoid main(){Data value;value.describe();}\n',{'Describe'})]
                retry=next((case[1] for case in cases if case[0]=='multiple_retry'),None)
                if retry:previews.append((retry,{'Count','Read'}))
                for index,(source,expected) in enumerate(previews):
                    path=work/f'preview-{index}.chl'
                    server.send(dict(method='textDocument/didOpen',params=dict(textDocument=dict(uri=path.as_uri(),text=source,version=1))))
                    preview=server.query('chali/expandMacro',dict(textDocument=dict(uri=path.as_uri()),mode='all',position=position(source,source.index('#derive')+3)))
                    assert not preview['diagnostics'],preview
                    entries=json.loads(preview['text'])['expansions']
                    assert len(entries)==len(expected) and {entry['name'] for entry in entries}==expected,preview
            finally:server.close()
    assert not failures,failures
    print(f'自定义派生通过：{len(cases)} 项')


if __name__=='__main__':main()
