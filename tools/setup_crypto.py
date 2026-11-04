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
    parser.add_argument('--cc', default='gcc' if os.name == 'nt' else 'cc')
    options = parser.parse_args()
    source = ROOT/'library/crypto'
    output = options.out.resolve()
    windows = os.name == 'nt'
    if not windows and not sys.platform.startswith('linux'):
        parser.error('当前支持 Windows x64 和 Linux')
    library = 'chali_crypto.dll' if windows else 'chali_crypto.so'
    with tempfile.TemporaryDirectory(prefix='crypto-build-') as directory:
        work = Path(directory)
        if windows:
            if platform.machine().lower() not in ('amd64','x86_64'):
                parser.error('Windows 宿主需要 x64')
            archive = options.archive
            if archive is None:
                archive = work/'sodium.tar.gz'
                with urllib.request.urlopen(URL, timeout=60) as response, archive.open('wb') as target:
                    shutil.copyfileobj(response,target)
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
        subprocess.run([options.cc,'-std=c99','-O2','-shared','-fvisibility=hidden',
                        str(source/'crypto.c'),'-o',str(work/library),*flags],check=True)
        module = output/'std'
        module.mkdir(parents=True,exist_ok=True)
        shutil.copy2(work/library,module/library)
        template = (source/'crypto.chl.in').read_text(encoding='utf-8')
        (module/'crypto.chl').write_text(template.replace('@LIBRARY@',library),encoding='utf-8')
        licenses = output/'licenses'
        licenses.mkdir(exist_ok=True)
        shutil.copy2(source/'libsodium-LICENSE',licenses)
    print(module/library)


if __name__ == '__main__':
    main()
