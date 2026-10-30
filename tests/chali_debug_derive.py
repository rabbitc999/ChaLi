"""验证 Debug 派生的字段、载荷、UTF-8 转义、嵌套协议与拒绝规则。"""
import argparse
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """以真实编译器执行调试协议，明确拒绝没有契约的字段。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    args = parser.parse_args()
    cases = [
        ('struct', '''#derive(Debug)
struct Data{int64 number;bool enabled;}
void main(){Data value;value.number=7;value.enabled=true;assert(value.debug()=="Data { number: 7, enabled: true }");}''', None),
        ('empty', '#derive(Debug)\nstruct Empty{}void main(){Empty value;assert(value.debug()=="Empty {}");}', None),
        ('static', '''#derive(Debug)
struct Data{static int64 counter;int64 value;}
void main(){Data::counter=99;Data value;value.value=7;assert(value.debug()=="Data { value: 7 }");}''', None),
        ('unicode', r'''#derive(Debug)
struct Text{String value;}
void main(){Text value;value.value="中文😀\n\r\t\"\\";value.value.append(std::lang::format("{}",cast<char>(1)));value.value.append(std::lang::format("{}",cast<char>(127)));assert(value.debug()=="Text { value: \"中文😀\\n\\r\\t\\\"\\\\\\x01\\x7f\" }");}''', None),
        ('scalars', '''#derive(Debug)
struct Numbers{int8 a;int16 b;int32 c;int64 d;uint8 e;uint16 f;uint32 g;uint64 h;float32 i;float64 j;bool k;char l;}
void main(){Numbers value;value.a=(int8)-8;value.b=(int16)-16;value.c=(int32)-32;value.d=-64;value.e=(uint8)8;value.f=(uint16)16;value.g=(uint32)32;value.h=18446744073709551615u;value.i=(float32)1.5;value.j=2.5;value.k=true;value.l='Q';String text=value.debug();assert(text.contains("a: -8")&&text.contains("h: 18446744073709551615")&&text.contains("l: 'Q'"));}''', None),
        ('nested', '''#derive(Debug)
struct Inner{int64 value;}
#derive(Debug)
struct Outer{Inner item;}
void main(){Outer value;value.item.value=7;assert(value.debug()=="Outer { item: Inner { value: 7 } }");}''', None),
        ('enum', '''#derive(Debug)
enum Event{None,One(int64 value),Pair(int64 left,String right),Many(int8 a,int16 b,int32 c,int64 d)};
void main(){assert(Event::None.debug()=="Event::None");assert(Event::One(7).debug()=="Event::One(value: 7)");assert(Event::Pair(7,"中文").debug()=="Event::Pair(left: 7, right: \\\"中文\\\")");assert(Event::Many((int8)1,(int16)2,(int32)3,4).debug()=="Event::Many(a: 1, b: 2, c: 3, d: 4)");}''', None),
        ('generic', '''#derive(Debug)
struct Box<T>{T value;}
void main(){Box<int16> value;value.value=(int16)7;assert(value.debug().contains("Box<int16>"));assert(value.debug().contains("value: 7"));}''', None),
        ('manual', '''struct Inner:Debug{String debug(){return "manual";}}
#derive(Debug)
struct Outer{Inner value;}
void main(){Outer value;assert(value.debug()=="Outer { value: manual }");}''', None),
        ('interface', '''#derive(Debug)
struct Data{int64 value;}
void main(){Data value;value.value=7;Debug contract=value;assert(contract.debug()=="Data { value: 7 }");}''', None),
        ('nested_enum', '''#derive(Debug)
enum State{None,Value(int64 number)};
#derive(Debug)
struct Holder{State state;}
void main(){Holder value;value.state=State::Value(7);assert(value.debug()=="Holder { state: State::Value(number: 7) }");}''', None),
        ('resource', '''int64 closed=0;
struct Resource:Closeable,Debug{int64 id;void close(){if(self.id!=0){closed+=1;self.id=0;}}String debug(){return std::lang::format("resource {}",self.id);}}
#derive(Debug)
struct Holder{Resource resource;}
void inspect(){Holder value;value.resource.id=7;assert(value.debug()=="Holder { resource: resource 7 }");assert(value.resource.id==7&&closed==0);}
void main(){inspect();assert(closed==1);}''', None),
        ('character_escape', r'''#derive(Debug)
struct Letter{char value;}
void main(){Letter value;value.value='\n';assert(value.debug()=="Letter { value: '\\n' }");value.value='\'';assert(value.debug()=="Letter { value: '\\'' }");}''', None),
        ('inherited', '''interface Inspect:Debug{}
struct Inner:Inspect{String debug(){return "inherited";}}
#derive(Debug)
struct Holder{Inner value;}
void main(){Holder value;assert(value.debug()=="Holder { value: inherited }");}''', None),
        ('string_shadow', '''struct String:Debug{std::lang::String debug(){return "custom";}}
#derive(Debug)
struct Holder{String custom;std::lang::String text;}
void main(){Holder value;value.text="中文";assert(value.debug()=="Holder { custom: custom, text: \\\"中文\\\" }");}''', None),
        ('combined', '''#derive(Equal,Hash,Clone,Debug)
struct Data{String text;}
void main(){Data value;value.text="中文";let copied=value.clone();assert(value==copied&&value.hash(value)==copied.hash(copied)&&value.debug()==copied.debug());}''', None),
        ('module_name', '''module debug_shapes;
#derive(Debug)
struct Data{int64 value;}
void main(){Data value;value.value=7;assert(value.debug()=="debug_shapes::Data { value: 7 }");}''', None),
        ('protocol_shadow', '''interface Debug{int64 identifier();}
#derive(Debug)
struct Data{int64 value;}
void main(){Data value;value.value=7;std::lang::Debug contract=value;assert(contract.debug()=="Data { value: 7 }");}''', None),
        ('conflict', '#derive(Debug)\nstruct Data{String debug(){return "manual";}}void main(){}', ['不覆盖已有方法']),
        ('missing', 'struct Inner{int64 value;}\n#derive(Debug)\nstruct Outer{Inner value;}void main(){}', ['未实现 Debug']),
        ('array', '#derive(Debug)\nstruct Data{int64 values[2];}void main(){}', ['数组字段']),
        ('reference', '#derive(Debug)\nstruct Data{int64 const& value;}void main(){}', ['Debug']),
        ('container', '#derive(Debug)\nstruct Data{List<int64> values;}void main(){}', ['Debug']),
        ('function', '#derive(Debug)\nstruct Data{int64 fn(int64) callback;}void main(){}', ['Debug']),
    ]
    if args.case:
        cases = [case for case in cases if case[0] in args.case]
        assert len(cases) == len(set(args.case))
    env = dict(os.environ, CHALI_LANG='zh')
    with tempfile.TemporaryDirectory(prefix='debug-derive-') as directory:
        work = Path(directory)
        for name, code, diagnostic in cases:
            source = work / (name + '.chl')
            source.write_text(code, encoding='utf-8')
            command = [str(args.chali.resolve()), 'check' if diagnostic else 'run', str(source)]
            if not diagnostic:
                command += ['--out', str(work / name)]
            result = subprocess.run(command, cwd=work, env=env, capture_output=True, encoding='utf-8', timeout=180)
            if diagnostic:
                assert result.returncode != 0 and all(text in result.stdout + result.stderr for text in diagnostic), (name, result)
            else:
                assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), (name, result)
            print(name + ' 通过', flush=True)
    print(f'Debug 派生通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
