"""核对自举 IEEE754、裸指针视图与 AOT 位转换指令。"""
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
    """执行相同 VM/AOT 样本，保留输出、编译器摘要及优化 LLVM IR。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='bootstrap-numeric-'))
    env = dict(os.environ)
    clang = env.setdefault('CHALI_LLVM_CLANG', str(ROOT / 'library/llvm/toolchain/bin/clang.exe')
                           if os.name == 'nt' else 'clang')

    def execute(command, label):
        """保存每一步原始输出，并在执行失败时给出证据目录。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env, capture_output=True,
                                text=True, encoding='utf-8', timeout=600)
        (work / (label + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
        assert result.returncode == 0, (label, result.returncode, work)
        return result

    source = ROOT / 'tests/fixtures/bootstrap_numeric_default.chl'
    virtual = execute([compiler, 'run', source, '--out', work / 'vm'], 'vm')
    prefix = work / 'aot'
    execute([compiler, 'aot', source, '--out', prefix], 'build')
    native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'aot')
    assert virtual.stdout == native.stdout == 'bootstrap-numeric-default-ok\n', work
    assert virtual.stderr == native.stderr == '', work
    optimized = work / 'optimized.ll'
    execute([clang, '-O3', '-S', '-emit-llvm', prefix.with_suffix('.ll'), '-o', optimized], 'optimize')
    llvm = optimized.read_text(encoding='utf-8')
    assert re.search(r'bitcast double .* to i64', llvm), work
    assert re.search(r'bitcast float .* to i32', llvm), work
    assert re.search(r'bitcast i64 .* to double', llvm), work
    assert re.search(r'bitcast i32 .* to float', llvm), work
    for line in llvm.splitlines():
        assert not (re.search(r'\b(?:call|invoke)\b', line)
                    and re.search(r'__(?:pointerCast|bootFloat(?:32|64)(?:Bits|FromBits))', line)), (line, work)
    result = dict(compiler=str(compiler), compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(),
                  vm='passed', aot='passed', optimized_bitcasts='passed', conversion_calls='eliminated')
    (work / 'results.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('bootstrap numeric VM/AOT and LLVM checks passed; logs:', work)


if __name__ == '__main__':
    main()
