"""在真实导出工程中编译公开 C 头和注册句柄私有头，检查重新定位的依赖。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """生成新工程，分别用 C11 和 C++17 检查实际导出的头文件。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--cc', default='gcc')
    parser.add_argument('--cxx', default='g++')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='c-api-export-') as folder:
        work = Path(folder)
        project = work / 'project'
        environment = dict(os.environ, CHALI_LANG='zh')

        def run(*command):
            result = subprocess.run([str(x) for x in command], cwd=work, env=environment,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=180)
            assert result.returncode == 0, (command, result.stdout, result.stderr)

        run(args.chali.resolve(), 'gen', 'compiler', '--out-dir', project)
        for compiler, standard, extension, headers in (
            (args.cc, 'c11', 'c', ['abi/chali_abi.h', 'abi/chali_standard.h']),
            (args.cxx, 'c++17', 'cpp', ['abi/chali_abi_registry.hpp', 'abi/chali_abi_runtime.hpp'])):
            source = work / ('headers.' + extension)
            source.write_text(''.join('#include "' + name + '"\n' for name in headers) + 'int main(void){return 0;}\n', encoding='utf-8')
            run(compiler, '-std=' + standard, '-fsyntax-only', '-I' + str(project), '-I' + str(project / 'runtime'), source)
        assert not (project / 'glue/std_parse.cpp').exists()
        assert not (project / 'glue/std_parse.hpp').exists()
        assert (project / 'abi/abi_parse.c').is_file()
        embedded = (project / 'chali_stdlib_embedded.cpp').read_text(encoding='utf-8')
        assert '"ircode/native_protocol.hpp"' in embedded
        assert '"ircode/runtime_internal.hpp"' in embedded
    print('fresh compiler export: C11 public and C++17 private C API headers compile; old parse glue absent')


if __name__ == '__main__':
    main()
