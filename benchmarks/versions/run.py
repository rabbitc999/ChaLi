"""用相同冻结负载比较真实 1.0.10 与 1.0.11，只运行 ChaLi。

先 --build-only 完成构建和结果核对，再 --measure-only 采样；Python 不生成 LLVM IR。
"""
import argparse
from datetime import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import statistics
import subprocess
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'benchmarks/perf'))
sys.path.insert(0, str(ROOT / 'tools'))
from build_aot import llvm_compiler
from project_temp import tempfile

BASIC = ('int_chain', 'float_chain', 'call', 'array', 'list_growth', 'map_growth', 'set_growth')
FEATURES = ('enum_payload', 'interface_mono', 'interface_poly', 'struct_nested', 'array_2d', 'array_3d')
STRINGS = ('string_read', 'string_concat', 'string_slice', 'string_find')
CONFIGS = ('1.0.10-pool', '1.0.11-pool', '1.0.11-malloc')


def digest(path):
    """对实际文件字节取摘要。"""
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def execute(command, cwd=ROOT, env=None):
    """执行构建或验证，调用者保留真实命令和输出。"""
    return subprocess.run(list(map(str, command)), cwd=cwd, env=env, capture_output=True,
                          text=True, encoding='utf-8', errors='replace', timeout=1800)


def cpu_sample():
    """读取计时外的 CPU 启动检查；当前版本对照在 Windows 实测。"""
    result = execute(['powershell', '-NoProfile', '-Command',
                      '(Get-CimInstance Win32_Processor | Measure-Object LoadPercentage -Average).Average'])
    if result.returncode:
        raise RuntimeError(result.stderr)
    return dict(time=datetime.now().astimezone().isoformat(), cpu_percent=float(result.stdout.strip()))


def main():
    """构建三个真实版本配置，核对冻结结果并交错采集五进程各五样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--old', type=Path, default=ROOT / 'dist/llvm-external/chali-1.0.10-llvm-external-windows-x64.exe')
    parser.add_argument('--new', type=Path, default=ROOT / 'chali-1.0.11-llvm-external-windows-x64.exe')
    parser.add_argument('--new-source', help='构建测量时所用编译器的实际源码提交')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--cases', default=','.join((*BASIC, *FEATURES, *STRINGS)))
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--build-only', action='store_true')
    mode.add_argument('--measure-only', action='store_true')
    args = parser.parse_args()
    if os.name != 'nt':
        parser.error('当前版本对照驱动仅核对 Windows x64；其他平台请勿复用本机结果')
    if args.build_only and not args.new_source:
        parser.error('--build-only 必须用 --new-source 指明实际编译器源码提交')
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    report_path = out / 'results.json'
    if args.build_only:
        if report_path.exists():
            parser.error('输出报告已存在，请使用新的目录，禁止覆盖最终测量')
        cases = args.cases.split(',')
        assert len(set(cases)) == len(cases) and set(cases) <= set((*BASIC, *FEATURES, *STRINGS))
        sources = [Path(__file__), ROOT / 'benchmarks/perf/build_aot.py', ROOT / 'benchmarks/features/timing.c',
                   ROOT / 'benchmarks/strings/cases.chl']
        sources += [ROOT / 'benchmarks/perf' / (case + ext) for case in cases if case in BASIC for ext in ('.chl', '.c')]
        sources += [ROOT / 'benchmarks/features' / (case + '.chl') for case in cases if case in FEATURES]
        refs = {key: ROOT / 'benchmarks/results/1.0.11-final/references' / (key + '.json')
                for key in ('basic', 'features', 'strings')}
        reference = {key: json.loads(path.read_text(encoding='utf-8')) for key, path in refs.items()}
        expected = {case: reference['basic']['sinks'][case]['lua'] for case in BASIC}
        values = {**reference['features']['oracle_values'], **reference['strings']['model_values']}
        expected.update({case: str(row[0] + sum(row)) for case, row in values.items()})
        clang = Path(llvm_compiler()).resolve()
        report = dict(generated=datetime.now().astimezone().isoformat(), platform=platform.platform(),
                      processor=platform.processor(), runs=5, reps_per_run=5, warmups=1,
                      benchmark_commit=execute(['git', 'rev-parse', 'HEAD']).stdout.strip(),
                      working_tree_dirty=bool(execute(['git', 'status', '--porcelain']).stdout.strip()),
                      source_commit=execute(['git', 'rev-parse', args.new_source]).stdout.strip(),
                      workloads_sha256={str(p.relative_to(ROOT)).replace('\\', '/'): digest(p) for p in sources},
                      references={key: dict(path=str(path), sha256=digest(path), rerun=False) for key, path in refs.items()},
                      llvm=dict(path=str(clang), sha256=digest(clang), version=execute([clang, '--version']).stdout),
                      compilers={}, cases=cases, expected_sinks=expected, oracle_values=values,
                      environment={key: value for key, value in os.environ.items() if key.startswith('CHALI_NO_')},
                      configurations={case: {} for case in cases})
        if any(report['environment'].values()):
            raise RuntimeError('正式对照不允许遗留 CHALI_NO_* 开关')
        work = Path(tempfile.mkdtemp(prefix='version-comparison-'))
        # 1.0.10 的内嵌 ABI 头以 ast.hpp 短路径包含 AST 头；补充同版本头搜索路径，
        # 不替换发行编译器、运行时、标准库或任何生产 LLVM 生成代码。
        archive = work / 'legacy-headers.zip'
        archived = execute(['git', 'archive', '--format=zip', '--output', archive, '001cdd6', 'source/ast'])
        assert archived.returncode == 0, archived.stderr
        legacy = work / 'legacy-headers'
        with zipfile.ZipFile(archive) as packed:
            packed.extractall(legacy)
        include = str(legacy / 'source/ast')
        report['legacy_abi_headers'] = dict(source_commit='001cdd6', include_path=include,
            sha256={str(p.relative_to(legacy)).replace('\\', '/'): digest(p) for p in legacy.rglob('*.hpp')})
        for config in CONFIGS:
            compiler = (args.old if config.startswith('1.0.10') else args.new).resolve()
            report['compilers'][config] = dict(path=str(compiler), sha256=digest(compiler),
                source_commit='001cdd6' if config.startswith('1.0.10') else report['source_commit'],
                allocator='malloc' if config.endswith('malloc') else 'pool',
                allocator_selection='1.0.10 release default' if config.startswith('1.0.10') else 'explicit CLI')
        report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')

        for config in CONFIGS:
            compiler = Path(report['compilers'][config]['path'])
            allocator_flags = [] if config.startswith('1.0.10') else ['--allocator', report['compilers'][config]['allocator']]
            common_string = None
            for case in cases:
                folder = work / config / case
                folder.mkdir(parents=True)
                prefix = folder / 'kernel'
                row = dict(status='building', builds=[], processes=[])
                report['configurations'][case][config] = row

                def build(command):
                    """保留准确构建命令，不把不支持的旧版用例伪装为样本。"""
                    env = None
                    if config.startswith('1.0.10'):
                        env = dict(os.environ)
                        env['CPLUS_INCLUDE_PATH'] = include + (os.pathsep + env['CPLUS_INCLUDE_PATH'] if env.get('CPLUS_INCLUDE_PATH') else '')
                    result = execute(command, folder, env)
                    row['builds'].append(dict(command=list(map(str, command)), code=result.returncode,
                                              stdout=result.stdout, stderr=result.stderr))
                    if result.returncode:
                        raise RuntimeError(result.stdout + result.stderr)

                try:
                    if case in BASIC:
                        source = (ROOT / 'benchmarks/perf' / (case + '.chl')).read_text(encoding='utf-8')
                        source = source.split('void main()', 1)[0].replace('import std::clock;', '')
                        source = re.sub(r'const int64 REPS = 5;', '', source)
                        source = re.sub(r'(?m)^(int64|float64) work\(', r'#export("f_work") \1 work(', source)
                        path = folder / 'work.chl'
                        path.write_text(source, encoding='utf-8')
                    else:
                        path = ROOT / ('benchmarks/features/' + case + '.chl' if case in FEATURES else 'benchmarks/strings/cases.chl')
                    if case not in STRINGS or common_string is None:
                        build([compiler, 'aot', path, '--shared', '--unchecked', '--llvm', clang, *allocator_flags, '--out', prefix])
                        if case in STRINGS:
                            common_string = prefix.with_suffix('.dll')
                    library = common_string if case in STRINGS else prefix.with_suffix('.dll')
                    product = folder / 'benchmark.exe'
                    if case in BASIC:
                        harness = (ROOT / 'benchmarks/perf' / (case + '.c')).read_text(encoding='utf-8')
                        harness = '#include <stdint.h>\n#include <stdio.h>\n#include <time.h>\n#include "kernel.h"\n#define REPS 5\nstatic long long now_ns' + harness.split('static long long now_ns', 1)[1]
                        harness = re.sub(r'\bwork\b', 'f_work', harness)
                        harness = re.sub(r'int main\(void\)\s*\{', 'int main(void) {\n    chali_init();', harness)
                        harness = harness.replace('    return 0;', '    chali_finish();\n    return 0;')
                        harness = harness.replace('#include <time.h>', '#include <windows.h>')
                        clock = '''static long long now_ns(void) {
    LARGE_INTEGER frequency, ticks;
    QueryPerformanceFrequency(&frequency); QueryPerformanceCounter(&ticks);
    return ticks.QuadPart / frequency.QuadPart * 1000000000LL
           + ticks.QuadPart % frequency.QuadPart * 1000000000LL / frequency.QuadPart;
}'''
                        harness = re.sub(r'static long long now_ns\(void\)\s*\{.*?\n\}', clock, harness, count=1, flags=re.S)
                        driver = folder / 'driver.c'
                        driver.write_text(harness, encoding='utf-8')
                        build([clang, '-O3', '-DNDEBUG', driver, library, '-lm', '-o', product])
                    else:
                        flags = ['-Dfeature_work=' + case] if case in STRINGS else []
                        build([clang, '-O3', '-DNDEBUG', '-DCHALI_FEATURE_AOT=1', *flags,
                               ROOT / 'benchmarks/features/timing.c', library, '-o', product])
                    # 字符串四项共用同一个 DLL，Windows 加载器仍需在各计时程序旁找到它。
                    if library.parent != folder:
                        import shutil
                        shutil.copy2(library, folder / library.name)
                    verified = execute([product, '--verify'], folder)
                    if verified.returncode or verified.stderr:
                        raise RuntimeError(verified.stdout + verified.stderr)
                    if case in BASIC:
                        sink = re.search(r'^sink (\S+)$', verified.stdout, re.M)
                        assert sink and sink[1] == expected[case], (case, config, verified.stdout)
                        row['verification'] = dict(sink=sink[1])
                    else:
                        got = re.findall(r'^value (\d+) (-?\d+)$', verified.stdout, re.M)
                        assert got == [(str(i), str(value)) for i, value in enumerate(values[case])], (case, config, verified.stdout)
                        row['verification'] = dict(values=got)
                    row.update(status='ready', product=str(product), kernel_sha256=digest(library), product_sha256=digest(product))
                    print('ready', case, config, flush=True)
                except (RuntimeError, AssertionError) as error:
                    row.update(status='unavailable', error=str(error))
                    print('unavailable', case, config, str(error)[:180], flush=True)
                report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        return

    report = json.loads(report_path.read_text(encoding='utf-8'))
    if report.get('completed') and not any(cell.get('status') == 'ready'
            for rows in report['configurations'].values() for cell in rows.values()):
        print('正式测量已完成，保持原报告，不重复计时。', flush=True)
        return
    for config, compiler in report['compilers'].items():
        assert digest(compiler['path']) == compiler['sha256'], ('编译器发生变化', config)
    for case in report['cases']:
        rows = report['configurations'][case]
        pending = [config for config in CONFIGS if rows[config]['status'] == 'ready']
        if not pending:
            continue
        check = cpu_sample()
        report.setdefault('cpu_preflight', []).append(dict(case=case, **check))
        report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        if check['cpu_percent'] >= 40:
            print('CPU >=40%，暂缓正式计时；可再次 --measure-only 继续。', flush=True)
            return 3
        for turn in range(5):
            # 固定轮换顺序，避免始终先测某一版本；每个配置仍为五个独立进程。
            order = pending[turn % len(pending):] + pending[:turn % len(pending)]
            for config in order:
                row = rows[config]
                assert digest(row['product']) == row['product_sha256']
                result = execute([row['product']], Path(row['product']).parent)
                samples = list(map(int, re.findall(r'^rep (\d+)$', result.stdout, re.M)))
                sink = re.search(r'^sink (\S+)$', result.stdout, re.M)
                assert result.returncode == 0 and not result.stderr and len(samples) == 5 and min(samples) > 0, (case, config, result)
                assert sink and sink[1] == report['expected_sinks'][case], (case, config, result.stdout)
                row['processes'].append(dict(command=[row['product']], samples_ns=samples,
                                              stdout=result.stdout, stderr=result.stderr))
        for config in pending:
            row = rows[config]
            samples = [value for process in row['processes'] for value in process['samples_ns']]
            assert len(samples) == 25
            row.update(status='measured', median_ms=statistics.median(samples) / 1e6,
                       min_ms=min(samples) / 1e6, max_ms=max(samples) / 1e6)
            print(case, config, row['median_ms'], 'ms', flush=True)
        report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    report['completed'] = datetime.now().astimezone().isoformat()
    report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')


if __name__ == '__main__':
    sys.exit(main())
