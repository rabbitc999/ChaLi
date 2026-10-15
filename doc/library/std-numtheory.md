# std::numtheory — 整数数论

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```magel
import std::numtheory;
```

## 公共约定与错误边界

gcd/lcm 采用 uint64。模运算要求 modulus>0；模乘通过逐位加法避免乘法中间溢出。isPrime 使用适用于 uint64 的确定性 Miller-Rabin。modInverse 要求 modulus>1 且与 value 互素；不存在逆元时报告运行错误。lcm 溢出也报告运行错误。

## 参考代码

完整程序：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

```powershell
magel run example/samples/std_numerics_number_theory.mgl
```

<details>
<summary>展开完整参考代码</summary>

```magel
import std::linalg;
import std::numerics;
import std::numtheory;

import std::math;

float64 square(float64 x) { return x*x; }
float64 rootFunction(float64 x) { return x*x-2.0; }
float64 exponentialRate(float64 time, float64 value) { return value; }

int64 main() {
    std::linalg::Vector<float32> left=std::linalg::Vector<float32>::zeros(2);
    left.set(0,(float32)3.0); left.set(1,(float32)4.0);
    std::linalg::Vector<float32> right=left+left;
    std::linalg::Vector<float32> difference=right-left;
    std::linalg::Vector<float32> scaled=left*(float32)2.0;
    assert(difference.at(0)==(float32)3.0 && scaled.at(1)==(float32)8.0);
    std::linalg::Matrix<float32> matrix=std::linalg::Matrix<float32>::identity(2);
    std::linalg::Matrix<float32> doubled=matrix+matrix;
    std::linalg::Matrix<float32> recovered=doubled-matrix;
    std::linalg::Matrix<float32> scaledMatrix=matrix*(float32)3.0;
    std::linalg::Vector<float32> mapped=doubled*left;
    assert(mapped.at(0)==(float32)6.0 && mapped.at(1)==(float32)8.0);
    assert(recovered.at(1,1)==(float32)1.0 && scaledMatrix.at(0,0)==(float32)3.0);
    std::linalg::Matrix<float32> product=matrix*matrix;
    assert(product.at(0,0)==(float32)1.0);

    float64 integral=std::numerics::Numerics::integrateSimpson(square,0.0,1.0,20);
    float64 slope=std::numerics::Numerics::derivative(square,2.0,0.0001);
    float64 root=std::numerics::Numerics::bisect(rootFunction,1.0,2.0,0.00000001,100);
    float64 newton=std::numerics::Numerics::newton(rootFunction,1.0,0.00000001,20);
    List<float64> euler=std::numerics::Numerics::solveOdeEuler(exponentialRate,0.0,1.0,0.1,10);
    List<float64> rk4=std::numerics::Numerics::solveOdeRK4(exponentialRate,0.0,1.0,0.1,10);
    assert(std::math::abs(integral-1.0/3.0)<0.00001);
    assert(std::math::abs(slope-4.0)<0.00001);
    assert(std::math::abs(root-std::math::sqrt(2.0))<0.000001);
    assert(std::math::abs(newton-std::math::sqrt(2.0))<0.000001);
    assert(euler.len()==11 && rk4.len()==11);
    assert(rk4.at(10)>euler.at(10) &&
                std::math::abs(rk4.at(10)-std::math::E)<0.00001);

    assert(std::numtheory::NumberTheory::gcd((uint64)54,(uint64)24)==6);
    assert(std::numtheory::NumberTheory::lcm((uint64)6,(uint64)8)==24);
    assert(std::numtheory::NumberTheory::modInverse((int64)3,(int64)11)==4);
    assert(std::numtheory::NumberTheory::modInverse((int64)-3,(int64)11)==7);
    assert(std::numtheory::NumberTheory::powMod((uint64)7,(uint64)128,(uint64)97)==35);
    assert(std::numtheory::NumberTheory::isPrime((uint64)97));
    assert(!std::numtheory::NumberTheory::isPrime((uint64)561));
    uint64 largePrime=(uint64)2305843009213693951;
    assert(largePrime==(((uint64)1<<61)-1));
    assert(std::numtheory::NumberTheory::mulMod(largePrime-1,largePrime-1,largePrime)==1);
    assert(std::numtheory::NumberTheory::isPrime((uint64)65537));
    println("generic matrix numerics number theory OK");
    return 0;
}
```

</details>

<details>
<summary>示例输出</summary>

```text
generic matrix numerics number theory OK
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-293"></a>`struct NumberTheory` | 整数数论算法的静态入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [numtheoryGcd](#api-0) | `uint64 numtheoryGcd(uint64 left, uint64 right)` |
| [numtheoryLcm](#api-1) | `uint64 numtheoryLcm(uint64 left, uint64 right)` |
| [numtheoryAddMod](#api-2) | `uint64 numtheoryAddMod(uint64 left, uint64 right, uint64 modulus)` |
| [numtheoryMulMod](#api-3) | `uint64 numtheoryMulMod(uint64 left, uint64 right, uint64 modulus)` |
| [numtheoryPowMod](#api-4) | `uint64 numtheoryPowMod(uint64 base, uint64 exponent, uint64 modulus)` |
| [numtheoryIsPrime](#api-5) | `bool numtheoryIsPrime(uint64 value)` |
| [numtheoryModInverse](#api-6) | `int64 numtheoryModInverse(int64 value, int64 modulus)` |
| [NumberTheory::gcd](#api-7) | `static uint64 gcd(uint64 left, uint64 right)` |
| [NumberTheory::lcm](#api-8) | `static uint64 lcm(uint64 left, uint64 right)` |
| [NumberTheory::addMod](#api-9) | `static uint64 addMod(uint64 left, uint64 right, uint64 modulus)` |
| [NumberTheory::mulMod](#api-10) | `static uint64 mulMod(uint64 left, uint64 right, uint64 modulus)` |
| [NumberTheory::powMod](#api-11) | `static uint64 powMod(uint64 base, uint64 exponent, uint64 modulus)` |
| [NumberTheory::isPrime](#api-12) | `static bool isPrime(uint64 value)` |
| [NumberTheory::modInverse](#api-13) | `static int64 modInverse(int64 value, int64 modulus)` |

<a id="api-0"></a>

### numtheoryGcd（1）

**声明**

```magel
uint64 numtheoryGcd(uint64 left, uint64 right);
```

**行为**

宿主入口：计算最大公约数。

整数数论。实现下沉到宿主 C++（glue/std_numtheory.hpp），本文件只留宿主签名 + 极薄 wrapper。 语义与拆分前一致：模乘用逐位加法避免 uint64 乘法溢出，isPrime 用确定性 Miller-Rabin。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |

**返回值**

`uint64`。宿主入口：计算最大公约数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryGcd(left, right);
```

**源码**：[source/std/numtheory.mgl:5](../../source/std/numtheory.mgl#L5)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-1"></a>

### numtheoryLcm（2）

**声明**

```magel
uint64 numtheoryLcm(uint64 left, uint64 right);
```

**行为**

宿主入口：计算最小公倍数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |

**返回值**

`uint64`。宿主入口：计算最小公倍数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryLcm(left, right);
```

**源码**：[source/std/numtheory.mgl:6](../../source/std/numtheory.mgl#L6)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-2"></a>

### numtheoryAddMod（3）

**声明**

```magel
uint64 numtheoryAddMod(uint64 left, uint64 right, uint64 modulus);
```

**行为**

宿主入口：计算 (left+right) mod modulus。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。宿主入口：计算 (left+right) mod modulus。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryAddMod(left, right, modulus);
```

**源码**：[source/std/numtheory.mgl:7](../../source/std/numtheory.mgl#L7)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-3"></a>

### numtheoryMulMod（4）

**声明**

```magel
uint64 numtheoryMulMod(uint64 left, uint64 right, uint64 modulus);
```

**行为**

宿主入口：计算 (left×right) mod modulus，避免直接乘法的中间溢出。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。宿主入口：计算 (left×right) mod modulus，避免直接乘法的中间溢出。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryMulMod(left, right, modulus);
```

**源码**：[source/std/numtheory.mgl:8](../../source/std/numtheory.mgl#L8)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-4"></a>

### numtheoryPowMod（5）

**声明**

```magel
uint64 numtheoryPowMod(uint64 base, uint64 exponent, uint64 modulus);
```

**行为**

宿主入口：通过快速幂计算 base^exponent mod modulus。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `base` | `uint64 base` | 幂底数 | 无 |
| `exponent` | `uint64 exponent` | 指数 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。宿主入口：通过快速幂计算 base^exponent mod modulus。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryPowMod(base, exponent, modulus);
```

**源码**：[source/std/numtheory.mgl:9](../../source/std/numtheory.mgl#L9)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-5"></a>

### numtheoryIsPrime（6）

**声明**

```magel
bool numtheoryIsPrime(uint64 value);
```

**行为**

宿主入口：判断 uint64 是否为素数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `uint64 value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。宿主入口：判断 uint64 是否为素数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryIsPrime(value);
```

**源码**：[source/std/numtheory.mgl:10](../../source/std/numtheory.mgl#L10)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-6"></a>

### numtheoryModInverse（7）

**声明**

```magel
int64 numtheoryModInverse(int64 value, int64 modulus);
```

**行为**

宿主入口：计算模乘逆元；模数不大于 1 或不互素时报运行错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |
| `modulus` | `int64 modulus` | 模数 | 无 |

**返回值**

`int64`。宿主入口：计算模乘逆元；模数不大于 1 或不互素时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::numtheoryModInverse(value, modulus);
```

**源码**：[source/std/numtheory.mgl:11](../../source/std/numtheory.mgl#L11)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-7"></a>

### NumberTheory::gcd（8）

**声明**

```magel
static uint64 gcd(uint64 left, uint64 right);
```

**行为**

计算最大公约数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |

**返回值**

`uint64`。计算最大公约数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::gcd(left, right);
```

**源码**：[source/std/numtheory.mgl:14](../../source/std/numtheory.mgl#L14)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-8"></a>

### NumberTheory::lcm（9）

**声明**

```magel
static uint64 lcm(uint64 left, uint64 right);
```

**行为**

计算最小公倍数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |

**返回值**

`uint64`。计算最小公倍数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::lcm(left, right);
```

**源码**：[source/std/numtheory.mgl:15](../../source/std/numtheory.mgl#L15)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-9"></a>

### NumberTheory::addMod（10）

**声明**

```magel
static uint64 addMod(uint64 left, uint64 right, uint64 modulus);
```

**行为**

计算 (left+right) mod modulus。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。计算 (left+right) mod modulus。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::addMod(left, right, modulus);
```

**源码**：[source/std/numtheory.mgl:16](../../source/std/numtheory.mgl#L16)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-10"></a>

### NumberTheory::mulMod（11）

**声明**

```magel
static uint64 mulMod(uint64 left, uint64 right, uint64 modulus);
```

**行为**

计算 (left×right) mod modulus，避免直接乘法的中间溢出。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `uint64 left` | 左操作数或区间左端 | 无 |
| `right` | `uint64 right` | 右操作数或区间右端 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。计算 (left×right) mod modulus，避免直接乘法的中间溢出。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::mulMod(left, right, modulus);
```

**源码**：[source/std/numtheory.mgl:17](../../source/std/numtheory.mgl#L17)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-11"></a>

### NumberTheory::powMod（12）

**声明**

```magel
static uint64 powMod(uint64 base, uint64 exponent, uint64 modulus);
```

**行为**

通过快速幂计算 base^exponent mod modulus。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `base` | `uint64 base` | 幂底数 | 无 |
| `exponent` | `uint64 exponent` | 指数 | 无 |
| `modulus` | `uint64 modulus` | 模数 | 无 |

**返回值**

`uint64`。通过快速幂计算 base^exponent mod modulus。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::powMod(base, exponent, modulus);
```

**源码**：[source/std/numtheory.mgl:18](../../source/std/numtheory.mgl#L18)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-12"></a>

### NumberTheory::isPrime（13）

**声明**

```magel
static bool isPrime(uint64 value);
```

**行为**

判断 uint64 是否为素数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `uint64 value` | 输入值或写入的新值 | 无 |

**返回值**

`bool`。判断 uint64 是否为素数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::isPrime(value);
```

**源码**：[source/std/numtheory.mgl:19](../../source/std/numtheory.mgl#L19)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-13"></a>

### NumberTheory::modInverse（14）

**声明**

```magel
static int64 modInverse(int64 value, int64 modulus);
```

**行为**

计算模乘逆元；模数不大于 1 或不互素时报运行错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |
| `modulus` | `int64 modulus` | 模数 | 无 |

**返回值**

`int64`。计算模乘逆元；模数不大于 1 或不互素时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numtheory::NumberTheory::modInverse(value, modulus);
```

**源码**：[source/std/numtheory.mgl:20](../../source/std/numtheory.mgl#L20)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。
