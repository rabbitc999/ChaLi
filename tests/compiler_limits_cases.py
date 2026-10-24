"""三类代表用例：已有业务预期、组合语义，以及有界压力输入。"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

BUSINESS = [
    'collection_literals', 'map_get_option', 'map_pair_iterator', 'container_struct_map',
    'set_key_protocol', 'global_container_defaults', 'std_collections_algorithms',
    'std_errors', 'std_structured_data', 'std_csv', 'std_datetime', 'std_encoding',
    'std_utf8', 'std_binary', 'std_string_builder', 'std_text_kmp', 'std_io_extended',
    'std_file_result', 'std_file_bytes', 'std_file_strings', 'reflect_serialization',
    'std_graph', 'std_tree', 'std_linkedlist', 'std_algorithms_extra',
]
ADVANCED = [
    'generic_inference', 'generic_qualified', 'generic_inherited_constraint',
    'associated_types', 'lambda_callbacks', 'lambda_capture_modes',
    'lambda_captures', 'any_generics', 'nested_result', 'nested_enum_ownership',
    'aggregate_ownership', 'allocated_default_ownership', 'temporary_ownership',
    'recursive_callable_ownership', 'interface_view_ownership', 'block_expression',
    'match_expression', 'iter_pipeline', 'foreach_reference', 'update_compound_nested',
]


def program(body, declarations='', imports=''):
    """为独立用例添加统一入口和成功标记，断言承担结果校验。"""
    return imports + '\n' + declarations + '\nvoid main()\n{\n' + body + '\n    println("limits-ok");\n}\n'


def catalog():
    """返回固定的 75 项；每项都有源码、独立预期和允许的资源边界。"""
    cases = []
    for group, names in [('business', BUSINESS), ('advanced', ADVANCED)]:
        for name in names:
            source = ROOT / 'example/samples' / (name + '.chl')
            expected = ROOT / 'example/expected' / (name + '.txt')
            cases.append(dict(group=group, name=name, source=source.read_text(encoding='utf-8'),
                              origin=str(source.relative_to(ROOT)), expected=expected.read_text(encoding='utf-8'),
                              outcome='success'))
    functions = 'int64 inc(int64 x) { return x + 1; }\nint64 twice(int64 x) { return x * 2; }\n'
    factory = 'T fn(T) choose<T>(T fn(T) a, T fn(T) b) { return a; }\n'
    probes = [
        ('generic_function_return_explicit', factory + functions,
         '    let f = choose<int64>(inc, twice);\n    assert(f(6) == 7);', 'success'),
        ('generic_function_return_inferred', factory + functions,
         '    let f = choose(inc, twice);\n    assert(f(6) == 7);', 'success'),
        ('generic_function_return_composed', functions +
         'T fn(T) compose<T>(T fn(T) a, T fn(T) b) { return fn copy(T x) { a(b(x)) }; }\n',
         '    let f = compose(inc, twice);\n    assert(f(6) == 13);', 'success'),
        ('generic_function_return_nested', factory + functions +
         'T invoke<T>((T fn(T)) fn(T fn(T), T fn(T)) factory, T fn(T) a, T fn(T) b, T x) { return factory(a, b)(x); }\n',
         '    assert(invoke<int64>(choose<int64>, inc, twice, 6) == 7);', 'success'),
        ('generic_function_return_mismatch', functions +
         'T fn(T) wrong<T>(T fn(T) a, T fn(T) b) { return 42; }\n',
         '    let f = wrong<int64>(inc, twice);\n    println("{}", f(6));', 'reject'),
    ]
    for name, declarations, body, outcome in probes:
        cases.append(dict(group='advanced', name=name, source=program(body, declarations),
                          expected='limits-ok\n', outcome=outcome))

    pressure = [
        ('list_growth', '    List<int64> a;\n    { int64 i = 0; while (i < 20000) { a.push(i);  i = i + 1; } }\n    assert(a.len() == 20000 && a[19999] == 19999);'),
        ('list_pop', '    List<int64> a;\n    { int64 i = 0; while (i < 12000) { a.push(i);  i = i + 1; } }\n    int64 total = 0;\n    while (a.len() > 0) { total = total + a.pop(); }\n    assert(total == 71994000);'),
        ('list_copy', '    List<int64> a;\n    { int64 i = 0; while (i < 4096) { a.push(i);  i = i + 1; } }\n    { int64 j = 0; while (j < 32) { List<int64> b = a; b[0] = j; assert(a[0] == 0 && b[4095] == 4095);  j = j + 1; } }'),
        ('list_front_insert', '    List<int64> a;\n    { int64 i = 0; while (i < 1024) { a.insert(0, i);  i = i + 1; } }\n    assert(a.len() == 1024 && a[0] == 1023 && a[1023] == 0);'),
        ('map_growth', '    Map<int64, int64> a;\n    { int64 i = 0; while (i < 12000) { a.put(i, i * 3);  i = i + 1; } }\n    assert(a.len() == 12000 && a[11999] == 35997);'),
        ('map_update', '    Map<int64, int64> a;\n    { int64 i = 0; while (i < 12000) { a.put(i % 97, i);  i = i + 1; } }\n    assert(a.len() == 97 && a[68] == 11999);'),
        ('map_delete', '    Map<int64, int64> a;\n    { int64 i = 0; while (i < 4096) { a.put(i, i);  i = i + 1; } }\n    { int64 i = 0; while (i < 4096) { a.remove(i);  i = i + 1; } }\n    assert(a.len() == 0);'),
        ('map_miss', '    Map<int64, int64> a; a.put(1, 7);\n    { int64 i = 2; while (i < 20000) { assert(a.getOption(i).isNone()); i = i + 1; } }\n    assert(a[1] == 7);'),
        ('set_growth', '    Set<int64> a;\n    { int64 i = 0; while (i < 12000) { a.add(i);  i = i + 1; } }\n    assert(a.len() == 12000 && a.has(11999) != 0);'),
        ('set_duplicate', '    Set<int64> a;\n    { int64 i = 0; while (i < 20000) { a.add(i % 101);  i = i + 1; } }\n    assert(a.len() == 101 && a.has(100) != 0);'),
        ('set_delete', '    Set<int64> a;\n    { int64 i = 0; while (i < 4096) { a.add(i);  i = i + 1; } }\n    { int64 i = 0; while (i < 4096) { a.remove(i);  i = i + 1; } }\n    assert(a.len() == 0);'),
        ('string_growth', '    String a = "";\n    { int64 i = 0; while (i < 4096) { a = a + "x";  i = i + 1; } }\n    assert(a.length() == 4096);'),
        ('string_copy', '    String a = "中文与 bytes";\n    { int64 i = 0; while (i < 10000) { String b = a; assert(b == a);  i = i + 1; } }'),
        ('array_rw', '    int64 a[8192];\n    { int64 i = 0; while (i < 8192) { a[i] = i;  i = i + 1; } }\n    int64 total = 0;\n    { int64 i = 0; while (i < 8192) { total = total + a[i];  i = i + 1; } }\n    assert(total == 33550336);'),
        ('dynamic_array', '    int64 a[] = new[65536];\n    a[0] = 17; a[65535] = 29;\n    assert(a[0] + a[65535] == 46);'),
        ('nested_containers', '    List<List<int64>> a;\n    { int64 i = 0; while (i < 128) { List<int64> row; row.push(i); a.push(row);  i = i + 1; } }\n    assert(a[127][0] == 127);'),
        ('container_recreate', '    { int64 i = 0; while (i < 2000) { List<int64> a; Map<int64, int64> b; Set<int64> c; a.push(i); b.put(i, i); c.add(i); assert(a[0] == b[i] && c.has(i) != 0);  i = i + 1; } }'),
        ('long_expression', '    int64 a = ' + ' + '.join(['1'] * 512) + ';\n    assert(a == 512);'),
        ('many_locals', '\n'.join('    int64 x' + str(i) + ' = ' + str(i) + ';' for i in range(256)) + '\n    assert(x0 + x255 == 255);'),
        ('nested_blocks', '    int64 x = 0;\n' + '    {\n' * 48 + '    x = 7;\n' + '    }\n' * 48 + '    assert(x == 7);'),
    ]
    for name, body in pressure:
        cases.append(dict(group='pressure', name=name, source=program(body), expected='limits-ok\n', outcome='bounded'))
    extra = [
        ('recursion', '    assert(sum(128) == 8256);',
         'int64 sum(int64 n) { if (n == 0) { return 0; } return n + sum(n - 1); }'),
        ('call_chain', '    assert(f0(7) == 103);',
         '\n'.join('int64 f' + str(i) + '(int64 x) { return ' + ('x' if i == 95 else 'f' + str(i + 1) + '(x)') + ' + 1; }' for i in range(96))),
        ('generic_instantiations', '\n'.join('    Box<int64, ' + str(i) + '> a' + str(i) + '; a' + str(i) + '.value = ' + str(i) + ';' for i in range(48)) + '\n    assert(a47.value == 47);',
         'struct Box<T, N:int64> { T value; };'),
        ('nested_types', '    ' + 'List<' * 16 + 'int64' + '>' * 16 + ' values;\n    assert(values.len() == 0);', ''),
        ('runtime_budget', '    int64 values[] = new[16777216];\n    values[0] = 1;\n    assert(values[0] == 1);', ''),
    ]
    for name, body, declarations in extra:
        cases.append(dict(group='pressure', name=name, source=program(body, declarations), expected='limits-ok\n', outcome='bounded'))
    assert len(cases) == 75
    assert all(sum(c['group'] == group for c in cases) == 25 for group in ['business', 'advanced', 'pressure'])
    return cases
