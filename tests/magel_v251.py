"""0.17.1 语法与语义回归：实际解释执行、AOT、退出清理与编译期诊断。"""
from pathlib import Path
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMPILER = Path(os.environ.get('MAGEL_TEST_COMPILER', str(ROOT / ('magel.exe' if os.name == 'nt' else 'magel'))))
CASES = {
    'match': r'''
struct Payload { int64 value; String text; };
enum Packet { Data(Payload value), Empty };
int64 side=0;
int32 fallback(){side=side+1;return 0;}
int64 main(){
 Payload p;p.value=4;p.text="original";Packet packet=Packet::Data(p);
 int64 value=match(packet){Packet::Data(copy)=>{copy.value=7;copy.text="changed";break copy.value;}Packet::Empty=>{0}};
 assert(value==7);
 match(packet){Packet::Data(copy)=>{assert(copy.value==4);assert(copy.text=="original");}Packet::Empty=>{panic("wrong branch");}};
 int32 small=match(3){3=>{7}_=>{fallback()}};assert(small==7);assert(side==0);
 small=10+(match(4){3=>{fallback()}_=>{2}});assert(small==12);
 String text=match(packet){Packet::Data(copy)=>{copy.text+"!"}Packet::Empty=>{"none"}};
 assert(text=="original!");
 small=match(4){3 if fallback()==0=>{1}[4,6)=>{int32 local=9;break local;}_=>{0}};assert(small==9);assert(side==0);
 println("match ok");return 0;
}''',
    'interpolation': r'''
$replace ECHO(value) $stringify(value)
$replace f "prefix"
int64 counter=0;
int64 next(){counter=counter+1;return counter;}
int64 main(){
 int64 f=7;String ordinary="{f}";assert(ordinary=="{f}");
 String text=f"value {f+1} {next()} {next()} \{literal\} {f"inner {3}"}";
 assert(text=="value 8 1 2 {literal} inner 3");assert(counter==2);
 text=f"{if(false){next()}else{4}} {match(f){7=>{5}_=>{6}}}";assert(text=="4 5");assert(counter==2);
 text=f"{format("{}", f)} {"quote"}";assert(text=="7 quote");
 assert($ECHO(f"hello {3}")=="f\"hello {3}\"");assert($stringify(f"hello {3}")=="f\"hello {3}\"");
 String plain=$f"suffix";assert(plain=="prefixsuffix");
 String multiline=f"first
second";assert(multiline=="first\nsecond");
 println("interpolation ok");return 0;
}''',
    'associated': r'''
interface Container { type Item; Item get(int64 index); Item first(){return self.get(0);} };
interface Derived : Container { };
struct Numbers : Derived { type Item=int64; Item get(int64 index){return index+11;} };
struct Words : Container { type Item=String; Item get(int64 index){return "word";} };
C.Item first<C:Container>(C value){return value.get(0);}
C.Item second<C:Container>(C value){C.Item item=value.get(1);return item;}
int64 main(){Numbers values;Words words;int64 n=first<Numbers>(values);String s=first<Words>(words);
 assert(n==11);assert(s=="word");assert(second<Numbers>(values)==12);assert(values.first()==11);
 println("associated ok");return 0;}
''',
    'defer': r'''
int64 sequence=0;
void add(int64 n){sequence=sequence*10+n;}
struct Owned : Drop { void drop(){add(9);} };
struct Receiver { int64 value; void finish(){defer self.value=self.value+1;scope(exit){value=value+2;}} };
int64 run(){int64 n=1;defer add(n);n=3;scope(exit){add(2);}return 7;}
String snapshot(){String text="before";defer text="after";return text;}
Option<int64> fail(){defer add(6);Option<int64> missing=Option<int64>::None;auto value=missing?;return Option<int64>::Value(value);}
int64 main(){assert(run()==7);assert(sequence==23);int64 c=0;
 while(c<3){defer add(4);c=c+1;if(c<3){continue;}break;}assert(sequence==23444);
 {Owned owner;defer add(5);}assert(sequence==2344459);
 auto result=fail();assert(result.isNone());assert(sequence==23444596);
 const int64 n=8;{defer add(n);}assert(sequence==234445968);
 Receiver receiver;receiver.finish();assert(receiver.value==3);
 assert(snapshot()=="before");
 String matching=match(1){1=>{String text="before";defer text="after";break text;}_=>{"unused"}};
 String conditional=if(true){String text="first";defer text="after";break text;}else{"unused"};
 String looping=loop{String text="loop";defer text="after";break text;};
 assert(matching=="before");assert(conditional=="first");assert(looping=="loop");
 {int64 mut& memory=new;defer delete memory;memory=4;}
 println("defer ok");return 0;}
''',
    'macros': r'''
$macro expr SUM(n)
$if n == 0
0
$elif n > 0
n + $SUM((n)-1)
$else
0
$end_if
$end_macro
$macro expr SELECT(a, ...rest)
$let total = $argc
$if total == 1
a
$elif $count(rest) == 1
$typecheck(int64, a + rest)
$else
0
$end_if
$end_macro
$macro expr STYLE(T)
$if $stringify(T) == "int64"
4
$else
8
$end_if
$end_macro
$macro POSITIVE(...rest)
$if false
$for item in missing
invalid;
$end_for
$else
$for item in rest
$if item > 0
total=total+item;
$end_if
$end_for
$end_if
$end_macro
int64 main(){int64 sum=$SUM(5);assert(sum==15);assert($SELECT(3)==3);assert($SELECT(3,4)==7);
 assert($SELECT(1,2,3)==0);assert($count(a,b)==2);assert($STYLE(int64)==4);assert($STYLE(String)==8);
 int64 total=0;$POSITIVE(1,-2,3) assert(total==4);println("macros ok");return 0;}
''',
}

CASES['defer_coroutine'] = r'''
import std::coroutine;
int64 completed=0;
void first(){defer completed=completed+1;std::coroutine::yield();}
void second(){defer completed=completed+2;std::coroutine::yield();std::coroutine::yield();}
int64 main(){
 auto a=std::coroutine::create(first);auto b=std::coroutine::create(second);
 std::coroutine::wait(a);std::coroutine::wait(b);assert(completed==3);
 println("coroutine defer ok");return 0;
}
'''

CASES['const_generics'] = r'''
struct Buffer<T,N:int64> { T data[N]; int64 len(){return N;} };
struct Matrix<T,rows:int64,cols:int64> { T data[rows][cols]; int64 size(){return rows*cols;} };
struct DefaultBuffer<T,N:int64=2> { T data[N];int64 len(){return N;} };
struct Defaults<A:int64=2,B:int64=A*3> { int64 len(){return B;} };
int64 length<T,N:int64>(Buffer<T,N> value){return N;}
int64 dropped=0;
struct OwnedElement : Drop { String value;void drop(){dropped=dropped+1;} };
const int64 size=2+2;
int64 main(){Buffer<int64,4> a;a.data[0]=3;a.data[3]=7;
 Buffer<int64,2+2> copy=a;copy.data[0]=9;assert(a.data[0]==3);assert(copy.data[0]==9);
 assert(a.len()==4);assert(length<int64,4>(a)==4);assert(length(a)==4);
 Buffer<int64,5> different;assert(different.len()==5);
 Buffer<int64,size> named;assert(named.len()==4);
 Defaults<2+2> derived;assert(derived.len()==12);assert(length(a)==4);
 Matrix<int64,2,3> matrix;matrix.data[1][2]=8;assert(matrix.data[1][2]==8);assert(matrix.size()==6);
 DefaultBuffer<String> texts;texts.data[0]="zero";texts.data[1]="one";auto other=texts;other.data[0]="changed";
 assert(texts.data[0]=="zero");assert(other.data[0]=="changed");
 {Buffer<OwnedElement,2> entries;entries.data[0].value="original";
  auto cloned=entries;cloned.data[0].value="copy";assert(entries.data[0].value=="original");}
 assert(dropped==4);
 println("const generics ok");return 0;}
'''

ERRORS = {
    'match_type': ('int64 main(){auto y=match(1){1=>{3}_=>{"text"}};return 0;}', '类型'),
    'match_missing_value': ('int64 main(){int64 y=match(1){1=>{3;}_=>{0}};return 0;}', '产生结果'),
    'match_incomplete': ('enum E{A,B};int64 main(){E e=E::A;auto y=match(e){E::A=>{1}};return 0;}', '覆盖所有'),
    'match_missing_default': ('int64 main(){auto y=match(1){1=>{3}};return 0;}', '兜底'),
    'interpolation_empty': ('int64 main(){String s=f"{}";return 0;}', '表达式'),
    'interpolation_unclosed': ('int64 main(){String s=f"{1";return 0;}', '缺少'),
    'associated_missing': ('interface C{type Item;Item get();};struct X:C{int64 get(){return 1;}};int64 main(){return 0;}', '关联类型'),
    'associated_signature': ('interface C{type Item;Item get();};struct X:C{type Item=int64;String get(){return "bad";}};int64 main(){return 0;}', '完全实现'),
    'associated_cycle': ('interface C{type Item;};struct X:C{type Item=Item;};int64 main(){return 0;}', '循环'),
    'scope_mode': ('int64 main(){scope(success){}return 0;}', 'scope(exit)'),
    'defer_readonly_receiver': ('struct X{int64 value;void update(){defer self.value=1;}};int64 main(){X x;X const& r=&x;r.update();return 0;}', '只读'),
    'macro_count': ('$replace ADD(a,b) a+b\nint64 main(){return $ADD(1);}', '实参'),
    'macro_recursion': ('$replace RECURSE(n) $RECURSE(n)\nint64 main(){return $RECURSE(1);}', '64 层'),
    'macro_type': ('int64 main(){auto x=$typecheck(String,1);return 0;}', '类型'),
    'macro_pair': ('$macro BAD()\n$if true\n1\n$end_macro\nint64 main(){return 0;}', '$end_if'),
    'const_runtime': ('struct Buffer<T,N:int64>{T data[N];};int64 main(){int64 n=4;Buffer<int64,n> value;return 0;}', '编译期常量'),
    'const_type': ('struct Buffer<T,N:int64>{T data[N];};int64 main(){Buffer<int64,String> value;return 0;}', '编译期常量'),
    'const_size': ('struct Buffer<T,N:int64>{T data[N];};int64 main(){Buffer<int64,0> value;return 0;}', '正整数'),
    'const_overflow': ('struct Value<N:int64>{int64 get(){return N;}};int64 main(){Value<9223372036854775808> value;return 0;}', 'int64 范围'),
}


def main():
    """对两个后端比较结果，并检查不能接受的语法与类型。"""
    env = dict(os.environ, MAGEL_LANG='zh', MAGEL_NO_STDIN_INPUT='1')
    with tempfile.TemporaryDirectory(prefix='magel-251-') as directory:
        folder = Path(directory)
        for name, source in CASES.items():
            selected = os.environ.get('MAGEL_V251_CASE')
            if selected and selected != name:
                continue
            path = folder / (name + '.mgl'); path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(COMPILER), 'run', str(path), 'out', str(folder / (name + '-vm'))],
                                    capture_output=True, text=True, encoding='utf-8', env=env, timeout=90)
            assert result.returncode == 0 and not result.stderr, (name, result.stdout, result.stderr)
            if os.environ.get('MAGEL_V251_VM_ONLY'):
                print(name + ': VM 通过', flush=True); continue
            executable = folder / (name + ('.exe' if os.name == 'nt' else ''))
            build = subprocess.run([str(COMPILER), 'aot', str(path), 'out', str(executable), 'opt', '0'],
                                   capture_output=True, text=True, encoding='utf-8', env=env, timeout=180)
            assert build.returncode == 0, (name, build.stdout, build.stderr)
            aot = subprocess.run([str(executable)], capture_output=True, text=True, encoding='utf-8', timeout=30)
            assert aot.returncode == 0 and aot.stdout == result.stdout and not aot.stderr, (name, aot.stdout, aot.stderr, result.stdout)
            print(name + ': VM / AOT 通过', flush=True)
        if not os.environ.get('MAGEL_V251_CASE'):
            for name, (source, fragment) in ERRORS.items():
                path = folder / (name + '.mgl'); path.write_text(source, encoding='utf-8')
                result = subprocess.run([str(COMPILER), 'check', str(path)], capture_output=True, text=True,
                                        encoding='utf-8', env=env, timeout=90)
                assert result.returncode != 0 and fragment in result.stdout + result.stderr, (name, result.stdout, result.stderr)
            source = folder / 'panic.mgl'
            source.write_text('void inner(){defer println("inner");panic("bad");}int64 main(){defer println("outer");inner();return 0;}', encoding='utf-8')
            result = subprocess.run([str(COMPILER), 'run', str(source), 'out', str(folder / 'panic-vm')],
                                    capture_output=True, text=True, encoding='utf-8', env=env, timeout=60)
            assert result.returncode != 0 and result.stdout == 'inner\nouter\n', (result.stdout, result.stderr)
            if not os.environ.get('MAGEL_V251_VM_ONLY'):
                executable = folder / ('panic.exe' if os.name == 'nt' else 'panic')
                build = subprocess.run([str(COMPILER), 'aot', str(source), 'out', str(executable), 'opt', '0'],
                                       capture_output=True, text=True, encoding='utf-8', env=env, timeout=180)
                assert build.returncode == 0, (build.stdout, build.stderr)
                result = subprocess.run([str(executable)], capture_output=True, text=True, encoding='utf-8', timeout=30)
                assert result.returncode != 0 and result.stdout == 'inner\nouter\n', (result.stdout, result.stderr)
    print('0.17.1 六项功能与编译期诊断回归全部通过。')


if __name__ == '__main__':
    main()
