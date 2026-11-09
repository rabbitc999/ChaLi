# std::json — JSON 文档与反射编解码

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-json.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::json;
```

## 公共约定与错误边界

parse 返回 Result<JsonValue,MessageError>。节点由拥有型 opcode 保存独立子树；普通复制深复制，close 只关闭当前值，已取得的子值和副本继续有效，重复关闭返回 false。asXxx 返回 Option，不符合类型时为 None；asFloat64 可读取整数。含零字节的文本/键使用 asBytes/findBytes/keyBytesAt。kind：0 null、1 bool、2 int、3 float、4 string、5 array、6 object、7 date-time。

## 参考代码

完整程序：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

```powershell
chali run example/samples/std_structured_data.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
/*
 * std_structured_data.chl —— `std::json` 与 `std::toml` 的读写样例。
 *
 * 本例将 find / keyAt / asString 的结果分别保存，方便逐层检查 Result/Option。
 * 当前语言也支持 root.find("x").unwrap() 等持资源结果的链式访问；编译器在完整
 * 表达式结束后清理中间值。节点拥有独立子树，close 不影响副本及已提取的子值。
 */
import std::json;
import std::toml;

void main() {
    Result<JsonValue, Error> parsed = std::json::parse(
    "{\"name\":\"中文\",\"items\":[1,2,3],\"active\":true,\"nothing\":null,"
    "\"ratio\":1.5,\"min\":-9223372036854775808}");
    JsonValue root = parsed.unwrap();
    assert(root.isObject() && root.len() == 6);
    Option<String> firstKey = root.keyAt(0);
    assert(firstKey.unwrap() == "name");
    Option<JsonValue> nameField = root.find("name");
    JsonValue nameValue = nameField.unwrap();
    Option<String> nameText = nameValue.asString();
    assert(nameText.unwrap() == "中文");

    Option<JsonValue> itemsField = root.find("items");
    JsonValue items = itemsField.unwrap();
    int64 sum = 0;
    for (JsonValue item in items) {
        Option<int64> itemNumber = item.asInt();
        sum = sum + itemNumber.unwrap();
    }
    assert(sum == 6 && items.at(-1).isNone() && items.at(3).isNone());
    Option<JsonValue> missingField = root.find("missing");
    assert(missingField.isNone());
    Option<JsonValue> activeField = root.find("active");
    JsonValue activeValue = activeField.unwrap();
    Option<bool> activeBool = activeValue.asBool();
    assert(activeBool.unwrap());
    Option<JsonValue> nothingField = root.find("nothing");
    JsonValue nothingValue = nothingField.unwrap();
    assert(nothingValue.isNull());
    Option<int64> nothingInt = nothingValue.asInt();
    assert(nothingInt.isNone());
    Option<JsonValue> ratioField = root.find("ratio");
    JsonValue ratioValue = ratioField.unwrap();
    Option<float64> ratioNumber = ratioValue.asFloat64();
    assert(ratioNumber.unwrap() == 1.5);
    Option<int64> ratioInt = ratioValue.asInt();
    assert(ratioInt.isNone());
    Option<JsonValue> minField = root.find("min");
    JsonValue minValue = minField.unwrap();
    Option<int64> minNumber = minValue.asInt();
    assert(format("{}", minNumber.unwrap()) == "-9223372036854775808");

    String rootText = root.dump();
    Result<JsonValue, Error> reparsed = std::json::parse(rootText);
    JsonValue reloaded = reparsed.unwrap();
    Option<JsonValue> reloadedName = reloaded.find("name");
    JsonValue reloadedNameValue = reloadedName.unwrap();
    Option<String> reloadedText = reloadedNameValue.asString();
    assert(reloadedText.unwrap() == "中文");
    assert(reloaded.close());
    assert(root.close() && items.isValid() && !root.close());

    Result<JsonValue, Error> parsedZero =
    std::json::parse("{\"a\\u0000b\":\"x\\u0000y\"}");
    JsonValue zero = parsedZero.unwrap();
    Option<String> zeroKey = zero.keyAt(0);
    assert(zeroKey.isNone());
    Option<List<uint8>> keyResult = zero.keyBytesAt(0);
    List<uint8> key = keyResult.unwrap();
    assert(key.len() == 3 && key[1] == (uint8)0);
    Option<JsonValue> zeroTextField = zero.findBytes(key);
    JsonValue zeroText = zeroTextField.unwrap();
    Option<String> zeroTextString = zeroText.asString();
    assert(zeroTextString.isNone());
    Option<List<uint8>> rawResult = zeroText.asBytes();
    List<uint8> raw = rawResult.unwrap();
    assert(raw.len() == 3 && raw[0] == (uint8)120 && raw[1] == (uint8)0 &&
    raw[2] == (uint8)121);
    assert(zero.close());
    Result<JsonValue, Error> badJson = std::json::parse("{\"x\":}");
    assert(badJson.isErr() && len(badJson.error().toString()) > 0);
    Result<JsonValue, Error> trailing = std::json::parse("[1,]");
    assert(trailing.isErr());

    Result<TomlValue, Error> parsedConfig = std::toml::parse(
    "title = \"ChaLi\"\nreleased = 1979-05-27T07:32:00Z\n[server]\nport = 8080\n"
    "enabled = true\nvalues = [1, 2, 3]\n[[products]]\nname = \"one\"\n"
    "[[products]]\nname = \"two\"\n");
    TomlValue config = parsedConfig.unwrap();
    assert(config.isTable());
    Option<String> titleKey = config.keyAt(0);
    assert(titleKey.unwrap() == "title");
    Option<TomlValue> titleField = config.find("title");
    TomlValue titleValue = titleField.unwrap();
    Option<String> titleText = titleValue.asString();
    assert(titleText.unwrap() == "ChaLi");
    Option<TomlValue> serverField = config.find("server");
    TomlValue server = serverField.unwrap();
    Option<TomlValue> portField = server.find("port");
    TomlValue portValue = portField.unwrap();
    Option<int64> portNumber = portValue.asInt();
    assert(portNumber.unwrap() == 8080);
    Option<TomlValue> enabledField = server.find("enabled");
    TomlValue enabledValue = enabledField.unwrap();
    Option<bool> enabledBool = enabledValue.asBool();
    assert(enabledBool.unwrap());
    Option<TomlValue> releasedField = config.find("released");
    TomlValue releasedValue = releasedField.unwrap();
    Option<String> releasedText = releasedValue.asDateTime();
    assert(releasedText.unwrap() == "1979-05-27T07:32:00Z");

    Option<TomlValue> productsField = config.find("products");
    TomlValue products = productsField.unwrap();
    assert(products.len() == 2);
    Option<TomlValue> secondProduct = products.at(1);
    TomlValue second = secondProduct.unwrap();
    Option<TomlValue> secondName = second.find("name");
    TomlValue secondNameValue = secondName.unwrap();
    Option<String> secondNameText = secondNameValue.asString();
    assert(secondNameText.unwrap() == "two");
    int64 members = 0;
    for (TomlValue item in server) { members = members + 1; }
    assert(members == 3);
    Option<String> missingMember = server.keyAt(3);
    assert(missingMember.isNone());

    String configText = config.dump();
    Result<TomlValue, Error> reparsedConfig = std::toml::parse(configText);
    TomlValue copy = reparsedConfig.unwrap();
    Option<TomlValue> copyServer = copy.find("server");
    TomlValue copyServerValue = copyServer.unwrap();
    Option<TomlValue> copyPort = copyServerValue.find("port");
    TomlValue copyPortValue = copyPort.unwrap();
    Option<int64> copyPortNumber = copyPortValue.asInt();
    assert(copyPortNumber.unwrap() == 8080);
    assert(copy.close() && config.close() && server.isValid());
    Result<TomlValue, Error> duplicate = std::toml::parse("x = 1\nx = 2\n");
    assert(duplicate.isErr() && len(duplicate.error().toString()) > 0);
    for (int64 i = 0; i < 50; i = i + 1) {

        Result<JsonValue, Error> parsedSmall = std::json::parse("[]");
        JsonValue small = parsedSmall.unwrap();
        assert(small.close());
        Result<TomlValue, Error> invalid = std::toml::parse("x = [");
        assert(invalid.isErr());

    }
    Result<JsonValue, Error> trailingOk = std::json::parse("true");
    assert(trailingOk.isOk()); // 注册表结束时清理未显式关闭的文档
    println("JSON and TOML parsing OK");
}
```

</details>

<details>
<summary>示例输出</summary>

```text
JSON and TOML parsing OK
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 宏定义

<a id="symbol-311"></a>

### `Json`

```chali
$macro Json<T>(encode:ident, decode:ident) -> member
```

为具体类型 T 生成实例编码方法和静态解码方法。两个名称由调用者指定，编解码使用反射并返回 Result。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-312"></a>`interface Json` | 结构化 JSON 编码协议；配套的成员宏可生成静态解码入口。 |
| 模块 | <a id="symbol-313"></a>`struct JsonValue : Iterator<JsonValue>` | 独立拥有 JSON 子树的值，关闭不影响副本或已提取子值。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [Json::toJson](#api-0) | `Result<String,Error> toJson()` |
| [serialize](#api-1) | `Result<String,Error> serialize<T>(T const& value)` |
| [deserialize](#api-2) | `Result<T,Error> deserialize<T>(String text)` |
| [JsonValue::isValid](#api-3) | `bool isValid()` |
| [JsonValue::close](#api-4) | `bool close()` |
| [JsonValue::kind](#api-5) | `int64 kind()` |
| [JsonValue::isNull](#api-6) | `bool isNull()` |
| [JsonValue::isBool](#api-7) | `bool isBool()` |
| [JsonValue::isInt](#api-8) | `bool isInt()` |
| [JsonValue::isFloat](#api-9) | `bool isFloat()` |
| [JsonValue::isString](#api-10) | `bool isString()` |
| [JsonValue::isArray](#api-11) | `bool isArray()` |
| [JsonValue::isObject](#api-12) | `bool isObject()` |
| [JsonValue::isTable](#api-13) | `bool isTable()` |
| [JsonValue::isDateTime](#api-14) | `bool isDateTime()` |
| [JsonValue::len](#api-15) | `int64 len()` |
| [JsonValue::asInt](#api-16) | `Option<int64> asInt()` |
| [JsonValue::asFloat64](#api-17) | `Option<float64> asFloat64()` |
| [JsonValue::asBool](#api-18) | `Option<bool> asBool()` |
| [JsonValue::asString](#api-19) | `Option<String> asString()` |
| [JsonValue::asBytes](#api-20) | `Option<List<uint8>> asBytes()` |
| [JsonValue::asDateTime](#api-21) | `Option<String> asDateTime()` |
| [JsonValue::find](#api-22) | `Option<JsonValue> find(String key)` |
| [JsonValue::findBytes](#api-23) | `Option<JsonValue> findBytes(List<uint8> key)` |
| [JsonValue::at](#api-24) | `Option<JsonValue> at(int64 index)` |
| [JsonValue::iterator](#api-25) | `Option<JsonValue> iterator(int64 index)` |
| [JsonValue::keyAt](#api-26) | `Option<String> keyAt(int64 index)` |
| [JsonValue::keyBytesAt](#api-27) | `Option<List<uint8>> keyBytesAt(int64 index)` |
| [JsonValue::dump](#api-28) | `String dump()` |
| [parse](#api-29) | `Result<JsonValue, Error> parse(String text)` |

<a id="api-0"></a>

### Json::toJson（1）

**声明**

```chali
Result<String,Error> toJson();
```

**行为**

将当前值编码为 JSON；失败返回统一 Error，具体字段格式由实现决定。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<String,Error>`。将当前值编码为 JSON；失败返回统一 Error，具体字段格式由实现决定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toJson();
```

**源码**：[source/std/json.chl:9](../../source/std/json.chl#L9)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-1"></a>

### serialize（2）

**声明**

```chali
Result<String,Error> serialize<T>(T const& value);
```

**行为**

按具体反射类型编码独立值，失败返回诊断。

将已暴露给反射的值编码为 JSON，数字和字段类型保持精确。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T const& value` | 输入值或写入的新值 | 无 |

**返回值**

`Result<String,Error>`。按具体反射类型编码独立值，失败返回诊断。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
serialize<T>(value);
```

**源码**：[source/std/json.chl:27](../../source/std/json.chl#L27)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-2"></a>

### deserialize（3）

**声明**

```chali
Result<T,Error> deserialize<T>(String text);
```

**行为**

按具体反射类型严格校验并恢复独立拥有的值。

按具体类型验证并恢复拥有独立存储的值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<T,Error>`。按具体反射类型严格校验并恢复独立拥有的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
deserialize<T>(text);
```

**源码**：[source/std/json.chl:34](../../source/std/json.chl#L34)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-3"></a>

### JsonValue::isValid（4）

**声明**

```chali
bool isValid();
```

**行为**

判断共享文档是否仍有效。

检查当前值是否有效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断共享文档是否仍有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[source/std/json.chl:75](../../source/std/json.chl#L75)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-4"></a>

### JsonValue::close（5）

**声明**

```chali
bool close();
```

**行为**

关闭底层资源；文档节点会使同文档所有视图失效。

关闭当前值；副本与已提取的子值继续有效，重复关闭返回 false。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。关闭底层资源；文档节点会使同文档所有视图失效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[source/std/json.chl:77](../../source/std/json.chl#L77)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-5"></a>

### JsonValue::kind（6）

**声明**

```chali
int64 kind();
```

**行为**

返回节点类型编号。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回节点类型编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.kind();
```

**源码**：[source/std/json.chl:78](../../source/std/json.chl#L78)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-6"></a>

### JsonValue::isNull（7）

**声明**

```chali
bool isNull();
```

**行为**

判断是否为 null/NULL。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为 null/NULL。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isNull();
```

**源码**：[source/std/json.chl:79](../../source/std/json.chl#L79)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-7"></a>

### JsonValue::isBool（8）

**声明**

```chali
bool isBool();
```

**行为**

判断是否为布尔节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为布尔节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isBool();
```

**源码**：[source/std/json.chl:80](../../source/std/json.chl#L80)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-8"></a>

### JsonValue::isInt（9）

**声明**

```chali
bool isInt();
```

**行为**

判断是否为整数节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为整数节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isInt();
```

**源码**：[source/std/json.chl:81](../../source/std/json.chl#L81)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-9"></a>

### JsonValue::isFloat（10）

**声明**

```chali
bool isFloat();
```

**行为**

判断是否为浮点节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为浮点节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isFloat();
```

**源码**：[source/std/json.chl:82](../../source/std/json.chl#L82)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-10"></a>

### JsonValue::isString（11）

**声明**

```chali
bool isString();
```

**行为**

判断是否为字符串节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为字符串节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isString();
```

**源码**：[source/std/json.chl:83](../../source/std/json.chl#L83)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-11"></a>

### JsonValue::isArray（12）

**声明**

```chali
bool isArray();
```

**行为**

判断是否为数组节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为数组节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isArray();
```

**源码**：[source/std/json.chl:84](../../source/std/json.chl#L84)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-12"></a>

### JsonValue::isObject（13）

**声明**

```chali
bool isObject();
```

**行为**

判断是否为对象/表节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为对象/表节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isObject();
```

**源码**：[source/std/json.chl:85](../../source/std/json.chl#L85)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-13"></a>

### JsonValue::isTable（14）

**声明**

```chali
bool isTable();
```

**行为**

判断是否为表/对象节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为表/对象节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isTable();
```

**源码**：[source/std/json.chl:86](../../source/std/json.chl#L86)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-14"></a>

### JsonValue::isDateTime（15）

**声明**

```chali
bool isDateTime();
```

**行为**

判断是否为日期时间节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断是否为日期时间节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isDateTime();
```

**源码**：[source/std/json.chl:87](../../source/std/json.chl#L87)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-15"></a>

### JsonValue::len（16）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

数组元素数或对象成员数，标量为零。

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

**源码**：[source/std/json.chl:89](../../source/std/json.chl#L89)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-16"></a>

### JsonValue::asInt（17）

**声明**

```chali
Option<int64> asInt();
```

**行为**

提取整数值；具体转换规则见所属类型。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。提取整数值；具体转换规则见所属类型。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asInt();
```

**源码**：[source/std/json.chl:90](../../source/std/json.chl#L90)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-17"></a>

### JsonValue::asFloat64（18）

**声明**

```chali
Option<float64> asFloat64();
```

**行为**

提取浮点值，也接受整数节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<float64>`。提取浮点值，也接受整数节点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asFloat64();
```

**源码**：[source/std/json.chl:94](../../source/std/json.chl#L94)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-18"></a>

### JsonValue::asBool（19）

**声明**

```chali
Option<bool> asBool();
```

**行为**

以 Option/Result 提取布尔值，类型不符走失败分支。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<bool>`。以 Option/Result 提取布尔值，类型不符走失败分支。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asBool();
```

**源码**：[source/std/json.chl:98](../../source/std/json.chl#L98)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-19"></a>

### JsonValue::asString（20）

**声明**

```chali
Option<String> asString();
```

**行为**

提取不含零字节的字符串；否则 None。

非字符串或含零字节时返回 None；含零字节的文本使用 asBytes。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<String>`。提取不含零字节的字符串；否则 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asString();
```

**源码**：[source/std/json.chl:103](../../source/std/json.chl#L103)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-20"></a>

### JsonValue::asBytes（21）

**声明**

```chali
Option<List<uint8>> asBytes();
```

**行为**

提取字符串的原始字节，包括零字节。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<List<uint8>>`。提取字符串的原始字节，包括零字节。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asBytes();
```

**源码**：[source/std/json.chl:108](../../source/std/json.chl#L108)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-21"></a>

### JsonValue::asDateTime（22）

**声明**

```chali
Option<String> asDateTime();
```

**行为**

提取日期时间的文本表示。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<String>`。提取日期时间的文本表示。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asDateTime();
```

**源码**：[source/std/json.chl:113](../../source/std/json.chl#L113)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-22"></a>

### JsonValue::find（23）

**声明**

```chali
Option<JsonValue> find(String key);
```

**行为**

查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `String key` | 查找键或下标 | 无 |

**返回值**

`Option<JsonValue>`。查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.find(key);
```

**源码**：[source/std/json.chl:118](../../source/std/json.chl#L118)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-23"></a>

### JsonValue::findBytes（24）

**声明**

```chali
Option<JsonValue> findBytes(List<uint8> key);
```

**行为**

按原始字节键查找成员，失败 None。

允许包含零字节的 UTF-8 键名。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `List<uint8> key` | 查找键或下标 | 无 |

**返回值**

`Option<JsonValue>`。按原始字节键查找成员，失败 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.findBytes(key);
```

**源码**：[source/std/json.chl:125](../../source/std/json.chl#L125)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-24"></a>

### JsonValue::at（25）

**声明**

```chali
Option<JsonValue> at(int64 index);
```

**行为**

读取从 0 开始的指定位置。

数组按索引读取；对象按插入顺序读取成员值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<JsonValue>`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(index);
```

**源码**：[source/std/json.chl:132](../../source/std/json.chl#L132)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-25"></a>

### JsonValue::iterator（26）

**声明**

```chali
Option<JsonValue> iterator(int64 index);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<JsonValue>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(index);
```

**源码**：[source/std/json.chl:138](../../source/std/json.chl#L138)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-26"></a>

### JsonValue::keyAt（27）

**声明**

```chali
Option<String> keyAt(int64 index);
```

**行为**

按遍历位置读取键。

对象成员的键名；越界或键中含零字节时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<String>`。按遍历位置读取键。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.keyAt(index);
```

**源码**：[source/std/json.chl:140](../../source/std/json.chl#L140)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-27"></a>

### JsonValue::keyBytesAt（28）

**声明**

```chali
Option<List<uint8>> keyBytesAt(int64 index);
```

**行为**

按成员位置读取原始字节键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`Option<List<uint8>>`。按成员位置读取原始字节键。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.keyBytesAt(index);
```

**源码**：[source/std/json.chl:145](../../source/std/json.chl#L145)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-28"></a>

### JsonValue::dump（29）

**声明**

```chali
String dump();
```

**行为**

将节点序列化为文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。将节点序列化为文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.dump();
```

**源码**：[source/std/json.chl:150](../../source/std/json.chl#L150)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。

<a id="api-29"></a>

### parse（30）

**声明**

```chali
Result<JsonValue, Error> parse(String text);
```

**行为**

解析JSON 文本；成功返回根节点，失败返回诊断字符串。

解析文本；失败返回包含行列信息的诊断。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 输入文本，位置与长度通常按字节 | 无 |

**返回值**

`Result<JsonValue, Error>`。解析JSON 文本；成功返回根节点，失败返回诊断字符串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
parse(text);
```

**源码**：[source/std/json.chl:154](../../source/std/json.chl#L154)。

**完整示例**：[example/samples/std_structured_data.chl](../../example/samples/std_structured_data.chl)。
