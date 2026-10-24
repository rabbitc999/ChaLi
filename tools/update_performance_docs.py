"""从真实容器与热点原始样本同步生成中英文性能页，保留历史版本身份。"""
import json
import math
from pathlib import Path
from documentation_navigation import decorate

ROOT = Path(__file__).resolve().parents[1]


def load(relative):
    """读取仓库内已完成的原始测量。"""
    return json.loads((ROOT / relative).read_text(encoding='utf-8'))


def table(headers, rows):
    """生成便于链接检查及阅读的 Markdown 表格。"""
    return '\n'.join(['| ' + ' | '.join(headers) + ' |', '| ' + ' | '.join(['---'] * len(headers)) + ' |'] +
                     ['| ' + ' | '.join(map(str, row)) + ' |' for row in rows])


def render(english=False):
    """从同一份样本输出两个语言版本，统一单位、统计口径与基线身份。"""
    containers = load('benchmarks/results/containers-0310/results.json')
    review = load('benchmarks/results/containers-0310/list-remove-review.json')
    hotspots = load('benchmarks/results/hotspots-0310/results.json')
    arrays = load('benchmarks/results/array-rw-0310/results.json')
    historical = load('benchmarks/results/hotspots-0310/vm-historical-before-after.json')
    relative = '../../' if english else '../'
    link = lambda name, path: '[' + name + '](' + relative + path + ')'
    before, after = containers['compilers']['before'], containers['compilers']['after']
    assert before['version'] == after['version'] == '0.31.0'
    assert historical['compiler']['before']['reported_version'] == '0.30.0'
    assert historical['compiler']['after']['reported_version'] == '0.31.0'
    title = '# VM and LLVM AOT performance measurements' if english else '# VM 与 LLVM AOT 性能测试'
    current_version = (ROOT / 'VERSION').read_text(encoding='utf-8').strip()
    introduction = (f'The current program version is **ChaLi {current_version}**; the latest measured baseline below is **ChaLi 0.31.0**. Measurements retain the actual binary versions and are not relabeled as {current_version}. Default List, Map and Set workloads were measured before and after the call-path changes on the same host. C++17, CPython and the official Lua VM were measured in the same campaign.' if english else
                    f'当前程序版本为 **ChaLi {current_version}**；下述最近实测基线为 **ChaLi 0.31.0**，保留真实二进制版本，不将测量改标为 {current_version}。默认 List、Map、Set 在同一台机器上使用优化前后两份编译器交错测量，并在同轮加入 C++17、CPython 和官方 Lua VM 对照。')
    evidence = link('Container raw samples' if english else '容器原始样本', 'benchmarks/results/containers-0310/results.json')
    method = ('Each configuration has three processes, one warm-up and five timed repetitions per process: 15 execution samples per workload. Configuration order changes between rounds. Work includes setup, allocation and destruction; compiler startup and compilation are outside the internal timer. Process wall times and build times remain in the raw report.' if english else
              '每配置运行 3 次进程，每进程预热 1 次并计时 5 次，每负载共 15 个执行样本；各轮改变配置执行顺序。work 内的建表、分配和清理计入耗时，编译器启动与编译不计入内部计时，进程总耗时和构建耗时仍保存在原始报告中。')
    environment = table(['Item', 'Configuration'] if english else ['项目', '配置'], [
        ['Host' if english else '宿主', containers['host']],
        ['CPU', containers['processor'] + ', ' + str(containers['cpu_count']) + (' logical processors' if english else ' 个逻辑处理器')],
        ['C++', containers['tools']['cpp'] + '，`-std=c++17 -O2 -DNDEBUG`'],
        ['Python', containers['tools']['python'].splitlines()[0]],
        ['Lua', containers['tools']['lua']],
        ['LLVM', containers['tools']['llvm'].splitlines()[0] + '，`-O2`'],
        ['VM', 'Highest defaults; reflection enabled; full quadruple optimizer with maxRounds = 4' if english else '默认最高优化，启用反射，完整四元式优化 maxRounds = 4'],
        ['AOT', 'LLVM machine code with default bounds and live-memory checks; shared standard-library runtime' if english else 'LLVM 机器码，默认保留边界与存活内存检查，复用共享标准库运行时'],
    ])
    identities = ('Both container binaries report 0.31.0. Before SHA256: `' + before['sha256'] + '`; after SHA256: `' + after['sha256'] + '`.' if english else
                  '容器对照的两份编译器均自报 0.31.0。优化前 SHA256：`' + before['sha256'] + '`；优化后 SHA256：`' + after['sha256'] + '`。')
    workload = ('All container elements, keys and values are int64. Each work call performs 50,000 primary loop iterations; read/update workloads prefill 1,024 entries. Build workloads insert distinct entries and read or query them; removal workloads build then empty the container. Set read queries alternate between the populated and missing halves of a 2,048-key range; Set write repeatedly inserts 1,024 distinct keys. Independent closed-form checksum formulas validate all configurations.' if english else
                '容器元素、键和值均为 int64。每次 work 的主要循环为 50,000 轮，读取和更新负载先填充 1,024 项。构建负载插入不同元素后读取或查询，删除负载建表后清空。Set 读取在 2,048 个键的范围内包含命中和未命中，Set 写入反复插入 1,024 个不同键。所有配置均核对独立闭式公式给出的校验值。')
    differences = ('C++ uses std::vector, std::unordered_map and std::unordered_set with default growth and no reserve. Python uses list/dict/set. Lua uses tables for sequences and maps and key-to-true entries with an explicit count for sets; dense integer keys may use its array part. These tables compare the specified programs and their default allocation strategies. C++ timing enters work through a volatile function pointer to prevent moving the computation outside the timer.' if english else
                   'C++ 使用默认增长的 std::vector、std::unordered_map、std::unordered_set，不调用 reserve；Python 使用 list/dict/set；Lua 用 table 表示序列与映射，Set 用键到 true 的映射并维护计数，密集整数键可能走 table 数组区。表格比较这些具体程序及默认分配策略。C++ 计时通过 volatile 函数入口调用 work，防止编译器把计算移出计时区间。')
    primary, cross, comparisons = [], [], []
    for name, row in containers['cases'].items():
        stats = row['statistics']
        values = [name]
        for backend in ['vm', 'aot']:
            old, new = stats['before_' + backend]['median'], stats['after_' + backend]['median']
            values += [f'{old / 1e6:.3f}', f'{new / 1e6:.3f}', f'{(new / old - 1) * 100:+.2f}%']
        primary.append(values)
        cross.append([name] + [f"{stats[key]['median'] / 1e6:.3f}" for key in ['after_vm', 'after_aot', 'cpp', 'python', 'lua']])
        vm_time = stats['after_vm']['median']
        comparisons.append([name] + [f"{vm_time / stats[key]['median']:.2f}x" for key in ['after_aot', 'cpp', 'python', 'lua']])
    primary_table = table(['Workload', 'VM before', 'VM after', 'VM change', 'AOT before', 'AOT after', 'AOT change'] if english else
                          ['负载', 'VM 优化前', 'VM 优化后', 'VM 耗时变化', 'AOT 优化前', 'AOT 优化后', 'AOT 耗时变化'], primary)
    cross_table = table(['Workload', 'VM', 'LLVM AOT', 'C++', 'Python', 'Lua'] if english else ['负载', 'VM', 'LLVM AOT', 'C++', 'Python', 'Lua'], cross)
    comparison_table = table(['Workload', 'AOT speedup over VM', 'VM / C++ time', 'VM / Python time', 'VM / Lua time'] if english else
                             ['负载', 'AOT 相对 VM 提速', 'VM / C++ 耗时', 'VM / Python 耗时', 'VM / Lua 耗时'], comparisons)
    aggregate = []
    for kind in ['list', 'map', 'set']:
        values = [kind.capitalize()]
        for backend in ['vm', 'aot']:
            speed = math.prod(row['statistics']['before_' + backend]['median'] / row['statistics']['after_' + backend]['median']
                              for name, row in containers['cases'].items() if name.startswith(kind + '_')) ** 0.25
            values.append(f'{(1 - 1 / speed) * 100:.2f}%')
        aggregate.append(values)
    aggregate_table = table(['Container', 'VM time reduction', 'AOT time reduction'] if english else ['容器', 'VM 耗时下降', 'AOT 耗时下降'], aggregate)
    review_stats = review['cases']['list_remove']['statistics']
    review_change = (review_stats['after_vm']['median'] / review_stats['before_vm']['median'] - 1) * 100
    review_text = ('The primary VM List removal result is approximately 1.5% slower. An independent seven-process review gives ' + f'{review_change:+.2f}%' + ' instead; both datasets are retained. This small VM change is not established as a consistent speedup. Aggregate reductions above use the geometric mean of speed ratios from the original four workloads per container, without replacing samples.' if english else
                   'VM 的 List 删除主测约慢 1.5%，单独用 7 次进程复核则为 ' + f'{review_change:+.2f}%' + '；两份数据均保留，不将这一小幅变化宣称为稳定提速。上表按每种容器原始四项负载的速度比取几何平均后换算耗时下降，没有替换主测样本。')
    review_text += ' ' + link('Review samples' if english else '独立复核样本', 'benchmarks/results/containers-0310/list-remove-review.json')
    implementation = ('The shared container glue borrows native arguments and reads slots on demand, removing an extra temporary vector and copy on each call. Internal combined operations use stack arrays. The AOT bridge reserves argument capacity once. Argument counts, value semantics, lifetimes, bounds checks and key policies remain covered by the VM/AOT regression suite.' if english else
                      '共享容器胶水借用原生实参并按需读取槽，取消每次调用额外构造临时数组及复制；内部组合操作使用栈数组，AOT 调用桥一次预留实参容量。参数数量、值语义、生命周期、边界和键策略继续由 VM/AOT 回归覆盖。')
    labels = {'chali': 'ChaLi 0.31.0 (VM, highest defaults)', 'chali-aot': 'ChaLi 0.31.0 (LLVM AOT, default checks)',
              'c': 'C (GCC -O2)', 'cpp': 'C++17 (GCC -O2)', 'lua': 'Lua 5.5.1 (official VM)', 'python': 'CPython 3.12'}
    hotspot_rows = [[labels[name] if english else row['label']] + [f"{row['median_ms'][key]:.3f}" for key in ['call', 'array', 'list']] for name, row in hotspots['configs'].items()]
    hotspot_table = table(['Configuration', 'Small calls', 'Fixed-array reads', 'List growth/traversal'] if english else ['配置', '小函数调用', '固定数组读取', '列表增长及遍历'], hotspot_rows)
    array_table = table(['Configuration', 'Fixed-array read/write'] if english else ['配置', '固定数组读写'],
                        [[labels[name] if english else row['label'], f"{row['median_ms']['array_rw']:.3f}"] for name, row in arrays['configs'].items()])
    hotspot_ratios = [[name, f"{hotspots['configs']['chali']['median_ms'][key] / hotspots['configs']['chali-aot']['median_ms'][key]:.2f}x"]
                      for key, name in [('call', 'Small calls' if english else '小函数调用'),
                                        ('array', 'Fixed-array reads' if english else '固定数组读取'),
                                        ('list', 'List growth/traversal' if english else '列表增长及遍历')]]
    hotspot_ratios.append(['Fixed-array read/write' if english else '固定数组读写',
                           f"{arrays['configs']['chali']['median_ms']['array_rw'] / arrays['configs']['chali-aot']['median_ms']['array_rw']:.2f}x"])
    hotspot_ratio_table = table(['Workload', 'AOT speedup over VM'] if english else ['负载', 'AOT 相对 VM 提速'], hotspot_ratios)
    historical_table = table(['Workload', 'Before 0.30.0', 'After 0.31.0', 'Speed ratio'] if english else ['负载', '历史优化前 0.30.0', '测量优化后 0.31.0', '速度比'],
                             [[name, f"{row['before_median'] / 1e6:.3f}", f"{row['after_median'] / 1e6:.3f}", f"{row['speedup']:.2f}x"] for name, row in historical['cases'].items()])
    sections = [title, introduction, evidence, '## Method and configuration' if english else '## 方法与配置', method, environment, identities,
                '## Default-container workloads' if english else '## 默认容器负载', workload, differences,
                link('Sources and generator' if english else '负载源码与生成器', 'benchmarks/perf/container_cases.py') + '；' + link('Driver' if english else '测量驱动', 'benchmarks/perf/containers.py'),
                '## Before/after container execution times (ms)' if english else '## 容器优化前后耗时（毫秒）', primary_table, aggregate_table, review_text, implementation,
                '## Same-campaign language comparison (ms)' if english else '## 同轮跨语言对照（毫秒）', cross_table,
                '## AOT versus VM execution performance and VM language comparisons' if english else '## AOT 与 VM 执行性能及 VM 跨语言速度比',
                'All ratios below divide the optimized VM execution time by the corresponding implementation time for the same workload. An AOT speedup of 1.20x means AOT completes the workload in 1 / 1.20 of the VM time. A VM / Python ratio of 20x means VM takes twenty times as long as Python. These are execution-performance comparisons, excluding compilation and startup.' if english else
                '下表均用优化后 VM 的执行耗时除以同一负载对应实现的耗时。“AOT 相对 VM 提速 1.20x”表示 AOT 耗时为 VM 的 1 / 1.20；“VM / Python 耗时 20x”表示 VM 耗时为 Python 的 20 倍。这些数值比较运行性能，不计编译与启动耗时。',
                comparison_table,
                'The current VM and AOT container paths still have substantial overhead relative to these other implementations; the measured reductions do not remove that gap.' if english else '当前 VM 和 AOT 容器路径相对这组其他语言实现仍有较大开销，表格中的改善幅度并未消除这一差距。',
                '## Small calls and fixed-array reads (ms)' if english else '## 小函数与固定数组读取（毫秒）', hotspot_table,
                link('Hotspot samples' if english else '热点原始样本', 'benchmarks/results/hotspots-0310/results.json'),
                'The hotspot List case has 400,000 elements and a preallocated C/C++ comparison. It is a separate workload from the default-container campaign above.' if english else '热点组的 List 为 400,000 元素，C/C++ 对照按已知数量预分配；它与上面的默认容器主测是两组不同负载。',
                '## Fixed-array read/write (ms)' if english else '## 固定数组读写（毫秒）', array_table,
                link('Array read/write samples' if english else '数组读写原始样本', 'benchmarks/results/array-rw-0310/results.json'),
                '## AOT versus VM hotspot execution performance' if english else '## AOT 与 VM 热点执行性能对比', hotspot_ratio_table,
                'The small-call workload permits inlining in every language. Its ratio measures the execution time of the complete program after each implementation optimizes it; it is not an isolated measurement of one non-inlined function call. Ratios are specific to these workloads.' if english else
                '小函数负载允许各语言内联，速度比反映整段程序经各自优化后的实际执行耗时，不等同于单次禁止内联调用的成本。各项比例只适用于对应负载。',
                '## Historical VM hotspot baseline (ms)' if english else '## 历史 VM 热点基线复核（毫秒）',
                'The preserved older compiler reports 0.30.0. It was rerun in alternating order with 0.31.0, retaining its actual version and binary identity. This is separate from the 0.31.0 container before/after baseline.' if english else '保存的旧编译器实际自报 0.30.0，本轮与 0.31.0 交错重测并保留真实版本及二进制身份。这组历史对照与本轮容器优化的 0.31.0 前后基线分别记录。',
                historical_table, link('Historical interleaved samples' if english else '历史交错原始样本', 'benchmarks/results/hotspots-0310/vm-historical-before-after.json'),
                link('Same-version VM comparison' if english else '同为 0.31.0 的 VM 前后对照', 'benchmarks/results/hotspots-0310/vm-before-after.json')]
    path = ROOT / ('doc/en/12-vm-performance.md' if english else 'doc/12-vm-performance.md')
    return decorate(path, '\n\n'.join(sections) + '\n')


def main():
    """将中英文页写回固定功能文档路径，数值仅来自原始 JSON。"""
    (ROOT / 'doc/12-vm-performance.md').write_text(render(), encoding='utf-8')
    (ROOT / 'doc/en/12-vm-performance.md').write_text(render(True), encoding='utf-8')


if __name__ == '__main__':
    main()
