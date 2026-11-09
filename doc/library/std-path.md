# std::path — 路径字符串

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-path.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::path;
```

## 公共约定与错误边界

只进行路径字符串处理，不创建文件、不验证目标存在；分隔符与规范化遵循宿主实现。不要把字符串规范化当作文件权限或符号链接安全校验。

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
| 模块 | <a id="symbol-384"></a>`struct PathString` | 路径文本组合与拆分的静态入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [PathString::join](#api-0) | `static String join(String left, String right)` |
| [PathString::fileName](#api-1) | `static String fileName(String path)` |
| [PathString::extension](#api-2) | `static String extension(String path)` |
| [PathString::normalize](#api-3) | `static String normalize(String path)` |
| [PathString::parent](#api-4) | `static String parent(String path)` |
| [PathString::stem](#api-5) | `static String stem(String path)` |

<a id="api-0"></a>

### PathString::join（1）

**声明**

```chali
static String join(String left, String right);
```

**行为**

将 left 与 right 组合为路径字符串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `String left` | 左操作数或区间左端 | 无 |
| `right` | `String right` | 右操作数或区间右端 | 无 |

**返回值**

`String`。将 left 与 right 组合为路径字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::join(left, right);
```

**源码**：[source/std/path.chl:130](../../source/std/path.chl#L130)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-1"></a>

### PathString::fileName（2）

**声明**

```chali
static String fileName(String path);
```

**行为**

返回路径末尾文件名。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`String`。返回路径末尾文件名。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::fileName(path);
```

**源码**：[source/std/path.chl:131](../../source/std/path.chl#L131)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-2"></a>

### PathString::extension（3）

**声明**

```chali
static String extension(String path);
```

**行为**

返回路径的扩展名。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`String`。返回路径的扩展名。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::extension(path);
```

**源码**：[source/std/path.chl:132](../../source/std/path.chl#L132)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-3"></a>

### PathString::normalize（4）

**声明**

```chali
static String normalize(String path);
```

**行为**

返回宿主规则下的规范化路径字符串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`String`。返回宿主规则下的规范化路径字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::normalize(path);
```

**源码**：[source/std/path.chl:133](../../source/std/path.chl#L133)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-4"></a>

### PathString::parent（5）

**声明**

```chali
static String parent(String path);
```

**行为**

返回创建当前任务的父线程 Thread 句柄。

返回归一化后的父路径；单段相对路径的父路径为点。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`String`。返回创建当前任务的父线程 Thread 句柄。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::parent(path);
```

**源码**：[source/std/path.chl:136](../../source/std/path.chl#L136)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。

<a id="api-5"></a>

### PathString::stem（6）

**声明**

```chali
static String stem(String path);
```

**行为**

返回末段名称去掉最后一个扩展名后的文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`String`。返回末段名称去掉最后一个扩展名后的文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
PathString::stem(path);
```

**源码**：[source/std/path.chl:152](../../source/std/path.chl#L152)。

**完整示例**：[example/samples/std_parse_random_path_memory.chl](../../example/samples/std_parse_random_path_memory.chl)。
