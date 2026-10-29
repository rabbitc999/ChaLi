# std::parse — 严格文本解析

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-parse.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::parse;
```

## 公共约定与错误边界

返回 Result<T,ParseError>，区分 Empty、Invalid、Overflow。不会静默返回零代替失败。整数的位宽与符号由具体函数确定；调用者处理 Err 后再取值。

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
    Result<int64, Error> integer = std::parse::Parse::parseInt64("-42");
    Result<uint64, Error> natural = std::parse::Parse::parseUInt64("18446744073709551615");
    Result<float64, Error> real = std::parse::Parse::parseFloat64("12.5");
    Result<bool, Error> flag = std::parse::Parse::parseBool("true");
    match (integer) {
        Result<int64, Error>::Ok(value) => { println("int64={}", value); }
        Result<int64, Error>::Err(error) => { println("int64=error"); }
    }
    match (natural) {
        Result<uint64, Error>::Ok(value) => { println("uint64={}", value); }
        Result<uint64, Error>::Err(error) => { println("uint64=error"); }
    }
    match (real) {
        Result<float64, Error>::Ok(value) => { println("float64={:1F}", value); }
        Result<float64, Error>::Err(error) => { println("float64=error"); }
    }
    match (flag) {
        Result<bool, Error>::Ok(value) => { println("bool={}", (int64)value); }
        Result<bool, Error>::Err(error) => { println("bool=error"); }
    }

    Result<int64, Error> empty = std::parse::Parse::parseInt64("");
    Result<int64, Error> invalid = std::parse::Parse::parseInt64("4x");
    Result<int64, Error> overflow =
    std::parse::Parse::parseInt64("999999999999999999999999");
    match (empty) {
        Result<int64, Error>::Ok(value) => { println("empty=bad"); }
        Result<int64, Error>::Err(error) => {
            match (error) {
                std::parse::ParseError(detail) => {
                    match(detail) {
                        std::parse::ParseError::Empty => { println("empty=ok"); }
                        _ => { println("empty=bad"); }
                    }
                }
                _ => { println("empty=bad"); }
            }
        }
    }
    match (invalid) {
        Result<int64, Error>::Ok(value) => { println("invalid=bad"); }
        Result<int64, Error>::Err(error) => {
            match (error) {
                std::parse::ParseError(detail) => {
                    match(detail) {
                        std::parse::ParseError::Invalid => { println("invalid=ok"); }
                        _ => { println("invalid=bad"); }
                    }
                }
                _ => { println("invalid=bad"); }
            }
        }
    }
    match (overflow) {
        Result<int64, Error>::Ok(value) => { println("overflow=bad"); }
        Result<int64, Error>::Err(error) => {
            match (error) {
                std::parse::ParseError(detail) => {
                    match(detail) {
                        std::parse::ParseError::Overflow => { println("overflow=ok"); }
                        _ => { println("overflow=bad"); }
                    }
                }
                _ => { println("overflow=bad"); }
            }
        }
    }

    std::random::Random first = std::random::Random::withSeed((uint64)7);
    std::random::Random second = std::random::Random::withSeed((uint64)7);
    uint64 firstValue = first.nextUInt64();
    uint64 secondValue = second.nextUInt64();
    assert(firstValue == secondValue);
    println("random={},{}", firstValue, first.nextInt(-10, 10));
    std::random::Random entropy = std::random::Random::fromEntropy();
    entropy.nextUInt64();

    String joined = std::path::PathString::join("alpha/beta", "../gamma.txt");
    String name = std::path::PathString::fileName("alpha/gamma.txt");
    String extension = std::path::PathString::extension("alpha/gamma.txt");
    String normalized = std::path::PathString::normalize("a\\b/./c/../d");
    println("path={}", joined);
    println("name={} ext={}", name, extension);
    println("normalized={}", normalized);

    int64 before = std::memory::Memory::stats().liveBytes;
    int64 during = before;
    {
        char bytes[] = new[16];
        std::memory::Span<char> view = std::memory::Span<char>::from(&bytes[0], 16);
        assert(view.len() == 16);
        during = std::memory::Memory::stats().liveBytes;
        assert(during >= before + 16);
    }
    int64 after = std::memory::Memory::stats().liveBytes;
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
| 模块 | <a id="symbol-348"></a>`enum ParseError : Error` | 从 lang.chl 原样迁出：符号名不变，只换了模块归属（现在必须 `import std::parse;` 才能用）。 |
| ParseError | `Empty, Invalid, Overflow` | 从 lang.chl 原样迁出：符号名不变，只换了模块归属（现在必须 `import std::parse;` 才能用）。 变体及载荷见声明。 |
| 模块 | <a id="symbol-349"></a>`struct Parse` | 文本到数值和布尔值的解析入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [ParseError::toString](#api-0) | `String toString()` |
| [Parse::parseInt64](#api-1) | `static Result<int64, std::lang::Error> parseInt64(std::lang::String text)` |
| [Parse::parseUInt64](#api-2) | `static Result<uint64, std::lang::Error> parseUInt64(std::lang::String text)` |
| [Parse::parseFloat64](#api-3) | `static Result<float64, std::lang::Error> parseFloat64(std::lang::String text)` |
| [Parse::parseBool](#api-4) | `static Result<bool, std::lang::Error> parseBool(std::lang::String text)` |

<a id="api-0"></a>

### ParseError::toString（1）

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

**源码**：[source/std/parse.chl:8](../../source/std/parse.chl#L8)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-1"></a>

### Parse::parseInt64（2）

**声明**

```chali
static Result<int64, std::lang::Error> parseInt64(std::lang::String text);
```

**行为**

严格解析有符号 64 位十进制整数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<int64, std::lang::Error>`。严格解析有符号 64 位十进制整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::parse::Parse::parseInt64(text);
```

**源码**：[source/std/parse.chl:27](../../source/std/parse.chl#L27)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-2"></a>

### Parse::parseUInt64（3）

**声明**

```chali
static Result<uint64, std::lang::Error> parseUInt64(std::lang::String text);
```

**行为**

严格解析无符号 64 位十进制整数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<uint64, std::lang::Error>`。严格解析无符号 64 位十进制整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::parse::Parse::parseUInt64(text);
```

**源码**：[source/std/parse.chl:37](../../source/std/parse.chl#L37)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-3"></a>

### Parse::parseFloat64（4）

**声明**

```chali
static Result<float64, std::lang::Error> parseFloat64(std::lang::String text);
```

**行为**

解析 64 位浮点数并报告非法输入/溢出。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<float64, std::lang::Error>`。解析 64 位浮点数并报告非法输入/溢出。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::parse::Parse::parseFloat64(text);
```

**源码**：[source/std/parse.chl:47](../../source/std/parse.chl#L47)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-4"></a>

### Parse::parseBool（5）

**声明**

```chali
static Result<bool, std::lang::Error> parseBool(std::lang::String text);
```

**行为**

解析布尔文本并报告非法输入。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<bool, std::lang::Error>`。解析布尔文本并报告非法输入。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::parse::Parse::parseBool(text);
```

**源码**：[source/std/parse.chl:57](../../source/std/parse.chl#L57)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。
