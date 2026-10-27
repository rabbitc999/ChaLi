"""下载并校验固定的 llvm-mingw 分发，仅安装外置 AOT 所需工具和目标库。"""
import argparse
import hashlib
import json
import os
import platform
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import tarfile
import urllib.request
import zipfile

from fetch_dependencies import download_lock
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
DESTINATION = ROOT / 'build/dependencies/llvm/toolchain'


def platform_key():
    """将宿主系统与架构归一为锁定分发键，不将交叉下载视为本机支持。"""
    system = {'Windows': 'windows', 'Linux': 'linux', 'Darwin': 'macos'}.get(platform.system(), platform.system().lower())
    machine = platform.machine().lower()
    architecture = {'amd64': 'x64', 'x86_64': 'x64', 'aarch64': 'arm64', 'arm64': 'arm64'}.get(machine, machine)
    return system + '-' + architecture


def prepare_unix(key, package, offline):
    """校验并安全安装官方 Linux/macOS 完整工具包；保留执行权限及安全的包内链接。"""
    from fetch_llvm_sdk import download
    destination = ROOT / 'build/dependencies/llvm' / key / 'toolchain'
    if not destination.resolve().is_relative_to(ROOT.resolve()):
        raise ValueError('LLVM 安装目标越出仓库')
    cache = ROOT / 'build/downloads/llvm'
    cache.mkdir(parents=True, exist_ok=True)
    with download_lock(cache / ('.' + key + '-lock')):
        marker = destination / 'chali-download.json'
        if marker.is_file():
            installed = json.loads(marker.read_text(encoding='utf-8'))
            if installed.get('sha256') == package['sha256'] and all((destination / name).is_file() and digest(destination / name) == value for name, value in installed['files'].items()):
                print('LLVM 工具包已核对：' + str(destination))
                return
        url, filename = package['url'].rsplit('/', 1)
        archive = download(dict(file=filename, bytes=package['bytes'], sha256=package['sha256']), url + '/', cache, offline)
        with tempfile.TemporaryDirectory(prefix='llvm-' + key + '-') as temporary:
            extracted = Path(temporary) / 'unpacked'
            extracted.mkdir()
            with tarfile.open(archive) as source:
                # data 过滤器拒绝越界路径、特殊文件和指向包外的链接。
                source.extractall(extracted, filter='data')
            candidates = [extracted, *[p for p in extracted.iterdir() if p.is_dir()]]
            stage = next((p for p in candidates if (p / 'bin/clang').is_file()), None)
            if stage is None:
                raise ValueError('官方工具包中缺少 bin/clang')
            if key == platform_key():
                subprocess.run([str(stage / 'bin/clang'), '--version'], check=True)
            record = {'platform': key, 'version': package['version'], 'sha256': package['sha256'],
                      'files': {p.relative_to(stage).as_posix(): digest(p) for p in stage.rglob('*') if p.is_file()}}
            (stage / 'chali-download.json').write_text(json.dumps(record, indent=2) + '\n', encoding='utf-8')
            destination.parent.mkdir(parents=True, exist_ok=True)
            backup = Path(temporary) / 'previous'
            if destination.exists():
                destination.rename(backup)
            try:
                stage.rename(destination)
            except BaseException:
                if backup.exists():
                    backup.rename(destination)
                raise
        print('LLVM ' + key + ' 已安装：' + str(destination))


def main():
    """按固定摘要下载缓存；安全解包成功后才替换本地工具链。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--platform', help='显式下载其他平台；不在本机执行不同平台的工具')
    parser.add_argument('--list', action='store_true', help='列出固定版本的可下载平台，不下载')
    args = parser.parse_args()
    manifest = json.loads((ROOT / 'library/llvm/manifest.json').read_text(encoding='utf-8'))
    if not DESTINATION.resolve().is_relative_to(ROOT.resolve()):
        raise ValueError('LLVM 安装目标越出仓库')
    if args.list:
        for key, record in manifest['platforms'].items():
            print(key + ': LLVM ' + record['version'] + ' ' + record['sha256'])
        return
    key = args.platform or platform_key()
    if key not in manifest['platforms']:
        parser.error('没有锁定的预编译分发：' + key + '；使用 --list 查看，或通过 CHALI_LLVM_CLANG 指定系统 LLVM')
    if key != 'windows-x64':
        prepare_unix(key, manifest['platforms'][key], args.offline)
        return
    cache = ROOT / 'build/downloads/llvm'
    cache.mkdir(parents=True, exist_ok=True)
    with download_lock(cache / '.toolchain-lock'):
        marker = DESTINATION / 'chali-download.json'
        if marker.is_file():
            record = json.loads(marker.read_text(encoding='utf-8'))
            if record.get('upstream_sha256') == manifest['upstream_sha256'] and all((DESTINATION / name).is_file() and digest(DESTINATION / name) == value for name, value in record['files'].items()):
                print('外置 LLVM 工具链已核对：' + str(DESTINATION))
                return
        archive = cache / manifest['upstream'].rsplit('/', 1)[-1]
        if not archive.is_file() or digest(archive) != manifest['upstream_sha256']:
            if args.offline:
                raise ValueError('离线 LLVM 归档缺失或摘要不符')
            with tempfile.TemporaryDirectory(prefix='llvm-toolchain-download-') as temporary:
                download = Path(temporary) / 'upstream.zip'
                print('下载外置 LLVM：' + manifest['upstream'], flush=True)
                with urllib.request.urlopen(manifest['upstream'], timeout=120) as response, download.open('wb') as output:
                    shutil.copyfileobj(response, output, 1024 * 1024)
                if digest(download) != manifest['upstream_sha256']:
                    raise ValueError('LLVM 上游归档 SHA256 不符')
                os.replace(download, archive)
        binaries = {'clang.exe', 'clang-23.exe', 'clang-target-wrapper.exe', 'ld.lld.exe',
                    'libLLVM-23.dll', 'libclang-cpp.dll', 'libc++.dll', 'libunwind.dll',
                    'libwinpthread-1.dll', 'libffi-8.dll', 'llvm-ar.exe'}
        with tempfile.TemporaryDirectory(prefix='llvm-toolchain-install-') as temporary:
            stage = Path(temporary) / 'toolchain'
            stage.mkdir()
            with zipfile.ZipFile(archive) as source:
                for item in source.infolist():
                    parts = PurePosixPath(item.filename).parts
                    if not parts or '..' in parts or any(':' in p for p in parts) or '\\' in item.filename or item.filename.startswith('/'):
                        raise ValueError('LLVM 归档路径不安全')
                    name = '/'.join(parts[1:])
                    keep = ((name.startswith('bin/') and (name[4:] in binaries or name.endswith('.cfg')))
                            or name.startswith(('x86_64-w64-mingw32/lib/', 'x86_64-w64-mingw32/include/', 'lib/clang/23/'))
                            or (name.startswith('include/') and not name.startswith('include/c++/')) or name == 'LICENSE.TXT')
                    if name.startswith('lib/clang/23/lib/') and not name.endswith('libclang_rt.builtins-x86_64.a'):
                        keep = False
                    if not keep or item.is_dir():
                        continue
                    target = stage / name
                    target.parent.mkdir(parents=True, exist_ok=True)
                    with source.open(item) as stream, target.open('wb') as output:
                        shutil.copyfileobj(stream, output)
            if not (stage / 'bin/clang.exe').is_file():
                raise ValueError('归档缺少 clang.exe')
            record = {'upstream_sha256': manifest['upstream_sha256'], 'files': {p.relative_to(stage).as_posix(): digest(p) for p in stage.rglob('*') if p.is_file()}}
            (stage / 'chali-download.json').write_text(json.dumps(record, indent=2) + '\n', encoding='utf-8')
            DESTINATION.parent.mkdir(parents=True, exist_ok=True)
            backup = Path(temporary) / 'previous'
            if DESTINATION.exists():
                DESTINATION.rename(backup)
            try:
                stage.rename(DESTINATION)
            except BaseException:
                if backup.exists():
                    backup.rename(DESTINATION)
                raise
        print('外置 LLVM 已安装：' + str(DESTINATION))


def digest(path):
    """分块计算真实文件字节的摘要。"""
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


if __name__ == '__main__':
    main()
