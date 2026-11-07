# std::compress — 无损压缩

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-compress.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::compress;
```

## 公共约定与错误边界

compress 输入文本，输出二进制 List<uint8>；不能把压缩结果当 UTF-8 文本输出。decompress 对无效编码或损坏数据报告宿主运行错误，不返回 Result。 lztext 内核通过 C 帧和公开二进制服务接入，压缩字节和原始解压内容保留；不经过旧 C++ 模板封送。

## 参考代码

完整程序：[example/samples/std_compress.chl](../../example/samples/std_compress.chl)。

```powershell
chali run example/samples/std_compress.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::compress;
import std::text;

/* std::compress —— 文本压成字节序列、再解回来（底层是仓库自带的 lztext）。
 * 压缩结果是二进制（含 NUL 字节），类型是 List<uint8>，不要当字符串拼接。 */
void main() {
    /* 高重复度文本：应该明显变小 */
    String text = Text::repeat("chali 文本压缩往返测试；", 40);
    List<uint8> packed = compress(text);
    println("原文 {} 压缩后 {} 变小 {}", len(text), packed.len(),
    packed.len() < len(text));
    String back = decompress(packed);
    assert(back == text);
    println("往返一致 {}", back == text);

    /* 空串与单字符：都要能原样解回来 */
    List<uint8> empty = compress("");
    assert(decompress(empty) == "");
    List<uint8> one = compress("a");
    assert(decompress(one) == "a");
    println("空串 {} 单字符 {}", empty.len(), one.len());

    /* 压不出收益时自动退回原样存储：体积不降，但解压照常正确 */
    String noise = "9f3a!Q7z@1mKp2Vb8Xc4Rt6Yh0Nj5Lw3Sg";
    List<uint8> packedNoise = compress(noise);
    assert(decompress(packedNoise) == noise);
    println("噪声 原文 {} 压缩后 {}", len(noise), packedNoise.len());
}
```

</details>

<details>
<summary>示例输出</summary>

```text
原文 1320 压缩后 179 变小 1
往返一致 1
空串 6 单字符 7
噪声 原文 34 压缩后 40
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [compress](#api-0) | `List<uint8> compress(String text)` |
| [decompress](#api-1) | `String decompress(List<uint8> bytes)` |

<a id="api-0"></a>

### compress（1）

**声明**

```chali
List<uint8> compress(String text);
```

**行为**

文本压缩与解压：要 `import std::compress;` 才可见。 lztext 内核通过 C 帧及公开的字符串／字节列表服务接入。 压缩结果是**二进制**（含 NUL 字节），所以类型是 std::core::List<uint8>， 不要当字符串打印或拼接。解压出来的才是 std::core::String。 注意：不保证一定比原文短——随机数据、极短文本这类压不出收益时， 底层会自动按原样存储，体积不变但解压照常正确。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`List<uint8>`。文本压缩与解压：要 `import std::compress;` 才可见。 lztext 内核通过 C 帧及公开的字符串／字节列表服务接入。 压缩结果是**二进制**（含 NUL 字节），所以类型是 std::core::List<uint8>， 不要当字符串打印或拼接。解压出来的才是 std::core::String。 注意：不保证一定比原文短——随机数据、极短文本这类压不出收益时， 底层会自动按原样存储，体积不变但解压照常正确。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
compress(text);
```

**源码**：[source/std/compress.chl:12](../../source/std/compress.chl#L12)。

**完整示例**：[example/samples/std_compress.chl](../../example/samples/std_compress.chl)。

<a id="api-1"></a>

### decompress（2）

**声明**

```chali
String decompress(List<uint8> bytes);
```

**行为**

将有效 lztext 二进制数据还原为文本；损坏时报运行错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bytes` | `List<uint8> bytes` | 输入或输出字节存储 | 无 |

**返回值**

`String`。将有效 lztext 二进制数据还原为文本；损坏时报运行错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
decompress(bytes);
```

**源码**：[source/std/compress.chl:13](../../source/std/compress.chl#L13)。

**完整示例**：[example/samples/std_compress.chl](../../example/samples/std_compress.chl)。
