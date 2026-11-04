"""实际执行中英文书稿的源码模块与 ThinLTO 导出示例。"""
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
    assert len(sources) == 2
    assert commands[0] == ['mkdir', '-p', 'build']
    assert [command[:2] for command in commands[1:]] == [['chali', 'run'], ['chali', 'aot']]
    return sources, commands[1:]


def main():
    """按书稿布局创建源码，并核对直接运行与 ThinLTO 导出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    chinese = chapter_example(root / 'doc/book/27-AOT导出.md', '### 从源码模块构建应用')
    english = chapter_example(root / 'doc/book/en/27-aot-export.md', '### Build an application with a source module')
    assert chinese == english, '中英文书稿代码和命令必须一致'
    sources, commands = chinese
    with tempfile.TemporaryDirectory(prefix='book-library-') as directory:
        work = Path(directory)
        (work / 'modules/example').mkdir(parents=True)
        (work / 'build').mkdir()
        provider = work / 'modules/example/counter.chl'
        provider.write_text(sources[0], encoding='utf-8')
        (work / 'main.chl').write_text(sources[1], encoding='utf-8')
        for command in commands:
            result = invoke([args.chali.resolve(), *command[1:]], work)
            if command[1] == 'run':
                assert result.stdout.strip() == '42', result.stdout
        executable = work / ('build/counter.exe' if os.name == 'nt' else 'build/counter')
        result = invoke([executable], work)
        assert result.stdout.strip() == '42', result.stdout
    print('书稿 AOT 示例：双语代码、源码模块运行及 ThinLTO 导出运行通过')


if __name__ == '__main__':
    main()
