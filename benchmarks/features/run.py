"""语言特征基线：参考语言仅首次测量，后续以 --reference 只复测 ChaLi。"""
import argparse
from datetime import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import statistics
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(ROOT / 'tests'))
sys.path.insert(0, str(ROOT / 'benchmarks/perf'))
from compiler_path import compiler_path
from build_aot import llvm_compiler
from compare_vm import compiler_version

REFERENCES = {
    'enum_payload': ['rust'],
    'interface_mono': ['java-jit'],
    'interface_poly': ['java-jit'],
    'struct_nested': ['cpp-o3', 'rust'],
    'array_2d': ['cpp-o3', 'rust'],
    'array_3d': ['cpp-o3', 'rust'],
}
EXTENSIONS = {'chali-aot': 'chl', 'cpp-o3': 'cpp', 'rust': 'rs', 'java-jit': 'java'}


def digest(path):
    """计算实际文件字节摘要。"""
    return hashlib.sha256(path.read_bytes()).hexdigest()


def execute(command, *, timeout=300):
    """运行真实工具或产物，失败时保留完整原始输出。"""
    result = subprocess.run(list(map(str, command)), cwd=ROOT, capture_output=True,
                            text=True, encoding='utf-8', errors='replace', timeout=timeout,
                            env=dict(os.environ, CHALI_LANG='zh', PYTHONIOENCODING='utf-8'))
    if result.returncode:
        raise RuntimeError((list(map(str, command)), result.returncode, result.stdout, result.stderr))
    return result


def oracle(case, seed, coverage=None):
    """用独立整数模型计算访问语义；不参与计时或生成 LLVM IR。"""
    acc = seed
    for i in range(262144):
        if case == 'enum_payload':
            index = (i * 13 + ((acc >> 7) & 255)) & 255
            if coverage is not None:
                coverage[index % 4] += 1
            value = (17, index * 3 + 7, index * 6 + 11, index * 8 + 13)[index % 4]
        elif case == 'struct_nested':
            index = (i * 13 + (acc & 255)) & 255
            value = (index * 3 + 1 + index * 7 + 3 + index + 5
                     + index * 13 + (i & 3) + index * 17 + ((i + seed) & 7) * 3 + index * 11 + 9)
        elif case == 'array_2d':
            value = ((acc >> 6) & 63) * 131 + ((i + acc) & 63) * 17 + 3
        elif case == 'array_3d':
            value = ((acc >> 4) & 15) * 257 + ((acc >> 8) & 15) * 31 + ((i + seed) & 15) * 7 + 3
        else:
            kind = 0 if case == 'interface_mono' else (i + seed) & 3
            if kind == 1:
                acc = (acc * 5 - 11) & 2147483647
            elif kind == 2:
                acc = ((acc * 7) ^ 13) & 2147483647
            else:
                acc = (acc * 3 + 7) & 2147483647
            continue
        acc = (acc * 33 + value) & 2147483647
    return acc


def build(case, config, compiler, scratch):
    """隔离生成各语言原生产物或 Java 类；全部构建在计时前完成。"""
    folder = scratch / case / config
    folder.mkdir(parents=True, exist_ok=True)
    product = folder / ('benchmark.exe' if os.name == 'nt' else 'benchmark')
    commands, logs = [], []

    def command(arguments):
        """执行并记录一条构建命令。"""
        commands.append(list(map(str, arguments)))
        result = execute(arguments)
        logs.append(dict(command=commands[-1], stdout=result.stdout, stderr=result.stderr))

    if config == 'chali-aot':
        prefix = folder / 'kernel'
        command([compiler, 'aot', HERE / (case + '.chl'), '--shared', '--unchecked',
                 '--allocator', 'malloc', '--out', prefix])
        library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
        command([llvm_compiler(), '-O3', '-DNDEBUG', '-DCHALI_FEATURE_AOT=1',
                 HERE / 'timing.c', library, '-o', product])
    elif config == 'cpp-o3':
        command([shutil.which('g++'), '-std=c++17', '-O3', '-DNDEBUG',
                 HERE / (case + '.cpp'), HERE / 'timing.c', '-o', product])
    elif config == 'rust':
        command([shutil.which('rustc'), '--edition=2021', '-O', HERE / (case + '.rs'), '-o', product])
    else:
        command([shutil.which('javac'), '-d', folder, HERE / 'FeatureTiming.java', HERE / (case + '.java')])
    return dict(folder=folder, product=product, commands=commands, logs=logs)


def launch(case, config, artifact, jit_log=None):
    """保留 Java 默认分层编译；正式测量时写独立 JIT 日志。"""
    if config != 'java-jit':
        return [str(artifact['product'])]
    command = [shutil.which('java'), '-Xbatch']
    if jit_log:
        command.append('-Xlog:jit+compilation=debug:file=' + str(jit_log.relative_to(ROOT)).replace('\\', '/'))
    return command + ['-cp', str(artifact['folder']), case]


def main():
    """先核对语义，再保存首次参考或只测 ChaLi 的真实原始样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--compiler-source-commit', help='实际编译器源码提交，与负载工作区提交分别记录')
    parser.add_argument('--cases', default=','.join(REFERENCES))
    parser.add_argument('--runs', type=int, default=5)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--preflight', action='store_true', help='只构建并核对逐个种子，无性能计时')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--establish-baseline', action='store_true', help='仅首次建立参考语言正式基线')
    group.add_argument('--reference', type=Path, help='复用既有参考；仅构建及测量 ChaLi')
    args = parser.parse_args()
    if not args.preflight and not (args.establish_baseline or args.reference):
        parser.error('首次使用 --establish-baseline，后续必须提供 --reference，避免重复评测参考语言')
    cases = args.cases.split(',')
    if any(case not in REFERENCES for case in cases) or args.runs < 1:
        parser.error('负载名称或进程数无效')
    compiler = args.chali.resolve()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    scratch = ROOT / 'tmp/features'
    reference = json.loads(args.reference.read_text(encoding='utf-8')) if args.reference else None
    names = {'run.py', 'timing.c', 'timing.rs', 'FeatureTiming.java'}
    for case in cases:
        names.add(case + '.chl')
        names.update(case + '.' + EXTENSIONS[config] for config in REFERENCES[case])
    hashes = {name: digest(HERE / name) for name in sorted(names)}
    if reference:
        for name, sha in hashes.items():
            assert reference['source_sha256'][name] == sha, ('参考负载已改变，不能复用', name)
    coverage = [[0, 0, 0, 0] for _ in range(5)]
    expected = {case: [oracle(case, seed, coverage[seed] if case == 'enum_payload' else None)
                       for seed in range(5)] for case in cases}
    if 'enum_payload' in cases:
        assert all(min(counts) > 10000 for counts in coverage), ('枚举变体分布退化', coverage)
    report = dict(generated=datetime.now().isoformat(), platform=platform.platform(),
                  processor=platform.processor(), runs=args.runs, reps_per_run=5,
                  compiler_sha256=digest(compiler), compiler_path=str(compiler),
                  compiler_source_commit=(execute(['git', 'rev-parse', args.compiler_source_commit]).stdout.strip()
                                          if args.compiler_source_commit else None),
                  compiler_version=compiler_version(compiler), allocator='malloc',
                  git_commit=execute(['git', 'rev-parse', 'HEAD']).stdout.strip(),
                  working_tree_dirty=bool(execute(['git', 'status', '--porcelain', '--untracked-files=normal']).stdout),
                  source_sha256=hashes, oracle_values=expected, enum_variant_counts=coverage, preflight=args.preflight,
                  reference_languages_rerun=bool(args.establish_baseline and not args.preflight),
                  configurations={}, tools={})
    if reference:
        report['reference'] = dict(path=str(args.reference), sha256=digest(args.reference),
                                   generated=reference['generated'])
        for case in cases:
            assert reference['oracle_values'][case] == expected[case]
    configs = {case: ['chali-aot'] + ([] if reference else REFERENCES[case]) for case in cases}
    selected = {config for group in configs.values() for config in group}
    for tool, arguments in [('clang', [llvm_compiler(), '--version']),
                            ('g++', [shutil.which('g++'), '--version']),
                            ('rustc', [shutil.which('rustc'), '--version']),
                            ('java', [shutil.which('java'), '-version']),
                            ('javac', [shutil.which('javac'), '-version'])]:
        if tool in ('java', 'javac') and 'java-jit' not in selected:
            continue
        if tool == 'rustc' and 'rust' not in selected or tool == 'g++' and 'cpp-o3' not in selected:
            continue
        version = execute(arguments)
        report['tools'][tool] = dict(command=arguments, output=version.stdout + version.stderr)
    artifacts = {}
    for case in cases:
        for config in configs[case]:
            print('build', case, config, flush=True)
            artifact = build(case, config, compiler, scratch)
            artifacts[case, config] = artifact
            report['configurations'].setdefault(case, {})[config] = dict(build=artifact['logs'], processes=[])
    for case in cases:
        for config in configs[case]:
            row = report['configurations'][case][config]
            artifact = artifacts[case, config]
            if args.preflight:
                result = execute(launch(case, config, artifact) + ['--verify'])
                values = [int(value) for _, value in re.findall(r'^value (\d+) (-?\d+)$', result.stdout, re.M)]
                assert values == expected[case] and not result.stderr, (case, config, result)
                row['verified_values'] = values
                print('verify', case, config, 'OK', flush=True)
            else:
                samples = []
                for run in range(args.runs):
                    jit_log = out / f'{case}-{run}-jit.log' if config == 'java-jit' else None
                    command = launch(case, config, artifact, jit_log)
                    result = execute(command)
                    ns = list(map(int, re.findall(r'^rep (\d+)$', result.stdout, re.M)))
                    sink = re.search(r'^sink (-?\d+)$', result.stdout, re.M)
                    assert len(ns) == 5 and min(ns) > 0 and sink and not result.stderr, (case, config, result)
                    assert int(sink[1]) == expected[case][0] + sum(expected[case]), (case, config, sink[1])
                    process = dict(command=command, samples_ns=ns, stdout=result.stdout, stderr=result.stderr)
                    if jit_log:
                        calls = int(re.search(r'^warmup_calls (\d+)$', result.stdout, re.M)[1])
                        warm_ns = int(re.search(r'^warmup_ns (\d+)$', result.stdout, re.M)[1])
                        uptime = int(re.search(r'^warmup_complete_uptime_ms (\d+)$', result.stdout, re.M)[1])
                        lines = [line for line in jit_log.read_text(encoding='utf-8').splitlines()
                                 if re.search(r'\s4\s+' + case + r'::work\b', line)]
                        assert calls >= 256 and warm_ns >= 1000000000 and lines, (case, '热身或 C2 编译证据不足')
                        assert any(float(re.search(r'\[([0-9.]+)s\]', line)[1]) * 1000 <= uptime for line in lines)
                        process.update(warmup_calls=calls, warmup_ns=warm_ns, c2_work_lines=lines,
                                       jit_log_sha256=digest(jit_log), warmup_complete_uptime_ms=uptime)
                    row['processes'].append(process)
                    samples.extend(ns)
                row['median_ms'] = statistics.median(samples) / 1000000
                print('median', case, config, row['median_ms'], 'ms', flush=True)
            (out / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('saved', out, flush=True)


if __name__ == '__main__':
    main()
