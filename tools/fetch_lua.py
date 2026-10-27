"""性能测试按需下载固定 Lua 源码，验证摘要并安装到构建依赖缓存。"""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import tarfile
import urllib.request

from fetch_dependencies import download_lock
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
DESTINATION = ROOT / 'build/dependencies/lua'


def prepare(offline=False):
    """下载并核对官方 Lua 归档，返回已核对源码目录；正常编译不调用此函数。"""
    manifest = json.loads((ROOT / 'library/lua/manifest.json').read_text(encoding='utf-8'))
    prefix = 'lua-' + manifest['version']
    cache = ROOT / 'build/downloads/lua'
    cache.mkdir(parents=True, exist_ok=True)
    with download_lock(cache / '.lock'):
        archive = cache / (prefix + '.tar.gz')
        if not archive.is_file() or digest(archive) != manifest['sha256']:
            if offline:
                raise ValueError('Lua 离线归档缺失或摘要不符')
            with tempfile.TemporaryDirectory(prefix='lua-download-') as temporary:
                candidate = Path(temporary) / archive.name
                print('下载官方 Lua：' + manifest['url'], flush=True)
                with urllib.request.urlopen(manifest['url'], timeout=120) as response, candidate.open('wb') as output:
                    shutil.copyfileobj(response, output)
                if digest(candidate) != manifest['sha256']:
                    raise ValueError('Lua 官方源码 SHA256 不符')
                os.replace(candidate, archive)
        target = DESTINATION / prefix
        # 总是对照已校验归档检查源码，避免复用损坏或被修改的源码。
        with tarfile.open(archive) as source:
            for member in source:
                relative = PurePosixPath(member.name)
                if relative.is_absolute() or '..' in relative.parts or any(':' in p for p in relative.parts) or '\\' in member.name or relative.parts[0] != prefix:
                    raise ValueError('Lua 归档包含不安全路径')
                if member.isdir():
                    continue
                if not member.isfile():
                    raise ValueError('Lua 归档包含非普通文件')
                output = DESTINATION.joinpath(*relative.parts)
                data = source.extractfile(member).read()
                if not output.is_file() or output.read_bytes() != data:
                    output.parent.mkdir(parents=True, exist_ok=True)
                    output.write_bytes(data)
        return target


def digest(path):
    """计算实际文件摘要。"""
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
    """显式准备性能对照依赖，不启动性能测量。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--offline', action='store_true')
    args = parser.parse_args()
    print(prepare(args.offline))


if __name__ == '__main__':
    main()
