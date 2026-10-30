"""编译并运行预编译单元二进制往返与损坏输入回归。"""
from pathlib import Path
import argparse
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """链接实际前端库，验证真实读写器和内嵌包装载器。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-root',type=Path,default=ROOT/'build')
    parser.add_argument('--cxx',default='g++')
    parser.add_argument('--sysroot',type=Path)
    options=parser.parse_args()
    build=options.build_root.resolve()
    with tempfile.TemporaryDirectory(prefix='standard-unit-') as directory:
        output = Path(directory) / ('unit.exe' if os.name == 'nt' else 'unit')
        includes = ('source', 'library', 'source/preprocess', 'source/ast', 'source/ircode',
                    'source/console', 'source/lztext', 'source/bignum', 'source/json')
        command = [options.cxx, '-std=c++17', '-O2', '-pthread', '-ffunction-sections', '-fdata-sections',
                   '-DCHALI_CUSTOM_ALLOCATOR=1', '-DCHALI_WITH_REFLECTION=1', '-DSTACKVM_WITH_REFLECTION=1',
                   '-DSTACKVM_AOT_ONLY=1', *['-I' + name for name in includes],
                   'tests/chali_standard_unit.cpp', str(build/'chali-frontend/libparser.a'),
                   str(build/'chali-runtime/libast.a'), '-Wl,--start-group',
                   str(build/'chali-ircode-aot-core/libstackvm.a'), str(build/'chali-host-records/librecords.a'),
                   '-Wl,--end-group', str(build/'chali-runtime/libpreprocess.a'), '-Wl,--gc-sections',
                   *(['-lwinhttp'] if os.name == 'nt' else []), '-o', str(output)]
        if options.sysroot: command.insert(1,'--sysroot='+options.sysroot.resolve().as_posix())
        if os.name=='nt': command.insert(1,'-static')
        subprocess.run(command, cwd=ROOT, check=True, timeout=180)
        subprocess.run([str(output)], cwd=ROOT, check=True, timeout=30)
    print('standard unit: semantic flags, hygiene identities, truncated/versioned inputs and package integrity passed')


if __name__ == '__main__':
    main()
