# List 连续数值槽测量

实际编译器源码 7b4e90c，SHA256 9b3f37c739ab19c0fbb94a4abaacbad03cdb01e50c2461d2529817c66e7aaf2d。LLVM O3、unchecked AOT，malloc。启动前 CPU 为 1%、11%、0%，均低于 40%，不代表全程负载监测。

冻结 list_growth 同算法、规模、种子和计时边界，只测 ChaLi：五进程各五样本，中位耗时 45.053 ms，结果校验通过。上一轮 4a0868f 为 69.951 ms，下降约 35.59%；冻结 Lua 为 11.722 ms，目前约为其 3.84 倍，未达标。其他语言未重测。

前次数据 ../1.0.11-numeric-list-inplace/results.json；Lua 参考 ../1.0.11-thread-state/results.json。本次仍逐元素调用原生存储入口，尚未实现 LLVM 直接读写。两次环境非隔离控制实验，不将全部差异归因于单一机制。命令与样本见 results.json，最终全量门禁未执行。
