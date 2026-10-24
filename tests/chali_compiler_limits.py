"""分离缺陷发现与修复：记录三类 75 项 VM/AOT 代表测试，不在运行中修改实现。"""
from compiler_path import compiler_path
from compiler_limits_cases import catalog, ROOT
from project_temp import tempfile
import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import signal
import subprocess
import time


def execute(command, cwd, timeout, input_text=''):
    """捕获原始字节和退出码；超时终止本次测试进程树，防止遗留压力进程。"""
    environment = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    started = time.perf_counter()
    process = subprocess.Popen(list(map(str, command)), cwd=cwd, env=environment,
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                               start_new_session=os.name != 'nt')
    timed_out = False
    try:
        stdout, stderr = process.communicate(input_text.encode('utf-8'), timeout=timeout)
    except subprocess.TimeoutExpired:
        timed_out = True
        if os.name == 'nt':
            subprocess.run(['taskkill', '/PID', str(process.pid), '/T', '/F'], capture_output=True)
        else:
            os.killpg(process.pid, signal.SIGKILL)
        stdout, stderr = process.communicate()
    return dict(command=list(map(str, command)), cwd=str(cwd), elapsed=time.perf_counter() - started,
                code=process.returncode, timeout=timed_out, stdout=stdout.hex(), stderr=stderr.hex(),
                stdout_text=stdout.decode('utf-8', errors='replace'), stderr_text=stderr.decode('utf-8', errors='replace'))


def classification(result, expected, outcome, compiling=False):
    """区分成功、已诊断拒绝、错算、进程崩溃和超时，不把两个后端一致当作正确。"""
    if result['timeout']:
        return 'timeout'
    code = result['code']
    stderr = bytes.fromhex(result['stderr']).decode('utf-8', errors='replace')
    if code < 0 or code >= 0x80000000 or any(word in stderr for word in ('terminate called', 'Segmentation fault', 'Assertion failed')):
        return 'crash'
    if code:
        if not stderr.strip():
            return 'silent_failure'
        if outcome == 'reject' and compiling:
            return 'rejected'
        if outcome == 'bounded' and re.search(r'内存不足|内存耗尽|内存分配失败|arena|out of memory|bad_alloc|预算|递归深度|嵌套深度|栈溢出|stack overflow|超过.*限制', stderr, re.I):
            return 'bounded_error'
        return 'compile_error' if compiling else 'runtime_error'
    if compiling:
        return 'unexpected_acceptance' if outcome == 'reject' else 'compiled'
    if outcome == 'reject':
        return 'unexpected_acceptance'
    actual = bytes.fromhex(result['stdout']).replace(b'\r\n', b'\n')
    return 'pass' if actual == expected.encode('utf-8').replace(b'\r\n', b'\n') else 'wrong_output'


def run_case(case, compiler, root, timeout):
    """在独立目录中检查一个源程序的两个后端，保留所有输出和生成源码。"""
    folder = root / case['group'] / case['name']
    folder.mkdir(parents=True)
    source = folder / (case['name'] + '.chl')
    source.write_text(case['source'], encoding='utf-8')
    (folder / 'expected.txt').write_text(case['expected'], encoding='utf-8')
    # 既有样例保留原始源位置，避免改写相对导入；运行产生的文件仍隔离在 tmp。
    original = ROOT / case['origin'] if 'origin' in case else source
    expected_path = ROOT / 'example/expected' / case['name']
    input_text = expected_path.with_suffix('.in').read_text(encoding='utf-8') if expected_path.with_suffix('.in').exists() else ''
    arguments = expected_path.with_suffix('.args').read_text(encoding='utf-8').splitlines() if expected_path.with_suffix('.args').exists() else []
    flags = ['--memory-size', '67108864'] if case['group'] == 'pressure' else []
    results = {}
    for backend in ['vm', 'aot']:
        cwd = folder / backend
        cwd.mkdir()
        (cwd / 'build').mkdir()
        (cwd / 'tmp').mkdir()
        prefix = cwd / 'program'
        # 先做只编译检查，再从同一源码执行 VM，分清编译拒绝与运行期失败。
        command = [compiler, 'aot' if backend == 'aot' else 'run', original, '--out', prefix, *flags]
        if backend == 'vm':
            command += ['--emit', '--svmb']
        built = execute(command, cwd, timeout)
        status = classification(built, case['expected'], case['outcome'], compiling=True)
        result = dict(build=built, status=status)
        if status == 'compiled':
            product = prefix.with_suffix('.exe' if os.name == 'nt' else '')
            run_command = [product, *arguments] if backend == 'aot' else [compiler, 'run', original, '--out', str(prefix) + '-run', *flags, '--', *arguments]
            executed = execute(run_command, cwd, min(timeout, 30) if backend == 'aot' else timeout, input_text)
            result.update(run=executed, status=classification(executed, case['expected'], case['outcome']))
        results[backend] = result
    return dict(group=case['group'], name=case['name'], outcome=case['outcome'],
                source_sha256=hashlib.sha256(case['source'].encode()).hexdigest(), results=results)


def main():
    """执行并冻结发现报告；报告包含基线身份，失败不会中止其余用例。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--jobs', type=int, default=2)
    parser.add_argument('--timeout', type=int, default=90)
    parser.add_argument('--llvm', type=Path, default=ROOT / 'library/llvm/toolchain/bin' / ('clang.exe' if os.name == 'nt' else 'clang'))
    parser.add_argument('--case', action='append', help='仅执行指定名称，可重复；完整发现必须不加此参数')
    parser.add_argument('--group', choices=['business', 'advanced', 'pressure'], help='只复验指定类别')
    parser.add_argument('--verify', action='store_true', help='发现后回归模式，未预期的失败使测试返回非零')
    options = parser.parse_args()
    compiler = options.chali.resolve()
    llvm = options.llvm.resolve()
    os.environ['CHALI_LLVM_CLANG'] = str(llvm)
    cases = catalog()
    if options.group:
        cases = [case for case in cases if case['group'] == options.group]
    if options.case:
        cases = [case for case in cases if case['name'] in options.case]
        if len(cases) != len(set(options.case)):
            parser.error('存在未知用例名称')
    root = Path(tempfile.mkdtemp(prefix='compiler-limits-'))
    llvm_version = execute([llvm, '--version'], root, 15)
    if llvm_version['code']:
        raise RuntimeError('LLVM 工具链预检失败：' + llvm_version['stderr_text'])
    messages = [dict(jsonrpc='2.0', id=1, method='initialize', params={}),
                dict(jsonrpc='2.0', id=2, method='shutdown', params=None),
                dict(jsonrpc='2.0', method='exit', params=None)]
    wire = ''
    for message in messages:
        body = json.dumps(message)
        wire += 'Content-Length: ' + str(len(body.encode('utf-8'))) + '\r\n\r\n' + body
    version = execute([compiler, 'lsp'], root, 15, wire)
    report = dict(phase='verification' if options.verify else 'discovery', target_version=(ROOT / 'VERSION').read_text(encoding='utf-8').strip(), compiler=str(compiler),
                  compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(),
                  source_commit=subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT).decode().strip(),
                  version=version, llvm=llvm_version, host=platform.platform(), jobs=options.jobs,
                  harness_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  catalog_sha256=hashlib.sha256(json.dumps(cases, sort_keys=True, ensure_ascii=False).encode()).hexdigest(),
                  optimization='默认四元式优化 4 轮与 VM 最高优化；反射编入', cases=[])
    print('发现报告目录：' + str(root), flush=True)
    with ThreadPoolExecutor(max_workers=options.jobs) as pool:
        pending = {pool.submit(run_case, case, compiler, root, options.timeout): case for case in cases}
        for future in as_completed(pending):
            case = pending[future]
            try:
                result = future.result()
            except Exception as error:
                result = dict(group=case['group'], name=case['name'], harness_error=repr(error))
            report['cases'].append(result)
            report['cases'].sort(key=lambda row: (row['group'], row['name']))
            (root / 'report.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
            statuses = ', '.join(key + '=' + value['status'] for key, value in result.get('results', {}).items())
            print(f"{len(report['cases']):02d}/{len(cases)} {case['group']}/{case['name']}: {statuses or result.get('harness_error')}", flush=True)
    digest = hashlib.sha256((root / 'report.json').read_bytes()).hexdigest()
    (root / 'report.sha256').write_text(digest + '\n', encoding='ascii')
    print('发现报告 SHA256：' + digest, flush=True)
    if options.verify:
        failures = []
        for row in report['cases']:
            if 'harness_error' in row:
                failures.append(row['name'])
            for backend, result in row.get('results', {}).items():
                allowed = ['rejected'] if row['outcome'] == 'reject' else ['pass', 'bounded_error'] if row['outcome'] == 'bounded' else ['pass']
                if result['status'] not in allowed:
                    failures.append(row['name'] + '/' + backend + ': ' + result['status'])
        if failures:
            raise AssertionError('回归未通过：' + ', '.join(failures))
        print('三类代表用例回归通过。', flush=True)
    else:
        print('本次仅记录观察结果，不在测试运行中修改实现。', flush=True)


if __name__ == '__main__':
    main()
