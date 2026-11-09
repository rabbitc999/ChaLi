# std::date_time — UTC 日期时间

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-date_time.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::date_time;
```

## 公共约定与错误边界

Unix 秒与公历 UTC 日期换算，年份 1..9999，秒 0..59，不表示闰秒。ISO 8601 首版只接受 YYYY-MM-DDTHH:MM:SSZ。纯换算不依赖平台时区；nowUtc 要求宿主时钟。

## 参考代码

完整程序：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

```powershell
chali run example/samples/std_datetime.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::date_time;
void main(){
    assert(formatUtc(fromUnixSeconds(0).unwrap()).unwrap()=="1970-01-01T00:00:00Z");
    assert(formatUtc(fromUnixSeconds(-1).unwrap()).unwrap()=="1969-12-31T23:59:59Z");
    DateTime leap=parseUtc("2024-02-29T12:34:56Z").unwrap();
    assert(toUnixSeconds(leap).unwrap()==1709210096);
    assert(formatUtc(addDays(leap,1).unwrap()).unwrap()=="2024-03-01T12:34:56Z");
    assert(formatUtc(addSeconds(leap,-1).unwrap()).unwrap()=="2024-02-29T12:34:55Z");
    assert(!parseUtc("1900-02-29T00:00:00Z").isOk() && parseUtc("2000-02-29T00:00:00Z").isOk());
    assert(!parseUtc("2024-01-01T24:00:00Z").isOk() && !parseUtc("2024-01-01T00:00:60Z").isOk());
    assert(!parseUtc("2024-01-01T00:00:00+08:00").isOk());
    assert(toUnixSeconds(parseUtc("0001-01-01T00:00:00Z").unwrap()).unwrap()== -62135596800);
    assert(toUnixSeconds(parseUtc("9999-12-31T23:59:59Z").unwrap()).unwrap()==253402300799);
    assert(!fromUnixSeconds(253402300800).isOk());
    List<int64> inputs1;inputs1.push(-62135596800);inputs1.push(-2208988800);inputs1.push(-86401);inputs1.push(-1);inputs1.push(0);inputs1.push(86400);inputs1.push(1709210096);inputs1.push(253402300799);for(int64 seconds in inputs1){
        assert(toUnixSeconds(fromUnixSeconds(seconds).unwrap()).unwrap()==seconds);
    }
    println("UTC dates, epochs and arithmetic OK");
}
```

</details>

<details>
<summary>示例输出</summary>

```text
UTC dates, epochs and arithmetic OK
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-140"></a>`struct DateTime` | UTC 民用时间，使用公历；有效年份为 1 到 9999，不表示闰秒。 |
| DateTime | <a id="symbol-141"></a>`int64 year` | 公历年份。 |
| DateTime | <a id="symbol-142"></a>`int64 month` | 月份，1–12。 |
| DateTime | <a id="symbol-143"></a>`int64 day` | 当月日期，从 1 开始。 |
| DateTime | <a id="symbol-144"></a>`int64 hour` | 小时，0–23。 |
| DateTime | <a id="symbol-145"></a>`int64 minute` | 分钟，0–59。 |
| DateTime | <a id="symbol-146"></a>`int64 second` | 秒，0–59，不表示闰秒。 |
| 模块 | <a id="symbol-147"></a>`enum DateTimeError : Error` | 日期、时间、格式、可表示范围或宿主时钟不可用的错误。 |
| DateTimeError | `InvalidDate, InvalidTime, InvalidFormat, OutOfRange, ClockUnavailable` | 日期、时间、格式、可表示范围或宿主时钟不可用的错误。 变体及载荷见声明。 |
| 模块 | <a id="symbol-148"></a>`struct DateTimeOffset` | 民用日期与显式 UTC 偏移；正偏移表示当地时间快于 UTC。 |
| DateTimeOffset | <a id="symbol-149"></a>`DateTime local` | 偏移后的民用日期和钟表时间。 |
| DateTimeOffset | <a id="symbol-150"></a>`int64 offset_minutes` | 当地时间相对 UTC 的有符号分钟数。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [DateTimeError::toString](#api-0) | `String toString()` |
| [isLeapYear](#api-1) | `bool isLeapYear(int64 year)` |
| [daysInMonth](#api-2) | `int64 daysInMonth(int64 year, int64 month)` |
| [toUnixSeconds](#api-3) | `Result<int64,Error> toUnixSeconds(DateTime value)` |
| [fromUnixSeconds](#api-4) | `Result<DateTime,Error> fromUnixSeconds(int64 seconds)` |
| [dateDigits](#api-5) | `String dateDigits(int64 value, int64 width)` |
| [formatUtc](#api-6) | `Result<String,Error> formatUtc(DateTime value)` |
| [parseUtc](#api-7) | `Result<DateTime,Error> parseUtc(String text)` |
| [addSeconds](#api-8) | `Result<DateTime,Error> addSeconds(DateTime value, int64 delta)` |
| [addDays](#api-9) | `Result<DateTime,Error> addDays(DateTime value, int64 days)` |
| [nowUtc](#api-10) | `Result<DateTime,Error> nowUtc()` |
| [DateTimeOffset::utc](#api-11) | `Result<DateTime,Error> utc()` |
| [atOffset](#api-12) | `Result<DateTimeOffset,Error> atOffset(DateTime utc, int64 offset_minutes)` |
| [formatOffset](#api-13) | `Result<String,Error> formatOffset(DateTimeOffset value)` |
| [parseOffset](#api-14) | `Result<DateTimeOffset,Error> parseOffset(String text)` |

<a id="api-0"></a>

### DateTimeError::toString（1）

**声明**

```chali
String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

返回具体错误原因，并保留载荷中的位置或宿主诊断。

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

**源码**：[source/std/date_time.chl:9](../../source/std/date_time.chl#L9)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-1"></a>

### isLeapYear（2）

**声明**

```chali
bool isLeapYear(int64 year);
```

**行为**

公历闰年规则。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `year` | `int64 year` | 公历年份。 | 无 |

**返回值**

`bool`。公历闰年规则。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
isLeapYear(year);
```

**源码**：[source/std/date_time.chl:20](../../source/std/date_time.chl#L20)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-2"></a>

### daysInMonth（3）

**声明**

```chali
int64 daysInMonth(int64 year, int64 month);
```

**行为**

返回月份天数；非法月份返回零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `year` | `int64 year` | 公历年份。 | 无 |
| `month` | `int64 month` | 月份，1 到 12。 | 无 |

**返回值**

`int64`。返回月份天数；非法月份返回零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
daysInMonth(year, month);
```

**源码**：[source/std/date_time.chl:22](../../source/std/date_time.chl#L22)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-3"></a>

### toUnixSeconds（4）

**声明**

```chali
Result<int64,Error> toUnixSeconds(DateTime value);
```

**行为**

将 UTC 日期转换为 Unix 秒；不依赖宿主时区。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `DateTime value` | 输入值或写入的新值 | 无 |

**返回值**

`Result<int64,Error>`。将 UTC 日期转换为 Unix 秒；不依赖宿主时区。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
toUnixSeconds(value);
```

**源码**：[source/std/date_time.chl:29](../../source/std/date_time.chl#L29)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-4"></a>

### fromUnixSeconds（5）

**声明**

```chali
Result<DateTime,Error> fromUnixSeconds(int64 seconds);
```

**行为**

将 Unix 秒转换为 UTC 日期，负数表示 1970 年之前。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `seconds` | `int64 seconds` | Unix 秒数。 | 无 |

**返回值**

`Result<DateTime,Error>`。将 Unix 秒转换为 UTC 日期，负数表示 1970 年之前。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fromUnixSeconds(seconds);
```

**源码**：[source/std/date_time.chl:44](../../source/std/date_time.chl#L44)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-5"></a>

### dateDigits（6）

**声明**

```chali
String dateDigits(int64 value, int64 width);
```

**行为**

输出固定位数的非负十进制字段。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |
| `width` | `int64 width` | 目标字节宽度或定宽字段大小 | 无 |

**返回值**

`String`。输出固定位数的非负十进制字段。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
dateDigits(value, width);
```

**源码**：[source/std/date_time.chl:58](../../source/std/date_time.chl#L58)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-6"></a>

### formatUtc（7）

**声明**

```chali
Result<String,Error> formatUtc(DateTime value);
```

**行为**

输出严格的 YYYY-MM-DDTHH:MM:SSZ 文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `DateTime value` | 输入值或写入的新值 | 无 |

**返回值**

`Result<String,Error>`。输出严格的 YYYY-MM-DDTHH:MM:SSZ 文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
formatUtc(value);
```

**源码**：[source/std/date_time.chl:66](../../source/std/date_time.chl#L66)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-7"></a>

### parseUtc（8）

**声明**

```chali
Result<DateTime,Error> parseUtc(String text);
```

**行为**

解析 UTC ISO 8601 秒精度文本，不接受时区偏移或隐式本地时区。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<DateTime,Error>`。解析 UTC ISO 8601 秒精度文本，不接受时区偏移或隐式本地时区。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
parseUtc(text);
```

**源码**：[source/std/date_time.chl:72](../../source/std/date_time.chl#L72)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-8"></a>

### addSeconds（9）

**声明**

```chali
Result<DateTime,Error> addSeconds(DateTime value, int64 delta);
```

**行为**

在 UTC 时间上增加秒数，越界返回错误，不发生整数回绕。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `DateTime value` | 输入值或写入的新值 | 无 |
| `delta` | `int64 delta` | 原子累加的 int64 增量 | 无 |

**返回值**

`Result<DateTime,Error>`。在 UTC 时间上增加秒数，越界返回错误，不发生整数回绕。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
addSeconds(value, delta);
```

**源码**：[source/std/date_time.chl:94](../../source/std/date_time.chl#L94)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-9"></a>

### addDays（10）

**声明**

```chali
Result<DateTime,Error> addDays(DateTime value, int64 days);
```

**行为**

增加固定 86400 秒的一天，适用于 UTC。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `DateTime value` | 输入值或写入的新值 | 无 |
| `days` | `int64 days` | UTC 日期偏移天数。 | 无 |

**返回值**

`Result<DateTime,Error>`。增加固定 86400 秒的一天，适用于 UTC。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
addDays(value, days);
```

**源码**：[source/std/date_time.chl:100](../../source/std/date_time.chl#L100)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-10"></a>

### nowUtc（11）

**声明**

```chali
Result<DateTime,Error> nowUtc();
```

**行为**

读取宿主当前 UTC 时间；无时钟宿主应报告不可用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<DateTime,Error>`。读取宿主当前 UTC 时间；无时钟宿主应报告不可用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
nowUtc();
```

**源码**：[source/std/date_time.chl:107](../../source/std/date_time.chl#L107)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-11"></a>

### DateTimeOffset::utc（12）

**声明**

```chali
Result<DateTime,Error> utc();
```

**行为**

返回对应 UTC 时间；检查偏移、日期和公历边界。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<DateTime,Error>`。返回对应 UTC 时间；检查偏移、日期和公历边界。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.utc();
```

**源码**：[source/std/date_time.chl:122](../../source/std/date_time.chl#L122)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-12"></a>

### atOffset（13）

**声明**

```chali
Result<DateTimeOffset,Error> atOffset(DateTime utc, int64 offset_minutes);
```

**行为**

按指定的 UTC 偏移表示同一时刻；偏移范围为 -14:00 到 +14:00。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `utc` | `DateTime utc` | UTC 民用日期和时间 | 无 |
| `offset_minutes` | `int64 offset_minutes` | 当地时间相对 UTC 的有符号分钟数 | 无 |

**返回值**

`Result<DateTimeOffset,Error>`。按指定的 UTC 偏移表示同一时刻；偏移范围为 -14:00 到 +14:00。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
atOffset(utc, offset_minutes);
```

**源码**：[source/std/date_time.chl:131](../../source/std/date_time.chl#L131)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-13"></a>

### formatOffset（14）

**声明**

```chali
Result<String,Error> formatOffset(DateTimeOffset value);
```

**行为**

输出秒精度 ISO 8601 文本，零偏移使用 Z。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `DateTimeOffset value` | 输入值或写入的新值 | 无 |

**返回值**

`Result<String,Error>`。输出秒精度 ISO 8601 文本，零偏移使用 Z。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
formatOffset(value);
```

**源码**：[source/std/date_time.chl:142](../../source/std/date_time.chl#L142)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。

<a id="api-14"></a>

### parseOffset（15）

**声明**

```chali
Result<DateTimeOffset,Error> parseOffset(String text);
```

**行为**

解析带 Z 或显式正负 HH:MM 偏移的秒精度 ISO 8601 文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<DateTimeOffset,Error>`。解析带 Z 或显式正负 HH:MM 偏移的秒精度 ISO 8601 文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
parseOffset(text);
```

**源码**：[source/std/date_time.chl:153](../../source/std/date_time.chl#L153)。

**完整示例**：[example/samples/std_datetime.chl](../../example/samples/std_datetime.chl)。
