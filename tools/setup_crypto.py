"""构建可选 libsodium 密码学宿主，并部署 std::crypto 模块。"""
from pathlib import Path
import argparse
import hashlib
import os
import platform
import shlex
import shutil
import subprocess
import sys
import tarfile
import urllib.request

from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
URL = 'https://download.libsodium.org/libsodium/releases/libsodium-1.0.22-stable-mingw.tar.gz'
SHA256 = '751d5048db30f157d7bf7afef48f06427d4b071dd7ad184c26939198753e5697'


def main():
    """使用锁定的 Windows x64 SDK 或 Linux 系统 libsodium 构建。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT/'build/crypto')
    parser.add_argument('--archive', type=Path)
    parser.add_argument('--offline', action='store_true', help='只使用已校验的本地 SDK 或系统开发包')
    parser.add_argument('--cc', default='gcc' if os.name == 'nt' else 'cc')
    parser.add_argument('--static', action='store_true', help='生成供编译器内嵌的静态归档及依赖')
    parser.add_argument('--jobs', type=int, default=4)
    options = parser.parse_args()
    source = ROOT/'library/crypto'
    output = options.out.resolve()
    windows = os.name == 'nt'
    if not windows and not sys.platform.startswith('linux'):
        parser.error('当前支持 Windows x64 和 Linux')
    library = 'chali_crypto.a' if options.static else 'chali_crypto.dll' if windows else 'chali_crypto.so'
    with tempfile.TemporaryDirectory(prefix='crypto-build-') as directory:
        work = Path(directory)
        if windows:
            if platform.machine().lower() not in ('amd64','x86_64'):
                parser.error('Windows 宿主需要 x64')
            archive = options.archive
            if archive is None:
                archive = ROOT/'build/downloads/libsodium-1.0.22-stable-mingw.tar.gz'
                archive.parent.mkdir(parents=True, exist_ok=True)
                if not archive.exists():
                    if options.offline:
                        parser.error('离线缓存缺失；请通过 --archive 提供锁定的 libsodium SDK')
                    download = work/'sodium.tar.gz'
                    with urllib.request.urlopen(URL, timeout=60) as response, download.open('wb') as target:
                        shutil.copyfileobj(response,target)
                    with download.open('rb') as stream:
                        if hashlib.file_digest(stream,'sha256').hexdigest() != SHA256:
                            raise ValueError('libsodium 下载摘要不符')
                    shutil.copy2(download, archive)
            with archive.open('rb') as stream:
                if hashlib.file_digest(stream,'sha256').hexdigest() != SHA256:
                    raise ValueError('libsodium 归档摘要不符')
            with tarfile.open(archive) as package:
                members = [m for m in package.getmembers() if m.name.startswith('libsodium-win64/')]
                package.extractall(work,members=members,filter='data')
            sdk = work/'libsodium-win64'
            flags = [str(source/'mingw_explicit_zero.c'),'-DSODIUM_STATIC','-I',str(sdk/'include'),str(sdk/'lib/libsodium.a'),'-lbcrypt']
        else:
            if options.archive:
                parser.error('--archive 只用于 Windows SDK')
            flags = ['-fPIC',*shlex.split(subprocess.check_output(
                ['pkg-config','--cflags','--libs','libsodium'],text=True))]
        compiler = shlex.split(options.cc, posix=os.name != 'nt')
        extra = shlex.split(os.environ.get('CFLAGS',''), posix=True)
        common = [*compiler,'-std=c99','-O2','-fvisibility=hidden',*extra]
        if options.static:
            objects = []
            cflags = ['-DSODIUM_STATIC','-I',str(sdk/'include')] if windows else ['-fPIC',*shlex.split(subprocess.check_output(['pkg-config','--cflags','libsodium'],text=True))]
            for unit in [source/'crypto.c'] + ([source/'mingw_explicit_zero.c'] if windows else []):
                obj = work/(unit.stem+'.o')
                subprocess.run([*common,'-DCHALI_CRYPTO_STATIC',*cflags,'-c',str(unit),'-o',str(obj)],check=True)
                objects.append(str(obj))
            ar = str(Path(compiler[0]).with_name('ar.exe')) if windows else os.environ.get('AR','ar')
            subprocess.run([ar,'rcs',str(work/library),*objects],check=True)
            dependencies = output/'lib'
            dependencies.mkdir(parents=True,exist_ok=True)
            sodium = sdk/'lib/libsodium.a' if windows else Path(subprocess.check_output(['pkg-config','--variable=libdir','libsodium'],text=True).strip())/'libsodium.a'
            shutil.copy2(sodium,dependencies/'libsodium.a')
            if windows:
                bcrypt = subprocess.check_output([*compiler,*extra,'-print-file-name=libbcrypt.a'],text=True).strip()
                shutil.copy2(bcrypt,dependencies/'libbcrypt.a')
        else:
            subprocess.run([*common,'-shared',str(source/'crypto.c'),'-o',str(work/library),*flags],check=True)
        module = output if options.static else output/'std'
        module.mkdir(parents=True,exist_ok=True)
        shutil.copy2(work/library,module/library)
        if not options.static:
            template = (source/'crypto.chl.in').read_text(encoding='utf-8')
            (module/'crypto.chl').write_text(template.replace('@LIBRARY@',library),encoding='utf-8')
        licenses = output/'licenses'
        licenses.mkdir(exist_ok=True)
        shutil.copy2(source/'libsodium-LICENSE',licenses)
    print(module/library)


if __name__ == '__main__':
    main()
