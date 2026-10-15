# Any、显式泛型与通用数据工具

[返回文档目录](README.md) · [Any、显式泛型与通用数据工具](en/22-any-generics-data.md)

编译器与运行时资产必须匹配；修改类型契约后需重新编译源码、生成宿主胶水并重建缓存。

## Any 与类型判断

公开类型 `Object` 改为 `Any`，核心模块自动加载，通常直接写 `Any`。装箱的复制、析构、引用和接口转换继续沿用值语义与借用检查；更名不允许把局部对象地址保存到更长寿命的盒子。

`nameof` 与盒子的 `.name()` 已删除。运行时内部标签不再作为应用 API；它们只在一次编译及其匹配运行时内有意义，不应写入文件、网络协议或数据库。

| 已移除写法 | 当前写法 |
| --- | --- |
| `Object value` | `Any value` |
| `value.name() == nameof(int64)` | `isType<int64>(&value)` |
| `value.name() == 0` | `value.isEmpty()` |
| 判断后调用 `getValue()` | 优先 `tryGet<T>(&value)`，失败返回 `None` |
| 按接口或具体类型借用 | `tryGetReference<T>(&value)` 或 `tryGetMut<T>(&value)` |

```magel
int64 main() {
    Any value;
    assert(value.isEmpty());
    value.set((int64)7);
    assert(isType<int64>(&value));
    Option<int64> number = tryGet<int64>(&value);
    assert(number.unwrap() == 7);
    assert(!tryGet<String>(&value).isValue());
    println("Any: {}", number.unwrap());
    return 0;
}
```

`isType<T>` 判断保存的精确类型，不隐式进行数值转换。`tryGet<T>` 返回拥有值的副本；只读／可写引用使用对应的借用接口，引用不能超过盒子和原始对象的寿命。接口借用可以接受实现该接口的目标，遵守现有接收者可变性规则。已知提取一定成功时仍可使用 `getValue`／`getReference`；不匹配会报告运行错误。

需要稳定的元数据身份、字段、方法或注解时使用 `std::reflect`。其 `getTypeId<T>()` 仍要求反射构建和 `--reflection`，与 Any 的内部运行期分派标签分开。私有 `__typeTag` 仅供核心库和反射生成代码使用，应用不能直接调用。

## 泛型参数必须明确

省略类型实参不再自动补成 Any。允许函数类型推导，也允许模板声明显式给出默认类型。

```magel
struct Box<T> { T value; };
struct ExplicitDefault<T = Any> { T value; };
int64 main() {
    Box<int64> number;
    number.value = 7;
    Box<Any> dynamic;
    dynamic.value.set("explicit");
    ExplicitDefault<> chosen;
    chosen.value.set(true);
    return 0;
}
```

例如 `Pair<int64>` 不能再省略第二个类型参数，应写成 `Pair<int64, String>` 或确实需要类型擦除时写成 `Pair<int64, Any>`。

## 新模块

| 模块 | 首批功能 | 错误边界与平台 |
| --- | --- | --- |
| [`std::encoding`](library/std-encoding.md) | Base64、Base64URL、十六进制 | 严格解码返回 `Result`，拒绝非法填充与非零尾部位；纯 Magel |
| [`std::csv`](library/std-csv.md) | 文本读写、转义、分隔符、位置诊断 | 引号字段可含换行；位置为一基行号和字节列；纯 Magel |
| [`std::datetime`](library/std-datetime.md) | Unix 秒、UTC 日期、解析格式化、日期运算 | 公历年份 1..9999，ISO 秒精度 UTC；换算为纯 Magel，当前时间需要宿主时钟 |
| [`std::io`](library/std-io.md) | 字节流、整文件、目录、文件信息、创建与重命名 | `FileStream` 字节接口走 FileApi；路径操作使用桌面宿主文件系统 |
| [`std::cli`](library/std-cli.md) | 长短选项、位置参数、默认值、帮助、校验 | 不自动打印或退出，应用读取解析结果；纯 Magel |
| [`std::utf8`](library/std-utf8.md) | 校验、码点编解码、数量、截取 | 严格 UTF-8；码点不是字素簇；字节接口完整支持 U+0000 |

没有名称冲突时示例优先使用简写。完整可运行程序和断言见各模块参考页。

```magel
import std::encoding;
import std::datetime;
import std::utf8;
int64 main() {
    List<uint8> bytes = decodeHex("666f6f").unwrap();
    println("{}", encodeBase64(bytes));
    DateTime utc = parseUtc("2024-02-29T12:34:56Z").unwrap();
    println("{}", formatUtc(addDays(utc, 1).unwrap()).unwrap());
    println("{}", sliceCodePoints("A中文😀", 1, 2).unwrap());
    return 0;
}
```

## 文件和板端边界

`FileStream.readBytes(count)` 在 EOF 返回短列表，`writeBytes` 保留零字节；失败返回 `IoError`。桌面、MCU stdio 和 FatFS 适配提供字节读取；自定义旧 FileApi 后端未实现字节读取时明确报告不支持。

`readAllBytes` 默认预算为 16 MiB，可显式传入上限。`readAllText` 读取完整内容而非一行，拒绝内嵌零字节；二进制文件必须使用字节接口。`listDirectory` 只枚举一层并排序；`fileInfo` 跟随符号链接；`renamePath` 的目标覆盖规则遵从平台。文件操作不承诺事务或原子写入。

MCU 的文件系统能力由 FileApi 注入；桌面目录操作不会自动出现在板端。日期换算不依赖操作系统时区，`nowUtc` 的宿主必须提供真实 UTC 时钟。

## 二进制中间产物与体积实测

默认 `emit` 输出 `.quad`（MGQI v1）、`.layout`（MGLY v1）与 `.data.bin`。四元式的操作符、参数和源文件名使用共享字符串表；数量、索引、位置使用无符号 LEB128。布局只保存运行所需的符号、偏移、容量和步长。`.data.bin` 保留原始内存映像，不再重复写 `.data` 文本或 `.data.h` C 数组；重新生成同前缀产物时会清理旧副本。

VM 与 AOT 导出器自动识别新格式，并继续接受旧文本四元式和布局。格式带版本号，解码器检查整数溢出、截断、越界字符串引用及尾部数据。二进制编码不包含加密，也不作为保护源码的承诺。

2026-10-13 在 Windows / MinGW `-O2` 下，对同一份内容编码前后比较，单位为字节：

| 示例 | 文本 quad → 二进制 quad | 文本 layout → 二进制 layout | 原四文件总量 → 新三文件总量 | 减少 |
| --- | ---: | ---: | ---: | ---: |
| std_io_extended | 903,013 → 216,295 | 33,548 → 2,038 | 976,988 → 223,797 | 77.1% |
| std_cli | 764,630 → 184,923 | 26,979 → 2,574 | 822,671 → 191,457 | 76.7% |

原总量包括 `.quad`、`.layout`、`.data`、`.data.bin`，新总量包括 `.quad`、`.layout`、`.data.bin`；`.data.bin` 分别为 5,464 / 3,960 字节，未改变。未计入旧 `.data.h`，因此没有夸大删除重复数组带来的收益。

`library/stackvm/test_artifacts.cpp` 在内存中反复解码至少 250 ms，包含结构分配和释放，排除磁盘、源码编译与 C++ 编译。首轮均次结果：

| 示例 | quad 文本 / 二进制（µs） | layout 文本 / 二进制（µs） |
| --- | ---: | ---: |
| std_io_extended | 19,213.7 / 2,766.85（约 6.9 倍） | 300.935 / 3.969（约 75.8 倍） |
| std_cli | 16,294.9 / 2,398.31（约 6.8 倍） | 284.044 / 4.734（约 60.0 倍） |

这些数据描述解码环节，不能直接解释为整体编译速度。CMake 的 `magel-artifact-tests` 目标默认检查小型夹具；传入旧文本 `.quad` 和 `.layout` 路径可复测其他内容。测试同时核对四元式、源码位置、组装指令及布局往返等价性。
