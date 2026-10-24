"""验证自动实例化产生的返回闭包完成登记，并保留类型与借用拒绝。"""
from compiler_path import compiler_path
from compiler_limits_cases import program, ROOT
from chali_compiler_limits import run_case
from project_temp import tempfile
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import os
from pathlib import Path


def cases():
    """覆盖显式/自动推导、复合函数类型、复制环境、容器和非法返回。"""
    declarations = '''
int64 inc(int64 x) { return x + 1; }
int64 twice(int64 x) { return x * 2; }
T fn(T) compose<T>(T fn(T) a, T fn(T) b)
{
    return fn copy(T x) { a(b(x)) };
}
T fn(T) offset<T>(T bias) { return fn copy(T x) { x + bias }; }
'''
    bodies = [
        ('implicit', '    let f = compose(inc, twice); assert(f(6) == 13);'),
        ('explicit', '    let f = compose<int64>(inc, twice); assert(f(6) == 13);'),
        ('numeric', '    let f = offset(7); assert(f(6) == 13);'),
        ('floating', '    let f = offset(1.5); assert(f(2.0) == 3.5);'),
        ('local_typed', '    int64 fn(int64) f = compose(inc, twice); assert(f(6) == 13);'),
        ('immediate', '    assert(compose(inc, twice)(6) == 13);'),
        ('list_storage', '    List<int64 fn(int64)> callbacks; callbacks.push(compose(inc, twice)); assert(callbacks[0](6) == 13);'),
        ('map_storage', '    Map<int64, int64 fn(int64)> callbacks; callbacks.put(7, compose(inc, twice)); assert(callbacks[7](6) == 13);'),
        ('closure_copy', '    let f = compose(inc, twice); let g = f; assert(g(6) == 13 && f(7) == 15);'),
        ('nested_composition', '    let f = compose(compose(inc, twice), inc); assert(f(6) == 15);'),
    ]
    result = [dict(group='generic_return', name=name, source=program(body, declarations),
                   expected='limits-ok\n', outcome='success') for name, body in bodies]
    for name, extra, body in [
        ('string_result', 'String suffix(String x) { return x + "!"; }\nString prefix(String x) { return "[" + x; }',
         '    let f = compose(suffix, prefix); assert(f("ok") == "[ok!");'),
        ('overloaded_callback', 'int64 inc(int64 x, int64 y) { return x + y; }',
         '    let f = compose(inc, twice); assert(f(6) == 13);'),
        ('overloaded_callback_types', 'String inc(String x) { return x + "!"; }',
         '    let f = compose(inc, twice); assert(f(6) == 13);'),
        ('overloaded_callback_explicit', 'int64 inc(int64 x, int64 y) { return x + y; }',
         '    let f = compose<int64>(inc, twice); assert(f(6) == 13);'),
        ('nested_factory_type', 'T applyFactory<T>((T fn(T)) fn(T fn(T), T fn(T)) factory, T fn(T) a, T fn(T) b, T x) { return factory(a, b)(x); }',
         '    assert(applyFactory<int64>(compose<int64>, inc, twice, 6) == 13);'),
    ]:
        result.append(dict(group='generic_return', name=name, source=program(body, declarations + extra),
                           expected='limits-ok\n', outcome='success'))
    for name, declaration, diagnostic in [
        ('wrong_value', 'T fn(T) bad<T>(T fn(T) a, T fn(T) b) { return 42; }', '不能隐式转换'),
        ('borrowed_local', 'T fn(T) bad<T>(T fn(T) a, T fn(T) b) { return fn(T x) { a(b(x)) }; }', 'CHL1204'),
    ]:
        result.append(dict(group='generic_return', name=name, source=program('    let f = bad(inc, twice); assert(f(6) == 13);', declarations + declaration),
                           expected='limits-ok\n', outcome='reject', diagnostic=diagnostic))
    result.append(dict(group='generic_return', name='ambiguous_callbacks', source=program(
        '    let f = compose(inc, twice); assert(f(6) == 13);',
        declarations + 'String inc(String x) { return x + "!"; }\nString twice(String x) { return x + x; }'),
        expected='limits-ok\n', outcome='reject', diagnostic='多个匹配重载'))
    return result


def main():
    """对同一输入验证两个后端，不接受仅编译通过或错误原因无关的结果。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    os.environ['CHALI_LLVM_CLANG'] = str(ROOT / 'library/llvm/toolchain/bin' / ('clang.exe' if os.name == 'nt' else 'clang'))
    work = Path(tempfile.mkdtemp(prefix='generic-function-returns-'))
    inputs = cases()
    with ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(lambda case: run_case(case, compiler, work, 180), inputs))
    (work / 'report.json').write_text(json.dumps(results, ensure_ascii=False, indent=2), encoding='utf-8')
    for case, result in zip(inputs, results):
        for backend, output in result['results'].items():
            expected = 'rejected' if case['outcome'] == 'reject' else 'pass'
            assert output['status'] == expected, (case['name'], backend, output, work)
            if case.get('diagnostic'):
                assert case['diagnostic'] in output['build']['stderr_text'], (case['name'], backend, output, work)
        print('泛型函数返回值两端通过：' + case['name'], flush=True)
    print('完整证据：' + str(work), flush=True)


if __name__ == '__main__':
    main()
