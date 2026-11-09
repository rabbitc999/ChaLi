# std::log — 结构化日志

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-log.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::log;
```

## 公共约定与错误边界

将级别、消息和字符串字段编码为 JSON 行，并可输出到标准错误。

## 参考代码

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-336"></a>`enum LogLevel` | 日志严重性。 |
| LogLevel | `Debug, Info, Warn, Error` | 日志严重性。 变体及载荷见声明。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [LogLevel::label](#api-0) | `String label()` |
| [formatRecord](#api-1) | `String formatRecord(LogLevel level, String message, Map<String,String> fields)` |
| [writeRecord](#api-2) | `void writeRecord(LogLevel level, String message, Map<String,String> fields)` |

<a id="api-0"></a>

### LogLevel::label（1）

**声明**

```chali
String label();
```

**行为**

取得稳定的 JSON 严重性名称。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。取得稳定的 JSON 严重性名称。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.label();
```

**源码**：[source/std/log.chl:8](../../source/std/log.chl#L8)。

<a id="api-1"></a>

### formatRecord（2）

**声明**

```chali
String formatRecord(LogLevel level, String message, Map<String,String> fields);
```

**行为**

编码一条 JSON 日志记录；字段保存在独立的 fields 对象中。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `level` | `LogLevel level` | 日志严重性 | 无 |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |
| `fields` | `Map<String,String> fields` | 记录的字符串字段 | 无 |

**返回值**

`String`。编码一条 JSON 日志记录；字段保存在独立的 fields 对象中。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
formatRecord(level, message, fields);
```

**源码**：[source/std/log.chl:37](../../source/std/log.chl#L37)。

<a id="api-2"></a>

### writeRecord（3）

**声明**

```chali
void writeRecord(LogLevel level, String message, Map<String,String> fields);
```

**行为**

将一条结构化记录写到标准错误，附加一个换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `level` | `LogLevel level` | 日志严重性 | 无 |
| `message` | `String message` | 诊断文本或消息载荷 | 无 |
| `fields` | `Map<String,String> fields` | 记录的字符串字段 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
writeRecord(level, message, fields);
```

**源码**：[source/std/log.chl:47](../../source/std/log.chl#L47)。
