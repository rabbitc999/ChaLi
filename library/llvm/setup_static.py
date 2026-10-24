"""自动准备固定源码并构建与 ChaLi 使用相同 GCC ABI 的 LLVM/LLD 静态开发库。"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import urllib.request

HERE = Path(__file__).resolve().parent
VERSION = '23.1.4'
SOURCE_URL = f'https://github.com/llvm/llvm-project/releases/download/llvmorg-{VERSION}/llvm-project-{VERSION}.src.tar.xz'
SOURCE_SHA256 = '819ecf7351f1a66f76b2d701d8942faaf5faeb324cb90c9d5b090de142434bb2'
TARGETS = ['llvm-config', 'lldMinGW', 'lldCOFF', 'LLVMX86CodeGen', 'LLVMX86AsmParser', 'LLVMPasses', 'LLVMIRReader']


def prepare_source(archive, destination):
    """仅从固定官方地址下载源码，核对摘要后安全提取构建所需子项目。"""
    if not archive.is_file():
        temporary = archive.with_suffix('.download')
        print('下载 LLVM 固定源码：' + SOURCE_URL, flush=True)
        urllib.request.urlretrieve(SOURCE_URL, temporary)
        if hashlib.sha256(temporary.read_bytes()).hexdigest() != SOURCE_SHA256:
            temporary.unlink()
            raise SystemExit('LLVM 上游源码 SHA-256 不匹配')
        temporary.replace(archive)
    if hashlib.sha256(archive.read_bytes()).hexdigest() != SOURCE_SHA256:
        raise SystemExit('LLVM 缓存源码 SHA-256 不匹配')
    marker = destination / '.source-sha256'
    if marker.is_file() and marker.read_text(encoding='ascii').strip() == SOURCE_SHA256:
        return
    destination.mkdir(parents=True, exist_ok=True)
    with tarfile.open(archive) as source:
        for item in source:
            parts = item.name.split('/', 1)
            if len(parts) != 2:
                continue
            relative = Path(parts[1])
            if not relative.parts or relative.parts[0] not in ('llvm', 'lld', 'libc', 'cmake', 'third-party'):
                continue
            if any(part in ('test', 'unittests', 'examples', 'benchmarks') for part in relative.parts):
                continue
            if 'docs' in relative.parts and relative.parts[0] != 'lld':
                continue
            if not item.isfile():
                continue
            target = (destination / relative).resolve()
            if not target.is_relative_to(destination.resolve()):
                raise SystemExit('LLVM 源码归档包含越界路径')
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(source.extractfile(item).read())
    marker.write_text(SOURCE_SHA256 + '\n', encoding='ascii')


def main():
    """复用已验收 SDK，或下载固定源码、增量构建静态库并安装所需接口。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=6)
    parser.add_argument('--rebuild', action='store_true')
    parser.add_argument('--source-archive', type=Path)
    args = parser.parse_args()
    if os.name != 'nt' or args.jobs < 1:
        parser.error('当前静态开发库构建目标为 Windows x64，jobs 必须大于零')
    sdk = HERE / 'static-sdk'
    manifest = sdk / 'manifest.json'
    if manifest.is_file() and not args.rebuild:
        data = json.loads(manifest.read_text(encoding='utf-8'))
        if data['source_sha256'] != SOURCE_SHA256:
            raise SystemExit('静态 SDK 与固定 LLVM 源码版本不一致')
        for name, expected in data['sha256'].items():
            if hashlib.sha256((sdk / name).read_bytes()).hexdigest() != expected:
                raise SystemExit('静态 SDK 文件校验失败：' + name)
        print('LLVM 静态 SDK 已核对：' + str(sdk))
        return
    cache = HERE / 'cache'
    cache.mkdir(exist_ok=True)
    archive = args.source_archive or cache / f'llvm-project-{VERSION}.src.tar.xz'
    source = HERE / 'static-source'
    build = HERE / 'static-build'
    prepare_source(archive, source)
    compiler = shutil.which(os.environ.get('CXX', 'g++'))
    cc = shutil.which(os.environ.get('CC', 'gcc'))
    ninja = shutil.which('ninja')
    if not ninja:
        bundled = Path(sys.executable).parent / 'Scripts/ninja.exe'
        if bundled.is_file():
            ninja = str(bundled)
    if not compiler or not cc or not ninja:
        parser.error('需要 GCC/G++、CMake 和 Ninja；Ninja 可通过 python -m pip install ninja 安装')
    options = ['cmake', '-G', 'Ninja', '-S', str(source / 'llvm'), '-B', str(build),
               '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_C_COMPILER=' + cc.replace('\\', '/'),
               '-DCMAKE_CXX_COMPILER=' + compiler.replace('\\', '/'), '-DCMAKE_MAKE_PROGRAM=' + ninja.replace('\\', '/'),
               '-DCMAKE_INSTALL_PREFIX=' + str(sdk), '-DLLVM_TARGETS_TO_BUILD=X86', '-DLLVM_ENABLE_PROJECTS=lld',
               '-DCMAKE_CXX_FLAGS_RELEASE=-O2 -DNDEBUG', '-DCMAKE_C_FLAGS_RELEASE=-O2 -DNDEBUG',
               '-DCMAKE_EXE_LINKER_FLAGS=-static']
    for option in ('LLVM_BUILD_LLVM_DYLIB', 'LLVM_LINK_LLVM_DYLIB', 'BUILD_SHARED_LIBS',
                   'LLVM_INCLUDE_TESTS', 'LLVM_INCLUDE_BENCHMARKS', 'LLVM_INCLUDE_EXAMPLES', 'LLVM_INCLUDE_DOCS',
                   'LLVM_ENABLE_ZLIB', 'LLVM_ENABLE_ZSTD', 'LLVM_ENABLE_LIBXML2', 'LLVM_ENABLE_CURL',
                   'LLVM_ENABLE_TERMINFO', 'LLVM_ENABLE_ASSERTIONS', 'LLVM_ENABLE_BINDINGS', 'LLVM_ENABLE_OCAMLDOC'):
        options.append('-D' + option + '=OFF')
    subprocess.run(options, check=True)
    subprocess.run(['cmake', '--build', str(build), '--parallel', str(args.jobs), '--target', *TARGETS], check=True)
    for original in (source / 'llvm/include', build / 'include', source / 'lld/include'):
        shutil.copytree(original, sdk / 'include', dirs_exist_ok=True)
    (sdk / 'lib').mkdir(parents=True, exist_ok=True)
    for library in (build / 'lib').glob('*.a'):
        shutil.copy2(library, sdk / 'lib' / library.name)
    (sdk / 'bin').mkdir(exist_ok=True)
    shutil.copy2(build / 'bin/llvm-config.exe', sdk / 'bin/llvm-config.exe')
    files = [sdk / 'bin/llvm-config.exe', *sorted((sdk / 'lib').glob('*.a'))]
    data = {'version': VERSION, 'source_url': SOURCE_URL, 'source_sha256': SOURCE_SHA256,
            'compiler': subprocess.check_output([compiler, '--version'], text=True).splitlines()[0],
            'sha256': {path.relative_to(sdk).as_posix(): hashlib.sha256(path.read_bytes()).hexdigest() for path in files}}
    manifest.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('LLVM 静态 SDK 构建完成：' + str(sdk))


if __name__ == '__main__':
    main()
