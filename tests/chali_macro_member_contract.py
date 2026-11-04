"""验证方法接收者、直接语法效果及关联类型的编译期快照。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """执行真实源码，分别检查具体绑定、未绑定要求和非法描述符操作。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    args = parser.parse_args()
    declarations = '''interface Source {type Item; Item get();}
interface Derived:Source {int64 count();}
struct Cell:Derived {type Item=int64;int64 get(){return 7;}int64 count(){return 1;}}
struct Box<T>{T value;T read(){return self.value;}static Box<T> create(T v){self.value=v;return self;}}
'''
    cases = [
        ('receiver', declarations+'''void main(){
assert(($methods<Cell>())[0].hasReceiver);
assert(($methods<Cell>())[0].receiverType.name=="Cell");
assert(($methods<Box<int16>>())[0].receiverType.name=="Box<int16>");
assert(!($methods<Box<int16>>())[1].hasReceiver);
}''',None),
        ('static_receiver', declarations+'void main(){($methods<Box<int16>>())[1].receiverType.name;}', ['没有接收者']),
        ('binding', declarations+'''void main(){
assert(($interfaces<Cell>())[0].associatedTypes.count==1);
assert(($interfaces<Cell>())[0].associatedTypes[0].name=="Item");
assert(($interfaces<Cell>())[0].associatedTypes[0].hasDefinition);
assert(($interfaces<Cell>())[0].associatedTypes[0].type.name=="int64");
assert(($interfaces<Cell>())[0].associatedTypes[0].declaringType=="program::Source");
assert(($methods<Cell>())[0].receiverType.associatedTypes.count==1);
assert(($methods<Cell>())[0].receiverType.associatedTypes[0].type.name=="int64");
}''',None),
        ('unbound', declarations+'''struct Uses{Source value;}
void main(){assert(($fields<Uses>())[0].type.associatedTypes.count==1);assert(!($fields<Uses>())[0].type.associatedTypes[0].hasDefinition);}''',None),
        ('unbound_type', declarations+'''struct Uses{Source value;}
void main(){($fields<Uses>())[0].type.associatedTypes[0].type.name;}''',['尚未绑定']),
        ('bounds', declarations+'void main(){($interfaces<Cell>())[0].associatedTypes[1].name;}', ['关联类型索引越界']),
        ('loop', declarations+'''$macro COUNT<T>() -> expr
int64 total=0;
$for contract in $interfaces<T>()
$for associated in contract.associatedTypes
$if associated.hasDefinition
total+=1;
$end_if
$end_for
$end_for
total
$end_macro
void main(){assert($COUNT<Cell>()==2);}''',None),
        ('effects', '''struct Ops{
int64 read(){return 1;}
void touch(int64 mut& value){*value+=1;}
int64 mut& address(int64 mut& value){return value;}
int64 unsafeRead(){unsafe {return 1;}}
}
void main(){
assert(!($methods<Ops>())[0].hasMutableParameters);
assert(!($methods<Ops>())[0].returnsMutableReference);
assert(!($methods<Ops>())[0].usesUnsafe);
assert(($methods<Ops>())[1].hasMutableParameters);
assert(($methods<Ops>())[2].returnsMutableReference);
assert(($methods<Ops>())[3].usesUnsafe);
}''',None),
        ('propagation', '''struct Ops{
Result<int64,Error> read(Result<int64,Error> result){let v=result?;return Result<int64,Error>::Ok(v);}
Result<int64,Error> nested(){let f=fn(Result<int64,Error> r){let v=r?;return Result<int64,Error>::Ok(v);};return f(Result<int64,Error>::Ok(7));}
}
void main(){assert(($methods<Ops>())[0].hasPropagation);assert(!($methods<Ops>())[1].hasPropagation);}''',None),
        ('generic_binding', '''interface Read{type Item;Item get();}
struct Holder<T>:Read{type Item=T;T value;T get(){return self.value;}}
void main(){assert(($interfaces<Holder<int16>>())[0].associatedTypes[0].type.name=="int16");assert(($interfaces<Holder<String>>())[0].associatedTypes[0].type.name=="std::core::String");}''',None),
        ('early_identity', '''interface Source{type Item;Item get();}
$macro SNAP<T>(name:ident) -> member
$compile_assert(($interfaces<T>())[0].associatedTypes[0].type.name=="int64");
uint64 name(){return ($interfaces<T>())[0].id;}
$end_macro
struct Cell:Source{type Item=int64;int64 get(){return 7;}$SNAP<Cell>(snapshot);}
void main(){Cell value;assert(value.snapshot()==($interfaces<Cell>())[0].id);}''',None),
        ('inherited_receiver', '''interface Base{int64 read(){return 7;}}
struct Cell:Base{}
void main(){assert(($methods<Cell>())[0].isInherited);assert(($methods<Cell>())[0].declaringType=="program::Base");assert(($methods<Cell>())[0].receiverType.name=="Cell");}''',None),
        ('early_method_identity', '''interface Source{type Item;Item get();}
$macro SNAP<T>(name:ident) -> member
uint64 name(){return ($methods<T>())[0].id;}
$end_macro
struct Cell:Source{type Item=int64;int64 get(){return 7;}$SNAP<Cell>(snapshot);}
void main(){Cell value;assert(value.snapshot()==($methods<Cell>())[0].id);}''',None),
    ]
    if args.case:
        cases=[case for case in cases if case[0] in args.case]
        assert len(cases)==len(set(args.case))
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-member-contract-') as directory:
        work=Path(directory)
        for name,source,diagnostics in cases:
            path=work/(name+'.chl')
            path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(args.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=result.returncode!=0 and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
    assert not failures,failures
    print(f'方法与关联类型元数据通过：{len(cases)} 项')


if __name__=='__main__':
    main()
