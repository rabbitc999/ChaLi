"""用真实 LLVM 转发器核对库 bitcode 物化与重编码的 O0/O3 选择及运行结果。"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
PROXY = r'''
#include "source/console/utf8.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#ifndef _WIN32
#include <unistd.h>
#endif
/**
 * @brief 记录真实编译器参数，再原样执行锁定的 LLVM，不模拟 LLVM 输出。
 * @param argc 宿主传入的实参数量。
 * @param argv 宿主实参，Windows 重新从宽字符 CRT 读取。
 * @return 真实 LLVM 的退出码；启动失败返回非零。
 */
int main(int argc, char **argv)
{
    auto arguments = tool_console::arguments(argc, argv);
    const std::string compiler = COMPILER_PATH;
    std::ofstream log(std::filesystem::u8path(LOG_PATH), std::ios::app);
    if (!log) return 90;
    log << '[';
    for (std::size_t i = 1; i < arguments.size(); ++i) {
        if (i > 1) log << ',';
        log << std::quoted(arguments[i]);
    }
    log << "]\n"; log.close();
#ifdef _WIN32
    std::wstring command = tool_console::quoteArgument(std::filesystem::u8path(compiler).wstring());
    for (std::size_t i = 1; i < arguments.size(); ++i)
        command += L' ' + tool_console::quoteArgument(std::filesystem::u8path(arguments[i]).wstring());
    STARTUPINFOW startup{}; startup.cb = sizeof(startup); startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    PROCESS_INFORMATION child{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startup, &child)) return 91;
    CloseHandle(child.hThread); WaitForSingleObject(child.hProcess, INFINITE);
    DWORD status = 92; GetExitCodeProcess(child.hProcess, &status); CloseHandle(child.hProcess);
    return static_cast<int>(status);
#else
    arguments[0] = compiler;
    std::vector<char *> raw;
    for (auto &argument : arguments) raw.push_back(argument.data());
    raw.push_back(nullptr); execv(compiler.c_str(), raw.data()); return 91;
#endif
}
'''


def main():
    """在三种 LTO 模式下执行真实物化；转发器仅观察参数，不改写模块。"""
    fallback = ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe' if os.name == 'nt' else shutil.which('clang') or 'clang'
    default = Path(os.environ.get('CHALI_LLVM_CLANG', str(fallback)))
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path, default=default)
    parser.add_argument('--cxx', default=os.environ.get('CHALI_ABI_CXX', shutil.which('g++')))
    args = parser.parse_args()
    clang = Path(shutil.which(str(args.external_clang)) or args.external_clang).resolve()
    assert args.cxx and clang.is_file(), '需要 C++17 编译器和匹配的真实 LLVM'
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='debug-bitcode-') as temporary:
        work = Path(temporary)
        modules = work / 'modules/demo'; modules.mkdir(parents=True)
        calls = work / 'llvm-calls.jsonl'
        proxy_source = work / 'proxy.cpp'
        proxy_source.write_text(PROXY.replace('COMPILER_PATH', json.dumps(clang.as_posix(), ensure_ascii=False))
                                .replace('LOG_PATH', json.dumps(calls.as_posix(), ensure_ascii=False)), encoding='utf-8')
        proxy = work / ('proxy.exe' if os.name == 'nt' else 'proxy')
        flags = ['-static'] if os.name == 'nt' else []
        sdk = ROOT / 'build/dependencies/llvm/static-sdk'
        if Path(shutil.which(args.cxx) or args.cxx).resolve().parent == (sdk / 'bin').resolve():
            flags.append('--sysroot=' + str(sdk))
        subprocess.run([args.cxx, '-std=c++17', '-O0', '-I' + str(ROOT), *flags, str(proxy_source), '-o', str(proxy)], check=True, timeout=120)

        def run(*arguments):
            """执行真实编译／程序并保留失败时的全部诊断。"""
            result = subprocess.run(list(map(str, arguments)), cwd=work, capture_output=True, encoding='utf-8', errors='replace', timeout=180)
            assert result.returncode == 0, (arguments, result.returncode, result.stdout, result.stderr)
            return result.stdout

        default_backend = 'builtin' if json.loads(run(compiler, '--build-info'))['llvm'] == 'static' else 'default-external'

        source = work / 'provider.chl'
        source.write_text('module demo::math;\nint64 times(int64 x){return x*3;}\n', encoding='utf-8')
        run(compiler, 'lib', 'build', source, '--module', 'demo::math', '--package-version', '1.0.0', '--with-bitcode', '--out', modules / 'math.chlib')
        source.unlink()
        consumer = work / 'main.chl'
        consumer.write_text('import demo::math;\nvoid main(){assert(demo::math::times(4)==12);println("BITCODE DEBUG MODE OK");}\n', encoding='utf-8')
        for external in [False, True]:
            for optimized in [False, True]:
                for mode in ['off', 'full', 'thin']:
                    calls.write_text('', encoding='utf-8')
                    prefix = work / (('external' if external else default_backend) + '-' + str(optimized) + '-' + mode)
                    run(compiler, 'aot', consumer, '--checked', '--debug-optimized' if optimized else '--debug',
                        '--library-implementation', 'bitcode', '--lto-mode', mode, '--out', prefix, *(['--llvm', proxy] if external else []))
                    executable = prefix.with_suffix('.exe') if os.name == 'nt' else prefix
                    assert 'BITCODE DEBUG MODE OK' in run(executable)
                    if external:
                        recorded = [json.loads(line) for line in calls.read_text(encoding='utf-8').splitlines()]
                        materialized = [row for row in recorded if '-c' in row and '-x' in row and 'ir' in row]
                        assert materialized, recorded
                        assert all(('-O3' if optimized else '-O0') in row for row in materialized), materialized
                    print(('external' if external else default_backend) + '/' + ('O3' if optimized else 'O0') + '/' + mode + ': actual bitcode compilation and result OK', flush=True)


if __name__ == '__main__':
    main()
