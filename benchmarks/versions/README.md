# 1.0.10 与 1.0.11 的同负载版本对照

当前驱动在 Windows x64 上比较原始 1.0.10 发行编译器、当前 1.0.11，并只运行 ChaLi。负载与既有基础、特征和字符串基准一致；其他语言使用冻结数据。

三组配置为 1.0.10 默认内存池、1.0.11 显式内存池、1.0.11 显式 malloc。前两组比较相同分配器下的版本变化，第三组保留当前 malloc 基准。均为 unchecked LLVM O3，C 计时壳也使用同一个 clang O3；初始化和全局 setup 不计时，工作函数里的分配与清理计时。

先构建及核对结果，再执行五进程、各五样本的正式测量：

```powershell
python benchmarks/versions/run.py --build-only --new-source a522cf0 --out benchmarks/results/1.0.11-final
python benchmarks/versions/run.py --measure-only --out benchmarks/results/1.0.11-final
```

测量按负载逐项检查 CPU，达到 40% 则暂缓，可再次执行 `--measure-only` 继续。每项在三个配置之间按轮次固定轮换顺序，避免始终先运行同一版本；不宣称全程 CPU 均低于 40%。各配置的原始样本、校验值、命令、实际编译器及内核摘要保存在结果文件中。

1.0.10 原产物的内嵌 ABI 头使用 `ast.hpp` 短路径。驱动从 Git 的 `001cdd6` 提取同版本 AST 头，仅为该版补充 C++ 搜索路径；同时为两个版本显式指定同一 clang。没有修改发行编译器、标准库、运行时或生产 LLVM 生成代码。旧版无法编译或无法通过结果核对的负载标记不可用，不填入虚构耗时。

Python 仅生成验证用的导出包装和 C 计时壳、构建、核对及调度；生产 LLVM IR 仍由各版 C++ 后端生成。临时产物位于根目录 tmp，最终数据归档后统一清理。
