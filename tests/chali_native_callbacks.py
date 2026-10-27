"""用 C 宿主验证 AOT 同步回调：具名函数、闭包捕获、标量/引用参数、重入与 panic 传播。

1.0.8 起没有字节码宿主，回调走 `#abi` 声明 + C 动态库：ChaLi 侧把函数值当机器函数指针传给 C，
C 侧直接调用，返回值按声明的标量类型回传。
"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import shutil
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
EXPECTED = 'callbacks: named inline copy move float32 bool void reference higher-order reentry OK\n'

HOST_C = r"""
#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32)
#define CHALI_ABI_EXPORT __declspec(dllexport)
#else
#define CHALI_ABI_EXPORT
#endif

CHALI_ABI_EXPORT int64_t hostApply(int64_t (*function)(int64_t), int64_t value) { return function(value); }
CHALI_ABI_EXPORT float hostFloat(float (*function)(float), float value) { return function(value); }
CHALI_ABI_EXPORT bool hostBool(bool (*function)(bool), bool value) { return function(value); }
CHALI_ABI_EXPORT void hostNotify(void (*function)(int64_t), int64_t value) { function(value); }
CHALI_ABI_EXPORT void hostUpdate(int64_t (*function)(int64_t), int64_t *value) { *value = function(*value); }
"""

DEMO = """#abi("C", "{library}", "hostApply")
int64 hostApply(int64 fn(int64) function, int64 value);
#abi("C", "{library}", "hostFloat")
float32 hostFloat(float32 fn(float32) function, float32 value);
#abi("C", "{library}", "hostBool")
bool hostBool(bool fn(bool) function, bool value);
#abi("C", "{library}", "hostNotify")
void hostNotify(void fn(int64) function, int64 value);
#abi("C", "{library}", "hostUpdate")
void hostUpdate(int64 fn(int64) function, int64 mut& value);

int64 notifications = 0;

int64 triple(int64 value) {{ return value * 3; }}
void notify(int64 value) {{ notifications += value; }}
int64 increment(int64 value) {{ return value + 1; }}
int64 applyTwice(int64 fn(int64) inner, int64 value)
{{
    return hostApply(inner, hostApply(inner, value));
}}
int64 reenter(int64 depth)
{{
    if (depth == 0) {{ return 1; }}
    return hostApply(reenter, depth - 1) + 1;
}}

void main()
{{
    assert(hostApply(triple, 7) == 21);
    assert(hostApply(int64 fn(int64 x) {{ x + 1 }}, 7) == 8);
    int64 offset = 5;
    let captured = int64 fn copy(int64 x) {{ x + offset }};
    assert(hostApply(captured, 7) == 12);
    int64 moved = 9;
    let owned = int64 fn move(int64 x) {{ x + moved }};
    assert(hostApply(owned, 7) == 16);
    assert(hostFloat(float32 fn(float32 x) {{ x + 0.5f }}, 1.25f) == 1.75f);
    assert(hostBool(bool fn(bool value) {{ !value }}, false));
    hostNotify(notify, 4);
    hostNotify(void fn(int64 value) {{ notify(value + 1); }}, 5);
    assert(notifications == 10);
    int64 counter = 6;
    hostUpdate(increment, &counter);
    assert(counter == 7);
    assert(applyTwice(triple, 7) == 63);
    assert(applyTwice(int64 fn(int64 x) {{ x + 1 }}, 7) == 9);
    assert(hostApply(reenter, 12) == 13);
    println("{expected}");
}}
"""

PANIC = """#abi("C", "{library}", "hostApply")
int64 hostApply(int64 fn(int64) function, int64 value);

void main()
{{
    int64 got = hostApply(int64 fn(int64 x) {{ panic("ChaLi callback panic"); return x; }}, 1);
    println("unexpected callback success {{}}", got);
}}
"""


def main():
    """构建宿主动态库、跑通回调组合，并确认回调里的 panic 会穿出 C 帧到顶层。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    cc = str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else shutil.which('clang')
    cc = os.environ.get('CHALI_LLVM_CLANG') or cc
    assert cc, '找不到 C 编译器：设置 CHALI_LLVM_CLANG 或安装 clang'

    with tempfile.TemporaryDirectory(prefix='chali-native-callbacks-') as directory:
        work = Path(directory)

        def invoke(command, **kwargs):
            """执行编译或测试程序，保留完整诊断。"""
            return subprocess.run(list(map(str, command)), env=env, capture_output=True, text=True,
                                  encoding='utf-8', errors='replace', timeout=300, **kwargs)

        (work / 'host.c').write_text(HOST_C, encoding='utf-8')
        library = work / ('host.dll' if os.name == 'nt' else 'host.so')
        result = invoke([cc, '-shared', '-O3', '-o', library, work / 'host.c'])
        assert result.returncode == 0, result.stderr

        # 动态库按源码目录解析相对路径，exe 与库同目录，运行时不需要额外 PATH。
        demo = work / 'demo.chl'
        demo.write_text(DEMO.format(library=library.name, expected=EXPECTED.strip()), encoding='utf-8')
        exe = work / 'demo'
        result = invoke([compiler, 'aot', demo, '--out', exe, '--llvm', cc])
        assert result.returncode == 0, result.stdout + result.stderr
        result = invoke([exe.with_suffix('.exe' if os.name == 'nt' else '')], cwd=work)
        assert result.returncode == 0 and result.stdout == EXPECTED and not result.stderr, result

        panic = work / 'panic.chl'
        panic.write_text(PANIC.format(library=library.name), encoding='utf-8')
        panicExe = work / 'panic'
        result = invoke([compiler, 'aot', panic, '--out', panicExe, '--llvm', cc])
        assert result.returncode == 0, result.stdout + result.stderr
        result = invoke([panicExe.with_suffix('.exe' if os.name == 'nt' else '')], cwd=work)
        assert result.returncode != 0, result
        assert 'ChaLi callback panic' in result.stderr, result.stderr
        assert 'unexpected callback success' not in result.stdout, result.stdout
    print('AOT sync callbacks: named, inline, copy/move capture, float32/bool/void, mut&, higher-order, reentry and panic passed')


if __name__ == '__main__':
    main()
