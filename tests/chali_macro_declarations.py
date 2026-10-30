"""验证显式宏结果类别与模块声明生成。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行生成声明，并确认错误位置、结果类别和冲突被拒绝。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    definitions = '''
$macro FUNCTION(name:ident) -> item
int64 name(){return 42;}
$end_macro
$macro RECORD<T>(name:ident) -> item
struct name {T value;}
$end_macro
$macro ID<T>(name:ident) -> item
T name(T value){return value;}
$end_macro
$macro CHOICE(name:ident) -> item
enum name {Value(int64 value), Empty}
$end_macro
$macro API(name:ident) -> item
interface name {int64 get();}
$end_macro
$macro PRIVATE(name:ident) -> item
int64 helper(){return 42;}
int64 name(){return helper();}
$end_macro
$macro EXPR(x:value) -> expr
x+x
$end_macro
$macro STMT(mut &x:value) -> stmt
x+=1;
$end_macro
'''
    cases = [('declarations', definitions + '''
$FUNCTION(answer);
$ID<int64>(identity);
$ID<String>(textIdentity);
$RECORD<int64>(Record);
$CHOICE(Choice);
$API(Read);
$PRIVATE(first);
$PRIVATE(second);
struct Reader:Read {int64 get(){return 42;}}
int64 helper(){return 7;}
void main(){
assert(answer()==42 && first()==42 && second()==42 && helper()==7);
Record record;record.value=identity(42);assert(record.value==42);
assert(textIdentity("hello")=="hello");
Choice choice=Choice::Value(42);
assert(match(choice){Choice::Value(v)=>{v} Choice::Empty=>{0}}==42);
Reader reader;Read read=reader;assert(read.get()==42);
int64 n=$EXPR(21);$STMT(n);assert(n==43);
}
''', None),
('nested', '''
$macro INNER(name:ident) -> item
int64 name(){return 42;}
$end_macro
$macro OUTER(name:ident) -> item
$INNER(name);
$end_macro
$OUTER(answer);
void main(){assert(answer()==42);}
''', None),
('private_types', """
$macro PRIVATE_TYPE(name:ident) -> item
struct Hidden {int64 value;}
enum HiddenChoice {Value(int64 value), Empty}
int64 name(){Hidden value;value.value=42;HiddenChoice choice=HiddenChoice::Value(value.value);return match(choice){HiddenChoice::Value(v)=>{v} HiddenChoice::Empty=>{0}};}
$end_macro
$PRIVATE_TYPE(first);
$PRIVATE_TYPE(second);
struct Hidden {String text;}
void main(){assert(first()==42 && second()==42);}
""", None),
('imported', """
import helpers;
$helpers::EXPORTED(answer);
$helpers::RECORD(PublicRecord);
int64 hidden(){return 7;}
void main(){assert(answer()==42 && hidden()==7);PublicRecord value;value.item=42;assert(value.item==42);}
""", None),
('duplicate', definitions+'$FUNCTION(answer);\n$FUNCTION(answer);\nvoid main(){}', '重复'),
('duplicate_type', definitions+'$RECORD<int64>(Record);\n$RECORD<int64>(Record);\nvoid main(){}', '重名'),
('body_check', '$macro BAD(name:ident) -> item\nint64 name(){return missing;}\n$end_macro\n$BAD(answer);\nvoid main(){}', '没声明'),
('missing_type', '$macro RECORD<T>(name:ident) -> item\nstruct name{T value;}\n$end_macro\n$RECORD(Record);\nvoid main(){}', '无法推导'),
('item_in_function', definitions+'void main(){$FUNCTION(answer);}', '声明位置'),
('stmt_in_expression', definitions+'void main(){int64 n=0;int64 x=$STMT(n);}', '表达式位置'),
('value_parameter', '$macro BAD(x:value) -> item\nint64 f(){return x;}\n$end_macro\nvoid main(){}', '显式标注语法类别'),
('bad_category', '$macro BAD() -> unknown\n0\n$end_macro\nvoid main(){}', '宏结果类别'),
('bad_shape', '$macro BAD() -> stmt\n1\n$end_macro\nvoid main(){$BAD();}', '显式类别'),
('cycle', '$macro LOOP() -> item\n$LOOP();\n$end_macro\n$LOOP();\nvoid main(){}', '循环依赖'),
('invalid_item', '$macro BAD() -> item\nint64 value=1;\n$end_macro\n$BAD();\nvoid main(){}', '只能生成'),
('unused_import', '$macro BAD() -> item\nimport helpers;\n$end_macro\nvoid main(){}', 'import 必须'),
('unused_module', '$macro BAD() -> item\nmodule unexpected;\n$end_macro\nvoid main(){}', 'module 声明必须'),
('legacy_at_top', '$macro OLD()\n1\n$end_macro\n$OLD();\nvoid main(){}', '声明位置'),
]
    failures = []
    with tempfile.TemporaryDirectory(prefix='macro-declarations-') as directory:
        (Path(directory)/'helpers.chl').write_text('module helpers;\nstruct HelperType{int64 value;}\nint64 hidden(){return 42;}\n$macro EXPORTED(name:ident) -> item\nint64 name(){return hidden();}\n$end_macro\n$macro RECORD(name:ident) -> item\nstruct name{int64 item;}\n$end_macro\n', encoding='utf-8')
        for name, source, diagnostic in cases:
            path = Path(directory)/f'{name}.chl'
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(options.chali.resolve()), 'check' if diagnostic else 'run', str(path), '--out', str(Path(directory)/name)], cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, encoding='utf-8', timeout=180)
            if diagnostic:
                if result.returncode == 0 or diagnostic not in result.stderr:
                    failures.append((name, result))
            else:
                if (result.returncode, result.stdout, result.stderr) != (0, '', ''):
                    failures.append((name, result))
    assert not failures, failures
    print(f'宏声明生成通过：{len(cases)} 项')


if __name__ == '__main__':
    main()
