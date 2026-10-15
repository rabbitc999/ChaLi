# std::text — UTF-8 文本工具

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```magel
import std::text;
```

## 公共约定与错误边界

长度、位置和填充宽度按 UTF-8 字节，不按字符数或显示宽度。大小写转换仅 ASCII。除 charAt 返回字节外，变换返回新字符串。字符串下标/切片不允许从多字节编码中间开始；处理中文要保持 UTF-8 边界。

## 参考代码

完整程序：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

```powershell
magel run example/samples/std_glue_edges.mgl
```

<details>
<summary>展开完整参考代码</summary>

```magel
import std::text;
import std::math;
int64 main() {
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
    return 0;
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
| 模块 | <a id="symbol-389"></a>`struct Text` | 字符串查找、比较、裁剪和替换的静态入口。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Text::find](#api-0) | `static int64 find(std::lang::String text, std::lang::String needle)` |
| [Text::rfind](#api-1) | `static int64 rfind(std::lang::String text, std::lang::String needle)` |
| [Text::contains](#api-2) | `static bool contains(std::lang::String text, std::lang::String needle)` |
| [Text::startsWith](#api-3) | `static bool startsWith(std::lang::String text, std::lang::String prefix)` |
| [Text::endsWith](#api-4) | `static bool endsWith(std::lang::String text, std::lang::String suffix)` |
| [Text::compare](#api-5) | `static int64 compare(std::lang::String left, std::lang::String right)` |
| [Text::toUpper](#api-6) | `static std::lang::String toUpper(std::lang::String text)` |
| [Text::toLower](#api-7) | `static std::lang::String toLower(std::lang::String text)` |
| [Text::trim](#api-8) | `static std::lang::String trim(std::lang::String text)` |
| [Text::ltrim](#api-9) | `static std::lang::String ltrim(std::lang::String text)` |
| [Text::rtrim](#api-10) | `static std::lang::String rtrim(std::lang::String text)` |
| [Text::replace](#api-11) | `static std::lang::String replace(std::lang::String text, std::lang::String from, std::lang::String to)` |
| [Text::repeat](#api-12) | `static std::lang::String repeat(std::lang::String text, int64 count)` |
| [Text::padStart](#api-13) | `static std::lang::String padStart(std::lang::String text, int64 width, char fill)` |
| [Text::padEnd](#api-14) | `static std::lang::String padEnd(std::lang::String text, int64 width, char fill)` |
| [Text::charAt](#api-15) | `static char charAt(std::lang::String text, int64 index)` |
| [Text::split](#api-16) | `static std::lang::List<std::lang::String> split(std::lang::String text, std::lang::String separator)` |
| [Text::join](#api-17) | `static std::lang::String join(std::lang::List<std::lang::String> parts, std::lang::String separator)` |

<a id="api-0"></a>

### Text::find（1）

**声明**

```magel
static int64 find(std::lang::String text, std::lang::String needle);
```

**行为**

查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

子串第一次出现的位置；找不到 -1（空子串给 0）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::lang::String needle` | 待查找子串 | 无 |

**返回值**

`int64`。查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::find(text, needle);
```

**源码**：[source/std/text.mgl:29](../../source/std/text.mgl#L29)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-1"></a>

### Text::rfind（2）

**声明**

```magel
static int64 rfind(std::lang::String text, std::lang::String needle);
```

**行为**

返回子串最后出现的字节下标；找不到为 -1。

子串最后一次出现的位置；找不到 -1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::lang::String needle` | 待查找子串 | 无 |

**返回值**

`int64`。返回子串最后出现的字节下标；找不到为 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::rfind(text, needle);
```

**源码**：[source/std/text.mgl:31](../../source/std/text.mgl#L31)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-2"></a>

### Text::contains（3）

**声明**

```magel
static bool contains(std::lang::String text, std::lang::String needle);
```

**行为**

判断是否包含目标值或匹配文本。

含不含子串（等价 `find >= 0`）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `needle` | `std::lang::String needle` | 待查找子串 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::contains(text, needle);
```

**源码**：[source/std/text.mgl:33](../../source/std/text.mgl#L33)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-3"></a>

### Text::startsWith（4）

**声明**

```magel
static bool startsWith(std::lang::String text, std::lang::String prefix);
```

**行为**

判断文本是否以 prefix 开始。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `prefix` | `std::lang::String prefix` | 前缀 | 无 |

**返回值**

`bool`。判断文本是否以 prefix 开始。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::startsWith(text, prefix);
```

**源码**：[source/std/text.mgl:34](../../source/std/text.mgl#L34)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-4"></a>

### Text::endsWith（5）

**声明**

```magel
static bool endsWith(std::lang::String text, std::lang::String suffix);
```

**行为**

判断文本是否以 suffix 结束。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `suffix` | `std::lang::String suffix` | 后缀 | 无 |

**返回值**

`bool`。判断文本是否以 suffix 结束。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::endsWith(text, suffix);
```

**源码**：[source/std/text.mgl:35](../../source/std/text.mgl#L35)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-5"></a>

### Text::compare（6）

**声明**

```magel
static int64 compare(std::lang::String left, std::lang::String right);
```

**行为**

按字典序比较，返回 -1、0 或 1。

字典序比较：左小 -1、相等 0、左大 1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `left` | `std::lang::String left` | 左操作数或区间左端 | 无 |
| `right` | `std::lang::String right` | 右操作数或区间右端 | 无 |

**返回值**

`int64`。按字典序比较，返回 -1、0 或 1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::compare(left, right);
```

**源码**：[source/std/text.mgl:37](../../source/std/text.mgl#L37)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-6"></a>

### Text::toUpper（7）

**声明**

```magel
static std::lang::String toUpper(std::lang::String text);
```

**行为**

将 ASCII 小写字母转换成大写。

ASCII 大小写（VM 里不认区域设置，结果可复现）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::lang::String`。将 ASCII 小写字母转换成大写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::toUpper(text);
```

**源码**：[source/std/text.mgl:39](../../source/std/text.mgl#L39)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-7"></a>

### Text::toLower（8）

**声明**

```magel
static std::lang::String toLower(std::lang::String text);
```

**行为**

将 ASCII 大写字母转换成小写。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::lang::String`。将 ASCII 大写字母转换成小写。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::toLower(text);
```

**源码**：[source/std/text.mgl:40](../../source/std/text.mgl#L40)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-8"></a>

### Text::trim（9）

**声明**

```magel
static std::lang::String trim(std::lang::String text);
```

**行为**

去掉两端 ASCII 空白。

去掉两端 / 左端 / 右端的空白（空格 \t \n \r \v \f）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::lang::String`。去掉两端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::trim(text);
```

**源码**：[source/std/text.mgl:42](../../source/std/text.mgl#L42)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-9"></a>

### Text::ltrim（10）

**声明**

```magel
static std::lang::String ltrim(std::lang::String text);
```

**行为**

去掉左端 ASCII 空白。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::lang::String`。去掉左端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::ltrim(text);
```

**源码**：[source/std/text.mgl:43](../../source/std/text.mgl#L43)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-10"></a>

### Text::rtrim（11）

**声明**

```magel
static std::lang::String rtrim(std::lang::String text);
```

**行为**

去掉右端 ASCII 空白。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`std::lang::String`。去掉右端 ASCII 空白。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::rtrim(text);
```

**源码**：[source/std/text.mgl:44](../../source/std/text.mgl#L44)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-11"></a>

### Text::replace（12）

**声明**

```magel
static std::lang::String replace(std::lang::String text, std::lang::String from, std::lang::String to);
```

**行为**

将所有 from 子串替换为 to；空 from 保留副本。

全部替换；`from` 为空时返回原串的副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `from` | `std::lang::String from` | 被替换的子串 | 无 |
| `to` | `std::lang::String to` | 替换文本 | 无 |

**返回值**

`std::lang::String`。将所有 from 子串替换为 to；空 from 保留副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::replace(text, from, to);
```

**源码**：[source/std/text.mgl:46](../../source/std/text.mgl#L46)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-12"></a>

### Text::repeat（13）

**声明**

```magel
static std::lang::String repeat(std::lang::String text, int64 count);
```

**行为**

重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

重复 `count` 次（`count <= 0` 给空串）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`std::lang::String`。重复给定值或文本；文本次数非正时为空串，迭代器版本为无限源。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::repeat(text, count);
```

**源码**：[source/std/text.mgl:50](../../source/std/text.mgl#L50)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-13"></a>

### Text::padStart（14）

**声明**

```magel
static std::lang::String padStart(std::lang::String text, int64 width, char fill);
```

**行为**

在左侧以 fill 补到 width 字节。

补到 `width` 字节宽；已经够宽就原样返回副本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `width` | `int64 width` | 目标字节宽度或定宽字段大小 | 无 |
| `fill` | `char fill` | 填充字节 | 无 |

**返回值**

`std::lang::String`。在左侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::padStart(text, width, fill);
```

**源码**：[source/std/text.mgl:52](../../source/std/text.mgl#L52)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-14"></a>

### Text::padEnd（15）

**声明**

```magel
static std::lang::String padEnd(std::lang::String text, int64 width, char fill);
```

**行为**

在右侧以 fill 补到 width 字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `width` | `int64 width` | 目标字节宽度或定宽字段大小 | 无 |
| `fill` | `char fill` | 填充字节 | 无 |

**返回值**

`std::lang::String`。在右侧以 fill 补到 width 字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::padEnd(text, width, fill);
```

**源码**：[source/std/text.mgl:55](../../source/std/text.mgl#L55)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-15"></a>

### Text::charAt（16）

**声明**

```magel
static char charAt(std::lang::String text, int64 index);
```

**行为**

读取 index 位置的字节，需处于有效 UTF-8 边界。

第 `index` 个字节；越界报错（和 `text[index]` 一样）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`char`。读取 index 位置的字节，需处于有效 UTF-8 边界。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::charAt(text, index);
```

**源码**：[source/std/text.mgl:59](../../source/std/text.mgl#L59)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-16"></a>

### Text::split（17）

**声明**

```magel
static std::lang::List<std::lang::String> split(std::lang::String text, std::lang::String separator);
```

**行为**

按 separator 分割，空分隔符把整串作为一项。

按分隔符切成列表；分隔符为空时整串作为一个元素。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `std::lang::String text` | 输入文本，位置与长度通常按字节 | 无 |
| `separator` | `std::lang::String separator` | 分隔文本 | 无 |

**返回值**

`std::lang::List<std::lang::String>`。按 separator 分割，空分隔符把整串作为一项。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::split(text, separator);
```

**源码**：[source/std/text.mgl:62](../../source/std/text.mgl#L62)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。

<a id="api-17"></a>

### Text::join（18）

**声明**

```magel
static std::lang::String join(std::lang::List<std::lang::String> parts, std::lang::String separator);
```

**行为**

用 separator 连接 parts。

用分隔符把列表拼成一串（空列表给空串）。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `parts` | `std::lang::List<std::lang::String> parts` | 待拼接文本列表 | 无 |
| `separator` | `std::lang::String separator` | 分隔文本 | 无 |

**返回值**

`std::lang::String`。用 separator 连接 parts。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```magel
std::text::Text::join(parts, separator);
```

**源码**：[source/std/text.mgl:67](../../source/std/text.mgl#L67)。

**完整示例**：[example/samples/std_glue_edges.mgl](../../example/samples/std_glue_edges.mgl)。
