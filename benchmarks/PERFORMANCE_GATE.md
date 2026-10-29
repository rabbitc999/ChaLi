# 已保存样本的性能验收

`evaluate_saved.py` 不编译、不重新计时，也不运行参考语言。当前门限如下：

- 有冻结 Lua 参考的负载，ChaLi 默认 LLVM O3 耗时必须严格小于 Lua；拼接采用逐次拼接和 `table.concat` 中较快的参考。
- 单态与多态接口各自要求中位耗时小于 10 ms；使用原冻结负载、五进程各五个样本。恰好 10 ms 不通过。
- C++ O2 只保留实测参考和比值，不参与判定。其他没有 Lua 参考、也没有独立门限的特征负载标记为“未设该项门限”，不冒充通过或快于 Lua。

历史 1.05／1.20／2 倍条件、原始报告、版本和样本保留，不重写旧报告。调整门限只重新计算保存的数据，不重跑任何基准。

正式输入必须为五进程、各五样本。工具重算中位数，核对冻结源码、驱动及计时壳摘要，拒绝失败进程、不足或损坏的样本、重复负载、缺项及不干净工作区的测量。默认要求同一编译器字节摘要、同一分配器、相同平台／处理器和全部 17 项数据；不平均抵消未达标项。

用下列历史快照检查程序，无需执行历史二进制：

```sh
python benchmarks/evaluate_saved.py --partial \
  --reports benchmarks/results/1.0.11-multi-address-basic/results.json \
            benchmarks/results/1.0.11-multi-address-features/results.json \
            benchmarks/results/1.0.11-multi-address-strings/results.json \
  --out tmp/historical-performance-gate
```

这份历史数据在已设条件的 13 项中通过 4 项，另有 4 项未设门限；程序返回 1，表示仍有未达标项。参数／数据错误返回 2。只有至少存在一个可判定项目且所有已选条件满足才返回 0；无比较依据的项目不计为通过。输出目录必须尚不存在，防止覆盖证据。

最终候选使用同一最终编译器产生的三组正式结果，并加 `--compiler 编译器路径` 核对真实字节，省略 `--partial`。`--allow-mixed-compilers` 只供明确标注的历史分析，不产生完整候选标记。输出保存工具和输入 SHA256、实际源码提交、样本数、逐项结论及未设门限项目。

性能判定不能代替功能回归、最终文档／资产构建和全量 CI。`full_ci_executed` 始终为 false，表示工具没有执行 CI；仍须按 TASK.md 完成最终门禁及发行产物核对。
