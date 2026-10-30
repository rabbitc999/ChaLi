"""验证类型成员宏的接收者、卫生名称、泛型与冲突边界。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """以原生运行与类型拒绝验证成员生成，保留可单独复验的案例名。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    options = parser.parse_args()
    getter = '''$macro GET(name:ident, field:ident) -> member
int64 name(){return self.field;}
$end_macro
'''
    cases = [
        ('getter', getter + 'struct Cell{int64 value;$GET(read,value);}void main(){Cell c;c.value=7;assert(c.read()==7);}', None),
        ('private', '''$macro METHODS(name:ident) -> member
int64 helper(){return 42;}
int64 name(){return self.helper();}
$end_macro
struct Cell{$METHODS(first);$METHODS(second);int64 helper(){return 7;}}
void main(){Cell c;assert(c.first()==42&&c.second()==42&&c.helper()==7);}
''', None),
        ('field', '''$macro CELL(field:ident, get:ident) -> member
int64 field;
int64 get(){return self.field;}
$end_macro
struct Cell{$CELL(value,read);}void main(){Cell c;c.value=3;assert(c.read()==3);}
''', None),
        ('generic', '''$macro VALUE<T>(get:ident) -> member
T get(){return self.value;}
$end_macro
struct Cell<T>{T value;$VALUE<T>(read);}
void main(){Cell<int64> c;c.value=7;assert(c.read()==7);Cell<String> s;s.value="hello";assert(s.read()=="hello");}
''', None),
        ('nested', getter + '''$macro OUT(name:ident, field:ident) -> member
$GET(name,field);
$end_macro
struct Cell{int64 value;$OUT(read,value);}void main(){Cell c;c.value=9;assert(c.read()==9);}
''', None),
        ('interface', '''$macro API(name:ident) -> member
int64 name();
$end_macro
interface Read{$API(read);}struct Cell:Read{int64 read(){return 7;}}
void main(){Cell c;Read r=c;assert(r.read()==7);}
''', None),
        ('interface_default', '''$macro API(name:ident) -> member
int64 name(){return 7;}
$end_macro
interface Read{$API(read);}struct Cell:Read{}
void main(){Cell c;Read r=c;assert(r.read()==7);}
''', None),
        ('conflict', getter + 'struct Cell{int64 value;$GET(read,value);int64 read(){return 0;}}void main(){}', ['冲突']),
        ('field_conflict', '''$macro FIELD(name:ident) -> member
int64 name;
$end_macro
struct Cell{int64 value;$FIELD(value);}void main(){}
''', ['冲突']),
        ('interface_field', '''$macro FIELD(name:ident) -> member
int64 name;
$end_macro
interface Cell{$FIELD(value);}void main(){}
''', ['接口不能']),
        ('cycle', '''$macro CYCLE() -> member
$CYCLE();
$end_macro
struct Cell{$CYCLE();}void main(){}
''', ['循环依赖']),
        ('wrong_result', '''$macro EXPR() -> expr
7
$end_macro
struct Cell{$EXPR();}void main(){}
''', ['member']),
        ('runtime_parameter', '''$macro BAD(n:value) -> member
int64 value;
$end_macro
struct Cell{$BAD(1);}void main(){}
''', ['语法类别']),
        ('prototype', '''$macro API(name:ident) -> member
int64 name();
$end_macro
struct Cell{$API(read);}void main(){}
''', ['方法体']),
    ]
    cases.extend([
        ('private_static', '''$macro COUNT<T>(name:ident) -> member
static int64 counter;
int64 name(){T::counter+=1;return T::counter;}
$end_macro
struct Cell{$COUNT<Cell>(first);$COUNT<Cell>(second);}
void main(){Cell c;assert(c.first()==1&&c.first()==2&&c.second()==1);}
''', None),
        ('owning_field_borrow', '''$macro OWN(field:ident, set:ident, read:ident) -> member
String field;
void set(String value){self.field=value;}
String const& read(){return &self.field;}
$end_macro
struct Cell{$OWN(text,set,read);}
void main(){Cell c;c.set("hello");String const& text=c.read();assert(text=="hello");}
''', None),
        ('deferred_generic', '''$macro VALUE<T>(name:ident) -> member
T name(){return self.value;}
$end_macro
struct Cell<T>{T value;$VALUE<T>(read);}
Cell<T> make<T>(T value){Cell<T> result;result.value=value;return result;}
void main(){let c=make(7);assert(c.read()==7);let s=make("text");assert(s.read()=="text");}
''', None),
        ('inherited_generic_interface', '''$macro API<T>(name:ident) -> member
T name();
$end_macro
interface Base<T>{$API<T>(read);}interface Read:Base<int64>{}
struct Cell:Read{int64 read(){return 7;}}
void main(){Cell c;Read r=c;assert(r.read()==7);}
''', None),
        ('constructor', '''$macro CTOR(name:ident) -> member
name(int64 value){self.value=value;}
$end_macro
struct Cell{int64 value;$CTOR(Cell);}void main(){Cell c=new Cell(7);assert(c.value==7);}
''', None),
        ('member_outside', getter + 'void main(){$GET(read,value);}', ['成员宏只能']),
        ('imported', 'import member_tools;\nstruct Cell{$member_tools::METHOD(read);}int64 helper(){return 99;}void main(){Cell c;assert(c.read()==7);}', None),
        ('generic_missing', '''$macro VALUE<T>(name:ident) -> member
T name(){return self.value;}
$end_macro
struct Cell{int64 value;$VALUE(read);}void main(){}
''', ['无法推导']),
        ('reserved_name', getter + 'struct Cell{int64 value;$GET(__chali_bad,value);}void main(){}', ['__chali_']),
        ('factory', '''$macro FACTORY<T>(name:ident) -> member
static T name(int64 value){self.value=value;return self;}
$end_macro
struct Cell{int64 value;$FACTORY<Cell>(create);}
void main(){Cell c=Cell::create(7);assert(c.value==7);}
''', None),
        ('static_receiver_rejected', '''$macro BAD(name:ident) -> member
static void name(){self.value=7;}
$end_macro
struct Cell{int64 value;$BAD(reset);}void main(){Cell::reset();}
''', ['只能用在 struct']),
        ('field_methods', '''$macro TOTAL<T>(name:ident) -> member
int64 name(){int64 total=0;
$for field in $fields<T>()
total+=$member(self,field);
$end_for
return total;}
$end_macro
struct Cell{int64 a;int64 b;$TOTAL<Cell>(total);}
void main(){Cell c;c.a=3;c.b=4;assert(c.total()==7);}
''', None),
        ('field_cycle', '''$macro BAD<T>(name:ident) -> member
$if $fields<T>().count==0
int64 name;
$end_if
$end_macro
struct Cell{$BAD<Cell>(value);}void main(){}
''', ['循环依赖']),
        ('conditional', '''$macro METHODS(name:ident) -> member
$if true
int64 name(){return 7;}
$else
Unknown invalid;
$end_if
$end_macro
struct Cell{$METHODS(read);}void main(){Cell c;assert(c.read()==7);}
''', None),
        ('associated', '''$macro ITEM(name:ident, get:ident) -> member
type name=int64;
name get(){return 7;}
$end_macro
struct Cell{$ITEM(Item,read);}void main(){Cell c;assert(c.read()==7);}
''', None),
        ('overload', '''$macro METHODS(name:ident) -> member
int64 name(int64 value){return value;}
int64 name(bool value){if(value){return 7;}return 0;}
$end_macro
struct Cell{$METHODS(read);}void main(){Cell c;assert(c.read(3)==3&&c.read(true)==7);}
''', None),
    ])
    if options.case:
        cases = [case for case in cases if case[0] in options.case]
    failures = []
    with tempfile.TemporaryDirectory(prefix='macro-members-') as directory:
        work = Path(directory)
        (work / 'member_tools.chl').write_text('module member_tools;\nint64 helper(){return 7;}\n$macro METHOD(name:ident) -> member\nint64 name(){return helper();}\n$end_macro\n', encoding='utf-8')
        for name, source, diagnostics in cases:
            path = work / f'{name}.chl'
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(options.chali.resolve()), 'check' if diagnostics else 'run', str(path), '--out', str(work / name)], cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=240)
            valid = result.returncode != 0 and result.stdout == '' and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode, result.stdout, result.stderr) == (0, '', '')
            if not valid:
                failures.append((name, result.returncode, result.stdout, result.stderr))
    assert not failures, failures
    print(f'成员宏通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
