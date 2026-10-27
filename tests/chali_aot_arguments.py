"""对照 VM 与原生产物的 UTF-8 命令行参数，覆盖空串、引号、反斜线及中文路径。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def invoke(command, work, environment, report):
    """执行有界子进程并及时保存原始输出，失败时留下完整复现路径。"""
    result = subprocess.run(list(map(str, command)), cwd=work, env=environment,
                            capture_output=True, timeout=180)
    report['commands'].append(dict(command=list(map(str, command)), code=result.returncode,
                                   stdout_hex=result.stdout.hex(), stderr_hex=result.stderr.hex()))
    (work / 'report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    assert result.returncode == 0 and not result.stderr, (command, result.returncode, result.stdout, result.stderr, work)
    return result.stdout.replace(b'\r\n', b'\n')


def main():
    """分别检查两端的独立预期，不接受两端共同丢失字符的结果。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    frozen = json.loads((ROOT / 'tests/fixtures/compiler_limits/arguments_followup_0_31_0.json').read_text(encoding='utf-8'))
    work = Path(tempfile.mkdtemp(prefix='aot-arguments-'))
    source = work / '参数.chl'
    source.write_text(frozen['source'], encoding='utf-8')
    output = work / '原生 参数'
    environment = dict(os.environ, CHALI_LLVM_CLANG=(str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang')))
    report = dict(compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), commands=[])
    assert invoke([compiler, 'aot', source, '--out', output], work, environment, report) == b''
    cases = [[], ['', 'x y', 'a"b', 'C:\\trailing\\'], frozen['arguments']]
    for values in cases:
        expected = (str(len(values)) + '\n' + ''.join('[' + value + ']\n' for value in values)).encode('utf-8')
        vm = invoke([compiler, 'run', source, '--out', work / 'vm', '--', *values], work, environment, report)
        native = invoke([str(output) + ('.exe' if os.name == 'nt' else ''), *values], work, environment, report)
        assert vm == expected and native == expected, (values, vm, native, expected, work)
    print('VM/AOT UTF-8 参数、空串、引号、反斜线及中文产物路径通过')
    print('完整证据：' + str(work))


if __name__ == '__main__':
    main()
