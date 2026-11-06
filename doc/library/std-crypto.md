# std::crypto — 密码学

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-crypto.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::crypto;
```

## 公共约定与错误边界

使用 libsodium 提供安全随机数、SHA-256、HMAC-SHA-256 和 Ed25519 验签。String 按 UTF-8 字节及其显式长度处理；摘要和随机输出为小写十六进制。randomHex、sha256 和 hmacSha256 通过 Result 报告失败；verifyEd25519 对格式错误、非法密钥或签名不匹配返回 false。随机字节数为 1–65536，Ed25519 公钥为 64 个十六进制字符，分离签名为 128 个十六进制字符。模块及其原生静态归档随支持原生服务的发行版内嵌。

## 参考代码

完整程序：[example/crypto/demo.chl](../../example/crypto/demo.chl)。

```powershell
chali run example/crypto/demo.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::crypto;
void main() {
    assert(std::crypto::sha256("abc").unwrap() == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    assert(std::crypto::hmacSha256("The quick brown fox jumps over the lazy dog","key").unwrap()
        == "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8");
    String key = "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a";
    String sig = "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e06522490155"
        + "5fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b";
    assert(std::crypto::verifyEd25519("",key,sig));
    assert(!std::crypto::verifyEd25519("changed",key,sig));
    assert(std::crypto::randomHex(0).isErr());
    assert(len(std::crypto::randomHex(16).unwrap()) == 32);
    println("crypto OK");
}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [randomHex](#api-0) | `Result<String,Error> randomHex(int64 count)` |
| [sha256](#api-1) | `Result<String,Error> sha256(String input)` |
| [hmacSha256](#api-2) | `Result<String,Error> hmacSha256(String input, String key)` |
| [verifyEd25519](#api-3) | `bool verifyEd25519(String input, String public_key, String signature)` |

<a id="api-0"></a>

### randomHex（1）

**声明**

```chali
Result<String,Error> randomHex(int64 count);
```

**行为**

生成 count 个安全随机字节并返回小写十六进制文本。

生成 count 个安全随机字节，以小写十六进制返回。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `count` | `int64 count` | 安全随机字节数量。 | 无 |

**返回值**

`Result<String,Error>`。生成 count 个安全随机字节并返回小写十六进制文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::crypto::randomHex(count);
```

**源码**：[library/crypto/crypto.chl.in:13](../../library/crypto/crypto.chl.in#L13)。

**完整示例**：[example/crypto/demo.chl](../../example/crypto/demo.chl)。

<a id="api-1"></a>

### sha256（2）

**声明**

```chali
Result<String,Error> sha256(String input);
```

**行为**

计算 input 的 SHA-256 并返回小写十六进制摘要。

计算 UTF-8 String 的 SHA-256，小写十六进制编码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `String input` | 参与摘要或验签的原始 UTF-8 文本。 | 无 |

**返回值**

`Result<String,Error>`。计算 input 的 SHA-256 并返回小写十六进制摘要。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::crypto::sha256(input);
```

**源码**：[library/crypto/crypto.chl.in:20](../../library/crypto/crypto.chl.in#L20)。

**完整示例**：[example/crypto/demo.chl](../../example/crypto/demo.chl)。

<a id="api-2"></a>

### hmacSha256（3）

**声明**

```chali
Result<String,Error> hmacSha256(String input, String key);
```

**行为**

以 key 为密钥计算 input 的 HMAC-SHA-256，返回小写十六进制摘要。

计算 HMAC-SHA-256，结果采用小写十六进制。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `String input` | 参与摘要或验签的原始 UTF-8 文本。 | 无 |
| `key` | `String key` | HMAC 密钥或按键码，按本项签名与行为解释。 | 无 |

**返回值**

`Result<String,Error>`。以 key 为密钥计算 input 的 HMAC-SHA-256，返回小写十六进制摘要。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::crypto::hmacSha256(input, key);
```

**源码**：[library/crypto/crypto.chl.in:27](../../library/crypto/crypto.chl.in#L27)。

**完整示例**：[example/crypto/demo.chl](../../example/crypto/demo.chl)。

<a id="api-3"></a>

### verifyEd25519（4）

**声明**

```chali
bool verifyEd25519(String input, String public_key, String signature);
```

**行为**

使用十六进制 Ed25519 公钥验证分离签名；格式不合法或验证失败返回 false。

验证十六进制 Ed25519 公钥和分离签名。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `String input` | 参与摘要或验签的原始 UTF-8 文本。 | 无 |
| `public_key` | `String public_key` | 64 个十六进制字符表示的 32 字节 Ed25519 公钥。 | 无 |
| `signature` | `String signature` | 128 个十六进制字符表示的 64 字节 Ed25519 分离签名。 | 无 |

**返回值**

`bool`。使用十六进制 Ed25519 公钥验证分离签名；格式不合法或验证失败返回 false。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::crypto::verifyEd25519(input, public_key, signature);
```

**源码**：[library/crypto/crypto.chl.in:34](../../library/crypto/crypto.chl.in#L34)。

**完整示例**：[example/crypto/demo.chl](../../example/crypto/demo.chl)。
