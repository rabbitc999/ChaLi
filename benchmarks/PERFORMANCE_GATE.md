# 已保存样本的性能验收

`evaluate_saved.py` 不编译、不重新计时，也不运行参考语言。它读取已保存的原始纳秒样本，以当前条件逐项判定：ChaLi 默认 LLVM O3 的耗时不超过 C++ O2 的 **2 倍**；有冻结 Lua 参考时还必须严格更快。Lua 拼接取逐次拼接和 `table.concat` 中较快的参考。没有 Lua 参考的特征项目明确留空，不宣称快于 Lua。

当前上限按原 1.20 倍上限 ÷ 1.2 × 2 直接换算；只重新判定保存的数据，不重新测试。

原始报告及冻结驱动里的历史 1.05／1.20 标签不改写。工具使用独立保存的 C++ O2 参考，不把驱动里的 C++ O3 列当作当前门槛。历史版本、参数、原始样本和参考出处均保留。

正式输入必须为五进程、各五样本。工具重算中位数，核对冻结源码、驱动及计时壳摘要，拒绝失败进程、不足或损坏的样本、重复负载、未完成项及不干净工作区的测量。默认要求同一编译器字节摘要、同一分配器、相同测量平台／处理器和全部 17 项负载；不计算抵消慢项的平均成绩。十进制计算两倍边界，避免二进制浮点乘法误拒绝恰好位于门槛的项目。

用下列历史快照检查程序本身，无需历史二进制，也不会重跑 Lua、Rust、Java 或 C++：

```sh
python benchmarks/evaluate_saved.py --partial \
  --reports benchmarks/results/1.0.11-multi-address-basic/results.json \
            benchmarks/results/1.0.11-multi-address-features/results.json \
            benchmarks/results/1.0.11-multi-address-strings/results.json \
  --out tmp/historical-performance-gate
```

这份历史数据的 C++ 条件通过数为 7/17，程序返回 **1**，表示仍有未达标项。参数／数据错误返回 **2**，只有所有已选项目满足性能条件才返回 **0**。输出目录必须尚不存在，防止覆盖证据。

最终候选使用同一最终编译器产生的三组正式结果，并加 `--compiler 编译器路径` 核对真实字节；省略 `--partial`。完整候选标记还要求全部项目和一致配置。`--allow-mixed-compilers` 只供明确标注的历史分析，不产生完整候选标记。工具同时保存自身及输入文件的 SHA256、实际编译器源码提交、样本数和逐项结论。

性能数据判定不能代替功能回归、最终文档／资产构建和仓库全量 CI。输出中的 `full_ci_executed` 始终为 false，表示本工具没有执行 CI。最后仍须按照 TASK.md 完成全量门禁及发行产物核对。
