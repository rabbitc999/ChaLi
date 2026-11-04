# LSP 最终对照数据（1.0.12）

本目录保存最终对照数据。`results.json` 的 baseline 对应提交 `d7c4be79`，current 对应提交 `f1ba443d`。两者均为 Windows x64 静态 LLVM 1.0.12，按实际 EXE SHA-256 区分。被测字节保存在本地 `build/independent-validation/` 的对应交付目录。

测试程序和完整测量方法见 [benchmark_lsp.py](../../../tools/benchmark_lsp.py)：200 个数值局部、一个卫生宏和局部类型推断；每份编译器使用新的进程与空 LSP 缓存，不清空操作系统文件缓存。启动和首次诊断各一次，编辑诊断、悬停和补全各 20 个样本；P50 为中位数，P95 为最近秩。计时包含 JSON-RPC 客户端往返，编辑诊断包含防抖。CPU 在各测量组前后采样，本轮为 0%～11%，均低于 40%。

并发语义检查由 3 次降为 1 次。编辑诊断 P50 为 2225.728／2230.992 ms，首次诊断为 2240.994／2281.769 ms，本轮没有明显改善这两项。悬停 P50 为 2.837／2.462 ms，补全为 4.416／4.000 ms；这两条主要沿用已有快速路径，仅凭这组样本不能将差异全部归因于语义合并。数据覆盖这个固定小程序，不代表大型项目或所有硬件。其他语言运行基线没有重测。

## Reproduce

```console
python tools/benchmark_lsp.py --baseline PATH_TO_P3_COMPILER --chali PATH_TO_P5_COMPILER --samples 20 --out tmp/lsp-comparison.json
```

The report records exact compiler hashes, actual build information, host environment, CPU samples, and all final latency samples. Timing is deferred when CPU is at least 40% or its counter is unavailable; partial timing numbers are discarded. Functional tests remain separate. This dataset does not replace or relabel the frozen language-runtime benchmarks.
