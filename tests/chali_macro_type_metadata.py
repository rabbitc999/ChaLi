"""验证枚举变体／载荷的编译期描述符、静态遍历与非法来源。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行真实编译器，核对身份、精确类型、嵌套遍历及拒绝诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    args = parser.parse_args()
    declarations = '''enum Choice{Empty,One(int8 value),Pair(String text,uint64 number)};
enum Box<T>{Value(T item),None};
struct Item{int32 score;};
'''
    cases = [
        ('variants', declarations + '''void main(){
assert($variants<Choice>().count==3);
assert(($variants<Choice>())[0].name=="Empty");
assert(($variants<Choice>())[1].index==1);
assert(($variants<Choice>())[0].id!=($variants<Choice>())[1].id);
assert(($variants<Choice>())[0].payloads.count==0);
assert(($variants<Choice>())[2].payloads.count==2);
assert(($variants<Choice>())[2].payloads[0].name=="text");
assert(($variants<Choice>())[2].payloads[1].index==1);
assert(($variants<Choice>())[1].payloads[0].type.name=="int8");
assert(($variants<Choice>())[2].payloads[1].type.name=="uint64");
assert(($variants<Choice>())[2].payloads[0].id!=($variants<Choice>())[2].payloads[1].id);
}''', None),
        ('generic', declarations + '''void main(){
assert(($variants<Box<int16>>())[0].payloads[0].type.name=="int16");
assert(($variants<Box<String>>())[0].payloads[0].type.name=="std::core::String");
assert(($variants<Box<Item>>())[0].payloads[0].type.fields[0].name=="score");
assert(($variants<Box<int16>>())[0].id!=($variants<Box<int32>>())[0].id);
}''', None),
        ('nested_types', declarations + '''enum Nested{Wrap(Choice value)};
void main(){assert(($variants<Nested>())[0].payloads[0].type.variants.count==3);}''', None),
        ('qualified', declarations + 'void main(){assert($std::core::variants<Choice>().count==3);}', None),
        ('loop', declarations + '''
$macro COUNT<T>() -> expr
int64 total=0;
$for variant in $variants<T>()
$for payload in variant.payloads
total+=1;
$end_for
$end_for
total
$end_macro
void main(){assert($COUNT<Choice>()==3);assert($COUNT<Box<int64>>()==1);}
''', None),
        ('conditional', declarations + '''
$macro COUNT<T>() -> expr
int64 total=0;
$for variant in $variants<T>()
$if variant.payloads.count==0
total+=10;
$else
total+=1;
$end_if
$end_for
total
$end_macro
void main(){assert($COUNT<Choice>()==12);}
''', None),
        ('diagnostic', declarations + '''
$macro CHECK<T>() -> stmt
$for variant in $variants<T>()
$for payload in variant.payloads
$compile_assert(payload.type.name=="int8","unsupported "+payload.type.name);
$end_for
$end_for
$end_macro
void main(){$CHECK<Choice>();}
''', ['unsupported', '载荷', 'text']),
        ('struct_rejected', 'struct Data{int64 value;};void main(){$variants<Data>().count;}', ['枚举']),
        ('scalar_rejected', 'void main(){$variants<int64>().count;}', ['枚举']),
        ('variant_bounds', declarations + 'void main(){($variants<Choice>())[3].name;}', ['变体索引越界']),
        ('payload_bounds', declarations + 'void main(){($variants<Choice>())[0].payloads[0].name;}', ['载荷索引越界']),
        ('negative_index', declarations + 'void main(){($variants<Choice>())[-1].name;}', ['非负整数常量']),
        ('runtime_index', declarations + 'void main(){int64 i=0;($variants<Choice>())[i].name;}', ['常量']),
        ('escape', declarations + 'void main(){let descriptor=$variants<Choice>();}', ['编译期元数据不能作为运行期值']),
        ('payload_not_field', declarations + '$macro BAD(v:value) -> stmt\n$member(v,($variants<Choice>())[1].payloads[0]);\n$end_macro\nvoid main(){Choice v=Choice::One((int8)1);$BAD(v);}', ['字段描述符']),
        ('value_arguments', declarations + 'void main(){$variants<Choice>(1).count;}', ['不接受值实参']),
        ('missing_type', declarations + 'void main(){$variants().count;}', ['显式类型实参']),
        ('reserved_variants', '$macro variants()\n1\n$end_macro\nvoid main(){}', ['内建名称']),
        ('reserved_methods', '$macro methods()\n1\n$end_macro\nvoid main(){}', ['内建名称']),
        ('reserved_interfaces', '$macro interfaces()\n1\n$end_macro\nvoid main(){}', ['内建名称']),
    ]
    member_declarations = '''
interface Root {int64 base(){return 3;}}
interface Left:Root {int64 left();}
interface Right:Root {int64 right();}
struct Reader:Left,Right {
int64 left(){return 1;}
int64 right(){return 2;}
}
struct Score {
int64 value;
static Score create(int64 value){self.value=value;return self;}
int64 read(){return self.value;}
int64 add(int16 step=(int16)1){return self.value+cast<int64>(step);}
}
'''
    cases.extend([
        ('type_fragments', declarations + '''
$macro COUNT(T:type) -> expr
int64 total=0;
$for variant in $variants<T>()
total+=1;
$end_for
total
$end_macro
$macro NAME(T:type) -> expr
($variants<T>())[0].name
$end_macro
void main(){assert($COUNT(Choice)==3);assert($NAME(Choice)=="Empty");}
''', None),
        ('field_type_fragments', 'struct Data{int64 a;int32 b;}\n$macro COUNT(T:type) -> expr\nint64 total=0;\n$for field in $fields<T>()\ntotal+=1;\n$end_for\ntotal\n$end_macro\nvoid main(){assert($COUNT(Data)==2);}', None),
        ('methods', member_declarations + '''void main(){
assert($methods<Score>().count==3);
assert(($methods<Score>())[0].name=="create");
assert(($methods<Score>())[0].isStatic && ($methods<Score>())[0].isConstructor);
assert(($methods<Score>())[0].hasBody);
assert(($methods<Score>())[0].parameters[0].name=="value");
assert(($methods<Score>())[0].parameters[0].type.name=="int64");
assert(($methods<Score>())[1].name=="read" && !($methods<Score>())[1].isStatic);
assert(($methods<Score>())[1].parameters.count==0);
assert(($methods<Score>())[1].result.name=="int64");
assert(($methods<Score>())[2].parameters[0].hasDefault);
assert(($methods<Score>())[0].id!=($methods<Score>())[1].id);
}''', None),
        ('interfaces', member_declarations + '''void main(){
assert($interfaces<Reader>().count==3);
assert(($interfaces<Reader>())[0].name=="program::Left");
assert(($interfaces<Reader>())[0].isDirect);
assert(($interfaces<Reader>())[1].name=="program::Root");
assert(!($interfaces<Reader>())[1].isDirect);
assert(($interfaces<Reader>())[2].name=="program::Right");
assert(($interfaces<Reader>())[2].isDirect);
assert($interfaces<Root>().count==0);
}''', None),
        ('interface_methods', member_declarations + '''void main(){
assert($methods<Left>().count==2);
assert(($methods<Left>())[0].name=="left" && !($methods<Left>())[0].hasBody);
assert(($methods<Left>())[1].name=="base" && ($methods<Left>())[1].hasBody);
assert(($methods<Left>())[1].isInherited);
assert($methods<Reader>().count==3);
assert(($methods<Reader>())[2].isInherited);
}''', None),
        ('overloaded', '''struct Data{int64 read(int8 x){return cast<int64>(x);} int64 read(int16 x){return cast<int64>(x);}}
void main(){assert($methods<Data>().count==2);assert(($methods<Data>())[0].id!=($methods<Data>())[1].id);assert(($methods<Data>())[0].parameters[0].type.name=="int8");}''', None),
        ('generic_methods', '''struct Box<T>{T value;T read(){return self.value;}}
void main(){assert(($methods<Box<int16>>())[0].result.name=="int16");assert(($methods<Box<String>>())[0].result.name=="std::core::String");}''', None),
        ('generic_interfaces', '''interface Read<T>{T get();}struct Box<T>:Read<T>{T value;T get(){return self.value;}}
void main(){assert($interfaces<Box<int16>>().count==1);assert(($interfaces<Box<int16>>())[0].type.methods[0].result.name=="int16");}''', None),
        ('method_loop', member_declarations + '''
$macro COUNT<T>() -> expr
int64 total=0;
$for method in $methods<T>()
$if method.isStatic
total+=10;
$else
total+=1;
$end_if
$for parameter in method.parameters
total+=1;
$end_for
$end_for
total
$end_macro
void main(){assert($COUNT<Score>()==14);}
''', None),
        ('interface_loop', member_declarations + '''
$macro COUNT<T>() -> expr
int64 total=0;
$for contract in $interfaces<T>()
$if contract.isDirect
total+=1;
$end_if
$end_for
total
$end_macro
void main(){assert($COUNT<Reader>()==2);}
''', None),
        ('method_bounds', member_declarations + 'void main(){($methods<Score>())[3].name;}', ['方法索引越界']),
        ('parameter_bounds', member_declarations + 'void main(){($methods<Score>())[1].parameters[0].name;}', ['参数索引越界']),
        ('interface_bounds', member_declarations + 'void main(){($interfaces<Root>())[0].name;}', ['接口索引越界']),
        ('method_runtime', member_declarations + 'void main(){let value=$methods<Score>();}', ['编译期元数据不能作为运行期值']),
        ('interface_runtime', member_declarations + 'void main(){let value=$interfaces<Reader>();}', ['编译期元数据不能作为运行期值']),
        ('method_scalar', 'void main(){$methods<int32>().count;}', ['类型成员元数据']),
        ('global_methods', member_declarations + 'const int64 count=$methods<Score>().count;void main(){assert(count==3);}', None),
        ('global_inheritance', member_declarations + 'const int64 count=$methods<Reader>().count;void main(){assert(count==3);}', None),
        ('variadic_method', 'struct Data{int64 sum(int64... values){return 0;}}void main(){assert(($methods<Data>())[0].parameters[0].isVariadic);}', None),
        ('cross_modules', '''import left;
import right;
void main(){assert(($variants<left::Choice>())[0].id!=($variants<right::Choice>())[0].id);assert(($methods<left::Reader>())[0].id!=($methods<right::Reader>())[0].id);}
''', None),
    ])
    identity_declarations = 'struct Box<T>{T value;T read(){return self.value;}}\n'
    identity_main = 'void main(){println("{} {}",($methods<Box<int64>>())[0].id,($methods<Box<int64>>())[0].parameters.count);}'
    cases.extend([
        ('identity_first', identity_declarations+'const uint64 a=($methods<Box<int64>>())[0].id;const uint64 b=($methods<Box<int16>>())[0].id;'+identity_main, None),
        ('identity_reordered', identity_declarations+'const uint64 b=($methods<Box<int16>>())[0].id;const uint64 a=($methods<Box<int64>>())[0].id;'+identity_main, None),
    ])
    if args.case:
        cases = [item for item in cases if item[0] in args.case]
        assert len(cases) == len(set(args.case)), args.case
    failures = []
    identities = {}
    with tempfile.TemporaryDirectory(prefix='macro-type-metadata-') as directory:
        work = Path(directory)
        for module in ['left', 'right']:
            (work/(module+'.chl')).write_text(f'module {module};\nenum Choice{{Same}};\nstruct Reader{{int64 read(){{return 1;}}}};\n', encoding='utf-8')
        for name, source, diagnostic in cases:
            path = work / (name + '.chl')
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(args.chali.resolve()), 'check' if diagnostic else 'run', str(path), '--out', str(work/name)],
                cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=240)
            valid = (result.returncode != 0 and all(part in result.stderr for part in diagnostic)) if diagnostic else (result.returncode, result.stdout, result.stderr) == (0, '', '')
            if name.startswith('identity_'):
                valid = result.returncode == 0 and result.stderr == '' and len(result.stdout.split()) == 2
                identities[name] = result.stdout
            if not valid:
                failures.append((name, result.returncode, result.stdout, result.stderr))
    if len(identities) == 2:
        assert identities['identity_first'] == identities['identity_reordered'], identities
    assert not failures, failures
    print(f'类型编译期元数据通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
