"""验证数据区字符串批量扫描及接口元数据读取，保留旧协议的差分证据。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """执行边界差分单测，并核对两分配器的 checked／unchecked 接口行为。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='data-cstring-'))
    records = []

    def execute(command, label):
        """保存原始命令、状态和输出，失败保留工作目录。"""
        command = list(map(str, command))
        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                                encoding='utf-8', timeout=600)
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
             ROOT / 'tests/chali_data_cstring.cpp', ROOT / 'source/allocator/system_arena.cpp',
             '-Wl,--gc-sections', '-Wl,--start-group', *libraries, '-Wl,--end-group',
             *(['-lwinhttp'] if os.name == 'nt' else []), '-o', unit], 'compile-unit')
    result = execute([unit], 'unit')
    assert 'boundaries, partial errors, UTF-8 and heap layouts OK' in result.stdout and not result.stderr, (result, work)
    fixture = ROOT / 'tests/fixtures/aot_interface_copy_scalar.chl'
    for allocator in ('malloc', 'pool'):
        prefix = work / allocator
        checked = execute([compiler, 'run', fixture, '--allocator', allocator,
                           '--out', str(prefix) + '-run'], allocator + '-run')
        assert checked.stdout == 'interface snapshots and scalar object copy OK\n' and not checked.stderr, (checked, work)
        execute([compiler, 'aot', fixture, '--allocator', allocator, '--out', prefix], allocator + '-build')
        native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], allocator + '-aot')
        assert native.stdout == checked.stdout and not native.stderr, (native, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('data CString: legacy differential, bounds, partial errors and 2 allocators checked/unchecked OK;', work)


if __name__ == '__main__':
    main()
