"""实际执行中英文书稿的多文件原生库交付命令，并移走提供方实现源码。"""
import argparse
import os
from pathlib import Path
import re
import shlex
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke


def chapter_example(path, heading):
    """提取书稿中的原样代码和命令，拒绝未核对的命令入口。"""
    text = path.read_text(encoding='utf-8').split(heading, 1)[1].split('## 27.3 ', 1)[0]
    sources = re.findall(r'^```chali\n(.*?)^```', text, re.MULTILINE | re.DOTALL)
    commands = [shlex.split(line) for block in re.findall(r'^```sh\n(.*?)^```', text, re.MULTILINE | re.DOTALL)
                for line in block.splitlines() if line.strip()]
    expected = re.findall(r'^```text\n(.*?)^```', text, re.MULTILINE | re.DOTALL)
    assert len(sources) == 2 and len(commands) == 5 and expected == ['42\n']
    assert all(command[0] == 'chali' for command in commands)
    return sources, commands


def main():
    """按书稿构建、移走源码、检查、原生运行及 ThinLTO 导出运行。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    chinese = chapter_example(root / 'doc/book/27-AOT导出.md', '### 交付一个原生语言库')
    english = chapter_example(root / 'doc/book/en/27-aot-export.md', '### Deliver a native language library')
    assert chinese == english, '中英文书稿代码和命令必须一致'
    sources, commands = chinese
    with tempfile.TemporaryDirectory(prefix='book-library-') as directory:
        work = Path(directory)
        (work / 'modules/example').mkdir(parents=True)
        (work / 'build').mkdir()
        provider = work / 'counter.chl'
        provider.write_text(sources[0], encoding='utf-8')
        (work / 'main.chl').write_text(sources[1], encoding='utf-8')
        for index, command in enumerate(commands):
            result = invoke([args.chali.resolve(), *command[1:]], work)
            if index == 0:
                provider.unlink()
            if command[1] == 'run':
                assert result.stdout.strip() == '42', result.stdout
        executable = work / ('build/counter.exe' if os.name == 'nt' else 'build/counter')
        result = invoke([executable], work)
        assert result.stdout.strip() == '42', result.stdout
    print('书稿原生库：双语原样代码、源码移走、检查、原生运行及 bitcode ThinLTO 导出运行通过')


if __name__ == '__main__':
    main()
