# std::io — 文件与流接口

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-io.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::io;
```

## 公共约定与错误边界

整文件字节读写由 ChaLi 组合 FileStream 实现，分块检查预算。文件句柄仍由宿主托管，保留自动关闭与错误状态。FileStream 返回 Result<T, Error>，失败载荷保留具体 IoError 类型；read/write/seek 之前检查打开结果。read 按底层文件读取一行。close 显式关闭句柄。Reader/Writer/Seek 是用户可实现的接口，接口本身不创建文件。路径与模式由宿主文件系统解释。

## 参考代码

完整程序：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

```powershell
chali run doc/examples/library/io.chl -- build/library-example.txt
```

先创建 `build` 目录；示例会覆盖给定路径，请使用专用测试文件。

<details>
<summary>展开完整参考代码</summary>

```chali
import std::io;

/** @brief 在调用者指定的测试文件中演示 FileStream 的 Result 错误处理。
 * @param args 仅一个文件路径，该文件会被覆盖。 @return 成功为 0。 */
void main(List<String> args) {
    assert(args.len() == 1, "请提供允许覆盖的示例文件路径");
    Result<FileStream,Error> opened = FileStream::open(args[0],"w+");
    assert(opened.isOk(), "打开失败");
    FileStream stream = opened.take();
    Result<int64,Error> written = stream.write("alpha\nbeta\n");
    assert(written.isOk());
    Result<int64,Error> flushed = stream.flush();
    assert(flushed.isOk());
    Result<int64,Error> located = stream.seek(0,SeekOrigin::Begin);
    assert(located.isOk());
    Result<String,Error> first = stream.read();
    assert(first.isOk() && first.unwrap() == "alpha");
    stream.close();
    println("io API ok");

}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-198"></a>`enum IoError : Error` | 从 core.chl 原样迁出：符号名不变，只换了模块归属（现在必须 `import std::io;` 才能用）。 |
| IoError | `OpenFailed(String message), ReadFailed(String message), WriteFailed(String message), SeekFailed(String message), Closed` | 从 core.chl 原样迁出：符号名不变，只换了模块归属（现在必须 `import std::io;` 才能用）。 变体及载荷见声明。 |
| 模块 | <a id="symbol-199"></a>`enum SeekOrigin` | 文件定位的基准：文件开始、当前位置或文件末尾。 |
| SeekOrigin | `Begin, Current, End` | 文件定位的基准：文件开始、当前位置或文件末尾。 变体及载荷见声明。 |
| 模块 | <a id="symbol-200"></a>`interface Reader` | 按字节读取数据的接口。 |
| 模块 | <a id="symbol-201"></a>`interface Writer` | 按字节写入数据的接口。 |
| 模块 | <a id="symbol-202"></a>`interface Seek` | 改变资源读写位置的接口。 |
| 模块 | <a id="symbol-203"></a>`interface ToString` | 将值转换成拥有字符串的接口。 |
| 模块 | <a id="symbol-204"></a>`interface Formattable` | 按格式说明生成文本的接口。 |
| 模块 | <a id="symbol-205"></a>`struct FileStream : Reader, Writer, Seek` | 拥有文件句柄的字节流；通过 Result 报告读写与定位错误。 |
| FileStream | <a id="symbol-206"></a>`File handle` | 宿主资源句柄；应通过所属类型的方法操作。 |
| 模块 | <a id="symbol-207"></a>`struct FileInfo` | 文件信息，时间为 Unix 秒；目录的 size 为零。 |
| FileInfo | <a id="symbol-208"></a>`int64 size` | 文件或响应报告的字节长度。 |
| FileInfo | <a id="symbol-209"></a>`bool isFile` | 目标是否为普通文件。 |
| FileInfo | <a id="symbol-210"></a>`bool isDirectory` | 目标是否为目录。 |
| FileInfo | <a id="symbol-211"></a>`int64 modifiedUnixSeconds` | 最后修改时间，以 Unix 秒表示。 |
| 模块 | <a id="symbol-212"></a>`struct IoReply` | 宿主回调的拥有型结果；字段顺序是胶水 ABI 的一部分。 |
| IoReply | <a id="symbol-213"></a>`String error` | 最近操作的错误说明。 |
| IoReply | <a id="symbol-214"></a>`List<uint8> bytes` | 字节数据，保留零字节。 |
| IoReply | <a id="symbol-215"></a>`List<String> paths` | 目录枚举返回的路径。 |
| IoReply | <a id="symbol-216"></a>`int64 count` | 本次操作实际处理的数量。 |
| IoReply | <a id="symbol-217"></a>`int64 size` | 文件或响应报告的字节长度。 |
| IoReply | <a id="symbol-218"></a>`bool isFile` | 目标是否为普通文件。 |
| IoReply | <a id="symbol-219"></a>`bool isDirectory` | 目标是否为目录。 |
| IoReply | <a id="symbol-220"></a>`int64 modifiedUnixSeconds` | 最后修改时间，以 Unix 秒表示。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [IoError::toString](#api-0) | `String toString()` |
| [Reader::read](#api-1) | `Result<String, Error> read()` |
| [Writer::write](#api-2) | `Result<int64, Error> write(String text)` |
| [Writer::flush](#api-3) | `Result<int64, Error> flush()` |
| [Seek::seek](#api-4) | `Result<int64, Error> seek(int64 offset, SeekOrigin origin)` |
| [ToString::toString](#api-5) | `String toString()` |
| [Formattable::toString](#api-6) | `String toString(String style)` |
| [FileStream::open](#api-7) | `static Result<FileStream, Error> open(String path, String mode)` |
| [FileStream::read](#api-8) | `Result<String, Error> read()` |
| [FileStream::readLine](#api-9) | `Result<String,Error> readLine()` |
| [FileStream::readString](#api-10) | `Result<String,Error> readString(int64 count)` |
| [FileStream::readAllString](#api-11) | `Result<String,Error> readAllString(int64 maxBytes=16777216)` |
| [FileStream::write](#api-12) | `Result<int64, Error> write(String text)` |
| [FileStream::print](#api-13) | `Result<int64, Error> print(String text)` |
| [FileStream::println](#api-14) | `Result<int64, Error> println(String text = "")` |
| [FileStream::readBytes](#api-15) | `Result<List<uint8>,Error> readBytes(int64 count)` |
| [FileStream::writeBytes](#api-16) | `Result<int64,Error> writeBytes(List<uint8> bytes)` |
| [FileStream::readInto](#api-17) | `Result<int64, Error> readInto(uint8[] mut& buffer)` |
| [FileStream::writeFrom](#api-18) | `Result<int64, Error> writeFrom(uint8[] const& buffer, int64 count)` |
| [FileStream::flush](#api-19) | `Result<int64, Error> flush()` |
| [FileStream::seek](#api-20) | `Result<int64, Error> seek(int64 offset, SeekOrigin origin)` |
| [FileStream::close](#api-21) | `int64 close()` |
| [fileOpen](#api-22) | `Result<File, Error> fileOpen(String path, String mode)` |
| [fileRead](#api-23) | `String fileRead(file const& handle)` |
| [fileReadLine](#api-24) | `String fileReadLine(File const& handle)` |
| [fileReadString](#api-25) | `String fileReadString(File const& handle, int64 count)` |
| [fileReadAllString](#api-26) | `String fileReadAllString(File const& handle, int64 maxBytes = 16777216)` |
| [fileWrite](#api-27) | `int64 fileWrite(file const& handle, String text)` |
| [filePrint](#api-28) | `int64 filePrint(file const& handle, String text)` |
| [filePrintln](#api-29) | `int64 filePrintln(file const& handle, String text = "")` |
| [fileEof](#api-30) | `int64 fileEof(file const& handle)` |
| [fileClose](#api-31) | `int64 fileClose(file const& handle)` |
| [readAllBytes](#api-32) | `Result<List<uint8>,Error> readAllBytes(String path, int64 maxBytes=16777216)` |
| [writeAllBytes](#api-33) | `Result<int64,Error> writeAllBytes(String path, List<uint8> bytes)` |
| [readAllText](#api-34) | `Result<String,Error> readAllText(String path, int64 maxBytes=16777216)` |
| [writeAllText](#api-35) | `Result<int64,Error> writeAllText(String path, String text)` |
| [listDirectory](#api-36) | `Result<List<String>,Error> listDirectory(String path)` |
| [fileInfo](#api-37) | `Result<FileInfo,Error> fileInfo(String path)` |
| [createDirectories](#api-38) | `Result<int64,Error> createDirectories(String path)` |
| [renamePath](#api-39) | `Result<int64,Error> renamePath(String from, String to)` |
| [copyTo](#api-40) | `Result<int64,Error> copyTo(FileStream mut& source, FileStream mut& destination)` |
| [walkDirectory](#api-41) | `Result<List<String>,Error> walkDirectory(String path)` |
| [createTempFile](#api-42) | `Result<String,Error> createTempFile(String path)` |
| [replaceFile](#api-43) | `Result<int64,Error> replaceFile(String from, String to)` |

<a id="api-0"></a>

### IoError::toString（1）

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

**源码**：[source/std/io.chl:9](../../source/std/io.chl#L9)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-1"></a>

### Reader::read（2）

**声明**

```chali
Result<String, Error> read();
```

**行为**

读取一行文本，失败返回 ReadFailed。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<String, Error>`。读取一行文本，失败返回 ReadFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.read();
```

**源码**：[source/std/io.chl:22](../../source/std/io.chl#L22)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-2"></a>

### Writer::write（3）

**声明**

```chali
Result<int64, Error> write(String text);
```

**行为**

写入文本，返回写入字节数或 WriteFailed。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<int64, Error>`。写入文本，返回写入字节数或 WriteFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.write(text);
```

**源码**：[source/std/io.chl:24](../../source/std/io.chl#L24)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-3"></a>

### Writer::flush（4）

**声明**

```chali
Result<int64, Error> flush();
```

**行为**

刷新输出缓冲，失败返回 WriteFailed。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int64, Error>`。刷新输出缓冲，失败返回 WriteFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.flush();
```

**源码**：[source/std/io.chl:25](../../source/std/io.chl#L25)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-4"></a>

### Seek::seek（5）

**声明**

```chali
Result<int64, Error> seek(int64 offset, SeekOrigin origin);
```

**行为**

按 origin 与 offset 定位，返回新位置或 SeekFailed。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `offset` | `int64 offset` | 字节偏移或 seek 相对位移 | 无 |
| `origin` | `SeekOrigin origin` | 定位基准或坐标原点 | 无 |

**返回值**

`Result<int64, Error>`。按 origin 与 offset 定位，返回新位置或 SeekFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.seek(offset, origin);
```

**源码**：[source/std/io.chl:27](../../source/std/io.chl#L27)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-5"></a>

### ToString::toString（6）

**声明**

```chali
String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

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

**源码**：[source/std/io.chl:28](../../source/std/io.chl#L28)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-6"></a>

### Formattable::toString（7）

**声明**

```chali
String toString(String style);
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `style` | `String style` | 格式化风格 | 无 |

**返回值**

`String`。由实现者将当前值转换为文本，Formattable 重载接收 style。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toString(style);
```

**源码**：[source/std/io.chl:29](../../source/std/io.chl#L29)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-7"></a>

### FileStream::open（8）

**声明**

```chali
static Result<FileStream, Error> open(String path, String mode);
```

**行为**

按 path/mode 打开文件，返回流或 OpenFailed。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `mode` | `String mode` | 宿主文件打开模式 | 无 |

**返回值**

`Result<FileStream, Error>`。按 path/mode 打开文件，返回流或 OpenFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
FileStream::open(path, mode);
```

**源码**：[source/std/io.chl:47](../../source/std/io.chl#L47)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-8"></a>

### FileStream::read（9）

**声明**

```chali
Result<String, Error> read();
```

**行为**

读取一行文本，失败返回 ReadFailed。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<String, Error>`。读取一行文本，失败返回 ReadFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.read();
```

**源码**：[source/std/io.chl:59](../../source/std/io.chl#L59)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-9"></a>

### FileStream::readLine（10）

**声明**

```chali
Result<String,Error> readLine();
```

**行为**

读取一行文本，保留空格，去掉 LF/CRLF；EOF 返回 Ok(空串)。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<String,Error>`。行文本或 ReadFailed，不隐式关闭文件。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.readLine();
```

**源码**：[source/std/io.chl:70](../../source/std/io.chl#L70)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-10"></a>

### FileStream::readString（11）

**声明**

```chali
Result<String,Error> readString(int64 count);
```

**行为**

从当前游标读取至多 count 个字节为文本；EOF 返回较短字符串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 非负字节数；不按字符或词语切分。 | 无 |

**返回值**

`Result<String,Error>`。文本或 ReadFailed；含零字节时返回错误，应改用 readBytes。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.readString(count);
```

**源码**：[source/std/io.chl:78](../../source/std/io.chl#L78)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-11"></a>

### FileStream::readAllString（12）

**声明**

```chali
Result<String,Error> readAllString(int64 maxBytes=16777216);
```

**行为**

从当前游标读到 EOF，保留文件换行字节；默认最多 16 MiB。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `maxBytes` | `int64 maxBytes` | 非负字节预算；超过预算或含零字节返回错误。 | `16777216` |

**返回值**

`Result<String,Error>`。剩余完整文本或 ReadFailed；预算失败时可能已消费一个额外字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.readAllString(maxBytes);
```

**源码**：[source/std/io.chl:90](../../source/std/io.chl#L90)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-12"></a>

### FileStream::write（13）

**声明**

```chali
Result<int64, Error> write(String text);
```

**行为**

写入文本，返回写入字节数或 WriteFailed。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<int64, Error>`。写入文本，返回写入字节数或 WriteFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.write(text);
```

**源码**：[source/std/io.chl:104](../../source/std/io.chl#L104)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-13"></a>

### FileStream::print（14）

**声明**

```chali
Result<int64, Error> print(String text);
```

**行为**

按 print 格式规则写入文本，不追加换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 已格式化的文本。 | 无 |

**返回值**

`Result<int64, Error>`。写入字节数或 WriteFailed，保留关闭和只读错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.print(text);
```

**源码**：[source/std/io.chl:118](../../source/std/io.chl#L118)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-14"></a>

### FileStream::println（15）

**声明**

```chali
Result<int64, Error> println(String text = "");
```

**行为**

按 print 格式规则写入文本并追加 LF。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 已格式化的文本，省略时只写换行。 | `""` |

**返回值**

`Result<int64, Error>`。包括换行的写入字节数或 WriteFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.println(text);
```

**源码**：[source/std/io.chl:124](../../source/std/io.chl#L124)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-15"></a>

### FileStream::readBytes（16）

**声明**

```chali
Result<List<uint8>,Error> readBytes(int64 count);
```

**行为**

读取至多 count 个字节；EOF 返回较短列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`Result<List<uint8>,Error>`。读取至多 count 个字节；EOF 返回较短列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.readBytes(count);
```

**源码**：[source/std/io.chl:127](../../source/std/io.chl#L127)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-16"></a>

### FileStream::writeBytes（17）

**声明**

```chali
Result<int64,Error> writeBytes(List<uint8> bytes);
```

**行为**

写入所有字节，保留零字节。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Result<int64,Error>`。写入所有字节，保留零字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.writeBytes(bytes);
```

**源码**：[source/std/io.chl:133](../../source/std/io.chl#L133)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-17"></a>

### FileStream::readInto（18）

**声明**

```chali
Result<int64, Error> readInto(uint8[] mut& buffer);
```

**行为**

将字节读入可复用缓冲区，未读取的尾部保留原内容。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `buffer` | `uint8[] mut& buffer` | 运行时字节数组的可变引用。 | 无 |

**返回值**

`Result<int64, Error>`。实际读取字节数，EOF 时可返回零。发生 I/O 错误时可能已读取部分数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.readInto(buffer);
```

**源码**：[source/std/io.chl:143](../../source/std/io.chl#L143)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-18"></a>

### FileStream::writeFrom（19）

**声明**

```chali
Result<int64, Error> writeFrom(uint8[] const& buffer, int64 count);
```

**行为**

从可复用缓冲区写入指定长度的前缀。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `buffer` | `uint8[] const& buffer` | 运行时字节数组的只读引用。 | 无 |
| `count` | `int64 count` | 要写入的字节数，必须在零至缓冲区长度之间。 | 无 |

**返回值**

`Result<int64, Error>`。成功时为写入字节数。发生 I/O 错误时可能已写入部分数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.writeFrom(buffer, count);
```

**源码**：[source/std/io.chl:158](../../source/std/io.chl#L158)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-19"></a>

### FileStream::flush（20）

**声明**

```chali
Result<int64, Error> flush();
```

**行为**

刷新输出缓冲，失败返回 WriteFailed。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int64, Error>`。刷新输出缓冲，失败返回 WriteFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.flush();
```

**源码**：[source/std/io.chl:170](../../source/std/io.chl#L170)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-20"></a>

### FileStream::seek（21）

**声明**

```chali
Result<int64, Error> seek(int64 offset, SeekOrigin origin);
```

**行为**

按 origin 与 offset 定位，返回新位置或 SeekFailed。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `offset` | `int64 offset` | 字节偏移或 seek 相对位移 | 无 |
| `origin` | `SeekOrigin origin` | 定位基准或坐标原点 | 无 |

**返回值**

`Result<int64, Error>`。按 origin 与 offset 定位，返回新位置或 SeekFailed。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.seek(offset, origin);
```

**源码**：[source/std/io.chl:179](../../source/std/io.chl#L179)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-21"></a>

### FileStream::close（22）

**声明**

```chali
int64 close();
```

**行为**

关闭底层资源；文档节点会使同文档所有视图失效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。关闭底层资源；文档节点会使同文档所有视图失效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[source/std/io.chl:194](../../source/std/io.chl#L194)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-22"></a>

### fileOpen（23）

**声明**

```chali
Result<File, Error> fileOpen(String path, String mode);
```

**行为**

按路径和模式打开文件，返回 Result<File,Error>，失败为 Fail。

文件包装函数由 std::io 模块公开，使用前导入 std::io。 底层 __file* 入口由原生运行时提供。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `mode` | `String mode` | 宿主文件打开模式 | 无 |

**返回值**

`Result<File, Error>`。按路径和模式打开文件，返回 Result<File,Error>，失败为 Fail。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileOpen(path, mode);
```

**源码**：[source/std/io.chl:199](../../source/std/io.chl#L199)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-23"></a>

### fileRead（24）

**声明**

```chali
String fileRead(file const& handle);
```

**行为**

从已打开文件读取文本行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |

**返回值**

`String`。从已打开文件读取文本行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileRead(handle);
```

**源码**：[source/std/io.chl:207](../../source/std/io.chl#L207)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-24"></a>

### fileReadLine（25）

**声明**

```chali
String fileReadLine(File const& handle);
```

**行为**

读取文件句柄的一行文本，不含 LF/CRLF；保留既有 fileRead 语义。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `File const& handle` | 已打开的文件。 | 无 |

**返回值**

`String`。行文本，EOF 为空串，读取错误触发 panic。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
fileReadLine(handle);
```

**源码**：[source/std/io.chl:210](../../source/std/io.chl#L210)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-25"></a>

### fileReadString（26）

**声明**

```chali
String fileReadString(File const& handle, int64 count);
```

**行为**

从文件借用中读取至多 count 个字节的文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `File const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`String`。从文件借用中读取至多 count 个字节的文本。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
fileReadString(handle, count);
```

**源码**：[source/std/io.chl:216](../../source/std/io.chl#L216)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-26"></a>

### fileReadAllString（27）

**声明**

```chali
String fileReadAllString(File const& handle, int64 maxBytes = 16777216);
```

**行为**

从文件借用中读取剩余文本，并检查字节预算。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `File const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |
| `maxBytes` | `int64 maxBytes` | 最多接收字节数 | `16777216` |

**返回值**

`String`。从文件借用中读取剩余文本，并检查字节预算。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
fileReadAllString(handle, maxBytes);
```

**源码**：[source/std/io.chl:228](../../source/std/io.chl#L228)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-27"></a>

### fileWrite（28）

**声明**

```chali
int64 fileWrite(file const& handle, String text);
```

**行为**

向文件写入文本并返回宿主状态/计数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`int64`。向文件写入文本并返回宿主状态/计数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileWrite(handle, text);
```

**源码**：[source/std/io.chl:242](../../source/std/io.chl#L242)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-28"></a>

### filePrint（29）

**声明**

```chali
int64 filePrint(file const& handle, String text);
```

**行为**

将格式化文本写入文件，不追加换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 已打开的文件句柄。 | 无 |
| `text` | `String text` | 已格式化文本，调用时支持格式串与变长实参。 | 无 |

**返回值**

`int64`。写入字节数，失败沿用 fileWrite 的诊断。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
filePrint(handle, text);
```

**源码**：[source/std/io.chl:249](../../source/std/io.chl#L249)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-29"></a>

### filePrintln（30）

**声明**

```chali
int64 filePrintln(file const& handle, String text = "");
```

**行为**

将格式化文本和 LF 换行写入文件。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 已打开的文件句柄。 | 无 |
| `text` | `String text` | 已格式化文本，省略时只写换行。 | `""` |

**返回值**

`int64`。包括换行的写入字节数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
filePrintln(handle, text);
```

**源码**：[source/std/io.chl:256](../../source/std/io.chl#L256)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-30"></a>

### fileEof（31）

**声明**

```chali
int64 fileEof(file const& handle);
```

**行为**

检查文件是否到达 EOF。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |

**返回值**

`int64`。检查文件是否到达 EOF。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileEof(handle);
```

**源码**：[source/std/io.chl:259](../../source/std/io.chl#L259)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-31"></a>

### fileClose（32）

**声明**

```chali
int64 fileClose(file const& handle);
```

**行为**

关闭原生文件句柄并返回宿主状态码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `handle` | `file const& handle` | 本模块创建且仍有效的宿主句柄 | 无 |

**返回值**

`int64`。关闭原生文件句柄并返回宿主状态码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileClose(handle);
```

**源码**：[source/std/io.chl:260](../../source/std/io.chl#L260)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-32"></a>

### readAllBytes（33）

**声明**

```chali
Result<List<uint8>,Error> readAllBytes(String path, int64 maxBytes=16777216);
```

**行为**

读完整文件，保留零字节；默认最多 16 MiB，调用者可以指定预算。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `maxBytes` | `int64 maxBytes` | 最多接收字节数 | `16777216` |

**返回值**

`Result<List<uint8>,Error>`。读完整文件，保留零字节；默认最多 16 MiB，调用者可以指定预算。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
readAllBytes(path, maxBytes);
```

**源码**：[source/std/io.chl:274](../../source/std/io.chl#L274)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-33"></a>

### writeAllBytes（34）

**声明**

```chali
Result<int64,Error> writeAllBytes(String path, List<uint8> bytes);
```

**行为**

覆盖写完整文件，返回字节数量；不隐式创建父目录。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`Result<int64,Error>`。覆盖写完整文件，返回字节数量；不隐式创建父目录。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
writeAllBytes(path, bytes);
```

**源码**：[source/std/io.chl:294](../../source/std/io.chl#L294)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-34"></a>

### readAllText（35）

**声明**

```chali
Result<String,Error> readAllText(String path, int64 maxBytes=16777216);
```

**行为**

读取完整文本；String 不支持内嵌零字节，此类文件应使用 readAllBytes。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `maxBytes` | `int64 maxBytes` | 最多接收字节数 | `16777216` |

**返回值**

`Result<String,Error>`。读取完整文本；String 不支持内嵌零字节，此类文件应使用 readAllBytes。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
readAllText(path, maxBytes);
```

**源码**：[source/std/io.chl:308](../../source/std/io.chl#L308)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-35"></a>

### writeAllText（36）

**声明**

```chali
Result<int64,Error> writeAllText(String path, String text);
```

**行为**

覆盖写完整文本，不自动追加换行。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<int64,Error>`。覆盖写完整文本，不自动追加换行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
writeAllText(path, text);
```

**源码**：[source/std/io.chl:317](../../source/std/io.chl#L317)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-36"></a>

### listDirectory（37）

**声明**

```chali
Result<List<String>,Error> listDirectory(String path);
```

**行为**

列出一层目录的完整路径，按字节字典序排序，不递归。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`Result<List<String>,Error>`。列出一层目录的完整路径，按字节字典序排序，不递归。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
listDirectory(path);
```

**源码**：[source/std/io.chl:322](../../source/std/io.chl#L322)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-37"></a>

### fileInfo（38）

**声明**

```chali
Result<FileInfo,Error> fileInfo(String path);
```

**行为**

查询跟随符号链接后的文件信息；不存在时返回错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`Result<FileInfo,Error>`。查询跟随符号链接后的文件信息；不存在时返回错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
fileInfo(path);
```

**源码**：[source/std/io.chl:328](../../source/std/io.chl#L328)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-38"></a>

### createDirectories（39）

**声明**

```chali
Result<int64,Error> createDirectories(String path);
```

**行为**

创建目录及缺失的父目录，已存在的目录视为成功。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`Result<int64,Error>`。创建目录及缺失的父目录，已存在的目录视为成功。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
createDirectories(path);
```

**源码**：[source/std/io.chl:336](../../source/std/io.chl#L336)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-39"></a>

### renamePath（40）

**声明**

```chali
Result<int64,Error> renamePath(String from, String to);
```

**行为**

按宿主文件系统规则重命名；目标覆盖规则由平台决定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `from` | `String from` | 被替换的子串 | 无 |
| `to` | `String to` | 替换文本 | 无 |

**返回值**

`Result<int64,Error>`。按宿主文件系统规则重命名；目标覆盖规则由平台决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
renamePath(from, to);
```

**源码**：[source/std/io.chl:342](../../source/std/io.chl#L342)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-40"></a>

### copyTo（41）

**声明**

```chali
Result<int64,Error> copyTo(FileStream mut& source, FileStream mut& destination);
```

**行为**

从当前读位置复制到目标流，复用一个缓冲区，成功返回总字节数。两端保持打开。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `source` | `FileStream mut& source` | 输入游标、存储或事件来源值 | 无 |
| `destination` | `FileStream mut& destination` | 可写目标存储首地址 | 无 |

**返回值**

`Result<int64,Error>`。从当前读位置复制到目标流，复用一个缓冲区，成功返回总字节数。两端保持打开。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
copyTo(source, destination);
```

**源码**：[source/std/io.chl:350](../../source/std/io.chl#L350)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-41"></a>

### walkDirectory（42）

**声明**

```chali
Result<List<String>,Error> walkDirectory(String path);
```

**行为**

递归列出目录后代，按 UTF-8 路径排序，不递归进入符号链接。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`Result<List<String>,Error>`。递归列出目录后代，按 UTF-8 路径排序，不递归进入符号链接。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
walkDirectory(path);
```

**源码**：[source/std/io.chl:370](../../source/std/io.chl#L370)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-42"></a>

### createTempFile（43）

**声明**

```chali
Result<String,Error> createTempFile(String path);
```

**行为**

在目录内独占创建空临时文件，成功后调用者拥有路径。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 文件/数据库路径 | 无 |

**返回值**

`Result<String,Error>`。在目录内独占创建空临时文件，成功后调用者拥有路径。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
createTempFile(path);
```

**源码**：[source/std/io.chl:377](../../source/std/io.chl#L377)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

<a id="api-43"></a>

### replaceFile（44）

**声明**

```chali
Result<int64,Error> replaceFile(String from, String to);
```

**行为**

以同一文件系统内的普通文件原子替换目标，成功后来源路径消失。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `from` | `String from` | 被替换的子串 | 无 |
| `to` | `String to` | 替换文本 | 无 |

**返回值**

`Result<int64,Error>`。以同一文件系统内的普通文件原子替换目标，成功后来源路径消失。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
replaceFile(from, to);
```

**源码**：[source/std/io.chl:384](../../source/std/io.chl#L384)。

**完整示例**：[doc/examples/library/io.chl](../../doc/examples/library/io.chl)。

## 补充完整示例

[example/samples/std_file_strings.chl](../../example/samples/std_file_strings.chl)。

```chali
import std::io;

void main(List<String> args) {
    String path=args[0];
    FileStream output=FileStream::open(path,"wb").take();
    output.write("abc\r\nline two\nlast").unwrap();
    output.close();
    FileStream input=FileStream::open(path,"rb").take();
    assert(input.readString(2).unwrap()=="ab");
    assert(input.readLine().unwrap()=="c");
    assert(input.readLine().unwrap()=="line two");
    assert(input.readAllString().unwrap()=="last");
    assert(input.readLine().unwrap()=="");
    input.close();
    File handle=fileOpen(path,"rb").take();
    assert(fileReadString(&handle,3)=="abc");
    assert(fileReadLine(&handle)=="");
    assert(fileReadAllString(&handle)=="line two\nlast");
    fileClose(&handle);
    println("file strings ok");
}
```

## 补充完整示例

[example/samples/std_file_buffer.chl](../../example/samples/std_file_buffer.chl)。

```chali
import std::io;

void main(List<String> args) {
    String path = args[0];
    uint8 bytes[] = new[65539];
    bytes[0] = (uint8)7;
    bytes[65537] = (uint8)255;
    FileStream output = FileStream::open(path, "wb").take();
    assert(output.readInto(&bytes).isErr());
    assert(output.writeFrom(&bytes, -1).isErr());
    assert(output.writeFrom(&bytes, 65540).isErr());
    assert(output.writeFrom(&bytes, 0).unwrap() == 0);
    assert(output.writeFrom(&bytes, 65538).unwrap() == 65538);
    assert(output.close() == 0);

    FileStream input = FileStream::open(path, "rb").take();
    uint8 empty[] = new[0];
    assert(input.readInto(&empty).unwrap() == 0);
    bytes[65538] = (uint8)99;
    assert(input.readInto(&bytes).unwrap() == 65538);
    assert(bytes[0] == (uint8)7 && bytes[1] == (uint8)0);
    assert(bytes[65537] == (uint8)255 && bytes[65538] == (uint8)99);
    assert(input.readInto(&bytes).unwrap() == 0);
    assert(input.close() == 0);
    assert(input.readInto(&bytes).isErr());
    println("reusable byte buffer OK");
}
```
