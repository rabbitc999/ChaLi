# 跨语言微基准（20261029-123026）

中位数（ms，越小越快），每格来自 5 次进程 × 5 轮。

| 配置 | 各语言均从空容器动态增长，再遍历归并 |
| --- | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 74.883 |

相对 ChaLi 1.0.11（LLVM AOT，unchecked 导出） 的倍数（同一列除以基线中位数）：

| 配置 | 各语言均从空容器动态增长，再遍历归并 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） |
| --- | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 1.00 |
