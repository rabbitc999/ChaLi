"""检查本次样本完整性并生成中英文性能文档；不读取旧报告。"""
from pathlib import Path
import argparse
import json
import math
import re
import statistics

CASES=('int_chain','float_chain','call','array','array_rw','list_growth','map_growth','set_growth','enum_payload','interface_mono','interface_poly','struct_nested','array_2d','array_3d','string_read','string_concat','string_slice','string_find')
LANGUAGES=('chali','c','cpp','rust','java','lua','python','go')
LABELS=('ChaLi','C','C++','Rust','Java','Lua','Python','Go')


def validate(data):
    """检查全部语言、项目、原始样本、逐种子结果和校验值，不比较速度优劣。"""
    if data['cases']!=list(CASES) or data['languages']!=list(LANGUAGES):raise ValueError('Incomplete workload/language coverage')
    if data.get('runs')!=5 or data.get('reps_per_process')!=5 or not data.get('completed'):raise ValueError('Incomplete measurements')
    expected_keys={c+'/'+l for c in CASES for l in LANGUAGES}
    if set(data['results'])!=expected_keys or set(data['summary'])!=expected_keys:raise ValueError('Missing combinations')
    for key in expected_keys:
        rows=data['results'][key];case=key.split('/')[0]
        values=data['oracle_values'][case]
        if len(values)!=5 or not all(type(v) in (int,float) and math.isfinite(v) for v in values) or len(rows)!=5 or {r['round'] for r in rows}!=set(range(5)):raise ValueError(('Missing process or invalid oracle',key))
        checks=data['builds'][key]['values']
        if len(checks)!=5:raise ValueError(('Missing seed verification',key))
        if any(not math.isclose(x,y,rel_tol=1e-12,abs_tol=1e-12) if case=='float_chain' else x!=y for x,y in zip(checks,values)):raise ValueError(('Wrong seed result',key))
        samples=[]
        sink=values[0]
        for v in values:sink=sink*0.5+v if case=='float_chain' else sink+v
        for row in rows:
            if row['code']!=0 or row['stderr'] or len(row['samples_ns'])!=5:raise ValueError(('Invalid process',key))
            if any(type(v)!=int or v<=0 for v in row['samples_ns']):raise ValueError(('Invalid samples',key))
            if [int(x) for x in re.findall(r'^rep (\d+)$',row['stdout'],re.M)]!=row['samples_ns']:raise ValueError(('Raw output mismatch',key))
            if re.findall(r'^sink (\S+)$',row['stdout'],re.M)!=[row['sink']]:raise ValueError(('Raw checksum mismatch',key))
            if (not math.isclose(float(row['sink']),sink,rel_tol=1e-12,abs_tol=1e-12)) if case=='float_chain' else int(row['sink'])!=sink:raise ValueError(('Wrong checksum',key))
            samples.extend(row['samples_ns'])
        summary=data['summary'][key]
        if summary['samples']!=25 or summary['median_ms']!=statistics.median(samples)/1e6 or summary['min_ms']!=min(samples)/1e6 or summary['max_ms']!=max(samples)/1e6:raise ValueError(('Summary mismatch',key))
    return len(expected_keys)*25


def table(data,cases,field='median_ms'):
    """按固定语言顺序展示，避免按单项快慢改变列的顺序。"""
    lines=['| Workload | '+' | '.join(LABELS)+' |','| --- | '+' | '.join(['---:']*8)+' |']
    for case in cases:
        cells=[]
        for lang in LANGUAGES:
            s=data['summary'][case+'/'+lang]
            cells.append(f"{s[field]:.4f}" if field!='range' else f"{s['min_ms']:.4f}–{s['max_ms']:.4f}")
        lines.append('| `'+case+'` | '+' | '.join(cells)+' |')
    return '\n'.join(lines)


def descriptive(data):
    """从原始纳秒样本计算样本方差、标准差、CV 和校正 Fisher–Pearson 偏度。"""
    validate(data)
    results={}
    for key,rows in data['results'].items():
        samples=[value/1e6 for row in rows for value in row['samples_ns']]
        n=len(samples);mean=statistics.mean(samples);variance=statistics.variance(samples);sd=math.sqrt(variance)
        skewness=n/((n-1)*(n-2))*math.fsum(((x-mean)/sd)**3 for x in samples) if sd else None
        ordered=sorted(samples)
        def quantile(p):
            """按 R7 在排序位置 (n-1)p 做线性插值。"""
            position=(n-1)*p;lower=math.floor(position);upper=math.ceil(position);fraction=position-lower
            return ordered[lower]*(1-fraction)+ordered[upper]*fraction
        results[key]=dict(samples=n,min_ms=min(samples),median_ms=statistics.median(samples),max_ms=max(samples),
                          mean_ms=mean,sample_variance_ms2=variance,sample_sd_ms=sd,cv_percent=sd/mean*100,
                          adjusted_skewness=skewness,p25_ms=quantile(.25),p50_ms=quantile(.5),p75_ms=quantile(.75),p100_ms=quantile(1))
    return results


def descriptive_table(data,shape=False,english=False):
    """用固定语言顺序展示完整统计，不根据快慢选择行。"""
    stats=descriptive(data)
    if shape:
        headers=['Workload','Language','Sample variance (ms²)','SD (ms)','CV (%)','Skewness G1'] if english else ['项目','语言','样本方差（ms²）','标准差（ms）','变异系数（%）','偏度 G1']
    else:
        headers=['Workload','Language','Minimum','Mean','P25','Median / P50','P75','Maximum / P100'] if english else ['项目','语言','最小值','平均值','P25','中位数／P50','P75','最大值／P100']
    lines=['| '+' | '.join(headers)+' |','| --- | --- | '+' | '.join(['---:']*(len(headers)-2))+' |']
    for case in CASES:
        for lang,label in zip(LANGUAGES,LABELS):
            row=stats[case+'/'+lang]
            values=[row['sample_variance_ms2'],row['sample_sd_ms'],row['cv_percent'],row['adjusted_skewness']] if shape else [row['min_ms'],row['mean_ms'],row['p25_ms'],row['p50_ms'],row['p75_ms'],row['p100_ms']]
            cells=[('undefined' if english else '未定义') if value is None else format(value,'.6g') if shape else format(value,'.4f') for value in values]
            lines.append('| `'+case+'` | '+label+' | '+' | '.join(cells)+' |')
    return '\n'.join(lines)


def render(data,english=False):
    """只写本次身份、实测数据、方法和解释边界。"""
    count=validate(data)
    machine=json.loads(data['machine']) if data['machine'].startswith('{') else {'Name':data['machine']}
    head='# ChaLi 1.0.0 performance measurements' if english else '# ChaLi 1.0.0 与其他语言的性能实测'
    nav='**English** · [简体中文](../12-performance.md)' if english else '[English](en/12-performance.md) · **简体中文**'
    source_prefix='../../benchmarks/current/' if english else '../benchmarks/current/'
    tool_rows=[]
    for key,label in zip(LANGUAGES,LABELS):
        version=data['tools'][key]['version'].splitlines()[0]
        if key=='chali':version='ChaLi 1.0.0 / static LLVM 23.1.4'
        tool_rows.append(f'| {label} | {version} |')
    title='Toolchain' if english else '工具链'
    lines=[head,'','<!-- LANGUAGE NAVIGATION -->',nav,'<!-- END LANGUAGE NAVIGATION -->','']
    if english:
        lines += [f"All values below were collected in this run on **{data['completed'][:10]}**, from {len(CASES)} workloads and eight languages: **144 combinations, {count} timed observations**. No earlier timings are reused. Each cell is the median of 25 intervals, in **milliseconds**.",
                  '', '## Environment and identity','',f"Host: **{machine['Name'].strip()}**, {data['logical_cpus']} logical CPUs; `{data['platform']}`. Sampling started at `{data['sampling_started']}` and finished at `{data['completed']}` (UTC+08:00).",'',
                  f"Measured ChaLi compiler source: `{data['compiler_source_commit']}`; executable SHA-256: `{data['compiler_sha256']}`. This identifies the measured executable; subsequent documentation builds may have different bytes. Workload sources, generated wrappers, tool versions and artifact hashes are retained with the raw records.",
                  '',f'### {title}','','| Language | Actual tool version |','| --- | --- |',*tool_rows,'',
                  '## Method','',
                  'Build and linking finish before sampling. Every combination runs in five independent processes. Each process records five intervals using seeds 0–4; native and interpreted implementations warm once beforehand. Java uses default tiered compilation and GC with `-Xbatch`, warming for at least 32 calls and at least one second per process. Warmup counts and durations remain in the raw output. This does not prove that every method has reached a final compilation tier.', '',
                  'ChaLi uses `build --shared --unchecked --allocator malloc`, with default LLVM O3/LTO. C/C++ use GCC O3/LTO; Rust uses optimization level 3, fat LTO and one codegen unit. Go uses its default optimizer. Lua and CPython use their normal interpreters and memory management. No optimization or implementation is changed after inspecting timings.', '',
                  'All workloads and languages are rerun. Five-seed integer results must match exactly; floating-point results are checked with relative and absolute tolerances of 1e-12. Compilation, process startup, global setup, warmup and checksum aggregation are outside each interval. Local allocation, growth, copying and cleanup performed by the work function are inside it. GC outside an interval is not charged back to that interval.', '',
                  'Sampling is serial; workload and language order are shuffled reproducibly with seed 20261106 on each round. All valid observations are retained, including slow samples. CPU frequency and affinity are not fixed and the operating system and other applications remain active. The ranges below show variation in this environment. No overall language score is calculated.', '',
                  '## Numeric and container workloads','',table(data,CASES[:8]),'',
                  'Sizes: integer chain and fixed-array tasks 4,000,000 iterations; float chain and small-function task 1,000,000; List 400,000 appends; Map/Set 20,000 dispersed keys with insert/overwrite or duplicate insert, hit/miss queries, deletion and reinsertion. All growth workloads start empty without reserving capacity. The previous mixed preallocation `list` case is retired in favor of `list_growth`.', '',
                  '## Payloads, method dispatch, structures and multidimensional arrays','',table(data,CASES[8:14]),'',
                  'Each work function executes 262,144 iterations. Enum and nested-structure inputs contain 256 entries; arrays are 64×64 and 16×16×16. Inputs are built before timing. The method tasks use one implementation or a 2:1:1 mix of three implementations. Normal inlining and devirtualization remain allowed.', '',
                  '## UTF-8 byte tasks','',table(data,CASES[14:]),'',
                  'The input contains 8448 UTF-8 bytes. Workloads perform 131,072 dependent byte reads, 2048 successive concatenations followed by complete-byte reduction, 1024 pairs of owning slices and comparisons, or 256 searches with present and absent needles. Positions and results are byte-based.', '',
                  '## Implementation differences','',
                  'Container, object representation, library algorithm and runtime costs are part of these implementations. C uses an explicit open-addressed hash reference; C++ and Rust use standard containers. Go uses slices and maps; Java uses ArrayList/HashMap/HashSet with Long boxing; Python uses list/dict/set; Lua uses tables. C models payloads and dispatch with tags and function pointers, Rust with enum/dyn trait, and dynamic languages with objects and method lookup. These are task comparisons, not identical ABIs or storage layouts.', '',
                  'String rows compare UTF-8 byte tasks, not all languages’ Unicode String APIs. C++ and Rust append to growable strings and can reuse capacity; the C, ChaLi, Lua, Python and Go/Java byte implementations create successive concatenation results. Python uses bytes; Go and Java use byte sequences with explicit owned slice copies. Java byte search is an explicit loop; other implementations may use optimized library searches. Allocation strategies and library algorithms differ; these rows do not isolate language-compiler costs.', '',
                  'Single-thread kernels, library operations and warmed method loops do not represent application startup, concurrent servers, I/O, build time or peak memory. No language-wide conclusion follows from one row. Read each result together with its source, representation and full range.', '',
                  '## Location, range and quantiles','',
                  'Every row uses all 25 observations. Time values are in milliseconds. Four quantile points are P25, P50, P75 and P100; P50 is the median and P100 is the maximum. R7 linearly interpolates the sorted sample at zero-based position (n−1)p. With n=25 these are the 7th, 13th, 19th and 25th ordered values. See the [R quantile definition](https://stat.ethz.ch/R-manual/R-devel/library/stats/html/quantile.html).','',
                  descriptive_table(data,english=True),'',
                  '## Dispersion and distribution shape','',
                  'Sample variance is s² = sum((x−mean)²)/(n−1), in ms²; sample SD is its square root, in ms. CV is 100s/mean, expressed as a percentage ([NIST definition](https://www.itl.nist.gov/div898/software/dataplot/refman2/auxillar/coefvari.htm)). Adjusted Fisher–Pearson sample skewness is G1 = n/((n−1)(n−2)) × sum(((x−mean)/s)³), a dimensionless value ([NIST skewness definitions](https://www.itl.nist.gov/div898/handbook/eda/section3/eda35b.htm)). When s=0, skewness is undefined rather than zero.','',
                  'These are descriptive statistics of the 25 intervals, including differences between seeds and execution environments. Five intervals share each process; they are not 25 independent processes and are not used here for significance claims. Positive skewness describes a longer right tail, not a speed-based judgment. No observations are removed.','',
                  descriptive_table(data,shape=True,english=True),'',
                  '## Reproduction and raw records','',f"See [the driver]({source_prefix}run.py), [implementations and method]({source_prefix}README.md), [identity audit]({source_prefix}audit.py), and [report generator]({source_prefix}report.py). Raw JSON contains commands, output, seed results, all samples, summaries, order and SHA-256 identities. The exported data archive also retains measured artifacts and source snapshots.", '',
                  '```powershell','python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --build-only','python benchmarks/current/audit.py build/validation/performance-current/run/results.json','python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --measure-only','python benchmarks/current/audit.py build/validation/performance-current/run/results.json --after','python benchmarks/current/report.py build/validation/performance-current/run/results.json --write-docs','```']
    else:
        lines += [f"以下数据全部采集于 **{data['completed'][:10]}** 的本次测试，覆盖 18 个项目和八种语言，共 **144 组组合、{count} 个正式观测**。不复用以前的计时。表格单位均为 **毫秒**，每格取 25 个区间的中位数。",'',
                  '## 环境与测量身份','',f"宿主：**{machine['Name'].strip()}**，{data['logical_cpus']} 个逻辑处理器；`{data['platform']}`。采样从 `{data['sampling_started']}` 开始，到 `{data['completed']}` 结束，时间为 UTC+08:00。",'',
                  f"被测 ChaLi 编译器源码提交为 `{data['compiler_source_commit']}`，可执行文件 SHA-256 为 `{data['compiler_sha256']}`。该摘要对应本次实际被测字节；后续仅因文档更新而重建的发行附件可能具有不同摘要。原始记录同时保存负载源码、生成的计时包装、工具版本和构件摘要。",'',
                  f'### {title}','','| 语言 | 实际工具版本 |','| --- | --- |',*tool_rows,'',
                  '## 测量方法','',
                  '先完成所有构建和链接，再采样。每个组合运行五个独立进程，每进程用种子 0～4 记录五个区间。原生及解释器实现先预热一次；Java 使用默认分层编译和 GC、`-Xbatch`，每进程预热至少 32 次且至少一秒。原始输出保留预热次数和耗时，不据此断言所有方法已经达到最终编译层级。', '',
                  'ChaLi 使用 `build --shared --unchecked --allocator malloc`，默认 LLVM O3/LTO；C/C++ 使用 GCC O3/LTO；Rust 使用优化等级 3、fat LTO 和一个代码生成单元；Go 使用默认优化。Lua 和 CPython 保留默认解释器与内存管理。本次不依据计时结果修改优化选项或挑选实现。', '',
                  '全部语言和项目均重新运行。整数结果按五个种子逐项完全一致；浮点按相对与绝对误差 1e-12 校验。构建、进程启动、全局输入初始化、预热和校验值归并不计入工作区间；工作函数内部的局部创建、增长、复制和返回前清理计时。发生在区间外的 GC 不会补记到该区间。', '',
                  '正式采样串行进行，每轮使用固定随机种子 20261106 改变项目与语言顺序。保留全部有效观测，包括较慢样本。未固定 CPU 频率与亲和性，操作系统及其他应用保持运行；下面同时列出本机环境中的波动范围。不合成语言总分。', '',
                  '## 数值、数组与容器','',table(data,CASES[:8]),'',
                  '规模：整数链与固定数组项目各 400 万轮；浮点链、小函数各 100 万轮；List 追加 40 万个元素；Map/Set 各 2 万个分散整数键，包含插入、覆盖或重复插入、命中与缺失查询、删除和重插。增长项目从空容器开始，不预留容量。预分配策略不一致的旧 `list` 对照已移除，以 `list_growth` 为准。', '',
                  '## 载荷、方法调用、结构体与多维数组','',table(data,CASES[8:14]),'',
                  '各工作函数执行 262144 轮。枚举和嵌套结构包含 256 个输入；二维数组为 64×64，三维数组为 16×16×16。输入在计时前构造。方法调用项目分别使用单一实现，或按 2:1:1 比例选择三种实现；允许正常内联和去虚化。', '',
                  '## UTF-8 字节任务','',table(data,CASES[14:]),'',
                  '输入总长 8448 字节。四项依次为 131072 次依赖索引的字节读取、2048 次逐次拼接后归并全部字节、1024 对拥有切片的比较、256 次包含命中与缺失模式的查找。位置和返回结果均按字节计算。', '',
                  '## 实现差异与适用范围','',
                  '容器、对象表示、标准库算法与运行时成本都属于本次实现。C 使用显式开放寻址参考；C++/Rust 使用标准容器；Go 使用 slice/map；Java 使用带 Long 装箱的 ArrayList/HashMap/HashSet；Python 使用 list/dict/set；Lua 使用 table。C 用标签和函数指针表达载荷及分派，Rust 使用 enum/dyn trait，动态语言使用对象和方法查找。这些项目完成同类任务，不要求 ABI 和存储布局相同。', '',
                  '字符串组比较 UTF-8 字节任务，不等同于全部语言的 Unicode String API。C++/Rust 向可增长字符串追加，可复用容量；C、ChaLi、Lua、Python 及本次 Go/Java 字节实现逐次创建拼接结果。Python 使用 bytes，Go/Java 使用字节序列，切片显式创建拥有副本。Java 字节查找使用显式循环，其他实现可以使用优化后的标准库查找。分配策略与库算法存在差异，这些行不用于隔离语言编译器本身的成本。', '',
                  '单线程计算、库操作与预热后的方法循环不能代表应用启动、并发服务、I/O、构建耗时或峰值内存。单项结果不推导为整个语言的结论，应结合源码、数据表示和完整波动范围阅读。', '',
                  '## 位置、范围与四个分位点','',
                  '每行使用全部 25 个观测，时间单位为毫秒。四个分位点是 P25、P50、P75、P100；P50 等于中位数，P100 等于最大值。R7 在排序后从零计数的位置 (n−1)p 做线性插值。本次 n=25，四个点分别是排序后的第 7、13、19、25 个值。计算方式见 [R 分位数定义](https://stat.ethz.ch/R-manual/R-devel/library/stats/html/quantile.html)。','',
                  descriptive_table(data),'',
                  '## 离散程度与分布形状','',
                  '样本方差 s² = sum((x−平均值)²)/(n−1)，单位 ms²；样本标准差 s 是其平方根，单位 ms。变异系数 CV = 100s/平均值，以百分比表示（[NIST 定义](https://www.itl.nist.gov/div898/software/dataplot/refman2/auxillar/coefvari.htm)）。偏度采用校正 Fisher–Pearson 样本偏度 G1 = n/((n−1)(n−2)) × sum(((x−平均值)/s)³)，无量纲（[NIST 偏度定义](https://www.itl.nist.gov/div898/handbook/eda/section3/eda35b.htm)）。标准差为零时偏度记为未定义，不填零。','',
                  '这些指标描述全部 25 个区间的分布，同时包含输入种子差异和执行环境的影响。其中每五个区间来自同一进程，不能视作 25 个独立进程作显著性推断。正偏度描述较长的右尾，不作为速度优劣判定；保留全部观测。','',
                  descriptive_table(data,shape=True),'',
                  '## 复现与原始记录','',f"[驱动]({source_prefix}run.py)、[实现与方法]({source_prefix}README.md)、[构件身份核对]({source_prefix}audit.py)和[报告生成器]({source_prefix}report.py)均保留。原始 JSON 保存命令、原始输出、逐种子结果、全部样本、统计、顺序和 SHA-256；导出的数据附件同时包含实际被测构件与源码快照。", '',
                  '```powershell','python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --build-only','python benchmarks/current/audit.py build/validation/performance-current/run/results.json','python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --measure-only','python benchmarks/current/audit.py build/validation/performance-current/run/results.json --after','python benchmarks/current/report.py build/validation/performance-current/run/results.json --write-docs','```']
    return '\n'.join(lines)+'\n'


def main():
    """默认只检查；显式指定时同步两份文档。"""
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('report',type=Path);parser.add_argument('--write-docs',action='store_true');args=parser.parse_args()
    data=json.loads(args.report.read_text(encoding='utf-8'));count=validate(data)
    if args.write_docs:
        root=Path(__file__).resolve().parents[2]
        (root/'doc/12-performance.md').write_text(render(data),encoding='utf-8')
        (root/'doc/en/12-performance.md').write_text(render(data,True),encoding='utf-8')
    print('Verified:',len(data['summary']),'combinations,',count,'raw observations')


if __name__=='__main__':main()
