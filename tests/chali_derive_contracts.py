"""验证内建与用户派生共存、成员冲突及标准泛型的名义身份。"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """执行拥有值和资源对照，并拒绝伪装标准类型与方法覆盖。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    args = parser.parse_args()
    label = '''interface Label{String label();}
$macro Label<T>(method:ident) -> member
$implement Label;
String debug(){return "helper";}
String method(){return self.debug();}
$end_macro
'''
    encoder = '''interface Encoder{Result<String,Error> toJson();}
$macro Encoder<T>(method:ident) -> member
$implement Encoder;
Result<String,Error> method(){return Result<String,Error>::Ok("{}");}
$end_macro
'''
    cases = []
    for index, macros in enumerate((('LABEL', 'DEBUG'), ('DEBUG', 'LABEL'))):
        cases.append(('helper_' + str(index), label + '''struct Data {int64 value; $std::macro::Clone<Data>(clone); ''' + ' '.join({'LABEL': '$Label<Data>(label);', 'DEBUG': '$std::macro::Debug<Data>(debug);'}[macro] for macro in macros) + ''' }
void main(){Data value;value.value=7;let copy=value.clone();assert(copy.value==7&&value.label()=="helper"&&value.debug()=="Data { value: 7 }");}''', None))
    cases += [
        ('enum_protocols', label + '''enum Event {
    None, Pair(int64 left, String right), Many(int64 a, int64 b, int64 c, int64 d);
    $std::macro::Clone<Event>(clone);
    $std::macro::Equal<Event>(leftEqual);
    $std::macro::Hash<Event>(hash);
    $Label<Event>(label);
    $std::macro::Debug<Event>(debug);
    $std::json::Json<Event>(toJson, fromJson);
}
void main(){let value=Event::Pair(7,"owned");let decoded=Event::fromJson(value.toJson().unwrap()).unwrap();let copy=decoded.clone();assert(value==copy&&value.hash(value)==copy.hash(copy)&&copy.label()=="helper");assert(copy.debug().contains("Event::Pair")&&copy.debug().contains("owned"));}''', None),
        ('resource_protocols', label + '''int64 closed=0;
struct Resource:Closeable,Clone<Resource>,Debug{int64 value;void close(){closed++;}Resource clone(){Resource result;result.value=self.value;return move result;}String debug(){return "resource";}}
struct Owner{Resource resource; $std::macro::Clone<Owner>(clone); $std::macro::Debug<Owner>(debug); $Label<Owner>(label); }
void main(){{Owner value;value.resource.value=7;let copy=value.clone();assert(copy.resource.value==7&&value.label()=="helper"&&value.debug()=="Owner { resource: resource }");}assert(closed==2);}''', None),
        ('option_lookalike', 'struct Option__Lookalike{int64 value;}\nstruct Outer{Option__Lookalike inner; $std::json::Json<Outer>(toJson, fromJson);}void main(){}', '嵌套类型必须实现 std::json::Json'),
        ('result_lookalike', 'enum Result__Lookalike{Ok(int64 value),Err(String message)}\nstruct Outer{Result__Lookalike inner; $std::json::Json<Outer>(toJson, fromJson);}void main(){}', '嵌套类型必须实现 std::json::Json'),
        ('standard_result', '''struct Data{Result<int64,String> value;Option<int64> optional; $std::json::Json<Data>(toJson, fromJson);}
void main(){Data value;value.value=Result<int64,String>::Err("expected");value.optional=Option<int64>::Value(7);let copied=Data::fromJson(value.toJson().unwrap()).unwrap();assert(copied.value.error()=="expected"&&copied.optional.unwrap()==7);}''', None),
        ('qualified_protocol', '''import derive_tools;
struct Data{int64 value; $derive_tools::Debug<Data>(label); $std::macro::Debug<Data>(debug); }
void main(){Data value;assert(value.label()=="custom"&&value.debug()=="Data { value: 0 }");}''', None),
        ('module_generics', '''import alpha;
import beta;
void main(){alpha::Box<int64> first;first.value=7;assert(first.debug().contains("value: 7"));beta::Box<int64> second;second.value=9;beta::Box<int64> copied=second;assert(second==copied&&second.hash(second)==copied.hash(copied));}''', None),
    ]
    for index, macros in enumerate((('JSON', 'ENCODER'), ('ENCODER', 'JSON'))):
        calls = {'JSON': '$std::json::Json<Data>(toJson, fromJson);', 'ENCODER': '$Encoder<Data>(toJson);'}
        cases.append(('encoder_collision_' + str(index), encoder + 'struct Data{int64 value;' + ' '.join(calls[macro] for macro in macros) + '}void main(){}', '生成成员名称或签名冲突'))
    if args.case:
        cases = [case for case in cases if case[0] in args.case]
        assert len(cases) == len(set(args.case))
    with tempfile.TemporaryDirectory(prefix='derive-contracts-') as directory:
        work = Path(directory)
        (work / 'derive_tools.chl').write_text('''module derive_tools;
interface Debug{String label();}
$macro Debug<T>(method:ident) -> member
$implement Debug;
String method(){return "custom";}
$end_macro
''', encoding='utf-8')
        (work / 'alpha.chl').write_text('module alpha;\nimport std::macro;\nstruct Box<T>{T value; $std::macro::Debug<Box<T>>(debug); }\n', encoding='utf-8')
        (work / 'beta.chl').write_text('module beta;\nimport std::macro;\nstruct Box<T> : Equal<bool, Box<T>>, Hash<Box<T>>{T value; $std::macro::Equal<Box<T>>(leftEqual); $std::macro::Hash<Box<T>>(hash); }\n', encoding='utf-8')
        def check(case):
            """每例使用独立编译器与产物，避免共享生成状态。"""
            name, code, diagnostic = case
            source = work / (name + '.chl')
            source.write_text('import std::json;\nimport std::macro;\n' + code, encoding='utf-8')
            command = [str(args.chali.resolve()), 'check' if diagnostic else 'run', str(source), '--out', str(work / name)]
            result = subprocess.run(command, cwd=work, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=240)
            if diagnostic:
                assert result.returncode != 0 and diagnostic in result.stderr, (name, result)
            else:
                assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), (name, result)
            print(name + ' 通过', flush=True)
        with ThreadPoolExecutor(max_workers=2) as pool:
            for future in [pool.submit(check, case) for case in cases]:
                future.result()
    print(f'派生冲突、资源与标准身份通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
