"""验证 LLVM AOT 列表值语义及用户 opcode 的 C ABI 生命周期。"""
import os
import ctypes
import _ctypes
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

NATIVE = r'''
#include <stdint.h>
#include <stdlib.h>
#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API
#endif
static int64_t values[4096], next = 1, live;
/**
 * @brief 创建独立的测试资源记录。
 * @return 新记录的句柄。
 */
API int64_t resource_new(void) { int64_t h = next++; values[h] = 17; ++live; return h; }
/**
 * @brief 复制记录并分配独立句柄。
 * @param h 源记录句柄。
 * @return 新记录句柄。
 */
API int64_t resource_copy(int64_t h) { if (!values[h]) abort(); int64_t n = resource_new(); values[n] = values[h]; return n; }
/**
 * @brief 严格销毁存活记录，重复销毁使测试失败。
 * @param h 待销毁句柄。
 */
API void resource_drop(int64_t h) { if (!values[h]) abort(); values[h] = 0; --live; }
/**
 * @brief 修改指定记录的数值。
 * @param h 存活句柄。
 * @param v 新数值。
 */
API void resource_set(int64_t h, int64_t v) { if (!values[h]) abort(); values[h] = v; }
/**
 * @brief 读取记录数值。
 * @param h 存活句柄。
 * @return 记录值。
 */
API int64_t resource_get(int64_t h) { if (!values[h]) abort(); return values[h]; }
/**
 * @brief 查询尚未销毁的记录数。
 * @return 存活资源数量。
 */
API int64_t resource_live(void) { return live; }
'''

SOURCE = '''abi "@LIB@"
{
    int64 resource_new() = "resource_new";
    int64 resource_copy(int64 handle) = "resource_copy";
    void resource_drop(int64 handle) = "resource_drop";
    void resource_set(int64 handle, int64 value) = "resource_set";
    int64 resource_get(int64 handle) = "resource_get";
    int64 resource_live() = "resource_live";
}
opcode Resource
{
    new = resource_new;
    copy = resource_copy;
    drop = resource_drop;
    int64 number;
    String label;
    int64 read() { return resource_get(self.opcode); }
    void write(int64 value) { resource_set(self.opcode, value); }
};
#NonCopyable
opcode Unique
{
    new = resource_new;
    drop = resource_drop;
    int64 number;
};
struct Pair { Resource first; Resource second; };
void exercise()
{
    Resource first = Resource::new();
    first.number = 8;
    first.label = "hello";
    Resource second = first;
    second.write(99);
    second.number = 9;
    second.label[0] = 'H';
    assert(first.read() == 17);
    assert(second.read() == 99);
    assert(first.number == 8);
    assert(first.label == "hello");
    assert(second.label == "Hello");
    Pair pair;
    pair.first = first;
    pair.second = move second;
    assert(pair.first.read() == 17);
    assert(pair.second.read() == 99);
    Unique unique;
    unique.number = 42;
    Unique moved = move unique;
    assert(moved.number == 42);
}
void main()
{
    exercise();
    assert(resource_live() == 0);
    exercise();
    assert(resource_live() == 0);
    println("AOT opcode lifecycle ok");
}
'''


def verify(invoke, compiler, cc, work):
    """对比真实 VM 与原生产物，并检查非法列表访问在执行时失败。"""
    suffix = '.exe' if os.name == 'nt' else ''
    for checked in (False, True):
        source = ROOT / 'tests/fixtures/aot_list_values.chl'
        expected = invoke([compiler, 'run', source])
        assert expected.returncode == 0, expected
        prefix = work / ('list_checked' if checked else 'list_values')
        result = invoke([compiler, 'aot', source, '--out', prefix, *(['--checked'] if checked else [])])
        assert result.returncode == 0, result
        result = invoke([prefix.with_suffix(suffix)])
        assert (result.returncode, result.stdout) == (0, expected.stdout), (result, expected)
    print('[通过] LLVM 列表整数、浮点、字符串、嵌套值、结构体、深复制和引用写回', flush=True)
    invalid = work / 'list_bounds.chl'
    invalid.write_text('''int64 index(int64 value) { return value; }
void main()
{
    List<int64> values;
    values.push(7);
    println("before list access");
    println("{}", values[index(9)]);
    println("after list access");
}
''', encoding='utf-8')
    for checked in (False, True):
        prefix = work / ('list_bounds_checked' if checked else 'list_bounds')
        result = invoke([compiler, 'aot', invalid, '--out', prefix, '--checked' if checked else '--unchecked'])
        assert result.returncode == 0, result
        result = invoke([prefix.with_suffix(suffix)])
        assert result.returncode != 0 and result.stdout == 'before list access\n' and '下标越界' in result.stderr, result
    print('[通过] LLVM 列表下标在两种模式下均按访问时机验证', flush=True)
    native = work / 'opcode_native.c'
    native.write_text(NATIVE, encoding='utf-8')
    library = work / ('opcode_native.dll' if os.name == 'nt' else 'libopcode_native.so')
    result = invoke([cc, '-O2', '-shared', *([] if os.name == 'nt' else ['-fPIC']), native, '-o', library])
    assert result.returncode == 0, result
    source = work / 'opcode_values.chl'
    source.write_text(SOURCE.replace('@LIB@', library.as_posix()), encoding='utf-8')
    expected = invoke([compiler, 'run', source])
    assert expected.returncode == 0, expected
    prefix = work / 'opcode_values'
    result = invoke([compiler, 'aot', source, '--out', prefix])
    assert result.returncode == 0, result
    result = invoke([prefix.with_suffix(suffix)])
    assert (result.returncode, result.stdout) == (0, expected.stdout), (result, expected)
    print('[通过] LLVM opcode 创建、深复制、成员、move、聚合字段及严格销毁', flush=True)

    # 无 main 的动态库必须把全局 opcode 的创建和销毁接入公开生命周期入口。
    source = work / 'opcode_library.chl'
    declaration = SOURCE.split('struct Pair', 1)[0].replace('@LIB@', library.as_posix())
    source.write_text(declaration + """
Resource shared = Resource::new();
int64 transient;
export int64 readShared()
{
    return shared.read();
}
export int64 nextScalar()
{
    transient += 1;
    return transient;
}
""", encoding='utf-8')
    prefix = work / 'opcode_library'
    result = invoke([compiler, 'aot', source, '--shared', '--out', prefix])
    assert result.returncode == 0, result
    counters = ctypes.CDLL(str(library))
    exported = ctypes.CDLL(str(prefix.with_suffix('.dll' if os.name == 'nt' else '.so')))
    counters.resource_live.restype = ctypes.c_int64
    exported.f_readShared.restype = ctypes.c_int64
    exported.f_nextScalar.restype = ctypes.c_int64
    exported.chali_init.restype = None
    exported.chali_finish.restype = None
    try:
        for _ in range(2):
            assert counters.resource_live() == 0
            exported.chali_init()
            assert counters.resource_live() == 1
            assert exported.f_readShared() == 17
            assert exported.f_nextScalar() == 1
            assert exported.f_nextScalar() == 2
            exported.chali_finish()
            assert counters.resource_live() == 0
    finally:
        close = _ctypes.FreeLibrary if os.name == 'nt' else _ctypes.dlclose
        close(exported._handle)
        close(counters._handle)
    print('[通过] 无 main 的 export 动态库全局资源初始化与清理', flush=True)
