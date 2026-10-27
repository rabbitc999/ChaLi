# AOT 容器实验：Map 三元组、Set 槽位数组和分块 List

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本轮按用户要求主要评估 LLVM AOT。正式结果只包含 AOT 程序；先前穿插 VM 的未完成试跑保存在 [mixed-partial.json](mixed-partial.json) 和 [日志](mixed-partial.log)，不混入正式中位数。编译器和默认标准库保持原状，本次改动是 ChaLi 实验源码及对照工具。

## 设计

- Map：单个槽位数组，每项为 `{ Value, HasValue, HashKey }`。`HashKey` 保存原始 int64 键，采用恒等散列再对容量取模，不只是保存桶号；查询会比较完整键。删除后移修复探测链，省去墓碑状态，未使用 Option 枚举。原自举 Map 本来也是三个并行数组而非 Option，因此该对照同时改变布局和删除策略。
- Set：独立槽位数组，每项仅 `{ HashKey, HasValue }`，不包装 Map，也不分配多余的 Value 槽。与新 Map 相同，最大装载率一半，删除后移修复探测链。后移删除最坏扫描整个簇，不能视为最坏常数时间。
- List：块指针表和独立堆块，每块 64 个元素。增长只扩展块表，不搬移已有数据块；按块号和块内偏移访问。pop 只缩短长度，保留块及槽值，供后续 push 复用。容器关闭时统一释放所有块。
- 分块 List 显式管理堆块，使用 Closeable 防止普通按值复制；`.copy()` 创建独立容器，但不实现 Copy 接口，遵守 Copy 与 NonCopyable 互斥。关闭后禁止追加。备用块默认构造 T，实验不承诺对非默认可构造或不可复制资源元素的适用性；延迟释放被删除值也会延长其资源持有时间。

参考 GitHub 上的 [Microsoft STL deque](https://github.com/microsoft/STL/blob/main/stl/inc/deque) 的块表及分块下标布局，以及 [Folly fbvector 设计说明](https://github.com/facebook/folly/blob/main/folly/docs/FBVector.md) 对增长、分配器和对象搬移成本的讨论。访问日期为 2026-10-26。采用独立实现，64 元素块大小是本实验选择，并非照搬 Microsoft 的块大小。

## 测量方法

- 编译器实际版本：1.0.5；SHA256：`5ba76f1ace72b64fbb6823d551c0fd30912d3656b09a5f67bed45d031992a6d8`。
- 宿主：`Windows-10-10.0.19045-SP0`；CPU：`AMD64 Family 25 Model 33 Stepping 2, AuthenticAMD`；12 个逻辑处理器。Windows x64，LLVM external 变体。
- 默认最高优化、完整四元式预算、前后端反射，保留边界和存活检查。
- 每负载 1,024 次操作，读取／更新工作集 1,024；三个实现均使用同一编译器。全部 AOT 程序先构建完成，再交错计时；每配置 3 个进程，每进程预热一次、计时 5 次，中位数取全部 15 个样本。12 组 × 3 个实现 × 15 个样本，共 540 个正式样本。
- 计时范围包含 work 内建表、分配、操作和最终清理，编译与启动不计入内部时间。每进程结果均与独立闭式公式核对。
- 原始构建记录、源文件 SHA256、stdout/stderr、纳秒样本和执行顺序见 [results.json](results.json)。复用预构建程序前核对编译器、配置及生成源码 SHA256。
- 同轮重新测量原自举版本和默认容器。此前 2,048 次操作的报告保持原样，不能把两轮绝对耗时直接相除。

## AOT 结果

时间单位为毫秒，最后一列为原自举版耗时 ÷ 新版耗时，大于 1 表示新版更快。报告中的 triples 配置代表新槽位 Map/Set 及分块 List。

| 负载 | 默认容器 | 原自举版 | 新版 | 原版／新版 |
| --- | ---: | ---: | ---: | ---: |
| list_build | 1.922 | 578.870 | 9.192 | 62.98× |
| list_read | 1.685 | 613.684 | 9.186 | 66.80× |
| list_write | 2.655 | 1471.006 | 12.488 | 117.79× |
| list_remove | 1.666 | 1614.362 | 8.452 | 191.01× |
| map_build | 2.054 | 4.978 | 6.982 | 0.71× |
| map_read | 2.141 | 5.022 | 6.895 | 0.73× |
| map_write | 3.016 | 5.646 | 7.613 | 0.74× |
| map_remove | 2.976 | 183.401 | 366.466 | 0.50× |
| set_build | 2.007 | 2.951 | 3.860 | 0.76× |
| set_read | 1.965 | 2.995 | 3.853 | 0.78× |
| set_write | 1.135 | 2.899 | 3.595 | 0.81× |
| set_remove | 4.810 | 183.935 | 317.579 | 0.58× |

## 解释与限制

分块 List 在本轮 AOT 的四项负载中比原自举版本快 62.98～191.01 倍，但仍比默认 List 慢约 4.70～5.45 倍。Map 三元组四项均回退，耗时约为原自举版本的 1.35～2.00 倍；独立 Set 槽位数组也全部回退，约为 1.24～1.73 倍。当前数据支持继续优化分块 List，暂不支持用新 Map/Set 替换原实现。

应逐项比较上表，不能因为布局变化就推定所有操作都会更快。List 同时改变了存储布局、拥有关系和 pop 路径；Map/Set 同时改变布局和删除策略，本轮没有把这些因素逐项分离归因。所有实现仍是基础操作实验，API、元素类型及所有权约束尚不等同于默认标准库。

Map/Set 的性能负载使用 int64 键和值。正确性测试另外覆盖 String 值、零值／空字符串、负数与极值键、跨数组尾部的冲突簇、连续删除和重复删除、扩容、全部删除后重用及独立副本。分块 List 另验证 300 个 String 元素、63/64 块边界、跨块复制、弹空后重新追加。

- [Map/Set VM 与 AOT 正确性](map-set-correctness.log)
- [组合容器 VM 与 AOT 正确性](combined-correctness.log)
- [分块 List 额外边界检查](segmented-boundaries.log)
- [显式关闭、重复关闭及禁止隐式复制](segmented-ownership.log)
- [全部新增用例定稿后的 AOT 复核](final-aot-correctness.log)

## 复现

```powershell
$env:TMPDIR="$PWD/tmp"; $env:TMP=$env:TMPDIR; $env:TEMP=$env:TMPDIR
python tests/chali_bootstrap_containers.py --map-triples --segmented-list --aot-only
python benchmarks/perf/bootstrap_containers.py --operations 1024 --runs 3 --map-triples --segmented-list --aot-only --out tmp/slot-segmented.json
```

源码：[Map/Set 槽位数组](../../perf/bootstrap_map_triples.chl)、[分块 List](../../perf/bootstrap_list_segmented.chl)、[对照脚本](../../perf/bootstrap_containers.py)。
