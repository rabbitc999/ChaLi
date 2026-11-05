"""以 C11 编译 C 服务，再验证网络、索引及真实集合存储的所有权和失败恢复。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """独立编译被测服务，避免使用宿主构建中可能陈旧的对象。"""
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    use_sdk = os.name == 'nt' and (sdk / 'bin/g++.exe').is_file()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default=str(sdk / 'bin/gcc.exe') if use_sdk else 'gcc')
    parser.add_argument('--cxx', default=str(sdk / 'bin/g++.exe') if use_sdk else 'g++')
    parser.add_argument('--sysroot', type=Path)
    parser.add_argument('--sanitize', action='store_true')
    parser.add_argument('--chali', type=Path, help='模块门禁兼容参数；本测试直接编译 C 服务')
    args = parser.parse_args()
    sysroot = args.sysroot
    if not sysroot and Path(args.cc).resolve().parent == (sdk / 'bin').resolve():
        sysroot = sdk
    flags = ['-O1' if args.sanitize else '-O2', '-g', '-Wall', '-Wextra', '-ffunction-sections', '-fdata-sections',
             '-Isource/bignum', '-Isource/runtime_c']
    if sysroot:
        flags += ['--sysroot=' + str(sysroot.resolve())]
    if args.sanitize:
        assert os.name != 'nt', 'ASan/UBSan 验收使用 Linux 工具链'
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    environment = dict(os.environ)
    if Path(args.cc).is_file():
        environment['PATH'] = str(Path(args.cc).resolve().parent) + os.pathsep + environment.get('PATH', '')
    work = Path(tempfile.mkdtemp(prefix='c-runtime-storage-'))
    records = []

    def execute(label, command, *, run=False):
        """保留精确命令和标准输出，失败立即停止并保留诊断。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=environment, capture_output=True,
                                text=True, encoding='utf-8', errors='replace', timeout=180)
        records.append(dict(label=label, command=list(map(str, command)), code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'results.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
        assert result.returncode == 0, (records[-1], work)
        if run:
            assert 'passed' in result.stdout, (records[-1], work)
            print(result.stdout.strip(), flush=True)

    objects = []
    for name in ('memory', 'buffer', 'sequence', 'index'):
        target = work / (name + '.o')
        execute('C11/' + name, [args.cc, '-std=c11', *flags, '-Werror', '-c',
                                 ROOT / 'source/runtime_c' / ('chali_' + name + '.c'), '-o', target])
        objects.append(target)
    network = work / 'network.o'
    execute('C11/network', [args.cc, '-std=c11', *flags, '-Werror', '-c', ROOT / 'source/abi/chali_net.c', '-o', network])
    thread = work / 'thread.o'
    execute('C11/thread', [args.cc, '-std=c11', *flags, '-pthread', '-Werror', '-c', ROOT / 'source/abi/chali_thread.c', '-o', thread])
    extension = '.exe' if os.name == 'nt' else ''
    linker = ['-Wl,--gc-sections'] + (['-static', '-fuse-ld=lld'] if use_sdk and sysroot else [])
    for name, source, units in [('network', 'chali_net_c_test.c', [network]),
                                 ('index', 'chali_c_index_test.c', objects)]:
        binary = work / (name + extension)
        execute('build/' + name, [args.cc, '-std=c11', *flags, ROOT / 'tests' / source, *units, *linker, '-o', binary])
        execute('run/' + name, [binary], run=True)
    integers = []
    for source in sorted((ROOT / 'source/bignum').glob('chali_bigint*.c')):
        target = work / (source.stem + '.o')
        execute('C11/' + source.stem, [args.cc, '-std=c11', *flags, '-Werror', '-c', source, '-o', target])
        integers.append(target)
    structure = work / 'cas-structure.o'
    execute('C11/cas-structure', [args.cc, '-std=c11', *flags, '-Werror', '-Ilibrary/caspp/include', '-c',
                                  ROOT / 'library/caspp/src/detail/node_structure.c', '-o', structure])
    binary = work / ('cas-structure' + extension)
    execute('build/cas-structure', [args.cc, '-std=c11', *flags, '-Ilibrary/caspp/include',
                                   ROOT / 'tests/chali_cas_structure_c.c', structure, *integers,
                                   *objects, *linker, '-lm', '-o', binary])
    execute('run/cas-structure', [binary], run=True)
    for collisions in (False, True):
        name = 'cas-symbols' + ('-collisions' if collisions else '')
        definitions = ['-DCAS_SYMBOL_TABLE_TESTING_COLLISIONS'] if collisions else []
        symbol = work / (name + '.o')
        execute('C11/' + name, [args.cc, '-std=c11', *flags, '-Werror', *definitions, '-Ilibrary/caspp/include', '-c',
                                 ROOT / 'library/caspp/src/detail/symbol_table.c', '-o', symbol])
        binary = work / (name + extension)
        execute('build/' + name, [args.cc, '-std=c11', *flags, '-pthread', '-Ilibrary/caspp/include',
                                  ROOT / 'tests/chali_cas_symbols_c.c', symbol, thread, *objects, *linker, '-o', binary])
        execute('run/' + name, [binary], run=True)
    for name, source, units in [('sequence', 'chali_c_sequence.cpp', []),
                                 ('collections', 'chali_collection_storage.cpp', [ROOT / 'source/abi/std_collection_storage.cpp'])]:
        binary = work / (name + extension)
        execute('build/' + name, [args.cxx, '-std=c++17', *flags, '-pthread', '-Isource', '-Isource/ircode', '-Ilibrary',
                                  ROOT / 'tests' / source, *units, ROOT / 'source/allocator/system_arena.cpp',
                                  *objects, *linker, '-o', binary])
        execute('run/' + name, [binary], run=True)
    print('C runtime storage and network checks passed; ' + str(work))


if __name__ == '__main__':
    main()
