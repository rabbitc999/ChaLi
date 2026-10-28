"""默认字符串四项跨语言基准；Python 仅构建、核对与计时调度，不生成 LLVM IR。"""
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
sys.path.insert(0, str(ROOT / 'benchmarks/perf'))
from project_temp import tempfile
from compiler_path import compiler_path
from build_aot import llvm_compiler
from build_lua import HOST as LUA_HOST
from compare_vm import compiler_version

CASES = ('string_read', 'string_concat', 'string_slice', 'string_find')
CONFIGS = ('chali-aot', 'cpp-o2', 'rust', 'lua')
UNIT = 'ChaLi-字符串-🙂-0123456789|'.encode('utf-8')
TEXT = b''.join(UNIT + bytes([65 + i % 26]) for i in range(256))
WIDTH = len(UNIT) + 1
MASK = 2147483647


def digest(path, text=False):
    """文本仅规范 CRLF，二进制保留真实字节。"""
    data = Path(path).read_bytes()
    return hashlib.sha256(data.replace(b'\r\n', b'\n') if text else data).hexdigest()


def source_hashes():
    """冻结负载、计时壳、驱动与 Lua 宿主来源。"""
    files = [*HERE.glob('cases.*'), HERE / 'timing.lua', Path(__file__),
             ROOT / 'benchmarks/features/timing.c', ROOT / 'benchmarks/features/timing.rs',
             ROOT / 'benchmarks/perf/luabench.c', ROOT / 'benchmarks/perf/build_lua.py',
             ROOT / 'library/lua/manifest.json']
    return {str(path.relative_to(ROOT)).replace('\\', '/'): digest(path, True) for path in sorted(files)}


def execute(command, build=False):
    """构建可保留警告；程序执行必须无 stderr，拒绝失败样本。"""
    result = subprocess.run(list(map(str, command)), cwd=ROOT, capture_output=True,
                            text=True, encoding='utf-8', errors='replace', timeout=1200)
    entry = dict(command=list(map(str, command)), code=result.returncode,
                 stdout=result.stdout, stderr=result.stderr)
    if result.returncode or (result.stderr and not build):
        raise RuntimeError(json.dumps(entry, ensure_ascii=False))
    return entry


def model(case, seed):
    """通过 Python bytes 的切片和查找独立核对相同字节语义，不参与计时。"""
    acc = seed
    if case == 'string_read':
        for index in range(131072):
            at = (index * 13 + ((acc >> 7) & 65535)) % len(TEXT)
            acc = (acc * 33 + TEXT[at]) & MASK
    elif case == 'string_concat':
        result = b''.join(UNIT + (b'a' if (index + seed) & 1 == 0 else b'B') for index in range(2048))
        for value in result:
            acc = (acc * 33 + value) & MASK
    elif case == 'string_slice':
        for index in range(1024):
            block = (index * 13 + (acc & 127)) & 127
            start = block * WIDTH
            width = (((index + seed) & 3) + 1) * WIDTH
            other = start if index & 1 == 0 else ((block + 13) & 127) * WIDTH
            part, compared = TEXT[start:start + width], TEXT[other:other + width]
            part.decode('utf-8'); compared.decode('utf-8')
            acc = (acc * 33 + part[0] + part[-1] + int(part == compared)) & MASK
    elif case == 'string_find':
        needles = [value.encode('utf-8') for value in ('字符串', '🙂', '不存在!', '012345')]
        for index in range(256):
            start = (((acc >> 7) + index) & 255) * WIDTH
            acc = (acc * 33 + TEXT.find(needles[(index + seed) & 3], start) + 1) & MASK
    else:
        raise ValueError(case)
    return acc


def main():
    """预检先核对五个种子，正式参考只建立一次，之后默认仅运行 ChaLi。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--compiler-source-commit', required=True)
    parser.add_argument('--cases', default=','.join(CASES))
    parser.add_argument('--runs', type=int, default=5)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--preflight', action='store_true')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--establish-baseline', action='store_true')
    group.add_argument('--reference', type=Path)
    args = parser.parse_args()
    cases = args.cases.split(',')
    if args.runs < 1 or not cases or len(set(cases)) != len(cases) or any(case not in CASES for case in cases):
        parser.error('runs 必须为正数，cases 必须是互不重复的已有负载')
    if not args.preflight and not args.establish_baseline and not args.reference:
        parser.error('正式首次使用 --establish-baseline，之后使用 --reference')
    if not args.preflight and args.out.exists():
        parser.error('正式结果输出目录必须尚不存在，不能覆盖历史数据')
    scratch_root = ROOT / 'tmp'
    scratch_root.mkdir(exist_ok=True)
    for name in ('TMPDIR', 'TMP', 'TEMP'):
        os.environ[name] = str(scratch_root)
    args.out.mkdir(parents=True, exist_ok=True)
    hashes = source_hashes()
    reference = None
    if args.reference:
        reference = json.loads(args.reference.read_text(encoding='utf-8'))
        if reference['source_sha256'] != hashes or reference.get('mode') != 'baseline':
            raise ValueError('冻结字符串源码／计时壳摘要或参考模式不一致，不能冒用参考')
        for case in cases:
            assert all(key in reference['configurations'][case] for key in CONFIGS)
            if case == 'string_concat':
                assert 'lua-builder' in reference['configurations'][case]
    compiler = args.chali.resolve()
    clang = Path(llvm_compiler()).resolve()
    report = dict(mode='preflight' if args.preflight else 'baseline' if args.establish_baseline else 'chali-only',
                  generated=datetime.now().astimezone().isoformat(), platform=platform.platform(),
                  processor=platform.processor(), runs=args.runs, reps_per_run=5, warmups=1,
                  allocator='malloc', reference_allocators='language defaults',
                  compiler_source_commit=execute(['git', 'rev-parse', args.compiler_source_commit])['stdout'].strip(),
                  compiler_sha256=digest(compiler), compiler_path=str(compiler),
                  compiler_version=compiler_version(compiler),
                  git_commit=execute(['git', 'rev-parse', 'HEAD'])['stdout'].strip(),
                  working_tree_dirty=bool(execute(['git', 'status', '--porcelain'])['stdout'].strip()),
                  source_hash_normalization='CRLF to LF only', source_sha256=hashes,
                  llvm_clang=dict(path=str(clang), sha256=digest(clang), version=execute([clang, '--version'])['stdout']),
                  sizes=dict(unit_bytes=WIDTH, input_bytes=len(TEXT), read_iterations=131072,
                             concat_chunks=2048, concat_bytes=2048 * WIDTH, slice_iterations=1024, find_iterations=256),
                  model_values={case: [model(case, seed) for seed in range(5)] for case in cases},
                  environment={key: value for key, value in os.environ.items() if key.startswith('CHALI_NO_') or key == 'CHALI_BACKEND_PROFILE'},
                  tools={}, builds={}, configurations={case: {} for case in cases})
    if args.reference:
        report['reused_reference'] = dict(path=str(args.reference), sha256=digest(args.reference),
                                          generated=reference['generated'], rerun=False)
    configurations = ('chali-aot',) if reference else (*CONFIGS, 'lua-builder')
    cpp = rust = None
    if not reference:
        cpp, rust = shutil.which('g++'), shutil.which('rustc')
        if not cpp or not rust:
            raise RuntimeError('首次字符串参考需要 g++ 与 rustc')
        execute([sys.executable, ROOT / 'benchmarks/perf/build_lua.py'], build=True)
        for key, binary, flag in [('cpp-o2', cpp, '--version'), ('rust', rust, '--version'), ('lua', LUA_HOST, '--version')]:
            report['tools'][key] = dict(path=str(binary), sha256=digest(binary), version=execute([binary, flag])['stdout'])

    def save():
        (args.out / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

    save()
    with tempfile.TemporaryDirectory(prefix='string-benchmark-') as folder:
        work = Path(folder)
        products = {}
        common = work / 'chali-kernel'
        report['builds']['chali-module'] = [execute([compiler, 'aot', HERE / 'cases.chl', '--shared', '--unchecked',
                                                    '--allocator', 'malloc', '--out', common], build=True)]
        library = Path(str(common) + ('.dll' if os.name == 'nt' else '.so'))
        report['chali_kernel_sha256'] = digest(library)
        for case in cases:
            for config in configurations:
                if config == 'lua-builder' and case != 'string_concat':
                    continue
                print('build', case, config, flush=True)
                prefix = work / f'{case}-{config}'
                product = Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))
                builds = []
                if config == 'chali-aot':
                    command = [clang, '-O3', '-DNDEBUG', '-DCHALI_FEATURE_AOT=1', '-Dfeature_work=' + case,
                               ROOT / 'benchmarks/features/timing.c', library, '-o', product]
                elif config == 'cpp-o2':
                    command = [cpp, '-std=c++17', '-O2', '-DNDEBUG', '-finput-charset=UTF-8', '-Dfeature_work=' + case,
                               HERE / 'cases.cpp', ROOT / 'benchmarks/features/timing.c', '-o', product]
                elif config == 'rust':
                    command = [rust, '-O', '--edition=2021', '--cfg', case, '-A', 'dead_code', HERE / 'cases.rs', '-o', product]
                else:
                    wrappers = []
                    for verify in (False, True):
                        wrapper = Path(str(prefix) + ('-verify.lua' if verify else '.lua'))
                        lua_case = 'string_concat_builder' if config == 'lua-builder' else case
                        script = 'BENCH_CASE=' + json.dumps(lua_case) + '\nVERIFY=' + ('true' if verify else 'false') + '\n'
                        for source in (HERE / 'cases.lua', HERE / 'timing.lua'):
                            script += 'dofile(' + json.dumps(source.as_posix(), ensure_ascii=False) + ')\n'
                        wrapper.write_text(script, encoding='utf-8')
                        wrappers.append([str(LUA_HOST), str(wrapper)])
                    products[case, config] = dict(run=wrappers[0], verify=wrappers[1], binary_sha256=digest(LUA_HOST))
                    report['builds'][case + '/' + config] = builds
                    save()
                    continue
                builds.append(execute(command, build=True))
                products[case, config] = dict(run=[str(product)], verify=[str(product), '--verify'], binary_sha256=digest(product))
                report['builds'][case + '/' + config] = builds
                save()
        save()
        # 所有构建已结束，先核对各种子；正式计时阶段不穿插编译或其他语言复测。
        for case in cases:
            for config in configurations:
                if (case, config) not in products:
                    continue
                product = products[case, config]
                checked = execute(product['verify'])
                values = {int(seed): int(value) for seed, value in re.findall(r'^value (\d+) (-?\d+)$', checked['stdout'], re.M)}
                assert values == dict(enumerate(report['model_values'][case])), checked
                report['configurations'][case][config] = dict(binary_sha256=product['binary_sha256'], preflight=checked, processes=[])
                print('verify', case, config, 'OK', flush=True)
                save()
        if not args.preflight:
            for case in cases:
                expected = report['model_values'][case][0] + sum(report['model_values'][case])
                for config in configurations:
                    if (case, config) not in products:
                        continue
                    samples = []
                    for _ in range(args.runs):
                        measured = execute(products[case, config]['run'])
                        values = [int(value) for value in re.findall(r'^rep (\d+)$', measured['stdout'], re.M)]
                        sink = re.findall(r'^sink (-?\d+)$', measured['stdout'], re.M)
                        assert len(values) == 5 and all(value > 0 for value in values) and sink == [str(expected)], measured
                        measured['samples_ns'] = values
                        report['configurations'][case][config]['processes'].append(measured)
                        samples.extend(values)
                        save()
                    report['configurations'][case][config]['median_ms'] = statistics.median(samples) / 1e6
                    report['configurations'][case][config]['min_ms'] = min(samples) / 1e6
                    report['configurations'][case][config]['max_ms'] = max(samples) / 1e6
                    print('median', case, config, report['configurations'][case][config]['median_ms'], 'ms', flush=True)
                    save()
    if not args.preflight:
        references = reference or report
        report['comparisons'] = {}
        table = ['| 负载 | ChaLi / ms | C++ O2 / ms | Rust / ms | 较快 Lua / ms | Lua 配置 | 当前门槛 |',
                 '| --- | ---: | ---: | ---: | ---: | --- | --- |']
        for case in cases:
            values = references['configurations'][case]
            lua_key = min((key for key in ('lua', 'lua-builder') if key in values),
                          key=lambda key: values[key]['median_ms'])
            chali = report['configurations'][case]['chali-aot']['median_ms']
            cpp, rust_value, lua = (values[key]['median_ms'] for key in ('cpp-o2', 'rust', lua_key))
            passed = chali <= cpp * 1.05 and chali < lua
            report['comparisons'][case] = dict(chali_ms=chali, cpp_o2_ms=cpp, rust_ms=rust_value,
                                               lua_ms=lua, lua_config=lua_key, passed=passed)
            table.append(f'| {case} | {chali:.4f} | {cpp:.4f} | {rust_value:.4f} | {lua:.4f} | {lua_key} | {"通过" if passed else "未通过"} |')
        save()
        (args.out / 'summary.md').write_text('# 默认 String：逐项性能对照\n\n' + '\n'.join(table) +
            '\n\n每格五个样本／进程，实际进程数见 results.json；Lua 拼接取逐次拼接和 table.concat 中更快者。参考语言首次建立后仅复用。最终门槛逐项要求快于 Lua，且不超过 C++ O2 的 1.05 倍。\n', encoding='utf-8')
    print('saved', args.out.resolve(), flush=True)


if __name__ == '__main__':
    main()
