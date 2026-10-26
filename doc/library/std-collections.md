# std::collections — 集合扩展

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-collections.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::collections;
```

## 公共约定与错误边界

Deque、Queue、Channel 基于 List；Channel 是当前执行上下文的值队列，不是跨线程消息系统。FixedVector 与 RingBuffer 的容量固定，满时 push 返回 false；空时 pop/receive 返回 None。索引从 0 开始。

## 参考代码

完整程序：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

```powershell
chali run example/samples/std_collections_algorithms.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::collections;

import std::memory;

import std::algorithms;

bool positive(int64 value) { return value > 0; }
int64 addValues(int64 left, int64 right) { return left + right; }

void main() {
    List<int64> values;
    values.push(4);
    values.push(1);
    values.push(3);
    List<int64> sorted = std::algorithms::sortValue<int64>(values);
    println("{} {} {} {} {}", sorted[0], sorted[1], sorted[2],
    std::algorithms::binarySearchValue<int64>(sorted, 3),
    std::algorithms::foldValue<int64>(sorted, 0, addValues));
    println("{} {} {}", values[0], values[1], values[2]);

    List<float64> stableValues;
    stableValues.push(-0.0);
    stableValues.push(2.0);
    stableValues.push(0.0);
    stableValues.push(1.0);
    List<float64> stableSorted = std::algorithms::sortValue<float64>(stableValues);
    println("{1:1F} {2:1F} {3:1F} {4:1F}", stableSorted[0], stableSorted[1],
    stableSorted[2], stableSorted[3]);
    println("{} {} {} {} {}", std::algorithms::findValue<int64>(sorted, 4),
    std::algorithms::minValue<int64>(9, 2), std::algorithms::maxValue<int64>(9, 2),
    std::algorithms::clampValue<int64>(12, 0, 10),
    std::algorithms::allValue<int64>(sorted, positive));
    println("{} {} {} {}", std::algorithms::minValue<int64>(9, 2, 7, -1),
    std::algorithms::maxValue<int64>(9, 2, 12, 7),
    std::algorithms::minValue<int64>(5), std::algorithms::maxValue<int64>(6));

    std::collections::Deque<int64> deque = std::collections::Deque<int64>::create();
    deque.pushBack(2);
    deque.pushFront(1);
    println("{} {} {}", deque.len(), deque.popFront().unwrap(),
    deque.popBack().unwrap());

    std::collections::Queue<int64> queue = std::collections::Queue<int64>::create();
    queue.push(7);
    queue.push(8);
    println("{} {}", queue.peek().unwrap(), queue.pop().unwrap());

    std::collections::RingBuffer<int64> ring = std::collections::RingBuffer<int64>::withCapacity(2);
    println("{} {} {}", ring.push(5), ring.push(6), ring.push(7));
    println("{} {}", ring.pop().unwrap(), ring.available());

    std::collections::FixedVector<int64> fixed = std::collections::FixedVector<int64>::withCapacity(1);
    println("{} {} {}", fixed.push(9), fixed.push(10), fixed[0]);

    int64 raw[] = new[2];
    raw[0] = 11;
    raw[1] = 12;
    std::memory::Span<int64> span = std::memory::Span<int64>::from(&raw[0], len(raw));
    println("{} {} {}", span.len(), span[0], span[1]);

    std::collections::Channel<int64> channel = std::collections::Channel<int64>::create();
    channel.send(21);
    println("{} {}", channel.len(), channel.receive().unwrap());

    Pair<int64, String> pair = Pair<int64, String>::create(7, "seven");
    println("{} {}", pair.first, pair.second);
    raw.drop();
}
```

</details>

<details>
<summary>示例输出</summary>

```text
1 3 4 1 8
4 1 3
-0.0 0.0 1.0 2.0
2 2 9 10 1
-1 12 5 6
2 1 2
7 7
1 1 0
5 1
1 0 9
2 11 12
1 21
7 seven
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-25"></a>`struct Deque<T>: Iterator<T>, Index<T,int64>, Drop` | 两个相反方向的栈组成的双端队列，避免首部删除时整体平移。 |
| Deque | <a id="symbol-26"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-27"></a>`struct Queue<T>: Iterator<T>, Drop` | 先进先出的值队列；出队返回 Option，空队列返回 None。 |
| Queue | <a id="symbol-28"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-29"></a>`struct Stack<T>: Iterator<T>, Drop` | ChaLi 后进先出栈，迭代顺序为栈顶到栈底。 |
| Stack | <a id="symbol-30"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-31"></a>`struct RingBuffer<T>: Iterator<T>, Drop` | 容量固定的环形值缓冲区；满时写入返回 false。 |
| RingBuffer | <a id="symbol-32"></a>`Deque<T> values` | 按声明顺序保存的元素存储。 |
| RingBuffer | <a id="symbol-33"></a>`int64 capacity` | 最多可保存的元素数量。 |
| 模块 | <a id="symbol-34"></a>`struct FixedVector<T>: Iterator<T>, Index<T, int64>, Drop` | 容量固定的顺序容器；保存的元素数量不得超过 capacity。 |
| FixedVector | <a id="symbol-35"></a>`std::lang::List<T> values` | 按声明顺序保存的元素存储。 |
| FixedVector | <a id="symbol-36"></a>`int64 capacity` | 最多可保存的元素数量。 |
| 模块 | <a id="symbol-37"></a>`struct Channel<T>: Iterator<T>, Drop` | 当前执行上下文中的值消息队列，不提供跨线程同步。 |
| Channel | <a id="symbol-38"></a>`Queue<T> pending` | 等待消费的元素或队列。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Deque::create](#api-0) | `static Deque<T> create()` |
| [Deque::pushBack](#api-1) | `void pushBack(T value)` |
| [Deque::pushFront](#api-2) | `void pushFront(T value)` |
| [Deque::popBack](#api-3) | `Option<T> popBack()` |
| [Deque::popFront](#api-4) | `Option<T> popFront()` |
| [Deque::index](#api-5) | `T index(int64 key)` |
| [Deque::iterator](#api-6) | `Option<T> iterator(int64 index)` |
| [Deque::len](#api-7) | `int64 len()` |
| [Deque::isEmpty](#api-8) | `bool isEmpty()` |
| [Deque::clear](#api-9) | `void clear()` |
| [Deque::drop](#api-10) | `void drop()` |
| [Queue::create](#api-11) | `static Queue<T> create()` |
| [Queue::push](#api-12) | `void push(T value)` |
| [Queue::pop](#api-13) | `Option<T> pop()` |
| [Queue::peek](#api-14) | `Option<T> peek()` |
| [Queue::iterator](#api-15) | `Option<T> iterator(int64 index)` |
| [Queue::len](#api-16) | `int64 len()` |
| [Queue::isEmpty](#api-17) | `bool isEmpty()` |
| [Queue::clear](#api-18) | `void clear()` |
| [Queue::drop](#api-19) | `void drop()` |
| [Stack::create](#api-20) | `static Stack<T> create()` |
| [Stack::push](#api-21) | `void push(T value)` |
| [Stack::pop](#api-22) | `Option<T> pop()` |
| [Stack::peek](#api-23) | `Option<T> peek()` |
| [Stack::iterator](#api-24) | `Option<T> iterator(int64 index)` |
| [Stack::len](#api-25) | `int64 len()` |
| [Stack::isEmpty](#api-26) | `bool isEmpty()` |
| [Stack::clear](#api-27) | `void clear()` |
| [Stack::drop](#api-28) | `void drop()` |
| [RingBuffer::withCapacity](#api-29) | `static RingBuffer<T> withCapacity(int64 capacity)` |
| [RingBuffer::push](#api-30) | `bool push(T value)` |
| [RingBuffer::pop](#api-31) | `Option<T> pop()` |
| [RingBuffer::iterator](#api-32) | `Option<T> iterator(int64 index)` |
| [RingBuffer::len](#api-33) | `int64 len()` |
| [RingBuffer::available](#api-34) | `int64 available()` |
| [RingBuffer::drop](#api-35) | `void drop()` |
| [FixedVector::withCapacity](#api-36) | `static FixedVector<T> withCapacity(int64 capacity)` |
| [FixedVector::push](#api-37) | `bool push(T value)` |
| [FixedVector::pop](#api-38) | `Option<T> pop()` |
| [FixedVector::index](#api-39) | `T index(int64 key)` |
| [FixedVector::iterator](#api-40) | `Option<T> iterator(int64 index)` |
| [FixedVector::len](#api-41) | `int64 len()` |
| [FixedVector::capacityValue](#api-42) | `int64 capacityValue()` |
| [FixedVector::drop](#api-43) | `void drop()` |
| [Channel::create](#api-44) | `static Channel<T> create()` |
| [Channel::send](#api-45) | `void send(T value)` |
| [Channel::receive](#api-46) | `Option<T> receive()` |
| [Channel::iterator](#api-47) | `Option<T> iterator(int64 index)` |
| [Channel::len](#api-48) | `int64 len()` |
| [Channel::isEmpty](#api-49) | `bool isEmpty()` |
| [Channel::drop](#api-50) | `void drop()` |

<a id="api-0"></a>

### Deque::create（1）

**声明**

```chali
static Deque<T> create();
```

**行为**

创建Deque，返回可供后续操作的值。

创建空双端队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Deque<T>`。创建Deque，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::collections::Deque<T>::create();
```

**源码**：[source/std/collections.chl:18](../../source/std/collections.chl#L18)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-1"></a>

### Deque::pushBack（2）

**声明**

```chali
void pushBack(T value);
```

**行为**

将 value 追加到尾部。

在队尾接管一个参数值。

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
obj.pushBack(value);
```

**源码**：[source/std/collections.chl:20](../../source/std/collections.chl#L20)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-2"></a>

### Deque::pushFront（3）

**声明**

```chali
void pushFront(T value);
```

**行为**

将 value 插入首部。

在队首接管一个参数值。

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
obj.pushFront(value);
```

**源码**：[source/std/collections.chl:22](../../source/std/collections.chl#L22)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-3"></a>

### Deque::popBack（4）

**声明**

```chali
Option<T> popBack();
```

**行为**

移除尾部并返回 Option；空时 None。

弹出队尾，必要时分摊搬移另一端的一半元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除尾部并返回 Option；空时 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.popBack();
```

**源码**：[source/std/collections.chl:24](../../source/std/collections.chl#L24)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-4"></a>

### Deque::popFront（5）

**声明**

```chali
Option<T> popFront();
```

**行为**

移除首部并返回 Option；空时 None。

弹出队首，必要时分摊搬移另一端的一半元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除首部并返回 Option；空时 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.popFront();
```

**源码**：[source/std/collections.chl:26](../../source/std/collections.chl#L26)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-5"></a>

### Deque::index（6）

**声明**

```chali
T index(int64 key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

按队首为零的逻辑下标读取独立值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/collections.chl:28](../../source/std/collections.chl#L28)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-6"></a>

### Deque::iterator（7）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

按队首到队尾迭代。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:30](../../source/std/collections.chl#L30)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-7"></a>

### Deque::len（8）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回元素数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:32](../../source/std/collections.chl#L32)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-8"></a>

### Deque::isEmpty（9）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/collections.chl:34](../../source/std/collections.chl#L34)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-9"></a>

### Deque::clear（10）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清除两端的有效元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/collections.chl:36](../../source/std/collections.chl#L36)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-10"></a>

### Deque::drop（11）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

释放两个拥有数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:38](../../source/std/collections.chl#L38)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-11"></a>

### Queue::create（12）

**声明**

```chali
static Queue<T> create();
```

**行为**

创建Queue，返回可供后续操作的值。

创建空队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Queue<T>`。创建Queue，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::collections::Queue<T>::create();
```

**源码**：[source/std/collections.chl:45](../../source/std/collections.chl#L45)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-12"></a>

### Queue::push（13）

**声明**

```chali
void push(T value);
```

**行为**

追加元素。

在队尾追加一个值。

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
obj.push(value);
```

**源码**：[source/std/collections.chl:47](../../source/std/collections.chl#L47)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-13"></a>

### Queue::pop（14）

**声明**

```chali
Option<T> pop();
```

**行为**

移除并返回下一元素；空容器返回 None。

弹出队首，空队列返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除并返回下一元素；空容器返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/std/collections.chl:51](../../source/std/collections.chl#L51)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-14"></a>

### Queue::peek（15）

**声明**

```chali
Option<T> peek();
```

**行为**

查看下一项并保留该项供后续消费。

读取队首的独立值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。查看下一项并保留该项供后续消费。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peek();
```

**源码**：[source/std/collections.chl:53](../../source/std/collections.chl#L53)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-15"></a>

### Queue::iterator（16）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

按先进先出顺序迭代。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:55](../../source/std/collections.chl#L55)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-16"></a>

### Queue::len（17）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回有效元素数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:57](../../source/std/collections.chl#L57)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-17"></a>

### Queue::isEmpty（18）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/collections.chl:59](../../source/std/collections.chl#L59)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-18"></a>

### Queue::clear（19）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/collections.chl:61](../../source/std/collections.chl#L61)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-19"></a>

### Queue::drop（20）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

释放两个拥有数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:63](../../source/std/collections.chl#L63)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-20"></a>

### Stack::create（21）

**声明**

```chali
static Stack<T> create();
```

**行为**

创建Stack，返回可供后续操作的值。

创建空栈。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Stack<T>`。创建Stack，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::collections::Stack<T>::create();
```

**源码**：[source/std/collections.chl:70](../../source/std/collections.chl#L70)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-21"></a>

### Stack::push（22）

**声明**

```chali
void push(T value);
```

**行为**

将 value 追加到列表尾部。

接管参数值并追加到栈顶。

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
obj.push(value);
```

**源码**：[source/std/collections.chl:72](../../source/std/collections.chl#L72)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-22"></a>

### Stack::pop（23）

**声明**

```chali
Option<T> pop();
```

**行为**

移除列表尾元素并返回值，空列表时报错。

弹出栈顶，空栈返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除列表尾元素并返回值，空列表时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/std/collections.chl:74](../../source/std/collections.chl#L74)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-23"></a>

### Stack::peek（24）

**声明**

```chali
Option<T> peek();
```

**行为**

查看下一项并保留该项供后续消费。

读取栈顶的独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。查看下一项并保留该项供后续消费。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peek();
```

**源码**：[source/std/collections.chl:76](../../source/std/collections.chl#L76)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-24"></a>

### Stack::iterator（25）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

从栈顶开始迭代。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:78](../../source/std/collections.chl#L78)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-25"></a>

### Stack::len（26）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回元素数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:80](../../source/std/collections.chl#L80)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-26"></a>

### Stack::isEmpty（27）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/collections.chl:82](../../source/std/collections.chl#L82)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-27"></a>

### Stack::clear（28）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空所有有效元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/collections.chl:84](../../source/std/collections.chl#L84)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-28"></a>

### Stack::drop（29）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

释放拥有的数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:86](../../source/std/collections.chl#L86)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-29"></a>

### RingBuffer::withCapacity（30）

**声明**

```chali
static RingBuffer<T> withCapacity(int64 capacity);
```

**行为**

用指定 capacity 创建有界容器。

创建非零容量缓冲。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 原始存储的元素容量，必须非负。 | 无 |

**返回值**

`RingBuffer<T>`。用指定 capacity 创建有界容器。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::collections::RingBuffer<T>::withCapacity(capacity);
```

**源码**：[source/std/collections.chl:93](../../source/std/collections.chl#L93)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-30"></a>

### RingBuffer::push（31）

**声明**

```chali
bool push(T value);
```

**行为**

追加 value；容量已满时返回 false，成功为 true。

未满时在队尾插入，满时返回 false。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。追加 value；容量已满时返回 false，成功为 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.push(value);
```

**源码**：[source/std/collections.chl:95](../../source/std/collections.chl#L95)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-31"></a>

### RingBuffer::pop（32）

**声明**

```chali
Option<T> pop();
```

**行为**

移除并返回下一元素；空容器返回 None。

从队首弹出值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除并返回下一元素；空容器返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/std/collections.chl:97](../../source/std/collections.chl#L97)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-32"></a>

### RingBuffer::iterator（33）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

按先进先出顺序迭代。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:99](../../source/std/collections.chl#L99)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-33"></a>

### RingBuffer::len（34）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回有效元素数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:101](../../source/std/collections.chl#L101)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-34"></a>

### RingBuffer::available（35）

**声明**

```chali
int64 available();
```

**行为**

返回剩余可写容量。

返回剩余可写数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回剩余可写容量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.available();
```

**源码**：[source/std/collections.chl:103](../../source/std/collections.chl#L103)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-35"></a>

### RingBuffer::drop（36）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

释放队列存储。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:105](../../source/std/collections.chl#L105)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-36"></a>

### FixedVector::withCapacity（37）

**声明**

```chali
static FixedVector<T> withCapacity(int64 capacity);
```

**行为**

用指定 capacity 创建有界容器。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 原始存储的元素容量，必须非负。 | 无 |

**返回值**

`FixedVector<T>`。用指定 capacity 创建有界容器。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`capacity >= 0`。

**调用形式**

```chali
std::collections::FixedVector<T>::withCapacity(capacity);
```

**源码**：[source/std/collections.chl:112](../../source/std/collections.chl#L112)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-37"></a>

### FixedVector::push（38）

**声明**

```chali
bool push(T value);
```

**行为**

追加 value；容量已满时返回 false，成功为 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。追加 value；容量已满时返回 false，成功为 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.push(value);
```

**源码**：[source/std/collections.chl:121](../../source/std/collections.chl#L121)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-38"></a>

### FixedVector::pop（39）

**声明**

```chali
Option<T> pop();
```

**行为**

移除并返回下一元素；空容器返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。移除并返回下一元素；空容器返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/std/collections.chl:126](../../source/std/collections.chl#L126)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-39"></a>

### FixedVector::index（40）

**声明**

```chali
T index(int64 key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/collections.chl:130](../../source/std/collections.chl#L130)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-40"></a>

### FixedVector::iterator（41）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:131](../../source/std/collections.chl#L131)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-41"></a>

### FixedVector::len（42）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:135](../../source/std/collections.chl#L135)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-42"></a>

### FixedVector::capacityValue（43）

**声明**

```chali
int64 capacityValue();
```

**行为**

返回固定容量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回固定容量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.capacityValue();
```

**源码**：[source/std/collections.chl:136](../../source/std/collections.chl#L136)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-43"></a>

### FixedVector::drop（44）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:137](../../source/std/collections.chl#L137)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-44"></a>

### Channel::create（45）

**声明**

```chali
static Channel<T> create();
```

**行为**

创建当前线程内的本地值队列。

创建空消息队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Channel<T>`。创建当前线程内的本地值队列。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::collections::Channel<T>::create();
```

**源码**：[source/std/collections.chl:144](../../source/std/collections.chl#L144)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-45"></a>

### Channel::send（46）

**声明**

```chali
void send(T value);
```

**行为**

将 value 加入本地值队列。

向队尾发送一个值。

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
obj.send(value);
```

**源码**：[source/std/collections.chl:146](../../source/std/collections.chl#L146)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-46"></a>

### Channel::receive（47）

**声明**

```chali
Option<T> receive();
```

**行为**

取出队首值；空队列返回 None。

取得队首消息，空时返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。取出队首值；空队列返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.receive();
```

**源码**：[source/std/collections.chl:148](../../source/std/collections.chl#L148)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-47"></a>

### Channel::iterator（48）

**声明**

```chali
Option<T> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

按消息顺序迭代。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/collections.chl:150](../../source/std/collections.chl#L150)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-48"></a>

### Channel::len（49）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回消息数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/collections.chl:152](../../source/std/collections.chl#L152)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-49"></a>

### Channel::isEmpty（50）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断队列是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/collections.chl:154](../../source/std/collections.chl#L154)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-50"></a>

### Channel::drop（51）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

释放消息队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/collections.chl:156](../../source/std/collections.chl#L156)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。
