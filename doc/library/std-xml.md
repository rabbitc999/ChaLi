# std::xml — XML 文档

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-xml.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::xml;
```

## 公共约定与错误边界

parse 接收 UTF-8 文本，成功返回拥有节点值，失败返回 Error，解析诊断包含从 1 开始的行列号。childCount 统计元素与文本组成的混合内容；child 和 text 使用相同的从 0 开始的下标，类型不匹配或越界返回 None。节点通过值复制独立保存。escapeText 与 escapeAttribute 只转义文本，不解析文档。

## 参考代码

完整程序：[example/xml/demo.chl](../../example/xml/demo.chl)。

```powershell
chali run example/xml/demo.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::xml;
void main() {
    let document = parse("<message from='ChaLi'>Hi<em>there</em></message>").unwrap();
    assert(document.name() == "message");
    assert(document.attribute("from").unwrap() == "ChaLi");
    assert(document.text(0).unwrap() == "Hi");
    assert(document.child(1).unwrap().name() == "em");
    assert(escapeAttribute("<&\"") == "&lt;&amp;&quot;");
    assert(parse("<bad></wrong>").isErr());
    assert(parse("<!DOCTYPE a SYSTEM 'file:///missing'><a/>").isErr());
    println("XML OK");
}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-553"></a>`struct XmlNode` | An XML element with ordered mixed content. |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [XmlNode::name](#api-0) | `String name()` |
| [XmlNode::attribute](#api-1) | `Option<String> attribute(String name)` |
| [XmlNode::childCount](#api-2) | `int64 childCount()` |
| [XmlNode::child](#api-3) | `Option<XmlNode> child(int64 index)` |
| [XmlNode::text](#api-4) | `Option<String> text(int64 index)` |
| [parse](#api-5) | `Result<XmlNode,Error> parse(String text)` |
| [escapeText](#api-6) | `String escapeText(String value)` |
| [escapeAttribute](#api-7) | `String escapeAttribute(String value)` |

<a id="api-0"></a>

### XmlNode::name（1）

**声明**

```chali
String name();
```

**行为**

返回元素的源限定名。

Read the element's qualified source name.

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。返回元素的源限定名。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.name();
```

**源码**：[library/xml/xml.chl.in:12](../../library/xml/xml.chl.in#L12)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-1"></a>

### XmlNode::attribute（2）

**声明**

```chali
Option<String> attribute(String name);
```

**行为**

按精确名称读取属性；不存在返回 None。

Read an attribute by its exact source name.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |

**返回值**

`Option<String>`。按精确名称读取属性；不存在返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.attribute(name);
```

**源码**：[library/xml/xml.chl.in:15](../../library/xml/xml.chl.in#L15)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-2"></a>

### XmlNode::childCount（3）

**声明**

```chali
int64 childCount();
```

**行为**

返回按源顺序排列的元素及文本片段总数。

Return the number of element and text fragments in source order.

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回按源顺序排列的元素及文本片段总数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.childCount();
```

**源码**：[library/xml/xml.chl.in:23](../../library/xml/xml.chl.in#L23)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-3"></a>

### XmlNode::child（4）

**声明**

```chali
Option<XmlNode> child(int64 index);
```

**行为**

按混合内容下标读取元素子节点；越界或该位置为文本时返回 None。

Return an element child at a mixed-content index.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从零开始的下标。 | 无 |

**返回值**

`Option<XmlNode>`。按混合内容下标读取元素子节点；越界或该位置为文本时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.child(index);
```

**源码**：[library/xml/xml.chl.in:26](../../library/xml/xml.chl.in#L26)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-4"></a>

### XmlNode::text（5）

**声明**

```chali
Option<String> text(int64 index);
```

**行为**

按混合内容下标读取文本；越界或该位置为元素时返回 None。

Return a text fragment at a mixed-content index.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从零开始的下标。 | 无 |

**返回值**

`Option<String>`。按混合内容下标读取文本；越界或该位置为元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.text(index);
```

**源码**：[library/xml/xml.chl.in:35](../../library/xml/xml.chl.in#L35)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-5"></a>

### parse（6）

**声明**

```chali
Result<XmlNode,Error> parse(String text);
```

**行为**

解析一份 UTF-8 XML 文档，成功返回根节点，失败返回带行列的 Error。

Parse one UTF-8 document. Error messages include one-based line and column.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Result<XmlNode,Error>`。解析一份 UTF-8 XML 文档，成功返回根节点，失败返回带行列的 Error。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::xml::parse(text);
```

**源码**：[library/xml/xml.chl.in:43](../../library/xml/xml.chl.in#L43)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-6"></a>

### escapeText（7）

**声明**

```chali
String escapeText(String value);
```

**行为**

转义文本中的 &、< 和 >。

Escape text content for an XML element.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `String value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`String`。转义文本中的 &、< 和 >。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
escapeText(value);
```

**源码**：[library/xml/xml.chl.in:57](../../library/xml/xml.chl.in#L57)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。

<a id="api-7"></a>

### escapeAttribute（8）

**声明**

```chali
String escapeAttribute(String value);
```

**行为**

转义文本、单双引号以及回车、换行和制表符，适用于引号包围的属性值。

Escape a value for a quoted XML attribute.

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `String value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`String`。转义文本、单双引号以及回车、换行和制表符，适用于引号包围的属性值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
escapeAttribute(value);
```

**源码**：[library/xml/xml.chl.in:62](../../library/xml/xml.chl.in#L62)。

**完整示例**：[example/xml/demo.chl](../../example/xml/demo.chl)。
