"""校验新建接口绑定移交所有权，既有接口参数仍保留独立快照。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """两种分配器及关闭开关分别核对 checked、unchecked 与 LLVM 调用结构。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='interface-binding-move-'))
    clang = os.environ.get('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe')
                           if os.name == 'nt' else 'clang')
    records = []
    for allocator in ('malloc', 'pool'):
        for fallback in (False, True):
            env = dict(os.environ, CHALI_LANG='zh')
            env.pop('CHALI_NO_INTERFACE_BINDING_MOVE', None)
            env.pop('CHALI_NO_AOT_OBJECT_COPY_SCALAR', None)
            if fallback:
                env['CHALI_NO_INTERFACE_BINDING_MOVE'] = '1'
            label = allocator + ('-copy' if fallback else '-move')
            prefix = work / label
            fixture = ROOT / 'tests/fixtures/aot_interface_copy_scalar.chl'

            def execute(command, suffix):
                """保留命令和原始输出，拒绝任何失败或意外标准错误。"""
                args = list(map(str, command))
                result = subprocess.run(args, cwd=ROOT, env=env, capture_output=True,
                                        text=True, encoding='utf-8', timeout=600)
                record = dict(label=label + '-' + suffix, command=args, code=result.returncode,
                              stdout=result.stdout, stderr=result.stderr)
                records.append(record)
                assert result.returncode == 0 and not result.stderr, (record, work)
                return result

            checked = execute([compiler, 'run', fixture, '--allocator', allocator,
                               '--out', str(prefix) + '-run'], 'checked')
            assert checked.stdout == 'interface snapshots and scalar object copy OK\n', (checked, work)
            execute([compiler, 'aot', fixture, '--allocator', allocator, '--out', prefix], 'build')
            native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'unchecked')
            assert native.stdout == checked.stdout, (native, work)
            optimized = work / (label + '-o3.ll')
            execute([clang, '-O3', '-S', '-emit-llvm', prefix.with_suffix('.ll'), '-o', optimized], 'optimize')
            llvm = optimized.read_text(encoding='utf-8')
            for function in ('choose', 'callOperation'):
                bodies = re.findall(r'^define[^\n]*program__' + function + r'\(.*?^\}', llvm, re.M | re.S)
                assert len(bodies) == 1, (function, label, work)
                body = bodies[0]
                copies = len(re.findall(r'call i64 @chali_host_object_scalar\(', body))
                assert (copies > 0) == (fallback or function == 'callOperation'), (function, label, copies, work)
                if function == 'callOperation':
                    assert re.search(r'call i64 @chali_host_object_scalar\(i64(?: noundef)? 2,', body), (label, work)
                (work / (label + '-' + function + '.ll')).write_text(body + '\n', encoding='utf-8')
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('interface binding move: 4 modes checked/unchecked, fresh returns and independent snapshots OK;', work)


if __name__ == '__main__':
    main()
