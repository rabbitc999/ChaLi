"""只测 ChaLi 的 malloc/pool O3 对照，复用冻结负载并交错测量进程。"""
from pathlib import Path
import argparse
import hashlib
import json
import math
import os
import platform
import re
import shutil
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / 'tools'), str(ROOT / 'benchmarks/perf')]
from project_temp import tempfile
from compiler_path import compiler_path
from build_aot import llvm_compiler


def digest(path):
    """记录文件真实字节摘要。"""
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    """先完成全部 O3 构建，再交错执行两种分配器，保存每个原始样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--compiler-source-commit', required=True)
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    if args.runs < 1:
        parser.error('--runs 必须为正整数')
    args.out.mkdir(parents=True, exist_ok=False)
    compiler = args.chali.resolve()
    basic = json.loads((ROOT / 'benchmarks/results/1.0.11-reference-storage-children/results.json').read_text(encoding='utf-8'))
    strings = json.loads((ROOT / 'benchmarks/results/1.0.11-string-baseline/results.json').read_text(encoding='utf-8'))
    cases = ['list_growth', 'map_growth', 'set_growth', 'string_concat']
    report = dict(kind='allocator-exploration', runs=args.runs, reps_per_process=5,
                  platform=platform.platform(), processor=platform.processor(), logical_cpus=os.cpu_count(),
                  compiler_sha256=digest(compiler), compiler_source_commit=args.compiler_source_commit,
                  source_sha256={}, builds=[], measurements=[], results={},
                  selection_rule='four allocation workloads, equally weighted geometric mean of pool/malloc medians; final gates remain per workload')

    def save():
        (args.out / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

    def execute(command):
        command = list(map(str, command))
        ran = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=600)
        record = dict(command=command, code=ran.returncode, stdout=ran.stdout, stderr=ran.stderr)
        assert ran.returncode == 0 and not ran.stderr.strip(), record
        return record

    report['git_commit'] = execute(['git', 'rev-parse', 'HEAD'])['stdout'].strip()
    report['compiler_source_commit'] = execute(['git', 'rev-parse', '--verify', args.compiler_source_commit + '^{commit}'])['stdout'].strip()
    report['working_tree_dirty'] = bool(execute(['git', 'status', '--porcelain'])['stdout'].strip())
    for case in cases[:3]:
        for name in [case + '.chl', case + '.c', 'build_aot.py']:
            path = ROOT / 'benchmarks/perf' / name
            assert digest(path) == basic['source_sha256'][name], name
            report['source_sha256']['benchmarks/perf/' + name] = digest(path)
    for name in ['benchmarks/strings/cases.chl', 'benchmarks/features/timing.c']:
        path = ROOT / name
        normalized = hashlib.sha256(path.read_bytes().replace(b'\r\n', b'\n')).hexdigest()
        assert normalized == strings['source_sha256'][name], name
        report['source_sha256'][name] = digest(path)
    report['driver_sha256'] = digest(__file__)
    models = strings['model_values']['string_concat']
    expected = {case: basic['sinks'][case]['chali-aot'] for case in cases[:3]}
    expected['string_concat'] = str(models[0] + sum(models))
    report['expected_sinks'] = expected
    save()
    with tempfile.TemporaryDirectory(prefix='allocator-compare-') as temporary:
        work = Path(temporary)
        products = {}
        for allocator in ['malloc', 'pool']:
            for case in cases:
                folder = work / allocator / case
                folder.mkdir(parents=True)
                product = folder / ('benchmark.exe' if os.name == 'nt' else 'benchmark')
                if case != 'string_concat':
                    report['builds'].append(execute([sys.executable, ROOT / 'benchmarks/perf/build_aot.py', '--allocator', allocator, case, compiler]))
                    shutil.copy2(ROOT / 'benchmarks/perf' / (case + '-aot.exe'), product)
                    library = ROOT / 'benchmarks/perf' / (case + '-kernel' + ('.dll' if os.name == 'nt' else '.so'))
                    target_library = folder / library.name
                    shutil.copy2(library, target_library)
                else:
                    prefix = folder / 'kernel'
                    report['builds'].append(execute([compiler, 'aot', ROOT / 'benchmarks/strings/cases.chl', '--shared', '--unchecked', '--allocator', allocator, '--out', prefix]))
                    target_library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
                    report['builds'].append(execute([llvm_compiler(), '-O3', '-DNDEBUG', '-DCHALI_FEATURE_AOT=1', '-Dfeature_work=string_concat', ROOT / 'benchmarks/features/timing.c', target_library, '-o', product]))
                    check = execute([product, '--verify'])
                    actual = [int(value) for value in re.findall(r'^value \d+ (-?\d+)$', check['stdout'], re.M)]
                    assert actual == models, check
                    report['builds'].append(check)
                products[allocator, case] = product
                report['results'].setdefault(case, {})[allocator] = dict(binary_sha256=digest(product), kernel_sha256=digest(target_library), samples_ns=[])
                save()
                print('built', case, allocator, flush=True)
        for index in range(args.runs):
            for case in cases:
                order = ['malloc', 'pool'] if index % 2 == 0 else ['pool', 'malloc']
                for allocator in order:
                    record = execute([products[allocator, case]])
                    samples = [int(value) for value in re.findall(r'^rep (\d+)$', record['stdout'], re.M)]
                    sink = re.findall(r'^sink (-?\d+)$', record['stdout'], re.M)
                    assert len(samples) == 5 and all(value > 0 for value in samples) and sink == [expected[case]], record
                    record.update(case=case, allocator=allocator, process=index, samples_ns=samples)
                    report['measurements'].append(record)
                    report['results'][case][allocator]['samples_ns'].extend(samples)
                    save()
        ratios = []
        table = ['| 负载 | malloc / ms | pool / ms | pool / malloc |', '| --- | ---: | ---: | ---: |']
        for case in cases:
            cell = report['results'][case]
            for allocator in ['malloc', 'pool']:
                cell[allocator]['median_ms'] = statistics.median(cell[allocator]['samples_ns']) / 1e6
            ratio = cell['pool']['median_ms'] / cell['malloc']['median_ms']
            ratios.append(ratio)
            table.append(f"| {case} | {cell['malloc']['median_ms']:.4f} | {cell['pool']['median_ms']:.4f} | {ratio:.4f} |")
        score = math.exp(sum(map(math.log, ratios)) / len(ratios))
        report['pool_over_malloc_geomean'] = score
        report['selected_allocator'] = 'pool' if score < 1 else 'malloc'
        save()
        (args.out / 'summary.md').write_text('# LLVM O3 分配器小测\n\n' + '\n'.join(table) + f"\n\n四项等权几何比值：{score:.4f}；本次选择 {report['selected_allocator']}。此平均值仅用于选择分配器，最终性能门槛仍逐项核对。每项三进程各五样本（实际次数见 JSON），未执行其他语言。\n", encoding='utf-8')
        print('selected', report['selected_allocator'], 'pool/malloc', score, flush=True)


if __name__ == '__main__':
    main()
