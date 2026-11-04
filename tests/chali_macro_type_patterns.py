"""验证宏类型比较、通配实参、子字段和递归 JSON 生成。"""
import argparse
import json
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / 'tests' / 'fixtures'


def main():
    """编译运行正例，核对错误类型和惰性分支的诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    patterns = (FIXTURES / 'macro_type_patterns.chl').read_text(encoding='utf-8')
    nested = (FIXTURES / 'macro_nested_json.chl').read_text(encoding='utf-8')

    with tempfile.TemporaryDirectory(prefix='macro-type-patterns-') as directory:
        work = Path(directory)

        def invoke(name, text, diagnostic=None):
            """在独立目录执行一个用例并核对结果。"""
            source = work / (name + '.chl')
            source.write_text(text, encoding='utf-8')
            command = [str(compiler), 'run' if diagnostic is None else 'check', str(source)]
            if diagnostic is None:
                command += ['--out', str(work / name)]
            result = subprocess.run(command, cwd=work, env=dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1'),
                                    capture_output=True, encoding='utf-8', timeout=180)
            if diagnostic is None:
                assert result.returncode == 0 and not result.stderr, (name, result)
            else:
                assert result.returncode and diagnostic in result.stderr, (name, result)
            return result.stdout

        invoke('patterns', patterns)
        output = invoke('nested', nested)
        leaf = {'count': 42, 'text': 'a"\\\n', 'enabled': True, 'empty': {}}
        assert json.loads(output) == {'middle': {'inner': leaf}, 'items': [5, 8], 'unique': ['only'],
                                      'byName': {'b"ox': [leaf]}, 'emptyList': [], 'emptySet': [],
                                      'emptyMap': {}}, output
        invoke('unsupported_json', nested.replace('int64 count;', 'float64 count;'), '普通结构体')
        definitions = nested[:nested.index('\nstruct Empty')]
        invoke('unsupported_map_key', definitions + '''
struct Invalid { Map<int64, bool> value; }
void main() { Invalid value; $JSON_VALUE(Invalid)[value]; }
''', 'JSON Map 的键必须是 String')
        children = '''struct Leaf<T> { T first; T second; }
struct Root { Leaf<int64> leaf; }
$macro READ_CHILDREN<T>(&value:value) -> expr
int64 result = 0;
$for field in $fields<T>()
$compile_assert(field.chlids.count == 2 && field.chlids[0].name == "first");
$for child in field.chlids
result = result * 10 + $member($member(value, field), child);
$end_for
$end_for
result
$end_macro
void main() { Root value; value.leaf.first = 2; value.leaf.second = 7; assert($READ_CHILDREN<Root>(value) == 27); }
'''
        invoke('children', children)
        invoke('type_fields', children.replace('field.chlids', 'field.type.fields'))
        invoke('shadowed_string', '''struct String { int64 number; }
struct Root { String custom; std::core::String standard; }
$macro CHECK<T>() -> expr
int64 result = 0;
$for field in $fields<T>()
$if field.type == String
result += 1;
$elif field.type == std::core::String
result += 2;
$else
$compile_error("unrecognized string type");
$end_if
$end_for
result
$end_macro
void main() { assert($CHECK<Root>() == 3); }
''')
        for module in ('left', 'right'):
            (work / (module + '.chl')).write_text(f'module {module};\nstruct Box<T> {{ T value; }}\n', encoding='utf-8')
        scoped = '''import left;
import right;
struct Root { left::Box<int64> field; }
$macro CHECK<T>() -> expr
int64 result = 0;
$for field in $fields<T>()
$if field.type == left::Box<?> && field.type != right::Box<?>
$compile_assert(field.genericName == "left::Box");
result = 1;
$elif field.type == Unknown<?>
result = -1;
$end_if
$end_for
result
$end_macro
void main() { assert($CHECK<Root>() == 1); }
'''
        invoke('qualified', scoped)
        invoke('ambiguous', scoped.replace('left::Box<?> && field.type != right::Box<?>', 'Box<?>'), '多个可见定义')
        invoke('unknown', scoped.replace('left::Box<?> && field.type != right::Box<?>', 'Unknown<?>'), '没有声明的泛型类型')
        invoke('ordinary_wildcard', 'struct Box<T>{T value;} void main(){Box<?> value;}', '类型通配符')
    print('macro type patterns, concrete arguments, child fields and recursive JSON passed')


if __name__ == '__main__':
    main()
