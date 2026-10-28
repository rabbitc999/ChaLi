# 接口描述符标量读取：仅测 ChaLi

编译器源码 fa45b73，SHA256 bdcc1398f4ec24b9ff3254e1f3d7b468679fea950d5478827185d6152a3801bc；测量工作区干净。系统 malloc、unchecked LLVM AOT、Windows x64，每格五进程各五轮，50 样本均通过模型校验。本轮全部源码／计时壳摘要逐项匹配冻结参考。

| 负载 | ChaLi / ms | 上轮 ChaLi / ms | C++ O2（复用）/ ms | Java JIT（复用）/ ms | O2 门槛 |
| --- | ---: | ---: | ---: | ---: | --- |
| interface_mono | 254.2797 | 514.8049 | 0.2825 | 0.2845 | 未通过 |
| interface_poly | 999.2430 | 1268.6220 | 0.5095 | 1.0682 | 未通过 |

C++ O2 来自 ../1.0.11-cpp-o2-reference/results.json；Java 来自 ../1.0.11-feature-baseline/results.json，保留首轮 HotSpot 热身和 C2 日志证据，未执行 Java 或 C++。原始版本、配置、时间与摘要均保留原参考。上轮为 ../1.0.11-feature-reference-storage-children/results.json。

直接标量入口降低通用封送开销，但接口描述符复制／拥有值处理仍走运行时对象表。相对上轮单态约减少 50.6% 耗时、多态约减少 21.2%；仍远未达到 O2 或 Java 稳态性能，不能宣称完成。不同时间段样本可能受系统波动影响，不把全部差值都当作单一改动的因果效应。本轮没有其他负载数据，也没有对应 Lua 接口负载，不虚构比较。
