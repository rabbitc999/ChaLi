"""核对无多线程运行的中英文错误及失败后的默认工作区清理。"""
import argparse
import os
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke


def main():
    """实际触发原生服务错误，不把消息表存在当作翻译已生效。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source = (root / 'example/samples-error/thread_disabled.chl').read_text(encoding='utf-8')
    with tempfile.TemporaryDirectory(prefix='thread-diagnostics-') as directory:
        work = Path(directory)
        for language, expected in [
            ('zh', '多线程已关闭；run／aot 构建时去掉 --no-threads'),
            ('en', 'Multithreading is disabled; remove --no-threads when building with run/aot'),
        ]:
            file = work / (language + '.chl')
            file.write_text(source, encoding='utf-8')
            os.environ['CHALI_LANG'] = language
            result = invoke([args.chali.resolve(), 'run', file, '--no-threads', '--quiet'], work, False)
            assert expected in result.stderr, result.stderr
            assert '解释运行' not in result.stderr and not result.stdout, result
            assert not (work / '.chali').exists(), '运行失败后遗留默认工作区'
    print('线程关闭诊断：原生错误中英文、过时用法清除及 .chali 清理通过')


if __name__ == '__main__':
    main()
