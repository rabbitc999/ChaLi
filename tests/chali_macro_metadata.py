"""验证字段元数据的精确类型、声明顺序、注解、卫生绑定和编译期边界。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """通过真实编译器执行标量查询，并拒绝元数据逃逸及非法下标。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[], help='仅运行指定用例，可重复提供')
    options = parser.parse_args()
    definitions = '''
struct Empty{}
struct Inner{int32 code;}
struct Record{int64 last;static int64 skipped;Inner nested;uint8 first;float32 ratio;int16 grid[2][3];}
struct Holder<T>{T value;T extra;}
'''
    cases = [('fields', definitions + '''
void main(){
assert($fields<Empty>().count==0);
assert($fields<Record>().count==5);
assert(($fields<Record>())[0].name=="last" && ($fields<Record>())[0].index==0);
assert(($fields<Record>())[4].name=="grid" && ($fields<Record>())[4].index==4);
assert(($fields<Record>())[2].type.name=="uint8");
assert(($fields<Record>())[3].type.name=="float32");
assert(($fields<Record>())[4].type.name=="int16[2][3]");
assert(($fields<Record>())[1].type.fields.count==1);
assert(($fields<Record>())[1].type.fields[0].name=="code");
assert(($fields<Record>())[1].type.fields[0].type.name=="int32");
assert(($fields<Record>())[0].attributes.count==0);
assert($fields<Record>().id==$fields<Record>().id);
assert(($fields<Record>())[0].id!=($fields<Record>())[1].id);
assert(($fields<Record>())[0].type.id!=($fields<Record>())[2].type.id);
assert(($fields<Record>())[$fields<Record>().count-1].name=="grid");
}
''', None, False),
('basics', """
struct Basics{int8 a;uint8 b;int16 c;uint16 d;int32 e;uint32 f;int64 g;uint64 h;float32 i;float64 j;bool k;char l;String m;List<int64> n;Map<String,int64> o;Set<uint32> p;}
struct Other{List<int32> n;Map<String,int16> o;Set<uint64> p;}
void main(){
assert($fields<Basics>().count==16);
""" + ''.join(f'assert(($fields<Basics>())[{index}].type.name=="{name}");\n' for index,name in enumerate(['int8','uint8','int16','uint16','int32','uint32','int64','uint64','float32','float64','bool','char','std::lang::String'])) + """
assert(($fields<Basics>())[13].type.id!=($fields<Other>())[0].type.id);
assert(($fields<Basics>())[14].type.id!=($fields<Other>())[1].type.id);
assert(($fields<Basics>())[15].type.id!=($fields<Other>())[2].type.id);
}
""", None, False),
('generic', definitions + '''
void main(){
assert($fields<Holder<int16>>().count==2);
assert(($fields<Holder<int16>>())[0].type.name=="int16");
assert(($fields<Holder<String>>())[0].type.name=="std::lang::String");
assert(($fields<Holder<Inner>>())[0].type.fields[0].name=="code");
assert($fields<Holder<int16>>().id!=$fields<Holder<int32>>().id);
}
''', None, False),
('macros', definitions + '''
$macro NAME(T:type) -> expr
($fields<T>())[0].name
$end_macro
$macro TYPE<T>() -> expr
($fields<T>())[0].type.name
$end_macro
void main(){assert($NAME(Record)=="last");assert($TYPE<Holder<int8>>()=="int8");}
''', None, False),
('attributes', '''
@annotation Label(text:String,limit:int16=(int16)7,active:bool=true,ratio:float32=(float32)1.5)
struct Item{@Label("hello") int64 value;}
void main(){
assert(($fields<Item>())[0].attributes.count==1);
assert(($fields<Item>())[0].attributes[0].name=="program::Label");
assert(($fields<Item>())[0].attributes[0].arguments.count==4);
assert(($fields<Item>())[0].attributes[0].arguments[0].name=="text");
assert(($fields<Item>())[0].attributes[0].arguments[0].value=="hello");
assert(($fields<Item>())[0].attributes[0].arguments[1].type.name=="int16");
assert(($fields<Item>())[0].attributes[0].arguments[1].value==(int16)7);
assert(($fields<Item>())[0].attributes[0].arguments[2].value);
assert(($fields<Item>())[0].attributes[0].arguments[3].value==(float32)1.5);
}
''', None, True),
('global', definitions + 'const int64 count=$fields<Record>().count;const uint64 identity=$fields<Record>().id;void main(){assert(count==5 && identity==$fields<Record>().id);}', None, False),
('shadow', 'const int64 SIZE=2;struct Sized{int32 items[SIZE];}void main(){int64 SIZE=8;assert(($fields<Sized>())[0].type.name=="int32[2]");}', None, False),
('modules', '''import left;
import right;
void main(){assert(($fields<left::Item>())[0].name=="leftValue");assert(($fields<right::Item>())[0].name=="rightValue");assert($fields<left::Item>().id!=$fields<right::Item>().id);}
''', None, False),
]
    for name, expression, diagnostic in [
        ('escape_range','$fields<Record>()','编译期元数据不能作为运行期值'),
        ('escape_field','($fields<Record>())[0]','编译期元数据不能作为运行期值'),
        ('escape_type','($fields<Record>())[0].type','编译期元数据不能作为运行期值'),
        ('negative','($fields<Record>())[-1].name','非负整数常量'),
        ('bounds','($fields<Record>())[5].name','字段索引越界'),
        ('dynamic','($fields<Record>())[next()].name','常量表达式'),
        ('invalid_type','$fields<int64>().count','只支持'),
        ('attribute_bounds','($fields<Record>())[0].attributes[0].name','注解索引越界'),
        ('unknown_property','($fields<Record>())[0].offset','没有此编译期属性'),
    ]:
        cases.append((name,definitions+'int64 next(){return 0;}void main(){let value='+expression+';}',diagnostic,False))
    cases.append(('dependency_cycle','struct Bad{int64 items[$fields<Bad>().count];}void main(){}','循环依赖',False))
    declarations=['struct First{int64 value;}', 'struct Second{First nested;}']
    identity_body='void main(){println("{} {} {}",$fields<First>().id,($fields<First>())[0].id,($fields<First>())[0].type.id);}'
    cases += [('identity_first', '\n'.join(declarations)+identity_body,None,False),
              ('identity_reordered', '\n'.join(reversed(declarations))+identity_body,None,False)]
    if options.case:
        cases=[case for case in cases if case[0] in options.case]
        assert len(cases)==len(set(options.case)), options.case
    failures=[]
    identities={}
    with tempfile.TemporaryDirectory(prefix='macro-metadata-') as directory:
        work=Path(directory)
        for module,field in [('left','leftValue'),('right','rightValue')]:
            (work/f'{module}.chl').write_text(f'module {module};\nstruct Item{{int64 {field};}}\n',encoding='utf-8')
        for name,source,diagnostic,reflection in cases:
            path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostic else 'run',str(path),'--out',str(work/name),*(['--reflection'] if reflection else [])],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=(result.returncode!=0 and diagnostic in result.stderr) if diagnostic else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if name.startswith('identity_'):
                valid=result.returncode==0 and result.stderr=='' and len(result.stdout.split())==3
                identities[name]=result.stdout
            if not valid: failures.append((name,result))
    if len(identities)==2:
        assert identities['identity_first']==identities['identity_reordered'],identities
    assert not failures,failures
    print(f'编译期字段元数据通过：{len(cases)} 项')


if __name__=='__main__':
    main()
