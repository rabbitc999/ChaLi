"""冻结静态编译器与两层库，串行核对并测量原生／bitcode 及 off／full／thin。"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import random
import shutil
import statistics
import subprocess
import sys
import time
from datetime import datetime, timezone

from cpu_budget import timing_budget
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
VARIANTS = [('native-off', 'native', 'off'), ('native-full', 'native', 'full'),
            ('bitcode-off', 'bitcode', 'off'), ('bitcode-full', 'bitcode', 'full'),
            ('bitcode-thin', 'bitcode', 'thin')]
CASES = ['call', 'bulk']


def digest(path):
    """记录真实文件字节，不规范化二进制时间戳或空白。"""
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def reference(seed):
    """独立核对两个负载完成的相同一百零四万步整数依赖链。"""
    value = seed + 1
    for index in range(1048576):
        value = (value * 48271 + index) & 2147483647
    return value


class Deferred(RuntimeError):
    """无法满足 CPU 条件时保留已完成证据并返回待测状态。"""


def main():
    """先验证全部组合，再在计时区间外检查 CPU 并记录原始样本和冻结身份。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, required=True)
    parser.add_argument('--compiler-source-commit', required=True, help='被测编译器实际构建源码提交，不推断为当前 HEAD')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--cc', default=os.environ.get('CHALI_ABI_CC', shutil.which('gcc')))
    parser.add_argument('--build-repetitions', type=int, default=3)
    parser.add_argument('--runtime-processes', type=int, default=5)
    parser.add_argument('--max-cpu-wait', type=float, default=120)
    parser.add_argument('--seed', type=int, default=20261102)
    parser.add_argument('--verify-only', action='store_true', help='只完成冻结与全部组合核对，不计时')
    args = parser.parse_args()
    if args.build_repetitions < 3 or args.runtime_processes < 3 or args.max_cpu_wait < 0:
        parser.error('构建和运行至少各三轮，CPU 等待不能为负数')
    if not args.cc:
        parser.error('需要 C 编译器构建计时壳')
    output = args.out.resolve()
    output.mkdir(parents=True, exist_ok=False)
    frozen = output / 'frozen'
    frozen.mkdir()
    environment = dict(os.environ)
    temp_root = str((ROOT / 'tmp').resolve())
    environment.update(TMPDIR=temp_root, TMP=temp_root, TEMP=temp_root, PYTHONUTF8='1', PYTHONDONTWRITEBYTECODE='1',
                       CHALI_BACKEND_PROFILE='0', CHALI_AOT_FULL_STDLIB='0')
    compiler = frozen / args.chali.name
    shutil.copy2(args.chali.resolve(), compiler)
    compiler_hash = digest(compiler)
    commit = subprocess.check_output(['git', 'rev-parse', args.compiler_source_commit + '^{commit}'], cwd=ROOT, text=True).strip()
    sources = [*sorted((ROOT / 'benchmarks/library_lto').glob('*.*')), ROOT / 'benchmarks/features/timing.c',
               Path(__file__).resolve(), ROOT / 'tools/cpu_budget.py']
    identities = {}
    for source in sources:
        relative = source.relative_to(ROOT)
        copy = frozen / relative
        copy.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, copy)
        identities[relative.as_posix()] = digest(copy)
    report = dict(status='preparing', protocol='library-lto-v1', startedAt=datetime.now(timezone.utc).isoformat(),
                  compiler=dict(path=str(compiler), sha256=compiler_hash, sourceCommit=commit),
                  sourceSha256=identities, python=sys.version, platform=platform.platform(), machine=platform.machine(),
                  seed=args.seed, buildRepetitions=args.build_repetitions, runtimeProcesses=args.runtime_processes,
                  verificationOnly=args.verify_only, cachePolicy='warmed by the complete functional matrix; not cleared between formal samples',
                  environment={name: environment.get(name) for name in ['CHALI_BACKEND_PROFILE', 'CHALI_AOT_FULL_STDLIB', 'CHALI_SKIP_LSP_TIMING']},
                  kernelSamplesPerProcess=5, cpuLimitPercent=40, cpuChecks=[], commands=[], builds=[], runtime=[],
                  limitations=['CPU frequency and affinity are not fixed', 'CPU checks describe command startup only',
                               'compile/link wall times include process startup', 'kernel timing excludes initialization and cleanup',
                               'separate link uses a native consumer object; total AOT includes main-unit LTO',
                               'only these two arithmetic workloads and this static compiler are measured'])
    inputs = {}

    def save():
        """阶段性保存原始结果，失败或 CPU 延后也不丢弃前面的证据。"""
        (output / 'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')

    def execute(command, work, timed=False, label=''):
        """仅在区间外 CPU 核对通过后测量单个子进程；所有子进程继承同一临时根。"""
        assert digest(compiler) == compiler_hash, '冻结编译器字节发生变化'
        for path, expected_hash in inputs.items():
            assert digest(path) == expected_hash, str(path)
        cpu = None
        if timed:
            deadline = time.monotonic() + args.max_cpu_wait
            while True:
                cpu = timing_budget()
                report['cpuChecks'].append(dict(label=label, checkedAt=datetime.now(timezone.utc).isoformat(), **cpu))
                save()
                if cpu['allowed']:
                    break
                if time.monotonic() >= deadline or cpu['reason'] == 'explicitly deferred':
                    raise Deferred(label + ': ' + cpu['reason'])
                time.sleep(min(5, max(0, deadline - time.monotonic())))
        arguments = list(map(str, command))
        started = time.perf_counter_ns()
        result = subprocess.run(arguments, cwd=work, env=environment, capture_output=True,
                                encoding='utf-8', errors='strict', timeout=300)
        elapsed = time.perf_counter_ns() - started
        record = dict(label=label, command=arguments, cwd=str(work), timed=timed, exitCode=result.returncode,
                      stdout=result.stdout, stderr=result.stderr)
        if timed:
            record.update(elapsedNs=elapsed, cpuStartup=cpu)
        report['commands'].append(record)
        save()
        if result.returncode:
            raise RuntimeError(str(arguments) + '\n' + result.stdout + result.stderr)
        return result, record

    try:
        with tempfile.TemporaryDirectory(prefix='library-lto-measure-') as directory:
            work = Path(directory)
            info = json.loads(execute([compiler, '--build-info'], work)[0].stdout)
            if info.get('llvm') != 'static' or info.get('execution') != 'aot':
                raise RuntimeError('本协议要求自包含静态 LLVM 编译器，避免未冻结的外置 LLVM 参与计时')
            report['compiler']['buildInfo'] = info
            cc = Path(shutil.which(args.cc) or args.cc).resolve()
            report['cCompiler'] = dict(path=str(cc), sha256=digest(cc), version=execute([cc, '--version'], work)[0].stdout)
            if os.name == 'nt':
                shell = shutil.which('pwsh') or 'powershell'
                cpu_info = execute([shell, '-NoProfile', '-Command',
                                    'Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors,MaxClockSpeed | ConvertTo-Json -Compress'], work)[0].stdout
                report['processor'] = json.loads(cpu_info)
            else:
                report['processor'] = Path('/proc/cpuinfo').read_text() if Path('/proc/cpuinfo').is_file() else platform.processor()
            manifest = work / 'chali.toml'
            manifest.write_text('[build]\nallocator = "malloc"\nlto = "full"\nlibrary_implementation = "native"\n', encoding='utf-8')
            shutil.copy2(manifest, frozen / 'chali.toml')
            report['projectSha256'] = digest(manifest)
            for name in ['base', 'bridge', *CASES]:
                shutil.copy2(frozen / ('benchmarks/library_lto/' + name + '.chl'), work / (name + '.chl'))
            modules = work / 'modules/bench'
            modules.mkdir(parents=True)
            for name in ['base', 'bridge']:
                execute([compiler, 'lib', 'build', work / (name + '.chl'), '--module', 'bench::' + name,
                         '--package-version', '1.0.0', '--with-bitcode', '--out', modules / (name + '.chlib')], work)
            shutil.copytree(work / 'modules', frozen / 'modules')
            report['packages'] = {name: json.loads(execute([compiler, 'lib', 'inspect', modules / (name + '.chlib')], work)[0].stdout)
                                  for name in ['base', 'bridge']}
            for name in ['base', 'bridge']:
                (work / (name + '.chl')).unlink()
            for path in [manifest, *(work / (name + '.chl') for name in CASES), *modules.glob('*.chlib')]:
                inputs[path] = digest(path)
            report['workingInputs'] = {str(path.relative_to(work)): value for path, value in inputs.items()}
            expected = [reference(seed) for seed in range(5)]
            report['expected'] = expected
            extension = '.dll' if os.name == 'nt' else '.so'
            exe_suffix = '.exe' if os.name == 'nt' else ''
            common = ['--unchecked', '--allocator', 'malloc', '--memory-size', '536870912']
            matrix = {}

            def verify(driver):
                """逐个种子核对独立模型，初始化和预热不计为正式样本。"""
                result = execute([driver, '--verify'], work)[0]
                values = [int(line.split()[2]) for line in result.stdout.splitlines() if line.startswith('value ')]
                if values != expected:
                    raise RuntimeError('运行结果与独立模型不一致：' + result.stdout)

            def driver_for(library, name):
                """使用同一冻结 C11 计时壳链接实际动态库。"""
                driver = library.parent / (name + exe_suffix)
                command = [cc, '-std=c11', '-O3', '-DNDEBUG', '-I' + str(library.parent),
                           frozen / 'benchmarks/library_lto/timing.c', '-o', driver]
                if os.name == 'nt':
                    command.append(library)
                else:
                    command += ['-L' + str(library.parent), '-l:' + library.name,
                                '-D_POSIX_C_SOURCE=200809L', '-Wl,-rpath,$ORIGIN']
                execute(command, work)
                verify(driver)
                return driver

            for case in CASES:
                native = work / (case + '-native')
                object_command = [compiler, 'build', work / (case + '.chl'), '--shared', '--export', 'work', '--emit-obj',
                                  '--lto-mode', 'off', *common, '--out', native]
                execute(object_command, work)
                for name, implementation, mode in VARIANTS:
                    directory = work / case / name
                    directory.mkdir(parents=True)
                    prefix = directory / 'kernel'
                    aot = [compiler, 'build', work / (case + '.chl'), '--shared', '--export', 'work', *common,
                           '--library-implementation', implementation, '--lto-mode', mode, '--out', prefix]
                    execute(aot, work)
                    driver = driver_for(prefix.with_suffix(extension), 'driver')
                    linked = directory / ('linked' + extension)
                    link = [compiler, 'link', native.with_suffix('.o'), '--shared', '--export', 'f_work',
                            '--export', 'chali_init', '--export', 'chali_finish', '--package', modules / 'bridge.chlib',
                            '--library-implementation', implementation, '--lto-mode', mode, '--out', linked]
                    execute(link, work)
                    driver_for(linked, 'linked-driver')
                    runtime_dir = frozen / 'runtime' / case / name
                    runtime_dir.mkdir(parents=True)
                    for path in [prefix.with_suffix(extension), prefix.with_suffix('.h'), driver]:
                        shutil.copy2(path, runtime_dir / path.name)
                    verify(runtime_dir / driver.name)
                    matrix[(case, name)] = dict(case=case, name=name, implementation=implementation, mode=mode,
                                               aot=aot, link=link, object=object_command, library=prefix.with_suffix(extension),
                                               linked=linked, native=native.with_suffix('.o'), runtime=runtime_dir / driver.name)
            report['status'] = 'functional-verified'
            report['runtimeArtifacts'] = {str(path.relative_to(frozen)): dict(sha256=digest(path), bytes=path.stat().st_size)
                                          for path in (frozen / 'runtime').rglob('*') if path.is_file()}
            (output / 'baseline.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
            print('all ten AOT/link combinations verified; compiler, sources, packages and runtime artifacts frozen', flush=True)
            if args.verify_only:
                save()
                return 0
            rng = random.Random(args.seed)
            report['status'] = 'measuring-builds'
            report['measuredAt'] = datetime.now(timezone.utc).isoformat()
            for repetition in range(args.build_repetitions):
                for case in CASES:
                    entry = matrix[(case, VARIANTS[0][0])]
                    _, record = execute(entry['object'], work, True, case + ':native-object:' + str(repetition))
                    report['builds'].append(dict(case=case, variant='shared-native-object', phase='compile', repetition=repetition,
                                                 elapsedNs=record['elapsedNs'], sha256=digest(entry['native']), bytes=entry['native'].stat().st_size))
                order = list(matrix.values())
                rng.shuffle(order)
                for entry in order:
                    for phase, command, product in [('link', entry['link'], entry['linked']), ('aot-total', entry['aot'], entry['library'])]:
                        _, record = execute(command, work, True, entry['case'] + ':' + entry['name'] + ':' + phase + ':' + str(repetition))
                        report['builds'].append(dict(case=entry['case'], variant=entry['name'], phase=phase, repetition=repetition,
                                                     elapsedNs=record['elapsedNs'], sha256=digest(product), bytes=product.stat().st_size))
                save()
            report['status'] = 'measuring-runtime'
            for repetition in range(args.runtime_processes):
                order = list(matrix.values())
                rng.shuffle(order)
                for entry in order:
                    result, record = execute([entry['runtime']], work, True, entry['case'] + ':' + entry['name'] + ':runtime:' + str(repetition))
                    samples = [int(line.split()[1]) for line in result.stdout.splitlines() if line.startswith('rep ')]
                    sinks = [int(line.split()[1]) for line in result.stdout.splitlines() if line.startswith('sink ')]
                    if len(samples) != 5 or min(samples) <= 0 or sinks != [expected[0] + sum(expected)]:
                        raise RuntimeError('正式样本的校验值或时间非法：' + result.stdout)
                    report['runtime'].append(dict(case=entry['case'], variant=entry['name'], repetition=repetition,
                                                  samplesNs=samples, sink=sinks[0], cpuStartup=record['cpuStartup']))
                    save()
            report['summary'] = []
            for case, name in matrix:
                item = dict(case=case, variant=name)
                for phase in ['compile', 'link', 'aot-total']:
                    records = [record for record in report['builds'] if record['case'] == case and record['phase'] == phase
                               and record['variant'] == ('shared-native-object' if phase == 'compile' else name)]
                    item[phase + 'MedianMs'] = statistics.median(record['elapsedNs'] for record in records) / 1e6
                samples = [sample for record in report['runtime'] if record['case'] == case and record['variant'] == name for sample in record['samplesNs']]
                item['kernelMedianMs'] = statistics.median(samples) / 1e6
                entry = matrix[(case, name)]
                frozen_library = entry['runtime'].parent / entry['library'].name
                item['aotBytes'] = frozen_library.stat().st_size
                item['linkBytes'] = entry['linked'].stat().st_size
                report['summary'].append(item)
                artifacts = output / 'build-products' / case / name
                artifacts.mkdir(parents=True)
                for path in [entry['library'], entry['linked'], entry['native']]:
                    shutil.copy2(path, artifacts / path.name)
            assert digest(compiler) == compiler_hash
            for relative, expected_hash in identities.items():
                assert digest(frozen / relative) == expected_hash, relative
            for relative, identity in report['runtimeArtifacts'].items():
                assert digest(frozen / relative) == identity['sha256'], relative
            report['status'] = 'complete'
            save()
            print(json.dumps(report['summary'], ensure_ascii=False, indent=2), flush=True)
            return 0
    except Deferred as error:
        report.update(status='deferred', reason=str(error))
        save()
        print(str(error), file=sys.stderr)
        return 2
    except BaseException as error:
        report.update(status='failed', reason=repr(error))
        save()
        raise


if __name__ == '__main__':
    sys.exit(main())
