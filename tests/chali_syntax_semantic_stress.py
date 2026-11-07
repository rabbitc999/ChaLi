"""复杂语法、格式化往返、语义组合和已知限制的双模式压力回归。"""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import argparse
import os
import signal
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
PROGRAMS = (
    'syntax_loop_label_matrix',
    'syntax_for_else_stress',
    'syntax_for_else',
    'loop_statement_ownership',
    'syntax_for_loops',
    'syntax_for_minimal',
    'syntax_borrowed_array_named',
    'syntax_pressure_depth',
    'syntax_macro_type_tokens',
    'syntax_nested_function_argument',
    'syntax_semantic_composition',
)


def execute(command, env, timeout=240):
    """隔离捕获输出；超时结束进程树，将崩溃与正常诊断区分。"""
    process = subprocess.Popen(
        list(map(str, command)), cwd=ROOT, env=env,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        start_new_session=os.name != 'nt',
    )
    try:
        stdout, stderr = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        if os.name == 'nt':
            subprocess.run(['taskkill', '/PID', str(process.pid), '/T', '/F'], capture_output=True)
        else:
            os.killpg(process.pid, signal.SIGKILL)
        process.communicate()
        raise AssertionError(('syntax stress timeout', command))
    assert process.returncode in (0, 1, 2), ('compiler/runtime crash', command, process.returncode, stderr)
    return process.returncode, stdout.decode('utf-8').replace('\r\n', '\n'), stderr.decode('utf-8')


def main():
    """核对原文及格式化后的双模式输出，并验证诊断边界无崩溃或静默失败。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1', CHALI_NO_STDIN_INPUT='1')
    includes = ['--include-path', str(ROOT / 'example/samples/modules')]
    with tempfile.TemporaryDirectory(prefix='syntax-semantic-') as directory:
        work = Path(directory)

        def verify(name):
            """同一完整程序从原文和格式化文本构建，分别核对独立预期。"""
            folder = work / name
            folder.mkdir()
            source = ROOT / 'example/samples' / (name + '.chl')
            formatted = folder / (name + '.chl')
            formatted.write_text(source.read_text(encoding='utf-8'), encoding='utf-8')
            code, stdout, stderr = execute([compiler, 'fmt', formatted, '--write'], env)
            assert code == 0 and not stderr, (name, stdout, stderr)
            first = formatted.read_bytes()
            code, stdout, stderr = execute([compiler, 'fmt', formatted, '--write'], env)
            assert code == 0 and not stderr and formatted.read_bytes() == first, (name, 'formatter not idempotent', stderr)
            expected = (ROOT / 'example/expected' / (name + '.txt')).read_text(encoding='utf-8')
            for original, path in ((True, source), (False, formatted)):
                code, stdout, stderr = execute([compiler, 'check', path, *includes], env)
                assert code == 0 and not stderr, (name, original, stdout, stderr)
                for checked in (True, False):
                    prefix = folder / ('original' if original else 'formatted')
                    prefix = prefix.with_name(prefix.name + ('-checked' if checked else '-unchecked'))
                    code, stdout, stderr = execute([
                        compiler, 'build', path, '--checked' if checked else '--unchecked',
                        '--out', prefix, *includes,
                    ], env)
                    assert code == 0 and not stderr, (name, original, checked, stdout, stderr)
                    executable = prefix.with_suffix('.exe') if os.name == 'nt' else prefix
                    code, stdout, stderr = execute([executable], env, timeout=90)
                    assert (code, stdout, stderr) == (0, expected, ''), (name, original, checked, code, stdout, stderr)
            return name

        with ThreadPoolExecutor(max_workers=2) as pool:
            for name in pool.map(verify, PROGRAMS):
                print(name + ': original/formatted checked/unchecked OK', flush=True)
        negatives = list((ROOT / 'example/samples-error').glob('syntax_limit_*.chl'))
        negatives += list((ROOT / 'example/samples-error').glob('borrow_loop_*.chl'))
        negatives += list((ROOT / 'example/samples-error').glob('syntax_semantic_*.chl'))
        negatives += list((ROOT / 'example/samples-error').glob('syntax_for_*_bad.chl'))
        assert len(negatives) >= 8
        for source in sorted(negatives):
            code, stdout, stderr = execute([compiler, 'check', source], env, timeout=120)
            expected = (ROOT / 'example/expected-error' / source.with_suffix('.txt').name).read_text(encoding='utf-8').strip()
            assert code in (1, 2) and stderr and (expected in stderr or expected.split('：', 1)[-1] in stderr), (source.name, code, stdout, stderr)
    print(f'复杂语法与语义压力：{len(PROGRAMS)}个完整程序、{len(PROGRAMS)*4}次双模式原生执行、格式化幂等及已知限制／生命周期拒绝边界通过')


if __name__ == '__main__':
    main()
