"""CHALI project packages: metadata, includes/imports, origins and cache invalidation."""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import shutil
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]
COMPILER = compiler_path()

def run(args, success=True, cwd=ROOT, input=None, env=None):
    result = subprocess.run(list(map(str, args)), cwd=cwd, input=input, capture_output=True, text=True, encoding='utf-8', env=env or dict(os.environ, CHALI_LANG='zh'))
    assert (result.returncode == 0) == success, f'exit={result.returncode}\n' + result.stdout + result.stderr
    return result

def main():
    global COMPILER
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=COMPILER)
    COMPILER = parser.parse_args().chali.resolve()
    (ROOT / 'tmp').mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='chali-package-', dir=ROOT / 'tmp') as directory:
        work = Path(directory)
        project = work / 'app'
        shutil.copytree(ROOT / 'tests/fixtures/package', project)
        manifest = project / 'chali.toml'
        native = project / 'include/native.c'
        native.write_text('#include <stdint.h>\n#ifdef _WIN32\n#define API __declspec(dllexport)\n#else\n#define API\n#endif\nAPI int32_t identity(int32_t n) { return n; }\n', encoding='utf-8')
        library = project / 'include' / ('native.dll' if os.name == 'nt' else 'native.so')
        run(['gcc', '-shared', *(['-fPIC'] if os.name != 'nt' else []), native, '-o', library])
        (project / 'include/nativeapi.chl').write_text('module nativeapi;\n#abi("C", "./' + library.name + '", "identity") int32 nativeIdentity(int32 n);\n', encoding='utf-8')
        original = manifest.read_text(encoding='utf-8').replace('commands = [', 'commands = [\n    \'import nativeapi;\',')
        manifest.write_text(original, encoding='utf-8')
        source = project / 'src/main.chl'
        prefix = work / 'program'
        args = [COMPILER, 'run', source, '--out', prefix]

        def execute(*extra, success=True):
            """在项目目录里执行：`#abi` 的动态库放在 include 下，运行时要能从 PATH 找到它。"""
            env = dict(os.environ, CHALI_LANG='zh',
                       PATH=str(project / 'include') + os.pathsep + os.environ.get('PATH', ''))
            return run([*args, *extra], success=success, cwd=project, env=env)
        source.write_text('/*\nmodule hidden;\nimport absent;\n*/\n' + source.read_text(encoding='utf-8').replace('    std::lang::println("chali-ok");', '    std::lang::assert(nativeapi::nativeIdentity((int32)7) == 7);\n    std::lang::println("chali-ok");'), encoding='utf-8')
        assert execute().stdout == 'chali-ok\n'
        emitted = [*args, '--emit', '--incremental']
        first = run(emitted)
        assert library.as_posix() in prefix.with_suffix('.abi.cpp').read_text(encoding='utf-8')
        quad = prefix.with_suffix('.quad')
        stamp = quad.stat().st_mtime_ns
        cached = run(emitted)
        assert '增量缓存命中' in cached.stderr, cached.stderr
        assert quad.stat().st_mtime_ns == stamp
        dependencies = execute('--deps').stdout.replace('\\', '/')
        for file in (manifest, source, project / 'include/shared.chl', project / 'modules/tools/math.chl'):
            assert file.as_posix() in dependencies, dependencies
        manifest.write_text(original.replace('version = "0.1.0"', 'version = "0.1.1"'), encoding='utf-8')
        rebuilt = run(emitted)
        assert '增量缓存命中' not in rebuilt.stderr, rebuilt.stderr
        (project / 'include/extra.chl').write_text('module extra;\nint64 extraValue() { return 2; }\n', encoding='utf-8')
        assert '增量缓存命中' not in run(emitted).stderr
        (project / 'include/extra.chl').write_text('module extra;\nint64 extraValue() { return 1; }\n', encoding='utf-8')
        manifest.write_text(original, encoding='utf-8')
        assert execute().stdout == 'chali-ok\n'
        (project / 'include/extra.chl').write_text('module extra;\nint64 extraValue( { return 1; }\n', encoding='utf-8')
        failed = execute(success=False)
        assert 'extra.chl' in failed.stderr and '第 2 行' in failed.stderr, failed.stderr
        (project / 'include/extra.chl').write_text('module extra;\nint64 extraValue() { return 1; }\n', encoding='utf-8')
        manifest.write_text(original.replace('tools::math', 'missing::module'), encoding='utf-8')
        assert 'missing::module' in execute(success=False).stderr
        for invalid in ('[package]\nname = "a"\nname = "b"\n', '[chali]\ncommands = ["%define BAD 1"]\n', '[chali]\nimport = ["bad::"]\n', '[chali]\ninclude = ["missing.inc"]\n', '[other]\ninclude = []\n', '[package]\nversion = 42\n', '[chali]\ndebug = "yes"\n', '[chali]\nrun = "bogus"\n', '[chali]\ninclude = ["unfinished"\n'):
            manifest.write_text(invalid, encoding='utf-8')
            assert execute(success=False).stderr
        manifest.write_text(original, encoding='utf-8')
        nested = project / 'src/nested'
        nested.mkdir()
        (nested / 'chali.toml').write_text('[package]\nname="nested"\nversion="1.0.0"\nauthor="A"\n', encoding='utf-8')
        nested_source = nested / 'main.chl'
        nested_source.write_text('void main() { }\n', encoding='utf-8')
        assert run([COMPILER, 'run', nested_source, '--out', work / 'nested']).stdout == ''
        binary = work / 'binary'

        def product(prefix):
            """返回 AOT 导出产物路径（Windows 带 .exe）。"""
            return Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))

        # 项目 `debug = true` 等价于命令行 `--debug`：不裁剪符号、降低优化级别，产物更大。
        run([COMPILER, 'aot', source, '--out', binary], cwd=project)
        released = product(binary).stat().st_size
        manifest.write_text(original.replace('[chali]', '[chali]\ndebug = true'), encoding='utf-8')
        run([COMPILER, 'aot', source, '--out', binary], cwd=project)
        assert product(binary).stat().st_size > released
        manifest.write_text(original, encoding='utf-8')
        run([COMPILER, 'aot', source, '--out', binary, '--debug'], cwd=project)
        assert product(binary).stat().st_size > released
        run([COMPILER, 'aot', source, '--out', binary, '--no-debug'], cwd=project)
        assert product(binary).stat().st_size == released
        manifest.write_text(original.replace('[chali]', '[chali]\nencrypt = true'), encoding='utf-8')
        rejected = run([COMPILER, 'run', source, '--out', binary], success=False, cwd=project)
        assert 'unknown key: chali.encrypt' in rejected.stderr
        manifest.write_text(original, encoding='utf-8')
        rejected = run([COMPILER, 'svmb', source, '--out', binary], success=False)
        assert '.svmb' in rejected.stderr
        rejected = run([COMPILER, 'run', source, '--out', binary, '--encrypt'], success=False, cwd=project)
        assert '--encrypt' in rejected.stderr
        manifest.write_text(original + '\n[embedded]\nenabled = true\nmax_memory = 1024\nrestrict_modules = ["std::net"]\n', encoding='utf-8')
        rejected = run([COMPILER, 'run', source, '--out', binary], success=False)
        assert 'unknown or repeated table: embedded' in rejected.stderr, rejected.stderr
        manifest.write_text(original, encoding='utf-8')
        probe = work / 'metadata.cpp'
        probe.write_text('#include "chali_package.hpp"\n#include <iostream>\nint main(int argc, char **argv) { chali_package::Project p; Memory::String e; if (!chali_package::read(std::filesystem::u8path(argv[1]), p, e)) { std::cerr << e.c_str() << "\\n"; return 1; } std::cout << p.name << "\\n" << p.version << "\\n" << p.authors.at(0) << "\\n" << (p.debug ? "debug=1" : "debug=0") << "\\n" << p.run << "\\n"; }\n', encoding='utf-8')
        exe = work / ('metadata.exe' if os.name == 'nt' else 'metadata')
        run(['g++', '-std=c++17', '-I', ROOT / 'source', '-I', ROOT / 'library', probe, ROOT / 'source/chali_package.cpp', ROOT / 'source/toml/toml.cpp', ROOT / 'source/toml/toml_source.cpp', ROOT / 'source/json/json.cpp', '-o', exe])
        assert run([exe, manifest]).stdout == 'chali-demo\n0.1.0\n示例作者\ndebug=0\nrun\n'
        manifest.write_text(original.replace('[chali]', '[chali]\ndebug = true'), encoding='utf-8')
        assert run([exe, manifest]).stdout == 'chali-demo\n0.1.0\n示例作者\ndebug=1\nrun\n'
        # `interpret` 是 1.0.8 之前的名字，仍归一化到唯一的运行方式；`smvb` / `svmb` 一律拒绝。
        manifest.write_text(original.replace('[chali]', '[chali]\nrun = "interpret"'), encoding='utf-8')
        assert run([exe, manifest]).stdout == 'chali-demo\n0.1.0\n示例作者\ndebug=0\nrun\n'
        manifest.write_text(original.replace('[chali]', '[chali]\nrun = "smvb"'), encoding='utf-8')
        rejected = run([exe, manifest], success=False)
        assert '.svmb was removed' in rejected.stderr, rejected.stderr
        manifest.write_text(original, encoding='utf-8')
    print('CHALI package checks passed')
if __name__ == '__main__':
    main()
