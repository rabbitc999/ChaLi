# std::text — UTF-8 文本工具

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-text.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::text;
```

## 公共约定与错误边界

长度、位置和填充宽度按 UTF-8 字节，不按字符数或显示宽度。大小写转换仅 ASCII。除 charAt 返回字节外，变换返回新字符串。字符串下标要求 UTF-8 字符边界；切片允许任意字节端点，处理中文码点时使用显式 UTF-8 方法。kmpFind/kmpFindAll/kmpFailureTable 是字节级 KMP：kmpFind 返回首个匹配的字节下标，找不到为 -1，空针返回 0（与 find 口径一致）；kmpFindAll 返回升序命中下标、允许重叠匹配，空针给空列表；kmpFailureTable 返回长度等于针长的失配表。KMP 失配表、首次及重叠匹配均由 ChaLi 实现，不依赖宿主 KMP 胶水。stripPrefix 与 stripSuffix 各移除一次匹配，isBlank 只识别 ASCII 空白，countMatches 统计互不重叠的匹配且空子串计零。

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
    List<String> parts = std::text::Text::split("::甲::::乙::", "::");
    println("{} [{}]", parts.len(), std::text::Text::join(parts, "|"));
    List<String> empty = std::text::Text::split("", ",");
    List<String> whole = std::text::Text::split("甲乙", "");
    List<String> none;
    println("{} [{}] [{}]", empty.len(), whole[0], std::text::Text::join(none, ","));
    parts[1] = "changed";
    println("[{}]", std::text::Text::join(parts, "|"));
    float64 notNumber = std::math::sqrt(-1.0);
    println("{} {} {}", std::math::min(notNumber, 2.0), std::math::isNaN(std::math::max(2.0, notNumber)), std::math::isNaN(std::math::clamp(notNumber, 0.0, 1.0)));
    int64 smallest = (int64)((uint64)1 << 63);
    println("{} {} {} {}", std::math::abs(smallest) == smallest, std::math::min(4, 3), std::math::max(-4.0, -3.0), std::math::clamp(-4, -3, 2));
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
| 模块 | <a id="symbol-477"></a>`struct KmpReply` | KMP 操作回执，所有字段由语言实现直接更新。 |
| KmpReply | <a id="symbol-478"></a>`String error` | 本次胶水调用的错误说明；成功时为空串。 |
| KmpReply | <a id="symbol-479"></a>`int64 value` | 首个匹配的字节下标；找不到为 -1。 |
| KmpReply | <a id="symbol-480"></a>`int64 flag` | 布尔结果：1 = 命中。 |
| KmpReply | <a id="symbol-481"></a>`List<int64> table` | 长度为针长的 KMP 失配表。 |
| KmpReply | <a id="symbol-482"></a>`List<int64> positions` | 全部命中下标，升序且允许重叠。 |
| 模块 | <a id="symbol-483"></a>`struct Text` | 字符串查找、比较、裁剪和替换的静态入口。 |
| 模块 | <a id="symbol-484"></a>`struct StringBuilder : Drop` | 字符串构建器：把片段收进列表，最后一次分配写入结果区（与 `Text::join` 同口径）。
`s = s + t` 每次复制整串，逐块累加是 O(n²)；构建器把每段代价压到拷贝一次片段，
`toString` 再用 O(n log k) 的合并收尾。循环拼字符串一律走这里。 |
| StringBuilder | <a id="symbol-485"></a>`std::core::List<std::core::String> chunks` | 已累计的字符串片段。 |
| StringBuilder | <a id="symbol-486"></a>`int64 length` | 已累计的总字节数。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Text::find](#api-0) | `static int64 find(std::core::String text, std::core::String needle)` |
| [Text::rfind](#api-1) | `static int64 rfind(std::core::String text, std::core::String needle)` |
| [Text::contains](#api-2) | `static bool contains(std::core::String text, std::core::String needle)` |
| [Text::startsWith](#api-3) | `static bool startsWith(std::core::String text, std::core::String prefix)` |
| [Text::endsWith](#api-4) | `static bool endsWith(std::core::String text, std::core::String suffix)` |
| [Text::stripPrefix](#api-5) | `static std::core::String stripPrefix(std::core::String text, std::core::String prefix)` |
| [Text::stripSuffix](#api-6) | `static std::core::String stripSuffix(std::core::String text, std::core::String suffix)` |
| [Text::isBlank](#api-7) | `static bool isBlank(std::core::String text)` |
| [Text::countMatches](#api-8) | `static int64 countMatches(std::core::String text, std::core::String needle)` |
| [Text::compare](#api-9) | `static int64 compare(std::core::String left, std::core::String right)` |
| [Text::toUpper](#api-10) | `static std::core::String toUpper(std::core::String text)` |
| [Text::toLower](#api-11) | `static std::core::String toLower(std::core::String text)` |
| [Text::trim](#api-12) | `static std::core::String trim(std::core::String text)` |
| [Text::ltrim](#api-13) | `static std::core::String ltrim(std::core::String text)` |
| [Text::rtrim](#api-14) | `static std::core::String rtrim(std::core::String text)` |
| [Text::replace](#api-15) | `static std::core::String replace(std::core::String text, std::core::String from, std::core::String to)` |
| [Text::repeat](#api-16) | `static std::core::String repeat(std::core::String text, int64 count)` |
| [Text::padStart](#api-17) | `static std::core::String padStart(std::core::String text, int64 width, char fill)` |
| [Text::padEnd](#api-18) | `static std::core::String padEnd(std::core::String text, int64 width, char fill)` |
| [Text::charAt](#api-19) | `static char charAt(std::core::String text, int64 index)` |
| [Text::split](#api-20) | `static std::core::List<std::core::String> split(std::core::String text, std::core::String separator)` |
| [Text::join](#api-21) | `static std::core::String join(std::core::List<std::core::String> parts, std::core::String separator)` |
| [Text::kmpFind](#api-22) | `static int64 kmpFind(std::core::String text, std::core::String needle)` |
| [Text::kmpFindAll](#api-23) | `static std::core::List<int64> kmpFindAll(std::core::String text, std::core::String needle)` |
| [Text::kmpFailureTable](#api-24) | `static std::core::List<int64> kmpFailureTable(std::core::String needle)` |
| [StringBuilder::drop](#api-25) | `void drop()` |
| [StringBuilder::create](#api-26) | `static StringBuilder create()` |
| [StringBuilder::len](#api-27) | `int64 len()` |
| [StringBuilder::isEmpty](#api-28) | `bool isEmpty()` |
| [StringBuilder::append](#api-29) | `void append(std::core::String text)` |
| [StringBuilder::appendLine](#api-30) | `void appendLine(std::core::String text)` |
| [StringBuilder::appendChar](#api-31) | `void appendChar(char value)` |
| [StringBuilder::appendInt](#api-32) | `void appendInt(int64 value)` |
| [StringBuilder::appendFloat](#api-33) | `void appendFloat(float64 value)` |
| [StringBuilder::appendBool](#api-34) | `void appendBool(bool value)` |
| [StringBuilder::clear](#api-35) | `void clear()` |
| [StringBuilder::toString](#api-36) | `std::core::String toString()` |

<a id="api-0"></a>

### Text::find（1）

**声明**

```chali
static int64 find(std::core::String text, std::core::String needle);
```

**行为**

查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

子串第一次出现的位置；找不到 -1（空子串给 0）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::core::String needle` | 待查找子串 | 无 |

**返回值**

`int64`。查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::find(text, needle);
```

**源码**：[source/std/text.chl:280](../../source/std/text.chl#L280)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-1"></a>

### Text::rfind（2）

**声明**

```chali
static int64 rfind(std::core::String text, std::core::String needle);
```

**行为**

返回子串最后出现的字节下标；找不到为 -1。

子串最后一次出现的位置；找不到 -1（空子串给串长）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::core::String needle` | 待查找子串 | 无 |

**返回值**

`int64`。返回子串最后出现的字节下标；找不到为 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::rfind(text, needle);
```

**源码**：[source/std/text.chl:284](../../source/std/text.chl#L284)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-2"></a>

### Text::contains（3）

**声明**

```chali
static bool contains(std::core::String text, std::core::String needle);
```

**行为**

判断是否包含目标值或匹配文本。

含不含子串（等价 `find >= 0`）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::core::String needle` | 待查找子串 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::contains(text, needle);
```

**源码**：[source/std/text.chl:288](../../source/std/text.chl#L288)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-3"></a>

### Text::startsWith（4）

**声明**

```chali
static bool startsWith(std::core::String text, std::core::String prefix);
```

**行为**

判断文本是否以 prefix 开始。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `prefix` | `std::core::String prefix` | 前缀 | 无 |

**返回值**

`bool`。判断文本是否以 prefix 开始。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::startsWith(text, prefix);
```

**源码**：[source/std/text.chl:291](../../source/std/text.chl#L291)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-4"></a>

### Text::endsWith（5）

**声明**

```chali
static bool endsWith(std::core::String text, std::core::String suffix);
```

**行为**

判断文本是否以 suffix 结束。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `suffix` | `std::core::String suffix` | 后缀 | 无 |

**返回值**

`bool`。判断文本是否以 suffix 结束。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::endsWith(text, suffix);
```

**源码**：[source/std/text.chl:297](../../source/std/text.chl#L297)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-5"></a>

### Text::stripPrefix（6）

**声明**

```chali
static std::core::String stripPrefix(std::core::String text, std::core::String prefix);
```

**行为**

去掉一次匹配的前缀，未匹配时返回原值副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `prefix` | `std::core::String prefix` | 前缀 | 无 |

**返回值**

`std::core::String`。去掉一次匹配的前缀，未匹配时返回原值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::stripPrefix(text, prefix);
```

**源码**：[source/std/text.chl:304](../../source/std/text.chl#L304)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-6"></a>

### Text::stripSuffix（7）

**声明**

```chali
static std::core::String stripSuffix(std::core::String text, std::core::String suffix);
```

**行为**

去掉一次匹配的后缀，未匹配时返回原值副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `suffix` | `std::core::String suffix` | 后缀 | 无 |

**返回值**

`std::core::String`。去掉一次匹配的后缀，未匹配时返回原值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::stripSuffix(text, suffix);
```

**源码**：[source/std/text.chl:309](../../source/std/text.chl#L309)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-7"></a>

### Text::isBlank（8）

**声明**

```chali
static bool isBlank(std::core::String text);
```

**行为**

判断文本是否为空或仅由 ASCII 空白组成。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`bool`。判断文本是否为空或仅由 ASCII 空白组成。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::isBlank(text);
```

**源码**：[source/std/text.chl:314](../../source/std/text.chl#L314)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-8"></a>

### Text::countMatches（9）

**声明**

```chali
static int64 countMatches(std::core::String text, std::core::String needle);
```

**行为**

统计从左到右互不重叠的匹配，空子串返回零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::core::String needle` | 待查找子串 | 无 |

**返回值**

`int64`。统计从左到右互不重叠的匹配，空子串返回零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::countMatches(text, needle);
```

**源码**：[source/std/text.chl:322](../../source/std/text.chl#L322)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-9"></a>

### Text::compare（10）

**声明**

```chali
static int64 compare(std::core::String left, std::core::String right);
```

**行为**

按字典序比较，返回 -1、0 或 1。

字典序比较：左小 -1、相等 0、左大 1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `std::core::String left` | 左操作数或区间左端 | 无 |
| `right` | `std::core::String right` | 右操作数或区间右端 | 无 |

**返回值**

`int64`。按字典序比较，返回 -1、0 或 1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::compare(left, right);
```

**源码**：[source/std/text.chl:335](../../source/std/text.chl#L335)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-10"></a>

### Text::toUpper（11）

**声明**

```chali
static std::core::String toUpper(std::core::String text);
```

**行为**

将 ASCII 小写字母转换成大写。

ASCII 大小写（VM 里不认区域设置，结果可复现）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::core::String`。将 ASCII 小写字母转换成大写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::toUpper(text);
```

**源码**：[source/std/text.chl:341](../../source/std/text.chl#L341)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-11"></a>

### Text::toLower（12）

**声明**

```chali
static std::core::String toLower(std::core::String text);
```

**行为**

将 ASCII 大写字母转换成小写。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::core::String`。将 ASCII 大写字母转换成小写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::toLower(text);
```

**源码**：[source/std/text.chl:342](../../source/std/text.chl#L342)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-12"></a>

### Text::trim（13）

**声明**

```chali
static std::core::String trim(std::core::String text);
```

**行为**

去掉两端 ASCII 空白。

去掉两端 / 左端 / 右端的空白（空格 \t \n \r \v \f）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::core::String`。去掉两端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::trim(text);
```

**源码**：[source/std/text.chl:344](../../source/std/text.chl#L344)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-13"></a>

### Text::ltrim（14）

**声明**

```chali
static std::core::String ltrim(std::core::String text);
```

**行为**

去掉左端 ASCII 空白。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::core::String`。去掉左端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::ltrim(text);
```

**源码**：[source/std/text.chl:345](../../source/std/text.chl#L345)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-14"></a>

### Text::rtrim（15）

**声明**

```chali
static std::core::String rtrim(std::core::String text);
```

**行为**

去掉右端 ASCII 空白。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::core::String`。去掉右端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::rtrim(text);
```

**源码**：[source/std/text.chl:346](../../source/std/text.chl#L346)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-15"></a>

### Text::replace（16）

**声明**

```chali
static std::core::String replace(std::core::String text, std::core::String from, std::core::String to);
```

**行为**

将所有 from 子串替换为 to；空 from 保留副本。

全部替换；`from` 为空时返回原串的副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `from` | `std::core::String from` | 被替换的子串 | 无 |
| `to` | `std::core::String to` | 替换文本 | 无 |

**返回值**

`std::core::String`。将所有 from 子串替换为 to；空 from 保留副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::replace(text, from, to);
```

**源码**：[source/std/text.chl:348](../../source/std/text.chl#L348)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-16"></a>

### Text::repeat（17）

**声明**

```chali
static std::core::String repeat(std::core::String text, int64 count);
```

**行为**

重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

重复 `count` 次（`count <= 0` 给空串）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`std::core::String`。重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::repeat(text, count);
```

**源码**：[source/std/text.chl:352](../../source/std/text.chl#L352)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-17"></a>

### Text::padStart（18）

**声明**

```chali
static std::core::String padStart(std::core::String text, int64 width, char fill);
```

**行为**

在左侧以 fill 补到 width 字节。

补到 `width` 字节宽；已经够宽就原样返回副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `width` | `int64 width` | 目标字节宽度或定宽字段大小 | 无 |
| `fill` | `char fill` | 填充字节 | 无 |

**返回值**

`std::core::String`。在左侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::padStart(text, width, fill);
```

**源码**：[source/std/text.chl:356](../../source/std/text.chl#L356)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-18"></a>

### Text::padEnd（19）

**声明**

```chali
static std::core::String padEnd(std::core::String text, int64 width, char fill);
```

**行为**

在右侧以 fill 补到 width 字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `width` | `int64 width` | 目标字节宽度或定宽字段大小 | 无 |
| `fill` | `char fill` | 填充字节 | 无 |

**返回值**

`std::core::String`。在右侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::padEnd(text, width, fill);
```

**源码**：[source/std/text.chl:359](../../source/std/text.chl#L359)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-19"></a>

### Text::charAt（20）

**声明**

```chali
static char charAt(std::core::String text, int64 index);
```

**行为**

读取 index 位置的字节，需处于有效 UTF-8 边界。

第 `index` 个字节；越界报错（和 `text[index]` 一样）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`char`。读取 index 位置的字节，需处于有效 UTF-8 边界。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::charAt(text, index);
```

**源码**：[source/std/text.chl:363](../../source/std/text.chl#L363)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-20"></a>

### Text::split（21）

**声明**

```chali
static std::core::List<std::core::String> split(std::core::String text, std::core::String separator);
```

**行为**

按 separator 分割，空分隔符把整串作为一项。

按分隔符切成列表；分隔符为空时整串作为一个元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `separator` | `std::core::String separator` | 分隔文本 | 无 |

**返回值**

`std::core::List<std::core::String>`。按 separator 分割，空分隔符把整串作为一项。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::split(text, separator);
```

**源码**：[source/std/text.chl:366](../../source/std/text.chl#L366)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-21"></a>

### Text::join（22）

**声明**

```chali
static std::core::String join(std::core::List<std::core::String> parts, std::core::String separator);
```

**行为**

用 separator 连接 parts。

用分隔符把列表拼成一串（空列表给空串）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `parts` | `std::core::List<std::core::String> parts` | 待拼接文本列表 | 无 |
| `separator` | `std::core::String separator` | 分隔文本 | 无 |

**返回值**

`std::core::String`。用 separator 连接 parts。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::join(parts, separator);
```

**源码**：[source/std/text.chl:371](../../source/std/text.chl#L371)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-22"></a>

### Text::kmpFind（23）

**声明**

```chali
static int64 kmpFind(std::core::String text, std::core::String needle);
```

**行为**

KMP 查找：针在串中首次出现的字节下标，O(n + m)。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 干草堆。 | 无 |
| `needle` | `std::core::String needle` | 针。 | 无 |

**返回值**

`int64`。KMP 查找：针在串中首次出现的字节下标，O(n + m)。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::kmpFind(text, needle);
```

**源码**：[source/std/text.chl:382](../../source/std/text.chl#L382)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-23"></a>

### Text::kmpFindAll（24）

**声明**

```chali
static std::core::List<int64> kmpFindAll(std::core::String text, std::core::String needle);
```

**行为**

KMP 查找全部（可重叠）匹配位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 干草堆。 | 无 |
| `needle` | `std::core::String needle` | 针。 | 无 |

**返回值**

`std::core::List<int64>`。KMP 查找全部（可重叠）匹配位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::kmpFindAll(text, needle);
```

**源码**：[source/std/text.chl:394](../../source/std/text.chl#L394)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-24"></a>

### Text::kmpFailureTable（25）

**声明**

```chali
static std::core::List<int64> kmpFailureTable(std::core::String needle);
```

**行为**

计算 KMP 失配表：table[i] 是 needle[0..i] 的最长相等真前后缀长度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `needle` | `std::core::String needle` | 针。 | 无 |

**返回值**

`std::core::List<int64>`。计算 KMP 失配表：table[i] 是 needle[0..i] 的最长相等真前后缀长度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::Text::kmpFailureTable(needle);
```

**源码**：[source/std/text.chl:405](../../source/std/text.chl#L405)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-25"></a>

### StringBuilder::drop（26）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

< 已累计的总字节数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/text.chl:422](../../source/std/text.chl#L422)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-26"></a>

### StringBuilder::create（27）

**声明**

```chali
static StringBuilder create();
```

**行为**

创建StringBuilder，返回可供后续操作的值。

新建空构建器（`create` 是具名构造入口：体内写 self，不写 return）。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`StringBuilder`。创建StringBuilder，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::text::StringBuilder::create();
```

**源码**：[source/std/text.chl:425](../../source/std/text.chl#L425)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-27"></a>

### StringBuilder::len（28）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

当前累计的字节数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/text.chl:430](../../source/std/text.chl#L430)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-28"></a>

### StringBuilder::isEmpty（29）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

是否还没有追加过任何内容。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/text.chl:432](../../source/std/text.chl#L432)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-29"></a>

### StringBuilder::append（30）

**声明**

```chali
void append(std::core::String text);
```

**行为**

追加一段文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.append(text);
```

**源码**：[source/std/text.chl:437](../../source/std/text.chl#L437)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-30"></a>

### StringBuilder::appendLine（31）

**声明**

```chali
void appendLine(std::core::String text);
```

**行为**

追加一行（文本后补 LF）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::core::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.appendLine(text);
```

**源码**：[source/std/text.chl:442](../../source/std/text.chl#L442)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-31"></a>

### StringBuilder::appendChar（32）

**声明**

```chali
void appendChar(char value);
```

**行为**

追加一个 ASCII 字符。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `char value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.appendChar(value);
```

**源码**：[source/std/text.chl:444](../../source/std/text.chl#L444)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-32"></a>

### StringBuilder::appendInt（33）

**声明**

```chali
void appendInt(int64 value);
```

**行为**

追加一个整数的十进制文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.appendInt(value);
```

**源码**：[source/std/text.chl:446](../../source/std/text.chl#L446)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-33"></a>

### StringBuilder::appendFloat（34）

**声明**

```chali
void appendFloat(float64 value);
```

**行为**

追加一个浮点数的默认格式文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.appendFloat(value);
```

**源码**：[source/std/text.chl:448](../../source/std/text.chl#L448)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-34"></a>

### StringBuilder::appendBool（35）

**声明**

```chali
void appendBool(bool value);
```

**行为**

追加 `true` / `false`。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.appendBool(value);
```

**源码**：[source/std/text.chl:450](../../source/std/text.chl#L450)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-35"></a>

### StringBuilder::clear（36）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空已累计的片段，容量语义归零。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/text.chl:456](../../source/std/text.chl#L456)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。

<a id="api-36"></a>

### StringBuilder::toString（37）

**声明**

```chali
std::core::String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

摊平成最终字符串；之后可继续追加。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`std::core::String`。由实现者将当前值转换为文本，Formattable 重载接收 style。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toString();
```

**源码**：[source/std/text.chl:463](../../source/std/text.chl#L463)。

**完整示例**：[example/samples/std_glue_edges.chl](../../example/samples/std_glue_edges.chl)。
