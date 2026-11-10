# std::memory — 内存视图与统计

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-memory.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::memory;
```

## 公共约定与错误边界

Span<T> 是引用加长度，不拥有缓冲区；iterator 返回元素 mut&。调用者保证底层连续存储有效。缓冲区可用 new 分配，并由普通作用域管理所有权；视图不能超过底层存储的有效期。Rc<T> 在同一线程内共享堆值，clone 增加强引用，最后一个强引用负责清理值。Weak<T> 只保留控制块，upgrade 在值失效后返回空 Rc。Memory.stats 返回宿主分配器统计，不代表整个操作系统进程内存。

## 参考代码

完整程序：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

```powershell
chali run example/samples/std_parse_random_path_memory.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::memory;
import std::parse;
import std::path;
import std::random;

void main() {
    Result<int64, Error> integer = Parse::parseInt64("-42");
    Result<uint64, Error> natural = Parse::parseUInt64("18446744073709551615");
    Result<float64, Error> real = Parse::parseFloat64("12.5");
    Result<bool, Error> flag = Parse::parseBool("true");
    match (integer) {
        Result<int64, Error>::Ok(value) => { println("int64={}", value); }
        Result<int64, Error>::Fail(error) => { println("int64=error"); }
    }
    match (natural) {
        Result<uint64, Error>::Ok(value) => { println("uint64={}", value); }
        Result<uint64, Error>::Fail(error) => { println("uint64=error"); }
    }
    match (real) {
        Result<float64, Error>::Ok(value) => { println("float64={:1F}", value); }
        Result<float64, Error>::Fail(error) => { println("float64=error"); }
    }
    match (flag) {
        Result<bool, Error>::Ok(value) => { println("bool={}", (int64)value); }
        Result<bool, Error>::Fail(error) => { println("bool=error"); }
    }

    Result<int64, Error> empty = Parse::parseInt64("");
    Result<int64, Error> invalid = Parse::parseInt64("4x");
    Result<int64, Error> overflow =
    Parse::parseInt64("999999999999999999999999");
    match (empty) {
        Result<int64, Error>::Ok(value) => { println("empty=bad"); }
        Result<int64, Error>::Fail(error) => {
            match (error) {
                ParseError(detail) => {
                    match(detail) {
                        ParseError::Empty => { println("empty=ok"); }
                        _ => { println("empty=bad"); }
                    }
                }
                _ => { println("empty=bad"); }
            }
        }
    }
    match (invalid) {
        Result<int64, Error>::Ok(value) => { println("invalid=bad"); }
        Result<int64, Error>::Fail(error) => {
            match (error) {
                ParseError(detail) => {
                    match(detail) {
                        ParseError::Invalid => { println("invalid=ok"); }
                        _ => { println("invalid=bad"); }
                    }
                }
                _ => { println("invalid=bad"); }
            }
        }
    }
    match (overflow) {
        Result<int64, Error>::Ok(value) => { println("overflow=bad"); }
        Result<int64, Error>::Fail(error) => {
            match (error) {
                ParseError(detail) => {
                    match(detail) {
                        ParseError::Overflow => { println("overflow=ok"); }
                        _ => { println("overflow=bad"); }
                    }
                }
                _ => { println("overflow=bad"); }
            }
        }
    }

    Random first = Random::withSeed((uint64)7);
    Random second = Random::withSeed((uint64)7);
    uint64 firstValue = first.nextUInt64();
    uint64 secondValue = second.nextUInt64();
    assert(firstValue == secondValue);
    println("random={},{}", firstValue, first.nextInt(-10, 10));
    Random entropy = Random::fromEntropy();
    entropy.nextUInt64();

    String joined = PathString::join("alpha/beta", "../gamma.txt");
    String name = PathString::fileName("alpha/gamma.txt");
    String extension = PathString::extension("alpha/gamma.txt");
    String normalized = PathString::normalize("a\\b/./c/../d");
    println("path={}", joined);
    println("name={} ext={}", name, extension);
    println("normalized={}", normalized);

    int64 before = Memory::stats().liveBytes;
    int64 during = before;
    {
        char bytes[] = new[16];
        Span<char> view = Span<char>::from(&bytes[0], 16);
        assert(view.len() == 16);
        during = Memory::stats().liveBytes;
        assert(during >= before + 16);
    }
    int64 after = Memory::stats().liveBytes;
    assert(after <= during - 16);
    println("memory=ok");
}
```

</details>

<details>
<summary>示例输出</summary>

```text
int64=-42
uint64=18446744073709551615
float64=12.5
bool=1
empty=ok
invalid=ok
overflow=ok
random=7191089600892374487,-6
path=alpha/gamma.txt
name=gamma.txt ext=.txt
normalized=a/b/d
memory=ok
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-350"></a>`struct Span<T>: Iterator<T mut&>, Index<T, int64>` | `Span<T>` 是"指针 + 长度"的视图，所以它的迭代器**返回元素指针**：
`for (T mut& p in span) { *p = …; }` 拿到的是可写元素地址，不复制元素；
只要值就读 `span[i]` 或先 `T v = *p;`。 |
| Span | <a id="symbol-351"></a>`T mut& data` | 视图引用的连续存储；视图不延长来源的生命周期。 |
| Span | <a id="symbol-352"></a>`int64 length` | 连续存储中可访问的元素数量。 |
| 模块 | <a id="symbol-353"></a>`struct MemoryStats` | 当前活跃分配的字节数、块数及历史字节峰值快照。 |
| MemoryStats | <a id="symbol-354"></a>`int64 liveBytes` | 当前活跃分配的字节数。 |
| MemoryStats | <a id="symbol-355"></a>`int64 liveBlocks` | 当前活跃分配的块数。 |
| MemoryStats | <a id="symbol-356"></a>`int64 peakBytes` | 历史活跃分配字节数的峰值。 |
| 模块 | <a id="symbol-357"></a>`struct Memory` | 内存统计与宿主内存管理操作的静态入口。 |
| 模块 | <a id="symbol-358"></a>`struct Rc<T>: Closeable` | 同一线程内共享堆值的强引用。默认构造为空，使用 clone 显式增加拥有者。 |
| 模块 | <a id="symbol-359"></a>`struct Weak<T>: Closeable` | 同一线程内观察 Rc 值的弱引用。可通过 upgrade 尝试取得拥有者。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Span::from](#api-0) | `static Span<T> from(T mut& data, int64 length)` |
| [Span::index](#api-1) | `T index(int64 key)` |
| [Span::iterator](#api-2) | `Option<T mut&> iterator(int64 index)` |
| [Span::len](#api-3) | `int64 len()` |
| [Memory::stats](#api-4) | `static MemoryStats stats()` |
| [Rc::isEmpty](#api-5) | `bool isEmpty()` |
| [Rc::clone](#api-6) | `Rc<T> clone()` |
| [Rc::get](#api-7) | `T get()` |
| [Rc::downgrade](#api-8) | `Weak<T> downgrade()` |
| [Rc::close](#api-9) | `void close()` |
| [Weak::isExpired](#api-10) | `bool isExpired()` |
| [Weak::upgrade](#api-11) | `Rc<T> upgrade()` |
| [Weak::close](#api-12) | `void close()` |
| [rc](#api-13) | `Rc<T> rc<T>(T value)` |

<a id="api-0"></a>

### Span::from（1）

**声明**

```chali
static Span<T> from(T mut& data, int64 length);
```

**行为**

用首元素引用和长度创建非拥有 Span 视图。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T mut& data` | 连续存储的首元素引用，调用者保证范围有效 | 无 |
| `length` | `int64 length` | 目标有效元素数，必须非负。 | 无 |

**返回值**

`Span<T>`。用首元素引用和长度创建非拥有 Span 视图。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`length >= 0`。

**调用形式**

```chali
Span<T>::from(data, length);
```

**源码**：[source/std/memory.chl:11](../../source/std/memory.chl#L11)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-1"></a>

### Span::index（2）

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
显式前置检查：`key >= 0 && key < self.length`。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/memory.chl:19](../../source/std/memory.chl#L19)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-2"></a>

### Span::iterator（3）

**声明**

```chali
Option<T mut&> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<T mut&>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/memory.chl:23](../../source/std/memory.chl#L23)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-3"></a>

### Span::len（4）

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

**源码**：[source/std/memory.chl:27](../../source/std/memory.chl#L27)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-4"></a>

### Memory::stats（5）

**声明**

```chali
static MemoryStats stats();
```

**行为**

返回当前分配器的活动字节、活动块及峰值统计。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`MemoryStats`。返回当前分配器的活动字节、活动块及峰值统计。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Memory::stats();
```

**源码**：[source/std/memory.chl:41](../../source/std/memory.chl#L41)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-5"></a>

### Rc::isEmpty（6）

**声明**

```chali
bool isEmpty();
```

**行为**

查询强引用句柄是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询强引用句柄是否为空。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/memory.chl:74](../../source/std/memory.chl#L74)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-6"></a>

### Rc::clone（7）

**声明**

```chali
Rc<T> clone();
```

**行为**

返回共享同一值的新拥有者。空句柄返回空句柄。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Rc<T>`。返回共享同一值的新拥有者。空句柄返回空句柄。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`state.strong < 9223372036854775807, "Rc 强引用计数溢出"`。

**调用形式**

```chali
obj.clone();
```

**源码**：[source/std/memory.chl:77](../../source/std/memory.chl#L77)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-7"></a>

### Rc::get（8）

**声明**

```chali
T get();
```

**行为**

返回盒中可复制值的副本。空句柄触发断言。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。返回盒中可复制值的副本。空句柄触发断言。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`!self.isEmpty(), "Rc 为空"`。

**调用形式**

```chali
obj.get();
```

**源码**：[source/std/memory.chl:91](../../source/std/memory.chl#L91)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-8"></a>

### Rc::downgrade（9）

**声明**

```chali
Weak<T> downgrade();
```

**行为**

创建弱引用。空句柄产生空弱引用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Weak<T>`。创建弱引用。空句柄产生空弱引用。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`state.weak < 9223372036854775807, "Rc 弱引用计数溢出"`。

**调用形式**

```chali
obj.downgrade();
```

**源码**：[source/std/memory.chl:97](../../source/std/memory.chl#L97)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-9"></a>

### Rc::close（10）

**声明**

```chali
void close();
```

**行为**

释放当前强引用；最后一个强引用负责销毁值。

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

**源码**：[source/std/memory.chl:111](../../source/std/memory.chl#L111)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-10"></a>

### Weak::isExpired（11）

**声明**

```chali
bool isExpired();
```

**行为**

值已经销毁或句柄为空时返回 true。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。值已经销毁或句柄为空时返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isExpired();
```

**源码**：[source/std/memory.chl:132](../../source/std/memory.chl#L132)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-11"></a>

### Weak::upgrade（12）

**声明**

```chali
Rc<T> upgrade();
```

**行为**

值仍存活时返回新的强引用，否则返回空 Rc。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Rc<T>`。值仍存活时返回新的强引用，否则返回空 Rc。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`state.strong < 9223372036854775807, "Rc 强引用计数溢出"`。

**调用形式**

```chali
obj.upgrade();
```

**源码**：[source/std/memory.chl:138](../../source/std/memory.chl#L138)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-12"></a>

### Weak::close（13）

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

**源码**：[source/std/memory.chl:152](../../source/std/memory.chl#L152)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-13"></a>

### rc（14）

**声明**

```chali
Rc<T> rc<T>(T value);
```

**行为**

创建第一个拥有堆值的强引用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 要交给引用计数管理的值。 | 无 |

**返回值**

`Rc<T>`。拥有该值的 Rc。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
rc(value);
```

**源码**：[source/std/memory.chl:164](../../source/std/memory.chl#L164)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

## 补充完整示例

[example/samples/std_rc.chl](../../example/samples/std_rc.chl)。

```chali
import std::memory;

int64 closed = 0;

struct Resource : Closeable {
    int64 id;
    void close() { closed += self.id; }
};

enum SharedResource { Owned(Rc<Resource> owner, Weak<Resource> observer); };

void unpack(SharedResource payload) {
    match (move payload) {
        Owned(_, observer) if observer.isExpired() => { assert(false); }
        Owned(owner, observer) => {
            assert(!observer.isExpired());
            Rc<Resource> moved = move owner;
            moved.close();
            assert(observer.isExpired());
        }
    }
}

void main() {
    int64 before = Memory::stats().liveBlocks;
    {
        Rc<int64> first = rc(42);
        let second = first.clone();
        let observer = first.downgrade();
        first.close();
        assert(second.get() == 42);
        assert(!observer.isExpired());
        let third = observer.upgrade();
        second.close();
        assert(third.get() == 42);
        third.close();
        assert(observer.isExpired());
        let expired = observer.upgrade();
        assert(expired.isEmpty());
    }
    {
        Weak<Resource> observer;
        {
            Resource resource;
            resource.id = 7;
            let first = rc(move resource);
            let second = first.clone();
            observer = second.downgrade();
            first.close();
            assert(closed == 0);
            assert(!observer.isExpired());
        }
        assert(closed == 7);
        assert(observer.isExpired());
        observer.close();
        observer.close();
    }
    {
        Weak<int64> empty;
        assert(empty.isExpired());
    }
    {
        Resource resource;
        resource.id = 13;
        let owner = rc(move resource);
        let observer = owner.downgrade();
        unpack(SharedResource::Owned(move owner, move observer));
        assert(closed == 20);
    }
    assert(Memory::stats().liveBlocks == before);
    println("Rc and Weak OK");
}
```
