# 跨语言微基准（20261028-233521）

中位数（ms，越小越快），每格来自 5 次进程 × 5 轮。

| 配置 | Map 空表增长、覆盖、查询、删除与重新插入 | Set 空表增长、重复插入、查询、删除与重新插入 |
| --- | ---: | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 159.625 | 121.402 |

相对 ChaLi 1.0.11（LLVM AOT，unchecked 导出） 的倍数（同一列除以基线中位数）：

| 配置 | Map 空表增长、覆盖、查询、删除与重新插入 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | Set 空表增长、重复插入、查询、删除与重新插入 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） |
| --- | ---: | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 1.00 | 1.00 |
