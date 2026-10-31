"""分别编译 C 宿主与 C++ 内部测试，连接同一运行时库验证 C API。"""
from pathlib import Path
import argparse
import json
import os
import shutil
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """验证公开 C 代码、事务失败回退及真实运行时服务。"""
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    static = os.name == 'nt' and (sdk / 'bin/g++.exe').is_file()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, help='供模块门禁兼容，单元直接连接运行时')
    parser.add_argument('--build-root', type=Path, default=ROOT / ('build/llvm-static-gcc16' if static else 'build'))
    parser.add_argument('--cxx')
    parser.add_argument('--cc')
    parser.add_argument('--sysroot', type=Path)
    args = parser.parse_args()
    build = args.build_root.resolve()
    use_sdk = static and build == (ROOT / 'build/llvm-static-gcc16').resolve()
    args.cxx = args.cxx or (str(sdk / 'bin/g++.exe') if use_sdk else 'g++')
    sdk_compiler = Path(args.cxx).resolve().parent == (sdk / 'bin').resolve()
    args.cc = args.cc or (str(sdk / 'bin/gcc.exe') if sdk_compiler else 'gcc')
    args.sysroot = args.sysroot or (sdk if sdk_compiler else None)
    libraries = [build / 'chali-ircode-aot-core/libstackvm.a', build / 'chali-host-records/librecords.a']
    assert all(p.is_file() for p in libraries), '先构建匹配配置的运行时和宿主记录库'
    environment = dict(os.environ)
    environment.setdefault('CHALI_ABI_CXX', shutil.which('g++') or 'g++')
    environment['PATH'] = str(Path(args.cxx).resolve().parent) + os.pathsep + environment.get('PATH', '')
    with tempfile.TemporaryDirectory(prefix='chali-c-api-registry-') as directory:
        work = Path(directory)
        def run(*command):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=environment,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=180)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            return result.stdout
        flags = ['-O1', '-g', '-DCHALI_CUSTOM_ALLOCATOR=1', '-DSTACKVM_AOT_ONLY=1',
                 '-DSTACKVM_WITH_REFLECTION=1', '-DCHALI_WITH_REFLECTION=1',
                 '-I' + str(ROOT / 'source'), '-I' + str(ROOT / 'source/abi'),
                 '-I' + str(ROOT / 'source/ircode'), '-I' + str(ROOT / 'library')]
        if args.sysroot:
            flags += ['--sysroot=' + str(args.sysroot.resolve())]
        system = work / 'system_arena.o'
        run(args.cxx, '-std=c++17', *flags, '-c', ROOT / 'source/allocator/system_arena.cpp', '-o', system)
        sources = [('c-host', ROOT / 'example/host-c-api/main.c', args.cc, 'c11'),
                   ('registry', ROOT / 'tests/chali_c_api_registry.cpp', args.cxx, 'c++17'),
                   ('services', ROOT / 'tests/chali_c_api_runtime.cpp', args.cxx, 'c++17')]
        for label, source, compiler, standard in sources:
            obj = work / (label + '.o')
            executable = work / (label + ('.exe' if os.name == 'nt' else ''))
            run(compiler, '-std=' + standard, *flags, '-c', source, '-o', obj)
            link = [args.cxx, *flags, '-Wl,--gc-sections', '-o', executable, obj, system,
                    '-Wl,--start-group', *libraries, '-Wl,--end-group', '-pthread']
            if os.name == 'nt':
                link += ['-static', '-fuse-ld=lld', '-lws2_32', '-lwinhttp', '-lbcrypt', '-lcrypt32']
            else:
                link += ['-ldl']
            run(*link)
            print(label + ': ' + run(executable).strip(), flush=True)
        if args.chali:
            provider = work / ('callback.dll' if os.name == 'nt' else 'callback.so')
            run(args.cc, '-std=c11', *flags, '-shared', '-fPIC',
                ROOT / 'tests/fixtures/c_api_owned_callback.c', '-o', provider)
            source = work / 'library.chl'
            source.write_text('import std::parse;\n'
                '#abi("C", ' + json.dumps(provider.as_posix()) + ', "owned_after")\n'
                'String ownedAfter(int64 fn(int64) callback,int64 value) ["release_owned"];\n'
                '#export("calculate")\n'
                'int64 calculate(int64 input){int64 offset=2;'
                'let callback=int64 fn copy(int64 value){return value+offset;};'
                'String text=ownedAfter(callback,input);'
                'return std::parse::Parse::parseInt64(text).unwrap();}\n', encoding='utf-8')
            obj = work / 'native-host.o'
            executable = work / ('native-host.exe' if os.name == 'nt' else 'native-host')
            run(args.cc, '-std=c11', *flags, '-c', ROOT / 'tests/fixtures/c_api_library_host.c', '-o', obj)
            link = [args.cxx, *flags, '-Wl,--gc-sections', '-o', executable, obj, system,
                    '-Wl,--start-group', *libraries, '-Wl,--end-group', '-pthread']
            link += ['-static', '-fuse-ld=lld', '-lws2_32', '-lwinhttp', '-lbcrypt', '-lcrypt32'] if os.name == 'nt' else ['-ldl']
            run(*link)
            for allocator in ('malloc', 'pool'):
                for checks in ('checked', 'unchecked'):
                    prefix = work / ('language-' + allocator + '-' + checks)
                    run(args.chali.resolve(), 'aot', source, '--shared', '--allocator', allocator, '--' + checks, '--out', prefix)
                    library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
                    output = run(executable, library, provider)
                    assert output == 'C registry -> native language -> captured C callback: 42, owned result released once\n', output
                    print(allocator + '/' + checks + ': ' + output.strip(), flush=True)


if __name__ == '__main__':
    main()
