"""验证综合标准错误样例的前端预算边界，禁止未捕获分配异常与异常退出。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]


def invoke(command, environment, directory):
    """保留实际命令、原始输出、退出状态与耗时。"""
    started = time.perf_counter()
    result = subprocess.run(list(map(str, command)), cwd=directory, env=environment,
                            capture_output=True, timeout=300)
    return dict(command=list(map(str, command)), elapsed=time.perf_counter() - started,
                code=result.returncode, stdout=result.stdout.decode('utf-8', errors='replace'),
                stderr=result.stderr.decode('utf-8', errors='replace'))


def main():
    """允许成功或明确的预算拒绝，两端都必须保持有界退出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    frozen = json.loads((ROOT / 'tests/fixtures/compiler_limits/budget_followup_0_31_0.json').read_text(encoding='utf-8'))
    work = Path(tempfile.mkdtemp(prefix='compiler-budget-'))
    source = work / 'all_errors.chl'
    source.write_text(frozen['source'], encoding='utf-8')
    report = dict(compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(),
                  source=frozen['source'], results={})
    for backend, language, marker in [('run', 'zh', '编译器内存预算不足'),
                                      ('aot', 'en', 'compiler memory budget exhausted')]:
        output = work / backend
        environment = dict(os.environ, CHALI_LANG=language)
        command = [compiler, backend, source, '--out', output]
        if backend == 'aot':
            command += ['--llvm', ROOT / 'build/dependencies/llvm/toolchain/bin' / ('clang.exe' if os.name == 'nt' else 'clang')]
        result = invoke(command, environment, work)
        report['results'][backend] = result
        (work / 'report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        assert result['code'] in (0, 1), (backend, result, work)
        assert 'terminate called' not in result['stderr'] and 'std::bad_alloc' not in result['stderr'], (backend, result, work)
        if result['code'] == 1:
            assert marker in result['stderr'], (backend, result, work)
            result['outcome'] = 'diagnosed_budget_limit'
        else:
            executed = result
            if backend == 'aot':
                executed = invoke([str(output) + ('.exe' if os.name == 'nt' else '')], environment, work)
                result['execution'] = executed
            assert executed['code'] == 0 and executed['stdout'].replace('\r\n', '\n') == 'all Error modules OK\n', (backend, executed, work)
            assert not executed['stderr'].strip(), (backend, executed, work)
            result['outcome'] = 'pass'
        (work / 'report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        print('综合标准错误编译预算验证：' + backend + ' ' + result['outcome'], flush=True)
    print('完整证据：' + str(work), flush=True)


if __name__ == '__main__':
    main()
