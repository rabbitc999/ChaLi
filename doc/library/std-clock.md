# std::clock — 单调时钟

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-clock.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::clock;
```

## 公共约定与错误边界

采用宿主 steady_clock。读数的起点由平台决定，只能用于差值与计时，不代表日期时间；单位分别为毫秒和纳秒，实际分辨率由平台决定。Duration 表示非负时长，Instant 表示单调时刻。截止时间的 remaining 返回零或剩余时长，toMillisCeil 向上取整以适配毫秒超时接口。

## 参考代码

完整程序：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

```powershell
chali run example/samples/std_math_clock_random.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
/* std_math_clock_random.chl —— 第二档：Math 补齐、Clock、Random 扩展。
 *
 * 三条路线各不相同，改这块前先分清：
 *   * Math 的 float64 取整 / 判定走**宿主胶水**（glue/std_math.hpp 绑 <cmath>）；
 *   * Math 的 int64 版是**ChaLi 重载转发到宿主胶水**（写在同一模块里，实参完全匹配优先于 int64→float64 放宽）；
 *   * Clock 是宿主胶水（chrono 的 steady_clock）；
 *   * Random 的 nextRange / nextBool 是 宿主胶水的薄封装。
 *
 * `choice` / `shuffle` 不在 `Random::` 下：ChaLi 没有泛型方法，而 `std::random::Random` 这个
 * 名字已经被同名的 struct 占着（再开同名模块会让 `Random::withSeed` 找不到）。
 * 它们放在 `algorithms`，导入后写 `choiceValue(表, 种子)`。
 */

import std::random;

import std::math;
import std::algorithms;

import std::clock;

void main() {
    /* --- Math：float64 取整 / 取余 / 特殊值判定（宿主 <cmath>） --- */
    println("{} {} {} {}", floor(2.7), ceil(2.1),
    round(2.5), trunc(-2.7));
    println("{} {}", fmod(7.5, 2.0), fmod(-7.5, 2.0));

    /* ChaLi 里浮点除零是运行期错误，所以 NaN / Inf 从宿主函数拿：
     * sqrt(-1) = NaN、ln(0) = -Inf。 */
    float64 notNumber = sqrt(-1.0);
    float64 infinite = ln(0.0);
    println("{} {} {} {}", isNaN(notNumber), isNaN(1.0),
    isInf(infinite), isInf(1.0));

    /* --- Math：int64 版（ChaLi 重载转发到宿主胶水）+ float64 版 abs 仍然可用 --- */
    println("{} {} {} {}", abs(-5), min(3, 8),
    max(3, 8), clamp(12, 0, 10));
    println("{}", abs(-2.5));

    /* --- Clock：单调时钟。读数本身不可复现，只断言"拿到了、且不倒退"。 --- */
    int64 before = nowNanos();
    int64 millis = nowMs();
    int64 after = nowNanos();
    println("{} {} {}", before > 0, millis > 0, after >= before);

    /* --- Random：分布 --- */
    Random rng = Random::withSeed((uint64)12345);
    float64 ranged = rng.nextRange(10.0, 20.0);
    int64 drawn = rng.nextInt(0, 10);
    bool coin = rng.nextBool();
    /* 固定种子 -> 固定序列（和 std_parse_random_path_memory 一个口径：钉住随机输出）。 */
    println("{} {} {}", ranged, drawn, coin);
    println("{} {}", ranged >= 10.0, ranged < 20.0);

    /* --- 取一个 / 洗一副（种子自己给，结果可复现） --- */
    List<int64> items;
    items.push(1);
    items.push(2);
    items.push(3);
    /* 种子 0：`0 % 3 == 0`，无论随机数怎么算都取到第 0 个。 */
    println("{}", choiceValue(items, (uint64)0));

    List<int64> mixed = shuffleValue(items, (uint64)4242);
    println("{} {} {}", mixed.len(),
    mixed[0] + mixed[1] + mixed[2],   /* 洗牌不改总和 */
    items[0]);                        /* 原表不动 */

    List<int64> again = shuffleValue(items, (uint64)4242);
    println("{} {} {}", again[0] == mixed[0], again[1] == mixed[1],
    again[2] == mixed[2]);
}
```

</details>

<details>
<summary>示例输出</summary>

```text
2.00000000 3.00000000 3.00000000 -2.00000000
1.50000000 -1.50000000
1 0 1 0
5 3 8 10
2.50000000
1 1 1
11.33079669 7 0
1 1
1
3 6 1
1 1 1
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-27"></a>`struct Duration` | 以纳秒计的非负时长。默认值为零。 |
| 模块 | <a id="symbol-28"></a>`struct Instant` | 单调时钟上的时刻，用于计算耗时和截止时间。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [nowMs](#api-0) | `int64 nowMs()` |
| [nowNanos](#api-1) | `int64 nowNanos()` |
| [Duration::fromNanos](#api-2) | `static Duration fromNanos(int64 nanos)` |
| [Duration::fromMillis](#api-3) | `static Duration fromMillis(int64 millis)` |
| [Duration::toNanos](#api-4) | `int64 toNanos()` |
| [Duration::toMillisCeil](#api-5) | `int64 toMillisCeil()` |
| [Instant::now](#api-6) | `static Instant now()` |
| [Instant::plus](#api-7) | `Instant plus(Duration duration)` |
| [Instant::durationSince](#api-8) | `Duration durationSince(Instant earlier)` |
| [Instant::elapsed](#api-9) | `Duration elapsed()` |
| [Instant::remaining](#api-10) | `Duration remaining()` |

<a id="api-0"></a>

### nowMs（1）

**声明**

```chali
int64 nowMs();
```

**行为**

读取单调时钟毫秒计数。

单调时钟：`steady_clock`，不受系统时间调整影响，测速 / 基准用。 基准点由平台定，只能拿两次读数做差（不要当成挂钟时间）。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取单调时钟毫秒计数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
nowMs();
```

**源码**：[source/std/clock.chl:10](../../source/std/clock.chl#L10)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-1"></a>

### nowNanos（2）

**声明**

```chali
int64 nowNanos();
```

**行为**

读取单调时钟纳秒计数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取单调时钟纳秒计数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
nowNanos();
```

**源码**：[source/std/clock.chl:11](../../source/std/clock.chl#L11)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-2"></a>

### Duration::fromNanos（3）

**声明**

```chali
static Duration fromNanos(int64 nanos);
```

**行为**

用非负纳秒数构造时长。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `nanos` | `int64 nanos` | 非负的纳秒数。 | 无 |

**返回值**

`Duration`。指定时长。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`nanos >= 0, "Duration requires nonnegative nanoseconds"`。

**调用形式**

```chali
Duration::fromNanos(nanos);
```

**源码**：[source/std/clock.chl:21](../../source/std/clock.chl#L21)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-3"></a>

### Duration::fromMillis（4）

**声明**

```chali
static Duration fromMillis(int64 millis);
```

**行为**

用非负毫秒数构造时长。超出纳秒表示范围时触发断言。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `millis` | `int64 millis` | 非负的毫秒数。 | 无 |

**返回值**

`Duration`。指定时长。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`millis >= 0 && millis <= 9223372036854, "Duration milliseconds out of range"`。

**调用形式**

```chali
Duration::fromMillis(millis);
```

**源码**：[source/std/clock.chl:32](../../source/std/clock.chl#L32)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-4"></a>

### Duration::toNanos（5）

**声明**

```chali
int64 toNanos();
```

**行为**

返回精确的纳秒数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回精确的纳秒数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toNanos();
```

**源码**：[source/std/clock.chl:38](../../source/std/clock.chl#L38)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-5"></a>

### Duration::toMillisCeil（6）

**声明**

```chali
int64 toMillisCeil();
```

**行为**

向上取整到毫秒，适用于以整毫秒接收超时的接口。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。向上取整到毫秒，适用于以整毫秒接收超时的接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toMillisCeil();
```

**源码**：[source/std/clock.chl:41](../../source/std/clock.chl#L41)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-6"></a>

### Instant::now（7）

**声明**

```chali
static Instant now();
```

**行为**

取得当前单调时钟读数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Instant`。取得当前单调时钟读数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Instant::now();
```

**源码**：[source/std/clock.chl:53](../../source/std/clock.chl#L53)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-7"></a>

### Instant::plus（8）

**声明**

```chali
Instant plus(Duration duration);
```

**行为**

从此时刻向后推移，超出范围时饱和到最大时刻。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `duration` | `Duration duration` | 向后推移的时长。 | 无 |

**返回值**

`Instant`。推移后的单调时刻。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.plus(duration);
```

**源码**：[source/std/clock.chl:63](../../source/std/clock.chl#L63)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-8"></a>

### Instant::durationSince（9）

**声明**

```chali
Duration durationSince(Instant earlier);
```

**行为**

计算从 earlier 到此时刻的时长，反向区间返回零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `earlier` | `Instant earlier` | 用作差值起点的单调时刻。 | 无 |

**返回值**

`Duration`。两个时刻之间的非负时长。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.durationSince(earlier);
```

**源码**：[source/std/clock.chl:77](../../source/std/clock.chl#L77)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-9"></a>

### Instant::elapsed（10）

**声明**

```chali
Duration elapsed();
```

**行为**

计算从此时刻到现在的耗时。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Duration`。计算从此时刻到现在的耗时。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.elapsed();
```

**源码**：[source/std/clock.chl:83](../../source/std/clock.chl#L83)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

<a id="api-10"></a>

### Instant::remaining（11）

**声明**

```chali
Duration remaining();
```

**行为**

计算到此截止时刻的剩余时长，过期时返回零。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Duration`。计算到此截止时刻的剩余时长，过期时返回零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remaining();
```

**源码**：[source/std/clock.chl:86](../../source/std/clock.chl#L86)。

**完整示例**：[example/samples/std_math_clock_random.chl](../../example/samples/std_math_clock_random.chl)。

## 补充完整示例

[example/samples/std_monotonic_time.chl](../../example/samples/std_monotonic_time.chl)。

```chali
import std::clock;

void main() {
    Duration zero;
    assert(zero.toNanos() == 0);
    assert(Duration::fromNanos(1).toMillisCeil() == 1);
    assert(Duration::fromNanos(1000001).toMillisCeil() == 2);
    assert(Duration::fromMillis(2).toMillisCeil() == 2);
    let maximum = Duration::fromNanos(9223372036854775807);
    assert(maximum.toMillisCeil() == 9223372036855);

    let start = Instant::now();
    let end = start.plus(Duration::fromMillis(7));
    assert(end.durationSince(start).toNanos() == 7000000);
    assert(start.durationSince(end).toNanos() == 0);
    assert(start.remaining().toNanos() == 0);
    let distant = start.plus(maximum);
    assert(distant.plus(Duration::fromMillis(1)).durationSince(distant).toNanos() == 0);
    assert(start.elapsed().toNanos() >= 0);
    println("monotonic time OK");
}
```
