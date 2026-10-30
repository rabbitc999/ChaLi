"""验证宏编译期诊断、字段路径、常量限制和成功断言消除。"""
import argparse
import os
import re
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行诊断正反例，并运行静态字段工具的完整示例。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    options=parser.parse_args()
    cases=[
        ('direct', '$compile_assert(true,"top");void main(){$compile_assert(2+2==4);assert($compile_assert(true));}',None),
        ('parameter', '$macro CHECK(flag:expr) -> expr\n$compile_assert(flag,"must hold");\n3\n$end_macro\nvoid main(){assert($CHECK(true)==3);}',None),
        ('item', '$macro DECLARE(name:ident) -> item\n$compile_assert(true);\nint64 name(){return 7;}\n$end_macro\n$DECLARE(seven);void main(){assert(seven()==7);}',None),
        ('unused', '$macro GOOD\n$if false\n$compile_error("not selected");\n$end_if\n42\n$end_macro\nvoid main(){assert($GOOD==42);}',None),
        ('assert_false', '$macro BAD(flag:expr)\n$compile_assert(flag,"custom failure");\n$end_macro\nvoid main(){$BAD(false);}', ['custom failure','诊断实参位置','展开宏 $BAD']),
        ('error', '$macro BAD\n$compile_error("custom " + "error");\n$end_macro\nvoid main(){$BAD;}', ['custom error','展开宏 $BAD']),
        ('field', 'struct Data{int64 first;List<int64> items;}\n$macro CHECK<T>()\n$for field in $fields<T>()\n$compile_assert(field.type.name=="int64","unsupported "+field.type.name);\n$end_for\n$end_macro\nvoid main(){$CHECK<Data>();}', ['unsupported','Data.items','序号 1']),
        ('nested', '$macro INNER(flag:expr)\n$compile_assert(flag,"nested failure");\n$end_macro\n$macro OUTER(flag:expr)\n$INNER(flag);\n$end_macro\nvoid main(){$OUTER(false);}', ['nested failure','展开宏 $INNER','展开宏 $OUTER']),
        ('dynamic_condition', 'bool side(){return true;}void main(){$compile_assert(side());}', ['编译期 bool 常量']),
        ('dynamic_message', 'String message(){return "error";}void main(){$compile_error(message());}', ['字符串常量']),
        ('arity', 'void main(){$compile_assert();}', ['需要 bool 常量']),
        ('type', 'void main(){$compile_assert(1);}', ['编译期 bool 常量']),
        ('message_type', 'void main(){$compile_assert(true,1);}', ['字符串常量']),
        ('reserved', '$macro compile_error(text)\ntext\n$end_macro\nvoid main(){}', ['内建名称']),
        ('string_constant', '$macro CHECK(text:expr) -> expr\n$compile_assert(text=="ab","joined");\ntrue\n$end_macro\nvoid main(){assert($CHECK("a"+"b"));}',None),
    ]
    cases.extend([
        ('named_string', 'void main(){const String text="ab";$compile_assert(text=="a"+"b","named");}',None),
        ('named_message', 'void main(){const String message="named failure";$compile_error(message);}', ['named failure']),
    ])
    cases.append(('expression', 'bool echo(bool value){return value;}\n$macro VALUE() -> expr\necho($compile_assert(true))\n$end_macro\nvoid main(){assert($VALUE());let pair=($compile_assert(true),false);assert(pair.0 && !pair.1);}',None))
    example_prefix=(ROOT/'example/reflection/macro_fields.chl').read_text(encoding='utf-8').split('void main()',1)[0]
    cases.extend([
        ('json_unsupported',example_prefix+'struct Unsupported{List<int64> values;}void main(){Unsupported value;$JSON_FIELDS<Unsupported>(value);}', ['JSON 示例不支持类型','Unsupported.values']),
        ('validation_unsupported',example_prefix+'struct Unsupported{List<int64> values;}void main(){Unsupported value;$VALIDATE_FIELDS<Unsupported>(value);}', ['校验示例不支持类型','Unsupported.values']),
        ('empty_schema',example_prefix+'struct Empty{}void main(){Empty value;assert($JSON_FIELDS<Empty>(value)=="{}");assert($VALIDATE_FIELDS<Empty>(value));}',None),
        ('field_parameter_name',example_prefix+'struct Data{int64 value;}void main(){Data value;value.value=4;assert($JSON_FIELDS<Data>(value)=="{\\"value\\":4}");}',None),
    ])
    cases.extend([
        ('nested_field_path', 'struct Outer{int64 a;}struct Inner{List<int64> bad;}\n$macro CHECK()\n$for outer in $fields<Outer>()\n$for inner in $fields<Inner>()\n$compile_error("nested fields");\n$end_for\n$end_for\n$end_macro\nvoid main(){$CHECK();}', ['nested fields','Outer.a','Inner.bad']),
        ('runtime_const_message', 'String message(){println("must not execute");return "error";}void main(){const String text=message();$compile_error(text);}', ['字符串常量']),
    ])
    cases.extend([
        ('global_string', 'const String MESSAGE="global";$compile_assert(MESSAGE=="glo"+"bal","global const");void main(){}',None),
        ('global_message', 'const String MESSAGE="global failure";$compile_error(MESSAGE);void main(){}', ['global failure']),
        ('constant_cycle', 'const bool A=B;const bool B=A;$compile_assert(A);void main(){}', ['循环依赖']),
        ('mutable_shadow', 'const bool FLAG=true;void main(){bool FLAG=false;$compile_assert(FLAG);}', ['运行时变量']),
        ('imported_constant', 'import constants;\n$compile_assert(constants::LABEL=="imported");void main(){}',None),
    ])
    cases.append(('constant_width', 'const int8 SIGNED=0xff;const uint8 UNSIGNED=255;$compile_assert(SIGNED==cast<int8>(-1));$compile_assert(UNSIGNED==cast<uint8>(255));void main(){}',None))
    cases.extend([
        ('macro_local_const', '$macro LOCAL(flag:expr) -> expr\nconst bool allowed=flag;\n$compile_assert(allowed,"local");\n3\n$end_macro\nvoid main(){assert($LOCAL(true)==3);}',None),
        ('macro_local_shadow', 'const bool FLAG=true;\n$macro BAD()\nbool FLAG=false;\n$compile_assert(FLAG);\n$end_macro\nvoid main(){$BAD();}', ['运行时局部变量']),
        ('repeat_local_const', '$macro REPEAT(...xs:expr) -> expr\nconst bool allowed=true;\nint64 total=0;\n$for x in xs\n$if allowed\ntotal+=x;\n$end_if\n$end_for\ntotal\n$end_macro\nvoid main(){assert($REPEAT(1,2)==3);}',None),
        ('repeat_local_shadow', 'const bool FLAG=true;\n$macro BAD(...xs:expr)\nbool FLAG=false;\n$for x in xs\n$if FLAG\nx;\n$end_if\n$end_for\n$end_macro\nvoid main(){$BAD(1);}', ['运行时变量']),
    ])
    if options.case:cases=[case for case in cases if case[0] in options.case]
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-diagnostics-') as directory:
        work=Path(directory)
        for name,source,diagnostics in cases:
            if name=='imported_constant':(work/'constants.chl').write_text('module constants;\nconst String LABEL=\"imported\";',encoding='utf-8')
            path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=(result.returncode!=0 and result.stdout=='' and all(text in result.stderr for text in diagnostics)) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
            location=re.search(r'诊断实参位置 (.+):(\d+):(\d+)',result.stderr)
            if location:
                valid=valid and Path(location[1]).name==path.name and 1<=int(location[2])<=source.count('\n')+1
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
        if not options.case:
            example=ROOT/'example/reflection/macro_fields.chl'
            result=subprocess.run([str(options.chali.resolve()),'run',str(example),'--no-reflection','--out',str(work/'fields')],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            expected='name = Ada "A"\nscore = 95\nactive = true\njson = {"name":"Ada \\"A\\"","score":95,"active":true}\nvalid = true\n'
            if (result.returncode,result.stdout,result.stderr)!=(0,expected,''):failures.append(('example',result.returncode,result.stdout,result.stderr))
    assert not failures,failures
    print(f'宏编译期诊断通过：{len(cases)} 项'+('，字段打印／校验／JSON 原生示例通过（禁用运行期反射）' if not options.case else ''))


if __name__=='__main__':
    main()
