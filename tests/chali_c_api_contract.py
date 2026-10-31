"""以独立 C11/C++17 编译单元验证公开 C ABI，不依赖旧模板胶水。"""
from pathlib import Path
import argparse
import os
import shutil
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """用 C 实现分别连接 C 和 C++ 宿主，并核对协议输出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, help='供模块门禁兼容；本单元不调用编译器')
    default_clang = ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe'
    parser.add_argument('--clang', default=str(default_clang) if default_clang.is_file() else shutil.which('clang'))
    parser.add_argument('--sysroot', type=Path, help='独立 GCC 工具链的目标根目录')
    args = parser.parse_args()
    cc = os.environ.get('CHALI_C_API_CC', 'gcc')
    cxx = os.environ.get('CHALI_C_API_CXX', 'g++')
    with tempfile.TemporaryDirectory(prefix='chali-c-api-contract-') as folder:
        work = Path(folder)
        def run(*command):
            result = subprocess.run([str(x) for x in command], cwd=ROOT, capture_output=True,
                                    encoding='utf-8', errors='replace', timeout=120)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            return result.stdout
        common = ['-Wall', '-Wextra', '-Werror', '-I' + str(ROOT / 'source')]
        target_flags = ['--sysroot=' + str(args.sysroot.resolve())] if args.sysroot else []
        obj = work / 'contract.o'
        run(cc, '-std=c11', *target_flags, *common, '-c', ROOT / 'source/abi/chali_abi_contract.c', '-o', obj)
        for compiler, standard, language in ((cc, 'c11', 'c'), (cxx, 'c++17', 'c++')):
            host = work / ('host-' + language.replace('+', 'p') + ('.exe' if os.name == 'nt' else ''))
            run(compiler, '-std=' + standard, *target_flags, *common, '-x', language,
                ROOT / 'tests/chali_c_api_contract.c', '-x', 'none', obj, '-o', host)
            assert run(host) == 'C ABI target, module, frame and bounded error contracts passed\n'
        # 模拟宿主和模块采用不同打包选项；必须拒绝，不可误调函数。
        packed = work / 'packed.c'
        packed.write_text('#pragma pack(push,1)\n#include "abi/chali_abi.h"\n#pragma pack(pop)\n'
                          'int main(void){ChaliAbiInfo i=chali_abi_local_info(0);'
                          'return chali_abi_check(&i,0)==ChaliAbiIncompatible?0:1;}\n', encoding='utf-8')
        host = work / ('packed.exe' if os.name == 'nt' else 'packed')
        run(cc, '-std=c11', *target_flags, *common, packed, obj, '-o', host)
        run(host)
        objects = [obj]
        for name in ('chali_abi', 'abi_print', 'abi_string', 'abi_math', 'abi_clock', 'abi_random', 'abi_parse', 'abi_process'):
            module_object = work / (name + '.o')
            run(cc, '-std=c11', *target_flags, *common, '-c', ROOT / 'source/abi' / (name + '.c'), '-o', module_object)
            objects.append(module_object)
        pool_object = work / 'pool.o'
        run(cc, '-std=c11', *target_flags, *common, '-c', ROOT / 'source/allocator/chali_pool.c', '-o', pool_object)
        objects.append(pool_object)
        table = work / ('table.exe' if os.name == 'nt' else 'table')
        kernels = [ROOT / name for name in ('source/abi/abi_serialization.cpp', 'source/abi/abi_compress.cpp', 'source/abi/abi_scalar.cpp', 'source/abi/abi_file.cpp', 'source/abi/abi_filesystem.cpp', 'source/abi/abi_regex.cpp', 'source/abi/abi_structured_data.cpp', 'source/lztext/lztext.cpp', 'source/json/json.cpp', 'source/toml/toml.cpp', 'source/toml/toml_source.cpp')]
        run(cxx, '-std=c++17', *target_flags, *common,
            ROOT / 'tests/chali_abi_table.cpp', *objects, *kernels, '-lm', '-o', table)
        output = run(table)
        assert '直通入口表自检通过' in output, output
        print(output.strip())
        overflow = work / ('overflow.exe' if os.name == 'nt' else 'overflow')
        run(cc, '-std=c11', *target_flags, *common, ROOT / 'tests/chali_c_api_catalog_failure.c',
            work / 'chali_abi.o', '-o', overflow)
        assert run(overflow) == 'oversized C catalog rejected atomically\n'
        if args.clang:
            cross = work / 'cross.c'
            cross.write_text('#include "abi/chali_abi.h"\n#include "abi/chali_standard.h"\n'
                             '_Static_assert(sizeof(int64_t)==8,"slot");\n'
                             '_Static_assert(CHALI_ABI_TARGET_CONVENTION==EXPECTED_CC,"calling convention");\n'
                             'ChaliAbiInfo probe(void){return chali_abi_local_info(0);}\n', encoding='utf-8')
            for target, convention in (('x86_64-w64-windows-gnu', 1), ('i686-w64-windows-gnu', 1),
                                       ('x86_64-unknown-linux-gnu', 1), ('aarch64-unknown-linux-gnu', 1),
                                       ('armv7-unknown-linux-gnueabi', 1), ('armv7-unknown-linux-gnueabihf', 2)):
                run(args.clang, '--target=' + target, '-ffreestanding', '-std=c11',
                    '-DEXPECTED_CC=' + str(convention), *common, '-fsyntax-only', cross)
            print('Six target C header checks including ARM32 float ABI passed; this does not validate target AOT execution')
        else:
            print('clang unavailable: cross-target header checks not run')
    print('C11/C++17 interoperability and incompatible packing rejection passed')


if __name__ == '__main__':
    main()
