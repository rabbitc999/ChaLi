"""把共享原生运行时与链接依赖固化为当前宿主的 AOT 构建资产。"""
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile
import zlib
from embedded_asset_cache import AssetCache

ROOT = Path(__file__).resolve().parents[1]


def main():
    """合并已经构建的运行时对象并压缩，供两种 LLVM 后端统一链接。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="g++")
    parser.add_argument("--ar", default="ar")
    parser.add_argument("--object", type=Path, action='append', required=True)
    parser.add_argument("--output", type=Path, default=ROOT / "build/chali-aot-host/assets.txt")
    parser.add_argument("--build-root", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    build_root = args.build_root.resolve()
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    dependencies = [build_root / 'chali-ircode-aot-core/libstackvm.a',
                    build_root / 'chali-host-records/librecords.a',
                    *[path.resolve() for path in args.object], build_root / 'chali-frontend/lztext.o']
    names = ['libstdc++.a', 'libgcc_eh.a', 'libgcc.a']
    if os.name == 'nt':
        names += ['libmingw32.a', 'libmingwex.a', 'libmoldname.a', 'libwinpthread.a', 'libwinhttp.a', 'libws2_32.a', 'libcrypt32.a', 'libbcrypt.a']
    libraries = []
    for name in names:
        path = Path(subprocess.check_output([args.cxx, '-print-file-name=' + name], text=True).strip())
        if not path.is_file():
            raise SystemExit('找不到 AOT 原生链接依赖：' + name)
        packed_name = 'gcc_' + name if name in {'libmingw32.a', 'libmingwex.a', 'libmoldname.a'} else name
        libraries.append((packed_name, path))
    cache = AssetCache('aot-host', dict(libraries, **{
        'generator': Path(__file__),
        **{f'object:{index}': path for index, path in enumerate(dependencies)},
    }))
    if cache.valid([output]):
        print('AOT 原生服务资产 SHA256 相同，跳过生成。')
        return
    with tempfile.TemporaryDirectory(prefix="chali-aot-host-") as directory:
        archive = Path(directory) / "libchali_aot_host.a"
        # MRI 命令使用规范路径；隔离目录来自项目 tmp，路径不经 shell 解释。
        lines = ["CREATE " + archive.as_posix(),
                 "ADDLIB " + (build_root / "chali-ircode-aot-core/libstackvm.a").as_posix(),
                 "ADDLIB " + (build_root / "chali-host-records/librecords.a").as_posix(),
                 *["ADDMOD " + path.resolve().as_posix() for path in args.object],
                 "ADDMOD " + (build_root / "chali-frontend/lztext.o").as_posix(), "SAVE", "END"]
        subprocess.run([args.ar, "-M"], input="\n".join(lines) + "\n", text=True, check=True, cwd=ROOT)
        entries = [("libchali_aot_host.a", archive)] + libraries
        lines = []
        for name, path in entries:
            raw = path.read_bytes()
            compressor = zlib.compressobj(9, wbits=-15)
            packed = compressor.compress(raw) + compressor.flush()
            lines.append(f"{name} {len(raw)} {packed.hex()}\n")
        text = "".join(lines)
        if not output.exists() or output.read_text(encoding="ascii") != text:
            output.write_text(text, encoding="ascii")
        cache.save([output])
    print(f"AOT 原生服务资产：{len(entries)} 个链接文件")


if __name__ == "__main__":
    main()
