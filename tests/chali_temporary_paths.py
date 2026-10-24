"""验证 Python 与原生工具只在 ChaLi/tmp 创建临时文件夹。"""
from pathlib import Path
import os
import subprocess
import runpy
from unittest.mock import patch
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """故意指定上级临时目录，确认仍使用仓库根目录下的 tmp。"""
    expected = (ROOT / 'tmp').resolve()
    with tempfile.TemporaryDirectory(prefix='temporary-policy-', dir=ROOT.parent) as directory:
        folder = Path(directory)
        assert folder.parent.resolve() == expected, folder
        source = folder / 'temporary.cpp'
        source.write_text('''#include "chali_temp.hpp"
#include <iostream>
/**
 * @brief 输出编译器内部使用的临时目录。
 * @return 成功时返回零。
 */
int main()
{
    std::cout << chali_temp::root().u8string() << "\\n";
    return 0;
}
''', encoding='utf-8')
        product = folder / ('temporary.exe' if os.name == 'nt' else 'temporary')
        built = subprocess.run(['g++', '-std=c++17', '-I' + str(ROOT / 'source'), str(source),
                                str(ROOT / 'source/chali_temp.cpp'), '-o', str(product)],
                               cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=60)
        assert built.returncode == 0, built.stderr
        forbidden = str(ROOT.parent / 'tmp')
        result = subprocess.run([str(product)], cwd=ROOT.parent,
                                env=dict(os.environ, TMP=forbidden, TEMP=forbidden, TMPDIR=forbidden),
                                capture_output=True, text=True, encoding='utf-8', timeout=15)
        assert result.returncode == 0 and Path(result.stdout.strip()).resolve() == expected, result
    print('临时目录：Python 指定外部路径和原生工具从上级目录启动时，均限定在 ChaLi/tmp')
    node = subprocess.run(['node', '-e',
                           'const p = require(process.argv[1]); '
                           'console.log(p.root); '
                           'for (const k of ["TMPDIR", "TMP", "TEMP"]) '
                           'if (process.env[k] !== p.root) process.exit(1);',
                           str(ROOT / 'vscode/test/projectTemp.js')],
                          cwd=ROOT.parent,
                          env=dict(os.environ, TMP=str(ROOT.parent), TEMP=str(ROOT.parent), TMPDIR=str(ROOT.parent)),
                          capture_output=True, text=True, encoding='utf-8', timeout=15)
    assert node.returncode == 0 and Path(node.stdout.strip()).resolve() == expected, node
    generator = (ROOT / 'source/generator/generator_cli.cpp').read_text(encoding='utf-8')
    assert 'const fs::path stage = mc / "tmp" /' in generator
    assert 'const fs::path stage = root / "tmp" /' not in generator
    dependencies = (ROOT / 'tools/native_development.cpp').read_text(encoding='utf-8')
    assert 'auto staging = tmp.path / "unpacked";' in dependencies
    assert 'auto staging = tmp.path / "checkout";' in dependencies
    assert 'libraries / (".download-' not in dependencies
    assert 'libraries / (".git-download-' not in dependencies
    print('临时目录：Node 子进程环境及工程生成器暂存路径通过')
    previous = Path.cwd()

    def latex(command, **kwargs):
        """替代排版进程，只验证逐章工具传入的隔离输出位置。"""
        output = Path(next(value.split('=', 1)[1] for value in command if value.startswith('-output-directory=')))
        assert expected in output.resolve().parents, output
        source = Path(command[-1])
        assert source.parent == output, source
        source.with_suffix('.log').write_text('', encoding='utf-8')
        return subprocess.CompletedProcess(command, 0)

    try:
        with patch('subprocess.run', latex):
            runpy.run_path(str(ROOT / 'doc/book/latex/tools/check_chapters.py'))
    finally:
        os.chdir(previous)
    print('临时目录：书稿逐章工具的源码与排版输出均位于 ChaLi/tmp')


if __name__ == '__main__':
    main()
