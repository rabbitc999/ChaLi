# std::numerics — 数值分析

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```magel
import std::numerics;
```

## 公共约定与错误边界

回调应满足算法的连续性/可微性条件。积分子区间数、导数步长、误差容限、最大迭代次数及二分区间由宿主校验。二分需端点异号或端点为根；ODE 输出包含初值和后续 steps 个点。数值结果是近似值，不保证任意模型收敛。 derivative/rk4Step 的 step>0；Simpson 的 intervals 为正偶数；ODE 的 steps>=0。二分要求 left<right、tolerance>0、maxIterations>0。

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
| 模块 | <a id="symbol-292"></a>`struct Numerics` | 数值计算算法的静态入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [numericsDerivative](#api-0) | `float64 numericsDerivative(float64 fn(float64) function, float64 x, float64 step)` |
| [numericsIntegrateSimpson](#api-1) | `float64 numericsIntegrateSimpson(float64 fn(float64) function, float64 begin, float64 end, int64 intervals)` |
| [numericsBisect](#api-2) | `float64 numericsBisect(float64 fn(float64) function, float64 left, float64 right, float64 tolerance, int64 maxIterations)` |
| [numericsRk4Step](#api-3) | `float64 numericsRk4Step(float64 fn(float64, float64) derivative, float64 time, float64 value, float64 step)` |
| [numericsNewton](#api-4) | `float64 numericsNewton(float64 fn(float64) function, float64 initial, float64 tolerance, int64 maxIterations)` |
| [numericsSolveOdeEuler](#api-5) | `std::lang::List<float64> numericsSolveOdeEuler( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |
| [numericsSolveOdeRK4](#api-6) | `std::lang::List<float64> numericsSolveOdeRK4( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |
| [Numerics::derivative](#api-7) | `static float64 derivative(float64 fn(float64) function, float64 x, float64 step)` |
| [Numerics::integrateSimpson](#api-8) | `static float64 integrateSimpson(float64 fn(float64) function, float64 begin, float64 end, int64 intervals)` |
| [Numerics::bisect](#api-9) | `static float64 bisect(float64 fn(float64) function, float64 left, float64 right, float64 tolerance, int64 maxIterations)` |
| [Numerics::rk4Step](#api-10) | `static float64 rk4Step(float64 fn(float64, float64) derivative, float64 time, float64 value, float64 step)` |
| [Numerics::newton](#api-11) | `static float64 newton(float64 fn(float64) function, float64 initial, float64 tolerance, int64 maxIterations)` |
| [Numerics::solveOdeEuler](#api-12) | `static std::lang::List<float64> solveOdeEuler( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |
| [Numerics::solveOdeRK4](#api-13) | `static std::lang::List<float64> solveOdeRK4( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps)` |

<a id="api-0"></a>

### numericsDerivative（1）

**声明**

```magel
float64 numericsDerivative(float64 fn(float64) function, float64 x, float64 step);
```

**行为**

宿主入口：用中心差分近似 function 在 x 的导数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `x` | `float64 x` | 输入横坐标/标量 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |

**返回值**

`float64`。宿主入口：用中心差分近似 function 在 x 的导数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsDerivative(function, x, step);
```

**源码**：[source/std/numerics.mgl:7](../../source/std/numerics.mgl#L7)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-1"></a>

### numericsIntegrateSimpson（2）

**声明**

```magel
float64 numericsIntegrateSimpson(float64 fn(float64) function, float64 begin, float64 end, int64 intervals);
```

**行为**

宿主入口：用复合 Simpson 法近似定积分。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `begin` | `float64 begin` | 积分起点 | 无 |
| `end` | `float64 end` | 积分终点 | 无 |
| `intervals` | `int64 intervals` | 积分子区间数 | 无 |

**返回值**

`float64`。宿主入口：用复合 Simpson 法近似定积分。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsIntegrateSimpson(function, begin, end, intervals);
```

**源码**：[source/std/numerics.mgl:8](../../source/std/numerics.mgl#L8)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-2"></a>

### numericsBisect（3）

**声明**

```magel
float64 numericsBisect(float64 fn(float64) function, float64 left, float64 right, float64 tolerance, int64 maxIterations);
```

**行为**

宿主入口：在括根区间内二分求根。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `left` | `float64 left` | 左操作数或区间左端 | 无 |
| `right` | `float64 right` | 右操作数或区间右端 | 无 |
| `tolerance` | `float64 tolerance` | 收敛误差容限 | 无 |
| `maxIterations` | `int64 maxIterations` | 最大迭代次数 | 无 |

**返回值**

`float64`。宿主入口：在括根区间内二分求根。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsBisect(function, left, right, tolerance, maxIterations);
```

**源码**：[source/std/numerics.mgl:10](../../source/std/numerics.mgl#L10)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-3"></a>

### numericsRk4Step（4）

**声明**

```magel
float64 numericsRk4Step(float64 fn(float64, float64) derivative, float64 time, float64 value, float64 step);
```

**行为**

宿主入口：执行一次四阶 Runge-Kutta 更新。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `derivative` | `float64 fn(float64, float64) derivative` | 导数或变化率回调 | 无 |
| `time` | `float64 time` | 当前时间，秒 | 无 |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |

**返回值**

`float64`。宿主入口：执行一次四阶 Runge-Kutta 更新。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsRk4Step(derivative, time, value, step);
```

**源码**：[source/std/numerics.mgl:12](../../source/std/numerics.mgl#L12)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-4"></a>

### numericsNewton（5）

**声明**

```magel
float64 numericsNewton(float64 fn(float64) function, float64 initial, float64 tolerance, int64 maxIterations);
```

**行为**

宿主入口：从 initial 开始迭代 Newton 求根。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `initial` | `float64 initial` | 迭代/累积初值 | 无 |
| `tolerance` | `float64 tolerance` | 收敛误差容限 | 无 |
| `maxIterations` | `int64 maxIterations` | 最大迭代次数 | 无 |

**返回值**

`float64`。宿主入口：从 initial 开始迭代 Newton 求根。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsNewton(function, initial, tolerance, maxIterations);
```

**源码**：[source/std/numerics.mgl:14](../../source/std/numerics.mgl#L14)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-5"></a>

### numericsSolveOdeEuler（6）

**声明**

```magel
std::lang::List<float64> numericsSolveOdeEuler( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps);
```

**行为**

宿主入口：用 Euler 法求解标量初值问题并返回采样列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rate` | `float64 fn(float64, float64) rate` | 变化率回调 | 无 |
| `initialTime` | `float64 initialTime` | 初始时间 | 无 |
| `initialValue` | `float64 initialValue` | 初始状态值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |
| `steps` | `int64 steps` | 更新步数 | 无 |

**返回值**

`std::lang::List<float64>`。宿主入口：用 Euler 法求解标量初值问题并返回采样列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsSolveOdeEuler(rate, initialTime, initialValue, step, steps);
```

**源码**：[source/std/numerics.mgl:16](../../source/std/numerics.mgl#L16)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-6"></a>

### numericsSolveOdeRK4（7）

**声明**

```magel
std::lang::List<float64> numericsSolveOdeRK4( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps);
```

**行为**

宿主入口：用四阶 RK 法求解标量初值问题并返回采样列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rate` | `float64 fn(float64, float64) rate` | 变化率回调 | 无 |
| `initialTime` | `float64 initialTime` | 初始时间 | 无 |
| `initialValue` | `float64 initialValue` | 初始状态值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |
| `steps` | `int64 steps` | 更新步数 | 无 |

**返回值**

`std::lang::List<float64>`。宿主入口：用四阶 RK 法求解标量初值问题并返回采样列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::numericsSolveOdeRK4(rate, initialTime, initialValue, step, steps);
```

**源码**：[source/std/numerics.mgl:19](../../source/std/numerics.mgl#L19)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-7"></a>

### Numerics::derivative（8）

**声明**

```magel
static float64 derivative(float64 fn(float64) function, float64 x, float64 step);
```

**行为**

用中心差分近似 function 在 x 的导数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `x` | `float64 x` | 输入横坐标/标量 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |

**返回值**

`float64`。用中心差分近似 function 在 x 的导数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::derivative(function, x, step);
```

**源码**：[source/std/numerics.mgl:24](../../source/std/numerics.mgl#L24)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-8"></a>

### Numerics::integrateSimpson（9）

**声明**

```magel
static float64 integrateSimpson(float64 fn(float64) function, float64 begin, float64 end, int64 intervals);
```

**行为**

用复合 Simpson 法近似定积分。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `begin` | `float64 begin` | 积分起点 | 无 |
| `end` | `float64 end` | 积分终点 | 无 |
| `intervals` | `int64 intervals` | 积分子区间数 | 无 |

**返回值**

`float64`。用复合 Simpson 法近似定积分。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::integrateSimpson(function, begin, end, intervals);
```

**源码**：[source/std/numerics.mgl:27](../../source/std/numerics.mgl#L27)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-9"></a>

### Numerics::bisect（10）

**声明**

```magel
static float64 bisect(float64 fn(float64) function, float64 left, float64 right, float64 tolerance, int64 maxIterations);
```

**行为**

在括根区间内二分求根。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `left` | `float64 left` | 左操作数或区间左端 | 无 |
| `right` | `float64 right` | 右操作数或区间右端 | 无 |
| `tolerance` | `float64 tolerance` | 收敛误差容限 | 无 |
| `maxIterations` | `int64 maxIterations` | 最大迭代次数 | 无 |

**返回值**

`float64`。在括根区间内二分求根。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::bisect(function, left, right, tolerance, maxIterations);
```

**源码**：[source/std/numerics.mgl:31](../../source/std/numerics.mgl#L31)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-10"></a>

### Numerics::rk4Step（11）

**声明**

```magel
static float64 rk4Step(float64 fn(float64, float64) derivative, float64 time, float64 value, float64 step);
```

**行为**

执行一次四阶 Runge-Kutta 更新。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `derivative` | `float64 fn(float64, float64) derivative` | 导数或变化率回调 | 无 |
| `time` | `float64 time` | 当前时间，秒 | 无 |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |

**返回值**

`float64`。执行一次四阶 Runge-Kutta 更新。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::rk4Step(derivative, time, value, step);
```

**源码**：[source/std/numerics.mgl:35](../../source/std/numerics.mgl#L35)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-11"></a>

### Numerics::newton（12）

**声明**

```magel
static float64 newton(float64 fn(float64) function, float64 initial, float64 tolerance, int64 maxIterations);
```

**行为**

从 initial 开始迭代 Newton 求根。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `function` | `float64 fn(float64) function` | 数值算法回调 | 无 |
| `initial` | `float64 initial` | 迭代/累积初值 | 无 |
| `tolerance` | `float64 tolerance` | 收敛误差容限 | 无 |
| `maxIterations` | `int64 maxIterations` | 最大迭代次数 | 无 |

**返回值**

`float64`。从 initial 开始迭代 Newton 求根。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::newton(function, initial, tolerance, maxIterations);
```

**源码**：[source/std/numerics.mgl:39](../../source/std/numerics.mgl#L39)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-12"></a>

### Numerics::solveOdeEuler（13）

**声明**

```magel
static std::lang::List<float64> solveOdeEuler( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps);
```

**行为**

用 Euler 法求解标量初值问题并返回采样列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rate` | `float64 fn(float64, float64) rate` | 变化率回调 | 无 |
| `initialTime` | `float64 initialTime` | 初始时间 | 无 |
| `initialValue` | `float64 initialValue` | 初始状态值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |
| `steps` | `int64 steps` | 更新步数 | 无 |

**返回值**

`std::lang::List<float64>`。用 Euler 法求解标量初值问题并返回采样列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::solveOdeEuler(rate, initialTime, initialValue, step, steps);
```

**源码**：[source/std/numerics.mgl:44](../../source/std/numerics.mgl#L44)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。

<a id="api-13"></a>

### Numerics::solveOdeRK4（14）

**声明**

```magel
static std::lang::List<float64> solveOdeRK4( float64 fn(float64, float64) rate, float64 initialTime, float64 initialValue, float64 step, int64 steps);
```

**行为**

用四阶 RK 法求解标量初值问题并返回采样列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rate` | `float64 fn(float64, float64) rate` | 变化率回调 | 无 |
| `initialTime` | `float64 initialTime` | 初始时间 | 无 |
| `initialValue` | `float64 initialValue` | 初始状态值 | 无 |
| `step` | `float64 step` | 步长或时间增量 | 无 |
| `steps` | `int64 steps` | 更新步数 | 无 |

**返回值**

`std::lang::List<float64>`。用四阶 RK 法求解标量初值问题并返回采样列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::numerics::Numerics::solveOdeRK4(rate, initialTime, initialValue, step, steps);
```

**源码**：[source/std/numerics.mgl:49](../../source/std/numerics.mgl#L49)。

**完整示例**：[example/samples/std_numerics_number_theory.mgl](../../example/samples/std_numerics_number_theory.mgl)。
