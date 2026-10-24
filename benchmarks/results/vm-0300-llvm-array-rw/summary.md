# 跨语言微基准（20261024-143720）

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](summary.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

中位数（ms，越小越快），每格来自 3 次进程 × 5 轮。

| 配置 | 固定数组读写 |
| --- | ---: |
| ChaLi 0.30.0（StackVM，默认后端） | 1548.167 |
| ChaLi 0.30.0（LLVM AOT，默认不检查） | 7.231 |
| C（gcc -O2） | 8.669 |
| Lua 5.5.1（官方 VM，默认 GC） | 75.586 |

相对 C 的倍数（同一列除以基线中位数）：

| 配置 | 固定数组读写 ÷ C |
| --- | ---: |
| ChaLi 0.30.0（StackVM，默认后端） | 178.6 |
| ChaLi 0.30.0（LLVM AOT，默认不检查） | 0.8 |
| C（gcc -O2） | 1.00 |
| Lua 5.5.1（官方 VM，默认 GC） | 8.7 |
