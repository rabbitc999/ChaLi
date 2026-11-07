"""拆分接口选择、包装、返回、传参与分派成本；插桩与正式计时使用独立构件。"""
from collections import Counter
from datetime import datetime, timedelta, timezone
from pathlib import Path
import argparse
import hashlib
import json
import os
import platform
import random
import re
import shutil
import statistics
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from project_temp import tempfile

# 三个数值内核分别执行相同的操作，只有接口包装、返回及传递方式不同。
CASES = {
    'select_control': ('workSelectControl', 'selection', '只计算选择索引及校验值'),
    'choose_discard': ('workChooseDiscard', 'selection', '选择并返回接口，随后丢弃，不调用方法'),
    'mixed_concrete': ('workMixedConcrete', 'mixed', '按索引直接调用具体结构体方法'),
    'mixed_bound_direct': ('workMixedBoundDirect', 'mixed', '循环外建立三个接口，循环内选择并直接调用'),
    'mixed_bound_param': ('workMixedBoundParam', 'mixed', '已有三个接口，经 invoke 参数传递后调用'),
    'mixed_local_direct': ('workMixedLocalDirect', 'mixed', '循环内选择具体对象并建立接口，直接调用'),
    'mixed_return_direct': ('workMixedReturnDirect', 'mixed', 'choose 返回接口，局部绑定后直接调用'),
    'mixed_return_param': ('work', 'mixed', '原多态内核：choose 返回、局部绑定、invoke 传参'),
    'mono_concrete': ('workMonoConcrete', 'mono', '始终直接调用 first 的具体方法'),
    'mono_bound_direct': ('workMonoBoundDirect', 'mono', '循环外建立一个接口，循环内直接调用'),
    'mono_bound_param': ('workMonoBoundParam', 'mono', '已有一个接口，经 invoke 参数传递后调用'),
    'mono_local_direct': ('workMonoLocalDirect', 'mono', '每轮从 first 建立接口并直接调用'),
    'mono_return_direct': ('workMonoReturnDirect', 'mono', '每轮 chooseFirst 返回接口并直接调用'),
    'mono_return_param': ('workMonoReturnParam', 'mono', '每轮 chooseFirst 返回接口，经 invoke 调用'),
}


def now():
    """按用户所在的北京时间记录采样时间。"""
    return datetime.now(timezone(timedelta(hours=8))).isoformat()


def digest(path):
    """记录真实构件字节，拒绝把文件名或版本号当作构件身份。"""
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def save(path, value):
    """保留全部有效样本和命令，失败也留下已完成的检查点。"""
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')


def source(iterations):
    """以原多态内核为共同类型环境，生成只改变接口使用方式的对照函数。"""
    original = (ROOT / 'benchmarks/features/interface_poly.chl').read_text(encoding='utf-8')
    original = original.replace('262144', str(iterations))
    common = original.split('#export("feature_work")', 1)[0]
    # 保留原 work 的正文，供同模块和独立模块之间核对优化上下文影响。
    body = original.split('#export("feature_work")', 1)[1]
    combined = common + '#export("mixed_return_param")' + body
    combined += '''
/**
 * @brief 始终借用 first，测量接口返回路径而不混入动态选择。
 * @return first 的只读接口引用。
 */
NamedOperation const& chooseFirst() { return &first; }
'''
    for case, (name, group, description) in CASES.items():
        if case == 'mixed_return_param':
            continue
        before = ''
        if case.startswith('mixed_bound'):
            before = ('NamedOperation const& a = &first; NamedOperation const& b = &second; '
                      'NamedOperation const& c = &third;\n')
        elif case.startswith('mono_bound'):
            before = 'NamedOperation const& a = &first;\n'
        if group == 'selection':
            step = 'acc = (acc + index + 1) & 2147483647;'
            if case == 'choose_discard':
                step = 'NamedOperation const& unused = choose(index);\n' + step
        elif case in ('mixed_return_direct', 'mono_return_direct', 'mono_return_param'):
            choose = 'choose(index)' if group == 'mixed' else 'chooseFirst()'
            call = 'invoke(&view, acc)' if case.endswith('param') else 'view.apply(acc)'
            step = f'NamedOperation const& view = {choose};\nacc = {call};'
        elif group == 'mixed':
            pieces = []
            for index, concrete, bound in ((1, 'second', 'b'), (2, 'third', 'c'), (0, 'first', 'a')):
                if case == 'mixed_concrete':
                    action = f'acc = {concrete}.apply(acc);'
                elif case == 'mixed_local_direct':
                    action = f'NamedOperation const& view = &{concrete}; acc = view.apply(acc);'
                elif case == 'mixed_bound_direct':
                    action = f'acc = {bound}.apply(acc);'
                else:
                    action = f'acc = invoke(&{bound}, acc);'
                prefix = f'if (index == {index})' if index == 1 else f'elif (index == {index})' if index == 2 else 'else'
                pieces.append(prefix + ' { ' + action + ' }')
            step = '\n'.join(pieces)
        else:
            step = {
                'mono_concrete': 'acc = first.apply(acc);',
                'mono_bound_direct': 'acc = a.apply(acc);',
                'mono_bound_param': 'acc = invoke(&a, acc);',
                'mono_local_direct': 'NamedOperation const& view = &first; acc = view.apply(acc);',
            }[case]
        combined += f'''\n/**
 * @brief {description}。
 * @param seed 数值初值及混合类型选择的偏移。
 * @return 完成 {iterations} 轮计算的校验值。
 */
#export("{case}")
int64 {name}(int64 seed) {{
    {before}
    int64 acc = seed; int64 i = 0;
    while (i < {iterations}) {{
        int64 index = (i + seed) & 3;
        {step}
        i = i + 1;
    }}
    return acc;
}}
'''
    return original, combined


def driver(cases):
    """生成统一的 C 计时壳，初始化、预热、打印和清理均不计入工作区间。"""
    entries = ', '.join(cases)
    return f'''#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
typedef int64_t (*Work)(int64_t);
static Work kernels[] = {{ {entries} }};
/**
 * @brief 读取单调纳秒时钟。
 * @return 当前单调时刻，不含进程启动时间。
 */
static int64_t now_ns(void) {{
#ifdef _WIN32
    LARGE_INTEGER f, t; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&t);
    return t.QuadPart/f.QuadPart*1000000000LL+t.QuadPart%f.QuadPart*1000000000LL/f.QuadPart;
#else
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return (int64_t)t.tv_sec*1000000000LL+t.tv_nsec;
#endif
}}
/**
 * @brief 单独校验、执行一次插桩内核，或预热后采集五个输入的耗时。
 * @param argc 命令行参数数量。
 * @param argv 内核下标及可选的 --verify、--single 或 --empty。
 * @return 参数无效时为二，正常完成时为零。
 */
int main(int argc, char **argv) {{
    if (argc < 2) return 2;
    char *end = NULL; long index = strtol(argv[1], &end, 10);
    if (!end || *end || index < 0 || index >= (long)(sizeof(kernels)/sizeof(kernels[0]))) return 2;
    Work volatile kernel = kernels[index];
    chali_init(); feature_setup();
    if (argc > 2 && strcmp(argv[2], "--empty") == 0) {{ chali_finish(); return 0; }}
    if (argc > 2 && strcmp(argv[2], "--single") == 0) {{
        int64_t value = kernel(0); printf("value 0 %lld\\n", (long long)value);
        chali_finish(); return 0;
    }}
    if (argc > 2 && strcmp(argv[2], "--verify") == 0) {{
        for (int64_t seed=0; seed<5; ++seed) printf("value %lld %lld\\n", (long long)seed, (long long)kernel(seed));
        chali_finish(); return 0;
    }}
    volatile int64_t sink = kernel(0);
    for (int64_t seed=0; seed<5; ++seed) {{
        int64_t start=now_ns(); int64_t value=kernel(seed); int64_t elapsed=now_ns()-start;
        sink += value;
        printf("rep %lld %lld %lld\\n", (long long)seed, (long long)elapsed, (long long)value);
    }}
    printf("sink %lld\\n", (long long)sink); chali_finish(); return 0;
}}
'''


def oracle(group, iterations):
    """在独立 Python 算术模型中计算五组期望值，不读取旧性能结果。"""
    values = []
    for seed in range(5):
        acc = seed
        for i in range(iterations):
            index = (i + seed) & 3
            if group == 'selection':
                acc = acc + index + 1
            elif group == 'mono' or index not in (1, 2):
                acc = acc * 3 + 7
            elif index == 1:
                acc = acc * 5 - 11
            else:
                acc = (acc * 7) ^ 13
            acc &= 2147483647
        values.append(acc)
    return values


def optimized_functions(text, exports):
    """保留非插桩 O3 导出函数及静态调用点，辅助识别内联或被消除的路径。"""
    by_name = {name:body for body,name in re.findall(
        r'^(define[^\n]*?@([A-Za-z_][A-Za-z_0-9]*)\([^\n]*\).*?^\})', text, re.M | re.S)}
    result = {}
    for name, function in exports.items():
        symbol = '__chali_impl_program__' + function
        body = by_name.get(symbol)
        if body is None:
            raise ValueError('优化后的 LLVM IR 缺少导出函数：' + name)
        calls = Counter(re.findall(r'\b(?:call|invoke)\b[^\n]*?@([A-Za-z_][A-Za-z_0-9]*)\(', body))
        result[name] = dict(llvm_symbol=symbol, body=body, static_calls=dict(calls))
    return result


def main():
    """先构建及核对，再独立计数，最后串行正式采样并保存全部证据。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--runs', type=int, default=5)
    parser.add_argument('--iterations', type=int, default=262144)
    parser.add_argument('--allocator', choices=('malloc', 'pool'), default='malloc')
    parser.add_argument('--clang', type=Path, default=ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe' if os.name == 'nt' else None)
    parser.add_argument('--cc', type=Path, default=ROOT/'build/dependencies/llvm/static-sdk/bin/gcc.exe' if os.name == 'nt' else None)
    args = parser.parse_args()
    if args.runs < 1 or args.iterations < 1:
        parser.error('runs 和 iterations 必须大于零')
    compiler = args.chali.resolve()
    clang = args.clang.resolve() if args.clang else Path(shutil.which('clang') or 'clang')
    cc = args.cc.resolve() if args.cc else Path(shutil.which('cc') or 'cc')
    out = args.out.resolve()
    if out == ROOT or not out.is_relative_to(ROOT/'build/validation'):
        parser.error('--out 必须位于仓库 build/validation 内')
    if out.exists() and any(out.iterdir()):
        parser.error('输出目录非空，请选择新的测量目录')
    out.mkdir(parents=True, exist_ok=True)
    (out/'generated').mkdir()
    (out/'analysis').mkdir()
    env = dict(os.environ, CHALI_QUIET='1', CHALI_LANG='zh')
    env.pop('CHALI_BACKEND_PROFILE', None)
    env.pop('CHALI_BACKEND_PROFILE_OUTPUT', None)
    # 驱动的 SDK 编译器独立使用其 DLL 搜索路径；不改变 ChaLi 默认的宿主 ABI 编译器选择。
    cc_env = {'PATH':str(cc.parent)+os.pathsep+env.get('PATH','')}
    report = dict(schema=1, started=now(), platform=platform.platform(), logical_cpus=os.cpu_count(),
                  compiler=str(compiler), compiler_sha256=digest(compiler), iterations=args.iterations,
                  runs=args.runs, samples_per_process=5, allocator=args.allocator, commands=[], builds={},
                  cases={key:dict(function=v[0],group=v[1],description=v[2]) for key,v in CASES.items()},
                  profile={}, results={}, schedule=[],
                  policy='串行采样；不剔除慢样本；正式计时不含插桩；差值不是独立可相加的函数成本')
    report['cases']['original_poly'] = dict(function='work', group='mixed', description='原多态源码单独编译，核对模块上下文差异')

    def execute(command, extra=None):
        """捕获实际命令及全部输出，失败时保留已有证据并停止。"""
        command = list(map(str, command))
        result = subprocess.run(command, cwd=ROOT, env=dict(env, **(extra or {})),
                                capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=600)
        row = dict(command=command, code=result.returncode, stdout=result.stdout, stderr=result.stderr)
        report['commands'].append(row)
        save(out/'results.json', report)
        if result.returncode:
            raise RuntimeError(row)
        return result.stdout

    report['build_info'] = json.loads(execute([compiler, '--build-info']))
    report['git_commit'] = execute(['git', 'rev-parse', 'HEAD']).strip()
    report['working_tree'] = execute(['git', 'status', '--short'])
    report['cc_version'] = execute([cc, '--version'], cc_env)
    abi_cxx = env.get('CHALI_ABI_CXX') or shutil.which('g++')
    if abi_cxx:
        report['abi_cxx'] = dict(path=abi_cxx, version=execute([abi_cxx,'--version']))
    report['clang_version'] = execute([clang, '--version'])
    report['benchmark_sha256'] = digest(__file__)
    report['original_source_sha256'] = digest(ROOT/'benchmarks/features/interface_poly.chl')
    report['performance_switches'] = {name:value for name,value in env.items() if name.startswith('CHALI_NO_')}
    if os.name == 'nt':
        report['machine'] = execute(['powershell', '-NoProfile', '-Command',
            'Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors | ConvertTo-Json -Compress']).strip()
    expected = {group:oracle(group, args.iterations) for group in ('selection', 'mixed', 'mono')}
    report['oracle'] = expected
    with tempfile.TemporaryDirectory(prefix='interface-cost-') as temporary:
        work = Path(temporary)
        original, combined = source(args.iterations)
        products = {}
        for label, text, entries, instrumented in (
                ('normal', combined, list(CASES), False), ('profile', combined, list(CASES), True),
                ('original', original, ['feature_work'], False), ('original_profile', original, ['feature_work'], True)):
            print('BUILD', label, flush=True)
            folder = work/label
            folder.mkdir()
            path = folder/'kernel.chl'
            path.write_text(text, encoding='utf-8')
            (out/'generated'/(label+'.chl')).write_text(text,encoding='utf-8')
            prefix = folder/'kernel'
            execute([compiler, 'build', path, '--shared', '--unchecked', '--allocator', args.allocator, '--out', prefix],
                    {'CHALI_BACKEND_PROFILE':'1'} if instrumented else None)
            library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
            c = folder/'driver.c'
            c.write_text(driver(entries), encoding='utf-8')
            (out/'generated'/(label+'.c')).write_text(driver(entries),encoding='utf-8')
            exe = folder/('bench.exe' if os.name == 'nt' else 'bench')
            options = ['--sysroot='+str(cc.parent.parent)] if os.name == 'nt' and (cc.parent.parent/'chali-sdk.json').exists() else []
            execute([cc, *options, '-O3', '-DNDEBUG', *(['-static'] if os.name == 'nt' else []), c, library, '-o', exe], cc_env)
            products[label] = exe
            report['builds'][label] = dict(compiler_sha256=digest(compiler), library_sha256=digest(library),
                                          executable_sha256=digest(exe), instrumented=instrumented)
            if not instrumented:
                optimized = out/'analysis'/(label+'-optimized.ll')
                execute([clang, '-O3', '-S', '-emit-llvm', prefix.with_suffix('.ll'), '-o', optimized])
                llvm = optimized.read_text(encoding='utf-8')
                assert '@chali_backend_profile_event(' not in llvm, '正式计时构件混入了插桩'
                functions = optimized_functions(llvm, {name:CASES[name][0] for name in entries} if label == 'normal' else {'feature_work':'work'})
                for name,data in functions.items():
                    (folder/(name+'.ll')).write_text(data.pop('body')+'\n',encoding='utf-8')
                report['builds'][label]['optimized_exports'] = functions
        for index, case in enumerate(CASES):
            output = execute([products['normal'], index, '--verify'])
            values = [int(value) for _,value in re.findall(r'^value (\d+) (-?\d+)$', output, re.M)]
            assert values == expected[CASES[case][1]], (case,values,expected)
        original_values = execute([products['original'], 0, '--verify'])
        assert [int(v) for _,v in re.findall(r'^value (\d+) (-?\d+)$', original_values, re.M)] == expected['mixed']
        print('VERIFY all 15 cases x 5 inputs OK', flush=True)

        def counts(path):
            """合计插桩 AST 操作点；这些计数不是物理分配次数或存活字节。"""
            data = json.loads(path.read_text(encoding='utf-8'))
            total = Counter()
            for event in data['events']:
                total[event['site'].split('\t')[0]] += event['count']
            return total

        for label in ('profile','original_profile'):
            path = work/label/'empty.json'
            execute([products[label], 0, '--empty'], {'CHALI_BACKEND_PROFILE_OUTPUT':str(path)})
            empty = counts(path)
            cases = list(CASES) if label == 'profile' else ['original_poly']
            for index,case in enumerate(cases):
                path = work/label/(case+'.json')
                output = execute([products[label], index, '--single'], {'CHALI_BACKEND_PROFILE_OUTPUT':str(path)})
                assert output.strip() == 'value 0 '+str(expected[report['cases'][case]['group']][0]), (case,output)
                total = counts(path)
                assert all(total[k] >= v for k,v in empty.items()), case
                delta = {key:total[key]-empty[key] for key in total if total[key] != empty[key]}
                report['profile'][case] = dict(ast_events=delta, events_per_iteration={k:v/args.iterations for k,v in delta.items()})
                print('PROFILE', case, flush=True)
        assert digest(compiler) == report['compiler_sha256'], '编译器在构建期间被替换'
        print('BUILD AND PROFILE COMPLETE; SERIAL TIMING', flush=True)
        rng = random.Random(20261107)
        for round_index in range(args.runs):
            cases = list(report['cases'])
            rng.shuffle(cases)
            for case in cases:
                label = 'original' if case == 'original_poly' else 'normal'
                index = 0 if case == 'original_poly' else list(CASES).index(case)
                exe = products[label]
                assert digest(exe) == report['builds'][label]['executable_sha256']
                output = execute([exe, index])
                assert not report['commands'][-1]['stderr'], (case,report['commands'][-1])
                samples = [(int(s),int(t),int(v)) for s,t,v in re.findall(r'^rep (\d+) (\d+) (-?\d+)$', output, re.M)]
                values = expected[report['cases'][case]['group']]
                assert len(samples) == 5 and [s for s,_,_ in samples] == list(range(5)), (case,output)
                assert all(t>0 and v==values[s] for s,t,v in samples), (case,output)
                assert re.search(r'^sink '+str(values[0]+sum(values))+r'$', output,re.M), (case,output)
                report['results'].setdefault(case,[]).append(dict(round=round_index, timestamp=now(), samples=samples))
                report['schedule'].append([round_index,case])
                save(out/'results.json',report)
            print('MEASURE round', round_index+1, '/', args.runs, flush=True)
        report['summary'] = {}
        for case,rows in report['results'].items():
            times = [t for row in rows for _,t,_ in row['samples']]
            quartiles = statistics.quantiles(times,n=4)
            report['summary'][case] = dict(samples=len(times), median_ms=statistics.median(times)/1e6,
                min_ms=min(times)/1e6, max_ms=max(times)/1e6, q1_ms=quartiles[0]/1e6, q3_ms=quartiles[2]/1e6,
                median_ns_per_iteration=statistics.median(times)/args.iterations,
                process_medians_ms=[statistics.median(t for _,t,_ in row['samples'])/1e6 for row in rows])
        shutil.copytree(work,out/'artifacts')
        report['artifact_sha256'] = {p.relative_to(out).as_posix():digest(p) for p in (out/'artifacts').rglob('*') if p.is_file()}
        report['completed'] = now()
        save(out/'results.json',report)
        for case,data in report['summary'].items():
            print(case, 'median_ms=',round(data['median_ms'],4), 'ns/iteration=',round(data['median_ns_per_iteration'],2),flush=True)
    print('Evidence:',out,flush=True)


if __name__ == '__main__':
    main()
