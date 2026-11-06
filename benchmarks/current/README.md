# 当前跨语言性能测试 / Current cross-language measurements

统一入口为 `run.py`，包含 18 个项目：整数、浮点、小函数、数组读与读写、List/Map/Set 增长、载荷枚举、单态与多态接口、嵌套结构、二维与三维数组、UTF-8 字节读取、拼接、切片、查找。所有项目分别运行 ChaLi、C、C++、Rust、Java、Lua、Python、Go。

从空容器增长的 `list_growth` 替代旧的预分配策略不一致的 `list` 对照；不混合两种操作配置。原有 17 项负载全部覆盖，并补充固定数组读写。

```powershell
python benchmarks/current/run.py --chali path/to/chali.exe --compiler-source-commit COMMIT --out build/validation/performance-current/run --build-only
python benchmarks/current/run.py --chali path/to/chali.exe --compiler-source-commit COMMIT --out build/validation/performance-current/run --measure-only
```

第一阶段构建全部产物并逐种子核对结果；第二阶段串行运行。默认每种语言、每个项目运行 5 个独立进程，每进程先预热，再记录种子 0～4 的 5 个区间，共 3600 个正式观测。Java 每进程预热至少 32 次且至少 1 秒，保留预热次数与耗时。不把预热本身当成编译层级已稳定的证明。进程启动、构建、全局输入初始化和预热不计入工作区间。

ChaLi 使用 `build --shared --unchecked --allocator malloc`，默认 LLVM O3/LTO。C/C++ 使用 GCC O3/LTO；Windows 优先使用已有静态 LLVM SDK 的 GCC，并显式传入 sysroot。Rust 使用 `opt-level=3`、fat LTO 与一个代码生成单元。Go 使用默认优化，Windows 计时直接调用 QPC，避免短区间出现粗粒度零读数；Java 使用默认分层编译、`-Xbatch` 与默认 GC；Python、Lua 使用默认解释器及内存管理。

Lua 从官方 `lua/lua` 仓库的 `v5.5.1` 获取，固定提交 `7579fc9d7ed90240487251dfb69168f8e64e9294`，校验提交和工作区后用相同 GCC O3/LTO 构建。官方发布归档及校验值另见 [Lua 下载页](https://www.lua.org/ftp/)。Lua 宿主只提供单调纳秒时钟，保留其默认 GC。

所有语言使用相同规模、初值、操作顺序与结果归并。容器采用各语言常用表示：C 使用显式开放寻址参考，C++/Rust 使用标准容器，Go 使用 slice/map，Java 使用含 Long 装箱的 ArrayList/HashMap/HashSet，Python 使用 list/dict/set，Lua 使用 table。载荷和接口以各语言具备的结构或方法表示；C 使用标签和函数指针，Rust 使用 enum/dyn trait，Lua/Python 使用动态对象。允许编译器内联、去虚化及其他正常优化。

字符串组是 **UTF-8 字节任务**。ChaLi/C++/Rust/Lua 使用各自字符串表示；Python 使用 bytes，Go 和 Java 使用字节序列，切片显式生成拥有副本，拼接逐次构造完整结果。Java 字节查找为显式循环；其他语言可以使用标准库查找。不能据此推断所有语言的 Unicode 字符串 API 性能或把库算法差异归因于语言本身。

整数结果逐种子完全一致；浮点结果按相对与绝对误差 `1e-12` 核对。只有非零退出码、非法记录或计算结果不一致会使测试停止，不按速度剔除观测，不复用历史样本。每轮用固定随机种子改变项目和语言顺序，保存全部样本、中位数、最小值、最大值、执行顺序、工具版本、构件及源码 SHA-256。

原始结果仅保存到忽略的 `build/validation/`，构建与运行临时文件位于根目录 `tmp/`。交付前先保存需要的样本、源码快照和构件，再清理 `tmp/`。结果文档见 [中文](../../doc/12-performance.md) / [English](../../doc/en/12-performance.md)。

Run all builds and seed checks first, then serial measurements. Every one of the 18 workloads is executed in all eight languages. Five processes and five samples per process produce 3600 timed observations. Java warmup is recorded separately. Reports retain all valid samples, exact commands, output checks, versions and SHA-256 identities. Results describe the specified implementations and machine; they do not assign performance pass/fail labels or reuse older measurements.
