"""ABI 路径分隔符、导入模块、迁移目录与 raw 输入回归。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]
COMPILER = compiler_path()

def run(args, cwd=ROOT):
    result = subprocess.run(list(map(str, args)), cwd=cwd, capture_output=True, text=True, encoding='utf-8')
    assert result.returncode == 0, (result.returncode, args, result.stdout, result.stderr)
    return result

def main():
    options = argparse.ArgumentParser()
    options.add_argument('--compiler', type=Path, default=COMPILER)
    arguments = options.parse_args()
    compiler = arguments.compiler.resolve()
    (ROOT / 'tmp').mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='chali-platform-', dir=ROOT / 'tmp') as directory:
        work = Path(directory)
        folder = work / 'native paths'
        folder.mkdir()
        native = folder / 'native.c'
        native.write_text('#include <stdint.h>\n#ifdef _WIN32\n#define API __declspec(dllexport)\n#else\n#define API\n#endif\nAPI int32_t identity(int32_t n) { return n; }\n', encoding='utf-8')
        library = folder / ('native.dll' if os.name == 'nt' else 'native.so')
        run(['gcc', '-shared', *(['-fPIC'] if os.name != 'nt' else []), native, '-o', library])
        paths = [library.as_posix(), str(library).replace('/', '\\'), str(library).replace('/', '\\').replace('\\', '\\\\'), library.relative_to(work).as_posix().replace('/', '\\')]
        for index, path in enumerate(paths):
            source = work / f'path-{index}.chl'
            source.write_text('#abi("C", "' + path + '", "identity") int32 identity(int32 n);\nvoid main() { std::core::println("{}", identity((int32)7)); return; }\n', encoding='utf-8')
            prefix = work / f'path-{index}'
            result = run([compiler, 'run', source, '--out', prefix])
            assert result.stdout == '7\n' and (not result.stderr), result.stdout + result.stderr
            run([compiler, 'build', source, '--emit-llvm', '--out', prefix])
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            encoded = ''.join(f'\\{byte:02X}' for byte in library.as_posix().encode('utf-8'))
            assert encoded in llvm and 'chali_host_abi_symbol' in llvm, llvm[-2000:]
        nested = work / 'nested'
        nested.mkdir()
        signature = '#abi("C", "../native paths/' + library.name + '", "identity") int32 identity(int32 n);\n'
        (nested / 'api.chl').write_text('module nested::api;\n' + signature, encoding='utf-8')
        (nested / 'second.chl').write_text('module nested::second;\n' + signature, encoding='utf-8')
        imported = work / 'imported.chl'
        imported.write_text('import nested::api;\nvoid main() { std::core::println("{}", nested::api::identity((int32)8)); return; }\n', encoding='utf-8')
        included = work / 'included.chl'
        included.write_text('import nested::second;\nvoid main() { std::core::println("{}", nested::second::identity((int32)8)); return; }\n', encoding='utf-8')
        for source in (imported, included):
            prefix = work / source.stem
            result = run([compiler, 'run', source, '--out', prefix])
            assert result.stdout == '8\n', result.stdout + result.stderr
            run([compiler, 'build', source, '--emit-llvm', '--out', prefix])
            assert ''.join(f'\\{byte:02X}' for byte in library.as_posix().encode('utf-8')) in prefix.with_suffix('.ll').read_text(encoding='utf-8')
        # 同文模块移到另一目录后必须绑定新目录的库；旧库仍存在且返回不同值，避免仅靠缺失文件发现问题。
        relocated = work / 'relocated'
        (relocated / 'nested').mkdir(parents=True)
        (relocated / 'native paths').mkdir()
        (relocated / 'nested/api.chl').write_text((nested / 'api.chl').read_text(encoding='utf-8'), encoding='utf-8')
        relocated_source = relocated / 'imported.chl'
        relocated_source.write_text(imported.read_text(encoding='utf-8'), encoding='utf-8')
        relocated_native = relocated / 'native paths/native.c'
        relocated_native.write_text(native.read_text(encoding='utf-8').replace('return n;', 'return n + 100;'), encoding='utf-8')
        relocated_library = relocated / 'native paths' / library.name
        run(['gcc', '-shared', *(['-fPIC'] if os.name != 'nt' else []), relocated_native, '-o', relocated_library])
        relocated_result = run([compiler, 'run', relocated_source, '--out', relocated / 'output'])
        assert relocated_result.stdout == '108\n', relocated_result.stdout + relocated_result.stderr
        raw = work / 'raw.chl'
        raw.write_text('#abi("C", "./native paths/' + library.name + '", "identity") int32 identity(int32 n);\nvoid main() { std::core::println("{}", identity((int32)8)); return; }\n', encoding='utf-8')
        assert run([compiler, 'run', raw, '--raw', '--out', work / 'raw']).stdout == '8\n'
    print('ChaLi ABI path checks passed')
if __name__ == '__main__':
    main()
