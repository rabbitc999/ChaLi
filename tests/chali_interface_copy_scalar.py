"""核对对象复制标量 ABI 的继承接口、独立包装、深复制与原入口回退。"""
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
    """两种分配器和原语开关，分别运行 checked 与 unchecked 产物。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='interface-copy-scalar-'))
    clang = os.environ.get('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe')
                           if os.name == 'nt' else 'clang')
    records = []
    for allocator in ('malloc', 'pool'):
        for fallback in (False, True):
            env = dict(os.environ, CHALI_LANG='zh')
            env.pop('CHALI_NO_AOT_OBJECT_COPY_SCALAR', None)
            if fallback:
                env['CHALI_NO_AOT_OBJECT_COPY_SCALAR'] = '1'
            label = allocator + ('-fallback' if fallback else '-scalar')

            def execute(command, suffix):
                """记录命令、返回值和原始输出，失败保留工作目录。"""
                process = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                         capture_output=True, text=True, encoding='utf-8', timeout=600)
                item = dict(label=label + '-' + suffix, command=list(map(str, command)),
                            code=process.returncode, stdout=process.stdout, stderr=process.stderr)
                records.append(item)
                (work / (item['label'] + '.json')).write_text(json.dumps(item, ensure_ascii=False, indent=2), encoding='utf-8')
                assert process.returncode == 0, (item, work)
                return process

            fixture = ROOT / 'tests/fixtures/aot_interface_copy_scalar.chl'
            prefix = work / label
            checked = execute([compiler, 'run', fixture, '--allocator', allocator, '--out', str(prefix) + '-run'], 'run')
            assert checked.stdout == 'interface snapshots and scalar object copy OK\n' and not checked.stderr, (checked, work)
            execute([compiler, 'aot', fixture, '--allocator', allocator, '--out', prefix], 'build')
            native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'aot')
            assert native.stdout == checked.stdout and not native.stderr, (native, work)
            if allocator == 'malloc':
                optimized = work / (label + '-o3.ll')
                execute([clang, '-O3', '-S', '-emit-llvm', prefix.with_suffix('.ll'), '-o', optimized], 'optimize')
                llvm = optimized.read_text(encoding='utf-8')
                functions = re.findall(r'^define[^\n]*program__callOperation\(.*?^\}', llvm, re.M | re.S)
                assert len(functions) == 1, (label, work)
                body = functions[0]
                scalar = re.search(r'call i64 @chali_host_object_scalar\(i64(?: noundef)? 2,', body)
                assert bool(scalar) != fallback, (label, body, work)
                (work / (label + '-call-operation.ll')).write_text(body + '\n', encoding='utf-8')
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('interface object copy: 4 modes, checked/unchecked, snapshots, mutation, closures and O3 ABI OK;', work)


if __name__ == '__main__':
    main()
