"""拥有接口值的泛型载荷、动态调用、独立复制与销毁回归。"""
import argparse
import os
from pathlib import Path
import subprocess
import unittest

from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMPILER = compiler_path()


class InterfaceValueTests(unittest.TestCase):
    def program(self, source, success=True):
        with tempfile.TemporaryDirectory(prefix='interface-values-') as directory:
            path = Path(directory) / 'main.chl'
            path.write_text(source, encoding='utf-8')
            result = subprocess.run(
                [str(COMPILER), 'run' if success else 'check', str(path),
                 'out', str(Path(directory) / 'program.exe')],
                cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1'),
                capture_output=True, encoding='utf-8', errors='replace', timeout=240)
            self.assertEqual(result.returncode == 0, success, result.stdout + result.stderr)
            if success:
                self.assertEqual(result.stdout + result.stderr, '')

    def test_owned_value_copy_return_and_generic(self):
        self.program('''
interface Read { String read(); void change(String text); };
struct Text : Read, Drop {
    static int64 drops;
    String value;
    String read() { return self.value; }
    void change(String text) { self.value=text; }
    void drop() { Text::drops=Text::drops+1; }
};
Read make() { Text local; local.value="local"; return local; }
Read copy(Read value) { value.change("parameter"); return value; }
void main() {
    {
        Read a=make(); assert(a.read()=="local");
        Read b=a; b.change("copy");
        assert(a.read()=="local" && b.read()=="copy");
        Read c=copy(a); assert(c.read()=="parameter" && a.read()=="local");
        Result<int64, Read> result=Result<int64,Read>::Err(a);
        match(result) { Ok(value)=>{assert(false);} Err(error)=>{assert(error.read()=="local");} }
        Option<Read> option=Option<Read>::Value(b);
        assert(option.unwrap().read()=="copy");
        List<Read> items; items.push(a); assert(items[0].read()=="local");
    }
    assert(Text::drops>=4);
}
''')

    def test_unrelated_type_rejected(self):
        self.program('''interface Read { int64 read(); };
struct Other { int64 value; };
void main() { Other other; Read value=other; }
''', False)

    def test_joint_patterns_and_open_interface_fallback(self):
        self.program('''
interface A { int64 number(); };
interface B { int64 code(); };
struct Success : A { int64 number(){return 7;} };
struct E : B { int64 code(){return 11;} };
struct F : B { int64 code(){return 22;} };
struct Other : B { int64 code(){return 33;} };
int64 read(Result<A,B> res) {
    int64 result=0;
    match(res) {
        OK(A a)=>{result=a.number();}
        Ok(_)=>{result=-1;}
        Err(E b)=>{result=b.code();}
        Err(F c)=>{result=c.code();}
        Err(_ d)=>{result=d.code();}
    }
    return result;
}
void main() {
    Success s; E e; F f; Other other;
    assert(read(Result<A,B>::Ok(s))==7);
    assert(read(Result<A,B>::Err(e))==11);
    assert(read(Result<A,B>::Err(f))==22);
    assert(read(Result<A,B>::Err(other))==33);
    B error=f; int64 result=0;
    match(error) { E(value)=>{result=1;} F(value)=>{result=value.code();} _=>{result=3;} }
    assert(result==22);
    match(error) { E(value)=>{assert(false);} _(value)=>{assert(value.code()==22);} }
    Result<A,B> res=Result<A,B>::Err(other);
    match(res) { Ok(_)=>{assert(false);} Err(E e)=>{assert(false);} Err(_)=>{} }
}
''')

    def test_result_option_and_custom_propagation(self):
        self.program('''
struct IoError : Error {
    String message;
    String toString(){return self.message;}
};
Result<int64,IoError> leaf(bool fail) {
    if(fail) { IoError error; error.message="io"; return Result<int64,IoError>::Err(error); }
    return Result<int64,IoError>::Ok(9);
}
Result<int64,Error> outer(bool fail) { let value=leaf(fail)?; return Result<int64,Error>::Ok(value+1); }
Option<IoError> optional(bool none) {
    if(none){return Option<IoError>::None;}
    IoError error; error.message="option"; return Option<IoError>::Value(error);
}
Option<Error> optionOuter(bool none) { let value=optional(none)?; return Option<Error>::Value(value); }
enum Custom : Try<int64,IoError> {
    Good, Bad;
    TryBranch<int64,IoError> branch() {
        match(self) {
            Custom::Good=>{return TryBranch<int64,IoError>::Continue(12);}
            Custom::Bad=>{IoError error; error.message="custom"; return TryBranch<int64,IoError>::Break(error);}
        }
    }
};
struct Receiver : FromResidual<Receiver,Error> {
    Error error;
    int64 value;
    static Receiver fromResidual(Error error) { Receiver result; result.error=error; return result; }
};
Receiver customOuter(Custom input) { let value=input?; Receiver result; result.value=value; return result; }
interface Fallible : Try<int64,IoError> {};
struct Dynamic : Fallible {
    bool fail;
    TryBranch<int64,IoError> branch() {
        if(self.fail){IoError error;error.message="dynamic";return TryBranch<int64,IoError>::Break(error);}
        return TryBranch<int64,IoError>::Continue(19);
    }
};
Receiver dynamicOuter(Fallible input) { let value=input?; Receiver result;result.value=value;return result; }
void main() {
    std::lang::assert(outer(false).unwrap()==10,"result success",1);
    let failure=outer(true);
    match(failure) { Ok(_)=>{std::lang::assert(false,"result must fail",1);} Err(IoError error)=>{std::lang::assert(error.message=="io","result error text",1);} Err(_)=>{std::lang::assert(false,"dynamic error type",1);} }
    std::lang::assert(optionOuter(true).isNone(),"option none",1);
    std::lang::assert(optionOuter(false).unwrap().toString()=="option","option value",1);
    std::lang::assert(customOuter(Custom::Good).value==12,"custom success",1);
    std::lang::assert(customOuter(Custom::Bad).error.toString()=="custom","custom error",1);
    Dynamic dynamic;std::lang::assert(dynamicOuter(dynamic).value==19,"interface propagation success",1);
    dynamic.fail=true;std::lang::assert(dynamicOuter(dynamic).error.toString()=="dynamic","interface propagation error",1);
}
''')

    def test_generic_interface_and_exhaustive_returns(self):
        self.program('''
interface Value<T> { T value(); };
struct Number : Value<int64> { int64 value(){return 17;} };
int64 read(Result<Value<int64>, Error> result) {
    match(result) { Ok(Number n)=>{return n.value();} Ok(_)=>{return 0;} Err(_)=>{return -1;} }
}
int64 direct(Value<int64> value) {
    match(value) { Number(n)=>{return n.value();} _(other)=>{return other.value();} }
}
void main(){Number n; Value<int64> value=n;
assert(read(Result<Value<int64>,Error>::Ok(n))==17);
assert(direct(value)==17);}
''')

    def test_incomplete_or_duplicate_patterns_rejected(self):
        prefix='''interface Base {int64 value();};
struct First : Base {int64 value(){return 1;}};
struct Second : Base {int64 value(){return 2;}};
'''
        for body in (
            'First first;Base value=first;match(value){First(x)=>{}}',
            'First first;Result<int64,Base> value=Result<int64,Base>::Err(first);match(value){Ok(_)=>{} Err(First x)=>{} Err(Second y)=>{}}',
            'First first;Result<int64,Base> value=Result<int64,Base>::Err(first);match(value){Ok(_)=>{} Err(First x)=>{} Err(First y)=>{} Err(_)=>{}}',
        ):
            with self.subTest(body=body):
                self.program(prefix+'void main(){'+body+'}',False)

    def test_boxed_storage_returns_to_baseline(self):
        self.program('''
import std::memory;
interface TextValue {String text();};
struct Value : TextValue {String value;String text(){return self.value;}};
int64 blocks(){let stats=std::memory::Memory::stats();return stats.liveBlocks;}
void exercise(){
    int64 i=0;
    while(i<32){
        {Value original;original.value="owned";
         TextValue first=original;TextValue second=first;
         Result<int64,TextValue> result=Result<int64,TextValue>::Err(second);
         match(result){Ok(_)=>{assert(false);} Err(Value actual)=>{assert(actual.value=="owned");} Err(_)=>{assert(false);}}
        }
        i=i+1;
    }
}
void main(){let before=blocks();exercise();assert(blocks()==before);}
''')

    def test_owned_interface_cannot_hide_local_borrow(self):
        self.program('''interface Read {int64 read();};
struct Borrowed : Read {int64 const& value;int64 read(){return self.value;}};
Read escape(){int64 local=7;Borrowed source;source.value=&local;return source;}
void main(){Read value=escape();}
''',False)

    def test_typed_pattern_is_not_a_call_argument(self):
        self.program('''void consume(int64 value){} void main(){int64 value=7;consume(int64 value);}
''',False)

    def test_enum_interface_pattern_sugar(self):
        self.program('''import std::io;
struct LocalError : Error {String text;String toString(){return self.text;}};
void main(){
    LocalError error;error.text="local";
    Result<int64,Error> local=Result<int64,Error>::Err(error);
    int64 count=0;
    match(local){Ok(_)=>{assert(false);} Err::LocalError(e)=>{assert(e.text=="local");count=count+1;} Err(_)=>{assert(false);}}
    match(local){Ok(_)=>{assert(false);} Err::LocalError(_)=>{count=count+1;} Err(_)=>{assert(false);}}
    match(local){Ok(_)=>{assert(false);} LocalError(e)=>{assert(e.text=="local");count=count+1;} Err(_)=>{assert(false);}}
    Result<int64,Error> io=Result<int64,Error>::Err(std::io::IoError::Closed);
    match(io){Ok(_)=>{assert(false);} Err::std::io::IoError(e)=>{assert(e.toString()!="");count=count+1;} Err(_)=>{assert(false);}}
    assert(count==4);
}
''')

    def test_two_three_and_four_payloads(self):
        self.program('''
interface Base {int64 value();};
struct E : Base {int64 value(){return 1;}};
struct F : Base {int64 value(){return 2;}};
struct Other : Base {int64 value(){return 3;}};
enum Multi {
    Both(Base left,Base right), Three(Base left,Base right,String text),
    Four(Base left,Base right,String text,int64 number), Empty;
};
int64 read(Multi input){
    int64 result=0;
    match(input){
        Both(E a,F b) if false=>{assert(false);}
        Both(E a,F b)=>{result=a.value()*10+b.value();}
        Both(F a,E b)=>{result=a.value()*10+b.value();}
        Both(_,_)=>{result=-2;}
        Three(E a,F b,text)=>{assert(text=="three");result=33;}
        Three(_,_,_)=>{result=-3;}
        Four(F a,E b,text,count)=>{assert(text=="four" && count==4);result=44;}
        Four(E a,F b,String text,int64 count)=>{assert(text=="typed" && count==5);result=45;}
        Four(_,_,_,_)=>{result=-4;}
        Empty=>{result=0;}
    }
    return result;
}
enum Plain {Pair(int64 number,String text),Triple(int64 number,String text,bool flag),Four(int64 a,int64 b,int64 c,int64 d);};
int64 plain(Plain input){
    match(input){Pair(number,text)=>{assert(text=="pair");return number;}
        Triple(number,text,flag)=>{assert(text=="triple" && flag);return number;}
        Four(a,b,c,d)=>{return a+b+c+d;}}
}
void main(){E e;F f;Other other;
    assert(read(Multi::Both(e,f))==12 && read(Multi::Both(f,e))==21);
    assert(read(Multi::Both(other,e))==-2);
    assert(read(Multi::Three(e,f,"three"))==33);
    assert(read(Multi::Four(f,e,"four",4))==44);
    assert(read(Multi::Four(e,f,"typed",5))==45);
    assert(read(Multi::Empty)==0);
    let matched=match(Multi::Both(e,f)){
        Both(E a,F b) if false=>{99}
        Both(E a,F b)=>{a.value()+b.value()}
        _=>{0}
    };
    assert(matched==3);
    assert(plain(Plain::Pair(7,"pair"))==7);
    assert(plain(Plain::Triple(8,"triple",true))==8);
    assert(plain(Plain::Four(1,2,3,4))==10);
}
''')

    def test_omitted_tag_ambiguity_and_multi_payload_sugar_rejected(self):
        prefix='''interface Base {int64 value();};struct E : Base {int64 value(){return 1;}};
enum Choice {First(Base value),Second(Base value),Both(Base a,Base b);};
'''
        for pattern in ('E(e)', 'Both::E(e)', 'First::Bogus::E(e)'):
            self.program(prefix+'void main(){E e;Choice value=Choice::First(e);match(value){'+pattern+'=>{} _=>{}}}',False)
        self.program(prefix+'''void main(){E e;Choice value=Choice::First(e);int64 result=0;
match(value){First::E(item)=>{result=item.value();} _=>{assert(false);}}
assert(result==1);}
''')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=COMPILER)
    arguments, remaining = parser.parse_known_args()
    COMPILER = arguments.chali.resolve()
    unittest.main(argv=[__file__, *remaining], verbosity=2)
