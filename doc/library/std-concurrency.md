# std::concurrency — 线程通信与共享

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-concurrency.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::concurrency;
```

## 公共约定与错误边界

Channel<T> 是线程组内的有界多生产者多消费者通道，容量 1..1048576；队列按值复制，关闭后排空再返回 CLOSED。句柄可复制，显式 close 负责关闭，运行结束统一释放；最多 1024 个未排空通道。Receive<T> 只有 READY 时 value 有效。状态为 READY=0、TIMEOUT=1、CLOSED=2、CANCELLED=3、FULL=4、FAILED=5、INVALID=6。millis=-1 无限、0 非阻塞、正数限时；超时不取消任务。Shared<T> 单次 get/set/swap 原子；AtomicInt64.fetchAdd 原子返回旧值，按 int64 位宽回绕。多线程默认启用，--no-threads 可关闭；普通全局变量继续隔离。 Arc<T> 和 ArcWeak<T> 可跨线程复制。强引用共同拥有不可变快照，get 在当前线程恢复独立副本；最后一份强引用释放载荷，弱引用失效后升级返回空 Arc。Shared<T> 直接保存线程组共享槽的非拥有整数标识；复制引用不分配宿主记录，也不复制或销毁槽内值。

## 参考代码

完整程序：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

```powershell
chali run example/samples/typed_messages_shared.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::concurrency;


import std::message;
import std::thread;

struct Parcel { int64 id; String name; List<int64> readings; };

int64 isolatedCounter;

void typedWorker() {
    Shared<int64> sharedCounter = receiveThread<Shared<int64>>();
    Parcel received = receiveThread<Parcel>();
    int64 old = sharedCounter.swap(20);
    isolatedCounter = 99;
    sendThread<Parcel>(parent(), received);
    sendThread<int64>(parent(), old);
}

void main() {
    Shared<int64> sharedCounter = Shared<int64>::create(10);
    isolatedCounter = 3;
    Parcel original;
    original.id = 7;
    original.name = "before";
    List<int64> readings;
    readings.push(4);
    original.readings = readings;
    Thread task = create(typedWorker);
    sendThread<Shared<int64>>(task, sharedCounter);
    sendThread<Parcel>(task, original);
    original.id = 8;
    original.readings.push(6);
    Parcel reply = receiveThread<Parcel>();
    int64 previous = receiveThread<int64>();
    wait(task);
    println("{} {} {} {} {} {}", reply.id, reply.name, reply.readings.len(),
    previous, sharedCounter.get(), isolatedCounter);
}
```

</details>

<details>
<summary>示例输出</summary>

```text
7 before 1 10 20 3
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-44"></a>`const int64 READY = 0` | 统一跨线程结果：0 完成，1 超时，2 关闭，3 取消，4 队列满，5 失败，6 无效句柄。 |
| 模块 | <a id="symbol-45"></a>`const int64 TIMEOUT = 1` | 等待预算已耗尽，目标任务仍可能运行。 |
| 模块 | <a id="symbol-46"></a>`const int64 CLOSED = 2` | 队列已关闭且无可接收载荷。 |
| 模块 | <a id="symbol-47"></a>`const int64 CANCELLED = 3` | 任务已取消的状态常量，数值见所属模块声明。 |
| 模块 | <a id="symbol-48"></a>`const int64 FULL = 4` | 有界队列已满的状态常量。 |
| 模块 | <a id="symbol-49"></a>`const int64 FAILED = 5` | 任务失败的状态常量，数值见所属模块声明。 |
| 模块 | <a id="symbol-50"></a>`const int64 INVALID = 6` | 句柄无效的状态常量。 |
| 模块 | <a id="symbol-51"></a>`struct Receive<T>` | 操作状态与载荷；仅在 READY 时读取 value。 |
| Receive | <a id="symbol-52"></a>`int64 status` | 统一操作状态码；载荷仅在 READY 时有效。 |
| Receive | <a id="symbol-53"></a>`T value` | 保存的单个值。 |
| 模块 | <a id="symbol-54"></a>`struct Arc<T>` | 可跨线程复制的强引用。各副本共同拥有一个不可变的值快照。 |
| 模块 | <a id="symbol-55"></a>`struct ArcWeak<T>` | 可跨线程复制的弱引用，仅观察 Arc 载荷的存活状态。 |
| 模块 | <a id="symbol-56"></a>`struct Shared<T>` | 每次 get/set/swap 原子；组合调用不构成原子事务。 |
| 模块 | <a id="symbol-57"></a>`struct AtomicInt64` | 共享 int64 原子槽，fetchAdd 返回旧值。 |
| AtomicInt64 | <a id="symbol-58"></a>`Shared<int64> slot` | 显式共享的 int64 原子槽。 |
| 模块 | <a id="symbol-59"></a>`struct Channel<T>` | 跨线程有界传值通道；可复制句柄，显式关闭。 |
| Channel | <a id="symbol-60"></a>`int64 handle` | 宿主资源句柄；应通过所属类型的方法操作。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Receive::isReady](#api-0) | `bool isReady()` |
| [Arc::isEmpty](#api-1) | `bool isEmpty()` |
| [Arc::clone](#api-2) | `Arc<T> clone()` |
| [Arc::get](#api-3) | `T get()` |
| [Arc::downgrade](#api-4) | `ArcWeak<T> downgrade()` |
| [Arc::close](#api-5) | `void close()` |
| [ArcWeak::isExpired](#api-6) | `bool isExpired()` |
| [ArcWeak::upgrade](#api-7) | `Arc<T> upgrade()` |
| [ArcWeak::close](#api-8) | `void close()` |
| [arc](#api-9) | `Arc<T> arc<T>(T value)` |
| [Shared::create](#api-10) | `static Shared<T> create(T value)` |
| [Shared::get](#api-11) | `T get()` |
| [Shared::set](#api-12) | `void set(T value)` |
| [Shared::swap](#api-13) | `T swap(T value)` |
| [Shared::id](#api-14) | `int64 id()` |
| [AtomicInt64::create](#api-15) | `static AtomicInt64 create(int64 value)` |
| [AtomicInt64::get](#api-16) | `int64 get()` |
| [AtomicInt64::set](#api-17) | `void set(int64 value)` |
| [AtomicInt64::fetchAdd](#api-18) | `int64 fetchAdd(int64 delta)` |
| [Channel::create](#api-19) | `static Channel<T> create(int64 capacity)` |
| [Channel::sendFor](#api-20) | `int64 sendFor(T value, int64 millis)` |
| [Channel::send](#api-21) | `int64 send(T value)` |
| [Channel::trySend](#api-22) | `int64 trySend(T value)` |
| [Channel::receiveFor](#api-23) | `Receive<T> receiveFor(int64 millis)` |
| [Channel::receive](#api-24) | `Receive<T> receive()` |
| [Channel::tryReceive](#api-25) | `Receive<T> tryReceive()` |
| [Channel::close](#api-26) | `void close()` |

<a id="api-0"></a>

### Receive::isReady（1）

**声明**

```chali
bool isReady();
```

**行为**

操作状态为 READY 时返回 true。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。操作状态为 READY 时返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isReady();
```

**源码**：[source/std/concurrency.chl:15](../../source/std/concurrency.chl#L15)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-1"></a>

### Arc::isEmpty（2）

**声明**

```chali
bool isEmpty();
```

**行为**

查询句柄是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询句柄是否为空。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/concurrency.chl:43](../../source/std/concurrency.chl#L43)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-2"></a>

### Arc::clone（3）

**声明**

```chali
Arc<T> clone();
```

**行为**

增加一份强引用；普通赋值也采用相同的所有权语义。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Arc<T>`。增加一份强引用；普通赋值也采用相同的所有权语义。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clone();
```

**源码**：[source/std/concurrency.chl:46](../../source/std/concurrency.chl#L46)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-3"></a>

### Arc::get（4）

**声明**

```chali
T get();
```

**行为**

在当前线程恢复独立的值副本。空句柄触发运行时错误。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。在当前线程恢复独立的值副本。空句柄触发运行时错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.get();
```

**源码**：[source/std/concurrency.chl:49](../../source/std/concurrency.chl#L49)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-4"></a>

### Arc::downgrade（5）

**声明**

```chali
ArcWeak<T> downgrade();
```

**行为**

创建可跨线程传递的弱引用。空强引用产生空弱引用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`ArcWeak<T>`。创建可跨线程传递的弱引用。空强引用产生空弱引用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.downgrade();
```

**源码**：[source/std/concurrency.chl:52](../../source/std/concurrency.chl#L52)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-5"></a>

### Arc::close（6）

**声明**

```chali
void close();
```

**行为**

释放当前强引用。最后一份强引用释放载荷。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[source/std/concurrency.chl:59](../../source/std/concurrency.chl#L59)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-6"></a>

### ArcWeak::isExpired（7）

**声明**

```chali
bool isExpired();
```

**行为**

检查当前时刻是否已经失效。空句柄也视为失效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查当前时刻是否已经失效。空句柄也视为失效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isExpired();
```

**源码**：[source/std/concurrency.chl:67](../../source/std/concurrency.chl#L67)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-7"></a>

### ArcWeak::upgrade（8）

**声明**

```chali
Arc<T> upgrade();
```

**行为**

原子地尝试取得强引用，失效时返回空 Arc。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Arc<T>`。原子地尝试取得强引用，失效时返回空 Arc。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.upgrade();
```

**源码**：[source/std/concurrency.chl:70](../../source/std/concurrency.chl#L70)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-8"></a>

### ArcWeak::close（9）

**声明**

```chali
void close();
```

**行为**

释放当前弱引用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[source/std/concurrency.chl:77](../../source/std/concurrency.chl#L77)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-9"></a>

### arc（10）

**声明**

```chali
Arc<T> arc<T>(T value);
```

**行为**

创建可跨线程共享的值快照，元素须符合线程传值规则。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 用于生成共享快照的值。 | 无 |

**返回值**

`Arc<T>`。持有新快照的强引用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
arc(value);
```

**源码**：[source/std/concurrency.chl:84](../../source/std/concurrency.chl#L84)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-10"></a>

### Shared::create（11）

**声明**

```chali
static Shared<T> create(T value);
```

**行为**

创建值为 value 的原子共享槽。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`Shared<T>`。创建值为 value 的原子共享槽。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Shared<T>::create(value);
```

**源码**：[source/std/concurrency.chl:94](../../source/std/concurrency.chl#L94)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-11"></a>

### Shared::get（12）

**声明**

```chali
T get();
```

**行为**

读取共享槽当前值的副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。读取共享槽当前值的副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.get();
```

**源码**：[source/std/concurrency.chl:95](../../source/std/concurrency.chl#L95)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-12"></a>

### Shared::set（13）

**声明**

```chali
void set(T value);
```

**行为**

将指定位置或共享槽替换为 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(value);
```

**源码**：[source/std/concurrency.chl:96](../../source/std/concurrency.chl#L96)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-13"></a>

### Shared::swap（14）

**声明**

```chali
T swap(T value);
```

**行为**

原子替换共享槽并返回旧值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`T`。原子替换共享槽并返回旧值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.swap(value);
```

**源码**：[source/std/concurrency.chl:97](../../source/std/concurrency.chl#L97)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-14"></a>

### Shared::id（15）

**声明**

```chali
int64 id();
```

**行为**

共享槽的引用标识，不拥有槽内值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。共享槽的引用标识，不拥有槽内值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.id();
```

**源码**：[source/std/concurrency.chl:99](../../source/std/concurrency.chl#L99)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-15"></a>

### AtomicInt64::create（16）

**声明**

```chali
static AtomicInt64 create(int64 value);
```

**行为**

创建 int64 原子槽。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

`AtomicInt64`。创建 int64 原子槽。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
AtomicInt64::create(value);
```

**源码**：[source/std/concurrency.chl:105](../../source/std/concurrency.chl#L105)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-16"></a>

### AtomicInt64::get（17）

**声明**

```chali
int64 get();
```

**行为**

读取共享槽当前值的副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取共享槽当前值的副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.get();
```

**源码**：[source/std/concurrency.chl:106](../../source/std/concurrency.chl#L106)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-17"></a>

### AtomicInt64::set（18）

**声明**

```chali
void set(int64 value);
```

**行为**

将指定位置或共享槽替换为 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(value);
```

**源码**：[source/std/concurrency.chl:107](../../source/std/concurrency.chl#L107)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-18"></a>

### AtomicInt64::fetchAdd（19）

**声明**

```chali
int64 fetchAdd(int64 delta);
```

**行为**

原子累加 int64 并返回旧值，溢出按 64 位回绕。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `delta` | `int64 delta` | 原子累加的 int64 增量 | 无 |

**返回值**

`int64`。原子累加 int64 并返回旧值，溢出按 64 位回绕。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.fetchAdd(delta);
```

**源码**：[source/std/concurrency.chl:108](../../source/std/concurrency.chl#L108)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-19"></a>

### Channel::create（20）

**声明**

```chali
static Channel<T> create(int64 capacity);
```

**行为**

创建指定容量的跨线程通道。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 原始存储的元素容量，必须非负。 | 无 |

**返回值**

`Channel<T>`。创建指定容量的跨线程通道。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Channel<T>::create(capacity);
```

**源码**：[source/std/concurrency.chl:114](../../source/std/concurrency.chl#L114)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-20"></a>

### Channel::sendFor（21）

**声明**

```chali
int64 sendFor(T value, int64 millis);
```

**行为**

在截止时间内复制发送载荷，返回统一操作状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |
| `millis` | `int64 millis` | 等待毫秒数；-1 无限、0 非阻塞，waitFor 和 sleepFor 不接受负数 | 无 |

**返回值**

`int64`。在截止时间内复制发送载荷，返回统一操作状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.sendFor(value, millis);
```

**源码**：[source/std/concurrency.chl:115](../../source/std/concurrency.chl#L115)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-21"></a>

### Channel::send（22）

**声明**

```chali
int64 send(T value);
```

**行为**

按值复制并等待发送，队列满时施加背压。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。按值复制并等待发送，队列满时施加背压。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.send(value);
```

**源码**：[source/std/concurrency.chl:116](../../source/std/concurrency.chl#L116)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-22"></a>

### Channel::trySend（23）

**声明**

```chali
int64 trySend(T value);
```

**行为**

立即尝试复制发送，队列满返回 FULL。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。立即尝试复制发送，队列满返回 FULL。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.trySend(value);
```

**源码**：[source/std/concurrency.chl:117](../../source/std/concurrency.chl#L117)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-23"></a>

### Channel::receiveFor（24）

**声明**

```chali
Receive<T> receiveFor(int64 millis);
```

**行为**

显式限时接收，只有 READY 时 value 有效；-1 无限、0 非阻塞。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `millis` | `int64 millis` | 等待毫秒数；-1 无限、0 非阻塞，waitFor 和 sleepFor 不接受负数 | 无 |

**返回值**

`Receive<T>`。显式限时接收，只有 READY 时 value 有效；-1 无限、0 非阻塞。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.receiveFor(millis);
```

**源码**：[source/std/concurrency.chl:118](../../source/std/concurrency.chl#L118)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-24"></a>

### Channel::receive（25）

**声明**

```chali
Receive<T> receive();
```

**行为**

等待并按值接收下一项，关闭排空后返回 CLOSED。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Receive<T>`。等待并按值接收下一项，关闭排空后返回 CLOSED。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.receive();
```

**源码**：[source/std/concurrency.chl:125](../../source/std/concurrency.chl#L125)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-25"></a>

### Channel::tryReceive（26）

**声明**

```chali
Receive<T> tryReceive();
```

**行为**

立即尝试接收，未就绪返回 TIMEOUT，只有 READY 时读取 value。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Receive<T>`。立即尝试接收，未就绪返回 TIMEOUT，只有 READY 时读取 value。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.tryReceive();
```

**源码**：[source/std/concurrency.chl:126](../../source/std/concurrency.chl#L126)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

<a id="api-26"></a>

### Channel::close（27）

**声明**

```chali
void close();
```

**行为**

关闭队列并停止定时源；已排队的值先排空，再返回 CLOSED。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[source/std/concurrency.chl:127](../../source/std/concurrency.chl#L127)。

**完整示例**：[example/samples/typed_messages_shared.chl](../../example/samples/typed_messages_shared.chl)。

## 补充完整示例

[example/samples/thread_channel.chl](../../example/samples/thread_channel.chl)。

```chali
import std::thread;
import std::concurrency;
void worker(Channel<int64> values) {
    values.send(42);
    values.close();
}
void main() {
    let values = Channel<int64>::create(2);
    Thread task = createWith<Channel<int64>>(worker, values);
    let next = values.receive();
    assert(next.isReady() && next.value == 42);
    assert(values.receive().status == std::concurrency::CLOSED);
    let completed = waitResult(task);
    assert(completed.isReady());
    println("channel {}", next.value);
}
```

## 补充完整示例

[example/samples/std_arc.chl](../../example/samples/std_arc.chl)。

```chali
import std::concurrency;
import std::message;
import std::thread;

struct Parcel {
    String title;
    List<int64> values;
};

struct Task {
    Arc<Parcel> owner;
    ArcWeak<Parcel> observer;
    Channel<int64> gate;
};

void readParcel(Task task) {
    assert(task.gate.receive().isReady());
    let acquired = task.observer.upgrade();
    assert(!acquired.isEmpty());
    Parcel snapshot = acquired.get();
    assert(snapshot.title == "original" && snapshot.values.len() == 1);
    snapshot.values.push(99);
    assert(task.owner.get().values.len() == 1);
}

void leaveInboxUnread(Channel<int64> gate) {
    assert(gate.receive().isReady());
}

void main() {
    Parcel original;
    original.title = "original";
    original.values = [7];
    let owner = arc(original);
    let observer = owner.downgrade();
    original.values.push(8);

    Task task;
    task.owner = owner.clone();
    task.observer = observer;
    task.gate = Channel<int64>::create(1);
    Thread reader = createWith(readParcel, task);
    task.owner.close();
    owner.close();
    assert(!observer.isExpired());
    assert(task.gate.send(1) == READY);
    wait(reader);
    assert(observer.isExpired() && observer.upgrade().isEmpty());
    task.gate.close();

    let sharedNumber = arc(42);
    let queuedObserver = sharedNumber.downgrade();
    let values = Channel<Arc<int64>>::create(1);
    assert(values.send(sharedNumber) == READY);
    sharedNumber.close();
    assert(!queuedObserver.isExpired());
    {
        let delivery = values.receive();
        assert(delivery.isReady() && delivery.value.get() == 42);
    }
    assert(queuedObserver.isExpired());
    values.close();

    let gate = Channel<int64>::create(1);
    Thread recipient = createWith(leaveInboxUnread, gate);
    let unread = arc(99);
    let unreadObserver = unread.downgrade();
    sendThread<Arc<int64>>(recipient, unread);
    unread.close();
    assert(!unreadObserver.isExpired());
    assert(gate.send(1) == READY);
    wait(recipient);
    assert(unreadObserver.isExpired());
    gate.close();

    Arc<int64> empty;
    let emptyCopy = empty.clone();
    assert(emptyCopy.isEmpty() && empty.downgrade().isExpired());
    println("Arc and ArcWeak OK");
}
```
