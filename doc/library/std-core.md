# std::core — 核心库

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-core.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
// std::core 自动加载
```

## 公共约定与错误边界

自动加载，无需 import。错误输出包装和浮点去尾零由 ChaLi 实现；LLVM 构建的标准流直连运行时与 libc，显式执行步数限制保留宿主回退。标准库失败返回统一为 Result<T, Error>，具体错误实现 Error.toString()；文本诊断使用 MessageError，保留原始消息。String 提供字节查找、切片、替换、修剪、填充、连接、分割、ASCII 大小写与分类，以及严格 UTF-8 码点操作；普通方法返回独立值，字节索引与码点索引明确区分，详情见字符串方法文档。Option/Result 通过 match 或 isValue/isOk 分支处理；unwrap/expect 在失败分支触发 panic。浮点转整数向零截断，范围检查见各重载；字节编码默认小端。拥有值按值复制，引用的生命周期不得超过来源。 Box<T> 独占堆上的一个值，由 Box<T>::newBox(value) 装箱。get 复制值，take 消耗盒并移出值；两者在空盒上触发断言。close 和作用域退出清理剩余值。List.pop 空列表报错，列表/集合/映射的按位置访问需有效下标。Set/Map 的 has/contains 返回 int64 的 0/1；List 的同名接口返回 bool。Any.getReference/getOptional 的 const/mut 由接收上下文决定，返回引用受盒子寿命限制。内存内建接受有效存储引用，字节计数不是元素数量。$assert(condition) 和 $assert(condition,message) 为宏调用，自动添加调用点行号，宏体自带分号。

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

## 宏定义

<a id="symbol-61"></a>

### `assert`

```chali
$macro assert(condition)
```

系统断言宏：`$assert(condition)` / `$assert(condition, message)` 把**调用点**的行号 喂给上面带 `int64 line` 的重载。`$line` 在宏展开的位置取值，所以拿到的是调用处那一 行；限定名 `std::core::assert` 不会被这个同名宏吃掉（`.name` / `::name` 不当宏调用）。 宏体保存完整语句；调用处也写分号，与其他语句宏保持一致。 定义放在文件末尾：它上面那几行是 `assert` 函数自己的**定义**，不能被动到。

<a id="symbol-62"></a>

### `assert`

```chali
$macro assert(condition, message)
```

检查条件；失败时报告消息和宏调用处的源码行。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-63"></a>`opcode String` | 拥有 UTF-8 文本的 opcode 值，复制产生独立可写缓冲。 |
| 模块 | <a id="symbol-64"></a>`interface Debug` | 开发时的结构化显示协议；输出格式不保证跨版本稳定。 |
| 模块 | <a id="symbol-65"></a>`interface Error` | 标准库错误的统一接口；诊断文本保留具体失败原因。 |
| 模块 | <a id="symbol-66"></a>`struct MessageError : Error` | 只有文本诊断时使用的标准错误对象，消息按值独立拥有。 |
| MessageError | <a id="symbol-67"></a>`String message` | 供用户阅读的错误原因。 |
| 模块 | <a id="symbol-68"></a>`#NonCopyable opcode File` | 不可复制的文件 opcode，作用域退出释放文件资源。 |
| 模块 | <a id="symbol-69"></a>`#NonCopyable opcode Process` | 不可复制的进程 opcode，操作由宿主胶水提供。 |
| 模块 | <a id="symbol-70"></a>`enum TryBranch<T, R>` | 传播分支：Continue 产生成功值，Break 携带失败载荷退出当前函数。 |
| TryBranch | `Continue(T value), Break(R residual)` | 传播分支：Continue 产生成功值，Break 携带失败载荷退出当前函数。 变体及载荷见声明。 |
| 模块 | <a id="symbol-71"></a>`struct ErrorResidual<E>` | Result 的错误传播载荷，保留错误类型身份。 |
| ErrorResidual | <a id="symbol-72"></a>`E error` | 最近操作的错误说明。 |
| 模块 | <a id="symbol-73"></a>`enum NoneResidual` | Option 无值分支的传播载荷。 |
| NoneResidual | `None` | Option 无值分支的传播载荷。 变体及载荷见声明。 |
| 模块 | <a id="symbol-74"></a>`interface Try<T, R>` | 只读拆分传播值的显式编译期协议。 |
| 模块 | <a id="symbol-75"></a>`interface TryMove<T, R>` | 移动拥有值后拆分传播值的显式编译期协议。 |
| 模块 | <a id="symbol-76"></a>`interface FromResidual<F, R>` | 以静态方法从传播载荷构造函数返回值的编译期协议。 |
| 模块 | <a id="symbol-77"></a>`enum Option<T> : Try<T, NoneResidual>, TryMove<T, NoneResidual>, FromResidual<Option<T>, NoneResidual>` | 可选值：Value(T) 表示存在，None 表示缺失。 |
| Option | `Value(T value), None` | 可选值：Value(T) 表示存在，None 表示缺失。 变体及载荷见声明。 |
| 模块 | <a id="symbol-78"></a>`enum Result<T, E> : Try<T, ErrorResidual<E>>, TryMove<T, ErrorResidual<E>>, FromResidual<Result<T, E>, ErrorResidual<E>>` | 操作结果：Ok(T) 表示成功，Err(E) 保存错误。 |
| Result | `Ok(T value), Err(E error)` | 操作结果：Ok(T) 表示成功，Err(E) 保存错误。 变体及载荷见声明。 |
| 模块 | <a id="symbol-79"></a>`interface NextValue<T>` | 自定义区间的有状态前进协议。
@note nextValue 从接收者的游标状态产生下一项并推进状态；到达终点后返回 None。 |
| 模块 | <a id="symbol-80"></a>`struct Pair<K, V>` | 包含 first 和 second 的两个值，分别使用 K 与 V 类型。 |
| Pair | <a id="symbol-81"></a>`K first` | 第一项，类型为 K。 |
| Pair | <a id="symbol-82"></a>`V second` | 第二项，类型为 V。 |
| 模块 | <a id="symbol-83"></a>`struct RawPtr<T>` | 有元素类型及分配身份的原始指针；复制指针不复制元素，也不延长存储寿命。 |
| RawPtr | <a id="symbol-84"></a>`uint64 storage` | 对应原始存储的分配身份，不延长存储寿命。 |
| RawPtr | <a id="symbol-85"></a>`int64 index` | 当前元素序号，从 0 开始。 |
| 模块 | <a id="symbol-86"></a>`struct BarePtr<T>: Add<BarePtr<T>, int64>, RightAdd<BarePtr<T>, int64>, Subtract<BarePtr<T>, int64>, Subtract<int64, BarePtr<T>>` | 只保存地址的裸指针；不记录运行时元素类型或初始化状态，也不拥有分配。 |
| BarePtr | <a id="symbol-87"></a>`int64 address` | 运行时逻辑字节地址；不是可传给宿主 C 函数的机器地址。 |
| 模块 | <a id="symbol-88"></a>`struct RawStorage<T>: Closeable` | 独占原始容量；备用槽位不默认构造 T，关闭时仅清理已初始化元素。 |
| RawStorage | <a id="symbol-89"></a>`uint64 handle` | 宿主资源句柄；应通过所属类型的方法操作。 |
| 模块 | <a id="symbol-90"></a>`struct Box<T>: Closeable` | 独占堆上的一个值。默认构造为空，移动时转移所有权，离开作用域时自动释放。 |
| 模块 | <a id="symbol-91"></a>`interface Drop` | 显式释放拥有资源的析构协议。 |
| 模块 | <a id="symbol-92"></a>`interface Closeable` | 关闭外部资源的协议。 |
| 模块 | <a id="symbol-93"></a>`struct List<T>` | 可变长度的顺序值容器，支持按下标访问、追加、删除与复制。 |
| 模块 | <a id="symbol-94"></a>`struct Set<T>` | 保存唯一元素的值集合，支持存在检查和集合运算。 |
| 模块 | <a id="symbol-95"></a>`struct Map<K,V>` | 从唯一键到值的映射，支持键查询和按遍历位置读取。 |
| 模块 | <a id="symbol-96"></a>`struct Any` | 拥有一个装箱值的类型擦除容器；通过类型检查安全读取。 |
| 模块 | <a id="symbol-97"></a>`interface Add<R,T>` | 左操作数的加法重载接口。 |
| 模块 | <a id="symbol-98"></a>`interface RightAdd<R,T>` | 右操作数的加法重载接口。 |
| 模块 | <a id="symbol-99"></a>`interface Subtract<R,T>` | 左操作数的减法重载接口。 |
| 模块 | <a id="symbol-100"></a>`interface RightSubtract<R,T>` | 右操作数的减法重载接口。 |
| 模块 | <a id="symbol-101"></a>`interface Multiply<R,T>` | 左操作数的乘法重载接口。 |
| 模块 | <a id="symbol-102"></a>`interface RightMultiply<R,T>` | 右操作数的乘法重载接口。 |
| 模块 | <a id="symbol-103"></a>`interface Divide<R,T>` | 左操作数的除法重载接口。 |
| 模块 | <a id="symbol-104"></a>`interface RightDivide<R,T>` | 右操作数的除法重载接口。 |
| 模块 | <a id="symbol-105"></a>`interface Modulo<R,T>` | 左操作数的取模重载接口。 |
| 模块 | <a id="symbol-106"></a>`interface RightModulo<R,T>` | 右操作数的取模重载接口。 |
| 模块 | <a id="symbol-107"></a>`interface Equal<R,T>` | 左操作数的相等比较重载接口。 |
| 模块 | <a id="symbol-108"></a>`interface RightEqual<R,T>` | 右操作数的相等比较重载接口。 |
| 模块 | <a id="symbol-109"></a>`interface NotEqual<R,T>` | 左操作数的不等比较重载接口。 |
| 模块 | <a id="symbol-110"></a>`interface RightNotEqual<R,T>` | 右操作数的不等比较重载接口。 |
| 模块 | <a id="symbol-111"></a>`interface Less<R,T>` | 左操作数的小于比较重载接口。 |
| 模块 | <a id="symbol-112"></a>`interface RightLess<R,T>` | 右操作数的小于比较重载接口。 |
| 模块 | <a id="symbol-113"></a>`interface LessOrEqual<R,T>` | 左操作数的小于等于比较重载接口。 |
| 模块 | <a id="symbol-114"></a>`interface RightLessOrEqual<R,T>` | 右操作数的小于等于比较重载接口。 |
| 模块 | <a id="symbol-115"></a>`interface Greater<R,T>` | 左操作数的大于比较重载接口。 |
| 模块 | <a id="symbol-116"></a>`interface RightGreater<R,T>` | 右操作数的大于比较重载接口。 |
| 模块 | <a id="symbol-117"></a>`interface GreaterOrEqual<R,T>` | 左操作数的大于等于比较重载接口。 |
| 模块 | <a id="symbol-118"></a>`interface RightGreaterOrEqual<R,T>` | 右操作数的大于等于比较重载接口。 |
| 模块 | <a id="symbol-119"></a>`interface And<R,T>` | 左操作数的按位与重载接口。 |
| 模块 | <a id="symbol-120"></a>`interface RightAnd<R,T>` | 右操作数的按位与重载接口。 |
| 模块 | <a id="symbol-121"></a>`interface Or<R,T>` | 左操作数的按位或重载接口。 |
| 模块 | <a id="symbol-122"></a>`interface RightOr<R,T>` | 右操作数的按位或重载接口。 |
| 模块 | <a id="symbol-123"></a>`interface Xor<R,T>` | 左操作数的按位异或重载接口。 |
| 模块 | <a id="symbol-124"></a>`interface RightXor<R,T>` | 右操作数的按位异或重载接口。 |
| 模块 | <a id="symbol-125"></a>`interface ShiftLeft<R,T>` | 左操作数的左移重载接口。 |
| 模块 | <a id="symbol-126"></a>`interface RightShiftLeft<R,T>` | 右操作数的左移重载接口。 |
| 模块 | <a id="symbol-127"></a>`interface ShiftRight<R,T>` | 左操作数的右移重载接口。 |
| 模块 | <a id="symbol-128"></a>`interface RightShiftRight<R,T>` | 右操作数的右移重载接口。 |
| 模块 | <a id="symbol-129"></a>`interface Negate<R,T>` | 左操作数的取负重载接口。 |
| 模块 | <a id="symbol-130"></a>`interface BitNot<R,T>` | 左操作数的按位取反重载接口。 |
| 模块 | <a id="symbol-131"></a>`interface Positive<R,T>` | 左操作数的取正重载接口。 |
| 模块 | <a id="symbol-132"></a>`interface Hash<T>` | 为值提供哈希计算的接口。 |
| 模块 | <a id="symbol-133"></a>`interface Copy<T>` | 显式独立值复制协议，不改变赋值规则。 |
| 模块 | <a id="symbol-134"></a>`interface ExplicitConvert<T>` | 显式类型转换的接口。 |
| 模块 | <a id="symbol-135"></a>`interface ImplicitConvert<T>` | 隐式类型转换的接口。 |
| 模块 | <a id="symbol-136"></a>`interface Index<R,T>` | 自定义下标访问接口；与小写 index 连续存储函数区分。 |
| 模块 | <a id="symbol-137"></a>`interface Iterator<R>` | 供 for-in 获取和推进迭代状态的接口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [rawSize](#api-0) | `int64 rawSize<T>()` |
| [String::length](#api-1) | `int64 length()` |
| [String::size](#api-2) | `int64 size()` |
| [String::byteLength](#api-3) | `int64 byteLength()` |
| [String::capacity](#api-4) | `int64 capacity()` |
| [String::maxCapacity](#api-5) | `int64 maxCapacity()` |
| [String::reserve](#api-6) | `void reserve(int64 bytes)` |
| [String::append](#api-7) | `void append(String text)` |
| [String::clear](#api-8) | `void clear()` |
| [String::truncate](#api-9) | `void truncate(int64 bytes)` |
| [String::isEmpty](#api-10) | `bool isEmpty()` |
| [String::isBlank](#api-11) | `bool isBlank()` |
| [String::isDigit](#api-12) | `bool isDigit()` |
| [String::isAlpha](#api-13) | `bool isAlpha()` |
| [String::isAlnum](#api-14) | `bool isAlnum()` |
| [String::isAscii](#api-15) | `bool isAscii()` |
| [String::charLength](#api-16) | `int64 charLength()` |
| [String::indexOf](#api-17) | `int64 indexOf(String text, int64 start = 0)` |
| [String::lastIndexOf](#api-18) | `int64 lastIndexOf(String text, int64 start = 0 - 1)` |
| [String::contains](#api-19) | `bool contains(String text)` |
| [String::startsWith](#api-20) | `bool startsWith(String text)` |
| [String::endsWith](#api-21) | `bool endsWith(String text)` |
| [String::count](#api-22) | `int64 count(String text)` |
| [String::compare](#api-23) | `int64 compare(String text)` |
| [String::byteAt](#api-24) | `Option<uint8> byteAt(int64 index)` |
| [String::substring](#api-25) | `String substring(int64 start, int64 count = 0 - 1)` |
| [String::slice](#api-26) | `String slice(int64 start, int64 end)` |
| [String::left](#api-27) | `String left(int64 count)` |
| [String::right](#api-28) | `String right(int64 count)` |
| [String::trim](#api-29) | `String trim()` |
| [String::trimStart](#api-30) | `String trimStart()` |
| [String::trimEnd](#api-31) | `String trimEnd()` |
| [String::toLower](#api-32) | `String toLower()` |
| [String::toUpper](#api-33) | `String toUpper()` |
| [String::replaceFirst](#api-34) | `String replaceFirst(String before, String after)` |
| [String::replaceAll](#api-35) | `String replaceAll(String before, String after)` |
| [String::replace](#api-36) | `String replace(String before, String after)` |
| [String::repeat](#api-37) | `String repeat(int64 count)` |
| [String::reverse](#api-38) | `String reverse()` |
| [String::padStart](#api-39) | `String padStart(int64 length, String fill = " ")` |
| [String::padEnd](#api-40) | `String padEnd(int64 length, String fill = " ")` |
| [String::removePrefix](#api-41) | `String removePrefix(String prefix)` |
| [String::removeSuffix](#api-42) | `String removeSuffix(String suffix)` |
| [String::insert](#api-43) | `String insert(int64 index, String text)` |
| [String::remove](#api-44) | `String remove(int64 start, int64 count = 0 - 1)` |
| [String::utf8Substring](#api-45) | `String utf8Substring(int64 start, int64 count = 0 - 1)` |
| [String::charAt](#api-46) | `Option<String> charAt(int64 index)` |
| [String::split](#api-47) | `List<String> split(String separator, bool keepEmpty = true)` |
| [String::splitWhitespace](#api-48) | `List<String> splitWhitespace()` |
| [String::lines](#api-49) | `List<String> lines(bool keepEmpty = true)` |
| [String::unicodeChars](#api-50) | `List<String> unicodeChars()` |
| [String::bytes](#api-51) | `List<uint8> bytes()` |
| [String::join](#api-52) | `String join(List<String> items)` |
| [String::concat](#api-53) | `String concat(String text)` |
| [String::prepend](#api-54) | `String prepend(String text)` |
| [Debug::debug](#api-55) | `String debug()` |
| [Error::toString](#api-56) | `String toString()` |
| [MessageError::toString](#api-57) | `String toString()` |
| [MessageError::from](#api-58) | `static MessageError from(String text)` |
| [File::print](#api-59) | `int64 print(String text)` |
| [File::println](#api-60) | `int64 println(String text = "")` |
| [readLine](#api-61) | `String readLine()` |
| [readLines](#api-62) | `String readLines(int32 N)` |
| [readLines](#api-63) | `String readLines(int64 N)` |
| [toInt32](#api-64) | `Option<int32> toInt32(float64 value)` |
| [toInt32](#api-65) | `Option<int32> toInt32(float32 value)` |
| [toInt64](#api-66) | `int64 toInt64(float64 value)` |
| [toInt64](#api-67) | `int64 toInt64(float32 value)` |
| [float32ToBytes](#api-68) | `List<uint8> float32ToBytes(float32 value, bool bigEndian)` |
| [float32ToBytes](#api-69) | `List<uint8> float32ToBytes(float32 value)` |
| [bytesToFloat32](#api-70) | `Option<float32> bytesToFloat32(List<uint8> bytes, bool bigEndian)` |
| [bytesToFloat32](#api-71) | `Option<float32> bytesToFloat32(List<uint8> bytes)` |
| [bytesToFloat32](#api-72) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count, bool bigEndian)` |
| [bytesToFloat32](#api-73) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count)` |
| [bytesToFloat32](#api-74) | `Option<float32> bytesToFloat32(uint8 bytes[], bool bigEndian)` |
| [bytesToFloat32](#api-75) | `Option<float32> bytesToFloat32(uint8 bytes[])` |
| [float32ToBytes](#api-76) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [float32ToBytes](#api-77) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count)` |
| [float32ToBytes](#api-78) | `bool float32ToBytes(float32 value, uint8[] mut& bytes, bool bigEndian)` |
| [float32ToBytes](#api-79) | `bool float32ToBytes(float32 value, uint8[] mut& bytes)` |
| [float64ToBytes](#api-80) | `List<uint8> float64ToBytes(float64 value, bool bigEndian)` |
| [float64ToBytes](#api-81) | `List<uint8> float64ToBytes(float64 value)` |
| [bytesToFloat64](#api-82) | `Option<float64> bytesToFloat64(List<uint8> bytes, bool bigEndian)` |
| [bytesToFloat64](#api-83) | `Option<float64> bytesToFloat64(List<uint8> bytes)` |
| [bytesToFloat64](#api-84) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count, bool bigEndian)` |
| [bytesToFloat64](#api-85) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count)` |
| [bytesToFloat64](#api-86) | `Option<float64> bytesToFloat64(uint8 bytes[], bool bigEndian)` |
| [bytesToFloat64](#api-87) | `Option<float64> bytesToFloat64(uint8 bytes[])` |
| [float64ToBytes](#api-88) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [float64ToBytes](#api-89) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count)` |
| [float64ToBytes](#api-90) | `bool float64ToBytes(float64 value, uint8[] mut& bytes, bool bigEndian)` |
| [float64ToBytes](#api-91) | `bool float64ToBytes(float64 value, uint8[] mut& bytes)` |
| [assert](#api-92) | `void assert(bool condition)` |
| [assert](#api-93) | `void assert(bool condition, String message)` |
| [assert](#api-94) | `void assert(bool condition, int64 line)` |
| [assert](#api-95) | `void assert(bool condition, String message, int64 line)` |
| [Try::branch](#api-96) | `TryBranch<T, R> branch()` |
| [TryMove::intoBranch](#api-97) | `TryBranch<T, R> intoBranch()` |
| [FromResidual::fromResidual](#api-98) | `static F fromResidual(R residual)` |
| [Option::branch](#api-99) | `TryBranch<T, NoneResidual> branch()` |
| [Option::intoBranch](#api-100) | `TryBranch<T, NoneResidual> intoBranch()` |
| [Option::fromResidual](#api-101) | `static Option<T> fromResidual(NoneResidual residual)` |
| [Option::isValue](#api-102) | `bool isValue()` |
| [Option::isNone](#api-103) | `bool isNone()` |
| [Option::take](#api-104) | `T take()` |
| [Option::unwrap](#api-105) | `T unwrap()` |
| [Option::unwrapOr](#api-106) | `T unwrapOr(T fallback)` |
| [Option::expect](#api-107) | `T expect(String message)` |
| [Option::transform](#api-108) | `Option<T> transform(T fn(T) mapper)` |
| [Option::andThen](#api-109) | `Option<T> andThen(Option<T> fn(T) mapper)` |
| [Result::branch](#api-110) | `TryBranch<T, ErrorResidual<E>> branch()` |
| [Result::intoBranch](#api-111) | `TryBranch<T, ErrorResidual<E>> intoBranch()` |
| [Result::fromResidual](#api-112) | `static Result<T,E> fromResidual(ErrorResidual<E> residual)` |
| [Result::ok](#api-113) | `Option<T> ok()` |
| [Result::isOk](#api-114) | `bool isOk()` |
| [Result::isErr](#api-115) | `bool isErr()` |
| [Result::take](#api-116) | `T take()` |
| [Result::unwrap](#api-117) | `T unwrap()` |
| [Result::unwrapOr](#api-118) | `T unwrapOr(T fallback)` |
| [Result::expect](#api-119) | `T expect(String message)` |
| [Result::takeError](#api-120) | `E takeError()` |
| [Result::error](#api-121) | `E error()` |
| [Result::transform](#api-122) | `Result<T, E> transform(T fn(T) mapper)` |
| [Result::andThen](#api-123) | `Result<T, E> andThen(Result<T, E> fn(T) mapper)` |
| [NextValue::nextValue](#api-124) | `Option<T> nextValue(T end, bool inclusive)` |
| [Pair::create](#api-125) | `static Pair<K, V> create(K first, V second)` |
| [Pair::create](#api-126) | `static Pair<K, V> create((K, V) value)` |
| [Pair::toTuple](#api-127) | `(K, V) toTuple()` |
| [isType](#api-128) | `bool isType<T>(Any const& value)` |
| [tryGet](#api-129) | `Option<T> tryGet<T>(Any const& value)` |
| [tryGetReference](#api-130) | `Option<T const&> tryGetReference<T>(Any const& value)` |
| [tryGetMut](#api-131) | `Option<T mut&> tryGetMut<T>(Any mut& value)` |
| [stringBytes](#api-132) | `List<uint8> stringBytes(String text)` |
| [BarePtr::leftAdd](#api-133) | `BarePtr<T> leftAdd(int64 elements)` |
| [BarePtr::rightAdd](#api-134) | `BarePtr<T> rightAdd(int64 elements)` |
| [BarePtr::leftSub](#api-135) | `BarePtr<T> leftSub(int64 elements)` |
| [BarePtr::leftSub](#api-136) | `int64 leftSub(BarePtr<T> other)` |
| [barePointer](#api-137) | `unsafe BarePtr<T> barePointer<T>(RawPtr<T> pointer)` |
| [pointerFromAddress](#api-138) | `unsafe BarePtr<T> pointerFromAddress<T>(int64 address)` |
| [dereference](#api-139) | `unsafe T mut& dereference<T>(BarePtr<T> pointer)` |
| [memcpy](#api-140) | `unsafe void memcpy<T,U>(BarePtr<T> destination, BarePtr<U> source, int64 bytes)` |
| [memset](#api-141) | `unsafe void memset<T>(BarePtr<T> destination, uint8 value, int64 bytes)` |
| [initAt](#api-142) | `unsafe void initAt<T>(RawPtr<T> pointer, T value)` |
| [readAt](#api-143) | `unsafe T readAt<T>(RawPtr<T> pointer)` |
| [borrowAt](#api-144) | `unsafe T mut& borrowAt<T>(RawPtr<T> pointer)` |
| [takeAt](#api-145) | `unsafe T takeAt<T>(RawPtr<T> pointer)` |
| [dropAt](#api-146) | `unsafe void dropAt<T>(RawPtr<T> pointer)` |
| [relocateNonOverlapping](#api-147) | `unsafe void relocateNonOverlapping<T>(RawPtr<T> destination, RawPtr<T> source, int64 count)` |
| [RawStorage::allocate](#api-148) | `static RawStorage<T> allocate(int64 capacity)` |
| [RawStorage::capacity](#api-149) | `int64 capacity()` |
| [RawStorage::pointer](#api-150) | `RawPtr<T> pointer(int64 index)` |
| [RawStorage::isInitialized](#api-151) | `bool isInitialized(int64 index)` |
| [RawStorage::close](#api-152) | `void close()` |
| [Box::newBox](#api-153) | `static Box<T> newBox(T value)` |
| [Box::isEmpty](#api-154) | `bool isEmpty()` |
| [Box::get](#api-155) | `T get()` |
| [Box::take](#api-156) | `T take()` |
| [Box::close](#api-157) | `void close()` |
| [range](#api-158) | `__Range range(int64 start, int64 stop)` |
| [range](#api-159) | `__Range range(int64 start, int64 stop, int64 step)` |
| [rangeEq](#api-160) | `__Interval_int64 rangeEq(int64 start, int64 stop)` |
| [Drop::drop](#api-161) | `void drop()` |
| [Closeable::close](#api-162) | `void close()` |
| [print](#api-163) | `int64 print(String format, Any args...)` |
| [println](#api-164) | `int64 println(String format, Any args...)` |
| [eprint](#api-165) | `int64 eprint(String format, Any args...)` |
| [eprintln](#api-166) | `int64 eprintln(String format, Any args...)` |
| [format](#api-167) | `String format(String format, Any args...)` |
| [putChar](#api-168) | `int64 putChar(char value)` |
| [pause](#api-169) | `int64 pause()` |
| [inputLine](#api-170) | `String inputLine()` |
| [inputInt](#api-171) | `int64 inputInt()` |
| [inputUInt](#api-172) | `uint64 inputUInt()` |
| [inputFloat](#api-173) | `float64 inputFloat()` |
| [inputBool](#api-174) | `bool inputBool()` |
| [inputChar](#api-175) | `char inputChar()` |
| [inputBigInteger](#api-176) | `BigInteger inputBigInteger()` |
| [inputBigDecimal](#api-177) | `BigDecimal inputBigDecimal()` |
| [panic](#api-178) | `int64 panic(String message)` |
| [len](#api-179) | `int64 len(Any value)` |
| [toReference](#api-180) | `T mut& toReference<T>(Option<T mut&> value)` |
| [toOptional](#api-181) | `Option<T mut&> toOptional<T>(T mut& value)` |
| [memcpy](#api-182) | `int64 memcpy(char mut& destination, char const& source, int64 bytes)` |
| [memset](#api-183) | `int64 memset(char mut& destination, int64 value, int64 bytes)` |
| [List::copy](#api-184) | `List<T> copy()` |
| [List::push](#api-185) | `int64 push(T value)` |
| [List::insert](#api-186) | `int64 insert(int64 index, T value)` |
| [List::at](#api-187) | `T at(int64 index)` |
| [List::pop](#api-188) | `T pop()` |
| [List::remove](#api-189) | `int64 remove(int64 index)` |
| [List::indexOf](#api-190) | `int64 indexOf(T value)` |
| [List::contains](#api-191) | `bool contains(T value)` |
| [List::has](#api-192) | `bool has(T value)` |
| [List::clear](#api-193) | `int64 clear()` |
| [List::len](#api-194) | `int64 len()` |
| [Set::copy](#api-195) | `Set<T> copy()` |
| [Set::add](#api-196) | `int64 add(T value)` |
| [Set::has](#api-197) | `int64 has(T value)` |
| [Set::contains](#api-198) | `int64 contains(T value)` |
| [Set::remove](#api-199) | `int64 remove(T value)` |
| [Set::at](#api-200) | `T at(int64 index)` |
| [Set::clear](#api-201) | `int64 clear()` |
| [Set::len](#api-202) | `int64 len()` |
| [Map::copy](#api-203) | `Map<K,V> copy()` |
| [Map::put](#api-204) | `int64 put(K key,V value)` |
| [Map::has](#api-205) | `int64 has(K key)` |
| [Map::contains](#api-206) | `int64 contains(K key)` |
| [Map::remove](#api-207) | `int64 remove(K key)` |
| [Map::keyAt](#api-208) | `K keyAt(int64 index)` |
| [Map::valAt](#api-209) | `V valAt(int64 index)` |
| [Map::clear](#api-210) | `int64 clear()` |
| [Map::len](#api-211) | `int64 len()` |
| [Any::copy](#api-212) | `Any copy()` |
| [Any::isEmpty](#api-213) | `bool isEmpty()` |
| [Any::clear](#api-214) | `void clear()` |
| [Any::set](#api-215) | `int64 set(T value)` |
| [Any::getValue](#api-216) | `T getValue()` |
| [Any::getReference](#api-217) | `T mut& getReference()` |
| [Any::getOptional](#api-218) | `Option<T mut&> getOptional()` |
| [eq](#api-219) | `bool eq(T left,T right)` |
| [ne](#api-220) | `bool ne(T left,T right)` |
| [lt](#api-221) | `bool lt(T left,T right)` |
| [le](#api-222) | `bool le(T left,T right)` |
| [gt](#api-223) | `bool gt(T left,T right)` |
| [ge](#api-224) | `bool ge(T left,T right)` |
| [add](#api-225) | `R add(T left,U right)` |
| [sub](#api-226) | `R sub(T left,U right)` |
| [mul](#api-227) | `R mul(T left,U right)` |
| [div](#api-228) | `R div(T left,U right)` |
| [mod](#api-229) | `R mod(T left,U right)` |
| [index](#api-230) | `T index(T const& data,int64 index)` |
| [Add::leftAdd](#api-231) | `R leftAdd(T value)` |
| [RightAdd::rightAdd](#api-232) | `R rightAdd(T value)` |
| [Subtract::leftSub](#api-233) | `R leftSub(T value)` |
| [RightSubtract::rightSub](#api-234) | `R rightSub(T value)` |
| [Multiply::leftMul](#api-235) | `R leftMul(T value)` |
| [RightMultiply::rightMul](#api-236) | `R rightMul(T value)` |
| [Divide::leftDiv](#api-237) | `R leftDiv(T value)` |
| [RightDivide::rightDiv](#api-238) | `R rightDiv(T value)` |
| [Modulo::leftMod](#api-239) | `R leftMod(T value)` |
| [RightModulo::rightMod](#api-240) | `R rightMod(T value)` |
| [Equal::leftEqual](#api-241) | `R leftEqual(T value)` |
| [RightEqual::rightEqual](#api-242) | `R rightEqual(T value)` |
| [NotEqual::leftNotEqual](#api-243) | `R leftNotEqual(T value)` |
| [RightNotEqual::rightNotEqual](#api-244) | `R rightNotEqual(T value)` |
| [Less::leftLess](#api-245) | `R leftLess(T value)` |
| [RightLess::rightLess](#api-246) | `R rightLess(T value)` |
| [LessOrEqual::leftLessEqual](#api-247) | `R leftLessEqual(T value)` |
| [RightLessOrEqual::rightLessEqual](#api-248) | `R rightLessEqual(T value)` |
| [Greater::leftGreater](#api-249) | `R leftGreater(T value)` |
| [RightGreater::rightGreater](#api-250) | `R rightGreater(T value)` |
| [GreaterOrEqual::leftGreaterEqual](#api-251) | `R leftGreaterEqual(T value)` |
| [RightGreaterOrEqual::rightGreaterEqual](#api-252) | `R rightGreaterEqual(T value)` |
| [And::leftBitAnd](#api-253) | `R leftBitAnd(T value)` |
| [RightAnd::rightBitAnd](#api-254) | `R rightBitAnd(T value)` |
| [Or::leftBitOr](#api-255) | `R leftBitOr(T value)` |
| [RightOr::rightBitOr](#api-256) | `R rightBitOr(T value)` |
| [Xor::leftBitXor](#api-257) | `R leftBitXor(T value)` |
| [RightXor::rightBitXor](#api-258) | `R rightBitXor(T value)` |
| [ShiftLeft::leftShiftLeft](#api-259) | `R leftShiftLeft(T value)` |
| [RightShiftLeft::rightShiftLeft](#api-260) | `R rightShiftLeft(T value)` |
| [ShiftRight::leftShiftRight](#api-261) | `R leftShiftRight(T value)` |
| [RightShiftRight::rightShiftRight](#api-262) | `R rightShiftRight(T value)` |
| [Negate::negate](#api-263) | `R negate(T value)` |
| [BitNot::bitNot](#api-264) | `R bitNot(T value)` |
| [Positive::positive](#api-265) | `R positive(T value)` |
| [Hash::hash](#api-266) | `uint64 hash(T value)` |
| [Copy::copy](#api-267) | `T copy()` |
| [ExplicitConvert::explicitConvert](#api-268) | `T explicitConvert()` |
| [ImplicitConvert::implicitConvert](#api-269) | `T implicitConvert()` |
| [Index::index](#api-270) | `R index(T key)` |
| [Iterator::iterator](#api-271) | `Option<R> iterator(int64 index)` |
| [Result::mapErr](#api-272) | `Result<T,F> mapErr<F>(F fn(E) mapper)` |
| [Option::okOr](#api-273) | `Result<T,E> okOr<E>(E error)` |
| [Option::okOrElse](#api-274) | `Result<T,E> okOrElse<E>(E fn() factory)` |
| [BarePtr::castTo](#api-275) | `unsafe BarePtr<U> castTo<U>()` |

<a id="api-0"></a>

### rawSize（1）

**声明**

```chali
int64 rawSize<T>();
```

**行为**

查询 T 在当前 ChaLi 值存储中的字节数；不访问对象，也不返回动态分配占用。 uint8 与 char 使用一个字节，其余普通值槽为八字节；聚合按语言布局计算，不等于宿主 C ABI 的 sizeof。 无需 unsafe；不支持 void、直接数组或宿主别名，数组可放入具名结构体后查询。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。查询 T 在当前 ChaLi 值存储中的字节数；不访问对象，也不返回动态分配占用。 uint8 与 char 使用一个字节，其余普通值槽为八字节；聚合按语言布局计算，不等于宿主 C ABI 的 sizeof。 无需 unsafe；不支持 void、直接数组或宿主别名，数组可放入具名结构体后查询。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
rawSize<T>();
```

**源码**：[source/std/core.chl:8](../../source/std/core.chl#L8)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-1"></a>

### String::length（2）

**声明**

```chali
int64 length();
```

**行为**

返回 UTF-8 文本的字节长度。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。不包括终止零字节的长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.length();
```

**源码**：[source/std/core.chl:480](../../source/std/core.chl#L480)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-2"></a>

### String::size（3）

**声明**

```chali
int64 size();
```

**行为**

返回存储的总长度。

返回 UTF-8 字节数，与 length 一致。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回存储的总长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.size();
```

**源码**：[source/std/core.chl:482](../../source/std/core.chl#L482)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-3"></a>

### String::byteLength（4）

**声明**

```chali
int64 byteLength();
```

**行为**

返回字节数，不包含终止零字节。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回字节数，不包含终止零字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.byteLength();
```

**源码**：[source/std/core.chl:485](../../source/std/core.chl#L485)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-4"></a>

### String::capacity（5）

**声明**

```chali
int64 capacity();
```

**行为**

当前可复用的字节容量，不包含终止零字节。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。当前可复用的字节容量，不包含终止零字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.capacity();
```

**源码**：[source/std/core.chl:488](../../source/std/core.chl#L488)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-5"></a>

### String::maxCapacity（6）

**声明**

```chali
int64 maxCapacity();
```

**行为**

分配器允许的最大缓冲区容量，其他存活内存仍可能使申请失败。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。分配器允许的最大缓冲区容量，其他存活内存仍可能使申请失败。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.maxCapacity();
```

**源码**：[source/std/core.chl:491](../../source/std/core.chl#L491)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-6"></a>

### String::reserve（7）

**声明**

```chali
void reserve(int64 bytes);
```

**行为**

预留至少 bytes 字节；不足时几何增长，已有内容保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `int64 bytes` | 输入或输出字节存储 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.reserve(bytes);
```

**源码**：[source/std/core.chl:494](../../source/std/core.chl#L494)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-7"></a>

### String::append（8）

**声明**

```chali
void append(String text);
```

**行为**

原地追加文本字节，保留容量并允许向自身追加。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.append(text);
```

**源码**：[source/std/core.chl:497](../../source/std/core.chl#L497)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-8"></a>

### String::clear（9）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空有效字节，保留容量供后续追加复用。

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

**源码**：[source/std/core.chl:500](../../source/std/core.chl#L500)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-9"></a>

### String::truncate（10）

**声明**

```chali
void truncate(int64 bytes);
```

**行为**

按字节截短，超出现有长度时保持原值；保留容量，不进行 UTF-8 重编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `int64 bytes` | 输入或输出字节存储 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.truncate(bytes);
```

**源码**：[source/std/core.chl:503](../../source/std/core.chl#L503)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-10"></a>

### String::isEmpty（11）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

是否为空字符串。

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

**源码**：[source/std/core.chl:506](../../source/std/core.chl#L506)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-11"></a>

### String::isBlank（12）

**声明**

```chali
bool isBlank();
```

**行为**

是否仅含 ASCII 空白；空字符串也返回 true。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。是否仅含 ASCII 空白；空字符串也返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isBlank();
```

**源码**：[source/std/core.chl:509](../../source/std/core.chl#L509)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-12"></a>

### String::isDigit（13）

**声明**

```chali
bool isDigit();
```

**行为**

是否为非空 ASCII 十进制数字串。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。是否为非空 ASCII 十进制数字串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isDigit();
```

**源码**：[source/std/core.chl:512](../../source/std/core.chl#L512)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-13"></a>

### String::isAlpha（14）

**声明**

```chali
bool isAlpha();
```

**行为**

是否为非空 ASCII 字母串。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。是否为非空 ASCII 字母串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isAlpha();
```

**源码**：[source/std/core.chl:515](../../source/std/core.chl#L515)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-14"></a>

### String::isAlnum（15）

**声明**

```chali
bool isAlnum();
```

**行为**

是否为非空 ASCII 字母数字串。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。是否为非空 ASCII 字母数字串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isAlnum();
```

**源码**：[source/std/core.chl:518](../../source/std/core.chl#L518)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-15"></a>

### String::isAscii（16）

**声明**

```chali
bool isAscii();
```

**行为**

所有字节是否均小于 128；空串返回 true。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。所有字节是否均小于 128；空串返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isAscii();
```

**源码**：[source/std/core.chl:521](../../source/std/core.chl#L521)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-16"></a>

### String::charLength（17）

**声明**

```chali
int64 charLength();
```

**行为**

严格解码 UTF-8 后的 Unicode 码点数，非字素簇数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。严格解码 UTF-8 后的 Unicode 码点数，非字素簇数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.charLength();
```

**源码**：[source/std/core.chl:524](../../source/std/core.chl#L524)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-17"></a>

### String::indexOf（18）

**声明**

```chali
int64 indexOf(String text, int64 start = 0);
```

**行为**

返回首次出现的下标；未找到返回 -1。

从 start 字节起搜索，未找到返回 -1；负起点按零处理。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |
| `start` | `int64 start` | 起始值 | `0` |

**返回值**

`int64`。返回首次出现的下标；未找到返回 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.indexOf(text, start);
```

**源码**：[source/std/core.chl:527](../../source/std/core.chl#L527)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-18"></a>

### String::lastIndexOf（19）

**声明**

```chali
int64 lastIndexOf(String text, int64 start = 0 - 1);
```

**行为**

从 start 字节向前搜索，负起点表示从末尾开始。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |
| `start` | `int64 start` | 起始值 | `0 - 1` |

**返回值**

`int64`。从 start 字节向前搜索，负起点表示从末尾开始。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.lastIndexOf(text, start);
```

**源码**：[source/std/core.chl:530](../../source/std/core.chl#L530)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-19"></a>

### String::contains（20）

**声明**

```chali
bool contains(String text);
```

**行为**

判断是否包含目标值或匹配文本。

是否包含指定字节序列；空搜索串总是匹配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(text);
```

**源码**：[source/std/core.chl:533](../../source/std/core.chl#L533)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-20"></a>

### String::startsWith（21）

**声明**

```chali
bool startsWith(String text);
```

**行为**

判断文本是否以 prefix 开始。

是否以指定文本开头；空前缀总是匹配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`bool`。判断文本是否以 prefix 开始。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.startsWith(text);
```

**源码**：[source/std/core.chl:536](../../source/std/core.chl#L536)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-21"></a>

### String::endsWith（22）

**声明**

```chali
bool endsWith(String text);
```

**行为**

判断文本是否以 suffix 结束。

是否以指定文本结尾；空后缀总是匹配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`bool`。判断文本是否以 suffix 结束。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.endsWith(text);
```

**源码**：[source/std/core.chl:539](../../source/std/core.chl#L539)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-22"></a>

### String::count（23）

**声明**

```chali
int64 count(String text);
```

**行为**

读取结果首行首列的整数表示。

统计不重叠匹配数；空搜索串返回字节数加一。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`int64`。读取结果首行首列的整数表示。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.count(text);
```

**源码**：[source/std/core.chl:542](../../source/std/core.chl#L542)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-23"></a>

### String::compare（24）

**声明**

```chali
int64 compare(String text);
```

**行为**

按字典序比较，返回 -1、0 或 1。

按字节字典序比较，返回 -1、0 或 1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`int64`。按字典序比较，返回 -1、0 或 1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.compare(text);
```

**源码**：[source/std/core.chl:545](../../source/std/core.chl#L545)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-24"></a>

### String::byteAt（25）

**声明**

```chali
Option<uint8> byteAt(int64 index);
```

**行为**

读取字节，越界返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<uint8>`。读取字节，越界返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.byteAt(index);
```

**源码**：[source/std/core.chl:548](../../source/std/core.chl#L548)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-25"></a>

### String::substring（26）

**声明**

```chali
String substring(int64 start, int64 count = 0 - 1);
```

**行为**

按字节起点及长度截取，越界截断，负起点按零，负长度表示至末尾。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | `0 - 1` |

**返回值**

`String`。按字节起点及长度截取，越界截断，负起点按零，负长度表示至末尾。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.substring(start, count);
```

**源码**：[source/std/core.chl:551](../../source/std/core.chl#L551)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-26"></a>

### String::slice（27）

**声明**

```chali
String slice(int64 start, int64 end);
```

**行为**

按半开字节范围截取；结束不大于起点时返回空串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `end` | `int64 end` | 积分终点 | 无 |

**返回值**

`String`。按半开字节范围截取；结束不大于起点时返回空串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.slice(start, end);
```

**源码**：[source/std/core.chl:554](../../source/std/core.chl#L554)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-27"></a>

### String::left（28）

**声明**

```chali
String left(int64 count);
```

**行为**

取前 count 字节，负数按零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`String`。取前 count 字节，负数按零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.left(count);
```

**源码**：[source/std/core.chl:557](../../source/std/core.chl#L557)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-28"></a>

### String::right（29）

**声明**

```chali
String right(int64 count);
```

**行为**

取后 count 字节，负数按零，超出长度取全部。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`String`。取后 count 字节，负数按零，超出长度取全部。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.right(count);
```

**源码**：[source/std/core.chl:560](../../source/std/core.chl#L560)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-29"></a>

### String::trim（30）

**声明**

```chali
String trim();
```

**行为**

去掉两端 ASCII 空白。

去除两端 ASCII 空白，保留内部文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。去掉两端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.trim();
```

**源码**：[source/std/core.chl:563](../../source/std/core.chl#L563)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-30"></a>

### String::trimStart（31）

**声明**

```chali
String trimStart();
```

**行为**

去除起始 ASCII 空白。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。去除起始 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.trimStart();
```

**源码**：[source/std/core.chl:566](../../source/std/core.chl#L566)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-31"></a>

### String::trimEnd（32）

**声明**

```chali
String trimEnd();
```

**行为**

去除末尾 ASCII 空白。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。去除末尾 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.trimEnd();
```

**源码**：[source/std/core.chl:569](../../source/std/core.chl#L569)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-32"></a>

### String::toLower（33）

**声明**

```chali
String toLower();
```

**行为**

将 ASCII 大写字母转换成小写。

将 ASCII 大写字母转小写，其他字节保持原样。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。将 ASCII 大写字母转换成小写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toLower();
```

**源码**：[source/std/core.chl:572](../../source/std/core.chl#L572)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-33"></a>

### String::toUpper（34）

**声明**

```chali
String toUpper();
```

**行为**

将 ASCII 小写字母转换成大写。

将 ASCII 小写字母转大写，其他字节保持原样。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。将 ASCII 小写字母转换成大写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toUpper();
```

**源码**：[source/std/core.chl:575](../../source/std/core.chl#L575)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-34"></a>

### String::replaceFirst（35）

**声明**

```chali
String replaceFirst(String before, String after);
```

**行为**

替换首次正则匹配。

替换第一个精确匹配；空搜索串保持原值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `before` | `String before` | 待替换的精确匹配文本 | 无 |
| `after` | `String after` | 替换文本 | 无 |

**返回值**

`String`。替换首次正则匹配。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.replaceFirst(before, after);
```

**源码**：[source/std/core.chl:578](../../source/std/core.chl#L578)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-35"></a>

### String::replaceAll（36）

**声明**

```chali
String replaceAll(String before, String after);
```

**行为**

替换全部正则匹配。

替换全部不重叠精确匹配；空搜索串保持原值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `before` | `String before` | 待替换的精确匹配文本 | 无 |
| `after` | `String after` | 替换文本 | 无 |

**返回值**

`String`。替换全部正则匹配。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.replaceAll(before, after);
```

**源码**：[source/std/core.chl:581](../../source/std/core.chl#L581)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-36"></a>

### String::replace（37）

**声明**

```chali
String replace(String before, String after);
```

**行为**

将所有 from 子串替换为 to；空 from 保留副本。

替换全部不重叠匹配，与 replaceAll 一致。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `before` | `String before` | 待替换的精确匹配文本 | 无 |
| `after` | `String after` | 替换文本 | 无 |

**返回值**

`String`。将所有 from 子串替换为 to；空 from 保留副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.replace(before, after);
```

**源码**：[source/std/core.chl:584](../../source/std/core.chl#L584)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-37"></a>

### String::repeat（38）

**声明**

```chali
String repeat(int64 count);
```

**行为**

重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

重复 count 次，零次返回空串，负数报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`String`。重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.repeat(count);
```

**源码**：[source/std/core.chl:587](../../source/std/core.chl#L587)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-38"></a>

### String::reverse（39）

**声明**

```chali
String reverse();
```

**行为**

按 Unicode 码点反转，保持每个 UTF-8 编码完整。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。按 Unicode 码点反转，保持每个 UTF-8 编码完整。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.reverse();
```

**源码**：[source/std/core.chl:590](../../source/std/core.chl#L590)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-39"></a>

### String::padStart（40）

**声明**

```chali
String padStart(int64 length, String fill = " ");
```

**行为**

在左侧以 fill 补到 width 字节。

按字节目标长度向左循环填充；空填充串保持原值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `length` | `int64 length` | 目标有效元素数，必须非负。 | 无 |
| `fill` | `String fill` | 填充字节 | `" "` |

**返回值**

`String`。在左侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.padStart(length, fill);
```

**源码**：[source/std/core.chl:593](../../source/std/core.chl#L593)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-40"></a>

### String::padEnd（41）

**声明**

```chali
String padEnd(int64 length, String fill = " ");
```

**行为**

在右侧以 fill 补到 width 字节。

按字节目标长度向右循环填充；空填充串保持原值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `length` | `int64 length` | 目标有效元素数，必须非负。 | 无 |
| `fill` | `String fill` | 填充字节 | `" "` |

**返回值**

`String`。在右侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.padEnd(length, fill);
```

**源码**：[source/std/core.chl:596](../../source/std/core.chl#L596)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-41"></a>

### String::removePrefix（42）

**声明**

```chali
String removePrefix(String prefix);
```

**行为**

仅去除匹配的起始前缀一次。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `prefix` | `String prefix` | 前缀 | 无 |

**返回值**

`String`。仅去除匹配的起始前缀一次。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.removePrefix(prefix);
```

**源码**：[source/std/core.chl:599](../../source/std/core.chl#L599)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-42"></a>

### String::removeSuffix（43）

**声明**

```chali
String removeSuffix(String suffix);
```

**行为**

仅去除匹配的末尾后缀一次。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `suffix` | `String suffix` | 后缀 | 无 |

**返回值**

`String`。仅去除匹配的末尾后缀一次。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.removeSuffix(suffix);
```

**源码**：[source/std/core.chl:602](../../source/std/core.chl#L602)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-43"></a>

### String::insert（44）

**声明**

```chali
String insert(int64 index, String text);
```

**行为**

在 index 之前插入 value。

在截断到有效范围的字节位置插入文本，返回新值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`String`。在 index 之前插入 value。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.insert(index, text);
```

**源码**：[source/std/core.chl:605](../../source/std/core.chl#L605)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-44"></a>

### String::remove（45）

**声明**

```chali
String remove(int64 start, int64 count = 0 - 1);
```

**行为**

删除指定位置或键的元素。

移除指定字节段；负长度表示删除余下全部。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | `0 - 1` |

**返回值**

`String`。删除指定位置或键的元素。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(start, count);
```

**源码**：[source/std/core.chl:608](../../source/std/core.chl#L608)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-45"></a>

### String::utf8Substring（46）

**声明**

```chali
String utf8Substring(int64 start, int64 count = 0 - 1);
```

**行为**

按 Unicode 码点截取，严格验证 UTF-8，不按字素簇处理。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | `0 - 1` |

**返回值**

`String`。按 Unicode 码点截取，严格验证 UTF-8，不按字素簇处理。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.utf8Substring(start, count);
```

**源码**：[source/std/core.chl:611](../../source/std/core.chl#L611)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-46"></a>

### String::charAt（47）

**声明**

```chali
Option<String> charAt(int64 index);
```

**行为**

读取 index 位置的字节，需处于有效 UTF-8 边界。

读取一个完整 Unicode 码点，越界返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<String>`。读取 index 位置的字节，需处于有效 UTF-8 边界。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.charAt(index);
```

**源码**：[source/std/core.chl:614](../../source/std/core.chl#L614)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-47"></a>

### String::split（48）

**声明**

```chali
List<String> split(String separator, bool keepEmpty = true);
```

**行为**

按 separator 分割，空分隔符把整串作为一项。

按精确分隔文本分割，默认保留空项；空分隔符按 UTF-8 码点分割。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `separator` | `String separator` | 分隔文本 | 无 |
| `keepEmpty` | `bool keepEmpty` | 是否保留分割产生的空项 | `true` |

**返回值**

`List<String>`。按 separator 分割，空分隔符把整串作为一项。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.split(separator, keepEmpty);
```

**源码**：[source/std/core.chl:617](../../source/std/core.chl#L617)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-48"></a>

### String::splitWhitespace（49）

**声明**

```chali
List<String> splitWhitespace();
```

**行为**

按连续 ASCII 空白分词并忽略空项。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<String>`。按连续 ASCII 空白分词并忽略空项。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.splitWhitespace();
```

**源码**：[source/std/core.chl:620](../../source/std/core.chl#L620)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-49"></a>

### String::lines（50）

**声明**

```chali
List<String> lines(bool keepEmpty = true);
```

**行为**

按 LF、CRLF 或 CR 分行，默认保留空行和末尾空项。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `keepEmpty` | `bool keepEmpty` | 是否保留分割产生的空项 | `true` |

**返回值**

`List<String>`。按 LF、CRLF 或 CR 分行，默认保留空行和末尾空项。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.lines(keepEmpty);
```

**源码**：[source/std/core.chl:623](../../source/std/core.chl#L623)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-50"></a>

### String::unicodeChars（51）

**声明**

```chali
List<String> unicodeChars();
```

**行为**

严格按 UTF-8 Unicode 码点拆分，空串产生空列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<String>`。严格按 UTF-8 Unicode 码点拆分，空串产生空列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.unicodeChars();
```

**源码**：[source/std/core.chl:626](../../source/std/core.chl#L626)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-51"></a>

### String::bytes（52）

**声明**

```chali
List<uint8> bytes();
```

**行为**

导出独立字节列表，修改列表不影响原字符串。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<uint8>`。导出独立字节列表，修改列表不影响原字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bytes();
```

**源码**：[source/std/core.chl:629](../../source/std/core.chl#L629)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-52"></a>

### String::join（53）

**声明**

```chali
String join(List<String> items);
```

**行为**

用 separator 连接 parts。

以当前字符串为分隔符连接值列表，空列表返回空串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `items` | `List<String> items` | 参与处理的列表元素 | 无 |

**返回值**

`String`。用 separator 连接 parts。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.join(items);
```

**源码**：[source/std/core.chl:632](../../source/std/core.chl#L632)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-53"></a>

### String::concat（54）

**声明**

```chali
String concat(String text);
```

**行为**

返回两段文本的连接，不修改当前值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`String`。返回两段文本的连接，不修改当前值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.concat(text);
```

**源码**：[source/std/core.chl:635](../../source/std/core.chl#L635)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-54"></a>

### String::prepend（55）

**声明**

```chali
String prepend(String text);
```

**行为**

返回在当前值前添加文本的新字符串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`String`。返回在当前值前添加文本的新字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.prepend(text);
```

**源码**：[source/std/core.chl:638](../../source/std/core.chl#L638)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-55"></a>

### Debug::debug（56）

**声明**

```chali
String debug();
```

**行为**

返回供开发时检查的结构化文本；不作为持久化格式。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。返回供开发时检查的结构化文本；不作为持久化格式。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.debug();
```

**源码**：[source/std/core.chl:644](../../source/std/core.chl#L644)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-56"></a>

### Error::toString（57）

**声明**

```chali
String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

返回可直接显示给用户的错误说明。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。由实现者将当前值转换为文本，Formattable 重载接收 style。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toString();
```

**源码**：[source/std/core.chl:650](../../source/std/core.chl#L650)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-57"></a>

### MessageError::toString（58）

**声明**

```chali
String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

返回原始诊断文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。由实现者将当前值转换为文本，Formattable 重载接收 style。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toString();
```

**源码**：[source/std/core.chl:658](../../source/std/core.chl#L658)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-58"></a>

### MessageError::from（59）

**声明**

```chali
static MessageError from(String text);
```

**行为**

用首元素引用和长度创建非拥有 Span 视图。

从诊断文本构造拥有独立消息的错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`MessageError`。用首元素引用和长度创建非拥有 Span 视图。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
MessageError::from(text);
```

**源码**：[source/std/core.chl:660](../../source/std/core.chl#L660)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-59"></a>

### File::print（60）

**声明**

```chali
int64 print(String text);
```

**行为**

写入文本，格式参数沿用 print 的规则。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 待写入文本，支持由格式化调用生成。 | 无 |

**返回值**

`int64`。成功写入的字节数，失败沿用文件写入诊断。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.print(text);
```

**源码**：[source/std/core.chl:676](../../source/std/core.chl#L676)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-60"></a>

### File::println（61）

**声明**

```chali
int64 println(String text = "");
```

**行为**

写入文本并追加一个 LF 换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 待写入文本，省略时只写换行。 | `""` |

**返回值**

`int64`。包括换行的写入字节数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.println(text);
```

**源码**：[source/std/core.chl:682](../../source/std/core.chl#L682)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-61"></a>

### readLine（62）

**声明**

```chali
String readLine();
```

**行为**

读取一行标准输入，保留空格，去掉 LF/CRLF；EOF 返回空串，空行也返回空串。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。读取的行文本；String 不允许内嵌零字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
readLine();
```

**源码**：[source/std/core.chl:749](../../source/std/core.chl#L749)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-62"></a>

### readLines（63）

**声明**

```chali
String readLines(int32 N);
```

**行为**

最多读取 N 行，以 LF 连接，结果不额外追加换行；EOF 提前结束。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `N` | `int32 N` | 非负行数；零返回空串且不读取输入，负数触发 panic。 | 无 |

**返回值**

`String`。拼接后的文本，空行保留。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`N>=0,"readLines requires a nonnegative line count"`。

**调用形式**

```chali
readLines(N);
```

**源码**：[source/std/core.chl:753](../../source/std/core.chl#L753)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-63"></a>

### readLines（64）

**声明**

```chali
String readLines(int64 N);
```

**行为**

接受默认整数实参的多行输入重载，检查后转为 int32 行数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `N` | `int64 N` | 0 至 2147483647 的行数；超界触发 panic。 | 无 |

**返回值**

`String`。最多 N 行，以 LF 连接，EOF 提前结束。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`N>=0 && N<=2147483647,"readLines requires a nonnegative int32 line count"`。

**调用形式**

```chali
readLines(N);
```

**源码**：[source/std/core.chl:757](../../source/std/core.chl#L757)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-64"></a>

### toInt32（65）

**声明**

```chali
Option<int32> toInt32(float64 value);
```

**行为**

向零截断为 int32；非有限数或截断后越界返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<int32>`。向零截断为 int32；非有限数或截断后越界返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toInt32(value);
```

**源码**：[source/std/core.chl:761](../../source/std/core.chl#L761)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-65"></a>

### toInt32（66）

**声明**

```chali
Option<int32> toInt32(float32 value);
```

**行为**

向零截断为 int32；非有限数或截断后越界返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<int32>`。向零截断为 int32；非有限数或截断后越界返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toInt32(value);
```

**源码**：[source/std/core.chl:767](../../source/std/core.chl#L767)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-66"></a>

### toInt64（67）

**声明**

```chali
int64 toInt64(float64 value);
```

**行为**

向零截断为 int64，非有限数或截断后越界时报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。向零截断为 int64，非有限数或截断后越界时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toInt64(value);
```

**源码**：[source/std/core.chl:769](../../source/std/core.chl#L769)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-67"></a>

### toInt64（68）

**声明**

```chali
int64 toInt64(float32 value);
```

**行为**

向零截断为 int64，非有限数或截断后越界时报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。向零截断为 int64，非有限数或截断后越界时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toInt64(value);
```

**源码**：[source/std/core.chl:771](../../source/std/core.chl#L771)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-68"></a>

### float32ToBytes（69）

**声明**

```chali
List<uint8> float32ToBytes(float32 value, bool bigEndian);
```

**行为**

按 IEEE 754 binary32 输出 4 个字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`List<uint8>`。按 IEEE 754 binary32 输出 4 个字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bigEndian);
```

**源码**：[source/std/core.chl:774](../../source/std/core.chl#L774)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-69"></a>

### float32ToBytes（70）

**声明**

```chali
List<uint8> float32ToBytes(float32 value);
```

**行为**

默认小端编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |

**返回值**

`List<uint8>`。默认小端编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value);
```

**源码**：[source/std/core.chl:776](../../source/std/core.chl#L776)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-70"></a>

### bytesToFloat32（71）

**声明**

```chali
Option<float32> bytesToFloat32(List<uint8> bytes, bool bigEndian);
```

**行为**

读取列表前 4 字节，不足则返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float32>`。读取列表前 4 字节，不足则返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes, bigEndian);
```

**源码**：[source/std/core.chl:778](../../source/std/core.chl#L778)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-71"></a>

### bytesToFloat32（72）

**声明**

```chali
Option<float32> bytesToFloat32(List<uint8> bytes);
```

**行为**

默认小端列表解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float32>`。默认小端列表解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes);
```

**源码**：[source/std/core.chl:783](../../source/std/core.chl#L783)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-72"></a>

### bytesToFloat32（73）

**声明**

```chali
Option<float32> bytesToFloat32(uint8 const& bytes, int64 count, bool bigEndian);
```

**行为**

从固定数组或指针读取前 4 字节；调用者保证 count 为有效长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 const& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float32>`。从固定数组或指针读取前 4 字节；调用者保证 count 为有效长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes, count, bigEndian);
```

**源码**：[source/std/core.chl:785](../../source/std/core.chl#L785)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-73"></a>

### bytesToFloat32（74）

**声明**

```chali
Option<float32> bytesToFloat32(uint8 const& bytes, int64 count);
```

**行为**

默认小端固定数组解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 const& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`Option<float32>`。默认小端固定数组解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes, count);
```

**源码**：[source/std/core.chl:790](../../source/std/core.chl#L790)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-74"></a>

### bytesToFloat32（75）

**声明**

```chali
Option<float32> bytesToFloat32(uint8 bytes[], bool bigEndian);
```

**行为**

从运行时数组读取，自动获取长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 bytes[]` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float32>`。从运行时数组读取，自动获取长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes, bigEndian);
```

**源码**：[source/std/core.chl:792](../../source/std/core.chl#L792)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-75"></a>

### bytesToFloat32（76）

**声明**

```chali
Option<float32> bytesToFloat32(uint8 bytes[]);
```

**行为**

默认小端运行时数组解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 bytes[]` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float32>`。默认小端运行时数组解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes);
```

**源码**：[source/std/core.chl:794](../../source/std/core.chl#L794)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-76"></a>

### float32ToBytes（77）

**声明**

```chali
bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count, bool bigEndian);
```

**行为**

写入数组前 4 项；容量不足返回 false，数组保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8 mut& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`bool`。写入数组前 4 项；容量不足返回 false，数组保持不变。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bytes, count, bigEndian);
```

**源码**：[source/std/core.chl:796](../../source/std/core.chl#L796)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-77"></a>

### float32ToBytes（78）

**声明**

```chali
bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count);
```

**行为**

默认小端固定数组编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8 mut& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`bool`。默认小端固定数组编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bytes, count);
```

**源码**：[source/std/core.chl:801](../../source/std/core.chl#L801)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-78"></a>

### float32ToBytes（79）

**声明**

```chali
bool float32ToBytes(float32 value, uint8[] mut& bytes, bool bigEndian);
```

**行为**

编码到运行时数组的可变引用，自动获取容量；调用时传入 &array。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8[] mut& bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`bool`。编码到运行时数组的可变引用，自动获取容量；调用时传入 &array。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bytes, bigEndian);
```

**源码**：[source/std/core.chl:803](../../source/std/core.chl#L803)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-79"></a>

### float32ToBytes（80）

**声明**

```chali
bool float32ToBytes(float32 value, uint8[] mut& bytes);
```

**行为**

默认小端运行时数组编码；调用时传入 &array。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8[] mut& bytes` | 输入或输出字节存储 | 无 |

**返回值**

`bool`。默认小端运行时数组编码；调用时传入 &array。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bytes);
```

**源码**：[source/std/core.chl:808](../../source/std/core.chl#L808)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-80"></a>

### float64ToBytes（81）

**声明**

```chali
List<uint8> float64ToBytes(float64 value, bool bigEndian);
```

**行为**

按 IEEE 754 binary64 输出 8 个字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`List<uint8>`。按 IEEE 754 binary64 输出 8 个字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bigEndian);
```

**源码**：[source/std/core.chl:811](../../source/std/core.chl#L811)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-81"></a>

### float64ToBytes（82）

**声明**

```chali
List<uint8> float64ToBytes(float64 value);
```

**行为**

默认小端编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |

**返回值**

`List<uint8>`。默认小端编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value);
```

**源码**：[source/std/core.chl:813](../../source/std/core.chl#L813)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-82"></a>

### bytesToFloat64（83）

**声明**

```chali
Option<float64> bytesToFloat64(List<uint8> bytes, bool bigEndian);
```

**行为**

读取列表前 8 字节，不足则返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float64>`。读取列表前 8 字节，不足则返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes, bigEndian);
```

**源码**：[source/std/core.chl:815](../../source/std/core.chl#L815)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-83"></a>

### bytesToFloat64（84）

**声明**

```chali
Option<float64> bytesToFloat64(List<uint8> bytes);
```

**行为**

默认小端列表解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float64>`。默认小端列表解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes);
```

**源码**：[source/std/core.chl:820](../../source/std/core.chl#L820)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-84"></a>

### bytesToFloat64（85）

**声明**

```chali
Option<float64> bytesToFloat64(uint8 const& bytes, int64 count, bool bigEndian);
```

**行为**

从固定数组或指针读取前 8 字节；调用者保证 count 为有效长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 const& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float64>`。从固定数组或指针读取前 8 字节；调用者保证 count 为有效长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes, count, bigEndian);
```

**源码**：[source/std/core.chl:822](../../source/std/core.chl#L822)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-85"></a>

### bytesToFloat64（86）

**声明**

```chali
Option<float64> bytesToFloat64(uint8 const& bytes, int64 count);
```

**行为**

默认小端固定数组解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 const& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`Option<float64>`。默认小端固定数组解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes, count);
```

**源码**：[source/std/core.chl:827](../../source/std/core.chl#L827)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-86"></a>

### bytesToFloat64（87）

**声明**

```chali
Option<float64> bytesToFloat64(uint8 bytes[], bool bigEndian);
```

**行为**

从运行时数组读取，自动获取长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 bytes[]` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float64>`。从运行时数组读取，自动获取长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes, bigEndian);
```

**源码**：[source/std/core.chl:829](../../source/std/core.chl#L829)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-87"></a>

### bytesToFloat64（88）

**声明**

```chali
Option<float64> bytesToFloat64(uint8 bytes[]);
```

**行为**

默认小端运行时数组解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `uint8 bytes[]` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float64>`。默认小端运行时数组解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes);
```

**源码**：[source/std/core.chl:831](../../source/std/core.chl#L831)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-88"></a>

### float64ToBytes（89）

**声明**

```chali
bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count, bool bigEndian);
```

**行为**

写入数组前 8 项；容量不足返回 false，数组保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8 mut& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`bool`。写入数组前 8 项；容量不足返回 false，数组保持不变。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bytes, count, bigEndian);
```

**源码**：[source/std/core.chl:833](../../source/std/core.chl#L833)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-89"></a>

### float64ToBytes（90）

**声明**

```chali
bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count);
```

**行为**

默认小端固定数组编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8 mut& bytes` | 输入或输出字节存储 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`bool`。默认小端固定数组编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bytes, count);
```

**源码**：[source/std/core.chl:838](../../source/std/core.chl#L838)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-90"></a>

### float64ToBytes（91）

**声明**

```chali
bool float64ToBytes(float64 value, uint8[] mut& bytes, bool bigEndian);
```

**行为**

编码到运行时数组的可变引用，自动获取容量；调用时传入 &array。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8[] mut& bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`bool`。编码到运行时数组的可变引用，自动获取容量；调用时传入 &array。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bytes, bigEndian);
```

**源码**：[source/std/core.chl:840](../../source/std/core.chl#L840)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-91"></a>

### float64ToBytes（92）

**声明**

```chali
bool float64ToBytes(float64 value, uint8[] mut& bytes);
```

**行为**

默认小端运行时数组编码；调用时传入 &array。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bytes` | `uint8[] mut& bytes` | 输入或输出字节存储 | 无 |

**返回值**

`bool`。默认小端运行时数组编码；调用时传入 &array。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bytes);
```

**源码**：[source/std/core.chl:845](../../source/std/core.chl#L845)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-92"></a>

### assert（93）

**声明**

```chali
void assert(bool condition);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

中止统一使用 std::core::panic，编译器与运行时通过 `__std_panic` 执行停机。 断言使用下面的标准库函数，它们在条件失败时调用 `std::core::panic`。 带消息的重载让标准库自己的前置检查 也能给出人话（例如 range 的步长）。 带 `int64 line` 的重载给文件末尾的系统宏 `$assert(...)` 用：宏把调用点的 `$line` 传进来，失败时就地报出**源码行号**，不必每处自己写文案。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`x) 的 x 为假"`。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition);
```

**源码**：[source/std/core.chl:858](../../source/std/core.chl#L858)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-93"></a>

### assert（94）

**声明**

```chali
void assert(bool condition, String message);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition, message);
```

**源码**：[source/std/core.chl:862](../../source/std/core.chl#L862)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-94"></a>

### assert（95）

**声明**

```chali
void assert(bool condition, int64 line);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |
| `line` | `int64 line` | 用于诊断的源码行号 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition, line);
```

**源码**：[source/std/core.chl:866](../../source/std/core.chl#L866)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-95"></a>

### assert（96）

**声明**

```chali
void assert(bool condition, String message, int64 line);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |
| `line` | `int64 line` | 用于诊断的源码行号 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition, message, line);
```

**源码**：[source/std/core.chl:870](../../source/std/core.chl#L870)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-96"></a>

### Try::branch（97）

**声明**

```chali
TryBranch<T, R> branch();
```

**行为**

只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, R>`。只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.branch();
```

**源码**：[source/std/core.chl:881](../../source/std/core.chl#L881)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-97"></a>

### TryMove::intoBranch（98）

**声明**

```chali
TryBranch<T, R> intoBranch();
```

**行为**

在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, R>`。在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.intoBranch();
```

**源码**：[source/std/core.chl:883](../../source/std/core.chl#L883)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-98"></a>

### FromResidual::fromResidual（99）

**声明**

```chali
static F fromResidual(R residual);
```

**行为**

静态地将匹配的传播载荷构造为当前函数的返回类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `residual` | `R residual` | 向当前函数返回类型传递的失败载荷。 | 无 |

**返回值**

`F`。静态地将匹配的传播载荷构造为当前函数的返回类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
FromResidual<F, R>::fromResidual(residual);
```

**源码**：[source/std/core.chl:885](../../source/std/core.chl#L885)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-99"></a>

### Option::branch（100）

**声明**

```chali
TryBranch<T, NoneResidual> branch();
```

**行为**

只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, NoneResidual>`。只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.branch();
```

**源码**：[source/std/core.chl:890](../../source/std/core.chl#L890)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-100"></a>

### Option::intoBranch（101）

**声明**

```chali
TryBranch<T, NoneResidual> intoBranch();
```

**行为**

在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, NoneResidual>`。在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.intoBranch();
```

**源码**：[source/std/core.chl:896](../../source/std/core.chl#L896)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-101"></a>

### Option::fromResidual（102）

**声明**

```chali
static Option<T> fromResidual(NoneResidual residual);
```

**行为**

静态地将匹配的传播载荷构造为当前函数的返回类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `residual` | `NoneResidual residual` | 向当前函数返回类型传递的失败载荷。 | 无 |

**返回值**

`Option<T>`。静态地将匹配的传播载荷构造为当前函数的返回类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Option<T>::fromResidual(residual);
```

**源码**：[source/std/core.chl:900](../../source/std/core.chl#L900)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-102"></a>

### Option::isValue（103）

**声明**

```chali
bool isValue();
```

**行为**

判断 Option 是否含 Value。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Option 是否含 Value。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValue();
```

**源码**：[source/std/core.chl:901](../../source/std/core.chl#L901)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-103"></a>

### Option::isNone（104）

**声明**

```chali
bool isNone();
```

**行为**

判断 Option 是否为 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Option 是否为 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isNone();
```

**源码**：[source/std/core.chl:910](../../source/std/core.chl#L910)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-104"></a>

### Option::take（105）

**声明**

```chali
T take();
```

**行为**

转移整个结果并取出成功载荷；源结果随后不可使用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。转移整个结果并取出成功载荷；源结果随后不可使用。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.take();
```

**源码**：[source/std/core.chl:913](../../source/std/core.chl#L913)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-105"></a>

### Option::unwrap（106）

**声明**

```chali
T unwrap();
```

**行为**

提取成功载荷；None/Err 时 panic。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。提取成功载荷；None/Err 时 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.unwrap();
```

**源码**：[source/std/core.chl:923](../../source/std/core.chl#L923)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-106"></a>

### Option::unwrapOr（107）

**声明**

```chali
T unwrapOr(T fallback);
```

**行为**

提取成功载荷；None/Err 时返回 fallback。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `fallback` | `T fallback` | 失败/空值分支的替代值 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时返回 fallback。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.unwrapOr(fallback);
```

**源码**：[source/std/core.chl:932](../../source/std/core.chl#L932)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-107"></a>

### Option::expect（108）

**声明**

```chali
T expect(String message);
```

**行为**

提取成功载荷；None/Err 时以 message 触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时以 message 触发 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.expect(message);
```

**源码**：[source/std/core.chl:941](../../source/std/core.chl#L941)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-108"></a>

### Option::transform（109）

**声明**

```chali
Option<T> transform(T fn(T) mapper);
```

**行为**

成功时调用 mapper 转换载荷，失败分支保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `mapper` | `T fn(T) mapper` | 转换回调，签名决定输入和输出类型 | 无 |

**返回值**

`Option<T>`。成功时调用 mapper 转换载荷，失败分支保持不变。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.transform(mapper);
```

**源码**：[source/std/core.chl:950](../../source/std/core.chl#L950)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-109"></a>

### Option::andThen（110）

**声明**

```chali
Option<T> andThen(Option<T> fn(T) mapper);
```

**行为**

成功时调用返回同族 Option/Result 的 mapper，将结果展平。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `mapper` | `Option<T> fn(T) mapper` | 转换回调，签名决定输入和输出类型 | 无 |

**返回值**

`Option<T>`。成功时调用返回同族 Option/Result 的 mapper，将结果展平。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.andThen(mapper);
```

**源码**：[source/std/core.chl:959](../../source/std/core.chl#L959)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-110"></a>

### Result::branch（111）

**声明**

```chali
TryBranch<T, ErrorResidual<E>> branch();
```

**行为**

只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, ErrorResidual<E>>`。只读拆分传播值，Continue 产生成功载荷，Break 携带失败载荷。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.branch();
```

**源码**：[source/std/core.chl:980](../../source/std/core.chl#L980)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-111"></a>

### Result::intoBranch（112）

**声明**

```chali
TryBranch<T, ErrorResidual<E>> intoBranch();
```

**行为**

在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TryBranch<T, ErrorResidual<E>>`。在移动拥有值后拆分传播值，结果遵守成功与失败载荷的所有权规则。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.intoBranch();
```

**源码**：[source/std/core.chl:990](../../source/std/core.chl#L990)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-112"></a>

### Result::fromResidual（113）

**声明**

```chali
static Result<T,E> fromResidual(ErrorResidual<E> residual);
```

**行为**

静态地将匹配的传播载荷构造为当前函数的返回类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `residual` | `ErrorResidual<E> residual` | 向当前函数返回类型传递的失败载荷。 | 无 |

**返回值**

`Result<T,E>`。静态地将匹配的传播载荷构造为当前函数的返回类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Result<T, E>::fromResidual(residual);
```

**源码**：[source/std/core.chl:996](../../source/std/core.chl#L996)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-113"></a>

### Result::ok（114）

**声明**

```chali
Option<T> ok();
```

**行为**

成功时保留值，失败时明确丢弃错误信息并返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T>`。成功时保留值，失败时明确丢弃错误信息并返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.ok();
```

**源码**：[source/std/core.chl:997](../../source/std/core.chl#L997)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-114"></a>

### Result::isOk（115）

**声明**

```chali
bool isOk();
```

**行为**

判断 Result 是否为 Ok。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Result 是否为 Ok。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isOk();
```

**源码**：[source/std/core.chl:1003](../../source/std/core.chl#L1003)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-115"></a>

### Result::isErr（116）

**声明**

```chali
bool isErr();
```

**行为**

判断 Result 是否为 Err。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Result 是否为 Err。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isErr();
```

**源码**：[source/std/core.chl:1012](../../source/std/core.chl#L1012)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-116"></a>

### Result::take（117）

**声明**

```chali
T take();
```

**行为**

转移整个结果并取出成功载荷；源结果随后不可使用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。转移整个结果并取出成功载荷；源结果随后不可使用。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.take();
```

**源码**：[source/std/core.chl:1015](../../source/std/core.chl#L1015)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-117"></a>

### Result::unwrap（118）

**声明**

```chali
T unwrap();
```

**行为**

提取成功载荷；None/Err 时 panic。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。提取成功载荷；None/Err 时 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.unwrap();
```

**源码**：[source/std/core.chl:1025](../../source/std/core.chl#L1025)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-118"></a>

### Result::unwrapOr（119）

**声明**

```chali
T unwrapOr(T fallback);
```

**行为**

提取成功载荷；None/Err 时返回 fallback。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `fallback` | `T fallback` | 失败/空值分支的替代值 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时返回 fallback。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.unwrapOr(fallback);
```

**源码**：[source/std/core.chl:1034](../../source/std/core.chl#L1034)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-119"></a>

### Result::expect（120）

**声明**

```chali
T expect(String message);
```

**行为**

提取成功载荷；None/Err 时以 message 触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时以 message 触发 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.expect(message);
```

**源码**：[source/std/core.chl:1043](../../source/std/core.chl#L1043)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-120"></a>

### Result::takeError（121）

**声明**

```chali
E takeError();
```

**行为**

转移整个结果并取出错误载荷；源结果随后不可使用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`E`。转移整个结果并取出错误载荷；源结果随后不可使用。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.takeError();
```

**源码**：[source/std/core.chl:1053](../../source/std/core.chl#L1053)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-121"></a>

### Result::error（122）

**声明**

```chali
E error();
```

**行为**

提取 Err 载荷；Ok 时 panic。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`E`。提取 Err 载荷；Ok 时 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.error();
```

**源码**：[source/std/core.chl:1063](../../source/std/core.chl#L1063)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-122"></a>

### Result::transform（123）

**声明**

```chali
Result<T, E> transform(T fn(T) mapper);
```

**行为**

成功时调用 mapper 转换载荷，失败分支保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `mapper` | `T fn(T) mapper` | 转换回调，签名决定输入和输出类型 | 无 |

**返回值**

`Result<T, E>`。成功时调用 mapper 转换载荷，失败分支保持不变。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.transform(mapper);
```

**源码**：[source/std/core.chl:1072](../../source/std/core.chl#L1072)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-123"></a>

### Result::andThen（124）

**声明**

```chali
Result<T, E> andThen(Result<T, E> fn(T) mapper);
```

**行为**

成功时调用返回同族 Option/Result 的 mapper，将结果展平。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `mapper` | `Result<T, E> fn(T) mapper` | 转换回调，签名决定输入和输出类型 | 无 |

**返回值**

`Result<T, E>`。成功时调用返回同族 Option/Result 的 mapper，将结果展平。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.andThen(mapper);
```

**源码**：[source/std/core.chl:1081](../../source/std/core.chl#L1081)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-124"></a>

### NextValue::nextValue（125）

**声明**

```chali
Option<T> nextValue(T end, bool inclusive);
```

**行为**

使用接收者的游标状态、end 与 inclusive 产生下一项，并推进游标；返回 None 结束区间。

取得下一项，由实现判断边界并维护游标状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `end` | `T end` | 区间终点。 | 无 |
| `inclusive` | `bool inclusive` | 是否包含终点。 | 无 |

**返回值**

`Option<T>`。下一项，或表示遍历结束的 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.nextValue(end, inclusive);
```

**源码**：[source/std/core.chl:1102](../../source/std/core.chl#L1102)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-125"></a>

### Pair::create（126）

**声明**

```chali
static Pair<K, V> create(K first, V second);
```

**行为**

创建Pair，返回可供后续操作的值。

从两个值复制构造键值对。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `first` | `K first` | 键或第一项。 | 无 |
| `second` | `V second` | 值或第二项。 | 无 |

**返回值**

`Pair<K, V>`。拥有独立字段值的 Pair。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Pair<K, V>::create(first, second);
```

**源码**：[source/std/core.chl:1158](../../source/std/core.chl#L1158)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-126"></a>

### Pair::create（127）

**声明**

```chali
static Pair<K, V> create((K, V) value);
```

**行为**

创建Pair，返回可供后续操作的值。

从二元组复制构造键值对。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `(K, V) value` | 提供 first 与 second 的两个值。 | 无 |

**返回值**

`Pair<K, V>`。拥有独立字段值的 Pair。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Pair<K, V>::create(value);
```

**源码**：[source/std/core.chl:1168](../../source/std/core.chl#L1168)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-127"></a>

### Pair::toTuple（128）

**声明**

```chali
(K, V) toTuple();
```

**行为**

将键值对的两个字段复制为二元组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`(K, V)`。按 first、second 顺序保存独立值的二元组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toTuple();
```

**源码**：[source/std/core.chl:1177](../../source/std/core.chl#L1177)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-128"></a>

### isType（129）

**声明**

```chali
bool isType<T>(Any const& value);
```

**行为**

判断盒子是否保存精确的 T 类型，不比较公开整数编号。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Any const& value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。判断盒子是否保存精确的 T 类型，不比较公开整数编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
isType<T>(value);
```

**源码**：[source/std/core.chl:1200](../../source/std/core.chl#L1200)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-129"></a>

### tryGet（130）

**声明**

```chali
Option<T> tryGet<T>(Any const& value);
```

**行为**

类型匹配时复制拥有值；不匹配或空盒子返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Any const& value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<T>`。类型匹配时复制拥有值；不匹配或空盒子返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
tryGet<T>(value);
```

**源码**：[source/std/core.chl:1202](../../source/std/core.chl#L1202)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-130"></a>

### tryGetReference（131）

**声明**

```chali
Option<T const&> tryGetReference<T>(Any const& value);
```

**行为**

安全借用只读值或兼容接口，引用不得超过盒子的寿命。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Any const& value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<T const&>`。安全借用只读值或兼容接口，引用不得超过盒子的寿命。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
tryGetReference<T>(value);
```

**源码**：[source/std/core.chl:1208](../../source/std/core.chl#L1208)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-131"></a>

### tryGetMut（132）

**声明**

```chali
Option<T mut&> tryGetMut<T>(Any mut& value);
```

**行为**

安全借用可写值或兼容接口，要求盒子可写。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Any mut& value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<T mut&>`。安全借用可写值或兼容接口，要求盒子可写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
tryGetMut<T>(value);
```

**源码**：[source/std/core.chl:1213](../../source/std/core.chl#L1213)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-132"></a>

### stringBytes（133）

**声明**

```chali
List<uint8> stringBytes(String text);
```

**行为**

返回字符串的完整 UTF-8 字节副本，不执行字符边界索引。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`List<uint8>`。返回字符串的完整 UTF-8 字节副本，不执行字符边界索引。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
stringBytes(text);
```

**源码**：[source/std/core.chl:1228](../../source/std/core.chl#L1228)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-133"></a>

### BarePtr::leftAdd（134）

**声明**

```chali
BarePtr<T> leftAdd(int64 elements);
```

**行为**

实现 p + elements；按静态元素大小偏移，不访问内存或检测运行时类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `elements` | `int64 elements` | 按静态元素大小计算的指针偏移数量。 | 无 |

**返回值**

`BarePtr<T>`。实现 p + elements；按静态元素大小偏移，不访问内存或检测运行时类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftAdd(elements);
```

**源码**：[source/std/core.chl:1279](../../source/std/core.chl#L1279)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-134"></a>

### BarePtr::rightAdd（135）

**声明**

```chali
BarePtr<T> rightAdd(int64 elements);
```

**行为**

实现 elements + p，与 p + elements 相同。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `elements` | `int64 elements` | 按静态元素大小计算的指针偏移数量。 | 无 |

**返回值**

`BarePtr<T>`。实现 elements + p，与 p + elements 相同。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightAdd(elements);
```

**源码**：[source/std/core.chl:1285](../../source/std/core.chl#L1285)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-135"></a>

### BarePtr::leftSub（136）

**声明**

```chali
BarePtr<T> leftSub(int64 elements);
```

**行为**

实现 p - elements；按静态元素大小反向偏移。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `elements` | `int64 elements` | 按静态元素大小计算的指针偏移数量。 | 无 |

**返回值**

`BarePtr<T>`。实现 p - elements；按静态元素大小反向偏移。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftSub(elements);
```

**源码**：[source/std/core.chl:1287](../../source/std/core.chl#L1287)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-136"></a>

### BarePtr::leftSub（137）

**声明**

```chali
int64 leftSub(BarePtr<T> other);
```

**行为**

实现 p - other，返回完整元素距离；调用者保证属于同一分配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BarePtr<T> other` | 另一操作数 | 无 |

**返回值**

`int64`。实现 p - other，返回完整元素距离；调用者保证属于同一分配。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`bytes % __rawSize(T) == 0`。

**调用形式**

```chali
obj.leftSub(other);
```

**源码**：[source/std/core.chl:1293](../../source/std/core.chl#L1293)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-137"></a>

### barePointer（138）

**声明**

```chali
unsafe BarePtr<T> barePointer<T>(RawPtr<T> pointer);
```

**行为**

取得原始分配中的裸地址，允许未初始化及尾后位置，不检测元素类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

`BarePtr<T>`。取得原始分配中的裸地址，允许未初始化及尾后位置，不检测元素类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
barePointer(pointer);
```

**源码**：[source/std/core.chl:1303](../../source/std/core.chl#L1303)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-138"></a>

### pointerFromAddress（139）

**声明**

```chali
unsafe BarePtr<T> pointerFromAddress<T>(int64 address);
```

**行为**

将数值字节地址解释为裸指针，不验证类型、初始化状态或可解引用性。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `address` | `int64 address` | ChaLi 逻辑字节地址；调用方保证布局与有效访问范围。 | 无 |

**返回值**

`BarePtr<T>`。将数值字节地址解释为裸指针，不验证类型、初始化状态或可解引用性。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
pointerFromAddress<T>(address);
```

**源码**：[source/std/core.chl:1310](../../source/std/core.chl#L1310)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-139"></a>

### dereference（140）

**声明**

```chali
unsafe T mut& dereference<T>(BarePtr<T> pointer);
```

**行为**

将裸地址解释为可变引用；调用者负责布局、对齐、初始化和所有权正确性。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `BarePtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

`T mut&`。将裸地址解释为可变引用；调用者负责布局、对齐、初始化和所有权正确性。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
dereference(pointer);
```

**源码**：[source/std/core.chl:1325](../../source/std/core.chl#L1325)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-140"></a>

### memcpy（141）

**声明**

```chali
unsafe void memcpy<T,U>(BarePtr<T> destination, BarePtr<U> source, int64 bytes);
```

**行为**

按字节复制完整范围；不检测类型或处理资源所有权，非零范围必须有效且不重叠。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `BarePtr<T> destination` | 目标逻辑地址。 | 无 |
| `source` | `BarePtr<U> source` | 源逻辑地址；可与目标使用不同静态元素类型。 | 无 |
| `bytes` | `int64 bytes` | 复制的字节数，必须非负；零长度允许空指针。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
memcpy(destination, source, bytes);
```

**源码**：[source/std/core.chl:1332](../../source/std/core.chl#L1332)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-141"></a>

### memset（142）

**声明**

```chali
unsafe void memset<T>(BarePtr<T> destination, uint8 value, int64 bytes);
```

**行为**

按字节填充完整范围；不检测类型或清理旧值，调用者负责资源与初始化状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `BarePtr<T> destination` | 目标逻辑地址。 | 无 |
| `value` | `uint8 value` | 重复写入的单个字节。 | 无 |
| `bytes` | `int64 bytes` | 填充的字节数，必须非负；零长度允许空指针。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
memset(destination, value, bytes);
```

**源码**：[source/std/core.chl:1341](../../source/std/core.chl#L1341)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-142"></a>

### initAt（143）

**声明**

```chali
unsafe void initAt<T>(RawPtr<T> pointer, T value);
```

**行为**

在原始槽位就地接管一个完整值，目标必须未初始化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
initAt(pointer, value);
```

**源码**：[source/std/core.chl:1346](../../source/std/core.chl#L1346)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-143"></a>

### readAt（144）

**声明**

```chali
unsafe T readAt<T>(RawPtr<T> pointer);
```

**行为**

从已初始化槽位取得独立副本；需要调用方保证指针与存储寿命有效。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

`T`。从已初始化槽位取得独立副本；需要调用方保证指针与存储寿命有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
readAt(pointer);
```

**源码**：[source/std/core.chl:1354](../../source/std/core.chl#L1354)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-144"></a>

### borrowAt（145）

**声明**

```chali
unsafe T mut& borrowAt<T>(RawPtr<T> pointer);
```

**行为**

借出已初始化元素；调用方必须保证引用不超过原始存储的有效寿命。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

`T mut&`。借出已初始化元素；调用方必须保证引用不超过原始存储的有效寿命。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
borrowAt(pointer);
```

**源码**：[source/std/core.chl:1361](../../source/std/core.chl#L1361)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-145"></a>

### takeAt（146）

**声明**

```chali
unsafe T takeAt<T>(RawPtr<T> pointer);
```

**行为**

移出已初始化槽位；成功后原槽位不再持有元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

`T`。移出已初始化槽位；成功后原槽位不再持有元素。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
takeAt(pointer);
```

**源码**：[source/std/core.chl:1366](../../source/std/core.chl#L1366)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-146"></a>

### dropAt（147）

**声明**

```chali
unsafe void dropAt<T>(RawPtr<T> pointer);
```

**行为**

清理恰好一个已初始化元素，保留原始容量。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pointer` | `RawPtr<T> pointer` | 有元素类型的原始指针；调用方保证其分配与元素寿命有效。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
dropAt(pointer);
```

**源码**：[source/std/core.chl:1375](../../source/std/core.chl#L1375)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-147"></a>

### relocateNonOverlapping（148）

**声明**

```chali
unsafe void relocateNonOverlapping<T>(RawPtr<T> destination, RawPtr<T> source, int64 count);
```

**行为**

验证移动范围后逐元素转移所有权，目标范围必须全部未初始化且不重叠。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `RawPtr<T> destination` | 可写目标存储首地址 | 无 |
| `source` | `RawPtr<T> source` | 输入游标、存储或事件来源值 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`count >= 0`。
显式前置检查：`source.index >= 0 && source.index <= sourceCapacity && count <= sourceCapacity - source.index`。
显式前置检查：`destination.index >= 0 && destination.index <= destinationCapacity && count <= destinationCapacity - destination.index`。
显式前置检查：`source.storage != destination.storage || destination.index >= source.index + count || source.index >= destination.index + count`。

**调用形式**

```chali
relocateNonOverlapping(destination, source, count);
```

**源码**：[source/std/core.chl:1380](../../source/std/core.chl#L1380)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-148"></a>

### RawStorage::allocate（149）

**声明**

```chali
static RawStorage<T> allocate(int64 capacity);
```

**行为**



**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 原始存储的元素容量，必须非负。 | 无 |

**返回值**

`RawStorage<T>`。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
RawStorage<T>::allocate(capacity);
```

**源码**：[source/std/core.chl:1412](../../source/std/core.chl#L1412)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-149"></a>

### RawStorage::capacity（150）

**声明**

```chali
int64 capacity();
```

**行为**



**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.capacity();
```

**源码**：[source/std/core.chl:1420](../../source/std/core.chl#L1420)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-150"></a>

### RawStorage::pointer（151）

**声明**

```chali
RawPtr<T> pointer(int64 index);
```

**行为**



**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`RawPtr<T>`。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.handle != 0u && index >= 0 && index <= self.capacity()`。

**调用形式**

```chali
obj.pointer(index);
```

**源码**：[source/std/core.chl:1425](../../source/std/core.chl#L1425)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-151"></a>

### RawStorage::isInitialized（152）

**声明**

```chali
bool isInitialized(int64 index);
```

**行为**



**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`bool`。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isInitialized(index);
```

**源码**：[source/std/core.chl:1433](../../source/std/core.chl#L1433)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-152"></a>

### RawStorage::close（153）

**声明**

```chali
void close();
```

**行为**



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

**源码**：[source/std/core.chl:1437](../../source/std/core.chl#L1437)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-153"></a>

### Box::newBox（154）

**声明**

```chali
static Box<T> newBox(T value);
```

**行为**

把一个值放入独占的堆盒。可复制的实参可按值传入，转移所有权时使用 move。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 要装箱的值。 | 无 |

**返回值**

`Box<T>`。持有该值的 Box。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Box<T>::newBox(value);
```

**源码**：[source/std/core.chl:1463](../../source/std/core.chl#L1463)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-154"></a>

### Box::isEmpty（155）

**声明**

```chali
bool isEmpty();
```

**行为**

查询当前 Box 是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。默认构造或关闭后为 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/core.chl:1473](../../source/std/core.chl#L1473)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-155"></a>

### Box::get（156）

**声明**

```chali
T get();
```

**行为**

复制盒中的值，要求 T 可复制。空盒触发断言。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。盒中值的独立副本。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`!self.isEmpty(), "Box 为空"`。

**调用形式**

```chali
obj.get();
```

**源码**：[source/std/core.chl:1478](../../source/std/core.chl#L1478)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-156"></a>

### Box::take（157）

**声明**

```chali
T take();
```

**行为**

消耗当前 Box 并释放堆存储。空盒触发断言。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。从盒中移出的值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`!self.isEmpty(), "Box 为空"`。

**调用形式**

```chali
obj.take();
```

**源码**：[source/std/core.chl:1486](../../source/std/core.chl#L1486)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-157"></a>

### Box::close（158）

**声明**

```chali
void close();
```

**行为**

销毁仍在盒中的值并释放堆存储。可以重复关闭。

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

**源码**：[source/std/core.chl:1493](../../source/std/core.chl#L1493)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-158"></a>

### range（159）

**声明**

```chali
__Range range(int64 start, int64 stop);
```

**行为**

创建半开整数区间；正步长小于 stop，负步长大于 stop，步长不能为零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `stop` | `int64 stop` | 不包含的终止值 | 无 |

**返回值**

`__Range`。创建半开整数区间；正步长小于 stop，负步长大于 stop，步长不能为零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
range(start, stop);
```

**源码**：[source/chali_generic.cpp:2067](../../source/chali_generic.cpp#L2067)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-159"></a>

### range（160）

**声明**

```chali
__Range range(int64 start, int64 stop, int64 step);
```

**行为**

创建半开整数区间；正步长小于 stop，负步长大于 stop，步长不能为零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |
| `stop` | `int64 stop` | 不包含的终止值 | 无 |
| `step` | `int64 step` | 步长或时间增量 | 无 |

**返回值**

`__Range`。创建半开整数区间；正步长小于 stop，负步长大于 stop，步长不能为零。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`step != 0, "range 的步长不能为 0"`。

**调用形式**

```chali
range(start, stop, step);
```

**源码**：[source/chali_generic.cpp:2075](../../source/chali_generic.cpp#L2075)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-160"></a>

### rangeEq（161）

**声明**

```chali
__Interval_int64 rangeEq(int64 start, int64 stop);
```

**行为**

构造包含起点和终点的值区间；整数和 char 直接计算，其他类型通过 NextValue<T> 迭代。match 区间模式仅支持整数和 char。

构造包含起点和终点的整数区间。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 区间起点。 | 无 |
| `stop` | `int64 stop` | 包含在区间内的终点。 | 无 |

**返回值**

`__Interval_int64`。保存独立游标的闭区间值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
rangeEq(start, stop);
```

**源码**：[source/chali_generic.cpp:2090](../../source/chali_generic.cpp#L2090)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-161"></a>

### Drop::drop（162）

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

**源码**：[source/chali_generic.cpp:2296](../../source/chali_generic.cpp#L2296)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-162"></a>

### Closeable::close（163）

**声明**

```chali
void close();
```

**行为**

关闭底层资源；文档节点会使同文档所有视图失效。

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

**源码**：[source/chali_generic.cpp:2304](../../source/chali_generic.cpp#L2304)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-163"></a>

### print（164）

**声明**

```chali
int64 print(String format, Any args...);
```

**行为**

输出格式化文本，返回过程状态；占位符和变长实参由编译器检查。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `format` | `String format` | 格式模板，使用 {} 占位符 | 无 |
| `args` | `Any args...` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`int64`。输出格式化文本，返回过程状态；占位符和变长实参由编译器检查。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
print(format, args);
```

**源码**：[source/semantics/boxing.cpp:1363](../../source/semantics/boxing.cpp#L1363)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-164"></a>

### println（165）

**声明**

```chali
int64 println(String format, Any args...);
```

**行为**

输出格式化文本并换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `format` | `String format` | 格式模板，使用 {} 占位符 | 无 |
| `args` | `Any args...` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`int64`。输出格式化文本并换行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
println(format, args);
```

**源码**：[source/semantics/boxing.cpp:1363](../../source/semantics/boxing.cpp#L1363)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-165"></a>

### eprint（166）

**声明**

```chali
int64 eprint(String format, Any args...);
```

**行为**

格式化写入标准错误流，不追加换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `format` | `String format` | 格式模板，使用 {} 占位符 | 无 |
| `args` | `Any args...` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`int64`。格式化写入标准错误流，不追加换行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
eprint(format, args);
```

**源码**：[source/semantics/formatted_output.cpp:15](../../source/semantics/formatted_output.cpp#L15)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-166"></a>

### eprintln（167）

**声明**

```chali
int64 eprintln(String format, Any args...);
```

**行为**

格式化写入标准错误流并追加换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `format` | `String format` | 格式模板，使用 {} 占位符 | 无 |
| `args` | `Any args...` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`int64`。格式化写入标准错误流并追加换行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
eprintln(format, args);
```

**源码**：[source/semantics/formatted_output.cpp:15](../../source/semantics/formatted_output.cpp#L15)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-167"></a>

### format（168）

**声明**

```chali
String format(String format, Any args...);
```

**行为**

返回格式化的新字符串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `format` | `String format` | 格式模板，使用 {} 占位符 | 无 |
| `args` | `Any args...` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`String`。返回格式化的新字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
format(format, args);
```

**源码**：[source/semantics/boxing.cpp:1367](../../source/semantics/boxing.cpp#L1367)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-168"></a>

### putChar（169）

**声明**

```chali
int64 putChar(char value);
```

**行为**

输出一个字符。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `char value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。输出一个字符。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
putChar(value);
```

**源码**：[source/semantics/boxing.cpp:1413](../../source/semantics/boxing.cpp#L1413)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-169"></a>

### pause（170）

**声明**

```chali
int64 pause();
```

**行为**

调试暂停，提示写入标准错误；非交互输入立即返回。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。调试暂停，提示写入标准错误；非交互输入立即返回。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
pause();
```

**源码**：[source/semantics/boxing.cpp:1374](../../source/semantics/boxing.cpp#L1374)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-170"></a>

### inputLine（171）

**声明**

```chali
String inputLine();
```

**行为**

读取下一项原文，输入项按空白分隔，不是整行。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。读取下一项原文，输入项按空白分隔，不是整行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputLine();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-171"></a>

### inputInt（172）

**声明**

```chali
int64 inputInt();
```

**行为**

读取下一项有符号整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取下一项有符号整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputInt();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-172"></a>

### inputUInt（173）

**声明**

```chali
uint64 inputUInt();
```

**行为**

读取下一项无符号整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint64`。读取下一项无符号整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputUInt();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-173"></a>

### inputFloat（174）

**声明**

```chali
float64 inputFloat();
```

**行为**

读取下一项浮点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。读取下一项浮点数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputFloat();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-174"></a>

### inputBool（175）

**声明**

```chali
bool inputBool();
```

**行为**

读取下一项布尔值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取下一项布尔值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputBool();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-175"></a>

### inputChar（176）

**声明**

```chali
char inputChar();
```

**行为**

读取下一项字符。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`char`。读取下一项字符。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputChar();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-176"></a>

### inputBigInteger（177）

**声明**

```chali
BigInteger inputBigInteger();
```

**行为**

读取下一项任意精度整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。读取下一项任意精度整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputBigInteger();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-177"></a>

### inputBigDecimal（178）

**声明**

```chali
BigDecimal inputBigDecimal();
```

**行为**

读取下一项任意精度小数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigDecimal`。读取下一项任意精度小数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
inputBigDecimal();
```

**源码**：[source/semantics/boxing.cpp:1423](../../source/semantics/boxing.cpp#L1423)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-178"></a>

### panic（179）

**声明**

```chali
int64 panic(String message);
```

**行为**

终止当前执行并报告 message。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |

**返回值**

`int64`。终止当前执行并报告 message。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
panic(message);
```

**源码**：[source/semantics/boxing.cpp:1442](../../source/semantics/boxing.cpp#L1442)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-179"></a>

### len（180）

**声明**

```chali
int64 len(Any value);
```

**行为**

返回当前元素数量。

返回字符串字节长度、数组元素数量或容器长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Any value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
len(value);
```

**源码**：[source/semantics/boxing.cpp:793](../../source/semantics/boxing.cpp#L793)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-180"></a>

### toReference（181）

**声明**

```chali
T mut& toReference<T>(Option<T mut&> value);
```

**行为**

将可选可写引用转换成可写引用；None 无法转为有效引用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `Option<T mut&> value` | 输入值或写入的新值 | 无 |

**返回值**

`T mut&`。将可选可写引用转换成可写引用；None 无法转为有效引用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toReference<T>(value);
```

**源码**：[source/chali_generic.cpp:2004](../../source/chali_generic.cpp#L2004)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-181"></a>

### toOptional（182）

**声明**

```chali
Option<T mut&> toOptional<T>(T mut& value);
```

**行为**

将可写引用封装成可选引用，不复制指向的对象。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T mut& value` | 输入值或写入的新值 | 无 |

**返回值**

`Option<T mut&>`。将可写引用封装成可选引用，不复制指向的对象。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toOptional<T>(value);
```

**源码**：[source/chali_generic.cpp:2010](../../source/chali_generic.cpp#L2010)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-182"></a>

### memcpy（183）

**声明**

```chali
int64 memcpy(char mut& destination, char const& source, int64 bytes);
```

**行为**

按字节复制内存；调用者保证源与目标存储范围有效。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `char mut& destination` | 可写目标存储首地址 | 无 |
| `source` | `char const& source` | 输入游标、存储或事件来源值 | 无 |
| `bytes` | `int64 bytes` | 输入或输出字节存储 | 无 |

**返回值**

`int64`。按字节复制内存；调用者保证源与目标存储范围有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
memcpy(destination, source, bytes);
```

**源码**：[source/semantics/boxing.cpp:1459](../../source/semantics/boxing.cpp#L1459)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-183"></a>

### memset（184）

**声明**

```chali
int64 memset(char mut& destination, int64 value, int64 bytes);
```

**行为**

按字节填充存储；调用者保证目标范围有效。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `char mut& destination` | 可写目标存储首地址 | 无 |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |
| `bytes` | `int64 bytes` | 输入或输出字节存储 | 无 |

**返回值**

`int64`。按字节填充存储；调用者保证目标范围有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
memset(destination, value, bytes);
```

**源码**：[source/semantics/boxing.cpp:1459](../../source/semantics/boxing.cpp#L1459)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-184"></a>

### List::copy（185）

**声明**

```chali
List<T> copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<T>`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/aggregate_ownership.cpp:257](../../source/semantics/aggregate_ownership.cpp#L257)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-185"></a>

### List::push（186）

**声明**

```chali
int64 push(T value);
```

**行为**

将 value 追加到列表尾部。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.push(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:258](../../source/semantics/aggregate_ownership.cpp#L258)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-186"></a>

### List::insert（187）

**声明**

```chali
int64 insert(int64 index, T value);
```

**行为**

在 index 之前插入 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.insert(index, value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:259](../../source/semantics/aggregate_ownership.cpp#L259)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-187"></a>

### List::at（188）

**声明**

```chali
T at(int64 index);
```

**行为**

读取从 0 开始的指定位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(index);
```

**源码**：[source/semantics/aggregate_ownership.cpp:260](../../source/semantics/aggregate_ownership.cpp#L260)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-188"></a>

### List::pop（189）

**声明**

```chali
T pop();
```

**行为**

移除列表尾元素并返回值，空列表时报错。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。移除列表尾元素并返回值，空列表时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/semantics/aggregate_ownership.cpp:261](../../source/semantics/aggregate_ownership.cpp#L261)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-189"></a>

### List::remove（190）

**声明**

```chali
int64 remove(int64 index);
```

**行为**

删除指定位置或键的元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(index);
```

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-190"></a>

### List::indexOf（191）

**声明**

```chali
int64 indexOf(T value);
```

**行为**

返回首次出现的下标；未找到返回 -1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。返回首次出现的下标；未找到返回 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.indexOf(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:263](../../source/semantics/aggregate_ownership.cpp#L263)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-191"></a>

### List::contains（192）

**声明**

```chali
bool contains(T value);
```

**行为**

判断是否包含目标值或匹配文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-192"></a>

### List::has（193）

**声明**

```chali
bool has(T value);
```

**行为**

检查是否包含指定值或键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。检查是否包含指定值或键。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.has(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-193"></a>

### List::clear（194）

**声明**

```chali
int64 clear();
```

**行为**

移除全部元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/semantics/aggregate_ownership.cpp:266](../../source/semantics/aggregate_ownership.cpp#L266)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-194"></a>

### List::len（195）

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

**源码**：[source/semantics/aggregate_ownership.cpp:267](../../source/semantics/aggregate_ownership.cpp#L267)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-195"></a>

### Set::copy（196）

**声明**

```chali
Set<T> copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Set<T>`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/aggregate_ownership.cpp:257](../../source/semantics/aggregate_ownership.cpp#L257)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-196"></a>

### Set::add（197）

**声明**

```chali
int64 add(T value);
```

**行为**

计算逐项相加或追加元素，具体操作取决于所属类型。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.add(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:275](../../source/semantics/aggregate_ownership.cpp#L275)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-197"></a>

### Set::has（198）

**声明**

```chali
int64 has(T value);
```

**行为**

检查是否包含指定值或键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。存在返回 1，不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.has(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-198"></a>

### Set::contains（199）

**声明**

```chali
int64 contains(T value);
```

**行为**

判断是否包含目标值或匹配文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。存在返回 1，不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-199"></a>

### Set::remove（200）

**声明**

```chali
int64 remove(T value);
```

**行为**

删除指定位置或键的元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。找到并删除返回 1，键不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-200"></a>

### Set::at（201）

**声明**

```chali
T at(int64 index);
```

**行为**

读取从 0 开始的指定位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(index);
```

**源码**：[source/semantics/aggregate_ownership.cpp:260](../../source/semantics/aggregate_ownership.cpp#L260)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-201"></a>

### Set::clear（202）

**声明**

```chali
int64 clear();
```

**行为**

移除全部元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/semantics/aggregate_ownership.cpp:266](../../source/semantics/aggregate_ownership.cpp#L266)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-202"></a>

### Set::len（203）

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

**源码**：[source/semantics/aggregate_ownership.cpp:267](../../source/semantics/aggregate_ownership.cpp#L267)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-203"></a>

### Map::copy（204）

**声明**

```chali
Map<K,V> copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Map<K,V>`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/aggregate_ownership.cpp:257](../../source/semantics/aggregate_ownership.cpp#L257)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-204"></a>

### Map::put（205）

**声明**

```chali
int64 put(K key,V value);
```

**行为**

写入 key 对应的 value，已有键被替换。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `K key` | 查找键或下标 | 无 |
| `value` | `V value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.put(key, value);
```

**源码**：[source/semantics/aggregate_ownership.cpp:289](../../source/semantics/aggregate_ownership.cpp#L289)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-205"></a>

### Map::has（206）

**声明**

```chali
int64 has(K key);
```

**行为**

检查是否包含指定值或键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `K key` | 查找键或下标 | 无 |

**返回值**

`int64`。存在返回 1，不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.has(key);
```

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-206"></a>

### Map::contains（207）

**声明**

```chali
int64 contains(K key);
```

**行为**

判断是否包含目标值或匹配文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `K key` | 查找键或下标 | 无 |

**返回值**

`int64`。存在返回 1，不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(key);
```

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-207"></a>

### Map::remove（208）

**声明**

```chali
int64 remove(K key);
```

**行为**

删除指定位置或键的元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `K key` | 查找键或下标 | 无 |

**返回值**

`int64`。找到并删除返回 1，键不存在返回 0。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(key);
```

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-208"></a>

### Map::keyAt（209）

**声明**

```chali
K keyAt(int64 index);
```

**行为**

按遍历位置读取键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`K`。按遍历位置读取键。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.keyAt(index);
```

**源码**：[source/semantics/aggregate_ownership.cpp:293](../../source/semantics/aggregate_ownership.cpp#L293)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-209"></a>

### Map::valAt（210）

**声明**

```chali
V valAt(int64 index);
```

**行为**

按遍历位置读取值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`V`。按遍历位置读取值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.valAt(index);
```

**源码**：[source/semantics/aggregate_ownership.cpp:294](../../source/semantics/aggregate_ownership.cpp#L294)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-210"></a>

### Map::clear（211）

**声明**

```chali
int64 clear();
```

**行为**

移除全部元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。成功时返回 0；错误通过运行时诊断报告，该返回值不是成功标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/semantics/aggregate_ownership.cpp:266](../../source/semantics/aggregate_ownership.cpp#L266)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-211"></a>

### Map::len（212）

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

**源码**：[source/semantics/aggregate_ownership.cpp:267](../../source/semantics/aggregate_ownership.cpp#L267)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-212"></a>

### Any::copy（213）

**声明**

```chali
Any copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Any`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/boxing.cpp:123](../../source/semantics/boxing.cpp#L123)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-213"></a>

### Any::isEmpty（214）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

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

**源码**：[source/semantics/boxing.cpp:1046](../../source/semantics/boxing.cpp#L1046)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-214"></a>

### Any::clear（215）

**声明**

```chali
void clear();
```

**行为**

释放盒子拥有的载荷并恢复空状态；保留盒子自身的键身份。

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

**源码**：[source/semantics/boxing.cpp:1053](../../source/semantics/boxing.cpp#L1053)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-215"></a>

### Any::set（216）

**声明**

```chali
int64 set(T value);
```

**行为**

以 value 的值副本和类型编号替换盒子内容。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。以 value 的值副本和类型编号替换盒子内容。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(value);
```

**源码**：[source/semantics/boxing.cpp:1069](../../source/semantics/boxing.cpp#L1069)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-216"></a>

### Any::getValue（217）

**声明**

```chali
T getValue();
```

**行为**

按接收上下文要求的类型取出拥有值，类型不匹配时报错。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。按接收上下文要求的类型取出拥有值，类型不匹配时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.getValue();
```

**源码**：[source/semantics/boxing.cpp:150](../../source/semantics/boxing.cpp#L150)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-217"></a>

### Any::getReference（218）

**声明**

```chali
T mut& getReference();
```

**行为**

按上下文要求的类型借用盒子内的值，引用受盒子生命周期约束。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T mut&`。按上下文要求的类型借用盒子内的值，引用受盒子生命周期约束。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.getReference();
```

**源码**：[source/semantics/boxing.cpp:150](../../source/semantics/boxing.cpp#L150)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-218"></a>

### Any::getOptional（219）

**声明**

```chali
Option<T mut&> getOptional();
```

**行为**

按上下文要求的类型获取可选引用，类型不匹配为 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<T mut&>`。按上下文要求的类型获取可选引用，类型不匹配为 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.getOptional();
```

**源码**：[source/semantics/boxing.cpp:150](../../source/semantics/boxing.cpp#L150)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-219"></a>

### eq（220）

**声明**

```chali
bool eq(T left,T right);
```

**行为**

内建运算函数，等价于 left == right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left == right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
eq(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-220"></a>

### ne（221）

**声明**

```chali
bool ne(T left,T right);
```

**行为**

内建运算函数，等价于 left != right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left != right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
ne(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-221"></a>

### lt（222）

**声明**

```chali
bool lt(T left,T right);
```

**行为**

内建运算函数，等价于 left < right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left < right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
lt(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-222"></a>

### le（223）

**声明**

```chali
bool le(T left,T right);
```

**行为**

内建运算函数，等价于 left <= right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left <= right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
le(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-223"></a>

### gt（224）

**声明**

```chali
bool gt(T left,T right);
```

**行为**

内建运算函数，等价于 left > right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left > right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
gt(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-224"></a>

### ge（225）

**声明**

```chali
bool ge(T left,T right);
```

**行为**

内建运算函数，等价于 left >= right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`bool`。内建运算函数，等价于 left >= right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
ge(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-225"></a>

### add（226）

**声明**

```chali
R add(T left,U right);
```

**行为**

计算逐项相加或追加元素，具体操作取决于所属类型。

内建运算函数，等价于 left + right，结果由运算符解析决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `U right` | 右操作数或区间右端 | 无 |

**返回值**

`R`。计算逐项相加或追加元素，具体操作取决于所属类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
add(left, right);
```

**源码**：[source/semantics/boxing.cpp:1227](../../source/semantics/boxing.cpp#L1227)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-226"></a>

### sub（227）

**声明**

```chali
R sub(T left,U right);
```

**行为**

内建运算函数，等价于 left - right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `U right` | 右操作数或区间右端 | 无 |

**返回值**

`R`。内建运算函数，等价于 left - right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
sub(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-227"></a>

### mul（228）

**声明**

```chali
R mul(T left,U right);
```

**行为**

内建运算函数，等价于 left * right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `U right` | 右操作数或区间右端 | 无 |

**返回值**

`R`。内建运算函数，等价于 left * right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
mul(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-228"></a>

### div（229）

**声明**

```chali
R div(T left,U right);
```

**行为**

内建运算函数，等价于 left / right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `U right` | 右操作数或区间右端 | 无 |

**返回值**

`R`。内建运算函数，等价于 left / right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
div(left, right);
```

**源码**：[source/semantics/boxing.cpp:1](../../source/semantics/boxing.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-229"></a>

### mod（230）

**声明**

```chali
R mod(T left,U right);
```

**行为**

内建运算函数，等价于 left % right。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `T left` | 左操作数或区间左端 | 无 |
| `right` | `U right` | 右操作数或区间右端 | 无 |

**返回值**

`R`。内建运算函数，等价于 left % right。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
mod(left, right);
```

**源码**：[source/semantics/boxing.cpp:929](../../source/semantics/boxing.cpp#L929)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-230"></a>

### index（231）

**声明**

```chali
T index(T const& data,int64 index);
```

**行为**

读取 key 对应的元素，供下标语法调用。

读取连续存储的第 index 项，相当于下标操作。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T const& data` | 连续存储的首元素引用，调用者保证范围有效 | 无 |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::core::index(data, index);
```

**源码**：[source/semantics/boxing.cpp:771](../../source/semantics/boxing.cpp#L771)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-231"></a>

### Add::leftAdd（232）

**声明**

```chali
R leftAdd(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftAdd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftAdd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftAdd(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-232"></a>

### RightAdd::rightAdd（233）

**声明**

```chali
R rightAdd(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightAdd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightAdd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightAdd(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-233"></a>

### Subtract::leftSub（234）

**声明**

```chali
R leftSub(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftSub；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftSub；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftSub(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-234"></a>

### RightSubtract::rightSub（235）

**声明**

```chali
R rightSub(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightSub；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightSub；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightSub(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-235"></a>

### Multiply::leftMul（236）

**声明**

```chali
R leftMul(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftMul；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftMul；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMul(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-236"></a>

### RightMultiply::rightMul（237）

**声明**

```chali
R rightMul(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightMul；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightMul；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightMul(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-237"></a>

### Divide::leftDiv（238）

**声明**

```chali
R leftDiv(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftDiv；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftDiv；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftDiv(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-238"></a>

### RightDivide::rightDiv（239）

**声明**

```chali
R rightDiv(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightDiv；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightDiv；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightDiv(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-239"></a>

### Modulo::leftMod（240）

**声明**

```chali
R leftMod(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftMod；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftMod；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMod(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-240"></a>

### RightModulo::rightMod（241）

**声明**

```chali
R rightMod(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightMod；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightMod；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightMod(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-241"></a>

### Equal::leftEqual（242）

**声明**

```chali
R leftEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-242"></a>

### RightEqual::rightEqual（243）

**声明**

```chali
R rightEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-243"></a>

### NotEqual::leftNotEqual（244）

**声明**

```chali
R leftNotEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftNotEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftNotEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftNotEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-244"></a>

### RightNotEqual::rightNotEqual（245）

**声明**

```chali
R rightNotEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightNotEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightNotEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightNotEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-245"></a>

### Less::leftLess（246）

**声明**

```chali
R leftLess(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftLess；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftLess；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftLess(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-246"></a>

### RightLess::rightLess（247）

**声明**

```chali
R rightLess(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightLess；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightLess；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightLess(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-247"></a>

### LessOrEqual::leftLessEqual（248）

**声明**

```chali
R leftLessEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftLessEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftLessEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftLessEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-248"></a>

### RightLessOrEqual::rightLessEqual（249）

**声明**

```chali
R rightLessEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightLessEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightLessEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightLessEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-249"></a>

### Greater::leftGreater（250）

**声明**

```chali
R leftGreater(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftGreater；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftGreater；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftGreater(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-250"></a>

### RightGreater::rightGreater（251）

**声明**

```chali
R rightGreater(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightGreater；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightGreater；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightGreater(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-251"></a>

### GreaterOrEqual::leftGreaterEqual（252）

**声明**

```chali
R leftGreaterEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftGreaterEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftGreaterEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftGreaterEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-252"></a>

### RightGreaterOrEqual::rightGreaterEqual（253）

**声明**

```chali
R rightGreaterEqual(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightGreaterEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightGreaterEqual；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightGreaterEqual(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-253"></a>

### And::leftBitAnd（254）

**声明**

```chali
R leftBitAnd(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftBitAnd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftBitAnd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftBitAnd(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-254"></a>

### RightAnd::rightBitAnd（255）

**声明**

```chali
R rightBitAnd(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightBitAnd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightBitAnd；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightBitAnd(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-255"></a>

### Or::leftBitOr（256）

**声明**

```chali
R leftBitOr(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftBitOr；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftBitOr；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftBitOr(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-256"></a>

### RightOr::rightBitOr（257）

**声明**

```chali
R rightBitOr(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightBitOr；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightBitOr；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightBitOr(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-257"></a>

### Xor::leftBitXor（258）

**声明**

```chali
R leftBitXor(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftBitXor；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftBitXor；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftBitXor(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-258"></a>

### RightXor::rightBitXor（259）

**声明**

```chali
R rightBitXor(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightBitXor；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightBitXor；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightBitXor(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-259"></a>

### ShiftLeft::leftShiftLeft（260）

**声明**

```chali
R leftShiftLeft(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftShiftLeft；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftShiftLeft；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftShiftLeft(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-260"></a>

### RightShiftLeft::rightShiftLeft（261）

**声明**

```chali
R rightShiftLeft(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightShiftLeft；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightShiftLeft；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightShiftLeft(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-261"></a>

### ShiftRight::leftShiftRight（262）

**声明**

```chali
R leftShiftRight(T value);
```

**行为**

用户类型实现此运算符接口时提供 leftShiftRight；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 leftShiftRight；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftShiftRight(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-262"></a>

### RightShiftRight::rightShiftRight（263）

**声明**

```chali
R rightShiftRight(T value);
```

**行为**

用户类型实现此运算符接口时提供 rightShiftRight；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 rightShiftRight；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rightShiftRight(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-263"></a>

### Negate::negate（264）

**声明**

```chali
R negate(T value);
```

**行为**

用户类型实现此运算符接口时提供 negate；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 negate；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.negate(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-264"></a>

### BitNot::bitNot（265）

**声明**

```chali
R bitNot(T value);
```

**行为**

用户类型实现此运算符接口时提供 bitNot；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 bitNot；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bitNot(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-265"></a>

### Positive::positive（266）

**声明**

```chali
R positive(T value);
```

**行为**

用户类型实现此运算符接口时提供 positive；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`R`。用户类型实现此运算符接口时提供 positive；R 为运算结果类型，T 为操作数类型，左右方向由接口名决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.positive(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-266"></a>

### Hash::hash（267）

**声明**

```chali
uint64 hash(T value);
```

**行为**

返回输入值的哈希值，由实现者保持与相等性的一致性。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

`uint64`。返回输入值的哈希值，由实现者保持与相等性的一致性。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.hash(value);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-267"></a>

### Copy::copy（268）

**声明**

```chali
T copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/chali_generic.cpp:1987](../../source/chali_generic.cpp#L1987)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-268"></a>

### ExplicitConvert::explicitConvert（269）

**声明**

```chali
T explicitConvert();
```

**行为**

由用户类型提供显式目标类型转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。由用户类型提供显式目标类型转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.explicitConvert();
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-269"></a>

### ImplicitConvert::implicitConvert（270）

**声明**

```chali
T implicitConvert();
```

**行为**

由用户类型提供隐式目标类型转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T`。由用户类型提供隐式目标类型转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.implicitConvert();
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-270"></a>

### Index::index（271）

**声明**

```chali
R index(T key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `T key` | 查找键或下标 | 无 |

**返回值**

`R`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-271"></a>

### Iterator::iterator（272）

**声明**

```chali
Option<R> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<R>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/chali_generic.cpp:827](../../source/chali_generic.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-272"></a>

### Result::mapErr（273）

**声明**

```chali
Result<T,F> mapErr<F>(F fn(E) mapper);
```

**行为**

仅在失败时调用 mapper 转换错误类型，成功值保持不变。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `mapper` | `F fn(E) mapper` | 转换回调，签名决定输入和输出类型 | 无 |

**返回值**

`Result<T,F>`。仅在失败时调用 mapper 转换错误类型，成功值保持不变。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.mapErr(mapper);
```

**源码**：[source/std/core.chl:2](../../source/std/core.chl#L2)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-273"></a>

### Option::okOr（274）

**声明**

```chali
Result<T,E> okOr<E>(E error);
```

**行为**

将 None 转为给定错误，error 按普通实参规则求值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `error` | `E error` | 缺失时产生的错误值。 | 无 |

**返回值**

`Result<T,E>`。将 None 转为给定错误，error 按普通实参规则求值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.okOr(error);
```

**源码**：[source/std/core.chl:3](../../source/std/core.chl#L3)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-274"></a>

### Option::okOrElse（275）

**声明**

```chali
Result<T,E> okOrElse<E>(E fn() factory);
```

**行为**

只在 None 时调用错误工厂，成功分支不调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `factory` | `E fn() factory` | 缺失时才调用的无参数错误工厂。 | 无 |

**返回值**

`Result<T,E>`。只在 None 时调用错误工厂，成功分支不调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.okOrElse(factory);
```

**源码**：[source/std/core.chl:3](../../source/std/core.chl#L3)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-275"></a>

### BarePtr::castTo（276）

**声明**

```chali
unsafe BarePtr<U> castTo<U>();
```

**行为**



**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BarePtr<U>`。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.castTo<U>();
```

**源码**：[source/std/core.chl:4](../../source/std/core.chl#L4)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

## 补充完整示例

[example/samples/std_read_lines.chl](../../example/samples/std_read_lines.chl)。

```chali
void main() {
    String first=readLine();
    String rest=readLines(3);
    println("[{}]",first);
    println("[{}]",rest);
    assert(readLines(0)=="");
    assert(readLine()=="");
}
```

## 补充完整示例

[example/samples/std_box.chl](../../example/samples/std_box.chl)。

```chali
import std::macro;
import std::memory;

int64 closed = 0;
int64 calls = 0;

struct Resource : Closeable {
    int64 id;
    void close() { closed += self.id; }
};

int64 observed(int64 value) {
    calls += 1;
    return value;
}

void main() {
    assert($IN_RANGE(observed(4), observed(4), observed(6)));
    assert(calls == 3);
    assert($IN_RANGE(6, 4, 6));
    assert(!$IN_RANGE(7, 4, 6));
    assert(!$IN_RANGE(5, 6, 4));

    assert(Box<int64>::newBox(42).take() == 42);
    std::core::Box<int64> empty;
    assert(empty.isEmpty());
    empty.close();

    int64 before = Memory::stats().liveBlocks;
    {
        std::core::Box<List<int64>> numbers = Box<List<int64>>::newBox([2, 3]);
        List<int64> copy = numbers.get();
        copy.push(4);
        assert(numbers.get().len() == 2);
        List<int64> taken = numbers.take();
        assert(taken[0] == 2 && taken[1] == 3);
    }
    {
        Resource resource;
        resource.id = 7;
        let owned = $BOX(move resource);
        let moved = move owned;
        let extracted = moved.take();
        assert(extracted.id == 7 && closed == 0);
    }
    assert(closed == 7);
    {
        Resource resource;
        resource.id = 11;
        let owned = Box<Resource>::newBox(move resource);
    }
    assert(closed == 18);
    {
        Resource resource;
        resource.id = 13;
        let owned = std::core::Box<Resource>::newBox(move resource);
        owned.close();
        assert(owned.isEmpty());
        owned.close();
    }
    assert(closed == 31);
    assert(Memory::stats().liveBlocks == before);
    println("Box and standard macros OK");
}
```
