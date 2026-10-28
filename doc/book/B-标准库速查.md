# 附录 B 标准库速查

<!-- LANGUAGE NAVIGATION -->
[English](en/B-standard-library.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 的「标准库」不是一堆函数，而是一组**模块**：除核心 `std::lang` 自动加载外，
其余都得先 `import`。这份附录按**能力域**把它们列一遍，让你知道「想干这件事，
该去 `import` 谁」；每个模块的完整签名与错误边界，去[库函数 API 参考](../library/README.md)查。

终端中可以直接用 `chali man` 查模块，用 `chali man 模块::符号` 查签名、说明和示例；
命令说明见[命令行手册](../man/README.md)。本附录按当前 API 索引维护，不替代逐项接口参考。

---

## B.1 先看一个例子

<!-- verify-book -->
```chali
import std::math;
import std::text;
import std::iter;

void main()
{
    println("sqrt={}", std::math::sqrt(144.0));
    println("upper={}", std::text::Text::toUpper("chali"));
    int64 sum = 0;
    for (int64 v in std::iter::range(1, 5))
    {
        sum += v;
    }
    println("sum={}", sum);
}
```

```text
sqrt=12
upper=CHALI
sum=10
```

三行分别来自三个模块：数学、文本、迭代。注意 `range(1, 5)` 是**左闭右开**
（拿到 1、2、3、4），所以 `sum` 是 10。其他 API 的端点、索引和失败方式按各自参考页核对。

---

## B.2 核心模块 `std::lang`（不用 import）

这几样任何程序里都能直接用：

| 能力 | 用法 |
| --- | --- |
| 输出 | `println(fmt, …)`、`format(fmt, …)` |
| 断言 | `assert(cond)` |
| 取长度 | `len(x)`（字符串 / 容器） |
| 字符串 | `std::lang::String` |
| 容器 | `List<T>`、`Map<K, V>`、`Set<T>`、`Pair<K, V>` |
| 可选值与错误 | `Option<T>`、`Result<T, E>`；标准库错误实现 `Error.toString()`，文本错误使用 `MessageError`，配合模式匹配、查询和取值方法 |
| 类型擦除盒子 | `Any`（第 13 章的运行期多态） |
| 数学常数 | `Math::PI`、`Math::E` |
| 大数 | `BigInteger`、`BigDecimal` |
| 输入 | `inputInt()`、`inputUInt()`、`inputFloat()`、`inputBool()`、`inputChar()`、`inputBigInteger()`、`inputBigDecimal()` |
| 整行输入 | `readLine()`，保留空格，不含 LF/CRLF |
| 多行输入 | `readLines(int32 N)`，LF 连接，EOF 提前结束 |

完整类型、接口与内建入口见[核心库参考](../library/std-lang.md)。其余模块按需显式 `import`。

---

## B.3 按能力域速查

### 数学与数值

| 模块 | 能干什么 |
| --- | --- |
| [`std::math`](../library/std-math.md) | `sin` `cos` `sqrt` 等常用函数；`abs` `min` `max` 是泛型入口，`min`/`max` 可以给任意多个实参（常数见 `std::lang::Math`） |
| [`std::numerics`](../library/std-numerics.md) | 标量数值方法：导数、辛普森积分、二分、RK4、牛顿迭代 |
| [`std::numtheory`](../library/std-numtheory.md) | 整数数论：模块级泛型 `gcd` `lcm`（任意多个实参）、`addMod` `mulMod` `powMod` `modInverse`，以及确定性的 `isPrime`（Miller-Rabin） |
| [`std::complex`](../library/std-complex.md) | 复数 `Complex<T>`，加减乘除都是一等公民 |
| [`std::linalg`](../library/std-linalg.md) | 稠密向量与矩阵：`luDecompose` `symmetricEigen` `svd` `jordan2` `diagonalize2` |
| [`std::random`](../library/std-random.md) | 确定性随机数（LCG）与熵播种；句柄型状态，实现 `Drop` |
| [`std::signal`](../library/std-signal.md) | DFT / FFT 与逆变换；基 2 的 FFT 只接受 2 的幂长度 |
| [`std::cas`](../library/std-cas.md) | `Expr` 精确符号表达式：解析、节点遍历、化简、求导、积分、替换、极限和 Taylor 展开 |

### 文本与数据格式

| 模块 | 能干什么 |
| --- | --- |
| [`std::text`](../library/std-text.md) | `Text::find`、`split`、`join`、`replace`、`trim`、`toUpper`、`padStart` 及字节级 KMP |
| [`std::utf8`](../library/std-utf8.md) | UTF-8 有效性检查、码点解码与编码、按码点计数和截取 |
| [`std::encoding`](../library/std-encoding.md) | 十六进制、Base64 和 URL 百分号编码与解码 |
| [`std::csv`](../library/std-csv.md) | CSV 解析与写出，处理字段分隔、引号和换行 |
| [`std::json`](../library/std-json.md) | `parse` → `Result<JsonValue, MessageError>`；再 `isInt` `isObject` `len` `asInt` `asString` 取值 |
| [`std::toml`](../library/std-toml.md) | `TomlValue` 文档访问、解析和保留格式的转储，以及反射编解码 |
| [`std::regex`](../library/std-regex.md) | `Regex::compile`，整串匹配、包含、查找、捕获和替换；编译失败返回 `RegexError` |
| [`std::parse`](../library/std-parse.md) | 字符串 → 整数，失败给 `ParseError` |
| [`std::binary`](../library/std-binary.md) | 定宽解码、字节序处理与边界检查 |
| [`std::path`](../library/std-path.md) | 路径拼接、取文件名、取扩展名、规范化 |
| [`std::compress`](../library/std-compress.md) | 文本压缩 / 解压；压缩结果是**二进制** `List<uint8>` |

### 数据结构、通用算法与图像

| 模块 | 能干什么 |
| --- | --- |
| [`std::collections`](../library/std-collections.md) | `Deque`、`Queue`、当前上下文的 `Channel`、`FixedVector` 和 `RingBuffer` |
| [`std::linkedlist`](../library/std-linkedlist.md) | 双向链表，头尾操作、插入、删除、查找和遍历 |
| [`std::tree`](../library/std-tree.md) | 二叉搜索树、红黑树、最小堆和普通树；删除、遍历、校验与快照 |
| [`std::graph`](../library/std-graph.md) | `Graph`、`UnionFind`，BFS、DFS、拓扑排序、最短路、环检测和最小生成树 |
| [`std::algorithms`](../library/std-algorithms.md) | 排序、查找、折叠、映射、过滤、计数和聚合；回调遵循函数值的借用与生命周期规则 |
| [`std::image`](../library/std-image.md) | `Image`、RGBA 像素，BMP/PGM/PPM 读写、颜色转换、卷积、池化和大津法二值化 |

`std::collections::Channel` 是当前执行上下文的值队列。跨线程传递使用
`std::concurrency::Channel<T>` 或 `std::message`，两类队列的共享语义不同。

### 反射与模型编解码

| 模块 | 能干什么 |
| --- | --- |
| [`std::reflect`](../library/std-reflect.md) | 查询类型、字段、方法、注解和稳定类型 ID；受反射可见性约束的读写与动态调用 |
| [`std::serialization`](../library/std-serialization.md) | `serialize<T>`、`deserialize<T>`，字节流保存版本、稳定类型 ID 和长度，恢复时严格检查模型类型 |

JSON/TOML 的模型编解码见对应格式模块，数据库模型与动态 SQL 见 `std::sql`。
默认编译器同时编入前端与 VM 反射，源码编译默认启用反射；关闭本次元数据输出时显式使用 `--no-reflection`。

### 并发与协作

| 模块 | 能干什么 |
| --- | --- |
| [`std::thread`](../library/std-thread.md) | 隔离线程 `Thread`、取消、结果、线程组和固定线程池 |
| [`std::message`](../library/std-message.md) | 跨线程的消息 |
| [`std::concurrency`](../library/std-concurrency.md) | 有界跨线程 `Channel<T>`、显式共享槽 `Shared<T>` 和 `AtomicInt64`（普通全局变量仍隔离） |
| [`std::event`](../library/std-event.md) | 显式事件与定时源；通过 receive 接收，周期积压合并 |

> 线程执行、通道通信和显式事件分别由 std::thread、std::concurrency 和 std::event 提供。

### 系统与环境

| 模块 | 能干什么 |
| --- | --- |
| [`std::io`](../library/std-io.md) | 文件读写；错误类型 `IoError`（`OpenFailed` `ReadFailed` `WriteFailed` `SeekFailed` `Closed`） |
| [`std::net`](../library/std-net.md) | 独占的 `TcpSocket`、`TcpListener`，连接、监听、收发和 HTTP 客户端；HTTP 客户端不提供 TLS |
| [`std::sql`](../library/std-sql.md) | `Database`、参数化查询、事务与模型操作；连接 MySQL / SQLite / PostgreSQL / SQL Server |
| [`std::process`](../library/std-process.md) | `Process`，宿主进程创建、命令管道、等待与结果读取 |
| [`std::clock`](../library/std-clock.md) | 单调时钟 `steady_clock`，只用来做差值，别当挂钟用 |
| [`std::datetime`](../library/std-datetime.md) | `DateTime`、UTC 公历日期、Unix 秒、格式化和解析；不表示闰秒 |
| [`std::cli`](../library/std-cli.md) | 长短选项、组合短选项、位置参数、`--` 分隔、参数查询和帮助生成 |
| [`std::memory`](../library/std-memory.md) | `Span<T>` 借用视图与 `MemoryStats`；缓冲区用 `new` 分配，视图受底层存储有效期约束 |

### 迭代

| 模块 | 能干什么 |
| --- | --- |
| [`std::iter`](../library/std-iter.md) | `range`、`map` / `filter` / `collect` 惰性链，以及 `IterNext<T>` 接口 |

[`std::probe`](../library/std-probe.md)用于最小宿主集成探针与验证，不作为应用功能模块。

---

## B.4 三条容易踩的约定

**一、资源按类型的生命周期管理。** 拥有型对象按 `Drop` 约定自动释放，提前释放使用类型提供的接口。
不能假定所有对象都有相同的 `close()` 或重复关闭行为。独占文件、连接和进程不能随意复制；
转移所有权写 `move value`。复制是否深复制、借用何时失效，查对应类型的参考页。

**二、压缩结果是二进制，不是文本。** `std::compress` 的输出含 `\0` 字节，类型是
`List<uint8>`；**别拿它当字符串打印**，解压出来的才是 `std::lang::String`。

**三、字节与码点分清。** `String` 的长度和切片端点按字节计算，切片可以截取多字节编码的一部分；
普通字符串下标仍要求 UTF-8 字符边界。按 Unicode 码点处理时使用显式码点接口或 `std::utf8`。

实际文件、网络、数据库和线程能力以运行期胶水、系统库及构建配置为准。
本表列出的是标准库的完整面貌；[AOT 入口](../38-aot.md)已覆盖全部条目，但个别能力仍受目标平台限制，编译时以实际诊断为准确认。

---

## B.5 细节去哪儿查

| 你想查 | 去哪 |
| --- | --- |
| 某个函数的确切签名、默认值、返回与错误 | [库函数 API 参考](../library/README.md) 与[全部函数索引](../library/symbols.md) |
| 按能力域通读一遍标准库 | [07 标准库概览](../07-stdlib-overview.md) |
| 标准库源码（每个模块头部都有能力说明） | [`source/std/`](../../source/std) 下的 `source/std/<名字>.chl` |
| 宿主侧绑定（`std::*` 的真正实现） | [`source/glue/`](../../source/glue) 下按功能组织的 `std_*` 模块 |
| 可直接运行的标准库样例 | [`example/samples/`](../../example/samples/) |

模块声明、宿主胶水和编译器注入实现按职责组织，文件名不必与模块名逐字对应；
查询具体实现来源时以 API 索引中的源码位置为准。

---

## B.6 下一步

标准库认完了，前面二十几章也读完了——接下来该自己写点东西了。

- 想先复习报错怎么读：翻回[附录 A](A-常见错误速查.md)；
- 想练习模型、编解码与参数化查询的组合：[反射编解码与动态 SQL](../33-reflection-serialization-sql.md)；
- 想把这门语言接到你自己的 C/C++ 代码上：[第 21 章 与 C 互操作](21-与C互操作.md)。
