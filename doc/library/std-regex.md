# std::regex — 正则表达式

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-regex.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::regex;
```

## 公共约定与错误边界

Regex 是 opcode；复制共享不可变编译模式，每次匹配使用独立状态，最后一个值释放时回收模式。compile 失败返回 RegexError::Invalid。find 返回首个匹配的字节下标或 None。capture 的 group=0 表示完整匹配。语法和 replacement 捕获组替换规则采用宿主 std::regex 的默认 ECMAScript 规则。

## 参考代码

完整程序：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

```powershell
chali run example/samples/std_glue_edges.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::text;
import std::math;
void main() {
    List<String> parts = Text::split("::甲::::乙::", "::");
    println("{} [{}]", parts.len(), Text::join(parts, "|"));
    List<String> empty = Text::split("", ",");
    List<String> whole = Text::split("甲乙", "");
    List<String> none;
    println("{} [{}] [{}]", empty.len(), whole[0], Text::join(none, ","));
    parts[1] = "changed";
    println("[{}]", Text::join(parts, "|"));
    float64 notNumber = sqrt(-1.0);
    println("{} {} {}", min(notNumber, 2.0), isNaN(max(2.0, notNumber)), isNaN(clamp(notNumber, 0.0, 1.0)));
    int64 smallest = (int64)((uint64)1 << 63);
    println("{} {} {} {}", abs(smallest) == smallest, min(4, 3), max(-4.0, -3.0), clamp(-4, -3, 2));
}
```

</details>

<details>
<summary>示例输出</summary>

```text
5 [|甲||乙|]
1 [甲乙] []
[|changed||乙|]
2 1 1
1 3 -3 -3
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-446"></a>`enum RegexError : Error` | 复制共享不可变编译模式；匹配状态属于每次调用，最后一个值释放时回收模式。 |
| RegexError | `Invalid` | 复制共享不可变编译模式；匹配状态属于每次调用，最后一个值释放时回收模式。 变体及载荷见声明。 |
| 模块 | <a id="symbol-447"></a>`opcode Regex` | 共享不可变编译模式的 opcode，匹配状态按每次调用独立。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [RegexError::toString](#api-0) | `String toString()` |
| [Regex::compile](#api-1) | `static Result<Regex, Error> compile(String pattern)` |
| [Regex::fullMatch](#api-2) | `bool fullMatch(String text)` |
| [Regex::contains](#api-3) | `bool contains(String text)` |
| [Regex::find](#api-4) | `Option<int64> find(String text)` |
| [Regex::capture](#api-5) | `String capture(String text, int64 group)` |
| [Regex::replaceFirst](#api-6) | `String replaceFirst(String text, String replacement)` |
| [Regex::replaceAll](#api-7) | `String replaceAll(String text, String replacement)` |

<a id="api-0"></a>

### RegexError::toString（1）

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

**源码**：[source/std/regex.chl:9](../../source/std/regex.chl#L9)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-1"></a>

### Regex::compile（2）

**声明**

```chali
static Result<Regex, Error> compile(String pattern);
```

**行为**

编译正则模式；失败返回 Err(Invalid)。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `pattern` | `String pattern` | 正则模式 | 无 |

**返回值**

`Result<Regex, Error>`。编译正则模式；失败返回 Err(Invalid)。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
Regex::compile(pattern);
```

**源码**：[source/std/regex.chl:34](../../source/std/regex.chl#L34)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-2"></a>

### Regex::fullMatch（3）

**声明**

```chali
bool fullMatch(String text);
```

**行为**

判断整个 text 是否匹配模式。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`bool`。判断整个 text 是否匹配模式。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.fullMatch(text);
```

**源码**：[source/std/regex.chl:40](../../source/std/regex.chl#L40)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-3"></a>

### Regex::contains（4）

**声明**

```chali
bool contains(String text);
```

**行为**

判断是否包含目标值或匹配文本。

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

**源码**：[source/std/regex.chl:41](../../source/std/regex.chl#L41)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-4"></a>

### Regex::find（5）

**声明**

```chali
Option<int64> find(String text);
```

**行为**

查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Option<int64>`。查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.find(text);
```

**源码**：[source/std/regex.chl:42](../../source/std/regex.chl#L42)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-5"></a>

### Regex::capture（6）

**声明**

```chali
String capture(String text, int64 group);
```

**行为**

提取首次匹配的 group 捕获组。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |
| `group` | `int64 group` | 捕获组编号，0 为完整匹配 | 无 |

**返回值**

`String`。提取首次匹配的 group 捕获组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.capture(text, group);
```

**源码**：[source/std/regex.chl:47](../../source/std/regex.chl#L47)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-6"></a>

### Regex::replaceFirst（7）

**声明**

```chali
String replaceFirst(String text, String replacement);
```

**行为**

替换首次正则匹配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |
| `replacement` | `String replacement` | 正则替换文本 | 无 |

**返回值**

`String`。替换首次正则匹配。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.replaceFirst(text, replacement);
```

**源码**：[source/std/regex.chl:50](../../source/std/regex.chl#L50)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-7"></a>

### Regex::replaceAll（8）

**声明**

```chali
String replaceAll(String text, String replacement);
```

**行为**

替换全部正则匹配。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |
| `replacement` | `String replacement` | 正则替换文本 | 无 |

**返回值**

`String`。替换全部正则匹配。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.replaceAll(text, replacement);
```

**源码**：[source/std/regex.chl:53](../../source/std/regex.chl#L53)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。
