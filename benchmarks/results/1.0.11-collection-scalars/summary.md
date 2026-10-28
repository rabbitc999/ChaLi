# 跨语言微基准（20261028-155014）

中位数（ms，越小越快），每格来自 5 次进程 × 5 轮。

| 配置 | 整数依赖链（Lehmer 乘加） | 浮点依赖链（乘加） | 小函数调用 | 固定数组循环读取 | 各语言均从空容器动态增长，再遍历归并 | Map 空表增长、覆盖、查询、删除与重新插入 | Set 空表增长、重复插入、查询、删除与重新插入 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 0.865 | 1.301 | 0.216 | 13.979 | 318.770 | 219.212 | 125.642 |

相对 ChaLi 1.0.11（LLVM AOT，unchecked 导出） 的倍数（同一列除以基线中位数）：

| 配置 | 整数依赖链（Lehmer 乘加） ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 浮点依赖链（乘加） ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 小函数调用 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 固定数组循环读取 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 各语言均从空容器动态增长，再遍历归并 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | Map 空表增长、覆盖、查询、删除与重新插入 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） | Set 空表增长、重复插入、查询、删除与重新插入 ÷ ChaLi 1.0.11（LLVM AOT，unchecked 导出） |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ChaLi 1.0.11（LLVM AOT，unchecked 导出） | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 |
