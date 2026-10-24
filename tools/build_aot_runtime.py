"""用固定版本 LLVM 从手写 AOT 运行时生成可内嵌的目标 LLVM 模块。"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def main():
    """生成两个 x64 宿主模块，或核对现有模块与生成源一致。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--clang', default=os.environ.get('CHALI_LLVM_CLANG'))
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    bundled = ROOT / 'library/llvm/toolchain/bin/clang.exe'
    clang = args.clang or (str(bundled) if os.name == 'nt' else shutil.which('clang'))
    if not clang:
        parser.error('找不到 LLVM clang；Windows 先运行 python library/llvm/setup.py')
    version = subprocess.check_output([clang, '--version'], text=True)
    if 'version 23.1.4' not in version:
        parser.error('运行时资产必须用固定版本 LLVM 23.1.4 生成')
    with tempfile.TemporaryDirectory(prefix='chali-aot-runtime-') as directory:
        for platform, triple in [('windows', 'x86_64-w64-windows-gnu'), ('linux', 'x86_64-unknown-linux-gnu')]:
            output = Path(directory) / 'runtime.ll'
            command = [clang, '--target=' + triple, '-S', '-emit-llvm', '-O1', 'source/aot/runtime.c', '-o', str(output)]
            if platform == 'windows':
                command += ['-isystem', str(ROOT / 'library/llvm/toolchain/include')]
            subprocess.run(command, cwd=ROOT, check=True)
            generated = output.read_text(encoding='utf-8').replace(str(ROOT).replace('\\', '/') + '/', '')
            target = ROOT / f'source/aot/runtime_{platform}.ll'
            if args.check:
                if target.read_text(encoding='utf-8') != generated:
                    raise SystemExit(f'运行时 LLVM 模块不同步：{target}')
            else:
                target.write_text(generated, encoding='utf-8')
    print('AOT 运行时 LLVM 模块已核对' if args.check else 'AOT 运行时 LLVM 模块已生成')


if __name__ == '__main__':
    main()
