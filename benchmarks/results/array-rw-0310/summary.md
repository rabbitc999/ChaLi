# 跨语言微基准（20261024-200355）

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](summary.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

中位数（ms，越小越快），每格来自 3 次进程 × 5 轮。

| 配置 | 固定数组读写 |
| --- | ---: |
| ChaLi 0.31.0（StackVM，默认后端） | 625.920 |
| ChaLi 0.31.0（LLVM AOT，默认检查） | 156.419 |
| C（gcc -O2） | 8.656 |
| Lua 5.5.1（官方 VM，默认 GC） | 71.044 |

相对 C 的倍数（同一列除以基线中位数）：

| 配置 | 固定数组读写 ÷ C |
| --- | ---: |
| ChaLi 0.31.0（StackVM，默认后端） | 72.3 |
| ChaLi 0.31.0（LLVM AOT，默认检查） | 18.1 |
| C（gcc -O2） | 1.00 |
| Lua 5.5.1（官方 VM，默认 GC） | 8.2 |
