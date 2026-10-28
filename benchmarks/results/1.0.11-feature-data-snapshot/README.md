# 纯循环数据区快照：仅测 ChaLi

实际编译器源码 2609cf0，SHA256 4c93c6af8222b7ff382517ef589378499f5032bb8a73ee8b3d88c97bb147c4fd；工作区干净。系统 malloc，unchecked LLVM AOT，Windows x64；每格五进程各五轮，100 样本校验通过，本轮全部源码及计时壳摘要与冻结参考一致。

| 负载 | ChaLi / ms | C++ O2（复用）/ ms | Rust（复用）/ ms | O2 门槛 |
| --- | ---: | ---: | ---: | --- | 
| enum_payload | 0.8615 | 1.6884 | 1.214 | 通过 |
| struct_nested | 1.7515 | 1.1711 | 1.0543 | 未通过 |
| array_2d | 0.691 | 0.5668 | 0.5433 | 未通过 |
| array_3d | 0.7627 | 0.6806 | 0.6277 | 未通过 |

C++ O2 来自 ../1.0.11-cpp-o2-reference/results.json；Rust 来自 ../1.0.11-feature-baseline/results.json。保留原参考的真实配置、版本、时间及样本，未重跑任何参考语言。上轮 ../1.0.11-feature-reference-storage-children/results.json 的枚举、结构体、二维、三维分别为 1.4938、3.5490、0.7752、0.8885 ms。

枚举达到 O2 门槛，也快于保存的 Rust 参考；其余三项仍未达标。没有对应 Lua 特征负载，不虚构 Lua 比较。物理视图减少数据区重复解析，跨时段差值仍可能包含系统波动；不把局部达标写成整体任务完成。
