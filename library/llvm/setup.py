"""校验并解包仓库固定的 LLVM Windows x64 工具链，不访问网络。"""
import hashlib
import io
import json
from pathlib import Path
import zipfile


def main():
    """校验完整归档后在本目录恢复工具、运行库与链接依赖。"""
    root = Path(__file__).resolve().parent
    manifest = json.loads((root / 'manifest.json').read_text(encoding='utf-8'))
    data = bytearray()
    for part in manifest['archive_parts']:
        piece = (root / part['file']).read_bytes()
        if len(piece) != part['bytes'] or hashlib.sha256(piece).hexdigest() != part['sha256']:
            raise SystemExit('LLVM 归档分片校验失败：' + part['file'])
        data.extend(piece)
    digest = hashlib.sha256(data).hexdigest()
    if digest != manifest['archive_sha256']:
        raise SystemExit('LLVM 归档 SHA-256 不匹配')
    target = root / 'toolchain'
    with zipfile.ZipFile(io.BytesIO(data)) as package:
        for name in package.namelist():
            path = (target / name).resolve()
            if not path.is_relative_to(target.resolve()):
                raise SystemExit('LLVM 归档包含越界路径')
        package.extractall(target)
    print(f"LLVM {manifest['version']}: {target}")


if __name__ == '__main__':
    main()
