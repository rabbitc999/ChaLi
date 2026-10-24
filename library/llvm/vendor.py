"""从已校验的上游 LLVM 分发重建本仓库持有的最小离线归档。"""
import argparse
import hashlib
import io
import json
from pathlib import Path
import zipfile


def main():
    """验证上游字节，按固定清单和时间戳打包必要工具、库与头文件。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('upstream', type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    manifest = json.loads((root / 'manifest.json').read_text(encoding='utf-8'))
    if hashlib.sha256(args.upstream.read_bytes()).hexdigest() != manifest['upstream_sha256']:
        parser.error('上游分发 SHA-256 不匹配')
    binaries = {'clang.exe', 'clang-23.exe', 'clang-target-wrapper.exe', 'ld.lld.exe',
                'libLLVM-23.dll', 'libclang-cpp.dll', 'libc++.dll', 'libunwind.dll',
                'libwinpthread-1.dll', 'libffi-8.dll', 'llvm-ar.exe'}
    archive = io.BytesIO()
    with zipfile.ZipFile(args.upstream) as source, zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as output:
        for item in source.infolist():
            name = item.filename.split('/', 1)[1]
            keep = ((name.startswith('bin/') and (name[4:] in binaries or name.endswith('.cfg')))
                    or name.startswith('x86_64-w64-mingw32/lib/')
                    or name.startswith('x86_64-w64-mingw32/include/')
                    or name.startswith('lib/clang/23/')
                    or (name.startswith('include/') and not name.startswith('include/c++/'))
                    or name == 'LICENSE.TXT')
            if name.startswith('lib/clang/23/lib/') and not name.endswith('libclang_rt.builtins-x86_64.a'):
                keep = False
            if not keep or item.is_dir():
                continue
            info = zipfile.ZipInfo(name, (2026, 10, 20, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            output.writestr(info, source.read(item))
    data = archive.getvalue()
    digest = hashlib.sha256(data).hexdigest()
    if digest != manifest['archive_sha256']:
        raise SystemExit('重建归档与固定 SHA-256 不同，请核对 Python/zlib 版本和归档清单')
    offset = 0
    for part in manifest['archive_parts']:
        piece = data[offset:offset + part['bytes']]
        if hashlib.sha256(piece).hexdigest() != part['sha256']:
            raise SystemExit('重建分片与固定 SHA-256 不同：' + part['file'])
        path = root / part['file']
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(piece)
        offset += len(piece)
    if offset != len(data):
        raise SystemExit('归档分片总长度不匹配')
    print(f'已重建 LLVM 离线归档，SHA-256 {digest}')


if __name__ == '__main__':
    main()
