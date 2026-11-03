"""验证默认产物位于源码旁、隐藏工作区清理及失败时的既有文件保护。"""
from pathlib import Path
import argparse
import ctypes
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """从不同工作目录执行真实编译器，分别核对各导出格式和运行路径。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--llvm', type=Path)
    options = parser.parse_args()
    compiler = options.chali.resolve()
    backend = ['--llvm', str(options.llvm.resolve())] if options.llvm else []
    with tempfile.TemporaryDirectory(prefix='default-outputs-') as directory:
        work = Path(directory)
        source_directory = work / 'source 中文'
        source_directory.mkdir()
        caller = work / 'caller'
        caller.mkdir()
        source = source_directory / 'example.chl'
        valid = 'void main(){println("default output OK");}\n'
        source.write_text(valid, encoding='utf-8')
        executable = source.with_suffix('.exe' if os.name == 'nt' else '')
        workspace = source_directory / '.chali'

        def invoke(command, success=True):
            """执行有界命令并检查退出状态，不把失败诊断当作成功输出。"""
            result = subprocess.run(list(map(str, command)), cwd=caller,
                                    env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                    text=True, encoding='utf-8', errors='replace', timeout=180)
            assert (result.returncode == 0) == success, result
            if not success:
                assert result.stderr, result
            else:
                assert not result.stderr, result
            return result

        def cleaned():
            """确认自动工作区和调用目录没有遗留中间产物。"""
            assert not workspace.exists(), list(workspace.rglob('*')) if workspace.exists() else []
            assert not (caller / '.chali').exists()
            assert not list(source_directory.glob('*.quad'))
            assert not list(source_directory.glob('*.llvm.log'))

        invoke([compiler, 'aot', source, *backend])
        assert executable.is_file()
        assert invoke([executable]).stdout == 'default output OK\n'
        cleaned()
        assert invoke([compiler, 'run', source, *backend]).stdout == 'default output OK\n'
        assert executable.is_file()
        cleaned()
        for flag, suffix in [('--emit-obj', '.o'), ('--emit-asm', '.s'), ('--emit-llvm', '.ll')]:
            invoke([compiler, 'aot', source, flag, *backend])
            assert source.with_suffix(suffix).stat().st_size > 0
            cleaned()
        saved = executable.read_bytes()
        source.write_text('void main(){missingName();}\n', encoding='utf-8')
        invoke([compiler, 'aot', source, *backend], False)
        assert executable.read_bytes() == saved
        cleaned()
        source.write_text(valid, encoding='utf-8')
        invoke([compiler, 'aot', source, '--llvm', work / 'missing-clang'], False)
        assert executable.read_bytes() == saved
        cleaned()
        source.write_text('void main(){assert(false);}\n', encoding='utf-8')
        invoke([compiler, 'run', source, *backend], False)
        cleaned()
        source.write_text(valid, encoding='utf-8')
        workspace.mkdir()
        marker = workspace / 'user-file.txt'
        marker.write_text('preserve user data', encoding='utf-8')
        invoke([compiler, 'run', source, *backend])
        assert marker.read_text(encoding='utf-8') == 'preserve user data'
        assert list(workspace.iterdir()) == [marker]
        marker.unlink()
        workspace.rmdir()
        workspace.write_text('not a directory', encoding='utf-8')
        invoke([compiler, 'aot', source, *backend], False)
        assert workspace.read_text(encoding='utf-8') == 'not a directory'
        workspace.unlink()
        api = source_directory / 'api.chl'
        api.write_text('export int64 add(int64 value){return value+1;}\n', encoding='utf-8')
        invoke([compiler, 'aot', api, '--shared', '--emit-llvm', *backend])
        assert 'f_add(' in api.with_suffix('.h').read_text(encoding='utf-8')
        cleaned()
        invoke([compiler, 'aot', api, '--shared', *backend])
        library = api.with_suffix('.dll' if os.name == 'nt' else '.so')
        driver = source_directory / 'driver.c'
        driver.write_text('#include "api.h"\nint main(void){chali_init();int ok=f_add(41)==42;chali_finish();return !ok;}\n', encoding='utf-8')
        host = source_directory / ('driver.exe' if os.name == 'nt' else 'driver')
        invoke(['gcc', driver, library, *([] if os.name == 'nt' else ['-Wl,-rpath,$ORIGIN']), '-o', host])
        invoke([host])
        cleaned()
        invoke([compiler, 'aot', source, '--keep-artifacts', *backend])
        assert list(workspace.glob('build-*/*.ll'))
        if os.name == 'nt':
            kernel = ctypes.WinDLL('kernel32', use_last_error=True)
            kernel.GetFileAttributesW.argtypes = [ctypes.c_wchar_p]
            kernel.GetFileAttributesW.restype = ctypes.c_uint32
            assert kernel.GetFileAttributesW(str(workspace)) & 2
        assert not (caller / '.chali').exists()
    print('default outputs: source-adjacent exe/object/assembly/LLVM IR, hidden workspace, cleanup and failure preservation passed')


if __name__ == '__main__':
    main()
