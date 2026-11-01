"""在 CPU <40% 时比较两份真实编译器的 LSP 延迟，不修改工程或运行其他语言基准。"""
from pathlib import Path
import argparse
from datetime import datetime
import hashlib
import json
import math
import os
import platform
import shutil
import statistics
import subprocess
import sys
import time
from project_temp import tempfile
from cpu_budget import timing_budget

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
from native_lsp_transport import Transport, position


class Deferred(RuntimeError):
    """CPU 不满足计时条件，本轮数字不交付。"""


def cpu_check(samples):
    """在计时区间外采样 CPU，并拒绝忙碌或计数不可用的环境。"""
    budget = timing_budget()
    samples.append(budget)
    if not budget['allowed']:
        raise Deferred(budget['reason'])


def summary(values):
    """使用中位数与最近秩 P95，保留最终真实样本供复核。"""
    ordered = sorted(values)
    return dict(p50Ms=round(statistics.median(ordered), 6),
                p95Ms=round(ordered[math.ceil(len(ordered) * .95) - 1], 6),
                samplesMs=[round(value, 6) for value in values])


def program(value):
    """生成有宏及局部类型推断的固定规模程序，只替换实际字面量。"""
    locals = '\n'.join(f'int64 fixed{i}={i};' for i in range(200))
    return '$macro once(n)\nn\n$end_macro\nvoid main(){\n' + locals + f'\nlet count=$once({value});assert(count=={value});}}\n'


def measure(binary, count, cpu):
    """在空 LSP 缓存中测量启动、首次诊断及稳定请求，所有结果均校验。"""
    identity = json.loads(subprocess.check_output([str(binary), '--build-info'], text=True, encoding='utf-8'))
    record = dict(compilerSha256=hashlib.sha256(binary.read_bytes()).hexdigest(), buildInfo=identity)
    with tempfile.TemporaryDirectory(prefix='lsp-benchmark-') as directory:
        root = Path(directory)
        compiler = root / ('chali.exe' if sys.platform == 'win32' else 'chali')
        shutil.copy2(binary, compiler)
        source = root / 'main.chl'
        text = program(1)
        source.write_text(text, encoding='utf-8')
        uri = source.as_uri()
        cpu_check(cpu)
        started = time.perf_counter_ns()
        server = Transport(compiler, root)
        try:
            server.query('initialize', dict(rootUri=root.as_uri()))
            record['processStartMs'] = (time.perf_counter_ns() - started) / 1e6
            cpu_check(cpu)
            started = time.perf_counter_ns()
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=text, version=1))))
            server.diagnostic(uri, 1, lambda values: not values, timeout=90)
            record['firstDiagnosticsMs'] = (time.perf_counter_ns() - started) / 1e6
            cpu_check(cpu)
            # 同一新版本的类型提示、宏预览与后台诊断共享输入。
            before = server.query('chali/lspStats')['checks']
            text = program(1000)
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text=text)])))
            server.send(dict(id=100, method='textDocument/inlayHint', params=dict(textDocument=dict(uri=uri), range=dict(start=position(text, 0), end=position(text, len(text))))))
            server.send(dict(id=101, method='chali/expandMacro', params=dict(textDocument=dict(uri=uri), mode='all')))
            for serial in (100, 101):
                response = server.response(serial, timeout=90)
                assert 'error' not in response, response
            server.diagnostic(uri, 2, lambda values: not values, timeout=90)
            record['concurrentSemanticChecks'] = server.query('chali/lspStats')['checks'] - before
            cpu_check(cpu)
            edits = []
            for index in range(count):
                text = program(index + 2)
                started = time.perf_counter_ns()
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=index + 3), contentChanges=[dict(text=text)])))
                server.diagnostic(uri, index + 3, lambda values: not values, timeout=90)
                edits.append((time.perf_counter_ns() - started) / 1e6)
            record['editDiagnostics'] = summary(edits)
            cpu_check(cpu)
            hover, completion = [], []
            cursor = position(text, text.rindex('count') + 3)
            for _ in range(count):
                started = time.perf_counter_ns()
                result = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=cursor))
                hover.append((time.perf_counter_ns() - started) / 1e6)
                assert 'int64 count' in result['contents']['value'], result
                started = time.perf_counter_ns()
                result = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=cursor))
                completion.append((time.perf_counter_ns() - started) / 1e6)
                items = result.get('items', []) if isinstance(result, dict) else result
                assert any(item['label'] == 'count' for item in items), items
            record['hover'] = summary(hover)
            record['completion'] = summary(completion)
            record['stats'] = server.query('chali/lspStats')
            record['stats'].pop('cacheDirectory', None)
            cpu_check(cpu)
        finally:
            server.close()
    return record


def main():
    """记录真实字节身份和最终数据；CPU 超限时不保留部分性能数字。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--chali', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=20)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    if args.samples < 5:
        parser.error('至少需要五个样本')
    cpu = []
    try:
        baseline = measure(args.baseline.resolve(), args.samples, cpu)
        print('baseline completed', flush=True)
        current = measure(args.chali.resolve(), args.samples, cpu)
        report = dict(schemaVersion=1, status='measured', generatedAt=datetime.now().astimezone().isoformat(),
                      workload=dict(locals=200, hygienicMacro=True, samples=args.samples, cpuLimitPercent=40,
                                    cache='empty LSP cache per process; OS file cache not flushed',
                                    latency='JSON-RPC round trip; edit diagnostics include debounce', p95='nearest rank'),
                      environment=dict(platform=platform.platform(), machine=platform.machine(), processor=platform.processor(), logicalCpus=os.cpu_count(), clientPython=platform.python_version()),
                      cpu=cpu, baseline=baseline, current=current)
    except Deferred as error:
        report = dict(schemaVersion=1, status='deferred', reason=str(error), cpu=cpu)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(report['status'], flush=True)


if __name__ == '__main__':
    main()
