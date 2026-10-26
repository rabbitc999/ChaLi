# std::array — 可扩容数组

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-array.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::array;
```

## 公共约定与错误边界

DynamicArray<T> 使用运行时堆数组保存连续元素，支持 reserve、倍增 push、resize、capacity、set、pop、遍历和独立快照。负容量、负长度及越界触发错误；复制为独立值，清空保留容量。

## 参考代码

完整程序：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

```powershell
chali run example/samples/dynamic_array.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::array;
struct Cell { String text; List<int64> values; };
int64 probeDrops = 0;
struct Probe: Drop {
    int64 marker;
    void drop() { assert(self.marker == 7, "未使用的容量槽不应析构 T"); probeDrops = probeDrops + 1; }
};
void main() {
    {
        std::array::DynamicArray<Probe> reserved;
        reserved.reserve(16);
    }
    assert(probeDrops == 0);
    {
        std::array::DynamicArray<Probe> probes;
        Probe probe; probe.marker = 7;
        probes.push(probe);
        probes.reserve(8);
        Probe retrievedProbe = probes[0];
        assert(retrievedProbe.marker == 7);
        probes.clear();
    }
    assert(probeDrops > 0);
    std::array::DynamicArray<int64> values;
    assert(values.len() == 0 && values.capacity() == 0 && values.isEmpty());
    values.reserve(3);
    assert(values.capacity() == 3 && values.len() == 0);
    int64 i = 0;
    while (i < 20) { values.push(i * 2); i = i + 1; }
    assert(values.len() == 20 && values.capacity() >= 20 && values[19] == 38);
    values.set(0, 7);
    std::array::DynamicArray<int64> copied = values;
    copied.set(0, 9);
    assert(values[0] == 7 && copied[0] == 9);
    int64 capacity = values.capacity();
    values.resize(2);
    values.resize(6);
    assert(values.len() == 6 && values[0] == 7 && values[2] == 0 && values.capacity() == capacity);
    assert(values.pop().unwrap() == 0 && values.len() == 5);
    int64 snapshot[] = values.toArray();
    snapshot[0] = 13;
    assert(values[0] == 7 && len(snapshot) == 5);
    int64 sum = 0;
    for (int64 item in values) { sum = sum + item; }
    assert(sum == 9);
    values.clear();
    assert(values.isEmpty() && values.capacity() == capacity && values.pop().isNone());
    std::array::DynamicArray<String> texts = std::array::DynamicArray<String>::withCapacity(1);
    texts.push("first"); texts.push("second");
    std::array::DynamicArray<String> textCopy = texts;
    textCopy.set(0, "changed");
    assert(texts[0] == "first" && textCopy[0] == "changed");
    texts.resize(1); texts.resize(2);
    assert(texts[1] == "");
    std::array::DynamicArray<Cell> cells;
    Cell cell; cell.text = "cell"; cell.values.push(3);
    cells.push(cell); cells.reserve(8);
    Cell retrieved = cells[0]; retrieved.values.push(5);
    assert(cells[0].text == "cell" && cells[0].values.len() == 1);
    cells.clear(); cells.resize(1);
    assert(cells[0].text == "" && cells[0].values.len() == 0);
    println("dynamic array OK");
}
```

</details>

<details>
<summary>示例输出</summary>

```text
dynamic array OK
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-0"></a>`struct DynamicArray<T>: Index<T, int64>, Iterator<T>, Copy<DynamicArray<T>>` | 连续存储的可扩容值数组；复制时独立复制缓冲区，读取下标、写入使用 set。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [DynamicArray::create](#api-0) | `static DynamicArray<T> create()` |
| [DynamicArray::withCapacity](#api-1) | `static DynamicArray<T> withCapacity(int64 capacity)` |
| [DynamicArray::len](#api-2) | `int64 len()` |
| [DynamicArray::capacity](#api-3) | `int64 capacity()` |
| [DynamicArray::isEmpty](#api-4) | `bool isEmpty()` |
| [DynamicArray::copy](#api-5) | `DynamicArray<T> copy()` |
| [DynamicArray::reserve](#api-6) | `void reserve(int64 minimumCapacity)` |
| [DynamicArray::push](#api-7) | `void push(T value)` |
| [DynamicArray::resize](#api-8) | `void resize(int64 length)` |
| [DynamicArray::clear](#api-9) | `void clear()` |
| [DynamicArray::at](#api-10) | `T at(int64 position)` |
| [DynamicArray::index](#api-11) | `T index(int64 position)` |
| [DynamicArray::set](#api-12) | `void set(int64 position, T value)` |
| [DynamicArray::pop](#api-13) | `Option<T> pop()` |
| [DynamicArray::iterator](#api-14) | `Option<T> iterator(int64 position)` |
| [DynamicArray::toArray](#api-15) | `T[] toArray()` |

<a id="api-0"></a>

### DynamicArray::create（1）

**声明**

```chali
static DynamicArray<T> create();
```

**行为**

创建DynamicArray，返回可供后续操作的值。

创建空数组，不预先分配元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`DynamicArray<T>`。创建DynamicArray，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::array::DynamicArray<T>::create();
```

**源码**：[source/std/array.chl:27](../../source/std/array.chl#L27)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-1"></a>

### DynamicArray::withCapacity（2）

**声明**

```chali
static DynamicArray<T> withCapacity(int64 capacity);
```

**行为**

用指定 capacity 创建有界容器。

创建空数组并预留容量；负容量报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 容器的最大元素数量 | 无 |

**返回值**

`DynamicArray<T>`。用指定 capacity 创建有界容器。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::array::DynamicArray<T>::withCapacity(capacity);
```

**源码**：[source/std/array.chl:32](../../source/std/array.chl#L32)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-2"></a>

### DynamicArray::len（3）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回有效元素数量。

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

**源码**：[source/std/array.chl:38](../../source/std/array.chl#L38)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-3"></a>

### DynamicArray::capacity（4）

**声明**

```chali
int64 capacity();
```

**行为**

返回已分配的元素槽数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回已分配的元素槽数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.capacity();
```

**源码**：[source/std/array.chl:40](../../source/std/array.chl#L40)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-4"></a>

### DynamicArray::isEmpty（5）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断是否没有有效元素。

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

**源码**：[source/std/array.chl:42](../../source/std/array.chl#L42)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-5"></a>

### DynamicArray::copy（6）

**声明**

```chali
DynamicArray<T> copy();
```

**行为**

返回独立的值副本。

返回独立的值副本，保留长度与容量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`DynamicArray<T>`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/array.chl:44](../../source/std/array.chl#L44)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-6"></a>

### DynamicArray::reserve（7）

**声明**

```chali
void reserve(int64 minimumCapacity);
```

**行为**

至少预留 minimumCapacity 个槽；不改变有效长度，不缩容。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `minimumCapacity` | `int64 minimumCapacity` | 至少需要的元素槽数，必须非负。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.reserve(minimumCapacity);
```

**源码**：[source/std/array.chl:46](../../source/std/array.chl#L46)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-7"></a>

### DynamicArray::push（8）

**声明**

```chali
void push(T value);
```

**行为**

将 value 追加到列表尾部。

追加一个值；容量不足时按两倍增长，容量算术显式检查溢出。

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

**源码**：[source/std/array.chl:56](../../source/std/array.chl#L56)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-8"></a>

### DynamicArray::resize（9）

**声明**

```chali
void resize(int64 length);
```

**行为**

调整有效长度；新增元素默认初始化，缩短时释放移除值，保留容量。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `length` | `int64 length` | 目标有效元素数，必须非负。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.resize(length);
```

**源码**：[source/std/array.chl:67](../../source/std/array.chl#L67)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-9"></a>

### DynamicArray::clear（10）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清除有效元素并保留容量。

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

**源码**：[source/std/array.chl:81](../../source/std/array.chl#L81)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-10"></a>

### DynamicArray::at（11）

**声明**

```chali
T at(int64 position);
```

**行为**

读取从 0 开始的指定位置。

读取有效元素；越界报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(position);
```

**源码**：[source/std/array.chl:83](../../source/std/array.chl#L83)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-11"></a>

### DynamicArray::index（12）

**声明**

```chali
T index(int64 position);
```

**行为**

读取 key 对应的元素，供下标语法调用。

下标读取与 at 相同。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(position);
```

**源码**：[source/std/array.chl:88](../../source/std/array.chl#L88)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-12"></a>

### DynamicArray::set（13）

**声明**

```chali
void set(int64 position, T value);
```

**行为**

将指定位置或共享槽替换为 value。

替换有效元素；越界报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(position, value);
```

**源码**：[source/std/array.chl:90](../../source/std/array.chl#L90)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-13"></a>

### DynamicArray::pop（14）

**声明**

```chali
Option<T> pop();
```

**行为**

移除列表尾元素并返回值，空列表时报错。

弹出末尾元素；空数组返回 None，保留容量。

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

**源码**：[source/std/array.chl:95](../../source/std/array.chl#L95)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-14"></a>

### DynamicArray::iterator（15）

**声明**

```chali
Option<T> iterator(int64 position);
```

**行为**

读取指定迭代位置；无元素时返回 None。

按位置遍历有效元素，结束返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(position);
```

**源码**：[source/std/array.chl:102](../../source/std/array.chl#L102)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。

<a id="api-15"></a>

### DynamicArray::toArray（16）

**声明**

```chali
T[] toArray();
```

**行为**

复制成由调用者作用域管理的独立运行时数组。

导出只含有效元素的独立运行时数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T[]`。复制成由调用者作用域管理的独立运行时数组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toArray();
```

**源码**：[source/std/array.chl:107](../../source/std/array.chl#L107)。

**完整示例**：[example/samples/dynamic_array.chl](../../example/samples/dynamic_array.chl)。
