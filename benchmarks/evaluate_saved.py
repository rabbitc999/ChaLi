"""只核对已保存的 ChaLi 样本，以 C++ O2×1.20 和冻结 Lua 判定，不重新计时。"""
import argparse
from decimal import Decimal
import hashlib
import json
import math
from pathlib import Path
import statistics
import sys

ROOT = Path(__file__).resolve().parents[1]
BASIC = ('int_chain', 'float_chain', 'call', 'array', 'list_growth', 'map_growth', 'set_growth')
FEATURES = ('enum_payload', 'interface_mono', 'interface_poly', 'struct_nested', 'array_2d', 'array_3d')
STRINGS = ('string_read', 'string_concat', 'string_slice', 'string_find')
CASES = BASIC + FEATURES + STRINGS
CPP_FACTOR = Decimal('1.20')


def load(path, provenance):
    """保留每个原始输入的字节摘要，不修改测量报告。"""
    path = Path(path).resolve()
    data = path.read_bytes()
    provenance[str(path)] = hashlib.sha256(data).hexdigest()
    return json.loads(data.decode('utf-8-sig'))


def verdict(measured, cpp, lua):
    """逐项判定；Lua 相同耗时不满足更快，没有 Lua 参考则明确留空。"""
    if any(isinstance(value, bool) for value in (measured, cpp, lua)):
        raise ValueError('布尔值不是耗时')
    if not all(math.isfinite(x) and x > 0 for x in (measured, cpp)):
        raise ValueError('耗时必须为有限正数')
    if lua is not None and (not math.isfinite(lua) or lua <= 0):
        raise ValueError('Lua 参考必须为有限正数')
    limit = Decimal(str(cpp)) * CPP_FACTOR
    cpp_pass = Decimal(str(measured)) <= limit
    lua_pass = measured < lua if lua is not None else None
    return dict(cpp_pass=cpp_pass, cpp_limit_ms=float(limit), lua_pass=lua_pass,
                passed=cpp_pass and lua_pass is not False)


def samples_and_median(report, case):
    """从原始纳秒样本重算中位数，并核对旧报告的舍入值。"""
    if 'configs' in report:
        cell = report['configs']['chali-aot']
        samples = cell['samples_ns'][case]
        reported = cell['median_ms'][case]
    else:
        cell = report['configurations'][case]['chali-aot']
        samples = []
        for process in cell['processes']:
            if process.get('code', 0) != 0 or process.get('stderr'):
                raise ValueError(case + ' 存在失败或带 stderr 的进程')
            samples.extend(process['samples_ns'])
        reported = cell['median_ms']
    if len(samples) < 25 or any(isinstance(x, bool) or not isinstance(x, (int, float))
                                 or not math.isfinite(x) or x <= 0 for x in samples):
        raise ValueError(case + ' 需要至少 25 个有限正数纳秒样本')
    exact = statistics.median(samples) / 1_000_000
    if isinstance(reported, bool) or not math.isfinite(reported) or abs(exact - reported) > 0.000501:
        raise ValueError(case + ' 报告中位数与原始样本不一致')
    return exact, reported, len(samples)


def main():
    """缺项、重复、摘要不符或混用编译器默认失败，历史分析须显式选择宽松模式。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reports', nargs='+', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--compiler', type=Path, help='核对实际被交付编译器的字节摘要')
    parser.add_argument('--partial', action='store_true', help='只分析部分负载，不作为完整候选验证')
    parser.add_argument('--allow-mixed-compilers', action='store_true', help='仅供明确标注的历史快照')
    args = parser.parse_args()
    if args.out.exists():
        parser.error('输出目录必须尚不存在，禁止覆盖历史判定')
    provenance = {}
    saved = ROOT / 'benchmarks/results'
    cpp = load(saved / '1.0.11-cpp-o2-reference/results.json', provenance)
    lua = load(saved / '1.0.11-thread-state/results.json', provenance)
    canonical = {
        'basic': load(saved / '1.0.11-reference-storage-children/results.json', provenance),
        'features': load(saved / '1.0.11-feature-baseline/results.json', provenance),
        'strings': load(saved / '1.0.11-string-baseline/results.json', provenance),
    }
    rows, compilers, allocators, hosts = {}, set(), set(), set()
    for path in args.reports:
        report = load(path, provenance)
        if report.get('runs') != 5 or report.get('reps_per_run') != 5:
            raise ValueError(str(path) + ' 不是正式的五进程各五样本配置')
        compiler = report.get('chali_compiler', {}).get('sha256') or report.get('compiler_sha256')
        allocator = report.get('chali_allocator') or report.get('allocator')
        if (not isinstance(compiler, str) or len(compiler) != 64
            or any(letter not in '0123456789abcdefABCDEF' for letter in compiler)
            or allocator not in ('malloc', 'pool') or report.get('working_tree_dirty') is not False):
            raise ValueError(str(path) + ' 缺少可信编译器／分配器信息或测量工作区不干净')
        compilers.add(compiler.lower())
        allocators.add(allocator)
        hosts.add((report.get('platform', ''), report.get('processor', '')))
        cases = list(report['configs']['chali-aot']['median_ms']) if 'configs' in report else [
            case for case, cells in report['configurations'].items() if 'chali-aot' in cells]
        if not cases:
            raise ValueError(str(path) + ' 没有实际 ChaLi 样本')
        for case in cases:
            if case not in CASES or case in rows:
                raise ValueError('未知或重复负载：' + case)
            group = 'basic' if case in BASIC else 'features' if case in FEATURES else 'strings'
            expected_hashes = canonical[group]['source_sha256']
            hashes = report['source_sha256']
            required = 'benchmarks/strings/cases.chl' if group == 'strings' else case + '.chl'
            common = ({'run.py', 'build_aot.py', 'build_lua.py', 'luabench.c', 'hash_int.h'} if group == 'basic' else
                      {'run.py', 'timing.c', 'timing.rs', 'FeatureTiming.java'} if group == 'features' else set(expected_hashes))
            if not (common | {required}) <= hashes.keys() or any(expected_hashes.get(key) != value for key, value in hashes.items()):
                raise ValueError(case + ' 冻结源码／驱动／计时壳摘要不一致')
            measured, reported, count = samples_and_median(report, case)
            if count != 25:
                raise ValueError(case + ' 正式配置必须恰好包含 25 个样本')
            if group == 'strings':
                reference = canonical['strings']['configurations'][case]
                cpp_ms = reference['cpp-o2']['median_ms']
                lua_ms = min(cell['median_ms'] for name, cell in reference.items() if name.startswith('lua'))
            else:
                cpp_ms = cpp['cases'][case]['median_ms']
                lua_ms = lua['configs']['lua']['median_ms'].get(case)
            rows[case] = dict(case=case, chali_ms=measured, reported_chali_ms=reported, samples=count,
                              cpp_o2_ms=cpp_ms, cpp_ratio=measured / cpp_ms,
                              lua_ms=lua_ms, compiler_sha256=compiler, allocator=allocator,
                              compiler_source_commit=(report.get('chali_compiler', {}).get('source_commit')
                                                      or report.get('compiler_source_commit')),
                              measurement=str(path.resolve()), **verdict(measured, cpp_ms, lua_ms))
    if not args.allow_mixed_compilers and (len(compilers) != 1 or len(allocators) != 1):
        raise ValueError('必须使用同一实际编译器及分配器；历史分析须显式指定 --allow-mixed-compilers')
    if len(hosts) != 1:
        raise ValueError('不同测量平台或处理器的结果不能合并验收')
    missing = sorted(set(CASES) - rows.keys())
    if missing and not args.partial:
        raise ValueError('缺少负载：' + ', '.join(missing))
    binary_verified = False
    if args.compiler:
        actual = hashlib.sha256(args.compiler.read_bytes()).hexdigest()
        if compilers != {actual}:
            raise ValueError('提供的编译器与测量报告字节摘要不一致')
        binary_verified = True
    complete = (not missing and len(compilers) == len(allocators) == 1 and binary_verified
                and not args.partial and not args.allow_mixed_compilers)
    result = dict(kind='saved-performance-gate', cpp_factor=float(CPP_FACTOR), chali_optimization='LLVM O3',
                  tool_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  cpp_reference_optimization='O2', benchmarks_executed=False, full_ci_executed=False,
                  compiler_verified=binary_verified, complete_candidate=complete, missing_cases=missing,
                  measurement_hosts=[dict(platform=platform, processor=processor) for platform, processor in sorted(hosts)],
                  mixed_compilers_allowed=args.allow_mixed_compilers, input_sha256=provenance,
                  passed=all(row['passed'] for row in rows.values()),
                  cpp_pass_count=sum(row['cpp_pass'] for row in rows.values()),
                  rows=[rows[case] for case in CASES if case in rows])
    args.out.mkdir(parents=True)
    (args.out / 'results.json').write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    lines = ['# 已保存样本的 20% 性能门槛', '',
             '此入口只核对保存的数据，未重新计时；不能代替最终全量 CI。Lua 缺项明确标注，不冒充快于 Lua。', '',
             '| 负载 | ChaLi / ms | C++ O2 上限 / ms | Lua / ms | 判定 |',
             '| --- | ---: | ---: | ---: | --- |']
    for row in result['rows']:
        lua_text = '无该项参考' if row['lua_ms'] is None else f"{row['lua_ms']:.4f}"
        lines.append(f"| {row['case']} | {row['chali_ms']:.4f} | {row['cpp_limit_ms']:.4f} | {lua_text} | {'通过' if row['passed'] else '未通过'} |")
    lines += ['', f"C++ 条件通过 {result['cpp_pass_count']}/{len(rows)}；全部已选负载达标：{result['passed']}。",
              f"完整且已核对编译器字节的候选数据：{complete}；最终全量 CI：未由本工具执行。"]
    (args.out / 'README.md').write_text('\n'.join(lines) + '\n', encoding='utf-8')
    print(f"saved-data gate: {result['cpp_pass_count']}/{len(rows)} C++ conditions; passed={result['passed']}; {args.out}")
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (ValueError, KeyError, TypeError, OverflowError, OSError) as error:
        print('performance gate:', error, file=sys.stderr)
        sys.exit(2)
