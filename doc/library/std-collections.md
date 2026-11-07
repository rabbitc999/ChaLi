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

Deque、Queue、Channel 基于 List；Channel 是当前执行上下文的值队列，不是跨线程消息系统。FixedVector 与 RingBuffer 的容量固定，满时 push 返回 false；空时 pop/receive 返回 None。索引从 0 开始。Deque、Queue、Stack 和 RingBuffer 的查看方法返回独立值，空容器返回 None。Deque.toList 按队首到队尾复制，RingBuffer.clear 保留配置容量。

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
    List<int64> sorted = sortValue(values);
    println("{} {} {} {} {}", sorted[0], sorted[1], sorted[2],
    binarySearchValue(sorted, 3),
    foldValue(sorted, 0, addValues));
    println("{} {} {}", values[0], values[1], values[2]);

    List<float64> stableValues;
    stableValues.push(-0.0);
    stableValues.push(2.0);
    stableValues.push(0.0);
    stableValues.push(1.0);
    List<float64> stableSorted = sortValue(stableValues);
    println("{1:1F} {2:1F} {3:1F} {4:1F}", stableSorted[0], stableSorted[1],
    stableSorted[2], stableSorted[3]);
    println("{} {} {} {} {}", findValue(sorted, 4),
    minValue(9, 2), maxValue(9, 2),
    clampValue(12, 0, 10),
    allValue(sorted, positive));
    println("{} {} {} {}", minValue(9, 2, 7, -1),
    maxValue(9, 2, 12, 7),
    minValue(5), maxValue(6));

    Deque<int64> deque = Deque<int64>::create();
    deque.pushBack(2);
    deque.pushFront(1);
    println("{} {} {}", deque.len(), deque.popFront().unwrap(),
    deque.popBack().unwrap());

    Queue<int64> queue = Queue<int64>::create();
    queue.push(7);
    queue.push(8);
    println("{} {}", queue.peek().unwrap(), queue.pop().unwrap());

    RingBuffer<int64> ring = RingBuffer<int64>::withCapacity(2);
    println("{} {} {}", ring.push(5), ring.push(6), ring.push(7));
    println("{} {}", ring.pop().unwrap(), ring.available());

    FixedVector<int64> fixed = FixedVector<int64>::withCapacity(1);
    println("{} {} {}", fixed.push(9), fixed.push(10), fixed[0]);

    int64 raw[] = new[2];
    raw[0] = 11;
    raw[1] = 12;
    Span<int64> span = Span<int64>::from(&raw[0], len(raw));
    println("{} {} {}", span.len(), span[0], span[1]);

    Channel<int64> channel = Channel<int64>::create();
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
| 模块 | <a id="symbol-27"></a>`struct Deque<T>: Iterator<T>, Index<T,int64>, Drop` | 两个相反方向的栈组成的双端队列，避免首部删除时整体平移。 |
| Deque | <a id="symbol-28"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-29"></a>`struct Queue<T>: Iterator<T>, Drop` | 先进先出的值队列；出队返回 Option，空队列返回 None。 |
| Queue | <a id="symbol-30"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-31"></a>`struct Stack<T>: Iterator<T>, Drop` | ChaLi 后进先出栈，迭代顺序为栈顶到栈底。 |
| Stack | <a id="symbol-32"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-33"></a>`struct RingBuffer<T>: Iterator<T>, Drop` | 容量固定的环形值缓冲区；满时写入返回 false。 |
| RingBuffer | <a id="symbol-34"></a>`Deque<T> values` | 按声明顺序保存的元素存储。 |
| RingBuffer | <a id="symbol-35"></a>`int64 capacity` | 最多可保存的元素数量。 |
| 模块 | <a id="symbol-36"></a>`struct FixedVector<T>: Iterator<T>, Index<T, int64>, Drop` | 容量固定的顺序容器；保存的元素数量不得超过 capacity。 |
| FixedVector | <a id="symbol-37"></a>`List<T> values` | 按声明顺序保存的元素存储。 |
| FixedVector | <a id="symbol-38"></a>`int64 capacity` | 最多可保存的元素数量。 |
| 模块 | <a id="symbol-39"></a>`struct Channel<T>: Iterator<T>, Drop` | 当前执行上下文中的值消息队列，不提供跨线程同步。 |
| Channel | <a id="symbol-40"></a>`Queue<T> pending` | 等待消费的元素或队列。 |

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
| [Deque::peekFront](#api-7) | `Option<T> peekFront()` |
| [Deque::peekBack](#api-8) | `Option<T> peekBack()` |
| [Deque::toList](#api-9) | `List<T> toList()` |
| [Deque::len](#api-10) | `int64 len()` |
| [Deque::isEmpty](#api-11) | `bool isEmpty()` |
| [Deque::clear](#api-12) | `void clear()` |
| [Deque::drop](#api-13) | `void drop()` |
| [Queue::create](#api-14) | `static Queue<T> create()` |
| [Queue::push](#api-15) | `void push(T value)` |
| [Queue::pop](#api-16) | `Option<T> pop()` |
| [Queue::peek](#api-17) | `Option<T> peek()` |
| [Queue::peekBack](#api-18) | `Option<T> peekBack()` |
| [Queue::iterator](#api-19) | `Option<T> iterator(int64 index)` |
| [Queue::len](#api-20) | `int64 len()` |
| [Queue::isEmpty](#api-21) | `bool isEmpty()` |
| [Queue::clear](#api-22) | `void clear()` |
| [Queue::drop](#api-23) | `void drop()` |
| [Stack::create](#api-24) | `static Stack<T> create()` |
| [Stack::push](#api-25) | `void push(T value)` |
| [Stack::pop](#api-26) | `Option<T> pop()` |
| [Stack::peek](#api-27) | `Option<T> peek()` |
| [Stack::peekBottom](#api-28) | `Option<T> peekBottom()` |
| [Stack::iterator](#api-29) | `Option<T> iterator(int64 index)` |
| [Stack::len](#api-30) | `int64 len()` |
| [Stack::isEmpty](#api-31) | `bool isEmpty()` |
| [Stack::clear](#api-32) | `void clear()` |
| [Stack::drop](#api-33) | `void drop()` |
| [RingBuffer::withCapacity](#api-34) | `static RingBuffer<T> withCapacity(int64 capacity)` |
| [RingBuffer::push](#api-35) | `bool push(T value)` |
| [RingBuffer::pop](#api-36) | `Option<T> pop()` |
| [RingBuffer::iterator](#api-37) | `Option<T> iterator(int64 index)` |
| [RingBuffer::len](#api-38) | `int64 len()` |
| [RingBuffer::available](#api-39) | `int64 available()` |
| [RingBuffer::isEmpty](#api-40) | `bool isEmpty()` |
| [RingBuffer::isFull](#api-41) | `bool isFull()` |
| [RingBuffer::peek](#api-42) | `Option<T> peek()` |
| [RingBuffer::clear](#api-43) | `void clear()` |
| [RingBuffer::drop](#api-44) | `void drop()` |
| [FixedVector::withCapacity](#api-45) | `static FixedVector<T> withCapacity(int64 capacity)` |
| [FixedVector::push](#api-46) | `bool push(T value)` |
| [FixedVector::pop](#api-47) | `Option<T> pop()` |
| [FixedVector::index](#api-48) | `T index(int64 key)` |
| [FixedVector::iterator](#api-49) | `Option<T> iterator(int64 index)` |
| [FixedVector::len](#api-50) | `int64 len()` |
| [FixedVector::capacityValue](#api-51) | `int64 capacityValue()` |
| [FixedVector::drop](#api-52) | `void drop()` |
| [Channel::create](#api-53) | `static Channel<T> create()` |
| [Channel::send](#api-54) | `void send(T value)` |
| [Channel::receive](#api-55) | `Option<T> receive()` |
| [Channel::iterator](#api-56) | `Option<T> iterator(int64 index)` |
| [Channel::len](#api-57) | `int64 len()` |
| [Channel::isEmpty](#api-58) | `bool isEmpty()` |
| [Channel::drop](#api-59) | `void drop()` |

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
Deque<T>::create();
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
显式前置检查：`key>=0 && key<self.len(),"Deque 下标越界"`。

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

### Deque::peekFront（8）

**声明**

```chali
Option<T> peekFront();
```

**行为**

读取队首副本，空队列返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。读取队首副本，空队列返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peekFront();
```

**源码**：[source/std/collections.chl:32](../../source/std/collections.chl#L32)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-8"></a>

### Deque::peekBack（9）

**声明**

```chali
Option<T> peekBack();
```

**行为**

读取队尾副本，空队列返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。读取队尾副本，空队列返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peekBack();
```

**源码**：[source/std/collections.chl:34](../../source/std/collections.chl#L34)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-9"></a>

### Deque::toList（10）

**声明**

```chali
List<T> toList();
```

**行为**

按队首到队尾导出独立列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<T>`。按队首到队尾导出独立列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toList();
```

**源码**：[source/std/collections.chl:36](../../source/std/collections.chl#L36)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-10"></a>

### Deque::len（11）

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

**源码**：[source/std/collections.chl:46](../../source/std/collections.chl#L46)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-11"></a>

### Deque::isEmpty（12）

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

**源码**：[source/std/collections.chl:48](../../source/std/collections.chl#L48)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-12"></a>

### Deque::clear（13）

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

**源码**：[source/std/collections.chl:50](../../source/std/collections.chl#L50)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-13"></a>

### Deque::drop（14）

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

**源码**：[source/std/collections.chl:52](../../source/std/collections.chl#L52)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-14"></a>

### Queue::create（15）

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
Queue<T>::create();
```

**源码**：[source/std/collections.chl:59](../../source/std/collections.chl#L59)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-15"></a>

### Queue::push（16）

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

**源码**：[source/std/collections.chl:61](../../source/std/collections.chl#L61)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-16"></a>

### Queue::pop（17）

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

**源码**：[source/std/collections.chl:65](../../source/std/collections.chl#L65)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-17"></a>

### Queue::peek（18）

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

**源码**：[source/std/collections.chl:67](../../source/std/collections.chl#L67)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-18"></a>

### Queue::peekBack（19）

**声明**

```chali
Option<T> peekBack();
```

**行为**

读取队尾副本，空队列返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。读取队尾副本，空队列返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peekBack();
```

**源码**：[source/std/collections.chl:69](../../source/std/collections.chl#L69)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-19"></a>

### Queue::iterator（20）

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

**源码**：[source/std/collections.chl:71](../../source/std/collections.chl#L71)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-20"></a>

### Queue::len（21）

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

**源码**：[source/std/collections.chl:73](../../source/std/collections.chl#L73)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-21"></a>

### Queue::isEmpty（22）

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

**源码**：[source/std/collections.chl:75](../../source/std/collections.chl#L75)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-22"></a>

### Queue::clear（23）

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

**源码**：[source/std/collections.chl:77](../../source/std/collections.chl#L77)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-23"></a>

### Queue::drop（24）

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

**源码**：[source/std/collections.chl:79](../../source/std/collections.chl#L79)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-24"></a>

### Stack::create（25）

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
Stack<T>::create();
```

**源码**：[source/std/collections.chl:86](../../source/std/collections.chl#L86)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-25"></a>

### Stack::push（26）

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

**源码**：[source/std/collections.chl:88](../../source/std/collections.chl#L88)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-26"></a>

### Stack::pop（27）

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

**源码**：[source/std/collections.chl:90](../../source/std/collections.chl#L90)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-27"></a>

### Stack::peek（28）

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

**源码**：[source/std/collections.chl:92](../../source/std/collections.chl#L92)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-28"></a>

### Stack::peekBottom（29）

**声明**

```chali
Option<T> peekBottom();
```

**行为**

读取栈底副本，空栈返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。读取栈底副本，空栈返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peekBottom();
```

**源码**：[source/std/collections.chl:94](../../source/std/collections.chl#L94)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-29"></a>

### Stack::iterator（30）

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

**源码**：[source/std/collections.chl:96](../../source/std/collections.chl#L96)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-30"></a>

### Stack::len（31）

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

**源码**：[source/std/collections.chl:98](../../source/std/collections.chl#L98)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-31"></a>

### Stack::isEmpty（32）

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

**源码**：[source/std/collections.chl:100](../../source/std/collections.chl#L100)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-32"></a>

### Stack::clear（33）

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

**源码**：[source/std/collections.chl:102](../../source/std/collections.chl#L102)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-33"></a>

### Stack::drop（34）

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

**源码**：[source/std/collections.chl:104](../../source/std/collections.chl#L104)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-34"></a>

### RingBuffer::withCapacity（35）

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
显式前置检查：`capacity>0`。

**调用形式**

```chali
RingBuffer<T>::withCapacity(capacity);
```

**源码**：[source/std/collections.chl:111](../../source/std/collections.chl#L111)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-35"></a>

### RingBuffer::push（36）

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

**源码**：[source/std/collections.chl:113](../../source/std/collections.chl#L113)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-36"></a>

### RingBuffer::pop（37）

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

**源码**：[source/std/collections.chl:115](../../source/std/collections.chl#L115)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-37"></a>

### RingBuffer::iterator（38）

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

**源码**：[source/std/collections.chl:117](../../source/std/collections.chl#L117)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-38"></a>

### RingBuffer::len（39）

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

**源码**：[source/std/collections.chl:119](../../source/std/collections.chl#L119)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-39"></a>

### RingBuffer::available（40）

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

**源码**：[source/std/collections.chl:121](../../source/std/collections.chl#L121)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-40"></a>

### RingBuffer::isEmpty（41）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断缓冲是否为空。

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

**源码**：[source/std/collections.chl:123](../../source/std/collections.chl#L123)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-41"></a>

### RingBuffer::isFull（42）

**声明**

```chali
bool isFull();
```

**行为**

判断缓冲是否已满。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断缓冲是否已满。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isFull();
```

**源码**：[source/std/collections.chl:125](../../source/std/collections.chl#L125)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-42"></a>

### RingBuffer::peek（43）

**声明**

```chali
Option<T> peek();
```

**行为**

查看下一项并保留该项供后续消费。

读取下一次弹出的值的副本，空缓冲返回 None。

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

**源码**：[source/std/collections.chl:127](../../source/std/collections.chl#L127)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-43"></a>

### RingBuffer::clear（44）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清除有效元素，保留配置的容量。

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

**源码**：[source/std/collections.chl:129](../../source/std/collections.chl#L129)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-44"></a>

### RingBuffer::drop（45）

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

**源码**：[source/std/collections.chl:131](../../source/std/collections.chl#L131)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-45"></a>

### FixedVector::withCapacity（46）

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
FixedVector<T>::withCapacity(capacity);
```

**源码**：[source/std/collections.chl:138](../../source/std/collections.chl#L138)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-46"></a>

### FixedVector::push（47）

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

**源码**：[source/std/collections.chl:147](../../source/std/collections.chl#L147)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-47"></a>

### FixedVector::pop（48）

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

**源码**：[source/std/collections.chl:152](../../source/std/collections.chl#L152)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-48"></a>

### FixedVector::index（49）

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

**源码**：[source/std/collections.chl:156](../../source/std/collections.chl#L156)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-49"></a>

### FixedVector::iterator（50）

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

**源码**：[source/std/collections.chl:157](../../source/std/collections.chl#L157)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-50"></a>

### FixedVector::len（51）

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

**源码**：[source/std/collections.chl:161](../../source/std/collections.chl#L161)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-51"></a>

### FixedVector::capacityValue（52）

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

**源码**：[source/std/collections.chl:162](../../source/std/collections.chl#L162)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-52"></a>

### FixedVector::drop（53）

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

**源码**：[source/std/collections.chl:163](../../source/std/collections.chl#L163)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-53"></a>

### Channel::create（54）

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
Channel<T>::create();
```

**源码**：[source/std/collections.chl:170](../../source/std/collections.chl#L170)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-54"></a>

### Channel::send（55）

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

**源码**：[source/std/collections.chl:172](../../source/std/collections.chl#L172)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-55"></a>

### Channel::receive（56）

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

**源码**：[source/std/collections.chl:174](../../source/std/collections.chl#L174)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-56"></a>

### Channel::iterator（57）

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

**源码**：[source/std/collections.chl:176](../../source/std/collections.chl#L176)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-57"></a>

### Channel::len（58）

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

**源码**：[source/std/collections.chl:178](../../source/std/collections.chl#L178)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-58"></a>

### Channel::isEmpty（59）

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

**源码**：[source/std/collections.chl:180](../../source/std/collections.chl#L180)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-59"></a>

### Channel::drop（60）

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

**源码**：[source/std/collections.chl:182](../../source/std/collections.chl#L182)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。
