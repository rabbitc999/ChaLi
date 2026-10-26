"""同版本对照 ChaLi 自举 List/Map/Set 与内建容器，保存正确性及原始计时。"""
from pathlib import Path
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
import platform
import statistics
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from project_temp import tempfile
from compiler_path import compiler_path
from release import compiler_version
import container_cases


def execute(command, timeout=600):
    """执行并保存完整输出；失败不计入性能样本。"""
    started = time.perf_counter_ns()
    result = subprocess.run([str(x) for x in command], cwd=ROOT, capture_output=True,
                            text=True, encoding='utf-8', timeout=timeout)
    if result.returncode:
        raise RuntimeError(str(command) + '\n' + result.stdout + result.stderr)
    return dict(command=[str(x) for x in command], stdout=result.stdout, stderr=result.stderr,
                wall_ns=time.perf_counter_ns() - started)


def main():
    """先构建全部负载，再交错测量并核对独立闭式公式。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--operations', type=int, default=10000)
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--vm-only', action='store_true')
    parser.add_argument('--cases', nargs='+', help='只测量指定负载，例如 list_remove')
    parser.add_argument('--list-retain-slots', action='store_true', help='实验 List pop 仅缩短长度，不清空已删除槽位')
    options = parser.parse_args()
    if options.operations < 1024 or options.runs < 1:
        parser.error('operations 至少 1024，runs 必须为正数')
    compiler = options.chali.resolve()
    container_cases.N = options.operations
    output = options.out.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix='bootstrap-containers-'))
    prototype = (ROOT / 'benchmarks/perf/bootstrap_containers.chl').read_text(encoding='utf-8')
    if options.list_retain_slots:
        retained = (ROOT / 'source/std/array.chl').read_text(encoding='utf-8')
        retained = retained.replace('module std::array;', '')
        retained = retained.replace('DynamicArray', 'RetainedDynamicArray').replace('__ArraySlot', '__RetainedArraySlot')
        retained = retained.replace('self.resize(self.__count - 1);', 'self.__count = self.__count - 1;')
        prototype = ('import std::array;\n' + retained + '\n' +
                     prototype.replace('import std::array;', '').replace('std::array::DynamicArray', 'RetainedDynamicArray'))
    report = dict(version=compiler_version(compiler),
                  compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(),
                  prototype_sha256=hashlib.sha256(prototype.encode()).hexdigest(),
                  generated_utc=datetime.now(timezone.utc).isoformat(),
                  host=platform.platform(), processor=platform.processor(), cpu_count=os.cpu_count(),
                  operations=options.operations, width=container_cases.WIDTH,
                  process_runs=options.runs, repetitions=5, warmups=1, unit='nanoseconds',
                  list_retain_slots=options.list_retain_slots,
                  work=str(work), cases={}, execution_order=[],
                  limitations=['int64 elements and keys; generic Map values also checked separately',
                               'Map uses modulo hash, linear probing and tombstones; Set wraps Map',
                               'build, allocation and cleanup inside work are timed; compilation is excluded',
                               'default highest optimization, reflection and runtime safety checks retained'])

    def save():
        """逐步写出证据，失败后仍保留已完成部分。"""
        output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

    backends = ('vm',) if options.vm_only else ('vm', 'aot')
    for case, body in container_cases.kernels('chali').items():
        if options.cases and case not in options.cases:
            continue
        entry = dict(expected_sink=container_cases.expected(case), sources={}, builds={}, commands={}, measurements={})
        report['cases'][case] = entry
        for variant in ('native', 'bootstrap'):
            selected = body
            prefix = ''
            if variant == 'bootstrap':
                selected = selected.replace('List<int64> values;', 'BootstrapList<int64> values;')
                selected = selected.replace('Map<int64, int64> values;', 'BootstrapMap<int64> values;')
                selected = selected.replace('Set<int64> values;', 'BootstrapSet values;')
                selected = selected.replace('values[i % 1024] = i + seed;', 'values.set(i % 1024, i + seed);')
                prefix = prototype + '\n'
            path = work / (case + '-' + variant + '.chl')
            generated = container_cases.source('chali', selected)
            if prefix:
                generated = 'import std::clock;\n' + prefix + generated.removeprefix('import std::clock;\n')
            path.write_text(generated, encoding='utf-8')
            entry['sources'][variant] = dict(path=str(path), sha256=hashlib.sha256(path.read_bytes()).hexdigest())
            for backend in backends:
                name = variant + '_' + backend
                target = work / (case + '-' + name + ('.exe' if os.name == 'nt' else ''))
                if backend == 'aot':
                    entry['builds'][name] = execute([compiler, 'aot', path, '--out', target])
                    entry['commands'][name] = [str(target)]
                else:
                    entry['commands'][name] = [str(compiler), 'run', str(path), '--out', str(target)]
                entry['measurements'][name] = []
        save()
        print('准备完成：' + case, flush=True)
    for case, entry in report['cases'].items():
        for run in range(options.runs):
            names = list(entry['commands'])
            offset = run % len(names)
            names = names[offset:] + names[:offset]
            if run % 2:
                names.reverse()
            report['execution_order'].append(dict(case=case, run=run, order=names))
            for name in names:
                result = execute(entry['commands'][name])
                times = [int(line.split()[1]) for line in result['stdout'].splitlines() if line.startswith('rep ')]
                sinks = [int(line.split()[1]) for line in result['stdout'].splitlines() if line.startswith('sink ')]
                assert len(times) == 5 and sinks == [entry['expected_sink']], (case, name, result)
                result['times_ns'] = times
                entry['measurements'][name].append(result)
                save()
                print(case + ' ' + name + ' ' + str(run + 1) + ': ' + str(round(statistics.median(times) / 1e6, 3)) + ' ms', flush=True)
        entry['median_ns'] = {name: statistics.median(t for result in samples for t in result['times_ns'])
                              for name, samples in entry['measurements'].items()}
        entry['bootstrap_over_native'] = {backend: entry['median_ns']['bootstrap_' + backend] / entry['median_ns']['native_' + backend]
                                          for backend in backends}
        save()
    print('原始结果：' + str(output), flush=True)


if __name__ == '__main__':
    main()
