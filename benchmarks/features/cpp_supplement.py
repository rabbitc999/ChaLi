"""仅补测此前没有 C++ 参考的三个特征，不重新运行已有 Rust／Java／C++ 基线。"""
import argparse
from datetime import datetime
import json
from pathlib import Path
import platform
import re
import shutil
import statistics
import run as features


def main():
    """核对冻结负载，再建立三项新的 C++ O3 参考一次。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--runs', default=5, type=int)
    parser.add_argument('--preflight', action='store_true')
    args = parser.parse_args()
    cases = ['enum_payload', 'interface_mono', 'interface_poly']
    root, here = features.ROOT, features.HERE
    baseline = json.loads(args.reference.read_text(encoding='utf-8'))
    for name in ['timing.c', 'run.py'] + [case + '.chl' for case in cases]:
        assert features.digest(here / name) == baseline['source_sha256'][name], name
    expected = {case: [features.oracle(case, seed) for seed in range(5)] for case in cases}
    assert all(expected[case] == baseline['oracle_values'][case] for case in cases)
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    report = dict(generated=datetime.now().isoformat(), platform=platform.platform(),
                  git_commit=features.execute(['git', 'rev-parse', 'HEAD']).stdout.strip(),
                  working_tree_dirty=bool(features.execute(['git', 'status', '--porcelain']).stdout),
                  reference_path=str(args.reference), reference_sha256=features.digest(args.reference),
                  source_sha256={name: features.digest(here / name) for name in
                                 ['cpp_supplement.py', 'timing.c'] + [case + '.cpp' for case in cases]},
                  tool_version=features.execute([shutil.which('g++'), '--version']).stdout,
                  runs=args.runs, reps_per_run=5, preflight=args.preflight,
                  previous_reference_languages_rerun=False, cases={})
    artifacts = {case: features.build(case, 'cpp-o3', None, root / 'tmp/features-cpp-supplement') for case in cases}
    for case in cases:
        artifact = artifacts[case]
        row = dict(build=artifact['logs'], binary_sha256=features.digest(artifact['product']), processes=[])
        if args.preflight:
            result = features.execute([artifact['product'], '--verify'])
            values = [int(value) for _, value in re.findall(r'^value (\d+) (-?\d+)$', result.stdout, re.M)]
            assert values == expected[case] and not result.stderr, (case, result)
            row['verified_values'] = values
            print('verify', case, 'OK', flush=True)
        else:
            for _ in range(args.runs):
                result = features.execute([artifact['product']])
                samples = list(map(int, re.findall(r'^rep (\d+)$', result.stdout, re.M)))
                sink = re.search(r'^sink (-?\d+)$', result.stdout, re.M)
                assert len(samples) == 5 and min(samples) > 0 and sink and not result.stderr, result
                assert int(sink[1]) == expected[case][0] + sum(expected[case]), (case, sink[1])
                row['processes'].append(dict(samples_ns=samples, stdout=result.stdout, stderr=result.stderr))
            row['median_ms'] = statistics.median(n for process in row['processes'] for n in process['samples_ns']) / 1000000
            print('median', case, row['median_ms'], 'ms', flush=True)
        report['cases'][case] = row
        (out / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    main()
