# 语言特征性能基线

本组负载按用户要求建立一次其他语言参考，随后默认只复测 ChaLi。它们补充整数、浮点、数组、List、Map、Set 基线，不覆盖或重标旧测量。

| 负载 | 工作内容 | 首次参考 |
| --- | --- | --- |
| enum_payload | 四种载荷变体的借用匹配：无载荷、整数、整数对、嵌套结构体 | Rust 原生 enum／match |
| interface_mono | 派生接口借用视图调用继承方法，单一实现类型 | Java HotSpot JIT |
| interface_poly | 同一派生接口调用位置接收三种实现，比例 2:1:1 | Java HotSpot JIT |
| struct_nested | 256 个记录，嵌套坐标／速度、不同长度字段数组及跨层读取 | C++ O3、Rust -O |
| array_2d | 原生 `64 × 64` 二维数组的依赖索引读取 | C++ O3、Rust -O |
| array_3d | 原生 `16 × 16 × 16` 三维数组的跨层依赖索引读取 | C++ O3、Rust -O |

每次工作函数执行 262144 轮，输入种子 0–4；每格 5 个独立进程，每进程计时 5 轮。设置数据、创建对象／载荷、初始化 ChaLi 模块、预热、校验归并和进程启动均不计时。C 壳在独立翻译单元使用 volatile 函数入口，Rust 在工作函数外使用 [`black_box`](https://doc.rust-lang.org/std/hint/fn.black_box.html) 并保持函数入口，Java 使用结果消费和实际循环依赖，避免删除未使用的计算。内层没有人为插入的内存屏障或关闭优化开关。

ChaLi 使用系统 malloc、unchecked LLVM AOT、O3/LTO。C++ 使用 C++17、O3，Rust 使用 edition 2021、-O。数组保持各语言原生多维类型，不手工展平；枚举使用各语言自然表示，不强制相同 ABI。嵌套数组、结构体或枚举的实际表示、边界检查和访问成本均属于测量对象。结果只说明这些具体负载，不能由此概括整个语言。

接口负载声明父接口与派生接口，通过派生接口调用继承的方法。ChaLi 目前不将派生接口视图隐式转换为父接口形参，因此双方保持相同的派生接口形参，不把未支持的隐式上转型当作计时内容，也不为基准修改前端规则。单态和三类型多态分开测量，允许两种优化器按正常规则内联或去虚拟化。

Java 比较热身后的稳态：保留默认分层编译，使用 [`-Xbatch`](https://docs.oracle.com/en/java/javase/22/docs/specs/man/java.html) 等待前台编译完成。每个独立 JVM 至少热身 256 次且至少一秒，再测量 5 轮；各 JVM 保存 JIT 日志并要求工作函数的 level 4 编译在热身结束前出现。初始化与启动成本不混入结果。计时样本和热身轮数全部保留，未以“最快一次”代替中位数。

首先只进行无计时的逐种子语义核对：

```powershell
python benchmarks/features/run.py --preflight --out tmp/features-preflight
```

预检构建全部实现，并以独立整数模型核对每个种子的完整结果；枚举选择包含结果高位，模型保存四种变体的实际访问分布并拒绝退化为单一变体的负载。预检不生成 `rep` 样本，不构成重复性能评测。

首次正式建立参考：

```powershell
python benchmarks/features/run.py --establish-baseline --runs 5 --out benchmarks/results/1.0.11-feature-baseline
```

后续只测 ChaLi，保持其他语言原始版本与样本：

```powershell
python benchmarks/features/run.py --reference benchmarks/results/1.0.11-feature-baseline/results.json --runs 5 --out benchmarks/results/feature-followup
```

后续命令不会启动 Java、Rust 或 C++ 编译／评测；先核对工作负载与计时壳摘要及模型结果，再记录新 ChaLi 原始样本和旧参考来源。源负载发生语义变化时停止复用，单独说明变化及受影响项。所有构建目录位于仓库 `tmp/`，各子进程继承相同的 TMPDIR、TMP、TEMP，完成后按仓库约定保存证据并运行 `tools/clean_tmp.ps1`。

Python 只调度正式工具、运行整数模型和记录样本；所有 ChaLi LLVM IR 都由 C++ 编译器直接生成。
