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
$macro Label<T>() -> member
String debug(){return "helper";}
String label(){return self.debug();}
$end_macro
'''
    encoder = '''interface Encoder{Result<String,Error> toJson();}
$macro Encoder<T>() -> member
Result<String,Error> toJson(){return Result<String,Error>::Ok("{}");}
$end_macro
'''
    cases = []
    for index, traits in enumerate(('Label,Debug,Clone', 'Clone,Debug,Label')):
        cases.append(('helper_' + str(index), label + '#derive(' + traits + ''')
struct Data{int64 value;}
void main(){Data value;value.value=7;let copy=value.clone();assert(copy.value==7&&value.label()=="helper"&&value.debug()=="Data { value: 7 }");}''', None))
    cases += [
        ('enum_protocols', label + '''#derive(Label,Debug,Json,Clone,Equal,Hash)
enum Event{None,Pair(int64 left,String right),Many(int64 a,int64 b,int64 c,int64 d)};
void main(){let value=Event::Pair(7,"owned");let decoded=Event::fromJson(value.toJson().unwrap()).unwrap();let copy=decoded.clone();assert(value==copy&&value.hash(value)==copy.hash(copy)&&copy.label()=="helper");assert(copy.debug().contains("Event::Pair")&&copy.debug().contains("owned"));}''', None),
        ('resource_protocols', label + '''int64 closed=0;
struct Resource:Closeable,Clone<Resource>,Debug{int64 value;void close(){closed++;}Resource clone(){Resource result;result.value=self.value;return move result;}String debug(){return "resource";}}
#derive(Clone,Debug,Label)
struct Owner{Resource resource;}
void main(){{Owner value;value.resource.value=7;let copy=value.clone();assert(copy.resource.value==7&&value.label()=="helper"&&value.debug()=="Owner { resource: resource }");}assert(closed==2);}''', None),
        ('option_lookalike', 'struct Option__Lookalike{int64 value;}\n#derive(Json)\nstruct Outer{Option__Lookalike inner;}void main(){}', '嵌套类型必须派生 Json'),
        ('result_lookalike', 'enum Result__Lookalike{Ok(int64 value),Err(String message)}\n#derive(Json)\nstruct Outer{Result__Lookalike inner;}void main(){}', '嵌套类型必须派生 Json'),
        ('standard_result', '''#derive(Json)
struct Data{Result<int64,String> value;Option<int64> optional;}
void main(){Data value;value.value=Result<int64,String>::Err("expected");value.optional=Option<int64>::Value(7);let copied=Data::fromJson(value.toJson().unwrap()).unwrap();assert(copied.value.error()=="expected"&&copied.optional.unwrap()==7);}''', None),
        ('qualified_protocol', '''import derive_tools;
#derive(derive_tools::Debug,Debug)
struct Data{int64 value;}
void main(){Data value;assert(value.label()=="custom"&&value.debug()=="Data { value: 0 }");}''', None),
        ('module_generics', '''import alpha;
import beta;
void main(){alpha::Box<int64> first;first.value=7;assert(first.debug().contains("value: 7"));beta::Box<int64> second;second.value=9;beta::Box<int64> copied=second;assert(second==copied&&second.hash(second)==copied.hash(copied));}''', None),
    ]
    for index, traits in enumerate(('Json,Encoder', 'Encoder,Json')):
        cases.append(('encoder_collision_' + str(index), encoder + '#derive(' + traits + ')\nstruct Data{int64 value;}void main(){}', '不覆盖已有方法'))
    if args.case:
        cases = [case for case in cases if case[0] in args.case]
        assert len(cases) == len(set(args.case))
    with tempfile.TemporaryDirectory(prefix='derive-contracts-') as directory:
        work = Path(directory)
        (work / 'derive_tools.chl').write_text('''module derive_tools;
interface Debug{String label();}
$macro Debug<T>() -> member
String label(){return "custom";}
$end_macro
''', encoding='utf-8')
        (work / 'alpha.chl').write_text('module alpha;\n#derive(Debug)\nstruct Box<T>{T value;}\n', encoding='utf-8')
        (work / 'beta.chl').write_text('module beta;\n#derive(Equal,Hash)\nstruct Box<T>{T value;}\n', encoding='utf-8')
        def check(case):
            """每例使用独立编译器与产物，避免共享生成状态。"""
            name, code, diagnostic = case
            source = work / (name + '.chl')
            source.write_text('import std::json;\n' + code, encoding='utf-8')
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
