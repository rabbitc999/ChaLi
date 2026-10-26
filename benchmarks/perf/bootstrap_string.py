"""原生 String 与 ChaLi 原始存储字符串的可复现 VM/AOT 对照。"""
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

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from project_temp import tempfile
from compiler_path import compiler_path
from release import compiler_version
import container_cases


def digest(path):
    """读取实际文件的 SHA256。"""
    return hashlib.sha256(path.read_bytes()).hexdigest()


def execute(command, log):
    """保留输出，拒绝把失败执行作为性能样本。"""
    result = subprocess.run(list(map(str, command)), cwd=ROOT, capture_output=True,
                            text=True, encoding='utf-8', timeout=1200)
    log.write_text(result.stdout + result.stderr, encoding='utf-8')
    if result.returncode or result.stderr:
        raise RuntimeError(f'{command}: {result.returncode}\n{result.stdout}{result.stderr}')
    return result.stdout


def body(case, bootstrap, size):
    """生成等长 ASCII 输入；转换单列，其他自举操作不调用原生字符串算法。"""
    native_setup = f'String text = "a".repeat({size});'
    raw_setup = f'BootstrapString text; text.reserve({size}); int64 p = 0; while (p < {size}) {{ text.pushByte((uint8)97); p = p + 1; }}'
    setup = raw_setup if bootstrap else native_setup
    loops = size * 4
    if case == 'construct':
        if bootstrap:
            return f'BootstrapString text; text.fill((uint8)97, {size}); return text.byteLength() + seed;'
        return setup + ' return text.byteLength() + seed;'
    if case == 'append':
        if bootstrap:
            return f'BootstrapString text; int64 i = 0; while (i < {size}) {{ text.pushByte((uint8)97); i = i + 1; }} return text.byteLength() + seed;'
        return f'String text = ""; int64 i = 0; while (i < {size}) {{ text = text + "a"; i = i + 1; }} return text.byteLength() + seed;'
    if case == 'read':
        return setup + f' int64 sum = seed; int64 i = 0; while (i < {loops}) {{ int64 value = (int64)text.byteAt(i % {size}).unwrap(); sum = sum + value; i = i + 1; }} return sum;'
    if case == 'find':
        needle = 'BootstrapString needle; needle.pushByte((uint8)98);' if bootstrap else 'String needle = "b";'
        arg = '&needle' if bootstrap else 'needle'
        return setup + needle + f' int64 sum = seed; int64 i = 0; while (i < 32) {{ int64 position = text.indexOf({arg}); sum = sum + position; i = i + 1; }} return sum;'
    if case == 'copy':
        clone = 'BootstrapString duplicate = text.cloneBytes();' if bootstrap else 'String duplicate = text;'
        return setup + f' int64 sum = seed; int64 i = 0; while (i < 32) {{ {clone} int64 width = duplicate.byteLength(); int64 last = (int64)duplicate.byteAt({size - 1}).unwrap(); sum = sum + width + last; i = i + 1; }} return sum;'
    if case == 'from_native':
        convert = 'BootstrapString text; text.appendNative(input);' if bootstrap else 'String text = input;'
        return f'String input = "a".repeat({size}); {convert} return text.byteLength() + seed;'
    raise ValueError(case)


def main():
    """先完成全部构建与正确性，再交错计时并持续保存原始样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--baseline', type=Path, help='用真实旧编译器执行同一默认 String 源码，单列历史原生实现')
    parser.add_argument('--size', type=int, default=1024)
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--aot-only', action='store_true')
    parser.add_argument('--vm-only', action='store_true')
    args = parser.parse_args()
    if args.size < 1 or args.runs < 1 or (args.aot_only and args.vm_only):
        parser.error('size/runs 必须为正数，后端选项不能同时使用')
    compiler = args.chali.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    prototype = ROOT / 'benchmarks/perf/bootstrap_string.chl'
    check = ROOT / 'benchmarks/perf/bootstrap_string_check.chl'
    work = Path(tempfile.mkdtemp(prefix='bootstrap-string-'))
    report = dict(version=compiler_version(compiler), compiler_sha256=digest(compiler),
                  prototype_sha256=digest(prototype), check_sha256=digest(check),
                  generated_utc=datetime.now(timezone.utc).isoformat(), host=platform.platform(),
                  processor=platform.processor(), cpu_count=os.cpu_count(), size=args.size,
                  process_runs=args.runs, warmups=1, repetitions=5, unit='nanoseconds',
                  work=str(work), correctness={}, cases={}, execution_order=[],
                  implementation_names={'native': 'current default self-hosted String',
                                        'bootstrap': 'experimental raw-byte string',
                                        'baseline': 'original default String in baseline compiler'},
                  limitations=[
                      'experimental byte string does not implement the complete default Unicode String API',
                      'ASCII performance inputs; UTF-8 and raw zero/0xFF bytes checked separately',
                      'setup, allocations, copying and cleanup inside work are timed; compilation excluded',
                      'construct: native repeat versus bulk memset; append: native concatenation versus doubling buffer',
                      'find: native search versus ChaLi naive byte search; missing one-byte needle, 32 searches',
                      'copy: 32 independent copies, length and last byte consumed',
                      'from_native includes source creation and bytes-list conversion; native baseline is independent assignment',
                      'no conversion back to native String is measured',
                      'all default optimizations, reflection and runtime safety checks retained'])
    if baseline:
        report['baseline_compiler'] = dict(path=str(baseline), version=compiler_version(baseline), sha256=digest(baseline))
    args.out.parent.mkdir(parents=True, exist_ok=True)

    def save():
        """在失败或中断时保留已取得的证据。"""
        args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

    backends = ['aot'] if args.aot_only else ['vm'] if args.vm_only else ['vm', 'aot']
    check_source = work / 'check.chl'
    check_source.write_text(prototype.read_text(encoding='utf-8') + '\n' + check.read_text(encoding='utf-8'), encoding='utf-8')
    commands = {}
    for backend in backends:
        target = work / ('check-' + backend)
        output = execute([compiler, 'aot' if backend == 'aot' else 'run', check_source, '--out', target], work / ('check-' + backend + '-build.log'))
        if backend == 'aot':
            output = execute([str(target) + '.exe'], work / 'check-aot-run.log')
        if output != 'bootstrap String OK\n':
            raise AssertionError(output)
        report['correctness'][backend] = 'passed'
    for case in ['construct', 'append', 'read', 'find', 'copy', 'from_native']:
        report['cases'][case] = {}
        for implementation in (['native', 'bootstrap', 'baseline'] if baseline else ['native', 'bootstrap']):
            source = work / f'{case}-{implementation}.chl'
            prefix = prototype.read_text(encoding='utf-8') if implementation == 'bootstrap' else ''
            harness = container_cases.source('chali', body(case, implementation == 'bootstrap', args.size))
            source.write_text('import std::clock;\n' + prefix + '\n' + harness.replace('import std::clock;\n', '', 1), encoding='utf-8')
            for backend in backends:
                key = implementation + '-' + backend
                selected_compiler = baseline if implementation == 'baseline' else compiler
                target = work / f'{case}-{key}'
                if backend == 'aot':
                    execute([selected_compiler, 'aot', source, '--out', target], work / f'{case}-{key}-build.log')
                    command = [str(target) + '.exe']
                else:
                    command = [selected_compiler, 'run', source, '--out', target]
                commands[case, key] = command
                report['cases'][case][key] = dict(source=str(source), source_sha256=digest(source),
                                                 samples_ns=[], outputs=[], executable_bytes=Path(command[0]).stat().st_size if backend == 'aot' else None)
        save()
    for run in range(args.runs):
        for case, variants in report['cases'].items():
            keys = list(variants)
            if run % 2:
                keys.reverse()
            for key in keys:
                if digest(compiler) != report['compiler_sha256']:
                    raise RuntimeError('compiler changed during benchmark; discard mixed-version measurements')
                if baseline and digest(baseline) != report['baseline_compiler']['sha256']:
                    raise RuntimeError('baseline compiler changed during benchmark')
                output = execute(commands[case, key], work / f'{case}-{key}-run{run}.log')
                lines = output.splitlines()
                times = [int(line.split()[1]) for line in lines if line.startswith('rep ')]
                base = {'construct': args.size, 'append': args.size, 'read': args.size * 4 * 97,
                        'find': -32, 'copy': 32 * (args.size + 97), 'from_native': args.size}[case]
                if len(times) != 5 or lines[-1] != f'sink {base * 6 + 10}':
                    raise AssertionError((case, key, output))
                variants[key]['samples_ns'].extend(times)
                variants[key]['outputs'].append(output)
                variants[key]['median_ns'] = statistics.median(variants[key]['samples_ns'])
                report['execution_order'].append([run, case, key])
                save()
                print(case, key, variants[key]['median_ns'], flush=True)
    print('String comparison passed:', args.out, flush=True)


if __name__ == '__main__':
    main()
