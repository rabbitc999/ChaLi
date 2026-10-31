"""核对文件系统 C 入口的四配置、Unicode、排序、元数据和可恢复错误。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """每个配置使用独立的临时文件树，不修改仓库外或已有样例目录。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    template = (ROOT / 'tests/fixtures/filesystem_compatibility.chl').read_text(encoding='utf-8')
    with tempfile.TemporaryDirectory(prefix='filesystem-c-api-') as directory:
        work = Path(directory)

        def run(command):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'),
                                    capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=300)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            return result

        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                tree = work / f'{allocator}-{checks}-中文'
                tree.mkdir()
                for name, content in (('a.txt', b'a'), ('z.txt', b'zzz'), ('中文 file.txt', b'unicode')):
                    (tree / name).write_bytes(content)
                stamp = tree / 'stamp.txt'
                stamp.write_bytes(b'stamp')
                os.utime(stamp, (946684800, 946684800))
                source = work / f'{allocator}-{checks}.chl'
                source.write_text(template.replace('__ROOT__', json.dumps(tree.as_posix(), ensure_ascii=False)), encoding='utf-8')
                target = work / f'{allocator}-{checks}.exe'
                run([compiler, 'aot', source, '--allocator', allocator, '--' + checks, '--out', target])
                result = run([target])
                assert result.stdout == 'filesystem paths, metadata, copies and errors OK\n' and not result.stderr, result
                print(f'{allocator}/{checks}: sorted Unicode paths, owned copies, UTC and all error classes passed', flush=True)
        sample = work / 'extended.chl'
        text = (ROOT / 'example/samples/std_io_extended.chl').read_text(encoding='utf-8')
        sample.write_text(text.replace('"build/std-io-extended"', json.dumps((work / 'extended').as_posix())), encoding='utf-8')
        result = run([compiler, 'run', sample, '--out', work / 'extended-sample'])
        assert result.stdout == (ROOT / 'example/expected/std_io_extended.txt').read_text(encoding='utf-8') and not result.stderr, result
        print('existing whole-file, stream, directory and raw-byte sample passed in isolated temporary tree')


if __name__ == '__main__':
    main()
