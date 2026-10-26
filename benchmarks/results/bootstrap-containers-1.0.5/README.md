# ChaLi 1.0.5 自举 List、Map、Set 对照

同一份 1.0.5 编译器分别执行默认原生容器与 ChaLi 编写的实验容器。所有计时进程均校验独立闭式公式计算的结果；本报告只代表下面列出的负载和配置。

## 配置与方法

- 宿主：`Windows-10-10.0.19045-SP0`；CPU：`AMD64 Family 25 Model 33 Stepping 2, AuthenticAMD`；逻辑处理器：12。
- 实际验证平台：Windows x64；LLVM external 变体；默认最高四元式优化、完整预算、前后端反射及边界／存活检查。
- 实际编译器版本：`1.0.5`；SHA256：`5ba76f1ace72b64fbb6823d551c0fd30912d3656b09a5f67bed45d031992a6d8`。
- 每组操作数：2048；读写工作集：1024。每配置 3 个进程，每进程预热一次、计时 5 次，表中取全部 15 个样本的中位数。
- 同一负载交错执行原生／自举、VM／AOT；AOT 全部构建完成后才计时。表中时间为 work 内部时间，包含建表、分配和清理；编译、启动不计入内部时间。
- 原始 stdout、stderr、进程总时间、构建时间、源文件 SHA256、执行顺序和所有纳秒样本保存在 [results.json](results.json)。
- 10,000 次操作的试跑因自举 List AOT 耗时过长而中止；[partial-10000.json](partial-10000.json) 和 [日志](partial-10000.log) 保留已完成样本，不纳入下表。

## 结果

单位为毫秒；倍率为自举耗时 ÷ 原生耗时，小于 1 表示自举更快。

| 负载 | VM 原生 | VM 自举 | VM 倍率 | AOT 原生 | AOT 自举 | AOT 倍率 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| list_build | 4.212 | 482.736 | 114.62× | 3.360 | 2427.889 | 722.65× |
| list_read | 3.185 | 297.373 | 93.35× | 2.514 | 653.648 | 259.98× |
| list_write | 5.614 | 537.153 | 95.68× | 4.233 | 2739.870 | 647.25× |
| list_remove | 4.193 | 1316.834 | 314.05× | 3.297 | 5432.682 | 1647.92× |
| map_build | 6.278 | 1118.037 | 178.09× | 4.170 | 9.925 | 2.38× |
| map_read | 4.655 | 712.715 | 153.10× | 3.113 | 7.351 | 2.36× |
| map_write | 7.917 | 1008.758 | 127.42× | 4.939 | 8.597 | 1.74× |
| map_remove | 7.960 | 2928.826 | 367.92× | 5.926 | 739.474 | 124.79× |
| set_build | 4.907 | 1043.109 | 212.59× | 3.904 | 5.828 | 1.49× |
| set_read | 3.581 | 640.785 | 178.94× | 2.846 | 3.146 | 1.11× |
| set_write | 2.478 | 560.121 | 226.04× | 2.138 | 3.193 | 1.49× |
| set_remove | 7.411 | 2940.517 | 396.77× | 5.941 | 745.024 | 125.41× |

## List 删除时保留槽位

按追加要求，再测量只递减有效长度的 `pop()`：不清空已删除槽位，不缩容，保留原来的 `Option<T>` 返回和 `unwrap()`，在容器销毁时统一清理。原版 `pop()` 也保留缓冲区和容量，但通过 `resize(count - 1)` 清空被删除槽位。

此轮仍为 2,048 次操作、每配置 15 个样本，使用同一编译器；原生配置在该轮重新测量，完整输出见 [list-retain-slots.json](list-retain-slots.json)。

| 路径 | 原版自举删除 ms | 保留槽位 ms | 原版／保留槽位 | 耗时降低 |
| --- | ---: | ---: | ---: | ---: |
| VM | 1316.834 | 658.086 | 2.00× | 50.03% |
| LLVM AOT | 5432.682 | 3406.948 | 1.59× | 37.29% |

该改动有明显改善，但仍慢于默认容器。改善包含省去 `resize()` 调用、检查和逐槽清理，不能据此把瓶颈全部归因于归还内存。该负载的计时仍包含 List 构建、删除循环和最终清理，两个版本范围相同；结果不是单独一次 pop 的耗时。两个实验先后执行，绝对倍率也可能受系统状态影响，原始样本保留供核对。

仅在实验基准内启用这种语义，未修改标准库：对 String、资源或聚合值延迟清理可能增加资源持有时间。本轮元素为 int64；额外 VM/AOT 检查验证了长度递减、容量不变、删除后槽值仍保留、全部弹出后重新 push 的正确性，见 [检查记录](retained-slots-correctness.log)。

```powershell
python benchmarks/perf/bootstrap_containers.py --operations 2048 --runs 3 --cases list_remove --list-retain-slots --out tmp/list-retain-slots.json
```

## 结论与范围

本次数据用于判断当前实验实现，不建议仅因为容器用 ChaLi 编写就替换默认实现。逐项结果应按表中倍率比较；当前实现的值复制、泛型调用、容量槽标签和扩容都会增加成本，尚未对这些因素逐项剖析归因。

- List 使用 `DynamicArray<T>`；Map 为 int64 键、泛型值、取模哈希、线性探测与墓碑删除，最大装载率为一半；Set 包装 Map，额外保存值槽。实验 API 覆盖本次基础操作，不等同于默认容器的完整 API。
- 本次性能负载使用 int64 元素／键／值；String 值、负数键、极值键、冲突、删除后重用和独立复制另有正确性测试。不能把这组吞吐量推断为所有键类型、容量或应用的表现。
- 正确性测试在 VM 与独立 LLVM AOT 上均通过：1500 步操作与默认 Map/Set 逐项交叉检查；动态数组另覆盖 String、聚合值、析构、备用容量不构造 T、复制、缩短和重新增长。
- [专项数组记录](validation-arrays-final.log)、[Copy 互斥记录](validation-copy-final.log)、[自举正确性记录](validation-bootstrap-final.log)、[范围回归记录](validation.json)。这些是按改动范围执行的检查，不代表运行了仓库全部测试。

## 复现

```powershell
$env:TMPDIR="$PWD/tmp"; $env:TMP=$env:TMPDIR; $env:TEMP=$env:TMPDIR
python benchmarks/perf/bootstrap_containers.py --operations 2048 --runs 3 --out tmp/bootstrap-recheck.json
python tests/chali_bootstrap_containers.py
python tests/chali_multidimensional_arrays.py
python tests/chali_copy_interface.py
```

实验源码：[bootstrap_containers.chl](../../perf/bootstrap_containers.chl)；计时脚本：[bootstrap_containers.py](../../perf/bootstrap_containers.py)。历史 0.31.0 容器报告保留原始版本与数据。
