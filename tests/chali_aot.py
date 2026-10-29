"""验证 AOT 后端：四元式 → LLVM 中间表示 → 原生产物，输出与解释执行逐行一致。

覆盖五件事：

1. `chali aot` 编出的 exe 与 `chali run` 的 stdout 一致；
2. 默认保留越界及指针检查；仅显式 `--unchecked` 用于性能对照；
3. `--emit-llvm` 只出 LLVM 模块，不调编译器；
4. **AOT 调外部 C**：`abi` 块声明的函数在产物里变成 extern 调用并链接动态库；
5. **外部 C 调 AOT**：`--shared --export` 出动态库与头文件，C 驱动能调进去。
"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
import shutil
from project_temp import tempfile
import re
from chali_aot_values import verify as verify_values

ROOT = Path(__file__).resolve().parents[1]

NATIVE_C = r"""
#if defined(_WIN32)
#define CHALI_ABI_EXPORT __declspec(dllexport)
#else
#define CHALI_ABI_EXPORT
#endif
CHALI_ABI_EXPORT int aot_add(int left, int right) { return left + right; }
CHALI_ABI_EXPORT double aot_scale(double value) { return value * 1.5; }
CHALI_ABI_EXPORT unsigned int aot_high(void) { return 0xffffffffu; }
"""

ABI_SOURCE = """#abi("C", "{library}", "aot_add")
int32 aot_add(int32 left, int32 right);
#abi("C", "{library}", "aot_add")
int32 aot_sum(int32 left, int32 right);
#abi("C", "{library}", "aot_scale")
float64 aot_scale(float64 value);
#abi("C", "{library}", "aot_high")
uint32 aot_high();

void main()
{{
    int64 got = (int64)aot_add((int32)20, (int32)22);
    println("aot -> c: {{}}", got);
    println("abi values: {{}} {{}}", aot_scale(2.5), (uint64)aot_high());
    println("alias: {{}}", aot_sum((int32)19, (int32)23));
}}
"""

EXPORT_SOURCE = """int64 makeInitial()
{
    return 7;
}

int64 initial = makeInitial();

int64 hidden(int64 value)
{
    return value + initial;
}

export int64 add(int64 left, int64 right)
{
    return hidden(left) + right - 7;
}
"""

DRIVER_C = r"""
#include <stdio.h>
#include "exported.h"
int main(void)
{
    chali_init();
    printf("c -> aot: %lld\n", (long long)f_add(20, 22));
    chali_finish();
    return 0;
}
"""


def main():
    """跑 AOT 后端回归。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    cc = str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else shutil.which('clang')
    cc = os.environ.get('CHALI_LLVM_CLANG') or cc
    with tempfile.TemporaryDirectory(prefix='chali-aot-') as directory:
        work = Path(directory)

        def invoke(command, cwd=None, **kwargs):
            """执行编译或测试程序，捕获可比较的输出。"""
            return subprocess.run(list(map(str, command)), cwd=cwd or ROOT, env=env, capture_output=True,
                                  text=True, encoding='utf-8', errors='replace', timeout=300, **kwargs)

        # 1) 产物与解释执行一致
        expected = (ROOT / 'example/expected/aot_basic.txt').read_text(encoding='utf-8')
        sample = ROOT / 'example/samples/aot_basic.chl'
        result = invoke([compiler, 'run', sample])
        assert (result.returncode, result.stdout, result.stderr) == (0, expected, ''), result
        print('[通过] aot_basic 解释执行', flush=True)
        if cc is None:
            print('[跳过] 没有 C 编译器，AOT 产物相关检查不跑', flush=True)
            return
        exe = work / 'aot_basic'
        result = invoke([compiler, 'aot', sample, '--out', exe, '--llvm', cc])
        assert result.returncode == 0, (result.stdout, result.stderr)
        product = exe.with_suffix('.exe' if os.name == 'nt' else '')
        assert product.exists(), 'AOT 没产出可执行文件'
        result = invoke([product])
        assert (result.returncode, result.stdout) == (0, expected), result
        print('[通过] aot_basic AOT 产物与解释执行逐行一致', flush=True)

        # 2) 默认检查与 VM 一致；无检查只作为显式对照配置。
        opened = (exe.with_suffix('.ll')).read_text(encoding='utf-8')
        assert re.search(r'define internal i64 @f_(?:v)?\d+_program__fib\(', opened)
        assert all(not exe.with_suffix(suffix).exists() for suffix in ['.quad', '.data.bin', '.layout'])
        assert not exe.with_suffix('.c').exists() and not exe.with_suffix('.cpp').exists()
        assert not re.search(r'%s_.*__temp_.*= alloca', opened), '计算临时量不能占声明局部的帧槽'
        # 同一四元式定义配合 Linux 运行时必须能生成真正的 ELF 目标文件。
        module_data = re.search(r'^declare i64 @chali_host_call\(', opened, re.M)
        assert module_data, 'LLVM 模块缺少初始化数据定义'
        linux_module = work / 'linux.ll'
        linux_runtime = (ROOT / 'source/aot/runtime_linux.ll').read_text(encoding='utf-8')
        linux_runtime = re.sub(r'^@chali_(?:data_size|heap_size|data_image) = external .*\n', '', linux_runtime, flags=re.M)
        linux_module.write_text(linux_runtime + opened[module_data.start():], encoding='utf-8')
        linux_object = work / 'linux.o'
        result = invoke([cc, '--target=x86_64-unknown-linux-gnu', '-O2', '-c', linux_module, '-o', linux_object])
        assert result.returncode == 0 and linux_object.read_bytes()[:4] == b'\x7fELF', result
        print('[通过] Linux x64 LLVM 模块生成 ELF 目标文件', flush=True)
        assert 'checks: off' in opened
        unchecked_prefix = work / 'aot_checked'
        result = invoke([compiler, 'aot', sample, '--out', unchecked_prefix, '--llvm', cc, '--checked'])
        assert result.returncode == 0, (result.stdout, result.stderr)
        unchecked = unchecked_prefix.with_suffix('.ll').read_text(encoding='utf-8')
        assert 'checks: on' in unchecked
        # 运行时里本来就定义着 `chali_bounds`，所以比的是**调用点**的条数
        assert opened.count('@chali_check(') < unchecked.count('@chali_check('), '--checked 应该多出检查调用'
        print('[通过] AOT 默认 unchecked，显式 --checked 增加检查', flush=True)

        # 3) `--emit-llvm` 只出 LLVM 模块
        emit_only = work / 'aot_emit'
        result = invoke([compiler, 'aot', sample, '--out', emit_only, '--llvm', cc, '--emit-llvm'])
        assert result.returncode == 0, (result.stdout, result.stderr)
        assert (emit_only.with_suffix('.ll')).exists()
        assert not (emit_only.with_suffix('.exe' if os.name == 'nt' else '')).exists(), '--emit-llvm 不该出可执行文件'
        print('[通过] --emit-llvm 只出 LLVM 模块', flush=True)
        mode_only = work / 'mode_emit'
        result = invoke([compiler, '--mode', 'aot', sample, '--out', mode_only, '--emit-llvm'])
        assert result.returncode == 0, (result.stdout, result.stderr)
        assert mode_only.with_suffix('.ll').read_bytes() == emit_only.with_suffix('.ll').read_bytes()
        # 模块导出完全由编译器完成，不依赖本机 LLVM 工具可执行。
        result = invoke([compiler, 'aot', sample, '--out', work / 'without_tool', '--emit-llvm',
                         '--llvm', work / 'missing-clang'])
        assert result.returncode == 0, result
        result = invoke([compiler, 'aot', sample, '--llvm'])
        assert result.returncode == 2 and '缺少取值' in result.stderr, result
        print('[通过] --mode aot 与缺少参数诊断', flush=True)
        result = invoke([compiler, 'aot', sample, '--emit-llvm', '--shared', '--export', 'missing_function',
                         '--out', work / 'missing_export'])
        assert result.returncode != 0 and '找不到要导出的函数' in result.stderr, result


        # 4) AOT 调外部 C：abi 块 → extern 调用 + 链接动态库
        native = work / 'native.c'
        native.write_text(NATIVE_C, encoding='utf-8')
        library = work / ('native.dll' if os.name == 'nt' else 'native.so')
        build = subprocess.run([cc, '-shared', '-O2', '-o', str(library), str(native)],
                               capture_output=True, text=True)
        assert build.returncode == 0, build.stderr
        abi = work / 'abi_use.chl'
        abi.write_text(ABI_SOURCE.format(library=str(library).replace('\\', '/')), encoding='utf-8')
        # Windows 加载器按「模块目录 → 系统目录 → 当前目录 → PATH」找导入的 DLL；这一步在
        # 源码所在目录里跑，才和用户 `chali run` 时 DLL 与源码同级的实际顺序一致。
        result = invoke([compiler, 'run', abi], cwd=work)
        expected_abi = result.stdout
        assert result.returncode == 0 and expected_abi == 'aot -> c: 42\nabi values: 3.75 4294967295\nalias: 42\n', result
        abi_exe = work / 'abi_use'
        result = invoke([compiler, 'aot', abi, '--out', abi_exe, '--llvm', cc])
        assert result.returncode == 0, (result.stdout, result.stderr)
        abi_llvm = abi_exe.with_suffix('.ll').read_text(encoding='utf-8')
        encoded_symbol = ''.join(f'\\{byte:02X}' for byte in b'aot_add')
        assert '@chali_host_abi_symbol(' in abi_llvm and encoded_symbol in abi_llvm, '动态 ABI 缺少按库解析的符号入口'
        assert 'declare i32 @"aot_add"(' not in abi_llvm, '动态库调用不能退回无路径的裸导入'
        result = invoke([abi_exe.with_suffix('.exe' if os.name == 'nt' else '')])
        assert result.returncode == 0 and result.stdout == expected_abi, result
        print('[通过] AOT 产物调用外部 C 函数', flush=True)

        # 5) 外部 C 调 AOT：--shared --export 出动态库与带形参的头文件
        exported = work / 'exported.chl'
        exported.write_text(EXPORT_SOURCE, encoding='utf-8')
        shared = work / 'exported'
        result = invoke([compiler, 'aot', exported, '--out', shared, '--llvm', cc, '--shared'])
        assert result.returncode == 0, (result.stdout, result.stderr)
        header = (shared.with_suffix('.h')).read_text(encoding='utf-8')
        assert 'int64_t f_add(int64_t' in header, header
        assert 'void chali_init(void);' in header
        assert 'f_hidden(' not in header, header
        missing_main = invoke([compiler, 'aot', exported, '--emit-llvm', '--out', work / 'needs-main'])
        assert missing_main.returncode != 0 and 'main' in missing_main.stderr, missing_main
        ordinary = work / 'explicit.chl'
        ordinary.write_text('int64 plain(int64 x) { return x + 1; }\nvoid main() {}\n', encoding='utf-8')
        explicit = invoke([compiler, 'aot', ordinary, '--shared', '--emit-llvm', '--export', 'plain', '--out', work / 'explicit'])
        assert explicit.returncode == 0, explicit
        assert 'int64_t f_plain(' in (work / 'explicit.h').read_text(encoding='utf-8')
        driver = work / 'driver.c'
        driver.write_text(DRIVER_C, encoding='utf-8')
        driver_exe = work / ('driver.exe' if os.name == 'nt' else 'driver')
        suffix = '.dll' if os.name == 'nt' else '.so'
        build = subprocess.run([cc, '-O2', '-std=c99', '-I', str(work), '-o', str(driver_exe),
                                str(driver), str(shared.with_suffix(suffix))],
                               capture_output=True, text=True)
        assert build.returncode == 0, build.stderr
        result = subprocess.run([str(driver_exe)], capture_output=True, text=True, timeout=120)
        assert result.stdout.strip() == 'c -> aot: 42', result
        print('[通过] 外部 C 调用 AOT 导出的函数', flush=True)

        named = work / 'named.chl'
        named.write_text('''#export("named_add")
int64 addNumbers(int64 left, int64 right) { return left + right; }
#export("named_scale")
float64 scaleNumber(float64 value) { return value * 1.5; }
''', encoding='utf-8')
        named_prefix = work / 'named'
        built = invoke([compiler, 'aot', named, '--shared', '--out', named_prefix])
        assert built.returncode == 0, built
        named_driver = work / 'named_driver.c'
        named_driver.write_text('''#include "named.h"
/**
 * @brief 从 C 调用具名整数和浮点导出，并核对模块生命周期。
 * @return 值和清理均正确时返回零。
 */
int main(void) {
    chali_init();
    int correct = named_add(20, 22) == 42 && named_scale(2.0) == 3.0;
    chali_finish();
    return correct ? 0 : 1;
}
''', encoding='utf-8')
        named_exe = work / ('named_driver.exe' if os.name == 'nt' else 'named_driver')
        built = invoke([cc, '-O2', '-std=c99', named_driver, named_prefix.with_suffix(suffix), '-o', named_exe])
        assert built.returncode == 0, built
        result = invoke([named_exe])
        assert result.returncode == 0, result
        print('[通过] 具名 #export 的整数和浮点 ABI 适配独立编译与执行', flush=True)

        numeric = work / 'numeric.chl'
        numeric.write_text('''float32 half32(float32 value)
{
    return value / (float32)2;
}

void main()
{
    uint64 high = (uint64)-1;
    println("unsigned: {} {}", high / (uint64)3, high % (uint64)3);
    int8 small = (int8)258;
    uint8 byte = (uint8)-1;
    println("narrow: {} {}", (int64)small, (int64)byte);
    float32 values[2];
    values[0] = (float32)7.25;
    values[1] = half32(values[0]);
    println("float32: {} {}", values[0], values[1]);
    int16 items[2];
    items[0] = (int16)-1234;
    items[1] = (int16)30000;
    println("int16: {} {}", items[0], items[1]);
}
''', encoding='utf-8')
        expected_numeric = invoke([compiler, 'run', numeric])
        assert expected_numeric.returncode == 0, expected_numeric
        spaced = work / 'LLVM space path'
        spaced.mkdir()
        numeric_prefix = spaced / 'numeric'
        result = invoke([compiler, 'aot', numeric, '--out', numeric_prefix])
        assert result.returncode == 0, result
        result = invoke([numeric_prefix.with_suffix('.exe' if os.name == 'nt' else '')])
        assert (result.returncode, result.stdout) == (0, expected_numeric.stdout), (result, expected_numeric)
        print('[通过] LLVM 无符号除法、窄整数、float32 内存与空格路径', flush=True)

        bounds = work / 'bounds.chl'
        bounds.write_text('''int64 offset(int64 value) { return value; }
void main()
{
    int64 values[2];
    int64 index = offset(3);
    println("{}", values[index]);
}
''', encoding='utf-8')
        bounds_prefix = work / 'bounds'
        result = invoke([compiler, 'aot', bounds, '--checked', '--out', bounds_prefix])
        assert result.returncode == 0, result
        result = invoke([bounds_prefix.with_suffix('.exe' if os.name == 'nt' else '')])
        assert result.returncode != 0 and '越界' in result.stderr, result
        print('[通过] LLVM 检查模式越界按实际访问时机失败', flush=True)
        verify_values(invoke, compiler, cc, work)


if __name__ == '__main__':
    main()
