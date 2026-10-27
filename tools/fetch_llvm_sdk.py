"""下载锁定的 MSYS2 LLVM/LLD 静态 SDK 与匹配的 MinGW GCC；不在源码树保存二进制。"""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import platform
import shutil
import subprocess
import tarfile
import time
import urllib.request

from fetch_dependencies import download_lock
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
LOCK = Path(__file__).with_name('llvm_sdk.lock.json')
DESTINATION = ROOT / 'build/dependencies/llvm/static-sdk'
COMPONENTS = 'core irreader passes x86codegen x86asmparser dtlto libdriver lto option windowsdriver windowsmanifest all-targets'.split()


def checksum(path):
    """分块计算归档或安装文件摘要，不将整个 SDK 读入内存。"""
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def download(package, repository, cache, offline):
    """下载到私有临时文件，核对固定长度和摘要后原子写入缓存。"""
    archive = cache / package['file']
    def valid(path):
        return path.is_file() and path.stat().st_size == package['bytes'] and checksum(path) == package['sha256']
    if valid(archive):
        return archive
    if offline:
        raise RuntimeError('离线缓存缺失或损坏：' + archive.name)
    for attempt in range(3):
        try:
            with tempfile.TemporaryDirectory(prefix='llvm-download-') as temporary:
                output = Path(temporary) / archive.name
                print('下载：' + package['file'], flush=True)
                with urllib.request.urlopen(repository + package['file'], timeout=120) as response, output.open('wb') as stream:
                    shutil.copyfileobj(response, stream, 1024 * 1024)
                if not valid(output):
                    raise ValueError('下载长度或 SHA256 不匹配：' + archive.name)
                os.replace(output, archive)
            return archive
        except OSError:
            if attempt == 2:
                raise
            time.sleep(attempt + 1)


def extract(archive, destination):
    """仅安装 mingw64 前缀，检查路径及链接目标，Windows 上将包内链接实体化。"""
    try:
        import zstandard
    except ImportError as error:
        raise RuntimeError('解压预编译 SDK 需要 zstandard：python -m pip install zstandard') from error
    with tempfile.TemporaryDirectory(prefix='llvm-extract-') as temporary:
        unpacked = Path(temporary) / 'package.tar'
        with archive.open('rb') as source, unpacked.open('wb') as target:
            zstandard.ZstdDecompressor().copy_stream(source, target)
        with tarfile.open(unpacked) as package:
            for member in package:
                relative = PurePosixPath(member.name)
                if relative.is_absolute() or '..' in relative.parts or any(':' in p for p in relative.parts) or '\\' in member.name:
                    raise ValueError('SDK 归档包含不安全路径：' + member.name)
                if not relative.parts or relative.parts[0] != 'mingw64' or member.isdir():
                    continue
                if member.issym() or member.islnk():
                    link = PurePosixPath(member.linkname)
                    if link.is_absolute() or '..' in link.parts or any(':' in p for p in link.parts) or '\\' in member.linkname:
                        raise ValueError('SDK 归档包含不安全链接：' + member.name)
                elif not member.isfile():
                    raise ValueError('SDK 归档包含特殊文件：' + member.name)
                target = destination.joinpath(*relative.parts[1:])
                target.parent.mkdir(parents=True, exist_ok=True)
                with package.extractfile(member) as source, target.open('wb') as output:
                    shutil.copyfileobj(source, output, 1024 * 1024)


def validate(directory, version):
    """运行匹配的编译器和 llvm-config，要求真实静态库及 LLD 接口完整。"""
    env = dict(os.environ, PATH=str(directory / 'bin') + os.pathsep + os.environ.get('PATH', ''))
    config = directory / 'bin/llvm-config.exe'
    def query(*arguments):
        return subprocess.check_output([str(config), *arguments], env=env, text=True).strip()
    if query('--version') != version:
        raise ValueError('llvm-config 实际版本与锁定版本不同')
    compiler = subprocess.check_output([str(directory / 'bin/g++.exe'), '-dumpmachine'], env=env, text=True).strip()
    if compiler != 'x86_64-w64-mingw32':
        raise ValueError('SDK 编译器目标不匹配：' + compiler)
    libraries = query('--link-static', '--libnames', *COMPONENTS).split()
    for name in [*libraries, 'liblldMinGW.a', 'liblldCOFF.a', 'liblldCommon.a']:
        path = directory / 'lib' / name
        if not path.is_file() or path.suffix != '.a' or name.endswith('.dll.a'):
            raise ValueError('缺少真实静态库：' + str(path))
        with path.open('rb') as stream:
            if stream.read(8) != b'!<arch>\n':
                raise ValueError('无效静态库：' + name)
    if not (directory / 'include/lld/Common/Driver.h').is_file():
        raise ValueError('LLD 开发接口缺失')
    with tempfile.TemporaryDirectory(prefix='llvm-sdk-probe-') as temporary:
        source = Path(temporary) / 'probe.cpp'
        output = Path(temporary) / 'probe.exe'
        source.write_text('#include <llvm-c/Core.h>\n#include <lld/Common/Driver.h>\nLLD_HAS_DRIVER(mingw)\nint main() { volatile auto driver = &lld::mingw::link; auto c = LLVMContextCreate(); LLVMContextDispose(c); return driver ? 0 : 1; }\n', encoding='ascii')
        system = query('--link-static', '--system-libs', *COMPONENTS).replace('-lzstd.dll', '-lzstd').split()
        subprocess.run([str(directory / 'bin/g++.exe'), '--sysroot=' + str(directory), '-std=c++17', '-DLLVM_BUILD_STATIC', '-static', '-fuse-ld=lld',
                        '-I' + str(directory / 'include'), '-I' + str(directory / 'include/libxml2'),
                        str(source), str(ROOT / 'tools/llvm_static_imports.cpp'), '-L' + str(directory / 'lib'),
                        '-Wl,--start-group', '-llldMinGW', '-llldCOFF', '-llldCommon',
                        *query('--link-static', '--libs', *COMPONENTS).split(), *system,
                        '-liconv', '-lbcrypt', '-Wl,--end-group', '-o', str(output)], env=env, check=True)
        subprocess.run([str(output)], env=dict(env, PATH=os.environ.get('SystemRoot', 'C:/Windows') + '/System32'), check=True)
    print('已核对 LLVM ' + version + ' 静态库及匹配 GCC：' + str(directory), flush=True)


def main():
    """离线复用或顺序下载完整锁定依赖；全部验证后才替换本地 SDK。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dest', type=Path, default=DESTINATION)
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--check', action='store_true', help='仅核对已安装 SDK，不下载')
    parser.add_argument('--jobs', type=int, default=4, help='兼容构建入口；预编译 SDK 不需要源码构建')
    args = parser.parse_args()
    if os.name != 'nt' or platform.machine().lower() not in ('amd64', 'x86_64'):
        parser.error('锁定的预编译 SDK 目标为 Windows x64')
    lock = json.loads(LOCK.read_text(encoding='utf-8'))
    destination = args.dest.resolve()
    # SDK 是构建依赖，禁止写入 source/library 或仓库外的任意目录。
    dependency_root = (ROOT / 'build/dependencies').resolve()
    if not dependency_root.is_relative_to(ROOT.resolve()) or dependency_root not in destination.parents:
        parser.error('--dest 必须位于仓库 build/dependencies 内')
    cache = ROOT / 'build/downloads/llvm'
    cache.mkdir(parents=True, exist_ok=True)
    lock_hash = checksum(LOCK)
    with download_lock(cache / '.lock'):
        marker = destination / 'chali-sdk.json'
        if marker.is_file():
            manifest = json.loads(marker.read_text(encoding='utf-8'))
            if manifest.get('lock_sha256') == lock_hash and all((destination / name).is_file() and checksum(destination / name) == value for name, value in manifest['files'].items()):
                validate(destination, lock['version'])
                return
        if args.check:
            raise ValueError('SDK 未安装或文件校验失败')
        with tempfile.TemporaryDirectory(prefix='llvm-sdk-') as temporary:
            stage = Path(temporary) / 'sdk'
            stage.mkdir()
            for package in lock['packages']:
                extract(download(package, lock['repository'], cache, args.offline), stage)
            validate(stage, lock['version'])
            manifest = {'version': lock['version'], 'abi': lock['abi'], 'lock_sha256': lock_hash,
                        'files': {p.relative_to(stage).as_posix(): checksum(p) for p in sorted(stage.rglob('*')) if p.is_file()}}
            (stage / 'chali-sdk.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
            destination.parent.mkdir(parents=True, exist_ok=True)
            backup = Path(temporary) / 'previous-sdk'
            if destination.exists():
                destination.rename(backup)
            try:
                stage.rename(destination)
            except BaseException:
                if backup.exists():
                    backup.rename(destination)
                raise
            validate(destination, lock['version'])


if __name__ == '__main__':
    main()
