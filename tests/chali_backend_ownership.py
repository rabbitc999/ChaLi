"""核对后端所有权优化的结果、独立副本、回退及可选 AOT 计数。"""
from pathlib import Path
import argparse
from collections import Counter
import json
import os
import subprocess
import shutil
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
DISABLE = dict(CHALI_NO_AOT_ADDRESS_CACHE='1', CHALI_NO_AOT_BULK_COPY='1',
               CHALI_NO_AOT_INLINE_HINT='1', CHALI_NO_AOT_DIRECT_CALLS='1', CHALI_NO_ARRAY_COPY_BULK='1')
ENABLED = dict()


def main():
    """以同编译器关闭／开启新优化，核对实际输出及运行时操作计数。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    static = os.name == 'nt' and (sdk / 'bin/g++.exe').is_file()
    parser.add_argument('--build-root', type=Path, default=ROOT / ('build/llvm-static-gcc16' if static else 'build'))
    parser.add_argument('--cxx')
    parser.add_argument('--sysroot', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    build = args.build_root.resolve()
    cxx = Path(shutil.which(args.cxx or '') or args.cxx or (sdk / 'bin/g++.exe' if static else 'g++'))
    use_sdk = cxx.resolve().parent == (sdk / 'bin').resolve()
    environment = dict(os.environ, PATH=str(cxx.resolve().parent) + os.pathsep + os.environ.get('PATH', ''))
    with tempfile.TemporaryDirectory(prefix='backend-ownership-') as directory:
        work = Path(directory)
        libraries = [build / p for p in ['chali-frontend/libparser.a', 'chali-runtime/libast.a',
                     'chali-ircode-aot-core/libstackvm.a', 'chali-host-records/librecords.a', 'chali-runtime/libpreprocess.a']]
        assert all(p.is_file() for p in libraries), '先构建匹配工具链的后端归档'
        unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
        command = [str(cxx), '-std=c++17', '-O2', '-pthread', '-ffunction-sections', '-fdata-sections',
                   '-DCHALI_CUSTOM_ALLOCATOR=1', '-DSTACKVM_AOT_ONLY=1', '-DCHALI_WITH_REFLECTION=1', '-DSTACKVM_WITH_REFLECTION=1',
                   '-I'+str(ROOT), '-I'+str(ROOT/'source'), '-I'+str(ROOT/'library'), '-I'+str(ROOT/'source/preprocess'),
                   '-I'+str(ROOT/'source/ircode'), '-I'+str(ROOT/'source/ast'),
                   str(ROOT/'tests/backend_ownership_test.cpp'), str(ROOT/'source/allocator/system_arena.cpp'), '-Wl,--gc-sections',
                   '-Wl,--start-group', *map(str,libraries), '-Wl,--end-group',
                   *(['-lwinhttp'] if os.name == 'nt' else []), '-o', str(unit)]
        if args.sysroot or use_sdk:
            command += ['--sysroot=' + str((args.sysroot or sdk).resolve())]
        info = json.loads(subprocess.check_output([str(compiler), '--build-info'], cwd=ROOT, text=True, encoding='utf-8'))
        if info['llvm'] == 'static':
            config = sdk / 'bin/llvm-config.exe' if use_sdk else Path(shutil.which('llvm-config') or 'llvm-config')
            directory = subprocess.check_output([str(config), '--libdir'], text=True).strip()
            flags = subprocess.check_output([str(config), '--link-static', '--libs', 'all', '--system-libs'], text=True).split()
            command += ['-L' + directory, '-Wl,--start-group', *flags, '-Wl,--end-group']
        if os.name == 'nt': command += ['-static', '-lws2_32', '-lbcrypt', '-lcrypt32', *(['-fuse-ld=lld'] if use_sdk else [])]
        else: command += ['-ldl']
        result = subprocess.run(command, cwd=ROOT, env=environment, capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert result.returncode == 0, result.stdout + result.stderr
        result = subprocess.run([str(unit)], cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=120)
        assert result.returncode == 0 and result.stdout == 'backend ownership generation OK\n' and not result.stderr, result
        print('生成证据和检查屏障通过', flush=True)
        ranges = work / ('storage-ranges.exe' if os.name == 'nt' else 'storage-ranges')
        ranges_command = [str(ROOT/'tests/move_storage_ranges.cpp') if item == str(ROOT/'tests/backend_ownership_test.cpp')
                          else str(ranges) if item == str(unit) else item for item in command]
        result = subprocess.run(ranges_command, cwd=ROOT, env=environment, capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert result.returncode == 0, result.stdout + result.stderr
        result = subprocess.run([str(ranges)], cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=120)
        assert result.returncode == 0, result.stdout + result.stderr
        print('随机存储范围、调试状态独立性及代次耗尽通过', flush=True)
        for mode, expected in [('overlap','1'), ('self','3'), ('empty','1'), ('stale',None), ('partial',None)]:
            outcomes = []
            for name, flags in [('baseline', DISABLE), ('optimized', ENABLED)]:
                env = dict(os.environ, CHALI_LANG='zh', **flags)
                target = work/(mode+'-'+name)
                result = subprocess.run([str(unit), '--copy-fixture', str(target), mode], cwd=ROOT, env=env,
                                        capture_output=True, text=True, encoding='utf-8', timeout=240)
                assert result.returncode == 0, (mode, name, result.stdout, result.stderr)
                result = subprocess.run([str(target)+('.exe' if os.name=='nt' else '')], cwd=ROOT, env=env,
                                        capture_output=True, text=True, encoding='utf-8', timeout=60)
                if expected is None:
                    assert result.returncode != 0 and '访问未分配或已释放的内存' in result.stderr, (mode, name, result)
                else:
                    assert result.returncode == 0 and result.stdout == expected and not result.stderr, (mode, name, result)
                outcomes.append(result.returncode)
            assert outcomes[0] == outcomes[1], (mode, outcomes)
            print(mode+' 复制边界通过', flush=True)
        counts = {}
        for name, flags in [('baseline', DISABLE), ('optimized', ENABLED)]:
            env = dict(os.environ, CHALI_LANG='zh', **flags)
            for backend in ('run', 'aot'):
                target = work / (name + '-' + backend)
                result = subprocess.run([str(compiler), backend, str(ROOT/'example/samples/backend_ownership.chl'), '--out', str(target)],
                                        cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=300)
                assert result.returncode == 0, (name, backend, result.stdout, result.stderr)
                if backend == 'aot':
                    result = subprocess.run([str(target)+('.exe' if os.name=='nt' else '')], cwd=ROOT, env=env,
                                            capture_output=True, text=True, encoding='utf-8', timeout=120)
                assert result.returncode == 0 and result.stdout == 'backend ownership OK\n' and not result.stderr, (
                    name, backend, result.stdout, result.stderr)
                print(name+' '+backend+' 语义通过', flush=True)
            env['CHALI_BACKEND_PROFILE'] = '1'
            target = work / (name+'-profile')
            result = subprocess.run([str(compiler), 'aot', str(ROOT/'example/samples/backend_ownership.chl'), '--out', str(target)],
                                    cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=300)
            assert result.returncode == 0, result.stdout + result.stderr
            env['CHALI_BACKEND_PROFILE_OUTPUT'] = str(work/(name+'.json'))
            result = subprocess.run([str(target)+('.exe' if os.name=='nt' else '')], cwd=ROOT, env=env,
                                    capture_output=True, text=True, encoding='utf-8', timeout=120)
            assert result.returncode == 0 and result.stdout == 'backend ownership OK\n' and not result.stderr, result
            profile = json.loads(Path(env['CHALI_BACKEND_PROFILE_OUTPUT']).read_text(encoding='utf-8'))
            counts[name] = Counter()
            for event in profile['events']:
                counts[name][event['site'].split('\t')[0]] += event['count']
            assert counts[name]['allocation_request'] > 0 and counts[name]['cleanup'] > 0
        print('插桩不改变输出：'+str({name: dict(move=value['aggregate_move'], slot_copy=value['call:__chali_copy_struct']) for name,value in counts.items()}), flush=True)


if __name__ == '__main__':
    main()
