"""验证文件 C 入口兼容行为与 Windows UTF-8 文件及父目录。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """兼容用例可单独运行于迁移前版本，Unicode 修复在新版本完整核对。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--legacy-only', action='store_true')
    compiler = parser.parse_args()
    legacy_only = compiler.legacy_only
    compiler = compiler.chali.resolve()
    template = (ROOT / 'tests/fixtures/file_compatibility.chl').read_text(encoding='utf-8')
    with tempfile.TemporaryDirectory(prefix='file-c-api-') as directory:
        work = Path(directory)

        def run(command):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'),
                                    capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=300)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            return result

        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                tree = work / f'{allocator}-{checks}'
                tree.mkdir()
                source = work / f'{allocator}-{checks}.chl'
                source.write_text(template.replace('__ROOT__', json.dumps(tree.as_posix())), encoding='utf-8')
                target = work / f'{allocator}-{checks}.exe'
                run([compiler, 'aot', source, '--allocator', allocator, '--' + checks, '--out', target])
                result = run([target])
                assert result.stdout == 'file legacy lines, CRLF, bytes, modes, seek and cleanup OK\n' and not result.stderr, result
                print(f'{allocator}/{checks}: legacy lines, raw bytes, modes, seek, errors and ownership passed', flush=True)
                if legacy_only:
                    continue
                unicode_tree = work / f'{allocator}-{checks}-中文目录'
                unicode_tree.mkdir()
                paths = [tree / '中文文件.txt', unicode_tree / '文件.txt']
                source = work / f'{allocator}-{checks}-unicode.chl'
                code = 'import std::io;\nvoid main(){\n'
                for path in paths:
                    literal = json.dumps(path.as_posix(), ensure_ascii=False)
                    code += f'assert(writeAllText({literal},"中文😀").isOk());assert(readAllText({literal}).unwrap()=="中文😀");assert(fileInfo({literal}).unwrap().isFile);\n'
                code += '}\n'
                source.write_text(code, encoding='utf-8')
                target = work / f'{allocator}-{checks}-unicode.exe'
                run([compiler, 'aot', source, '--allocator', allocator, '--' + checks, '--out', target])
                result = run([target])
                assert not result.stdout and not result.stderr, result
                for path in paths:
                    assert path.read_bytes() == '中文😀'.encode('utf-8'), path
                print(f'{allocator}/{checks}: actual Unicode filename and parent directory bytes verified', flush=True)


if __name__ == '__main__':
    main()
