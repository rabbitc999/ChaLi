"""核对立即消费 String 字节 Option 的标量路径、回退及相同错误。"""
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
    """在两种分配器及开关下比较 checked run 和 unchecked AOT。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='string-byte-unwrap-'))
    results = []
    for allocator in ('malloc', 'pool'):
        for disabled in (False, True):
            env = dict(os.environ, CHALI_LANG='en')
            env.pop('CHALI_NO_STRING_BYTE_UNWRAP', None)
            if disabled:
                env['CHALI_NO_STRING_BYTE_UNWRAP'] = '1'
            label = allocator + ('-fallback' if disabled else '-scalar')

            def execute(command, suffix, success=True):
                """保存命令、返回值及诊断，禁止吞掉错误。"""
                process = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                         capture_output=True, text=True, encoding='utf-8', timeout=600)
                item = dict(label=label + '-' + suffix, command=list(map(str, command)),
                            code=process.returncode, stdout=process.stdout, stderr=process.stderr)
                results.append(item)
                (work / (item['label'] + '.json')).write_text(json.dumps(item, ensure_ascii=False, indent=2), encoding='utf-8')
                assert (process.returncode == 0) == success, (item, work)
                return process

            fixture = ROOT / 'tests/fixtures/aot_string_byte_unwrap.chl'
            prefix = work / label
            run = execute([compiler, 'run', fixture, '--allocator', allocator, '--out', str(prefix) + '-run'], 'run')
            assert run.stdout == 'String byte unwrap OK\n' and not run.stderr, (run, work)
            execute([compiler, 'aot', fixture, '--allocator', allocator, '--out', prefix], 'build')
            native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'aot')
            assert native.stdout == run.stdout and not native.stderr, (native, work)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            has_scalar = '__bootStringByteAtUnwrap' in llvm
            assert has_scalar != disabled, (label, has_scalar, work)
            for name, index in (('negative', '-1'), ('end', '3'), ('empty', '0')):
                source = work / (label + '-' + name + '.chl')
                text = '' if name == 'empty' else 'abc'
                source.write_text('void main(){ String text="' + text + '"; int64 i=' + index +
                                  '; println("{}", text.byteAt(i).unwrap()); }\n', encoding='utf-8')
                failure = execute([compiler, 'run', source, '--allocator', allocator,
                                   '--out', work / (label + '-' + name)], name, success=False)
                assert 'called Option.unwrap() on None' in failure.stderr, (failure, work)
                assert failure.stdout == '', (failure, work)
                error_prefix = work / (label + '-' + name + '-aot')
                execute([compiler, 'aot', source, '--allocator', allocator, '--out', error_prefix], name + '-build')
                native_failure = execute([error_prefix.with_suffix('.exe' if os.name == 'nt' else '')],
                                         name + '-aot', success=False)
                assert 'called Option.unwrap() on None' in native_failure.stderr, (native_failure, work)
                assert native_failure.stdout == '', (native_failure, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=results),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('String byte unwrap: 4 modes, checked/unchecked, conservative fallbacks and 24 errors OK;', work)


if __name__ == '__main__':
    main()
