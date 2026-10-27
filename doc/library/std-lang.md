# std::lang — 核心库

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-lang.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
// std::lang 自动加载
```

## 公共约定与错误边界

自动加载，无需 import。标准库错误统一实现 Error.toString()；文本诊断使用 MessageError，保留原始消息。String 提供字节查找、切片、替换、修剪、填充、连接、分割、ASCII 大小写与分类，以及严格 UTF-8 码点操作；普通方法返回独立值，字节索引与码点索引明确区分，详情见字符串方法文档。Option/Result 通过 match 或 isValue/isOk 分支处理；unwrap/expect 在失败分支触发 panic。浮点转整数向零截断，范围检查见各重载；字节编码默认小端。拥有值按值复制，引用的生命周期不得超过来源。 List.pop 空列表报错，列表/集合/映射的按位置访问需有效下标。Set/Map 的 has/contains 返回 int64 的 0/1；List 的同名接口返回 bool。Any.getReference/getOptional 的 const/mut 由接收上下文决定，返回引用受盒子寿命限制。BigDecimal 精度范围 1..100000。内存内建接受有效存储引用，字节计数不是元素数量。$assert(condition) 和 $assert(condition,message) 为宏调用，自动添加调用点行号，宏体自带分号。

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
| 模块 | <a id="symbol-219"></a>`opcode String` | 拥有 UTF-8 文本的 opcode 值，复制产生独立可写缓冲。 |
| 模块 | <a id="symbol-220"></a>`interface Error` | 标准库错误的统一接口；诊断文本保留具体失败原因。 |
| 模块 | <a id="symbol-221"></a>`struct MessageError : Error` | 只有文本诊断时使用的标准错误对象，消息按值独立拥有。 |
| MessageError | <a id="symbol-222"></a>`String message` | 供用户阅读的错误原因。 |
| 模块 | <a id="symbol-223"></a>`opcode BigInteger` | 任意精度有符号整数，算术由大数运行时实现。 |
| 模块 | <a id="symbol-224"></a>`opcode BigDecimal` | 可配置精度的十进制大数。 |
| 模块 | <a id="symbol-225"></a>`#NonCopyable opcode File` | 不可复制的文件 opcode，作用域退出释放文件资源。 |
| 模块 | <a id="symbol-226"></a>`#NonCopyable opcode Process` | 不可复制的进程 opcode，操作由宿主胶水提供。 |
| 模块 | <a id="symbol-227"></a>`enum TryBranch<T, R>` | 传播分支：Continue 产生成功值，Break 携带失败载荷退出当前函数。 |
| TryBranch | `Continue(T value), Break(R residual)` | 传播分支：Continue 产生成功值，Break 携带失败载荷退出当前函数。 变体及载荷见声明。 |
| 模块 | <a id="symbol-228"></a>`struct ErrorResidual<E>` | Result 的错误传播载荷，保留错误类型身份。 |
| ErrorResidual | <a id="symbol-229"></a>`E error` | 最近操作的错误说明。 |
| 模块 | <a id="symbol-230"></a>`enum NoneResidual` | Option 无值分支的传播载荷。 |
| NoneResidual | `None` | Option 无值分支的传播载荷。 变体及载荷见声明。 |
| 模块 | <a id="symbol-231"></a>`interface Try<T, R>` | 只读拆分传播值的显式编译期协议。 |
| 模块 | <a id="symbol-232"></a>`interface TryMove<T, R>` | 移动拥有值后拆分传播值的显式编译期协议。 |
| 模块 | <a id="symbol-233"></a>`interface FromResidual<F, R>` | 以静态方法从传播载荷构造函数返回值的编译期协议。 |
| 模块 | <a id="symbol-234"></a>`enum Option<T> : Try<T, NoneResidual>, TryMove<T, NoneResidual>, FromResidual<Option<T>, NoneResidual>` | 可选值：Value(T) 表示存在，None 表示缺失。 |
| Option | `Value(T value), None` | 可选值：Value(T) 表示存在，None 表示缺失。 变体及载荷见声明。 |
| 模块 | <a id="symbol-235"></a>`enum Result<T, E> : Try<T, ErrorResidual<E>>, TryMove<T, ErrorResidual<E>>, FromResidual<Result<T, E>, ErrorResidual<E>>` | 操作结果：Ok(T) 表示成功，Err(E) 保存错误。 |
| Result | `Ok(T value), Err(E error)` | 操作结果：Ok(T) 表示成功，Err(E) 保存错误。 变体及载荷见声明。 |
| 模块 | <a id="symbol-236"></a>`interface NextValue<T>` | 自定义区间的有状态前进协议。
@note nextValue 从接收者的游标状态产生下一项并推进状态；到达终点后返回 None。 |
| 模块 | <a id="symbol-237"></a>`struct Pair<K, V>` | 包含 first 和 second 的两个值，分别使用 K 与 V 类型。 |
| Pair | <a id="symbol-238"></a>`K first` | 第一项，类型为 K。 |
| Pair | <a id="symbol-239"></a>`V second` | 第二项，类型为 V。 |
| 模块 | <a id="symbol-240"></a>`struct RawPtr<T>` | 有元素类型及分配身份的原始指针；复制指针不复制元素，也不延长存储寿命。 |
| RawPtr | <a id="symbol-241"></a>`uint64 storage` | 对应原始存储的分配身份，不延长存储寿命。 |
| RawPtr | <a id="symbol-242"></a>`int64 index` | 当前元素序号，从 0 开始。 |
| 模块 | <a id="symbol-243"></a>`struct BarePtr<T>: Add<BarePtr<T>, int64>, RightAdd<BarePtr<T>, int64>, Subtract<BarePtr<T>, int64>, Subtract<int64, BarePtr<T>>` | 只保存地址的裸指针；不记录运行时元素类型或初始化状态，也不拥有分配。 |
| BarePtr | <a id="symbol-244"></a>`int64 address` | 运行时逻辑字节地址；不是可传给宿主 C 函数的机器地址。 |
| 模块 | <a id="symbol-245"></a>`struct RawStorage<T>: Closeable` | 独占原始容量；备用槽位不默认构造 T，关闭时仅清理已初始化元素。 |
| RawStorage | <a id="symbol-246"></a>`uint64 handle` | 宿主资源句柄；应通过所属类型的方法操作。 |
| 模块 | <a id="symbol-247"></a>`interface Drop` | 显式释放拥有资源的析构协议。 |
| 模块 | <a id="symbol-248"></a>`interface Closeable` | 关闭外部资源的协议。 |
| 模块 | <a id="symbol-249"></a>`struct List<T>` | 可变长度的顺序值容器，支持按下标访问、追加、删除与复制。 |
| 模块 | <a id="symbol-250"></a>`struct Set<T>` | 保存唯一元素的值集合，支持存在检查和集合运算。 |
| 模块 | <a id="symbol-251"></a>`struct Map<K,V>` | 从唯一键到值的映射，支持键查询和按遍历位置读取。 |
| 模块 | <a id="symbol-252"></a>`struct Any` | 拥有一个装箱值的类型擦除容器；通过类型检查安全读取。 |
| 模块 | <a id="symbol-253"></a>`struct Math` | 核心数学常量和数值操作的静态入口。 |
| Math | <a id="symbol-254"></a>`static const float64 PI` | 圆周率常量。 |
| Math | <a id="symbol-255"></a>`static const float64 E` | 自然对数底数常量。 |
| 模块 | <a id="symbol-256"></a>`interface Add<R,T>` | 左操作数的加法重载接口。 |
| 模块 | <a id="symbol-257"></a>`interface RightAdd<R,T>` | 右操作数的加法重载接口。 |
| 模块 | <a id="symbol-258"></a>`interface Subtract<R,T>` | 左操作数的减法重载接口。 |
| 模块 | <a id="symbol-259"></a>`interface RightSubtract<R,T>` | 右操作数的减法重载接口。 |
| 模块 | <a id="symbol-260"></a>`interface Multiply<R,T>` | 左操作数的乘法重载接口。 |
| 模块 | <a id="symbol-261"></a>`interface RightMultiply<R,T>` | 右操作数的乘法重载接口。 |
| 模块 | <a id="symbol-262"></a>`interface Divide<R,T>` | 左操作数的除法重载接口。 |
| 模块 | <a id="symbol-263"></a>`interface RightDivide<R,T>` | 右操作数的除法重载接口。 |
| 模块 | <a id="symbol-264"></a>`interface Modulo<R,T>` | 左操作数的取模重载接口。 |
| 模块 | <a id="symbol-265"></a>`interface RightModulo<R,T>` | 右操作数的取模重载接口。 |
| 模块 | <a id="symbol-266"></a>`interface Equal<R,T>` | 左操作数的相等比较重载接口。 |
| 模块 | <a id="symbol-267"></a>`interface RightEqual<R,T>` | 右操作数的相等比较重载接口。 |
| 模块 | <a id="symbol-268"></a>`interface NotEqual<R,T>` | 左操作数的不等比较重载接口。 |
| 模块 | <a id="symbol-269"></a>`interface RightNotEqual<R,T>` | 右操作数的不等比较重载接口。 |
| 模块 | <a id="symbol-270"></a>`interface Less<R,T>` | 左操作数的小于比较重载接口。 |
| 模块 | <a id="symbol-271"></a>`interface RightLess<R,T>` | 右操作数的小于比较重载接口。 |
| 模块 | <a id="symbol-272"></a>`interface LessOrEqual<R,T>` | 左操作数的小于等于比较重载接口。 |
| 模块 | <a id="symbol-273"></a>`interface RightLessOrEqual<R,T>` | 右操作数的小于等于比较重载接口。 |
| 模块 | <a id="symbol-274"></a>`interface Greater<R,T>` | 左操作数的大于比较重载接口。 |
| 模块 | <a id="symbol-275"></a>`interface RightGreater<R,T>` | 右操作数的大于比较重载接口。 |
| 模块 | <a id="symbol-276"></a>`interface GreaterOrEqual<R,T>` | 左操作数的大于等于比较重载接口。 |
| 模块 | <a id="symbol-277"></a>`interface RightGreaterOrEqual<R,T>` | 右操作数的大于等于比较重载接口。 |
| 模块 | <a id="symbol-278"></a>`interface And<R,T>` | 左操作数的按位与重载接口。 |
| 模块 | <a id="symbol-279"></a>`interface RightAnd<R,T>` | 右操作数的按位与重载接口。 |
| 模块 | <a id="symbol-280"></a>`interface Or<R,T>` | 左操作数的按位或重载接口。 |
| 模块 | <a id="symbol-281"></a>`interface RightOr<R,T>` | 右操作数的按位或重载接口。 |
| 模块 | <a id="symbol-282"></a>`interface Xor<R,T>` | 左操作数的按位异或重载接口。 |
| 模块 | <a id="symbol-283"></a>`interface RightXor<R,T>` | 右操作数的按位异或重载接口。 |
| 模块 | <a id="symbol-284"></a>`interface ShiftLeft<R,T>` | 左操作数的左移重载接口。 |
| 模块 | <a id="symbol-285"></a>`interface RightShiftLeft<R,T>` | 右操作数的左移重载接口。 |
| 模块 | <a id="symbol-286"></a>`interface ShiftRight<R,T>` | 左操作数的右移重载接口。 |
| 模块 | <a id="symbol-287"></a>`interface RightShiftRight<R,T>` | 右操作数的右移重载接口。 |
| 模块 | <a id="symbol-288"></a>`interface Negate<R,T>` | 左操作数的取负重载接口。 |
| 模块 | <a id="symbol-289"></a>`interface BitNot<R,T>` | 左操作数的按位取反重载接口。 |
| 模块 | <a id="symbol-290"></a>`interface Positive<R,T>` | 左操作数的取正重载接口。 |
| 模块 | <a id="symbol-291"></a>`interface Hash<T>` | 为值提供哈希计算的接口。 |
| 模块 | <a id="symbol-292"></a>`interface Copy<T>` | 显式独立值复制协议，不改变赋值规则。 |
| 模块 | <a id="symbol-293"></a>`interface ExplicitConvert<T>` | 显式类型转换的接口。 |
| 模块 | <a id="symbol-294"></a>`interface ImplicitConvert<T>` | 隐式类型转换的接口。 |
| 模块 | <a id="symbol-295"></a>`interface Index<R,T>` | 自定义下标访问接口；与小写 index 连续存储函数区分。 |
| 模块 | <a id="symbol-296"></a>`interface Iterator<R>` | 供 for-in 获取和推进迭代状态的接口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [String::length](#api-0) | `int64 length()` |
| [String::size](#api-1) | `int64 size()` |
| [String::byteLength](#api-2) | `int64 byteLength()` |
| [String::isEmpty](#api-3) | `bool isEmpty()` |
| [String::isBlank](#api-4) | `bool isBlank()` |
| [String::isDigit](#api-5) | `bool isDigit()` |
| [String::isAlpha](#api-6) | `bool isAlpha()` |
| [String::isAlnum](#api-7) | `bool isAlnum()` |
| [String::isAscii](#api-8) | `bool isAscii()` |
| [String::charLength](#api-9) | `int64 charLength()` |
| [String::indexOf](#api-10) | `int64 indexOf(String text, int64 start = 0)` |
| [String::lastIndexOf](#api-11) | `int64 lastIndexOf(String text, int64 start = 0 - 1)` |
| [String::contains](#api-12) | `bool contains(String text)` |
| [String::startsWith](#api-13) | `bool startsWith(String text)` |
| [String::endsWith](#api-14) | `bool endsWith(String text)` |
| [String::count](#api-15) | `int64 count(String text)` |
| [String::compare](#api-16) | `int64 compare(String text)` |
| [String::byteAt](#api-17) | `Option<uint8> byteAt(int64 index)` |
| [String::substring](#api-18) | `String substring(int64 start, int64 count = 0 - 1)` |
| [String::slice](#api-19) | `String slice(int64 start, int64 end)` |
| [String::left](#api-20) | `String left(int64 count)` |
| [String::right](#api-21) | `String right(int64 count)` |
| [String::trim](#api-22) | `String trim()` |
| [String::trimStart](#api-23) | `String trimStart()` |
| [String::trimEnd](#api-24) | `String trimEnd()` |
| [String::toLower](#api-25) | `String toLower()` |
| [String::toUpper](#api-26) | `String toUpper()` |
| [String::replaceFirst](#api-27) | `String replaceFirst(String before, String after)` |
| [String::replaceAll](#api-28) | `String replaceAll(String before, String after)` |
| [String::replace](#api-29) | `String replace(String before, String after)` |
| [String::repeat](#api-30) | `String repeat(int64 count)` |
| [String::reverse](#api-31) | `String reverse()` |
| [String::padStart](#api-32) | `String padStart(int64 length, String fill = " ")` |
| [String::padEnd](#api-33) | `String padEnd(int64 length, String fill = " ")` |
| [String::removePrefix](#api-34) | `String removePrefix(String prefix)` |
| [String::removeSuffix](#api-35) | `String removeSuffix(String suffix)` |
| [String::insert](#api-36) | `String insert(int64 index, String text)` |
| [String::remove](#api-37) | `String remove(int64 start, int64 count = 0 - 1)` |
| [String::utf8Substring](#api-38) | `String utf8Substring(int64 start, int64 count = 0 - 1)` |
| [String::charAt](#api-39) | `Option<String> charAt(int64 index)` |
| [String::split](#api-40) | `List<String> split(String separator, bool keepEmpty = true)` |
| [String::splitWhitespace](#api-41) | `List<String> splitWhitespace()` |
| [String::lines](#api-42) | `List<String> lines(bool keepEmpty = true)` |
| [String::unicodeChars](#api-43) | `List<String> unicodeChars()` |
| [String::bytes](#api-44) | `List<uint8> bytes()` |
| [String::join](#api-45) | `String join(List<String> items)` |
| [String::concat](#api-46) | `String concat(String text)` |
| [String::prepend](#api-47) | `String prepend(String text)` |
| [Error::toString](#api-48) | `String toString()` |
| [MessageError::toString](#api-49) | `String toString()` |
| [MessageError::from](#api-50) | `static MessageError from(String text)` |
| [File::print](#api-51) | `int64 print(String text)` |
| [File::println](#api-52) | `int64 println(String text = "")` |
| [readLine](#api-53) | `String readLine()` |
| [readLines](#api-54) | `String readLines(int32 N)` |
| [readLines](#api-55) | `String readLines(int64 N)` |
| [toInt32](#api-56) | `Option<int32> toInt32(float64 value)` |
| [toInt32](#api-57) | `Option<int32> toInt32(float32 value)` |
| [toInt64](#api-58) | `int64 toInt64(float64 value)` |
| [toInt64](#api-59) | `int64 toInt64(float32 value)` |
| [float32ToBytes](#api-60) | `std::lang::List<uint8> float32ToBytes(float32 value, bool bigEndian)` |
| [float32ToBytes](#api-61) | `std::lang::List<uint8> float32ToBytes(float32 value)` |
| [bytesToFloat32](#api-62) | `Option<float32> bytesToFloat32(std::lang::List<uint8> bytes, bool bigEndian)` |
| [bytesToFloat32](#api-63) | `Option<float32> bytesToFloat32(std::lang::List<uint8> bytes)` |
| [bytesToFloat32](#api-64) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count, bool bigEndian)` |
| [bytesToFloat32](#api-65) | `Option<float32> bytesToFloat32(uint8 const& bytes, int64 count)` |
| [bytesToFloat32](#api-66) | `Option<float32> bytesToFloat32(uint8 bytes[], bool bigEndian)` |
| [bytesToFloat32](#api-67) | `Option<float32> bytesToFloat32(uint8 bytes[])` |
| [float32ToBytes](#api-68) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [float32ToBytes](#api-69) | `bool float32ToBytes(float32 value, uint8 mut& bytes, int64 count)` |
| [float32ToBytes](#api-70) | `bool float32ToBytes(float32 value, uint8[] mut& bytes, bool bigEndian)` |
| [float32ToBytes](#api-71) | `bool float32ToBytes(float32 value, uint8[] mut& bytes)` |
| [float64ToBytes](#api-72) | `std::lang::List<uint8> float64ToBytes(float64 value, bool bigEndian)` |
| [float64ToBytes](#api-73) | `std::lang::List<uint8> float64ToBytes(float64 value)` |
| [bytesToFloat64](#api-74) | `Option<float64> bytesToFloat64(std::lang::List<uint8> bytes, bool bigEndian)` |
| [bytesToFloat64](#api-75) | `Option<float64> bytesToFloat64(std::lang::List<uint8> bytes)` |
| [bytesToFloat64](#api-76) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count, bool bigEndian)` |
| [bytesToFloat64](#api-77) | `Option<float64> bytesToFloat64(uint8 const& bytes, int64 count)` |
| [bytesToFloat64](#api-78) | `Option<float64> bytesToFloat64(uint8 bytes[], bool bigEndian)` |
| [bytesToFloat64](#api-79) | `Option<float64> bytesToFloat64(uint8 bytes[])` |
| [float64ToBytes](#api-80) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count, bool bigEndian)` |
| [float64ToBytes](#api-81) | `bool float64ToBytes(float64 value, uint8 mut& bytes, int64 count)` |
| [float64ToBytes](#api-82) | `bool float64ToBytes(float64 value, uint8[] mut& bytes, bool bigEndian)` |
| [float64ToBytes](#api-83) | `bool float64ToBytes(float64 value, uint8[] mut& bytes)` |
| [assert](#api-84) | `void assert(bool condition)` |
| [assert](#api-85) | `void assert(bool condition, std::lang::String message)` |
| [assert](#api-86) | `void assert(bool condition, int64 line)` |
| [assert](#api-87) | `void assert(bool condition, std::lang::String message, int64 line)` |
| [Try::branch](#api-88) | `TryBranch<T, R> branch()` |
| [TryMove::intoBranch](#api-89) | `TryBranch<T, R> intoBranch()` |
| [FromResidual::fromResidual](#api-90) | `static F fromResidual(R residual)` |
| [Option::branch](#api-91) | `TryBranch<T, NoneResidual> branch()` |
| [Option::intoBranch](#api-92) | `TryBranch<T, NoneResidual> intoBranch()` |
| [Option::fromResidual](#api-93) | `static Option<T> fromResidual(NoneResidual residual)` |
| [Option::isValue](#api-94) | `bool isValue()` |
| [Option::isNone](#api-95) | `bool isNone()` |
| [Option::take](#api-96) | `T take()` |
| [Option::unwrap](#api-97) | `T unwrap()` |
| [Option::unwrapOr](#api-98) | `T unwrapOr(T fallback)` |
| [Option::expect](#api-99) | `T expect(std::lang::String message)` |
| [Option::transform](#api-100) | `Option<T> transform(T fn(T) mapper)` |
| [Option::andThen](#api-101) | `Option<T> andThen(Option<T> fn(T) mapper)` |
| [Result::branch](#api-102) | `TryBranch<T, ErrorResidual<E>> branch()` |
| [Result::intoBranch](#api-103) | `TryBranch<T, ErrorResidual<E>> intoBranch()` |
| [Result::fromResidual](#api-104) | `static Result<T,E> fromResidual(ErrorResidual<E> residual)` |
| [Result::ok](#api-105) | `Option<T> ok()` |
| [Result::isOk](#api-106) | `bool isOk()` |
| [Result::isErr](#api-107) | `bool isErr()` |
| [Result::take](#api-108) | `T take()` |
| [Result::unwrap](#api-109) | `T unwrap()` |
| [Result::unwrapOr](#api-110) | `T unwrapOr(T fallback)` |
| [Result::expect](#api-111) | `T expect(std::lang::String message)` |
| [Result::takeError](#api-112) | `E takeError()` |
| [Result::error](#api-113) | `E error()` |
| [Result::transform](#api-114) | `Result<T, E> transform(T fn(T) mapper)` |
| [Result::andThen](#api-115) | `Result<T, E> andThen(Result<T, E> fn(T) mapper)` |
| [NextValue::nextValue](#api-116) | `Option<T> nextValue(T end, bool inclusive)` |
| [Pair::create](#api-117) | `static Pair<K, V> create(K first, V second)` |
| [Pair::create](#api-118) | `static Pair<K, V> create((K, V) value)` |
| [Pair::toTuple](#api-119) | `(K, V) toTuple()` |
| [tryGet](#api-120) | `Option<T> tryGet<T>(Any const& value)` |
| [tryGetReference](#api-121) | `Option<T const&> tryGetReference<T>(Any const& value)` |
| [tryGetMut](#api-122) | `Option<T mut&> tryGetMut<T>(Any mut& value)` |
| [stringBytes](#api-123) | `List<uint8> stringBytes(String text)` |
| [BarePtr::leftAdd](#api-124) | `BarePtr<T> leftAdd(int64 elements)` |
| [BarePtr::rightAdd](#api-125) | `BarePtr<T> rightAdd(int64 elements)` |
| [BarePtr::leftSub](#api-126) | `BarePtr<T> leftSub(int64 elements)` |
| [BarePtr::leftSub](#api-127) | `int64 leftSub(BarePtr<T> other)` |
| [barePointer](#api-128) | `unsafe BarePtr<T> barePointer<T>(RawPtr<T> pointer)` |
| [pointerFromAddress](#api-129) | `unsafe BarePtr<T> pointerFromAddress<T>(int64 address)` |
| [dereference](#api-130) | `unsafe T mut& dereference<T>(BarePtr<T> pointer)` |
| [memcpy](#api-131) | `unsafe void memcpy<T,U>(BarePtr<T> destination, BarePtr<U> source, int64 bytes)` |
| [memset](#api-132) | `unsafe void memset<T>(BarePtr<T> destination, uint8 value, int64 bytes)` |
| [initAt](#api-133) | `unsafe void initAt<T>(RawPtr<T> pointer, T value)` |
| [readAt](#api-134) | `unsafe T readAt<T>(RawPtr<T> pointer)` |
| [borrowAt](#api-135) | `unsafe T mut& borrowAt<T>(RawPtr<T> pointer)` |
| [takeAt](#api-136) | `unsafe T takeAt<T>(RawPtr<T> pointer)` |
| [dropAt](#api-137) | `unsafe void dropAt<T>(RawPtr<T> pointer)` |
| [relocateNonOverlapping](#api-138) | `unsafe void relocateNonOverlapping<T>(RawPtr<T> destination, RawPtr<T> source, int64 count)` |
| [RawStorage::allocate](#api-139) | `static RawStorage<T> allocate(int64 capacity)` |
| [RawStorage::capacity](#api-140) | `int64 capacity()` |
| [RawStorage::pointer](#api-141) | `RawPtr<T> pointer(int64 index)` |
| [RawStorage::isInitialized](#api-142) | `bool isInitialized(int64 index)` |
| [RawStorage::close](#api-143) | `void close()` |
| [range](#api-144) | `__Range range(int64 start, int64 stop)` |
| [range](#api-145) | `__Range range(int64 start, int64 stop, int64 step)` |
| [rangeEq](#api-146) | `__Interval_int64 rangeEq(int64 start, int64 stop)` |
| [Drop::drop](#api-147) | `void drop()` |
| [Closeable::close](#api-148) | `void close()` |
| [print](#api-149) | `int64 print(String format, Any args...)` |
| [println](#api-150) | `int64 println(String format, Any args...)` |
| [eprint](#api-151) | `int64 eprint(String format, Any args...)` |
| [eprintln](#api-152) | `int64 eprintln(String format, Any args...)` |
| [format](#api-153) | `String format(String format, Any args...)` |
| [putChar](#api-154) | `int64 putChar(char value)` |
| [pause](#api-155) | `int64 pause()` |
| [inputLine](#api-156) | `String inputLine()` |
| [inputInt](#api-157) | `int64 inputInt()` |
| [inputUInt](#api-158) | `uint64 inputUInt()` |
| [inputFloat](#api-159) | `float64 inputFloat()` |
| [inputBool](#api-160) | `bool inputBool()` |
| [inputChar](#api-161) | `char inputChar()` |
| [inputBigInteger](#api-162) | `BigInteger inputBigInteger()` |
| [inputBigDecimal](#api-163) | `BigDecimal inputBigDecimal()` |
| [panic](#api-164) | `int64 panic(String message)` |
| [len](#api-165) | `int64 len(Any value)` |
| [toReference](#api-166) | `T mut& toReference<T>(Option<T mut&> value)` |
| [toOptional](#api-167) | `Option<T mut&> toOptional<T>(T mut& value)` |
| [memcpy](#api-168) | `int64 memcpy(char mut& destination, char const& source, int64 bytes)` |
| [memset](#api-169) | `int64 memset(char mut& destination, int64 value, int64 bytes)` |
| [List::copy](#api-170) | `List<T> copy()` |
| [List::push](#api-171) | `int64 push(T value)` |
| [List::insert](#api-172) | `int64 insert(int64 index, T value)` |
| [List::at](#api-173) | `T at(int64 index)` |
| [List::pop](#api-174) | `T pop()` |
| [List::remove](#api-175) | `int64 remove(int64 index)` |
| [List::indexOf](#api-176) | `int64 indexOf(T value)` |
| [List::contains](#api-177) | `bool contains(T value)` |
| [List::has](#api-178) | `bool has(T value)` |
| [List::clear](#api-179) | `int64 clear()` |
| [List::len](#api-180) | `int64 len()` |
| [Set::copy](#api-181) | `Set<T> copy()` |
| [Set::add](#api-182) | `int64 add(T value)` |
| [Set::has](#api-183) | `int64 has(T value)` |
| [Set::contains](#api-184) | `int64 contains(T value)` |
| [Set::remove](#api-185) | `int64 remove(T value)` |
| [Set::at](#api-186) | `T at(int64 index)` |
| [Set::clear](#api-187) | `int64 clear()` |
| [Set::len](#api-188) | `int64 len()` |
| [Map::copy](#api-189) | `Map<K,V> copy()` |
| [Map::put](#api-190) | `int64 put(K key,V value)` |
| [Map::has](#api-191) | `int64 has(K key)` |
| [Map::contains](#api-192) | `int64 contains(K key)` |
| [Map::remove](#api-193) | `int64 remove(K key)` |
| [Map::keyAt](#api-194) | `K keyAt(int64 index)` |
| [Map::valAt](#api-195) | `V valAt(int64 index)` |
| [Map::clear](#api-196) | `int64 clear()` |
| [Map::len](#api-197) | `int64 len()` |
| [BigInteger::str](#api-198) | `String str()` |
| [BigInteger::copy](#api-199) | `BigInteger copy()` |
| [BigInteger::pow](#api-200) | `BigInteger pow(int64 exponent)` |
| [BigInteger::powmod](#api-201) | `BigInteger powmod(BigInteger exponent, BigInteger modulus)` |
| [BigInteger::gcd](#api-202) | `BigInteger gcd(BigInteger other)` |
| [BigInteger::lcm](#api-203) | `BigInteger lcm(BigInteger other)` |
| [BigInteger::mod](#api-204) | `BigInteger mod(BigInteger modulus)` |
| [BigInteger::modInverse](#api-205) | `BigInteger modInverse(BigInteger modulus)` |
| [BigInteger::isProbablePrime](#api-206) | `bool isProbablePrime(int64 rounds)` |
| [BigInteger::nextProbablePrime](#api-207) | `BigInteger nextProbablePrime()` |
| [BigInteger::bitLength](#api-208) | `int64 bitLength()` |
| [BigInteger::bitAnd](#api-209) | `BigInteger bitAnd(BigInteger other)` |
| [BigInteger::bitOr](#api-210) | `BigInteger bitOr(BigInteger other)` |
| [BigInteger::bitXor](#api-211) | `BigInteger bitXor(BigInteger other)` |
| [BigInteger::strRadix](#api-212) | `String strRadix(int64 base)` |
| [BigInteger::iroot](#api-213) | `BigInteger iroot(int64 k)` |
| [BigDecimal::str](#api-214) | `String str()` |
| [BigDecimal::copy](#api-215) | `BigDecimal copy()` |
| [BigDecimal::setPrecision](#api-216) | `int64 setPrecision(int64 digits)` |
| [BigDecimal::pow](#api-217) | `BigDecimal pow(int64 exponent)` |
| [BigDecimal::abs](#api-218) | `BigDecimal abs()` |
| [BigDecimal::isInteger](#api-219) | `bool isInteger()` |
| [BigDecimal::floor](#api-220) | `BigInteger floor()` |
| [BigDecimal::ceil](#api-221) | `BigInteger ceil()` |
| [BigDecimal::trunc](#api-222) | `BigInteger trunc()` |
| [BigDecimal::sqrt](#api-223) | `BigDecimal sqrt(int64 digits)` |
| [BigDecimal::root](#api-224) | `BigDecimal root(int64 k, int64 digits)` |
| [Any::copy](#api-225) | `Any copy()` |
| [Any::isEmpty](#api-226) | `bool isEmpty()` |
| [Any::clear](#api-227) | `void clear()` |
| [Any::set](#api-228) | `int64 set(T value)` |
| [Any::getValue](#api-229) | `T getValue()` |
| [Any::getReference](#api-230) | `T mut& getReference()` |
| [Any::getOptional](#api-231) | `Option<T mut&> getOptional()` |
| [eq](#api-232) | `bool eq(T left,T right)` |
| [ne](#api-233) | `bool ne(T left,T right)` |
| [lt](#api-234) | `bool lt(T left,T right)` |
| [le](#api-235) | `bool le(T left,T right)` |
| [gt](#api-236) | `bool gt(T left,T right)` |
| [ge](#api-237) | `bool ge(T left,T right)` |
| [add](#api-238) | `R add(T left,U right)` |
| [sub](#api-239) | `R sub(T left,U right)` |
| [mul](#api-240) | `R mul(T left,U right)` |
| [div](#api-241) | `R div(T left,U right)` |
| [mod](#api-242) | `R mod(T left,U right)` |
| [index](#api-243) | `T index(T const& data,int64 index)` |
| [Add::leftAdd](#api-244) | `R leftAdd(T value)` |
| [RightAdd::rightAdd](#api-245) | `R rightAdd(T value)` |
| [Subtract::leftSub](#api-246) | `R leftSub(T value)` |
| [RightSubtract::rightSub](#api-247) | `R rightSub(T value)` |
| [Multiply::leftMul](#api-248) | `R leftMul(T value)` |
| [RightMultiply::rightMul](#api-249) | `R rightMul(T value)` |
| [Divide::leftDiv](#api-250) | `R leftDiv(T value)` |
| [RightDivide::rightDiv](#api-251) | `R rightDiv(T value)` |
| [Modulo::leftMod](#api-252) | `R leftMod(T value)` |
| [RightModulo::rightMod](#api-253) | `R rightMod(T value)` |
| [Equal::leftEqual](#api-254) | `R leftEqual(T value)` |
| [RightEqual::rightEqual](#api-255) | `R rightEqual(T value)` |
| [NotEqual::leftNotEqual](#api-256) | `R leftNotEqual(T value)` |
| [RightNotEqual::rightNotEqual](#api-257) | `R rightNotEqual(T value)` |
| [Less::leftLess](#api-258) | `R leftLess(T value)` |
| [RightLess::rightLess](#api-259) | `R rightLess(T value)` |
| [LessOrEqual::leftLessEqual](#api-260) | `R leftLessEqual(T value)` |
| [RightLessOrEqual::rightLessEqual](#api-261) | `R rightLessEqual(T value)` |
| [Greater::leftGreater](#api-262) | `R leftGreater(T value)` |
| [RightGreater::rightGreater](#api-263) | `R rightGreater(T value)` |
| [GreaterOrEqual::leftGreaterEqual](#api-264) | `R leftGreaterEqual(T value)` |
| [RightGreaterOrEqual::rightGreaterEqual](#api-265) | `R rightGreaterEqual(T value)` |
| [And::leftBitAnd](#api-266) | `R leftBitAnd(T value)` |
| [RightAnd::rightBitAnd](#api-267) | `R rightBitAnd(T value)` |
| [Or::leftBitOr](#api-268) | `R leftBitOr(T value)` |
| [RightOr::rightBitOr](#api-269) | `R rightBitOr(T value)` |
| [Xor::leftBitXor](#api-270) | `R leftBitXor(T value)` |
| [RightXor::rightBitXor](#api-271) | `R rightBitXor(T value)` |
| [ShiftLeft::leftShiftLeft](#api-272) | `R leftShiftLeft(T value)` |
| [RightShiftLeft::rightShiftLeft](#api-273) | `R rightShiftLeft(T value)` |
| [ShiftRight::leftShiftRight](#api-274) | `R leftShiftRight(T value)` |
| [RightShiftRight::rightShiftRight](#api-275) | `R rightShiftRight(T value)` |
| [Negate::negate](#api-276) | `R negate(T value)` |
| [BitNot::bitNot](#api-277) | `R bitNot(T value)` |
| [Positive::positive](#api-278) | `R positive(T value)` |
| [Hash::hash](#api-279) | `uint64 hash(T value)` |
| [Copy::copy](#api-280) | `T copy()` |
| [ExplicitConvert::explicitConvert](#api-281) | `T explicitConvert()` |
| [ImplicitConvert::implicitConvert](#api-282) | `T implicitConvert()` |
| [Index::index](#api-283) | `R index(T key)` |
| [Iterator::iterator](#api-284) | `Option<R> iterator(int64 index)` |
| [Result::mapErr](#api-285) | `Result<T,F> mapErr<F>(F fn(E) mapper)` |
| [Option::okOr](#api-286) | `Result<T,E> okOr<E>(E error)` |
| [Option::okOrElse](#api-287) | `Result<T,E> okOrElse<E>(E fn() factory)` |
| [BarePtr::castTo](#api-288) | `unsafe BarePtr<U> castTo<U>()` |

<a id="api-0"></a>

### String::length（1）

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

**源码**：[source/std/lang.chl:408](../../source/std/lang.chl#L408)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-1"></a>

### String::size（2）

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

**源码**：[source/std/lang.chl:410](../../source/std/lang.chl#L410)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-2"></a>

### String::byteLength（3）

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

**源码**：[source/std/lang.chl:413](../../source/std/lang.chl#L413)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-3"></a>

### String::isEmpty（4）

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

**源码**：[source/std/lang.chl:416](../../source/std/lang.chl#L416)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-4"></a>

### String::isBlank（5）

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

**源码**：[source/std/lang.chl:419](../../source/std/lang.chl#L419)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-5"></a>

### String::isDigit（6）

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

**源码**：[source/std/lang.chl:422](../../source/std/lang.chl#L422)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-6"></a>

### String::isAlpha（7）

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

**源码**：[source/std/lang.chl:425](../../source/std/lang.chl#L425)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-7"></a>

### String::isAlnum（8）

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

**源码**：[source/std/lang.chl:428](../../source/std/lang.chl#L428)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-8"></a>

### String::isAscii（9）

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

**源码**：[source/std/lang.chl:431](../../source/std/lang.chl#L431)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-9"></a>

### String::charLength（10）

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

**源码**：[source/std/lang.chl:434](../../source/std/lang.chl#L434)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-10"></a>

### String::indexOf（11）

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

**源码**：[source/std/lang.chl:437](../../source/std/lang.chl#L437)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-11"></a>

### String::lastIndexOf（12）

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

**源码**：[source/std/lang.chl:440](../../source/std/lang.chl#L440)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-12"></a>

### String::contains（13）

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

**源码**：[source/std/lang.chl:443](../../source/std/lang.chl#L443)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-13"></a>

### String::startsWith（14）

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

**源码**：[source/std/lang.chl:446](../../source/std/lang.chl#L446)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-14"></a>

### String::endsWith（15）

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

**源码**：[source/std/lang.chl:449](../../source/std/lang.chl#L449)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-15"></a>

### String::count（16）

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

**源码**：[source/std/lang.chl:452](../../source/std/lang.chl#L452)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-16"></a>

### String::compare（17）

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

**源码**：[source/std/lang.chl:455](../../source/std/lang.chl#L455)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-17"></a>

### String::byteAt（18）

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

**源码**：[source/std/lang.chl:458](../../source/std/lang.chl#L458)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-18"></a>

### String::substring（19）

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

**源码**：[source/std/lang.chl:461](../../source/std/lang.chl#L461)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-19"></a>

### String::slice（20）

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

**源码**：[source/std/lang.chl:464](../../source/std/lang.chl#L464)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-20"></a>

### String::left（21）

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

**源码**：[source/std/lang.chl:467](../../source/std/lang.chl#L467)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-21"></a>

### String::right（22）

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

**源码**：[source/std/lang.chl:470](../../source/std/lang.chl#L470)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-22"></a>

### String::trim（23）

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

**源码**：[source/std/lang.chl:473](../../source/std/lang.chl#L473)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-23"></a>

### String::trimStart（24）

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

**源码**：[source/std/lang.chl:476](../../source/std/lang.chl#L476)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-24"></a>

### String::trimEnd（25）

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

**源码**：[source/std/lang.chl:479](../../source/std/lang.chl#L479)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-25"></a>

### String::toLower（26）

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

**源码**：[source/std/lang.chl:482](../../source/std/lang.chl#L482)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-26"></a>

### String::toUpper（27）

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

**源码**：[source/std/lang.chl:485](../../source/std/lang.chl#L485)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-27"></a>

### String::replaceFirst（28）

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

**源码**：[source/std/lang.chl:488](../../source/std/lang.chl#L488)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-28"></a>

### String::replaceAll（29）

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

**源码**：[source/std/lang.chl:491](../../source/std/lang.chl#L491)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-29"></a>

### String::replace（30）

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

**源码**：[source/std/lang.chl:494](../../source/std/lang.chl#L494)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-30"></a>

### String::repeat（31）

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

**源码**：[source/std/lang.chl:497](../../source/std/lang.chl#L497)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-31"></a>

### String::reverse（32）

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

**源码**：[source/std/lang.chl:500](../../source/std/lang.chl#L500)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-32"></a>

### String::padStart（33）

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

**源码**：[source/std/lang.chl:503](../../source/std/lang.chl#L503)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-33"></a>

### String::padEnd（34）

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

**源码**：[source/std/lang.chl:506](../../source/std/lang.chl#L506)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-34"></a>

### String::removePrefix（35）

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

**源码**：[source/std/lang.chl:509](../../source/std/lang.chl#L509)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-35"></a>

### String::removeSuffix（36）

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

**源码**：[source/std/lang.chl:512](../../source/std/lang.chl#L512)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-36"></a>

### String::insert（37）

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

**源码**：[source/std/lang.chl:515](../../source/std/lang.chl#L515)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-37"></a>

### String::remove（38）

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

**源码**：[source/std/lang.chl:518](../../source/std/lang.chl#L518)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-38"></a>

### String::utf8Substring（39）

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

**源码**：[source/std/lang.chl:521](../../source/std/lang.chl#L521)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-39"></a>

### String::charAt（40）

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

**源码**：[source/std/lang.chl:524](../../source/std/lang.chl#L524)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-40"></a>

### String::split（41）

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

**源码**：[source/std/lang.chl:527](../../source/std/lang.chl#L527)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-41"></a>

### String::splitWhitespace（42）

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

**源码**：[source/std/lang.chl:530](../../source/std/lang.chl#L530)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-42"></a>

### String::lines（43）

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

**源码**：[source/std/lang.chl:533](../../source/std/lang.chl#L533)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-43"></a>

### String::unicodeChars（44）

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

**源码**：[source/std/lang.chl:536](../../source/std/lang.chl#L536)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-44"></a>

### String::bytes（45）

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

**源码**：[source/std/lang.chl:539](../../source/std/lang.chl#L539)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-45"></a>

### String::join（46）

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

**源码**：[source/std/lang.chl:542](../../source/std/lang.chl#L542)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-46"></a>

### String::concat（47）

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

**源码**：[source/std/lang.chl:545](../../source/std/lang.chl#L545)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-47"></a>

### String::prepend（48）

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

**源码**：[source/std/lang.chl:548](../../source/std/lang.chl#L548)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-48"></a>

### Error::toString（49）

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

**源码**：[source/std/lang.chl:554](../../source/std/lang.chl#L554)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-49"></a>

### MessageError::toString（50）

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

**源码**：[source/std/lang.chl:562](../../source/std/lang.chl#L562)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-50"></a>

### MessageError::from（51）

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

**源码**：[source/std/lang.chl:564](../../source/std/lang.chl#L564)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-51"></a>

### File::print（52）

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

**源码**：[source/std/lang.chl:590](../../source/std/lang.chl#L590)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-52"></a>

### File::println（53）

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

**源码**：[source/std/lang.chl:596](../../source/std/lang.chl#L596)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-53"></a>

### readLine（54）

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

**源码**：[source/std/lang.chl:629](../../source/std/lang.chl#L629)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-54"></a>

### readLines（55）

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

**调用形式**

```chali
readLines(N);
```

**源码**：[source/std/lang.chl:633](../../source/std/lang.chl#L633)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-55"></a>

### readLines（56）

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

**调用形式**

```chali
readLines(N);
```

**源码**：[source/std/lang.chl:637](../../source/std/lang.chl#L637)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-56"></a>

### toInt32（57）

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

**源码**：[source/std/lang.chl:641](../../source/std/lang.chl#L641)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-57"></a>

### toInt32（58）

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

**源码**：[source/std/lang.chl:647](../../source/std/lang.chl#L647)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-58"></a>

### toInt64（59）

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

**源码**：[source/std/lang.chl:649](../../source/std/lang.chl#L649)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-59"></a>

### toInt64（60）

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

**源码**：[source/std/lang.chl:651](../../source/std/lang.chl#L651)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-60"></a>

### float32ToBytes（61）

**声明**

```chali
std::lang::List<uint8> float32ToBytes(float32 value, bool bigEndian);
```

**行为**

按 IEEE 754 binary32 输出 4 个字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`std::lang::List<uint8>`。按 IEEE 754 binary32 输出 4 个字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value, bigEndian);
```

**源码**：[source/std/lang.chl:654](../../source/std/lang.chl#L654)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-61"></a>

### float32ToBytes（62）

**声明**

```chali
std::lang::List<uint8> float32ToBytes(float32 value);
```

**行为**

默认小端编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float32 value` | 输入值或写入的新值 | 无 |

**返回值**

`std::lang::List<uint8>`。默认小端编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float32ToBytes(value);
```

**源码**：[source/std/lang.chl:656](../../source/std/lang.chl#L656)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-62"></a>

### bytesToFloat32（63）

**声明**

```chali
Option<float32> bytesToFloat32(std::lang::List<uint8> bytes, bool bigEndian);
```

**行为**

读取列表前 4 字节，不足则返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `std::lang::List<uint8> bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float32>`。读取列表前 4 字节，不足则返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes, bigEndian);
```

**源码**：[source/std/lang.chl:658](../../source/std/lang.chl#L658)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-63"></a>

### bytesToFloat32（64）

**声明**

```chali
Option<float32> bytesToFloat32(std::lang::List<uint8> bytes);
```

**行为**

默认小端列表解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `std::lang::List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float32>`。默认小端列表解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat32(bytes);
```

**源码**：[source/std/lang.chl:663](../../source/std/lang.chl#L663)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-64"></a>

### bytesToFloat32（65）

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

**源码**：[source/std/lang.chl:665](../../source/std/lang.chl#L665)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-65"></a>

### bytesToFloat32（66）

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

**源码**：[source/std/lang.chl:670](../../source/std/lang.chl#L670)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-66"></a>

### bytesToFloat32（67）

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

**源码**：[source/std/lang.chl:672](../../source/std/lang.chl#L672)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-67"></a>

### bytesToFloat32（68）

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

**源码**：[source/std/lang.chl:674](../../source/std/lang.chl#L674)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-68"></a>

### float32ToBytes（69）

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

**源码**：[source/std/lang.chl:676](../../source/std/lang.chl#L676)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-69"></a>

### float32ToBytes（70）

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

**源码**：[source/std/lang.chl:681](../../source/std/lang.chl#L681)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-70"></a>

### float32ToBytes（71）

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

**源码**：[source/std/lang.chl:683](../../source/std/lang.chl#L683)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-71"></a>

### float32ToBytes（72）

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

**源码**：[source/std/lang.chl:688](../../source/std/lang.chl#L688)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-72"></a>

### float64ToBytes（73）

**声明**

```chali
std::lang::List<uint8> float64ToBytes(float64 value, bool bigEndian);
```

**行为**

按 IEEE 754 binary64 输出 8 个字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`std::lang::List<uint8>`。按 IEEE 754 binary64 输出 8 个字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value, bigEndian);
```

**源码**：[source/std/lang.chl:691](../../source/std/lang.chl#L691)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-73"></a>

### float64ToBytes（74）

**声明**

```chali
std::lang::List<uint8> float64ToBytes(float64 value);
```

**行为**

默认小端编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |

**返回值**

`std::lang::List<uint8>`。默认小端编码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
float64ToBytes(value);
```

**源码**：[source/std/lang.chl:693](../../source/std/lang.chl#L693)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-74"></a>

### bytesToFloat64（75）

**声明**

```chali
Option<float64> bytesToFloat64(std::lang::List<uint8> bytes, bool bigEndian);
```

**行为**

读取列表前 8 字节，不足则返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `std::lang::List<uint8> bytes` | 输入或输出字节存储 | 无 |
| `bigEndian` | `bool bigEndian` | true 为大端，false 为小端 | 无 |

**返回值**

`Option<float64>`。读取列表前 8 字节，不足则返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes, bigEndian);
```

**源码**：[source/std/lang.chl:695](../../source/std/lang.chl#L695)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-75"></a>

### bytesToFloat64（76）

**声明**

```chali
Option<float64> bytesToFloat64(std::lang::List<uint8> bytes);
```

**行为**

默认小端列表解码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `std::lang::List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Option<float64>`。默认小端列表解码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
bytesToFloat64(bytes);
```

**源码**：[source/std/lang.chl:700](../../source/std/lang.chl#L700)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-76"></a>

### bytesToFloat64（77）

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

**源码**：[source/std/lang.chl:702](../../source/std/lang.chl#L702)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-77"></a>

### bytesToFloat64（78）

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

**源码**：[source/std/lang.chl:707](../../source/std/lang.chl#L707)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-78"></a>

### bytesToFloat64（79）

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

**源码**：[source/std/lang.chl:709](../../source/std/lang.chl#L709)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-79"></a>

### bytesToFloat64（80）

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

**源码**：[source/std/lang.chl:711](../../source/std/lang.chl#L711)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-80"></a>

### float64ToBytes（81）

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

**源码**：[source/std/lang.chl:713](../../source/std/lang.chl#L713)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-81"></a>

### float64ToBytes（82）

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

**源码**：[source/std/lang.chl:718](../../source/std/lang.chl#L718)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-82"></a>

### float64ToBytes（83）

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

**源码**：[source/std/lang.chl:720](../../source/std/lang.chl#L720)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-83"></a>

### float64ToBytes（84）

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

**源码**：[source/std/lang.chl:725](../../source/std/lang.chl#L725)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-84"></a>

### assert（85）

**声明**

```chali
void assert(bool condition);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

中止统一使用 std::lang::panic，编译器与运行时通过 `__std_panic` 执行停机。 断言使用下面的标准库函数，它们在条件失败时调用 `std::lang::panic`。 带消息的重载让标准库自己的前置检查 也能给出人话（例如 range 的步长）。 带 `int64 line` 的重载给文件末尾的系统宏 `$assert(...)` 用：宏把调用点的 `$line` 传进来，失败时就地报出**源码行号**，不必每处自己写文案。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition);
```

**源码**：[source/std/lang.chl:738](../../source/std/lang.chl#L738)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-85"></a>

### assert（86）

**声明**

```chali
void assert(bool condition, std::lang::String message);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |
| `message` | `std::lang::String message` | 诊断文本或消息载荷 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
assert(condition, message);
```

**源码**：[source/std/lang.chl:742](../../source/std/lang.chl#L742)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-86"></a>

### assert（87）

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

**源码**：[source/std/lang.chl:746](../../source/std/lang.chl#L746)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-87"></a>

### assert（88）

**声明**

```chali
void assert(bool condition, std::lang::String message, int64 line);
```

**行为**

condition 为真时继续，为假时以指定诊断文本和可选行号触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `condition` | `bool condition` | 待验证的布尔条件 | 无 |
| `message` | `std::lang::String message` | 诊断文本或消息载荷 | 无 |
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

**源码**：[source/std/lang.chl:750](../../source/std/lang.chl#L750)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-88"></a>

### Try::branch（89）

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

**源码**：[source/std/lang.chl:761](../../source/std/lang.chl#L761)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-89"></a>

### TryMove::intoBranch（90）

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

**源码**：[source/std/lang.chl:763](../../source/std/lang.chl#L763)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-90"></a>

### FromResidual::fromResidual（91）

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

**源码**：[source/std/lang.chl:765](../../source/std/lang.chl#L765)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-91"></a>

### Option::branch（92）

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

**源码**：[source/std/lang.chl:770](../../source/std/lang.chl#L770)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-92"></a>

### Option::intoBranch（93）

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

**源码**：[source/std/lang.chl:776](../../source/std/lang.chl#L776)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-93"></a>

### Option::fromResidual（94）

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

**源码**：[source/std/lang.chl:780](../../source/std/lang.chl#L780)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-94"></a>

### Option::isValue（95）

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

**源码**：[source/std/lang.chl:781](../../source/std/lang.chl#L781)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-95"></a>

### Option::isNone（96）

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

**源码**：[source/std/lang.chl:790](../../source/std/lang.chl#L790)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-96"></a>

### Option::take（97）

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

**源码**：[source/std/lang.chl:793](../../source/std/lang.chl#L793)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-97"></a>

### Option::unwrap（98）

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

**源码**：[source/std/lang.chl:803](../../source/std/lang.chl#L803)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-98"></a>

### Option::unwrapOr（99）

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

**源码**：[source/std/lang.chl:812](../../source/std/lang.chl#L812)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-99"></a>

### Option::expect（100）

**声明**

```chali
T expect(std::lang::String message);
```

**行为**

提取成功载荷；None/Err 时以 message 触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `message` | `std::lang::String message` | 诊断文本或消息载荷 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时以 message 触发 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.expect(message);
```

**源码**：[source/std/lang.chl:821](../../source/std/lang.chl#L821)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-100"></a>

### Option::transform（101）

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

**源码**：[source/std/lang.chl:830](../../source/std/lang.chl#L830)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-101"></a>

### Option::andThen（102）

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

**源码**：[source/std/lang.chl:839](../../source/std/lang.chl#L839)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-102"></a>

### Result::branch（103）

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

**源码**：[source/std/lang.chl:860](../../source/std/lang.chl#L860)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-103"></a>

### Result::intoBranch（104）

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

**源码**：[source/std/lang.chl:870](../../source/std/lang.chl#L870)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-104"></a>

### Result::fromResidual（105）

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

**源码**：[source/std/lang.chl:876](../../source/std/lang.chl#L876)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-105"></a>

### Result::ok（106）

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

**源码**：[source/std/lang.chl:877](../../source/std/lang.chl#L877)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-106"></a>

### Result::isOk（107）

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

**源码**：[source/std/lang.chl:883](../../source/std/lang.chl#L883)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-107"></a>

### Result::isErr（108）

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

**源码**：[source/std/lang.chl:892](../../source/std/lang.chl#L892)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-108"></a>

### Result::take（109）

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

**源码**：[source/std/lang.chl:895](../../source/std/lang.chl#L895)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-109"></a>

### Result::unwrap（110）

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

**源码**：[source/std/lang.chl:905](../../source/std/lang.chl#L905)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-110"></a>

### Result::unwrapOr（111）

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

**源码**：[source/std/lang.chl:914](../../source/std/lang.chl#L914)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-111"></a>

### Result::expect（112）

**声明**

```chali
T expect(std::lang::String message);
```

**行为**

提取成功载荷；None/Err 时以 message 触发 panic。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `message` | `std::lang::String message` | 诊断文本或消息载荷 | 无 |

**返回值**

`T`。提取成功载荷；None/Err 时以 message 触发 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
obj.expect(message);
```

**源码**：[source/std/lang.chl:923](../../source/std/lang.chl#L923)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-112"></a>

### Result::takeError（113）

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

**源码**：[source/std/lang.chl:933](../../source/std/lang.chl#L933)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-113"></a>

### Result::error（114）

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

**源码**：[source/std/lang.chl:943](../../source/std/lang.chl#L943)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-114"></a>

### Result::transform（115）

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

**源码**：[source/std/lang.chl:952](../../source/std/lang.chl#L952)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-115"></a>

### Result::andThen（116）

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

**源码**：[source/std/lang.chl:961](../../source/std/lang.chl#L961)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-116"></a>

### NextValue::nextValue（117）

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

**源码**：[source/std/lang.chl:982](../../source/std/lang.chl#L982)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-117"></a>

### Pair::create（118）

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

**源码**：[source/std/lang.chl:1038](../../source/std/lang.chl#L1038)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-118"></a>

### Pair::create（119）

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

**源码**：[source/std/lang.chl:1048](../../source/std/lang.chl#L1048)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-119"></a>

### Pair::toTuple（120）

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

**源码**：[source/std/lang.chl:1057](../../source/std/lang.chl#L1057)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-120"></a>

### tryGet（121）

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

**源码**：[source/std/lang.chl:1081](../../source/std/lang.chl#L1081)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-121"></a>

### tryGetReference（122）

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

**源码**：[source/std/lang.chl:1087](../../source/std/lang.chl#L1087)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-122"></a>

### tryGetMut（123）

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

**源码**：[source/std/lang.chl:1092](../../source/std/lang.chl#L1092)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-123"></a>

### stringBytes（124）

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

**源码**：[source/std/lang.chl:1107](../../source/std/lang.chl#L1107)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-124"></a>

### BarePtr::leftAdd（125）

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

**源码**：[source/std/lang.chl:1158](../../source/std/lang.chl#L1158)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-125"></a>

### BarePtr::rightAdd（126）

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

**源码**：[source/std/lang.chl:1164](../../source/std/lang.chl#L1164)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-126"></a>

### BarePtr::leftSub（127）

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

**源码**：[source/std/lang.chl:1166](../../source/std/lang.chl#L1166)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-127"></a>

### BarePtr::leftSub（128）

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

**源码**：[source/std/lang.chl:1172](../../source/std/lang.chl#L1172)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-128"></a>

### barePointer（129）

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
barePointer<T>(pointer);
```

**源码**：[source/std/lang.chl:1182](../../source/std/lang.chl#L1182)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-129"></a>

### pointerFromAddress（130）

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

**源码**：[source/std/lang.chl:1189](../../source/std/lang.chl#L1189)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-130"></a>

### dereference（131）

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
dereference<T>(pointer);
```

**源码**：[source/std/lang.chl:1204](../../source/std/lang.chl#L1204)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-131"></a>

### memcpy（132）

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
memcpy<T,U>(destination, source, bytes);
```

**源码**：[source/std/lang.chl:1211](../../source/std/lang.chl#L1211)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-132"></a>

### memset（133）

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
memset<T>(destination, value, bytes);
```

**源码**：[source/std/lang.chl:1220](../../source/std/lang.chl#L1220)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-133"></a>

### initAt（134）

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
initAt<T>(pointer, value);
```

**源码**：[source/std/lang.chl:1225](../../source/std/lang.chl#L1225)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-134"></a>

### readAt（135）

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
readAt<T>(pointer);
```

**源码**：[source/std/lang.chl:1233](../../source/std/lang.chl#L1233)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-135"></a>

### borrowAt（136）

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
borrowAt<T>(pointer);
```

**源码**：[source/std/lang.chl:1240](../../source/std/lang.chl#L1240)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-136"></a>

### takeAt（137）

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
takeAt<T>(pointer);
```

**源码**：[source/std/lang.chl:1245](../../source/std/lang.chl#L1245)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-137"></a>

### dropAt（138）

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
dropAt<T>(pointer);
```

**源码**：[source/std/lang.chl:1254](../../source/std/lang.chl#L1254)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-138"></a>

### relocateNonOverlapping（139）

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
relocateNonOverlapping<T>(destination, source, count);
```

**源码**：[source/std/lang.chl:1259](../../source/std/lang.chl#L1259)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-139"></a>

### RawStorage::allocate（140）

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

**源码**：[source/std/lang.chl:1291](../../source/std/lang.chl#L1291)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-140"></a>

### RawStorage::capacity（141）

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

**源码**：[source/std/lang.chl:1299](../../source/std/lang.chl#L1299)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-141"></a>

### RawStorage::pointer（142）

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

**源码**：[source/std/lang.chl:1304](../../source/std/lang.chl#L1304)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-142"></a>

### RawStorage::isInitialized（143）

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

**源码**：[source/std/lang.chl:1312](../../source/std/lang.chl#L1312)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-143"></a>

### RawStorage::close（144）

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

**源码**：[source/std/lang.chl:1316](../../source/std/lang.chl#L1316)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-144"></a>

### range（145）

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

**源码**：[source/chali_generic.cpp:1844](../../source/chali_generic.cpp#L1844)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-145"></a>

### range（146）

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

**源码**：[source/chali_generic.cpp:1852](../../source/chali_generic.cpp#L1852)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-146"></a>

### rangeEq（147）

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

**源码**：[source/chali_generic.cpp:1867](../../source/chali_generic.cpp#L1867)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-147"></a>

### Drop::drop（148）

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

**源码**：[source/chali_generic.cpp:2074](../../source/chali_generic.cpp#L2074)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-148"></a>

### Closeable::close（149）

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

**源码**：[source/chali_generic.cpp:2082](../../source/chali_generic.cpp#L2082)。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-149"></a>

### print（150）

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

**源码**：[source/semantics/boxing.cpp:1315](../../source/semantics/boxing.cpp#L1315)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-150"></a>

### println（151）

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

**源码**：[source/semantics/boxing.cpp:1315](../../source/semantics/boxing.cpp#L1315)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-151"></a>

### eprint（152）

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

<a id="api-152"></a>

### eprintln（153）

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

<a id="api-153"></a>

### format（154）

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

**源码**：[source/semantics/boxing.cpp:1319](../../source/semantics/boxing.cpp#L1319)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-154"></a>

### putChar（155）

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

**源码**：[source/semantics/boxing.cpp:1365](../../source/semantics/boxing.cpp#L1365)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-155"></a>

### pause（156）

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

**源码**：[source/semantics/boxing.cpp:1326](../../source/semantics/boxing.cpp#L1326)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-156"></a>

### inputLine（157）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-157"></a>

### inputInt（158）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-158"></a>

### inputUInt（159）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-159"></a>

### inputFloat（160）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-160"></a>

### inputBool（161）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-161"></a>

### inputChar（162）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-162"></a>

### inputBigInteger（163）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-163"></a>

### inputBigDecimal（164）

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

**源码**：[source/semantics/boxing.cpp:1375](../../source/semantics/boxing.cpp#L1375)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-164"></a>

### panic（165）

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

**源码**：[source/semantics/boxing.cpp:1394](../../source/semantics/boxing.cpp#L1394)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-165"></a>

### len（166）

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

**源码**：[source/semantics/boxing.cpp:745](../../source/semantics/boxing.cpp#L745)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-166"></a>

### toReference（167）

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

**源码**：[source/chali_generic.cpp:1781](../../source/chali_generic.cpp#L1781)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-167"></a>

### toOptional（168）

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

**源码**：[source/chali_generic.cpp:1787](../../source/chali_generic.cpp#L1787)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-168"></a>

### memcpy（169）

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

**源码**：[source/semantics/boxing.cpp:1411](../../source/semantics/boxing.cpp#L1411)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-169"></a>

### memset（170）

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

**源码**：[source/semantics/boxing.cpp:1411](../../source/semantics/boxing.cpp#L1411)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-170"></a>

### List::copy（171）

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

**源码**：[source/semantics/aggregate_ownership.cpp:255](../../source/semantics/aggregate_ownership.cpp#L255)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-171"></a>

### List::push（172）

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

**源码**：[source/semantics/aggregate_ownership.cpp:256](../../source/semantics/aggregate_ownership.cpp#L256)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-172"></a>

### List::insert（173）

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

**源码**：[source/semantics/aggregate_ownership.cpp:257](../../source/semantics/aggregate_ownership.cpp#L257)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-173"></a>

### List::at（174）

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

**源码**：[source/semantics/aggregate_ownership.cpp:258](../../source/semantics/aggregate_ownership.cpp#L258)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-174"></a>

### List::pop（175）

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

**源码**：[source/semantics/aggregate_ownership.cpp:259](../../source/semantics/aggregate_ownership.cpp#L259)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-175"></a>

### List::remove（176）

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

**源码**：[source/semantics/aggregate_ownership.cpp:260](../../source/semantics/aggregate_ownership.cpp#L260)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-176"></a>

### List::indexOf（177）

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

**源码**：[source/semantics/aggregate_ownership.cpp:261](../../source/semantics/aggregate_ownership.cpp#L261)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-177"></a>

### List::contains（178）

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

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-178"></a>

### List::has（179）

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

**源码**：[source/semantics/aggregate_ownership.cpp:263](../../source/semantics/aggregate_ownership.cpp#L263)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-179"></a>

### List::clear（180）

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

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-180"></a>

### List::len（181）

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

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-181"></a>

### Set::copy（182）

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

**源码**：[source/semantics/aggregate_ownership.cpp:255](../../source/semantics/aggregate_ownership.cpp#L255)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-182"></a>

### Set::add（183）

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

**源码**：[source/semantics/aggregate_ownership.cpp:273](../../source/semantics/aggregate_ownership.cpp#L273)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-183"></a>

### Set::has（184）

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

**源码**：[source/semantics/aggregate_ownership.cpp:263](../../source/semantics/aggregate_ownership.cpp#L263)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-184"></a>

### Set::contains（185）

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

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-185"></a>

### Set::remove（186）

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

**源码**：[source/semantics/aggregate_ownership.cpp:260](../../source/semantics/aggregate_ownership.cpp#L260)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-186"></a>

### Set::at（187）

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

**源码**：[source/semantics/aggregate_ownership.cpp:258](../../source/semantics/aggregate_ownership.cpp#L258)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-187"></a>

### Set::clear（188）

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

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-188"></a>

### Set::len（189）

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

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-189"></a>

### Map::copy（190）

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

**源码**：[source/semantics/aggregate_ownership.cpp:255](../../source/semantics/aggregate_ownership.cpp#L255)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-190"></a>

### Map::put（191）

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

**源码**：[source/semantics/aggregate_ownership.cpp:287](../../source/semantics/aggregate_ownership.cpp#L287)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-191"></a>

### Map::has（192）

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

**源码**：[source/semantics/aggregate_ownership.cpp:263](../../source/semantics/aggregate_ownership.cpp#L263)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-192"></a>

### Map::contains（193）

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

**源码**：[source/semantics/aggregate_ownership.cpp:262](../../source/semantics/aggregate_ownership.cpp#L262)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-193"></a>

### Map::remove（194）

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

**源码**：[source/semantics/aggregate_ownership.cpp:260](../../source/semantics/aggregate_ownership.cpp#L260)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-194"></a>

### Map::keyAt（195）

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

**源码**：[source/semantics/aggregate_ownership.cpp:291](../../source/semantics/aggregate_ownership.cpp#L291)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-195"></a>

### Map::valAt（196）

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

**源码**：[source/semantics/aggregate_ownership.cpp:292](../../source/semantics/aggregate_ownership.cpp#L292)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-196"></a>

### Map::clear（197）

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

**源码**：[source/semantics/aggregate_ownership.cpp:264](../../source/semantics/aggregate_ownership.cpp#L264)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-197"></a>

### Map::len（198）

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

**源码**：[source/semantics/aggregate_ownership.cpp:265](../../source/semantics/aggregate_ownership.cpp#L265)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-198"></a>

### BigInteger::str（199）

**声明**

```chali
String str();
```

**行为**

生成大数的十进制文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。生成大数的十进制文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.str();
```

**源码**：[source/semantics/boxing.cpp:848](../../source/semantics/boxing.cpp#L848)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-199"></a>

### BigInteger::copy（200）

**声明**

```chali
BigInteger copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/boxing.cpp:838](../../source/semantics/boxing.cpp#L838)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-200"></a>

### BigInteger::pow（201）

**声明**

```chali
BigInteger pow(int64 exponent);
```

**行为**

计算当前值的 exponent 次非负整数次幂（快速幂）；exponent 为负时报运行错误。

非负整数次幂（快速幂）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `exponent` | `int64 exponent` | 指数 | 无 |

**返回值**

`BigInteger`。计算当前值的 exponent 次非负整数次幂（快速幂）；exponent 为负时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pow(exponent);
```

**源码**：[source/semantics/boxing.cpp:863](../../source/semantics/boxing.cpp#L863)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-201"></a>

### BigInteger::powmod（202）

**声明**

```chali
BigInteger powmod(BigInteger exponent, BigInteger modulus);
```

**行为**

通过快速幂计算 this^exponent mod modulus；指数可以为大整数，modulus 为零或 exponent 为负时报运行错误。

模幂：`this^exponent mod modulus`（指数可以是大整数）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `exponent` | `BigInteger exponent` | 指数 | 无 |
| `modulus` | `BigInteger modulus` | 模数 | 无 |

**返回值**

`BigInteger`。通过快速幂计算 this^exponent mod modulus；指数可以为大整数，modulus 为零或 exponent 为负时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.powmod(exponent, modulus);
```

**源码**：[source/semantics/boxing.cpp:869](../../source/semantics/boxing.cpp#L869)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-202"></a>

### BigInteger::gcd（203）

**声明**

```chali
BigInteger gcd(BigInteger other);
```

**行为**

返回两数绝对值的最大公约数。

最大公约数 / 最小公倍数（取绝对值）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BigInteger other` | 另一操作数 | 无 |

**返回值**

`BigInteger`。返回两数绝对值的最大公约数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.gcd(other);
```

**源码**：[source/semantics/boxing.cpp:875](../../source/semantics/boxing.cpp#L875)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-203"></a>

### BigInteger::lcm（204）

**声明**

```chali
BigInteger lcm(BigInteger other);
```

**行为**

返回两数绝对值的最小公倍数；任一操作数为零给零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BigInteger other` | 另一操作数 | 无 |

**返回值**

`BigInteger`。返回两数绝对值的最小公倍数；任一操作数为零给零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.lcm(other);
```

**源码**：[source/semantics/boxing.cpp:875](../../source/semantics/boxing.cpp#L875)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-204"></a>

### BigInteger::mod（205）

**声明**

```chali
BigInteger mod(BigInteger modulus);
```

**行为**

非负取模：结果落在 [0, |modulus|)；modulus 为零时报运行错误。

非负取模：结果落在 [0, |modulus|)。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `modulus` | `BigInteger modulus` | 模数 | 无 |

**返回值**

`BigInteger`。非负取模：结果落在 [0, |modulus|)；modulus 为零时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.mod(modulus);
```

**源码**：[source/semantics/boxing.cpp:883](../../source/semantics/boxing.cpp#L883)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-205"></a>

### BigInteger::modInverse（206）

**声明**

```chali
BigInteger modInverse(BigInteger modulus);
```

**行为**

计算模乘逆元，结果落在 [0, |modulus|)；模数绝对值不大于 1 或两数不互素时报运行错误。

模逆元；两数不互素时运行期报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `modulus` | `BigInteger modulus` | 模数 | 无 |

**返回值**

`BigInteger`。计算模乘逆元，结果落在 [0, |modulus|)；模数绝对值不大于 1 或两数不互素时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.modInverse(modulus);
```

**源码**：[source/semantics/boxing.cpp:889](../../source/semantics/boxing.cpp#L889)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-206"></a>

### BigInteger::isProbablePrime（207）

**声明**

```chali
bool isProbablePrime(int64 rounds);
```

**行为**

Miller-Rabin 概率素性测试，固定随机种子可复现；rounds 为测试轮数（非负）。

Miller-Rabin 概率素性测试（固定种子，可复现）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rounds` | `int64 rounds` | Miller-Rabin 素性测试轮数 | 无 |

**返回值**

`bool`。Miller-Rabin 概率素性测试，固定随机种子可复现；rounds 为测试轮数（非负）。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isProbablePrime(rounds);
```

**源码**：[source/semantics/boxing.cpp:895](../../source/semantics/boxing.cpp#L895)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-207"></a>

### BigInteger::nextProbablePrime（208）

**声明**

```chali
BigInteger nextProbablePrime();
```

**行为**

返回严格大于当前值的下一个概率素数（25 轮测试）。

严格大于当前值的下一个（概率）素数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。返回严格大于当前值的下一个概率素数（25 轮测试）。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.nextProbablePrime();
```

**源码**：[source/semantics/boxing.cpp:901](../../source/semantics/boxing.cpp#L901)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-208"></a>

### BigInteger::bitLength（209）

**声明**

```chali
int64 bitLength();
```

**行为**

返回当前值（绝对值）的二进制位长。

二进制位长。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前值（绝对值）的二进制位长。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bitLength();
```

**源码**：[source/semantics/boxing.cpp:907](../../source/semantics/boxing.cpp#L907)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-209"></a>

### BigInteger::bitAnd（210）

**声明**

```chali
BigInteger bitAnd(BigInteger other);
```

**行为**

按位与；仅支持非负操作数，负数报运行错误。

按位与 / 或 / 异或（仅非负数；负数运行期报错）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BigInteger other` | 另一操作数 | 无 |

**返回值**

`BigInteger`。按位与；仅支持非负操作数，负数报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bitAnd(other);
```

**源码**：[source/semantics/boxing.cpp:913](../../source/semantics/boxing.cpp#L913)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-210"></a>

### BigInteger::bitOr（211）

**声明**

```chali
BigInteger bitOr(BigInteger other);
```

**行为**

按位或；仅支持非负操作数，负数报运行错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BigInteger other` | 另一操作数 | 无 |

**返回值**

`BigInteger`。按位或；仅支持非负操作数，负数报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bitOr(other);
```

**源码**：[source/semantics/boxing.cpp:913](../../source/semantics/boxing.cpp#L913)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-211"></a>

### BigInteger::bitXor（212）

**声明**

```chali
BigInteger bitXor(BigInteger other);
```

**行为**

按位异或；仅支持非负操作数，负数报运行错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `BigInteger other` | 另一操作数 | 无 |

**返回值**

`BigInteger`。按位异或；仅支持非负操作数，负数报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bitXor(other);
```

**源码**：[source/semantics/boxing.cpp:913](../../source/semantics/boxing.cpp#L913)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-212"></a>

### BigInteger::strRadix（213）

**声明**

```chali
String strRadix(int64 base);
```

**行为**

生成 base（2..36）进制文本，数字用 0-9a-z，负数带前导 -；越界进制报运行错误。

2..36 进制字符串（数字用 0-9a-z，负数带 -）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `base` | `int64 base` | 幂底数 | 无 |

**返回值**

`String`。生成 base（2..36）进制文本，数字用 0-9a-z，负数带前导 -；越界进制报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.strRadix(base);
```

**源码**：[source/semantics/boxing.cpp:924](../../source/semantics/boxing.cpp#L924)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-213"></a>

### BigInteger::iroot（214）

**声明**

```chali
BigInteger iroot(int64 k);
```

**行为**

整数 k 次方根，向下取整；k 小于 1 报运行错误。

整数 k 次方根（向下取整）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `k` | `int64 k` | 方根次数 | 无 |

**返回值**

`BigInteger`。整数 k 次方根，向下取整；k 小于 1 报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iroot(k);
```

**源码**：[source/semantics/boxing.cpp:930](../../source/semantics/boxing.cpp#L930)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-214"></a>

### BigDecimal::str（215）

**声明**

```chali
String str();
```

**行为**

生成大数的十进制文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。生成大数的十进制文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.str();
```

**源码**：[source/semantics/boxing.cpp:848](../../source/semantics/boxing.cpp#L848)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-215"></a>

### BigDecimal::copy（216）

**声明**

```chali
BigDecimal copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigDecimal`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/semantics/boxing.cpp:838](../../source/semantics/boxing.cpp#L838)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-216"></a>

### BigDecimal::setPrecision（217）

**声明**

```chali
int64 setPrecision(int64 digits);
```

**行为**

设置此 BigDecimal 的十进制精度；要求有效正精度，操作修改当前值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `digits` | `int64 digits` | 十进制有效数字精度 | 无 |

**返回值**

`int64`。设置此 BigDecimal 的十进制精度；要求有效正精度，操作修改当前值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setPrecision(digits);
```

**源码**：[source/semantics/boxing.cpp:827](../../source/semantics/boxing.cpp#L827)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-217"></a>

### BigDecimal::pow（218）

**声明**

```chali
BigDecimal pow(int64 exponent);
```

**行为**

计算当前值的 exponent 次非负整数次幂，保持当前精度；exponent 为负时报运行错误。

精确非负整数次幂。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `exponent` | `int64 exponent` | 指数 | 无 |

**返回值**

`BigDecimal`。计算当前值的 exponent 次非负整数次幂，保持当前精度；exponent 为负时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pow(exponent);
```

**源码**：[source/semantics/boxing.cpp:863](../../source/semantics/boxing.cpp#L863)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-218"></a>

### BigDecimal::abs（219）

**声明**

```chali
BigDecimal abs();
```

**行为**

返回绝对值。

绝对值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigDecimal`。返回绝对值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.abs();
```

**源码**：[source/semantics/boxing.cpp:946](../../source/semantics/boxing.cpp#L946)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-219"></a>

### BigDecimal::isInteger（220）

**声明**

```chali
bool isInteger();
```

**行为**

判断当前值是否为整数。

是不是整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断当前值是否为整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isInteger();
```

**源码**：[source/semantics/boxing.cpp:952](../../source/semantics/boxing.cpp#L952)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-220"></a>

### BigDecimal::floor（221）

**声明**

```chali
BigInteger floor();
```

**行为**

向负无穷取整，返回大整数。

向负无穷 / 向正无穷 / 向零取整，返回大整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。向负无穷取整，返回大整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.floor();
```

**源码**：[source/semantics/boxing.cpp:958](../../source/semantics/boxing.cpp#L958)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-221"></a>

### BigDecimal::ceil（222）

**声明**

```chali
BigInteger ceil();
```

**行为**

向正无穷取整，返回大整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。向正无穷取整，返回大整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.ceil();
```

**源码**：[source/semantics/boxing.cpp:958](../../source/semantics/boxing.cpp#L958)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-222"></a>

### BigDecimal::trunc（223）

**声明**

```chali
BigInteger trunc();
```

**行为**

向零截断小数部分，返回大整数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BigInteger`。向零截断小数部分，返回大整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.trunc();
```

**源码**：[source/semantics/boxing.cpp:958](../../source/semantics/boxing.cpp#L958)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-223"></a>

### BigDecimal::sqrt（224）

**声明**

```chali
BigDecimal sqrt(int64 digits);
```

**行为**

计算平方根，保留 digits 位有效数字。

平方根，保留 digits 位有效数字。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `digits` | `int64 digits` | 十进制有效数字精度 | 无 |

**返回值**

`BigDecimal`。计算平方根，保留 digits 位有效数字。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.sqrt(digits);
```

**源码**：[source/semantics/boxing.cpp:968](../../source/semantics/boxing.cpp#L968)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-224"></a>

### BigDecimal::root（225）

**声明**

```chali
BigDecimal root(int64 k, int64 digits);
```

**行为**

计算 k 次方根，保留 digits 位有效数字；k 小于 1 或 digits 越界时报运行错误。

k 次方根，保留 digits 位有效数字。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `k` | `int64 k` | 方根次数 | 无 |
| `digits` | `int64 digits` | 十进制有效数字精度 | 无 |

**返回值**

`BigDecimal`。计算 k 次方根，保留 digits 位有效数字；k 小于 1 或 digits 越界时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.root(k, digits);
```

**源码**：[source/semantics/boxing.cpp:974](../../source/semantics/boxing.cpp#L974)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-225"></a>

### Any::copy（226）

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

**源码**：[source/semantics/boxing.cpp:838](../../source/semantics/boxing.cpp#L838)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-226"></a>

### Any::isEmpty（227）

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

**源码**：[source/semantics/boxing.cpp:1000](../../source/semantics/boxing.cpp#L1000)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-227"></a>

### Any::clear（228）

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

**源码**：[source/semantics/boxing.cpp:1007](../../source/semantics/boxing.cpp#L1007)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-228"></a>

### Any::set（229）

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

**源码**：[source/semantics/boxing.cpp:1023](../../source/semantics/boxing.cpp#L1023)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-229"></a>

### Any::getValue（230）

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

**源码**：[source/semantics/boxing.cpp:134](../../source/semantics/boxing.cpp#L134)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-230"></a>

### Any::getReference（231）

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

**源码**：[source/semantics/boxing.cpp:134](../../source/semantics/boxing.cpp#L134)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-231"></a>

### Any::getOptional（232）

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

**源码**：[source/semantics/boxing.cpp:134](../../source/semantics/boxing.cpp#L134)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-232"></a>

### eq（233）

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

<a id="api-233"></a>

### ne（234）

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

<a id="api-234"></a>

### lt（235）

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

<a id="api-235"></a>

### le（236）

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

<a id="api-236"></a>

### gt（237）

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

<a id="api-237"></a>

### ge（238）

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

<a id="api-238"></a>

### add（239）

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

**源码**：[source/semantics/boxing.cpp:1180](../../source/semantics/boxing.cpp#L1180)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-239"></a>

### sub（240）

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

<a id="api-240"></a>

### mul（241）

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

<a id="api-241"></a>

### div（242）

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

<a id="api-242"></a>

### mod（243）

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

**源码**：[source/semantics/boxing.cpp:883](../../source/semantics/boxing.cpp#L883)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-243"></a>

### index（244）

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
index(data, index);
```

**源码**：[source/semantics/boxing.cpp:1793](../../source/semantics/boxing.cpp#L1793)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-244"></a>

### Add::leftAdd（245）

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

<a id="api-245"></a>

### RightAdd::rightAdd（246）

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

<a id="api-246"></a>

### Subtract::leftSub（247）

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

<a id="api-247"></a>

### RightSubtract::rightSub（248）

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

<a id="api-248"></a>

### Multiply::leftMul（249）

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

<a id="api-249"></a>

### RightMultiply::rightMul（250）

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

<a id="api-250"></a>

### Divide::leftDiv（251）

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

<a id="api-251"></a>

### RightDivide::rightDiv（252）

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

<a id="api-252"></a>

### Modulo::leftMod（253）

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

<a id="api-253"></a>

### RightModulo::rightMod（254）

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

<a id="api-254"></a>

### Equal::leftEqual（255）

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

<a id="api-255"></a>

### RightEqual::rightEqual（256）

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

<a id="api-256"></a>

### NotEqual::leftNotEqual（257）

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

<a id="api-257"></a>

### RightNotEqual::rightNotEqual（258）

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

<a id="api-258"></a>

### Less::leftLess（259）

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

<a id="api-259"></a>

### RightLess::rightLess（260）

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

<a id="api-260"></a>

### LessOrEqual::leftLessEqual（261）

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

<a id="api-261"></a>

### RightLessOrEqual::rightLessEqual（262）

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

<a id="api-262"></a>

### Greater::leftGreater（263）

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

<a id="api-263"></a>

### RightGreater::rightGreater（264）

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

<a id="api-264"></a>

### GreaterOrEqual::leftGreaterEqual（265）

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

<a id="api-265"></a>

### RightGreaterOrEqual::rightGreaterEqual（266）

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

<a id="api-266"></a>

### And::leftBitAnd（267）

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

<a id="api-267"></a>

### RightAnd::rightBitAnd（268）

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

<a id="api-268"></a>

### Or::leftBitOr（269）

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

<a id="api-269"></a>

### RightOr::rightBitOr（270）

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

<a id="api-270"></a>

### Xor::leftBitXor（271）

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

<a id="api-271"></a>

### RightXor::rightBitXor（272）

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

<a id="api-272"></a>

### ShiftLeft::leftShiftLeft（273）

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

<a id="api-273"></a>

### RightShiftLeft::rightShiftLeft（274）

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

<a id="api-274"></a>

### ShiftRight::leftShiftRight（275）

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

<a id="api-275"></a>

### RightShiftRight::rightShiftRight（276）

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

<a id="api-276"></a>

### Negate::negate（277）

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

<a id="api-277"></a>

### BitNot::bitNot（278）

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

<a id="api-278"></a>

### Positive::positive（279）

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

<a id="api-279"></a>

### Hash::hash（280）

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

<a id="api-280"></a>

### Copy::copy（281）

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

**源码**：[source/chali_generic.cpp:1764](../../source/chali_generic.cpp#L1764)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-281"></a>

### ExplicitConvert::explicitConvert（282）

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

<a id="api-282"></a>

### ImplicitConvert::implicitConvert（283）

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

<a id="api-283"></a>

### Index::index（284）

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

<a id="api-284"></a>

### Iterator::iterator（285）

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

<a id="api-285"></a>

### Result::mapErr（286）

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
obj.mapErr<F>(mapper);
```

**源码**：[source/std/lang.chl:2](../../source/std/lang.chl#L2)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-286"></a>

### Option::okOr（287）

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
obj.okOr<E>(error);
```

**源码**：[source/std/lang.chl:3](../../source/std/lang.chl#L3)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-287"></a>

### Option::okOrElse（288）

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
obj.okOrElse<E>(factory);
```

**源码**：[source/std/lang.chl:3](../../source/std/lang.chl#L3)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

<a id="api-288"></a>

### BarePtr::castTo（289）

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

**源码**：[source/std/lang.chl:4](../../source/std/lang.chl#L4)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_collections_algorithms.chl](../../example/samples/std_collections_algorithms.chl)。

## 补充完整示例

[example/samples/std_read_lines.chl](../../example/samples/std_read_lines.chl)。

```chali
void main() {
    String first=std::lang::readLine();
    String rest=std::lang::readLines(3);
    println("[{}]",first);
    println("[{}]",rest);
    assert(std::lang::readLines(0)=="");
    assert(std::lang::readLine()=="");
}
```
