# 07 标准库概览

<!-- LANGUAGE NAVIGATION -->
[English](en/07-stdlib-overview.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

逐项查询函数的声明、参数、默认值、返回值、错误边界和完整程序，见 [库函数 API 参考](library/README.md) 与 [全部函数索引](library/symbols.md)。

按能力域列出 ChaLi 自带的模块。除核心模块 `std::core` 自动加载外，其余都要 `import`。

标准库失败返回统一使用 `Result<T, Error>`，拥有接口载荷中仍保存具体错误。`IoError`、`ParseError`、`CsvError` 等保留枚举分支、位置和原始消息；文本错误以 `MessageError::from(text)` 构造后装入 Error。通用函数可接收 Error 值、`Error const&` 或使用 `<E: Error>` 约束。自定义 `Result<T, E>` 仍可保留具体 E；当 E 实现 Error 时，`?` 支持向返回 `Result<U, Error>` 的函数传播。

```chali
import std::math;
import std::json;
void main() {
    println("sin0={} sqrt16={}", sin(0.0), sqrt(16.0));
    match (parse("{\"a\": 1}")) {
        Result<JsonValue, Error>::Ok(doc) => {
            println("isObject={} len={}", doc.isObject(), doc.len());
        }
        Result<JsonValue, Error>::Err(e) => { println("err={}", e); }
    }
}
```

```text
sin0=0 sqrt16=4
isObject=1 len=1
```

---

## 1. 核心模块 `std::core`（自动加载）

可以直接写 `String`、`List<T>`，也可以写 `std::core::String`、`std::core::List<T>`。显式写 `import std::core;` 也是有效的。

无需 `import`，直接可用。

| 能力 | 例子 |
| --- | --- |
| 输出与格式化 | `println(fmt, …)`、`format(fmt, …)` |
| 断言 | `assert(cond)` |
| 长度 | `len(x)`（字符串 / 容器） |
| 字符串 | `std::core::String`，下标与切片按 UTF-8 字节 |
| 容器 | `List<T>`、`Map<K,V>`、`Set<T>` |
| 类型擦除盒子 | `std::core::Any`（运行期多态用） |
| 大数 | `BigInteger`、`BigDecimal` |

公开 opcode 类型为 `std::big_number::BigInteger` 和 `std::big_number::BigDecimal`。可用 `import std::big_number;` 显式声明依赖，语言前置环境也提供两种类型的短名和字面量。

---

## 2. 按能力域速查

### 数学与数值

| 模块 | 内容 |
| --- | --- |
| `std::math` | `sin` `cos` `sqrt` 等常用函数；`abs` `min` `max` 是泛型入口（`min`/`max` 任意数量实参），`clamp` 分 `int64`/`float64` 两份；常数 `std::math::PI`、`std::math::E` |
| `std::numerics` | 标量数值方法：导数、辛普森积分、二分、RK4、牛顿 |
| `std::number_theory` | 整数数论：模块级泛型 `gcd` `lcm`（任意数量实参）、`addMod` `mulMod` `powMod` `modInverse`、确定性 Miller-Rabin `isPrime` |
| `std::complex` | 复数 `Complex<T>`，支持加减乘除 trait |
| `std::linear_algebra` | 稠密向量与矩阵、`luDecompose` `symmetricEigen` `svd` `jordan2` `diagonalize2` |
| `std::random` | 确定性 LCG 与熵播种；句柄型状态，实现 `Drop` |
| `std::signal` | DFT / FFT / 逆变换；基 2 FFT 只接受 2 的幂 |

### 标准宏

导入 `std::macro` 即可使用这些表达式宏。每个实参求值一次，包括最终没有成为结果的实参。

| 宏 | 行为 |
| --- | --- |
| `$MIN(left, right)` | 选择较小的值。 |
| `$MAX(left, right)` | 选择较大的值。 |
| `$CLAMP(value, low, high)` | 将值限制在闭区间内。上下界颠倒时触发断言。 |
| `$IN_RANGE(value, low, high)` | 判断值是否在闭区间内。上下界颠倒时返回 false。 |
| `$BOX(value)` | 创建 Box，并从表达式推导元素类型。 |

```chali
import std::macro;

int64 calls = 0;
int64 next(int64 value) {
    calls++;
    return value;
}
void main() {
    println("{} {} {}", $std::macro::MIN(next(7), next(3)), $MAX(7, 3), $CLAMP(9, 0, 6));
    println("{}", calls);
}
```

```text
3 7 6
2
```

### 文本与数据格式

| 模块 | 内容 |
| --- | --- |
| `std::text` | `Text::find` `split` `join` `replace` `trim` `toUpper` `padStart` …；`kmpFind` `kmpFindAll` `kmpFailureTable` 是字节级 KMP |
| `std::json` | `parse` → `Result<JsonValue, Error>`；`isInt` `isObject` `len` `asInt` `asString` … |
| `std::toml` | 与 json 同构的 TOML 文档访问 |
| `std::regex` | 模式在宿主侧按串缓存，ChaLi 侧只持整数句柄 |
| `std::parse` | 字符串 → 整数的解析，带 `ParseError` |
| `std::binary` | 定宽解码、字节序处理与边界检查 |
| `std::path` | 路径拼接、文件名、扩展名、规范化 |
| `std::compress` | 文本压缩 / 解压；压缩结果是**二进制** `List<uint8>`，不要当字符串打印 |
| `std::image` | RGBA 图片的加载 / 保存（BMP、PPM、PGM）、像素读写、卷积、池化、灰度 / ARGB8888 / RGB565 转换与大津法二值化 |

### 数据结构与算法

| 模块 | 内容 |
| --- | --- |
| `std::array` | ChaLi 实现的 `DynamicArray<T>`：连续存储、自动扩容、`reserve`、`resize`、`capacity`、安全读取与 `set` |
| `std::tree` | `BinarySearchTree`（增删查、三种遍历、min/max/height）、`RedBlackTree`（插入 + 双红修复）、`MinHeap`（可当优先队列）、`GeneralTree`（首个孩子 / 下一兄弟，可与二叉表示互转） |
| `std::graph` | `Graph` 链式前向星：`bfs` `dfs` `connectedComponents` `topologicalSort` `dijkstra` `hasCycle` `minimumSpanningTreeWeight`；`UnionFind` 路径压缩 + 按大小合并 |
| `std::linked_list` | `LinkedList` 双向链表：首尾与游标处 O(1) 插删、`reverse` `toArray` `fromArray` `find`，实现 `Iterator<int64>` 与 `Drop` |

这三个模块的全部算法都在宿主胶水（`glue/std_tree.hpp`、`glue/std_graph.hpp`、`glue/std_linkedlist.hpp`）里，`.chl` 只做公开 API 壳；`std::text` 的 KMP 同理（`glue/std_kmp.hpp`）。裁剪宏分别是 `NO_CHALI_STD_TREE` / `NO_CHALI_STD_GRAPH` / `NO_CHALI_STD_LINKEDLIST` / `NO_CHALI_STD_KMP`。

### 并发与协作

| 模块 | 内容 |
| --- | --- |
| `std::thread` | 隔离线程 `Thread` |
| `std::message` | 跨线程消息 |
| `std::concurrency` | 共享槽 `Shared<T>`、跨线程强引用 `Arc<T>` 和弱引用 `ArcWeak<T>`，普通全局变量在线程间隔离 |
| `std::event` | 显式事件与定时源；通过 receive 接收，周期积压合并 |

> 线程执行、通道通信和显式事件分别由 std::thread、std::concurrency 和 std::event 提供。

`AtomicInt64.compareExchange(expected, desired)` 在一次原子操作内比较当前值，相等时写入新值并返回 `true`。失败时返回 `false`，共享槽保留原值。

### 系统与环境

| 模块 | 内容 |
| --- | --- |
| `std::io` | 文件读写，错误类型 `IoError`（`OpenFailed` `ReadFailed` `WriteFailed` `SeekFailed` `Closed`） |
| `std::net` | TCP 与 HTTP 客户端；通过类型化套接字与监听器接口使用宿主网络服务 |
| `std::sql` | 连接 MySQL / SQLite / PostgreSQL / SQL Server；SQL 交给数据库引擎，ChaLi 负责收发 |
| `std::process` | 进程与系统调用；命令管道由宿主管柄池持有 |
| `std::clock` | 单调时刻 `Instant`、非负时长 `Duration`、纳秒和毫秒读数 |
| `std::memory` | `Span<T>`（引用地址 + 长度视图）；它的迭代器返回**元素引用**，`for (T mut& p in span)` 可直接写 |

`Instant::now().plus(Duration::fromMillis(250))` 生成截止时间。每次限时操作前用 `deadline.remaining().toMillisCeil()` 取得剩余毫秒数，多个操作可以共用一个预算。时间读数以单调时钟为基准。

`FileStream.readInto(&buffer)` 将字节读入已有的运行时 `uint8[]`，返回实际读取数量。`writeFrom(&buffer, count)` 写入指定前缀。`copyTo` 在整个复制过程复用一个缓冲区，并保持两个流打开。

### 迭代

| 模块 | 内容 |
| --- | --- |
| `std::iter` | `range`、`map` / `filter` / `collect` 惰性链、`IterNext<T>` 接口 |

`std::probe` 是测试用桩模块，不是给应用用的。

---

## 3. 使用约定

**按资源类型管理生命周期。** 文件、套接字和数据库连接遵守各自关闭协议，可显式关闭或由类型的作用域清理处理。JSON/TOML 值拥有独立数据，不把所有值都当作需要手动关闭的裸句柄；也不统一假定所有类型的重复关闭返回值相同。

**压缩结果是二进制。** `std::compress` 的输出含 NUL 字节，类型是 `std::core::List<uint8>`；解压出来的才是 `std::core::String`。

模块能力由目标宿主提供的胶水、系统库及实际构建配置决定。嵌入式 Linux 使用普通 Linux 宿主，不额外设置模块禁用模式。详见[部署说明](06-deployment.md)。

**除核心 `std::core` 外，模块都要显式 `import`。**

---

## 4. 完整 API 去哪查

标准库源码本身就是最好的参考，每个模块头部都有能力说明：

- [`source/std/`](../source/std) —— 全部模块源码，`source/std/<名字>.chl`
- [`source/abi/`](../source/abi) —— 宿主侧 C ABI 服务及必要的 C++ 适配；`abi_*.c` / `abi_*.cpp` 实现服务，`aot_modules.inc` 组织 LLVM 构建 按需绑定
- [`example/samples/`](../example/samples/) —— 按特性组织的可运行样例
- 各能力域的 API 参考：[`doc/library/`](library/README.md) 按模块分页（`std-json.md`、`std-toml.md`、`std-regex.md`、`std-linear_algebra.md`、`std-complex.md`、`std-net.md`、`std-sql.md` …）

---

## 5. 下一步

| 你想 | 去哪 |
| --- | --- |
| 让 ChaLi 调你的 C/C++ 代码 | [宿主集成与 C ABI](08-host-integration.md) |
| 查标准库之外的能力边界 | [语言参考](04-language-reference.md) |
| 看某个特性的可运行样例 | [`example/samples/`](../example/samples/) |

`std::encoding`、`std::csv`、`std::date_time`、`std::cli`、`std::utf8` 与文件系统扩展，见 [Any、显式泛型与通用数据工具](23-any-generics-data.md)。
