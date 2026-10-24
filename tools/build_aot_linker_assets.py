"""从固定 LLVM 工具链生成内置 LLD 所需的 Windows x64 链接资产。"""
import argparse
from pathlib import Path
import zlib
from embedded_asset_cache import AssetCache

ROOT = Path(__file__).resolve().parents[1]
NAMES = ['crt2.o', 'dllcrt2.o', 'crtbegin.o', 'crtend.o',
         *('lib' + name + '.a' for name in ('mingw32', 'unwind', 'moldname', 'mingwex',
                                          'msvcrt', 'advapi32', 'shell32', 'user32', 'kernel32'))]


def main():
    """逐个压缩固定清单的静态库与导入库，并生成或核对文本内嵌资产。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    toolchain = ROOT / 'library/llvm/toolchain'
    entries = [(name, toolchain / 'x86_64-w64-mingw32/lib' / name) for name in NAMES]
    entries.append(('libclang_rt.builtins-x86_64.a', toolchain / 'lib/clang/23/lib/windows/libclang_rt.builtins-x86_64.a'))
    target = ROOT / 'source/aot/linker_assets.txt'
    cache = AssetCache('aot-linker', dict(entries, **{'generator': Path(__file__)}))
    if cache.valid([target]):
        print('AOT 链接资产 SHA256 相同，跳过生成。')
        return
    lines = []
    for name, path in entries:
        data = path.read_bytes()
        compressor = zlib.compressobj(9, wbits=-15)
        packed = compressor.compress(data) + compressor.flush()
        assert zlib.decompress(packed, wbits=-15) == data
        lines.append(f'{name} {len(data)} {packed.hex()}\n')
    text = ''.join(lines)
    if args.check:
        assert target.read_text(encoding='ascii') == text, 'AOT 链接资产不同步'
    else:
        if not target.exists() or target.read_text(encoding='ascii') != text:
            target.write_text(text, encoding='ascii')
    cache.save([target])
    print(f'AOT 链接资产已核对：{len(entries)} 个文件' if args.check else f'AOT 链接资产已生成：{len(entries)} 个文件')


if __name__ == '__main__':
    main()
