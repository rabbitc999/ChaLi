"""验证泛型实参、默认类型和约束保留模块来源及核心别名身份。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """使用真实编译器对照标准类型与用户同名类型，不依赖默认全局裸名选择。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    args=parser.parse_args()
    cases=[
        ('empty_shadow','struct String{int64 tag;}void main(){}',None),
        ('standard_string','struct String{int64 tag;}void main(){std::core::String text="中文";assert(text==text);}',None),
        ('user_argument','''struct String{int64 tag;}
T identity<T>(T value){return value;}
void main(){String value;value.tag=7;String copy=identity<String>(value);assert(copy.tag==7);std::core::String text=identity<std::core::String>("中文");assert(text=="中文");}''',None),
        ('nested','''struct String{int64 tag;}
T identity<T>(T value){return value;}
void main(){List<String> values;String item;item.tag=7;values.push(item);let copied=identity<List<String>>(values);assert(copied[0].tag==7);List<std::core::String> texts;texts.push("中文");let textCopy=identity<List<std::core::String>>(texts);assert(textCopy[0]=="中文");}''',None),
        ('tuple','''struct String{int64 tag;}
T identity<T>(T value){return value;}
void main(){String value;value.tag=7;let pair=identity<(String,std::core::String)>((value,"text"));assert(pair[0].tag==7&&pair[1]=="text");}''',None),
        ('reference','''struct String{int64 tag;}
T identity<T>(T value){return value;}
void main(){String value;value.tag=7;let borrowed=identity<String const&>(&value);assert(borrowed.tag==7);}''',None),
        ('function_type','''struct String{int64 tag;}
T identity<T>(T value){return value;}
String echoUser(String value){return value;}
std::core::String echoText(std::core::String value){return value;}
void main(){String value;value.tag=7;let user=identity<String fn(String)>(echoUser);let text=identity<std::core::String fn(std::core::String)>(echoText);assert(user(value).tag==7&&text("中文")=="中文");}''',None),
        ('derive','''import std::macro;
struct String{int64 tag;}
struct Label : Equal<bool, Label>, Hash<Label>, Clone<Label>{std::core::String text; $std::macro::Equal<Label>(leftEqual); $std::macro::Hash<Label>(hash); $std::macro::Clone<Label>(clone); }
void main(){Label a;a.text="中文😀";let b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));}''',None),
        ('other_aliases','''struct File{int64 id;}struct Process{int64 id;}struct BigInteger{int64 id;}struct BigDecimal{int64 id;}
T identity<T>(T value){return value;}
void main(){File a;a.id=1;Process b;b.id=2;BigInteger c;c.id=3;BigDecimal d;d.id=4;assert(identity<File>(a).id==1&&identity<Process>(b).id==2&&identity<BigInteger>(c).id==3&&identity<BigDecimal>(d).id==4);}''',None),
        ('reserved_any','struct Any{int64 id;}void main(){}',['保留名']),
        ('local_module','''import alpha;
import beta;
void main(){let a=alpha::make();let b=beta::make();assert(a.tag==11&&b.tag==22);}''',None),
        ('default_definition','''import alpha;
struct String{int64 caller;}
void main(){alpha::Box<> value;value.value.tag=7;assert(value.value.tag==7);}''',None),
        ('caller_argument','''import alpha;
struct String{int64 caller;}
void main(){alpha::Box<String> value;value.value.caller=9;assert(value.value.caller==9);}''',None),
        ('standard_module','''import std::json;
struct String{int64 tag;}
void main(){std::core::String text="{\\\"value\\\":7}";let value=std::json::parse(text).unwrap();assert(value.find("value").unwrap().asInt().unwrap()==7);}''',None),
        ('reflection_interface','''import std::reflect;
@Reflect interface View{int64 read(){return 7;}}
@Reflect struct Record:View{}
void main(){Record mut& object=new Record;Any holder;holder.set(&object);View const& view=holder.getReference();List<std::reflect::ReflectValue> args;assert(std::reflect::callInterfaceConst<View>(view,"read",args).unwrap().asI64().unwrap()==7);object.drop();}''',None),
        ('ambiguous','''import alpha;
import beta;
struct Box<T>{T value;}
void main(){Box<String> value;}''',['歧义']),
        ('qualified_templates','''import alpha;
import beta;
void main(){alpha::Clash<int64> source;beta::Clash<int64> target=source;}''',
         ['alpha::Clash<int64> -> beta::Clash<int64>']),
        ('qualified_arguments','''import alpha;
import beta;
struct Wrap<T>{T value;}
void main(){Wrap<alpha::String> source;Wrap<beta::String> target=source;}''',
         ['Wrap<alpha::String> -> Wrap<beta::String>']),
        ('qualified_core_shadow','''struct String{int64 tag;}
struct Wrap<T>{T value;}
void main(){Wrap<String> source;Wrap<std::core::String> target=source;}''',
         ['Wrap<program::String> -> Wrap<std::core::String>']),
    ]
    if args.case:
        cases=[case for case in cases if case[0] in args.case]
        assert len(cases)==len(set(args.case))
    failures=[]
    with tempfile.TemporaryDirectory(prefix='generic-scope-') as directory:
        work=Path(directory)
        (work/'alpha.chl').write_text('''module alpha;
struct String{int64 tag;}
struct Box<T=String>{T value;}
struct Clash<T>{T value;}
T identity<T>(T value){return value;}
String make(){String value;value.tag=11;return identity<String>(value);}
''',encoding='utf-8')
        (work/'beta.chl').write_text('''module beta;
struct String{int64 tag;}
struct Clash<T>{T value;}
T identity<T>(T value){return value;}
String make(){String value;value.tag=22;return identity<String>(value);}
''',encoding='utf-8')
        for name,source,diagnostics in cases:
            path=work/(name+'.chl')
            path.write_text(source,encoding='utf-8')
            modes=[False,True] if name in {'standard_string','derive'} else [False]
            for uncached in modes:
                result=subprocess.run([str(args.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name),*(['--no-stdlib-cache'] if uncached else [])],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
                valid=result.returncode!=0 and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
                if not valid:failures.append((name,uncached,result.returncode,result.stdout,result.stderr))
                print(name,'source' if uncached else 'embedded','通过' if valid else ('失败 '+repr(failures[-1])),flush=True)
            if name.startswith('qualified_'):
                result=subprocess.run([str(args.chali.resolve()),'--check','--diagnostics=json',str(path)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
                assert result.returncode==1, (name,result)
                messages=[item['message'] for item in json.loads(result.stdout)['diagnostics']]
                assert any(all(text in message for text in diagnostics) and '__chl_' not in message for message in messages), (name,messages)
    assert not failures,failures
    print(f'泛型来源作用域通过：{len(cases)} 项，关键项对照源码与内嵌路径')


if __name__=='__main__':main()
