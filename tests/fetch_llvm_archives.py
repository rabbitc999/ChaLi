"""验证 Python 3.11 的 LLVM 解包兼容路径允许包内链接并拒绝越界及特殊文件。"""
from pathlib import Path
import io
import sys
import tarfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from fetch_llvm_toolchain import extract_legacy_data
from project_temp import tempfile


def archive(members):
    """在内存中构造带指定文件和链接的测试归档。"""
    buffer = io.BytesIO()
    with tarfile.open(fileobj=buffer, mode='w') as output:
        for name, kind, value in members:
            entry = tarfile.TarInfo(name)
            entry.type = kind
            entry.mode = 0o755
            if kind == tarfile.REGTYPE:
                entry.size = len(value)
                output.addfile(entry, io.BytesIO(value))
            else:
                entry.linkname = value
                output.addfile(entry)
    buffer.seek(0)
    return tarfile.open(fileobj=buffer)


def main():
    """保留真实 LLVM 包使用的相对链接，拒绝可把文件写到解包根外的条目。"""
    with tempfile.TemporaryDirectory(prefix='llvm-archive-check-') as directory:
        work = Path(directory)
        good = work / 'good'
        good.mkdir()
        with archive([('llvm/bin/clang-23', tarfile.REGTYPE, b'compiler'),
                      ('llvm/bin/clang', tarfile.SYMTYPE, 'clang-23'),
                      ('llvm/bin/clang-copy', tarfile.LNKTYPE, 'llvm/bin/clang-23')]) as source:
            extract_legacy_data(source, good)
        assert (good / 'llvm/bin/clang').read_bytes() == b'compiler'
        assert (good / 'llvm/bin/clang-copy').read_bytes() == b'compiler'
        bad = (
            [('../escape', tarfile.REGTYPE, b'bad')],
            [('/escape', tarfile.REGTYPE, b'bad')],
            [('link', tarfile.SYMTYPE, '../escape')],
            [('link', tarfile.LNKTYPE, '../escape')],
            [('pipe', tarfile.FIFOTYPE, '')],
            [('C:/escape', tarfile.REGTYPE, b'bad')],
            [('link', tarfile.LNKTYPE, 'future'), ('future', tarfile.SYMTYPE, '../escape')],
        )
        for index, members in enumerate(bad):
            output = work / str(index)
            output.mkdir()
            try:
                with archive(members) as source:
                    extract_legacy_data(source, output)
            except ValueError:
                pass
            else:
                raise AssertionError(members)
        assert not (work / 'escape').exists()
    print('LLVM 解包兼容路径：普通文件、符号/硬链接与七种非法归档边界通过')


if __name__ == '__main__':
    main()
