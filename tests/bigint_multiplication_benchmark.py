"""重复测量 C 大整数乘法交叉点；原始 CSV 和统计只写入仓库 tmp/。"""
import argparse
from collections import defaultdict
import csv
import hashlib
import io
from itertools import combinations
import json
import math
import os
from pathlib import Path
import platform
import random
import shlex
import statistics
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def dimensions(phase):
    """生成算法边界、平方和非平衡长度，避免只测二次幂。"""
    if phase == 'boundary':
        widths = [128, 144, 159, 160, 161, 174, 175, 176, 177, 190, 191, 192, 193,
                  215, 216, 239, 240, 255, 256, 257, 288, 384, 480, 511, 512, 513,
                  1024, 2048, 4096]
        return [(n, n, square) for n in widths for square in (0, 1)], [0, 1, 2, 3]
    if phase == 'skew':
        cases = {(n, max(1, n // ratio), 0) for n in (256, 512, 1024, 2048, 4096, 8192, 16384)
                 for ratio in (2, 3, 4, 8, 16, 32)}
        for n in (512, 2048, 4096, 8192, 16384):
            for center in ((n + 3) // 4, n // 3, 2 * ((n + 2) // 3)):
                cases.update((n, center + offset, 0) for offset in (-1, 0, 1) if 0 < center + offset <= n)
        return sorted(cases), [0, 2, 3, 4]
    if phase == 'shape':
        cases = [(longer, shorter, 0) for longer in (128, 512, 2048, 8192, 16384)
                 for shorter in (1, 2, 4, 8, 16, 32, 64, 128, 256, 512) if shorter <= longer // 2]
        return cases, [0, 1, 2]
    if phase == 'base':
        widths = [*range(1, 17), 20, 24, 28, 31, 32, 33, 40, 48, 56, 63, 64, 65,
                  72, 80, 88, 95, 96, 97, 112, 127, 128, 129, 144, 160, 192, 224, 256,
                  320, 384, 511, 512, 513, 640, 768, 1024, 1536]
        return [(n, n, square) for n in widths for square in (0, 1)], [1, 2]
    if phase == 'toom':
        widths = [64, 80, 96, 112, 128, 144, 160, 192, 216, 240, 256, 288, 320, 384,
                  432, 480, 512, 576, 640, 720, 768, 864, 960, 1024, 1152, 1280, 1440,
                  1536, 1728, 1920, 2048, 2304, 2560, 2880, 3072, 3456, 3840, 4096,
                  4608, 5120, 6144, 8192]
        return [(n, n, square) for n in widths for square in (0, 1)], [1, 2, 3]
    if phase == 'ntt':
        widths = [128, 192, 240, 241, 256, 384, 480, 481, 512, 640, 768, 960, 961,
                  1024, 1025, 1536, 1920, 1921, 2048, 2049, 3072, 3840, 3841, 4096,
                  6144, 7680, 7681, 8192, 12288, 15360, 15361, 16384, 24576, 32768]
        cases = [(n, n, square) for n in widths for square in (0, 1)]
        cases += [(n, max(1, n // ratio), 0) for n in (256, 512, 1024, 2048, 4096, 8192, 16384)
                  for ratio in (2, 4, 8, 32)]
        return cases, [0, 2, 3, 4]
    widths = [1, 2, 3, 4, 5, 8, 16, 24, 31, 32, 33, 48, 63, 64, 65, 96, 127, 128,
              129, 192, 239, 240, 241, 255, 256, 257, 384, 511, 512, 513, 768, 960, 961,
              1024, 1025, 1536, 1920, 1921, 2048, 2049, 3072, 3840, 3841, 4096, 8192, 16384]
    cases = [(n, n, square) for n in widths for square in (0, 1)]
    cases += [(n, max(1, n // ratio), 0) for n in (128, 512, 2048, 8192, 16384) for ratio in (2, 4, 8, 32, 128)]
    return cases, [0]


def percentile(values, fraction):
    """用线性插值报告分位数，保留样本波动。"""
    ordered = sorted(values)
    position = (len(ordered) - 1) * fraction
    low = math.floor(position)
    high = math.ceil(position)
    return ordered[low] + (ordered[high] - ordered[low]) * (position - low)


def build_binary(output, portable, optimization):
    """以 C11 和固定优化选项构建可复现基准，记录编译器实际版本。"""
    assert output.resolve().is_relative_to((ROOT / 'tmp').resolve())
    output.parent.mkdir(parents=True, exist_ok=True)
    sdk = ROOT / 'build/dependencies/llvm/static-sdk'
    env = dict(os.environ, TMP=str(ROOT / 'tmp'), TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'))
    compiler = os.environ.get('CC')
    flags = shlex.split(os.environ.get('CFLAGS', ''))
    if not compiler and os.name == 'nt':
        compiler = str(sdk / 'bin/gcc.exe')
        flags.append('--sysroot=' + sdk.as_posix())
        env['PATH'] = str(sdk / 'bin') + os.pathsep + env['PATH']
    compiler = compiler or 'gcc'
    sources = sorted((ROOT / 'source/bignum').glob('chali_bigint*.c'))
    sources += [ROOT / 'source/runtime_c/chali_memory.c', ROOT / 'source/runtime_c/chali_buffer.c',
                ROOT / 'tests/bigint_multiplication_benchmark.c']
    command = [compiler, '-std=c11', '-' + optimization, '-Wall', '-Wextra', '-Werror',
               '-DCHALI_BIGINT_TESTING', *(['-DBIGINT_FORCE_PORTABLE_MULTIPLY'] if portable else []),
               '-I' + str(ROOT / 'source/bignum'), *flags, *map(str, sources), '-o', str(output.resolve())]
    subprocess.run(command, check=True, env=env, cwd=ROOT)
    version = subprocess.check_output([compiler, '--version'], env=env, text=True).splitlines()[0]
    return dict(command=command, compiler=version, portable=portable, optimization=optimization)


def main():
    """生成随机次序的测量协议，在 C 内部计时并汇总原始样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--build', action='store_true', help='以 C11 -O3 构建 --binary 指定的 tmp 内文件')
    parser.add_argument('--portable', action='store_true', help='--build 时关闭原生 128 位乘积')
    parser.add_argument('--optimization', choices=['O2', 'O3'], default='O2', help='C 内核优化级别；默认与发布宿主一致使用 O2')
    parser.add_argument('--phase', choices=['base', 'toom', 'ntt', 'shape', 'verify', 'audit', 'boundary', 'skew'], required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=9)
    parser.add_argument('--target-us', type=int, default=2000)
    parser.add_argument('--school', type=int, default=256)
    parser.add_argument('--square', type=int, default=256)
    parser.add_argument('--toom', type=int, default=192)
    parser.add_argument('--toom-square', type=int, default=256)
    parser.add_argument('--ntt', type=int, default=(1 << 64) - 1)
    parser.add_argument('--guard', type=int, default=96)
    parser.add_argument('--seed', type=int, default=20261105)
    parser.add_argument('--patterns', default='0', help='逗号分隔：0 随机、1 全一、2 交替位、3 稀疏')
    parser.add_argument('--cpu', type=int, default=1, help='只设置当前基准线程的亲和性')
    parser.add_argument('--paired', action='store_true', help='在同一输入上逐轮交错算法，减少频率和后台负载偏差')
    parser.add_argument('--default-policy', action='store_true', help='保留二进制实际编译默认值，适用于最终与历史默认策略比较')
    parser.add_argument('--max-words', type=int, help='只测量不超过此长侧分块数的样本')
    parser.add_argument('--min-words', type=int, help='只测量达到此长侧分块数的样本')
    parser.add_argument('--short-words', help='shape 阶段覆盖的较短操作数长度，逗号分隔')
    parser.add_argument('--routes', help='覆盖该阶段默认算法列表，逗号分隔：0 自动、1 学校、2 Karatsuba、3 Toom、4 NTT')
    args = parser.parse_args()
    output = args.output.resolve()
    if not output.is_relative_to((ROOT / 'tmp').resolve()):
        parser.error('--output 必须位于仓库 tmp/ 内')
    output.parent.mkdir(parents=True, exist_ok=True)
    build = build_binary(args.binary, args.portable, args.optimization) if args.build else None
    cases, routes = dimensions(args.phase)
    if args.phase == 'audit':
        widths = [64, 65, 128, 129, 191, 192, 193, 255, 256, 257, 384, 511, 512, 513,
                  768, 1024, 1025, 1536, 2048, 2049, 3072, 3840, 3841, 4096, 6144,
                  7680, 7681, 8192, 12288, 15360, 15361, 16384, 24576, 32768]
        cases = [(n, n, s) for n in widths for s in (0, 1)]
        cases += [(n, k, 0) for n in (2048, 8192, 16384) for k in (1, 8, 64, 128, 129, 256, 257, 512, 513, 1024, 1025)
                  if k <= n // 2]
        routes = [0, 1, 2, 3, 4]
    if args.short_words:
        if args.phase != 'shape':
            parser.error('--short-words 只适用于 shape 阶段')
        shorter = [int(value) for value in args.short_words.split(',')]
        cases = [(n, k, 0) for n in (512, 2048, 8192, 16384) for k in shorter if 0 < k <= n // 2]
    if args.max_words:
        cases = [case for case in cases if max(case[:2]) <= args.max_words]
    if args.min_words:
        cases = [case for case in cases if max(case[:2]) >= args.min_words]
    if args.routes:
        routes = [int(value) for value in args.routes.split(',')]
        if not routes or len(routes) != len(set(routes)) or any(value not in range(5) for value in routes):
            parser.error('--routes 必须是零到四且不重复的算法编号')
    patterns = [int(value) for value in args.patterns.split(',')]
    requests = [(a, b, square, route, pattern) for a, b, square in cases for route in routes for pattern in patterns
                if not (args.phase == 'audit' and route == 1 and max(a, b) > 8192)]
    requested = set(requests)
    rng = random.Random(args.seed)
    rng.shuffle(requests)
    protocol = f'policy {args.school} {args.square} {args.toom} {args.toom_square} {args.ntt} {args.guard}\n'
    if args.default_policy:
        protocol = ''
    if args.paired:
        commands = []
        paired_cases = [(a, b, square, pattern) for a, b, square in cases for pattern in patterns]
        rng.shuffle(paired_cases)
        for a, b, square, pattern in paired_cases:
            for _ in range(args.samples):
                order = [route for route in routes if (a, b, square, route, pattern) in requested]
                rng.shuffle(order)
                commands.extend((a, b, square, route, pattern) for route in order)
        sample_count = 1
    else:
        commands = requests
        sample_count = args.samples
    protocol += ''.join(f'bench {a} {b} {square} {route} {args.target_us} {sample_count} '
                        f'{args.seed ^ a ^ (b << 16)} {pattern}\n' for a, b, square, route, pattern in commands)
    env = dict(os.environ, TMP=str(ROOT / 'tmp'), TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'),
               CHALI_BENCH_CPU=str(args.cpu))
    run = subprocess.run([str(args.binary.resolve())], input=protocol, cwd=ROOT, env=env,
                         capture_output=True, text=True, encoding='utf-8', timeout=1800)
    output.with_suffix('.csv').write_text(run.stdout, encoding='utf-8')
    assert run.returncode == 0 and not run.stderr, (run.returncode, run.stderr)
    groups = defaultdict(list)
    selected_routes = {}
    compiled_defaults = None
    reader = csv.reader(io.StringIO(run.stdout))
    for row in reader:
        if row and row[0] == '# route':
            a, b, square, pattern, route = map(int, row[1:])
            selected_routes[(a, b, square, pattern)] = route
        if row and row[0] == '# defaults':
            compiled_defaults = dict(zip(['school', 'square', 'toom', 'toom_square', 'ntt', 'guard'], map(int, row[1:])))
        if not row or row[0].startswith('#'):
            continue
        assert len(row) == 11, row
        groups[tuple(map(int, row[:5]))].append(row)
    assert len(groups) == len(requests), (len(groups), len(requests))
    records = []
    for key, rows in sorted(groups.items()):
        assert len(rows) == args.samples, (key, len(rows))
        times = [float(row[7]) for row in rows]
        cpu_times = [float(row[10]) for row in rows]
        a, b, square, route, pattern = key
        records.append(dict(a=a, b=b, square=square, route=route, pattern=pattern,
                            selected_route=selected_routes.get((a, b, square, pattern)) if route == 0 else route,
                            median_ns=statistics.median(times), p10_ns=percentile(times, .1),
                            p90_ns=percentile(times, .9), allocations=statistics.median(float(row[8]) for row in rows),
                            peak_extra_bytes=max(int(row[9]) for row in rows),
                            median_cpu_ticks=statistics.median(cpu_times), p10_cpu_ticks=percentile(cpu_times, .1),
                            p90_cpu_ticks=percentile(cpu_times, .9),
                            min_iterations=min(int(row[6]) for row in rows)))
    report = dict(phase=args.phase, platform=platform.system(), machine=platform.machine(),
                  seed=args.seed, samples=args.samples, target_us=args.target_us,
                  cpu=args.cpu, paired=args.paired,
                  max_words=args.max_words,
                  min_words=args.min_words,
                  short_words=args.short_words,
                  cpu_unit='thread_cycles' if os.name == 'nt' else 'thread_cpu_nanoseconds',
                  policy=dict(school=args.school, square=args.square, toom=args.toom,
                              toom_square=args.toom_square, ntt=args.ntt, guard=args.guard),
                  binary=args.binary.name, cases=len(requests), records=records)
    report['build'] = build
    report['paired_comparisons'] = []
    if args.paired:
        shapes = defaultdict(dict)
        for (a, b, square, route, pattern), rows in groups.items():
            shapes[(a, b, square, pattern)][route] = [float(row[7]) for row in rows]
        for (a, b, square, pattern), measurements in sorted(shapes.items()):
            for numerator, denominator in combinations(sorted(measurements), 2):
                ratios = [x / y for x, y in zip(measurements[numerator], measurements[denominator])]
                report['paired_comparisons'].append(dict(a=a, b=b, square=square, pattern=pattern,
                    numerator_route=numerator, denominator_route=denominator,
                    median_ratio=statistics.median(ratios), p10_ratio=percentile(ratios, .1),
                    p90_ratio=percentile(ratios, .9)))
    report['compiled_defaults'] = compiled_defaults
    report['default_policy'] = args.default_policy
    report['binary_sha256'] = hashlib.sha256(args.binary.resolve().read_bytes()).hexdigest()
    report['sources_sha256'] = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                               for p in sorted((ROOT / 'source/bignum').glob('chali_bigint*')) if p.is_file()}
    if args.default_policy:
        report['policy'] = 'compiled defaults; no setters invoked'
    output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(f'{args.phase}: {len(requests)} cells × {args.samples} samples; complete products checked; {output}', flush=True)


if __name__ == '__main__':
    main()
