"""校验并离线解包性能对照使用的官方 Lua 源码。"""
import hashlib
import json
from pathlib import Path
import tarfile


def main():
    """核对官方源码归档后，仅提取正常文件并保持归档目录结构。"""
    root = Path(__file__).resolve().parent
    manifest = json.loads((root / 'manifest.json').read_text(encoding='utf-8'))
    archive = root / f"lua-{manifest['version']}.tar.gz"
    if hashlib.sha256(archive.read_bytes()).hexdigest() != manifest['sha256']:
        raise SystemExit('Lua 官方源码归档 SHA-256 不匹配')
    target = (root / 'source').resolve()
    with tarfile.open(archive) as package:
        for member in package.getmembers():
            if member.isdir():
                continue
            if not member.isfile():
                raise SystemExit('Lua 归档中出现非普通文件')
            path = (target / member.name).resolve()
            if not path.is_relative_to(target):
                raise SystemExit('Lua 归档包含越界路径')
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(package.extractfile(member).read())
    print(f"Lua {manifest['version']} source: {target}")


if __name__ == '__main__':
    main()
