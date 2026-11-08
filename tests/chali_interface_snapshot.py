"""校验接口指针快照的分配减少、独立修改及旧路径回退。"""
from compiler_path import compiler_path, compiler_build_info
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
from itertools import product
import json
import os
import subprocess
import shutil

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行真实对象表计数单测及两分配器的共享／原路径 BUILD 对照。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--build-root', type=Path)
    parser.add_argument('--cxx')
    parser.add_argument('--sysroot', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    static = os.name == 'nt' and compiler_build_info(compiler)['llvm'] == 'static'
    build = (args.build_root or Path(os.environ.get('CHALI_BUILD_ROOT', str(ROOT / ('build/llvm-static-gcc16' if static else 'build'))))).resolve()
    cxx = Path(shutil.which(args.cxx or '') or args.cxx or (sdk / 'bin/g++.exe' if static else 'g++'))
    use_sdk = cxx.resolve().parent == (sdk / 'bin').resolve()
    tool_flags = ['--sysroot=' + str(args.sysroot or sdk), '-static'] if use_sdk else (['--sysroot=' + str(args.sysroot)] if args.sysroot else [])
    base_env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    if use_sdk:
        base_env['PATH'] = str(cxx.parent) + os.pathsep + base_env.get('PATH', '')
    work = Path(tempfile.mkdtemp(prefix='interface-snapshot-'))
    records = []

    def execute(command, label, env=None):
        """保存实际执行证据，并拒绝失败状态。"""
        command = list(map(str, command))
        result = subprocess.run(command, cwd=ROOT, env=base_env if env is None else env, capture_output=True,
                                text=True, encoding='utf-8', timeout=600)
        records.append(dict(label=label, command=command, code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'progress.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
        assert result.returncode == 0, (records[-1], work)
        return result

    unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
    libraries = [build / p for p in ('chali-ircode-aot-core/libstackvm.a',
                 'chali-host-records/librecords.a', 'chali-runtime/libast.a',
                 'chali-runtime/libpreprocess.a')]
    execute([cxx, *tool_flags, '-std=c++17', '-O2', '-pthread', '-ffunction-sections', '-fdata-sections',
             '-DRUNTIME_BUILD_ONLY=1', '-DCHALI_CUSTOM_ALLOCATOR=1', '-DCHALI_WITH_REFLECTION=1',
             '-DRUNTIME_WITH_REFLECTION=1', '-Isource', '-Ilibrary', '-I.',
             ROOT / 'tests/chali_interface_snapshot.cpp', ROOT / 'source/allocator/system_arena.cpp',
             '-Wl,--gc-sections', '-Wl,--start-group',
             *libraries, '-Wl,--end-group', *(['-lwinhttp'] if os.name == 'nt' else []), '-o', unit], 'compile-unit')
    result = execute([unit], 'unit')
    assert 'independent handles and detach OK' in result.stdout and not result.stderr, (result, work)
    for allocator, fallback, no_slots in product(('malloc', 'pool'), (False, True), (False, True)):
        env = dict(base_env)
        env.pop('CHALI_NO_INTERFACE_COPY_COW', None)
        env.pop('CHALI_NO_OBJECT_INLINE_SLOTS', None)
        if no_slots:
            env['CHALI_NO_OBJECT_INLINE_SLOTS'] = '1'
        if fallback:
            env['CHALI_NO_INTERFACE_COPY_COW'] = '1'
        label = allocator + ('-original' if fallback else '-shared') + ('-table' if no_slots else '-slots')
        fixture = ROOT / 'tests/fixtures/aot_interface_snapshot.chl'
        prefix = work / label
        checked = execute([compiler, 'run', fixture, '--allocator', allocator,
                           '--out', str(prefix) + '-run'], label + '-run', env)
        assert checked.stdout == 'interface pointer snapshots OK\n' and not checked.stderr, (checked, work)
        execute([compiler, 'build', fixture, '--allocator', allocator, '--out', prefix], label + '-build', env)
        native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], label + '-aot', env)
        assert native.stdout == checked.stdout and not native.stderr, (native, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('interface snapshots: allocation counts, 8 modes checked/unchecked and Any rebind OK;', work)


if __name__ == '__main__':
    main()
