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

返回 Result<T,Error>，错误中保存 ParseError 的 Empty、Invalid、Overflow。整数按完整十进制解析，不接受前后空白或加号；有符号入口允许负号，无符号入口拒绝负号。数字前缀已经溢出时保留 Overflow 优先级。bool 只接受 true 或 false。整数与布尔算法在 ChaLi 层执行；float64 保留 C strtod 的格式规则，非有限值和范围错误报告 Overflow。不会静默返回零代替失败。

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

    Result<int64, Error> empty = Parse::parseInt64("");
    Result<int64, Error> invalid = Parse::parseInt64("4x");
    Result<int64, Error> overflow =
    Parse::parseInt64("999999999999999999999999");
    match (empty) {
        Result<int64, Error>::Ok(value) => { println("empty=bad"); }
        Result<int64, Error>::Err(error) => {
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
        Result<int64, Error>::Err(error) => {
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
        Result<int64, Error>::Err(error) => {
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
| 模块 | <a id="symbol-384"></a>`enum ParseError : Error` | 区分空输入、格式错误与数值范围错误，作为标准 Error 接口返回。 |
| ParseError | `Empty, Invalid, Overflow` | 区分空输入、格式错误与数值范围错误，作为标准 Error 接口返回。 变体及载荷见声明。 |
| 模块 | <a id="symbol-385"></a>`struct Parse` | 文本到数值和布尔值的解析入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [ParseError::toString](#api-0) | `String toString()` |
| [Parse::parseInt64](#api-1) | `static Result<int64, Error> parseInt64(String text)` |
| [Parse::parseUInt64](#api-2) | `static Result<uint64, Error> parseUInt64(String text)` |
| [Parse::parseFloat64](#api-3) | `static Result<float64, Error> parseFloat64(String text)` |
| [Parse::parseBool](#api-4) | `static Result<bool, Error> parseBool(String text)` |

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
static Result<int64, Error> parseInt64(String text);
```

**行为**

严格解析有符号 64 位十进制整数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<int64, Error>`。严格解析有符号 64 位十进制整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Parse::parseInt64(text);
```

**源码**：[source/std/parse.chl:48](../../source/std/parse.chl#L48)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-2"></a>

### Parse::parseUInt64（3）

**声明**

```chali
static Result<uint64, Error> parseUInt64(String text);
```

**行为**

严格解析无符号 64 位十进制整数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<uint64, Error>`。严格解析无符号 64 位十进制整数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Parse::parseUInt64(text);
```

**源码**：[source/std/parse.chl:59](../../source/std/parse.chl#L59)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-3"></a>

### Parse::parseFloat64（4）

**声明**

```chali
static Result<float64, Error> parseFloat64(String text);
```

**行为**

解析 64 位浮点数并报告非法输入/溢出。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<float64, Error>`。解析 64 位浮点数并报告非法输入/溢出。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Parse::parseFloat64(text);
```

**源码**：[source/std/parse.chl:64](../../source/std/parse.chl#L64)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-4"></a>

### Parse::parseBool（5）

**声明**

```chali
static Result<bool, Error> parseBool(String text);
```

**行为**

解析布尔文本并报告非法输入。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<bool, Error>`。解析布尔文本并报告非法输入。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Parse::parseBool(text);
```

**源码**：[source/std/parse.chl:74](../../source/std/parse.chl#L74)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。
