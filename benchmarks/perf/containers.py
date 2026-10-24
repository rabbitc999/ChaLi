"""默认 Map/List/Set：交错测量优化前后 VM/AOT，并比较 C++、Python 和 Lua。"""
from container_cases import kernels, source, expected, N, WIDTH, REPS
from compare_vm import compiler_version
from compiler_path import compiler_path
from project_temp import tempfile
from build_lua import HOST as LUA_HOST
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]


def execute(command, cwd, timeout=240):
    """执行构建或负载，失败时保留原始输出，不把失败耗时作为性能样本。"""
    started = time.perf_counter_ns()
    result = subprocess.run(list(map(str, command)), cwd=cwd, capture_output=True, text=True,
                            encoding='utf-8', errors='replace', timeout=timeout)
    if result.returncode:
        raise RuntimeError(str(command) + '\n' + result.stdout + result.stderr)
    return dict(command=list(map(str, command)), stdout=result.stdout, stderr=result.stderr,
                wall_ns=time.perf_counter_ns() - started)


def prepare(before, after):
    """先完成所有构建，避免将编译负载混进随后的计时。"""
    work = Path(tempfile.mkdtemp(prefix='container-benchmark-'))
    report = dict(work=str(work), unit='nanoseconds', operations=N, width=WIDTH, repetitions=REPS,
                  generated_utc=datetime.now(timezone.utc).isoformat(), host=platform.platform(),
                  processor=platform.processor(), cpu_count=os.cpu_count(), compilers={}, builds={}, cases={})
    for label, compiler in [('before', before), ('after', after)]:
        report['compilers'][label] = dict(path=str(compiler), version=compiler_version(compiler),
                                         sha256=hashlib.sha256(compiler.read_bytes()).hexdigest())
    report['tools'] = dict(cpp=execute(['g++', '--version'], work)['stdout'].splitlines()[0],
                           python=sys.version, llvm=execute([os.environ['CHALI_LLVM_CLANG'], '--version'], work)['stdout'])
    execute([sys.executable, ROOT / 'benchmarks/perf/build_lua.py'], ROOT)
    report['tools']['lua'] = execute([LUA_HOST, '--version'], work)['stdout'].strip()
    for case in kernels('chali'):
        folder = work / case
        folder.mkdir()
        entry = dict(expected_sink=expected(case), commands={}, source_sha256={})
        paths = {}
        for language, extension in [('chali', 'chl'), ('cpp', 'cpp'), ('python', 'py'), ('lua', 'lua')]:
            path = folder / ('work.' + extension)
            path.write_text(source(language, kernels(language)[case]), encoding='utf-8')
            paths[language] = path
            entry['source_sha256'][language] = hashlib.sha256(path.read_bytes()).hexdigest()
        cpp = folder / ('cpp.exe' if os.name == 'nt' else 'cpp')
        report['builds'][case + '-cpp'] = execute(['g++', '-std=c++17', '-O2', '-DNDEBUG', paths['cpp'], '-o', cpp], folder)
        entry['commands'].update(cpp=[str(cpp)], python=[sys.executable, str(paths['python'])], lua=[str(LUA_HOST), str(paths['lua'])])
        for label, compiler in [('before', before), ('after', after)]:
            prefix = folder / label
            report['builds'][case + '-' + label] = execute([compiler, 'aot', paths['chali'], '--out', prefix], folder)
            entry['commands'][label + '_vm'] = [str(compiler), 'run', str(paths['chali']), '--out', str(folder / (label + '-vm'))]
            entry['commands'][label + '_aot'] = [str(prefix.with_suffix('.exe' if os.name == 'nt' else ''))]
        report['cases'][case] = entry
        (work / 'prepared.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        print('容器基准构建完成：' + case, flush=True)
    return report


def measure(report, runs):
    """轮流改变配置执行顺序；核对闭式公式后保存五轮内部计时与进程总耗时。"""
    work = Path(report['work'])
    report['runs'] = runs
    report['execution_order'] = []
    for case, entry in report['cases'].items():
        entry['measurements'] = {name: [] for name in entry['commands']}
        for run in range(runs):
            order = list(entry['commands'])
            offset = run % len(order)
            order = order[offset:] + order[:offset]
            if run % 2:
                order.reverse()
            report['execution_order'].append(dict(case=case, run=run, order=order))
            for name in order:
                result = execute(entry['commands'][name], work)
                lines = result['stdout'].splitlines()
                times = [int(line.split()[1]) for line in lines if line.startswith('rep ')]
                sinks = [int(line.split()[1]) for line in lines if line.startswith('sink ')]
                if len(times) != REPS or sinks != [entry['expected_sink']] or any(value <= 0 for value in times):
                    raise RuntimeError(f'{case}/{name} 计时或校验值错误：{result}')
                result['samples_ns'] = times
                entry['measurements'][name].append(result)
        entry['statistics'] = {}
        for name, measurements in entry['measurements'].items():
            samples = [value for measurement in measurements for value in measurement['samples_ns']]
            entry['statistics'][name] = dict(median=statistics.median(samples), min=min(samples), max=max(samples), samples=len(samples))
        print(case + ': ' + ', '.join(name + '=' + format(value['median'] / 1e6, '.3f') + 'ms' for name, value in entry['statistics'].items()), flush=True)
    return report


def main():
    """构建和计时可分两步执行，便于等待其他验证任务结束后再测量。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before', type=Path)
    parser.add_argument('--after', type=Path, default=compiler_path())
    parser.add_argument('--prepared', type=Path)
    parser.add_argument('--prepare-only', action='store_true')
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--out', type=Path)
    options = parser.parse_args()
    if options.runs < 1:
        parser.error('--runs 必须为正数')
    os.environ['CHALI_LLVM_CLANG'] = str(ROOT / 'library/llvm/toolchain/bin' / ('clang.exe' if os.name == 'nt' else 'clang'))
    if options.prepared:
        report = json.loads(options.prepared.read_text(encoding='utf-8'))
    else:
        if not options.before:
            parser.error('首次构建需要 --before 指定保存的基线编译器')
        report = prepare(options.before.resolve(), options.after.resolve())
    if options.prepare_only:
        print('已准备：' + str(Path(report['work']) / 'prepared.json'), flush=True)
        return
    if not options.out:
        parser.error('测量需要 --out 指定原始结果文件')
    result = measure(report, options.runs)
    options.out.parent.mkdir(parents=True, exist_ok=True)
    options.out.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    main()
