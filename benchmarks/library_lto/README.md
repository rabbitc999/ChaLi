# 预编译库 LTO 冻结对照

[English](README.en.md) · **简体中文**

本目录只比较同一 ChaLi 静态 LLVM 编译器的库实现与 LTO 配置，不重测或重标其他语言及旧版本基线。当前测量结果、身份与限制见[库包说明](../../doc/44-library-packages.md)。

- [base.chl](base.chl) 提供单步整数依赖链与每次 256 步的批量函数；[bridge.chl](bridge.chl) 构成第二层预编译库。
- [call.chl](call.chl) 跨库调用 1,048,576 次；[bulk.chl](bulk.chl) 跨库调用 4,096 次，每次执行 256 步。二者按相同顺序执行相同运算，五个种子的结果由 Python 独立模型核对。
- [timing.c](timing.c) 复用原有单调时钟壳，不修改旧计时源；C 宿主调用普通原生标量导出。模块初始化、预热、结果归并、输出和结束不计入内核时间。

[驱动](../../tools/benchmark_library_lto.py) 要求自包含的静态 LLVM 编译器。示例：

```sh
python tools/benchmark_library_lto.py --chali PATH_TO_STATIC_COMPILER --compiler-source-commit ACTUAL_COMPILER_SOURCE_COMMIT --out build/lto-comparison-new
```

输出目录必须尚不存在，避免混合或覆盖历史记录。先用另一个输出目录加 `--verify-only` 可只运行功能核对。`--compiler-source-commit` 必须填写该编译器实际构建源码身份，不自动当作当前 HEAD。

驱动先复制并核对编译器、负载、计时壳和脚本，构建两层双实现库并删除工作区中的提供方源码。全部十个配置的 AOT、独立链接产物及冻结副本逐种子核对通过后，才保存 `baseline.json` 并正式计时。原始命令、输出、文件摘要、CPU 检查和样本写入本地 `results.json`；运行二进制及最后一轮构建产物留在同一交付目录，不提交 Git。

五种配置为原生／off、原生／full、bitcode／off、bitcode／full、bitcode／thin，统一使用 unchecked、O3、malloc、512 MiB 堆预算。所有组合先做功能预热，不在正式样本之间清理编译器及标准库缓存；串行执行并按固定随机种子轮换配置顺序。每次计时命令前在区间外读取 CPU，达到 40% 或读数不可用则等待，超出等待预算则保存 `deferred`，不声称测量完成。频率和亲和性不固定，CPU 记录只描述启动检查。

构建默认各三轮：原生消费对象编译、该对象与所选库的独立链接、完整 AOT 分别计时。独立链接的消费对象已经是原生机器码，完整 AOT 则可优化主单元；前两项之和不等于完整 AOT。运行默认每配置五个独立进程，每进程预热一次后测五个种子，共 25 个内核样本，统计中位数。`--build-repetitions`、`--runtime-processes` 可增加样本数，但不能低于三轮。

文档定稿后的新编译器与 VSIX 另做功能验收，不把冻结编译器的实测结果改标为新产物的数据。本目录不能证明所有程序、平台或 LLVM 变体都会受益，也不提供跨语言排名。
