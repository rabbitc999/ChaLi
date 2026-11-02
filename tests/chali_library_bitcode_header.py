"""独立编码模型和真实 LLVM 文件核对 bitcode 兼容性头及拒绝边界。"""
import argparse
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


class Bits:
    """按格式说明写入低位优先字段，不调用生产解析器。"""
    def __init__(self):
        self.values = []

    def fixed(self, value, width):
        self.values.extend((value >> bit) & 1 for bit in range(width))

    def variable(self, value, width):
        mask = (1 << (width - 1)) - 1
        while value > mask:
            self.fixed((value & mask) | (1 << (width - 1)), width)
            value >>= width - 1
        self.fixed(value, width)

    def align(self):
        self.values.extend([0] * (-len(self.values) % 32))

    def record(self, code, values, width):
        self.fixed(3, width)
        self.variable(code, 6)
        self.variable(len(values), 6)
        for value in values:
            self.variable(value, 6)

    def block(self, code, width, body, outer=2):
        self.fixed(1, outer)
        self.variable(code, 8)
        self.variable(width, 4)
        self.align()
        self.fixed(len(body.values) // 32, 32)
        self.values.extend(body.values)

    def end(self, width):
        self.fixed(0, width)
        self.align()

    def bytes(self):
        assert len(self.values) % 32 == 0
        return bytes(sum(self.values[base + bit] << bit for bit in range(8)) for base in range(0, len(self.values), 8))


def model(epoch=0, duplicate=False, producer='LLVM23.1.4', triple='x86_64-w64-windows-gnu', layout='e-p:64:64', summary_block=20):
    """编码 BLOCKINFO 继承、Char6 生产者、Blob 布局和摘要子块。"""
    root, definitions, identity, module, summary = (Bits() for _ in range(5))
    definitions.record(1, [13], 2)
    definitions.fixed(2, 2)
    definitions.variable(3, 5)
    definitions.fixed(1, 1); definitions.variable(1, 8)
    definitions.fixed(0, 1); definitions.fixed(3, 3)
    definitions.fixed(0, 1); definitions.fixed(4, 3)
    definitions.end(2)
    root.block(0, 2, definitions)
    identity.fixed(4, 3)
    identity.variable(len(producer), 6)
    alphabet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._'
    for value in producer:
        identity.fixed(alphabet.index(value), 6)
    identity.record(2, [epoch], 3)
    identity.end(3)
    root.block(13, 3, identity)
    module.record(1, [2], 3)
    module.record(2, list(triple.encode()), 3)
    if duplicate:
        module.record(2, list(triple.encode()), 3)
    module.fixed(2, 3); module.variable(2, 5)
    module.fixed(1, 1); module.variable(3, 8)
    module.fixed(0, 1); module.fixed(5, 3)
    layout = layout.encode()
    module.fixed(4, 3); module.variable(len(layout), 6); module.align()
    for value in layout:
        module.fixed(value, 8)
    module.align()
    summary.end(2)
    module.block(summary_block, 2, summary, outer=3)
    module.end(3)
    root.block(8, 3, module)
    return b'BC\xc0\xde' + root.bytes()


def main():
    """编译独立检查器，核对模型、截断、封装和三目标真实 LLVM 编码。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path)
    parser.add_argument('--build-root', type=Path)
    parser.add_argument('--cxx', default=os.environ.get('CHALI_ABI_CXX', shutil.which('g++') or 'g++'))
    parser.add_argument('--sysroot', type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='bitcode-header-') as directory:
        work = Path(directory)
        source = work / 'reader.cpp'
        source.write_text('''#include "chali_library.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
/** @brief 打印实际 bitcode 头，失败时输出诊断。
 * @param argc 参数数量。
 * @param argv 输入路径。
 * @return 成功零，失败一。 */
int main(int argc,char**argv){try{if(argc!=2)return 2;std::ifstream f(argv[1],std::ios::binary);std::string b((std::istreambuf_iterator<char>(f)),{});auto i=chali_library::inspectBitcode(b);std::cout<<i.triple<<'\\n'<<i.dataLayout<<'\\n'<<i.producer<<'\\n'<<i.llvmVersion<<'\\n'<<i.epoch<<'\\n'<<i.summary<<'\\n';return 0;}catch(const std::exception&e){std::cerr<<e.what()<<'\\n';return 1;}}
''', encoding='utf-8')
        executable = work / ('reader.exe' if os.name == 'nt' else 'reader')
        cxx = Path(shutil.which(args.cxx) or args.cxx).resolve()
        sysroot = args.sysroot
        sdk = ROOT / 'build/dependencies/llvm/static-sdk'
        if not sysroot and cxx.parent == (sdk / 'bin').resolve():
            sysroot = sdk
        flags = ['--sysroot=' + str(sysroot.resolve())] if sysroot else []
        env = dict(os.environ)
        if sysroot:
            env['PATH'] = str(cxx.parent) + os.pathsep + env.get('PATH', '')
        result = subprocess.run([str(cxx), '-std=c++17', '-O2', *flags, '-I' + str(ROOT / 'source'),
                                 str(ROOT / 'source/chali_library_bitcode.cpp'), str(source), '-o', str(executable)],
                                cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=180)
        assert result.returncode == 0, result.stdout + result.stderr

        def inspect(data, success=True):
            """每次读取同一不可变字节文件并检查退出状态。"""
            path = work / 'input.bc'
            path.write_bytes(data)
            result = subprocess.run([str(executable), str(path)], cwd=ROOT, env=env, capture_output=True,
                                    text=True, encoding='utf-8', timeout=10)
            assert (result.returncode == 0) == success, (result.returncode, result.stdout, result.stderr)
            return result.stdout.splitlines()

        encoded = model()
        expected = ['x86_64-w64-windows-gnu', 'e-p:64:64', 'LLVM23.1.4', '23.1.4', '0', '1']
        assert inspect(encoded) == expected
        assert inspect(model(summary_block=24)) == expected[:-1] + ['0'], '完整 LTO 摘要不能识别成 ThinLTO 摘要'
        wrapper = struct.pack('<5I', 0x0b17c0de, 0, 20, len(encoded), 0) + encoded
        assert inspect(wrapper) == expected
        for size in range(len(encoded)):
            inspect(encoded[:size], False)
        inspect(model(epoch=1), False)
        inspect(model(duplicate=True), False)
        inspect(b'BAD!' + encoded[4:], False)
        inspect(wrapper[:8] + struct.pack('<I', 0xfffffff0) + wrapper[12:], False)
        clang = ROOT / 'build/dependencies/llvm/toolchain/bin' / ('clang.exe' if os.name == 'nt' else 'clang')
        clang = Path(os.environ.get('CHALI_LLVM_CLANG', str(clang)))
        fixture = work / 'sample.c'
        fixture.write_text('int transform(int value){return value*3+7;}\n', encoding='utf-8')
        for triple in ['x86_64-w64-windows-gnu', 'x86_64-unknown-linux-gnu', 'aarch64-unknown-linux-gnu']:
            llvm, code = work / 'sample.ll', work / 'sample.bc'
            for suffix, output in [(['-S', '-emit-llvm'], llvm), (['-c', '-emit-llvm'], code)]:
                compiled = subprocess.run([str(clang), '--target=' + triple, '-O3', *suffix, str(fixture), '-o', str(output)],
                                          cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=60)
                assert compiled.returncode == 0, compiled.stderr
            text = llvm.read_text(encoding='utf-8')
            actual = inspect(code.read_bytes())
            assert actual[0] == re.search(r'target triple = "([^"]+)"', text)[1]
            assert actual[1] == re.search(r'target datalayout = "([^"]+)"', text)[1]
            assert actual[2].startswith('LLVM') and actual[3] in actual[2]
    print('bitcode headers: independent abbreviations/Char6/Blob model, wrappers, every truncation, epoch/duplicate rejection and three real LLVM targets OK')


if __name__ == '__main__':
    main()
