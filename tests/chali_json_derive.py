"""验证受限 JSON 派生、字段注解、错误策略与拥有关系。"""
import argparse
import os
from pathlib import Path
import subprocess
from concurrent.futures import ThreadPoolExecutor

from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """通过实际编解码对照具体类型，拒绝不支持的模式。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    parser.add_argument('--jobs', type=int, default=2, help='独立编译器进程数；1 为串行')
    args = parser.parse_args()
    assert args.jobs > 0, 'jobs 必须为正整数'
    simple = 'struct Data{int64 value;String text; $std::json::Json<Data>(toJson, fromJson); }\n'
    cases = [
        ('basic', simple + '''void main(){Data value;value.value=7;value.text="中文😀";let encoded=value.toJson();assert(encoded.isOk());let decoded=Data::fromJson(encoded.unwrap());assert(decoded.isOk());let copied=decoded.take();assert(copied.value==7&&copied.text=="中文😀");copied.text="changed";assert(value.text=="中文😀");}''', None, []),
        ('rename_skip', '''struct Data{@std::json::Field(name:"identifier") int64 id;@std::json::Field(skip:true) String secret; $std::json::Json<Data>(toJson, fromJson); }
void main(){Data value;value.id=7;value.secret="hidden";let encoded=value.toJson().unwrap();assert(encoded=="{\\\"identifier\\\":7}");let decoded=Data::fromJson("{\\\"identifier\\\":9,\\\"secret\\\":\\\"ignored\\\"}").unwrap();assert(decoded.id==9&&decoded.secret=="");}''', None, []),
        ('missing', simple + r'''void main(){let result=Data::fromJson("{\"value\":7}");assert(result.isErr());assert(result.error().toString().contains("缺少字段：text"));}''', None, []),
        ('unknown', simple + r'''void main(){let result=Data::fromJson("{\"value\":7,\"text\":\"ok\",\"extra\":1}");assert(result.isErr());assert(result.error().toString().contains("未知字段：extra"));}''', None, []),
        ('wrong_type', simple + r'''void main(){let result=Data::fromJson("{\"value\":\"bad\",\"text\":\"ok\"}");assert(result.isErr());assert(result.error().toString().contains("value:"));}''', None, []),
        ('malformed', simple + 'void main(){assert(Data::fromJson("{").isErr());}', None, []),
        ('empty', 'struct Empty{ $std::json::Json<Empty>(toJson, fromJson); }void main(){Empty value;assert(value.toJson().unwrap()=="{}");assert(Empty::fromJson("{}").isOk());assert(Empty::fromJson("{\\\"x\\\":1}").isErr());}', None, []),
        ('static', 'struct Data{static int64 shared;int64 value; $std::json::Json<Data>(toJson, fromJson); }void main(){Data::shared=99;Data value;value.value=7;assert(value.toJson().unwrap()=="{\\\"value\\\":7}");}', None, []),
        ('nested', '''struct Inner{String text; $std::json::Json<Inner>(toJson, fromJson); }
struct Outer{Inner inner;List<int64> values;Map<String,String> names;Set<int16> unique;Option<String> optional; $std::json::Json<Outer>(toJson, fromJson); }
void main(){Outer value;value.inner.text="owned";value.values.push(7);value.names.put("中文","value");value.unique.add((int16)3);value.optional=Option<String>::Value("some");let result=Outer::fromJson(value.toJson().unwrap()).unwrap();assert(result.inner.text=="owned"&&result.values.at(0)==7&&result.names.getOption("中文").unwrap()=="value"&&result.unique.has((int16)3)!=0&&result.optional.unwrap()=="some");}''', None, []),
        ('enum', '''enum Event{None,One(int64 value),Pair(int64 value,String text),Many(int8 a,int16 b,int32 c,int64 d); $std::json::Json<Event>(toJson, fromJson); };
void main(){let a=Event::fromJson(Event::None.toJson().unwrap());let b=Event::fromJson(Event::Pair(7,"中文").toJson().unwrap());let c=Event::fromJson(Event::Many((int8)1,(int16)2,(int32)3,4).toJson().unwrap());assert(a.isOk()&&b.isOk()&&c.isOk());match(b.unwrap()){Event::Pair(number,text)=>{assert(number==7&&text=="中文");}_=>{assert(false);}}}''', None, []),
        ('generic', '''struct Box<T>{T value; $std::json::Json<Box<T>>(toJson, fromJson); }
void main(){Box<int16> a;a.value=(int16)7;let copy=Box<int16>::fromJson(a.toJson().unwrap()).unwrap();assert(copy.value==7);Box<String> b;b.value="中文";assert(Box<String>::fromJson(b.toJson().unwrap()).unwrap().value=="中文");}''', None, []),
        ('combined', '''struct Data : Equal<bool, Data>, Hash<Data>, Clone<Data>{String text;int64 value; $std::macro::Equal<Data>(leftEqual); $std::macro::Hash<Data>(hash); $std::macro::Clone<Data>(clone);  $std::macro::Debug<Data>(debug); $std::json::Json<Data>(toJson, fromJson); }
void main(){Data value;value.text="中文";value.value=7;let decoded=Data::fromJson(value.toJson().unwrap()).unwrap();let copy=decoded.clone();assert(value==copy&&value.hash(value)==copy.hash(copy)&&value.debug()==copy.debug());}''', None, []),
        ('interface', simple + 'void main(){Data value;value.value=7;Json contract=value;assert(contract.toJson().isOk());}', None, []),
        ('scalars', '''struct Numbers{int8 a;int16 b;int32 c;int64 d;uint8 e;uint16 f;uint32 g;uint64 h;float32 i;float64 j;bool k;char l; $std::json::Json<Numbers>(toJson, fromJson); }
void main(){Numbers value;value.a=(int8)-8;value.b=(int16)-16;value.c=(int32)-32;value.d=0-9223372036854775807-1;value.e=(uint8)8;value.f=(uint16)16;value.g=(uint32)32;value.h=18446744073709551615u;value.i=(float32)1.5;value.j=2.25;value.k=true;value.l='Q';let text=value.toJson().unwrap();let copied=Numbers::fromJson(text).unwrap();assert(copied.a==value.a&&copied.b==value.b&&copied.c==value.c&&copied.d==value.d&&copied.e==value.e&&copied.f==value.f&&copied.g==value.g&&copied.h==value.h&&copied.i==value.i&&copied.j==value.j&&copied.k&&copied.l=='Q');}''', None, []),
        ('enum_bad_shape', '''enum Event{None,One(int64 value); $std::json::Json<Event>(toJson, fromJson); };
void main(){assert(Event::fromJson("{\\\"$variant\\\":\\\"One\\\",\\\"$values\\\":[]}").isErr());assert(Event::fromJson("{\\\"$variant\\\":\\\"Missing\\\",\\\"$values\\\":[]}").isErr());assert(Event::fromJson("{\\\"$variant\\\":\\\"None\\\",\\\"$values\\\":[],\\\"extra\\\":1}").isErr());}''', None, []),
        ('string_shadow', '''struct String{int64 value; $std::json::Json<String>(toJson, fromJson); }
struct Holder{String custom;std::core::String text; $std::json::Json<Holder>(toJson, fromJson); }
void main(){Holder value;value.custom.value=7;value.text="中文";let copied=Holder::fromJson(value.toJson().unwrap()).unwrap();assert(copied.custom.value==7&&copied.text=="中文");}''', None, []),
        ('protocol_shadow', '''interface Json{int64 identifier();}
struct Data{int64 value; $std::json::Json<Data>(toJson, fromJson); }
void main(){Data value;std::json::Json contract=value;assert(contract.toJson().isOk());}''', None, []),
        ('floating_errors', '''import std::math;
struct Numbers{float32 small;float64 large; $std::json::Json<Numbers>(toJson, fromJson); }
void main(){Numbers value;value.small=(float32)1.5;value.large=std::math::acos(2.0);assert(value.toJson().isErr());value.large=std::math::exp(1000.0);assert(value.toJson().isErr());assert(Numbers::fromJson("{\\\"small\\\":1e100,\\\"large\\\":2.0}").isErr());}''', None, []),
        ('not_reflect_exposed', simple + 'void main(){assert(std::reflect::getStructInfo<Data>().isNone());}', None, []),
        ('recursive_container', '''struct Node{int64 value;List<Node> children; $std::json::Json<Node>(toJson, fromJson); }
void main(){Node root;root.value=1;Node child;child.value=2;root.children.push(child);let copied=Node::fromJson(root.toJson().unwrap()).unwrap();assert(copied.value==1&&copied.children.at(0).value==2);}''', None, []),
        ('duplicate_name', 'struct Data{@std::json::Field("same") int64 left;@std::json::Field("same") int64 right; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['重命名发生冲突'], []),
        ('duplicate_annotation', 'struct Data{@std::json::Field @std::json::Field int64 value; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['只能使用一次'], []),
        ('controlName', 'struct Data{@std::json::Field("bad\\n") int64 value; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['控制字节'], []),
        ('annotation_function', '@std::json::Field\nvoid main(){}', ['只能用于派生 Json'], []),
        ('annotation_owner', 'struct Data{@std::json::Field int64 value;}void main(){}', ['只能用于派生 Json'], []),
        ('annotation_static', 'struct Data{@std::json::Field static int64 value; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['只能用于派生 Json'], []),
        ('missing_nested', 'struct Inner{int64 value;}\nstruct Outer{Inner inner; $std::json::Json<Outer>(toJson, fromJson); }void main(){}', ['嵌套类型必须实现 std::json::Json'], []),
        ('map_key', 'struct Data{Map<int64,int64> values; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['Map 键必须是标准 String'], []),
        ('reference', 'struct Data{int64 const& value; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['不支持引用'], []),
        ('array', 'struct Data{int64 values[2]; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['不支持引用'], []),
        ('resource', 'struct Resource:Closeable{void close(){}}\nstruct Data{Resource resource; $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['不可复制资源'], []),
        ('conflict', 'struct Data{Result<String,Error> toJson(){return Result<String,Error>::Ok("{}");} $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['生成成员名称或签名冲突'], []),
        ('factory_conflict', 'struct Data{static int64 fromJson(String text){return 7;} $std::json::Json<Data>(toJson, fromJson); }void main(){}', ['生成成员名称或签名冲突'], []),
        ('no_reflection', simple + 'void main(){}', ['需要启用反射'], ['--no-reflection']),
        ('plain_no_reflection', 'void main(){assert(std::json::parse("{}").isOk());}', None, ['--no-reflection']),
    ]
    if args.case:
        cases = [case for case in cases if case[0] in args.case]
        assert len(cases) == len(set(args.case))
    env = dict(os.environ, CHALI_LANG='zh')
    with tempfile.TemporaryDirectory(prefix='json-derive-') as directory:
        work = Path(directory)
        def check(case):
            """每个用例使用独立源码、编译器进程和原生产物路径。"""
            name, code, diagnostic, flags = case
            source = work / (name + '.chl')
            source.write_text('import std::json;\nimport std::macro;\n' + code, encoding='utf-8')
            command = [str(args.chali.resolve()), 'check' if diagnostic else 'run', str(source), *flags]
            if not diagnostic:
                command += ['--out', str(work / name)]
            result = subprocess.run(command, cwd=work, env=env, capture_output=True, encoding='utf-8', timeout=240)
            if diagnostic:
                assert result.returncode != 0 and all(text in result.stdout + result.stderr for text in diagnostic), (name, result)
            else:
                assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), (name, result)
            print(name + ' 通过', flush=True)
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = [pool.submit(check, case) for case in cases]
            for future in futures:
                future.result()
    print(f'JSON 派生通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
