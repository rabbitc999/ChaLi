"""验证静态 LLVM 编译器在没有工具链、LLVM DLL 和 clang PATH 时生成原生产物。"""
import argparse
import ctypes
import os
from pathlib import Path
import re
import shutil
import subprocess
from project_temp import tempfile
SOURCE = '''int64 sum(int64 n)
{
    List<int64> values;
    int64 index = 0;
    while (index < n) { values.push(index); index += 1; }
    int64 total = 0;
    for (int64 value in values) { total += value; }
    return total;
}
void main() { println("static LLVM {}", sum(10)); }
'''


def main():
    """复制单个编译器到私有目录并实际验证内置代码生成、链接和 C 导出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, required=True)
    args = parser.parse_args()
    if os.name != 'nt':
        parser.error('本回归验收 Windows x64 静态 LLVM 分发')
    imports = subprocess.check_output(['objdump', '-p', str(args.chali.resolve())], text=True)
    dependencies = re.findall(r'DLL Name:\s*(\S+)', imports)
    assert not any(re.search(r'LLVM|clang|libstdc\+\+|libgcc|winpthread|libc\+\+', name, re.I) for name in dependencies), dependencies
    with tempfile.TemporaryDirectory(prefix='chali-llvm-standalone-') as directory:
        root = Path(directory)
        compiler = root / 'ChaLi.exe'
        shutil.copy2(args.chali, compiler)
        source = root / 'standalone.chl'
        source.write_text(SOURCE, encoding='utf-8')
        env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1',
                   PATH=str(Path(os.environ['SystemRoot']) / 'System32'),
                   CHALI_LLVM_CLANG=str(root / 'clang-does-not-exist.exe'))
        prefix = root / 'program'
        result = subprocess.run([str(compiler), 'aot', str(source), '--out', str(prefix)], cwd=root,
                                env=env, capture_output=True, text=True, encoding='utf-8', timeout=180)
        assert result.returncode == 0, result
        result = subprocess.run([str(prefix.with_suffix('.exe'))], cwd=root, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=30)
        assert (result.returncode, result.stdout) == (0, 'static LLVM 45\n'), result
        source.write_text(SOURCE.split('void main()', 1)[0].replace('int64 sum(', 'export int64 sum('), encoding='utf-8')
        library = root / 'shared'
        result = subprocess.run([str(compiler), 'aot', str(source), '--shared', '--out', str(library)],
                                cwd=root, env=env, capture_output=True, text=True, encoding='utf-8', timeout=180)
        assert result.returncode == 0, result
        native = ctypes.CDLL(str(library.with_suffix('.dll')))
        native.f_sum.argtypes = [ctypes.c_int64]
        native.f_sum.restype = ctypes.c_int64
        native.chali_init()
        assert native.f_sum(100) == 4950
        native.chali_finish()
        ctypes.windll.kernel32.FreeLibrary(ctypes.c_void_p(native._handle))
    print('静态 LLVM：单文件隔离编译、无 LLVM DLL、无 clang PATH、可执行文件与 C ABI 共享库全部通过')


if __name__ == '__main__':
    main()
