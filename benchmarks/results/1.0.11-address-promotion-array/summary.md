# 跨语言微基准（20261029-083106）

中位数（ms，越小越快），每格来自 5 次进程 × 5 轮。

| 配置 | 固定数组循环读取 |
| --- | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 10.179 |

相对 ChaLi 1.0.11（LLVM AOT，unchecked 导出） 的倍数（同一列除以基线中位数）：

| 配置 | 固定数组循环读取 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） |
| --- | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 1.00 |
