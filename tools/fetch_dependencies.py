"""下载锁定的 SQLite/miniz 到构建缓存，校验归档及每个文件，不把依赖源码写入仓库源码树。"""
from pathlib import Path, PurePosixPath
import argparse
import contextlib
import hashlib
import json
import os
import tempfile
import time
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]
LOCK = Path(__file__).with_name('dependencies.lock.json')


def digest(data):
    """返回实际字节的 SHA256。"""
    return hashlib.sha256(data).hexdigest()


@contextlib.contextmanager
def download_lock(path):
    """串行化共享下载缓存的写入，避免并行构建读取半成品。"""
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('a+b') as stream:
        if stream.tell() == 0:
            stream.write(b'0')
            stream.flush()
        stream.seek(0)
        if os.name == 'nt':
            import msvcrt
            deadline = time.monotonic() + 600
            while True:
                try:
                    msvcrt.locking(stream.fileno(), msvcrt.LK_NBLCK, 1)
                    break
                except OSError:
                    if time.monotonic() >= deadline:
                        raise TimeoutError('等待依赖缓存锁超时：' + str(path))
                    time.sleep(0.1)
        else:
            import fcntl
            fcntl.flock(stream, fcntl.LOCK_EX)
        try:
            yield
        finally:
            stream.seek(0)
            if os.name == 'nt':
                msvcrt.locking(stream.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(stream, fcntl.LOCK_UN)


def valid_package(directory, package):
    """逐文件验证构建缓存；丢失或损坏都要求从已验证归档恢复。"""
    return all((directory / name).is_file()
               and digest((directory / name).read_bytes()) == record['sha256']
               for name, record in package['files'].items())


def fetch(name, package, destination, cache, offline):
    """先验证全部归档成员，再逐文件原子安装，下载失败保留现有构建缓存。"""
    directory = destination / name
    if valid_package(directory, package):
        return
    archive = cache / (name + '-' + package['version'] + '.zip')
    if not archive.is_file() or digest(archive.read_bytes()) != package['sha256']:
        if offline:
            raise RuntimeError('离线缓存缺失或摘要不符：' + name)
        print('下载 ' + name + ' ' + package['version'], flush=True)
        with urllib.request.urlopen(package['url'], timeout=120) as response:
            data = response.read()
        if digest(data) != package['sha256']:
            raise RuntimeError('上游归档 SHA256 不符：' + name)
        with tempfile.NamedTemporaryFile(dir=cache, suffix='.download', delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(data)
        try:
            os.replace(temporary, archive)
        finally:
            temporary.unlink(missing_ok=True)
    outputs = {}
    with zipfile.ZipFile(archive) as source:
        for filename, record in package['files'].items():
            relative = PurePosixPath(filename)
            if relative.is_absolute() or '..' in relative.parts or '\\' in filename:
                raise ValueError('依赖清单路径不安全：' + filename)
            data = record['generated'].encode('utf-8') if 'generated' in record else source.read(package['prefix'] + filename)
            if digest(data) != record['sha256']:
                raise RuntimeError('依赖文件 SHA256 不符：' + name + '/' + filename)
            outputs[filename] = data
    directory.mkdir(parents=True, exist_ok=True)
    for filename, data in outputs.items():
        target = directory / filename
        target.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(dir=cache, suffix='.install', delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(data)
        try:
            os.replace(temporary, target)
        finally:
            temporary.unlink(missing_ok=True)


def main():
    """解析明确的缓存位置；版本与校验值只来自入库的锁定清单。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dest', type=Path, default=ROOT / 'build/dependencies')
    parser.add_argument('--cache', type=Path, default=ROOT / 'tmp/dependency_downloads')
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--package', choices=['sqlite', 'miniz'], action='append')
    options = parser.parse_args()
    lock = json.loads(LOCK.read_text(encoding='utf-8'))
    if lock['schema'] != 1:
        raise ValueError('不支持的依赖锁定格式')
    cache = options.cache.resolve()
    cache.mkdir(parents=True, exist_ok=True)
    with download_lock(cache / '.lock'):
        for name in options.package or lock['packages']:
            fetch(name, lock['packages'][name], options.dest.resolve(), cache, options.offline)


if __name__ == '__main__':
    main()
