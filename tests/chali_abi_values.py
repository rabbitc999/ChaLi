"""Compile a real C ABI library and verify exported CHALI value/pointer conversions."""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import json
import argparse
import os
import re
import subprocess
ROOT = Path(__file__).resolve().parents[1]
WORK = ROOT / 'build' / 'chali-abi-values'
EXE = '.exe' if os.name == 'nt' else ''

def run(args, succeeds=True, env=None):
    result = subprocess.run(list(map(str, args)), cwd=ROOT, text=True, encoding='utf-8', capture_output=True, env=env, timeout=180)
    assert (result.returncode == 0) == succeeds, result.stdout + result.stderr
    return result


def execute(compiler, source, output, succeeds=True, env=None, aot=False):
    """按同一预期验证源码执行或原生产物执行，编译失败也须保留原始诊断。"""
    if not aot:
        return run([compiler, 'run', source, '--out', output], succeeds=succeeds, env=env)
    environment = {**(os.environ if env is None else env)}
    environment['PATH'] = str(WORK) + os.pathsep + environment.get('PATH', '')
    environment['CHALI_LLVM_CLANG'] = (str(ROOT / 'library/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang'))
    command = [compiler, 'aot', source, '--out', output]
    result = subprocess.run(list(map(str, command)), cwd=ROOT, text=True, encoding='utf-8', capture_output=True, env=environment, timeout=180)
    if result.returncode != 0:
        assert not succeeds, result.stdout + result.stderr
        return result
    return run([str(output) + EXE], succeeds=succeeds, env=environment)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--aot', action='store_true', help='对相同结构体、引用与缓冲区案例验证 LLVM 原生产物')
    options = parser.parse_args()
    compiler = options.chali.resolve()
    WORK.mkdir(parents=True, exist_ok=True)
    library = WORK / ('native.dll' if os.name == 'nt' else 'native.so')
    native = WORK / 'native.c'
    fixture = ROOT / 'doc' / 'examples' / 'abi_buffers'
    native.write_text((fixture / 'native.c').read_text(encoding='utf-8'), encoding='utf-8')
    run(['gcc', '-shared', *(['-fPIC'] if os.name != 'nt' else []), native, '-o', library])
    mc = WORK / 'values.chl'
    mc.write_text((fixture / 'demo.chl').read_text(encoding='utf-8').replace('"./native.dll"', json.dumps(library.as_posix())), encoding='utf-8')
    from project_temp import tempfile
    temp_root = Path(tempfile.gettempdir())
    before = set(temp_root.glob('chali-abi-*'))
    immediate = execute(compiler, mc, WORK / 'immediate', aot=options.aot)
    assert immediate.stdout == 'abi-values-ok\n' and immediate.stderr == '', immediate.stdout + immediate.stderr
    assert set(temp_root.glob('chali-abi-*')) == before, 'ABI bridge temporary directory leaked'
    missing_compiler = {**os.environ, 'CHALI_ABI_CXX': str(WORK / 'missing-compiler'), 'CHALI_ABI_NO_CACHE': '1'}
    failed = execute(compiler, mc, WORK / 'missing', succeeds=False, env=missing_compiler, aot=options.aot)
    assert 'CHALI_ABI_CXX' in failed.stderr, failed.stderr
    assert set(temp_root.glob('chali-abi-*')) == before, 'failed bridge temporary directory leaked'
    scalar = WORK / 'scalar.chl'
    scalar.write_text('abi ' + json.dumps(library.as_posix()) + ' { int32 identity(int32 value) = "identity"; }\nvoid main() { std::lang::println("{}", identity((int32)7)); return; }\n', encoding='utf-8')
    small = execute(compiler, scalar, WORK / 'scalar', env=missing_compiler, aot=options.aot)
    assert small.stdout == '7\n' and small.stderr == '', small.stdout + small.stderr
    for length, message in ((3, 'returned null'), (-1, 'buffer length'), (100000000, 'buffer length')):
        bad = WORK / 'bad-length.chl'
        bad.write_text('abi ' + json.dumps(library.as_posix()) + f' {{ int32[] numbers(int32 count) = "numbers" [count]; }}\nvoid main() {{ int32 values[] = numbers((int32){length}); return; }}\n', encoding='utf-8')
        failed = execute(compiler, bad, WORK / 'bad-length', succeeds=False, aot=options.aot)
        assert message in failed.stderr, failed.stderr
        assert set(temp_root.glob('chali-abi-*')) == before, 'error bridge temporary directory leaked'
    bad = WORK / 'null-reference.chl'
    bad.write_text('struct Packet { int32 value; }; abi ' + json.dumps(library.as_posix()) + ' { Packet mut& nothing() = "nothing"; }\nvoid main() { Packet mut& value = nothing(); return; }\n', encoding='utf-8')
    failed = execute(compiler, bad, WORK / 'null-reference', succeeds=False, aot=options.aot)
    assert 'ABI returned null for nonnull reference' in failed.stderr, failed.stderr
    for declaration in ('struct Bad { String text; }; abi LIBRARY { void f(Bad p); }', 'struct Bad unknown { int32 value; }; abi LIBRARY { void f(Bad p); }', 'abi LIBRARY { int32[] f(int32 count) = "f" [missing]; }', 'abi LIBRARY { void f(int32 mut& mut& p); }'):
        bad = WORK / 'bad.chl'
        bad.write_text(declaration.replace('LIBRARY', json.dumps(library.as_posix())) + '\nvoid main() { return; }\n', encoding='utf-8')
        result = run([compiler, 'run', bad, '--emit', '--out', WORK / 'bad'], succeeds=False)
        assert 'abi' in result.stderr.lower() or '语法错误' in result.stderr, result.stderr
    print(('AOT' if options.aot else 'VM') + ' ABI 值、引用和非空返回检查通过')
if __name__ == '__main__':
    main()
