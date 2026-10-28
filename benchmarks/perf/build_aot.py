"""将同一份 ChaLi 计算函数导出为 LLVM 模块，由独立 C 计时壳测量原生产物。"""
import argparse
import os
import platform
from pathlib import Path
import re
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent


def llvm_compiler():
    """按显式环境、仓库工具和 PATH 查找 LLVM 驱动。"""
    explicit = os.environ.get('CHALI_LLVM_CLANG')
    if explicit:
        return explicit
    host = {'Darwin': 'macos', 'Linux': 'linux'}.get(platform.system(), platform.system().lower())
    arch = {'amd64': 'x64', 'x86_64': 'x64', 'aarch64': 'arm64', 'arm64': 'arm64'}.get(platform.machine().lower(), platform.machine().lower())
    relative = 'toolchain/bin/clang.exe' if os.name == 'nt' else host + '-' + arch + '/toolchain/bin/clang'
    bundled = HERE.parents[1] / 'build/dependencies/llvm' / relative
    return str(bundled) if bundled.is_file() else shutil.which('clang')


def main():
    """导出同一计算函数并链接计时壳；任何负载生成失败都使本轮测量失败。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--allocator', choices=['malloc', 'pool'], default='malloc')
    parser.add_argument('case')
    parser.add_argument('compiler', type=Path)
    options = parser.parse_args()
    case = options.case
    work = HERE.parents[1] / 'tmp/perf-aot' / case
    work.mkdir(parents=True, exist_ok=True)
    source = (HERE / (case + '.chl')).read_text(encoding='utf-8')
    source = source.split('void main()', 1)[0]
    source = source.replace('import std::clock;', '')
    source = re.sub(r'const int64 REPS = 5;', '', source)
    source = re.sub(r'(?m)^(int64|float64) work\(', r'#export("f_work") \1 work(', source)
    path = work / 'work.chl'
    path.write_text(source, encoding='utf-8')
    prefix = work / (case + '-kernel')
    result = subprocess.run([str(options.compiler.resolve()), 'aot', str(path),
                             '--shared', '--allocator', options.allocator, '--out', str(prefix)],
                            capture_output=True, text=True, encoding='utf-8', errors='replace',
                            env=dict(os.environ, CHALI_LANG='zh'))
    if result.returncode:
        print(result.stdout + result.stderr)
        return result.returncode
    # C 与 AOT 复用完全相同的计时和 sink 归并，仅替换被测函数；计时壳与内核同为 LLVM O3。
    harness = (HERE / (case + '.c')).read_text(encoding='utf-8')
    harness = '#include <stdint.h>\n#include <stdio.h>\n#include <time.h>\n#include "' + prefix.name + '.h"\n#define REPS 5\nstatic long long now_ns' + harness.split('static long long now_ns', 1)[1]
    harness = re.sub(r'\bwork\b', 'f_work', harness)
    harness = re.sub(r'int main\(void\)\s*\{', 'int main(void) {\n    chali_init();', harness)
    harness = harness.replace('    return 0;', '    chali_finish();\n    return 0;')
    if os.name == 'nt':
        # Windows 的两套 MinGW CRT 对 clock_gettime 的导入名不同；统一读取单调 QPC 时钟。
        harness = harness.replace('#include <time.h>', '#include <windows.h>')
        clock = '''static long long now_ns(void)
{
    LARGE_INTEGER frequency, ticks;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&ticks);
    return (ticks.QuadPart / frequency.QuadPart) * 1000000000LL +
           (ticks.QuadPart % frequency.QuadPart) * 1000000000LL / frequency.QuadPart;
}'''
        harness = re.sub(r'static long long now_ns\(void\)\s*\{.*?\n\}', clock, harness, count=1, flags=re.S)
    driver = work / 'driver.c'
    driver.write_text(harness, encoding='utf-8')
    clang = llvm_compiler()
    if not clang:
        parser.error('找不到 LLVM clang，请先恢复 library/llvm 工具链')
    library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
    shutil.copy2(library, HERE / library.name)
    return subprocess.run([clang, '-O3', '-DNDEBUG', str(driver),
                           str(library), '-lm', '-o', str(HERE / (case + '-aot.exe'))]).returncode


if __name__ == '__main__':
    sys.exit(main())
