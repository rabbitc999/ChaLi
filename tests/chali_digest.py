"""库包流式 SHA-256 的标准消息、边界长度和分块回归。"""
from pathlib import Path
import argparse
import hashlib
import os
import random
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
HARNESS = r'''
#include "chali_digest.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
/**
 * @brief 从二进制文件分块摘要，并核对状态机与错误输入。
 * @param argc 参数数量。
 * @param argv 文件路径与分块大小。
 * @return 摘要及状态检查成功时为零。
 */
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const auto count=std::stoul(argv[2]);
    if(!count||count>65536)return 3;
    std::ifstream input(argv[1],std::ios::binary);
    if(!input)return 4;
    chali_digest::Sha256 state;
    std::array<char,65536> bytes{};
    while(input){input.read(bytes.data(),count);state.update(bytes.data(),static_cast<std::size_t>(input.gcount()));}
    if(!input.eof())return 5;
    const auto digest=state.finish();
    if(digest!=state.finish())return 6;
    try{state.update(nullptr,0);return 7;}catch(const std::runtime_error &){}
    chali_digest::Sha256 invalid;
    try{invalid.update(nullptr,1);return 8;}catch(const std::runtime_error &){}
    if constexpr(sizeof(std::size_t)>=8){try{invalid.update(bytes.data(),std::numeric_limits<std::size_t>::max());return 9;}catch(const std::runtime_error &){}}
    if(invalid.finish()!=chali_digest::sha256(""))return 10;
    std::cout<<chali_digest::hex(digest)<<'\n';
}
'''


def main():
    """与独立 hashlib 对照字节流和所有相关填充边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,help='模块门禁兼容参数；本单元不调用编译器')
    parser.add_argument('--cxx',default='g++')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='digest-') as directory:
        work = Path(directory)
        source = work/'main.cpp'; source.write_text(HARNESS,encoding='utf-8')
        binary = work/('digest.exe' if os.name=='nt' else 'digest')
        subprocess.run([args.cxx,'-std=c++17','-O2',*(['-static'] if os.name=='nt' else []),'-I',str(ROOT/'source'),str(source),str(ROOT/'source/chali_digest.cpp'),'-o',str(binary)],check=True,cwd=ROOT)
        generator = random.Random(20261101)
        messages = [b'',b'abc',b'a'*1000000,bytes(range(256))*19]
        messages += [generator.randbytes(size) for size in [1,7,55,56,57,63,64,65,119,120,127,128,129,1000,65537]]
        for message in messages:
            path=work/'input.bin';path.write_bytes(message)
            expected=hashlib.sha256(message).hexdigest()
            for chunk in [1,7,63,64,65,4096,65536]:
                result=subprocess.run([str(binary),str(path),str(chunk)],capture_output=True,encoding='utf-8',timeout=30,check=True)
                assert result.stdout.strip()==expected,(len(message),chunk,result.stdout,expected)
    print('SHA-256: standard messages, all padding boundaries, chunk sizes and terminal/error states passed')


if __name__=='__main__':
    main()
