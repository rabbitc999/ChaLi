# std::big_number — 任意精度数值

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-big_number.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::big_number;
```

## 公共约定与错误边界

大整数和十进制大数支持算术、构造和格式化。BigDecimal 的有效位数范围为 1..100000。

## 参考代码

完整程序：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

```powershell
chali run example/samples/std_bignumber.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::big_number;
import std::json;
import std::reflect;

@Reflect
struct Numbers {
    BigInteger integer;
    BigDecimal decimal;
}

$macro CHECK_IDENTITIES<T>() -> stmt
$if ($fields<T>())[0].type != BigInteger
$compile_error("unexpected BigInteger identity");
$end_if
$if ($fields<T>())[1].type != BigDecimal
$compile_error("unexpected BigDecimal identity");
$end_if
$end_macro

void main() {
    $CHECK_IDENTITIES<Numbers>();
    Numbers values;
    values.integer = BigInteger("9000000000000000000") + BigInteger(7);
    values.decimal = BigDecimal("1.25") + BigDecimal("2.50");
    Option<BigInteger> boxed = Option<BigInteger>::Value(values.integer);
    uint64 integerId = getTypeId<BigInteger>();
    assert(integerId != 0 && integerId == getTypeId<BigInteger>());
    String encoded = serialize<Numbers>(&values).unwrap();
    Numbers decoded = deserialize<Numbers>(encoded).unwrap();
    assert(decoded.integer == boxed.unwrap() && decoded.decimal == values.decimal);
    println("{} {}", decoded.integer, decoded.decimal);
}
```

</details>

<details>
<summary>示例输出</summary>

```text
9000000000000000007 3.75
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-1"></a>`opcode BigInteger` | 任意精度有符号整数，算术由大数运行时实现。 |
| 模块 | <a id="symbol-2"></a>`opcode BigDecimal` | 可配置精度的十进制大数。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [BigInteger::str](#api-0) | `String str()` |
| [BigInteger::copy](#api-1) | `BigInteger copy()` |
| [BigInteger::pow](#api-2) | `BigInteger pow(int64 exponent)` |
| [BigInteger::powmod](#api-3) | `BigInteger powmod(BigInteger exponent, BigInteger modulus)` |
| [BigInteger::gcd](#api-4) | `BigInteger gcd(BigInteger other)` |
| [BigInteger::lcm](#api-5) | `BigInteger lcm(BigInteger other)` |
| [BigInteger::mod](#api-6) | `BigInteger mod(BigInteger modulus)` |
| [BigInteger::modInverse](#api-7) | `BigInteger modInverse(BigInteger modulus)` |
| [BigInteger::isProbablePrime](#api-8) | `bool isProbablePrime(int64 rounds)` |
| [BigInteger::nextProbablePrime](#api-9) | `BigInteger nextProbablePrime()` |
| [BigInteger::bitLength](#api-10) | `int64 bitLength()` |
| [BigInteger::bitAnd](#api-11) | `BigInteger bitAnd(BigInteger other)` |
| [BigInteger::bitOr](#api-12) | `BigInteger bitOr(BigInteger other)` |
| [BigInteger::bitXor](#api-13) | `BigInteger bitXor(BigInteger other)` |
| [BigInteger::strRadix](#api-14) | `String strRadix(int64 base)` |
| [BigInteger::iroot](#api-15) | `BigInteger iroot(int64 k)` |
| [BigDecimal::str](#api-16) | `String str()` |
| [BigDecimal::copy](#api-17) | `BigDecimal copy()` |
| [BigDecimal::setPrecision](#api-18) | `int64 setPrecision(int64 digits)` |
| [BigDecimal::pow](#api-19) | `BigDecimal pow(int64 exponent)` |
| [BigDecimal::abs](#api-20) | `BigDecimal abs()` |
| [BigDecimal::isInteger](#api-21) | `bool isInteger()` |
| [BigDecimal::floor](#api-22) | `BigInteger floor()` |
| [BigDecimal::ceil](#api-23) | `BigInteger ceil()` |
| [BigDecimal::trunc](#api-24) | `BigInteger trunc()` |
| [BigDecimal::sqrt](#api-25) | `BigDecimal sqrt(int64 digits)` |
| [BigDecimal::root](#api-26) | `BigDecimal root(int64 k, int64 digits)` |

<a id="api-0"></a>

### BigInteger::str（1）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-1"></a>

### BigInteger::copy（2）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-2"></a>

### BigInteger::pow（3）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-3"></a>

### BigInteger::powmod（4）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-4"></a>

### BigInteger::gcd（5）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-5"></a>

### BigInteger::lcm（6）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-6"></a>

### BigInteger::mod（7）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-7"></a>

### BigInteger::modInverse（8）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-8"></a>

### BigInteger::isProbablePrime（9）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-9"></a>

### BigInteger::nextProbablePrime（10）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-10"></a>

### BigInteger::bitLength（11）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-11"></a>

### BigInteger::bitAnd（12）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-12"></a>

### BigInteger::bitOr（13）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-13"></a>

### BigInteger::bitXor（14）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-14"></a>

### BigInteger::strRadix（15）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-15"></a>

### BigInteger::iroot（16）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-16"></a>

### BigDecimal::str（17）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-17"></a>

### BigDecimal::copy（18）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-18"></a>

### BigDecimal::setPrecision（19）

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

**源码**：[source/semantics/lowering.cpp:1781](../../source/semantics/lowering.cpp#L1781)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-19"></a>

### BigDecimal::pow（20）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-20"></a>

### BigDecimal::abs（21）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-21"></a>

### BigDecimal::isInteger（22）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-22"></a>

### BigDecimal::floor（23）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-23"></a>

### BigDecimal::ceil（24）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-24"></a>

### BigDecimal::trunc（25）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-25"></a>

### BigDecimal::sqrt（26）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

<a id="api-26"></a>

### BigDecimal::root（27）

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

**源码**：[source/semantics/lowering.cpp:1](../../source/semantics/lowering.cpp#L1)（编译器内建，签名为等价调用形式）。

**完整示例**：[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

## 补充完整示例

[example/samples/bigint_methods.chl](../../example/samples/bigint_methods.chl)。

```chali
/* bigint_methods.chl —— BigInteger / BigDecimal 的扩展方法。
 *
 *   ./chali samples/bigint_methods.chl
 *   → 幂 / 模幂 / 数论 / 素性 / 位运算 / 进制 / 开方（整数与十进制浮点）
 *
 * 这些方法在语言层直接落到 `source/bignum` 的 C++ 成员上：
 *   * 幂与数论：`.pow(e)` `.powmod(e, m)` `.mod(m)` `.modInverse(m)` `.gcd(b)` `.lcm(b)`
 *   * 素性：`.isProbablePrime(rounds)`（Miller-Rabin，固定种子可复现）`.nextProbablePrime()`
 *   * 位运算：`.bitLength()` `.bitAnd(b)` `.bitOr(b)` `.bitXor(b)`（仅非负数）
 *   * 进制与开方：`.strRadix(base)`（2..36）`.iroot(k)`（整数 k 次方根）
 *   * BigDecimal：`.pow(e)` `.sqrt(digits)` `.root(k, digits)` `.abs()` `.isInteger()`
 *     `.floor()` `.ceil()` `.trunc()`（取整返回 BigInteger）
 */

void main() {
    /* 1) 幂与数论 */
    BigInteger a = BigInteger("1000000000000000000000000000000");
    BigInteger big = a.pow(3);
    println("10^30 的 3 次方共 {} 位", len(big.str()));
    println("gcd(10^30, 70) = {}", a.gcd(BigInteger(70)).str());
    println("(-7) mod 3 = {}", BigInteger(-7).mod(BigInteger(3)).str());
    println("3 的模 11 逆元 = {}", BigInteger(3).modInverse(BigInteger(11)).str());
    println("7^300 mod 1000000007 = {}",
            BigInteger(7).powmod(BigInteger(300), BigInteger("1000000007")).str());
    println("lcm(12, 18) = {}", BigInteger(12).lcm(BigInteger(18)).str());

    /* 2) 素性（固定种子的 Miller-Rabin，同一输入多轮完全确定） */
    println("2^61 - 1 是素数？ {}", BigInteger("2305843009213693951").isProbablePrime(25));
    println("2^67 - 1 是素数？ {}", BigInteger("147573952589676412927").isProbablePrime(25));
    println("2^31 - 1 的下一个素数 = {}", BigInteger(2147483647).nextProbablePrime().str());

    /* 3) 位运算（仅非负数） */
    println("12 ^ 10 = {}", BigInteger(12).bitXor(BigInteger(10)).str());
    println("12 | 10 = {}", BigInteger(12).bitOr(BigInteger(10)).str());
    println("1024 的位长 = {}", BigInteger(1024).bitLength());

    /* 4) 进制与整数开方 */
    println("255 的十六进制 = {}", BigInteger(255).strRadix(16));
    println("1000 的立方根 = {}", BigInteger(1000).iroot(3).str());

    /* 5) BigDecimal：幂 / 开方 / 取整 */
    println("2 的 10 次幂 = {}", BigDecimal("2").pow(10).str());
    println("2 的平方根(12 位) = {}", BigDecimal("2").sqrt(12).str());
    println("8 的立方根 = {}", BigDecimal("8").root(3, 10).str());
    println("-2.7 向下取整 = {}，向零截断 = {}",
            BigDecimal("-2.7").floor().str(), BigDecimal("-2.7").trunc().str());
    println("-2.5 的绝对值 = {}，5.5 是整数？ {}", BigDecimal("-2.5").abs().str(),
            BigDecimal("5.5").isInteger());
}
```

## 补充完整示例

[example/samples/std_bignumber.chl](../../example/samples/std_bignumber.chl)。

```chali
import std::big_number;
import std::json;
import std::reflect;

@Reflect
struct Numbers {
    BigInteger integer;
    BigDecimal decimal;
}

$macro CHECK_IDENTITIES<T>() -> stmt
$if ($fields<T>())[0].type != BigInteger
$compile_error("unexpected BigInteger identity");
$end_if
$if ($fields<T>())[1].type != BigDecimal
$compile_error("unexpected BigDecimal identity");
$end_if
$end_macro

void main() {
    $CHECK_IDENTITIES<Numbers>();
    Numbers values;
    values.integer = BigInteger("9000000000000000000") + BigInteger(7);
    values.decimal = BigDecimal("1.25") + BigDecimal("2.50");
    Option<BigInteger> boxed = Option<BigInteger>::Value(values.integer);
    uint64 integerId = getTypeId<BigInteger>();
    assert(integerId != 0 && integerId == getTypeId<BigInteger>());
    String encoded = serialize<Numbers>(&values).unwrap();
    Numbers decoded = deserialize<Numbers>(encoded).unwrap();
    assert(decoded.integer == boxed.unwrap() && decoded.decimal == values.decimal);
    println("{} {}", decoded.integer, decoded.decimal);
}
```
