# ChaLi 1.0.6 自举容器 AOT 对照

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

同一新版编译器、默认 LLVM O3、完整四元式预算、反射及安全检查。每负载 1,024 次操作，三个实现均先构建完成再交错执行；每配置三次进程，每进程预热一次、计时五次，取 15 个内部样本中位数。计时包含创建、分配、操作和最终清理。每进程独立公式校验。

- native：默认容器。
- bootstrap：原 ChaLi 自举普通数组实现。
- triples：Map 为 `{ Value, HasValue, HashKey }` 单槽位数组；Set 为 `{ HashKey, HasValue }` 独立数组；List 为块指针表与 64 元素堆块。Map/Set 删除采用后移修复，List pop 保留已删除槽位和块至最终关闭。
- 实验 List 管理独立堆块并实现 Closeable，资源值的延迟释放是该实验的语义限制，尚不替换默认容器。性能负载仅 int64 键和值，不能推及其他元素类型。

原设计、GitHub 参考及 1.0.5 的独立数据见 [历史报告](../slot-segmented-1.0.5/README.md)。本轮重新测量所有实现，不把旧结果重标。当前原始构建、源码 SHA256、输出、执行顺序和纳秒样本见 [results.json](results.json)。

| 负载 | 默认 ms | 原自举 ms | 槽位／分块 ms | 原自举／新实验 |
| --- | ---: | ---: | ---: | ---: |
| list_build | 1.673 | 585.457 | 8.804 | 66.50× |
| list_read | 1.671 | 574.842 | 8.674 | 66.27× |
| list_write | 2.623 | 1332.255 | 12.515 | 106.45× |
| list_remove | 1.669 | 1635.717 | 8.486 | 192.76× |
| map_build | 2.017 | 4.717 | 6.343 | 0.74× |
| map_read | 2.055 | 4.707 | 6.335 | 0.74× |
| map_write | 3.026 | 5.308 | 6.818 | 0.78× |
| map_remove | 2.925 | 167.190 | 317.268 | 0.53× |
| set_build | 1.981 | 2.563 | 3.473 | 0.74× |
| set_read | 1.946 | 2.575 | 3.494 | 0.74× |
| set_write | 1.080 | 2.257 | 3.402 | 0.66× |
| set_remove | 2.952 | 161.269 | 290.870 | 0.55× |

List 同时改变布局、拥有关系和 pop 路径，Map/Set 同时改变布局和删除策略，本轮没有逐项分离归因。比值大于 1 表示新实验更快；小于 1 表示更慢。组合容器 VM/AOT 正确性见 [最终验证记录](../backend-1.0.6/validation/backend-containers-correctness-final.log)。
