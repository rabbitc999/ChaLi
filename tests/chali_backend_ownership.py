"""核对后端所有权优化的结果、独立副本、回退及可选 AOT 计数。"""
from pathlib import Path
import argparse
from collections import Counter
import json
import os
import subprocess
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
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='backend-ownership-') as directory:
        work = Path(directory)
        libraries = [ROOT / p for p in ['build/chali-frontend/libparser.a', 'build/chali-runtime/libast.a',
                     'build/chali-stackvm-core/libstackvm.a', 'build/chali-host-records/librecords.a', 'build/chali-runtime/libpreprocess.a']]
        unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
        command = ['g++', '-std=c++17', '-O2', '-pthread', '-ffunction-sections', '-fdata-sections',
                   '-DCHALI_CUSTOM_ALLOCATOR=1', '-DCHALI_WITH_REFLECTION=1', '-DSTACKVM_WITH_REFLECTION=1',
                   '-I'+str(ROOT), '-I'+str(ROOT/'source'), '-I'+str(ROOT/'library'), '-I'+str(ROOT/'source/preprocess'),
                   '-I'+str(ROOT/'source/ircode'), '-I'+str(ROOT/'source/ast'),
                   str(ROOT/'tests/backend_ownership_test.cpp'), '-Wl,--gc-sections',
                   '-Wl,--start-group', *map(str,libraries), '-Wl,--end-group',
                   *(['-lwinhttp'] if os.name == 'nt' else []), '-o', str(unit)]
        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert result.returncode == 0, result.stdout + result.stderr
        result = subprocess.run([str(unit)], cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=120)
        assert result.returncode == 0 and result.stdout == 'backend ownership generation OK\n' and not result.stderr, result
        print('生成证据和检查屏障通过', flush=True)
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
