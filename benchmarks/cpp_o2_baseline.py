"""建立当前 C++ O2 参考：复用四项旧数据，只首次测量缺少的九个负载。"""
import argparse
from datetime import datetime
import hashlib
import importlib.util
import json
from pathlib import Path
import platform
import re
import shutil
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
PERF = ROOT / 'benchmarks/perf'
FEATURES = ROOT / 'benchmarks/features'
sys.path.insert(0, str(FEATURES))
spec = importlib.util.spec_from_file_location('feature_workloads', FEATURES / 'run.py')
feature = importlib.util.module_from_spec(spec)
spec.loader.exec_module(feature)
REUSED = ['int_chain', 'float_chain', 'call', 'array']
NEW_BASIC = ['list_growth', 'map_growth', 'set_growth']
NEW_FEATURES = list(feature.REFERENCES)


def digest(path):
    """记录真实文件字节摘要。"""
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    """校验旧来源，隔离构建缺少项，显式授权首次配置测量后保存原始样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--runs', type=int, default=5)
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--build-only', action='store_true', help='只构建，不执行或测量参考程序')
    group.add_argument('--establish-baseline', action='store_true', help='仅首次测量此前缺少的 O2 配置')
    args = parser.parse_args()
    old_path = ROOT / 'benchmarks/results/aot-1.0.8/results.json'
    basic_path = ROOT / 'benchmarks/results/1.0.11-thread-state/results.json'
    feature_path = ROOT / 'benchmarks/results/1.0.11-feature-baseline/results.json'
    supplement_path = ROOT / 'benchmarks/results/1.0.11-cpp-feature-supplement/results.json'
    old, basic, baseline, supplement = [json.loads(path.read_text(encoding='utf-8'))
                                        for path in [old_path, basic_path, feature_path, supplement_path]]
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    compiler = shutil.which('g++')
    version = feature.execute([compiler, '--version']).stdout.strip()
    assert re.search(r'\d+\.\d+\.\d+', version)[0] == re.search(r'\d+\.\d+\.\d+', old['tool_versions']['g++'])[0]
    assert old['host'].lower() == platform.node().lower(), '旧参考来自不同宿主，不能无说明地复用'
    report = dict(generated=datetime.now().isoformat(), git_commit=feature.execute(['git', 'rev-parse', 'HEAD']).stdout.strip(),
                  working_tree_dirty=bool(feature.execute(['git', 'status', '--porcelain']).stdout),
                  platform=platform.platform(), processor=platform.processor(), optimization='-O2',
                  compiler_version=version, compiler_sha256=digest(Path(compiler)), runs=args.runs, reps_per_run=5,
                  old_reference_path=str(old_path.relative_to(ROOT)), old_reference_sha256=digest(old_path),
                  old_reference_generated=old['generated'], prior_reference_languages_rerun=False,
                  source_sha256={Path(__file__).name: digest(Path(__file__))}, cases={})
    for case in REUSED:
        source = PERF / (case + '.cpp')
        original = subprocess.check_output(['git', 'show', '48f3c15:' + source.relative_to(ROOT).as_posix()], cwd=ROOT)
        current = source.read_bytes()
        assert original.replace(b'\r\n', b'\n') == current.replace(b'\r\n', b'\n'), case
        assert old['sinks'][case]['cpp'] == basic['sinks'][case]['cpp-o3'], case
        report['cases'][case] = dict(provenance='reused', reference_config='cpp',
                                    median_ms=old['configs']['cpp']['median_ms'][case],
                                    samples_ns=old['configs']['cpp']['samples_ns'][case],
                                    sink=old['sinks'][case]['cpp'], current_source_sha256=digest(source),
                                    initial_git_blob_sha256=hashlib.sha256(original).hexdigest(),
                                    measured_source_sha256=None,
                                    verification='源码自初始入库未变、工具主版本和宿主及校验值一致；旧测量没有当时源码摘要，不伪造历史摘要')
    artifacts = {}
    for case in NEW_BASIC + NEW_FEATURES:
        folder = ROOT / 'tmp/cpp-o2-baseline' / case
        folder.mkdir(parents=True, exist_ok=True)
        product = folder / ('benchmark.exe' if sys.platform == 'win32' else 'benchmark')
        source = (PERF if case in NEW_BASIC else FEATURES) / (case + '.cpp')
        if case in NEW_BASIC:
            assert digest(source) == basic['source_sha256'][source.name], case
            command = [compiler, '-std=c++17', '-O2', '-DNDEBUG', source, '-o', product]
            expected = basic['sinks'][case]['cpp-o3']
        else:
            reference = supplement['source_sha256'] if case in supplement['cases'] else baseline['source_sha256']
            assert digest(source) == reference[source.name], case
            assert digest(FEATURES / 'timing.c') == baseline['source_sha256']['timing.c']
            command = [compiler, '-std=c++17', '-O2', '-DNDEBUG', source, FEATURES / 'timing.c', '-o', product]
            values = baseline['oracle_values'][case]
            expected = str(values[0] + sum(values))
        print('build O2', case, flush=True)
        compiled = feature.execute(command)
        report['source_sha256'][source.relative_to(ROOT).as_posix()] = digest(source)
        row = dict(provenance='new O2 configuration', command=list(map(str, command)),
                   build_stdout=compiled.stdout, build_stderr=compiled.stderr,
                   binary_sha256=digest(product), expected_sink=expected, processes=[])
        report['cases'][case] = row
        artifacts[case] = product
    report['source_sha256']['benchmarks/features/timing.c'] = digest(FEATURES / 'timing.c')
    for case in NEW_BASIC + NEW_FEATURES:
        row = report['cases'][case]
        if args.establish_baseline:
            for _ in range(args.runs):
                result = feature.execute([artifacts[case]])
                samples = list(map(int, re.findall(r'^rep (\d+)$', result.stdout, re.M)))
                sink = re.search(r'^sink (-?\d+)$', result.stdout, re.M)
                assert len(samples) == 5 and min(samples) > 0 and sink and not result.stderr, (case, result)
                assert sink[1] == row['expected_sink'], (case, sink[1])
                row['processes'].append(dict(samples_ns=samples, stdout=result.stdout, stderr=result.stderr))
            row['median_ms'] = statistics.median(n for process in row['processes'] for n in process['samples_ns']) / 1000000
            print('median O2', case, row['median_ms'], 'ms', flush=True)
        (out / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('saved; reused four old O2 items, measured only previously missing O2 configurations' if args.establish_baseline
          else 'build-only; no reference process executed', flush=True)


if __name__ == '__main__':
    main()
