"""校验接口指针快照的分配减少、独立修改及旧路径回退。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
from itertools import product
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行真实对象表计数单测及两分配器的共享／原路径 AOT 对照。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='interface-snapshot-'))
    records = []

    def execute(command, label, env=None):
        """保存实际执行证据，并拒绝失败状态。"""
        command = list(map(str, command))
        result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                                text=True, encoding='utf-8', timeout=600)
        records.append(dict(label=label, command=command, code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'progress.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
        assert result.returncode == 0, (records[-1], work)
        return result

    unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
    libraries = [ROOT / p for p in ('build/chali-ircode-aot-core/libstackvm.a',
                 'build/chali-host-records/librecords.a', 'build/chali-runtime/libast.a',
                 'build/chali-runtime/libpreprocess.a')]
    execute(['g++', '-std=c++17', '-O2', '-pthread', '-ffunction-sections', '-fdata-sections',
             '-DSTACKVM_AOT_ONLY=1', '-DCHALI_CUSTOM_ALLOCATOR=1', '-DCHALI_WITH_REFLECTION=1',
             '-DSTACKVM_WITH_REFLECTION=1', '-Isource', '-Ilibrary', '-I.',
             ROOT / 'tests/chali_interface_snapshot.cpp', ROOT / 'source/allocator/system_arena.cpp',
             '-Wl,--gc-sections', '-Wl,--start-group',
             *libraries, '-Wl,--end-group', *(['-lwinhttp'] if os.name == 'nt' else []), '-o', unit], 'compile-unit')
    result = execute([unit], 'unit')
    assert 'independent handles and detach OK' in result.stdout and not result.stderr, (result, work)
    for allocator, fallback, no_slots in product(('malloc', 'pool'), (False, True), (False, True)):
        env = dict(os.environ, CHALI_LANG='zh')
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
        execute([compiler, 'aot', fixture, '--allocator', allocator, '--out', prefix], label + '-build', env)
        native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], label + '-aot', env)
        assert native.stdout == checked.stdout and not native.stderr, (native, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('interface snapshots: allocation counts, 8 modes checked/unchecked and Any rebind OK;', work)


if __name__ == '__main__':
    main()
