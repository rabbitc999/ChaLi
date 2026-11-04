# 库包格式与资产检查

<!-- LANGUAGE NAVIGATION -->
[English](en/44-library-packages.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

`.chlib` 在单个文件中保存接口、目标资产及依赖身份。用 `chali lib template` 生成清单，用 `pack` 封装已有资产，用 `inspect` 检查格式并输出元数据。普通 ChaLi 导入读取 `.chl` 源码。链接原生代码时，直接把对象或归档交给链接器，并在源码里声明原生入口。

## 生成与检查

先准备 `public.chl` 和一个现有原生归档。例如接口可以声明最终链接提供的 C 符号：

```chali
module example::math;
#abi("C", "@link", "add")
int64 add(int64 a, int64 b);
```

下面以 Windows x64 为例，`build` 目录须已存在：

```sh
chali ar build/add.obj --out build/libmath.a
chali lib template --module example::math --package-version 1.2.3 --target windows-x64 --license Apache-2.0 --out build/math.manifest.json
chali lib pack --manifest build/math.manifest.json --entry "interface:interface/main.chl=public.chl" --entry "archive:native/x86_64-w64-windows-gnu/library.a=build/libmath.a" --out build/math.chlib
chali lib inspect build/math.chlib --module example::math --package-version 1.2.3 --target windows-x64
```

`template` 不给 `--out` 时输出规范单行 JSON。目标使用规范三元组，清单内的实现名与示例中的归档条目名一致。`pack` 读取这份规范清单；清单的字段顺序、集合排序与 JSON 编码属于格式的一部分，输出末尾允许 CR／LF 换行。资产参数是 `KIND:NAME=FILE`：`NAME` 是包内名字，`FILE` 是真实输入路径，二者分别处理。可重复指定资产，实际封装顺序按名字排序。

`inspect` 的报告包含整包 SHA-256、清单和每条资产的名字、种类、长度、负载 SHA-256。它不打印源文或二进制负载。`--module`、`--package-version` 可核对预期身份，`--target`／`--cpu` 可核对原生选择。不给目标时可以检查含交叉目标的包；检查成功不表示已在目标系统运行。

## 独立的兼容信息

| 清单信息 | 核对规则 |
| --- | --- |
| 容器格式 | 文件头当前为 1；其他格式拒绝 |
| 接口格式、语言 ABI、运行时 ABI | 分别记录，当前各为 1；不互相代替 |
| 编译器版本 | 当前要求与 `VERSION` 完全相同，不静默加载较旧资产 |
| 模块与包版本 | 完整模块名和精确版本；依赖还包含整个依赖包的 SHA-256 |
| 目标三元组 | 只接受规范的 Windows x64、Linux x64、Linux ARM64 三元组；每个目标至多一个实现 |
| 对象格式、位宽、端序、布局 | COFF／ELF 与目标一致，当前为 64 位小端，数据布局与登记的目标运行时一致 |
| CPU 与特性 | 默认 CPU 实现可供同架构目标使用；专用 CPU 要求匹配，未明确启用的必需特性拒绝 |
| 原生调用约定 | 清单独立记录原生 ABI、接口版本和运行时版本。模板生成 `c-v1`。 |
| bitcode | 可记录独立资产和精确 LLVM 版本；原生选择不把它当作机器对象 |
| 可忽略扩展 | 只放在 `extensions` 对象内；未知语义字段、必需能力或保留标志拒绝 |

`template` 可重复使用 `--dependency "MODULE=VERSION#SHA256"` 记录精确依赖，使用 `--system-library NAME` 记录链接库，使用 `--runtime-library NAME` 记录应用需要的动态库。格式检查核对这些记录的合法性；依赖的文件解析与最终交付需要由使用这些资产的构建流程完成。

许可证名称通过 `--license` 记录。`--license-asset legal/LICENSE.txt` 记录包内许可证资产名，封装时再加入 `--entry "license:legal/LICENSE.txt=LICENSE"`。可包含多个许可证与署名文件。编译器工具链依赖、静态链接输入和应用运行时动态库分别记录，原生应用不因生产端内置 LLVM 就需要携带 LLVM DLL。

## 二进制编码

整数明确使用小端编码，不直接写 C++ 结构体、指针或 `size_t`。摘要保存原始 32 字节。SHA-256 的消息规则依据 [NIST FIPS 180-4](https://csrc.nist.gov/pubs/fips/180-4/final)；摘要检查内容变化，来源认证需要独立的签名或可信发布渠道。

| 文件头偏移 | 宽度 | 含义 |
| --- | --- | --- |
| 0 | 8 | ASCII `CHLIB1\r\n` |
| 8 | 4 | 容器版本 1 |
| 12 | 4 | 条目数量 |
| 16 | 8 | 规范清单字节数 |
| 24 | 32 | 清单 SHA-256 |
| 56 | 清单长度 | 无 BOM 的 UTF-8 规范 JSON |

清单后依次保存所有条目。每条是：`u16 名字长度`、`u16 种类`、`u32 标志`、`u64 负载长度`、`32 字节记录摘要`、`名字`、`负载`。记录摘要覆盖前述 16 字节元数据、名字和完整负载，因此名字、种类与长度的变化也会被检测。没有填充字节或隐式宿主对齐。名字必须严格递增；重复、越界段、空路径段和尾部未登记数据均拒绝。

| 种类编号 | 含义 |
| --- | --- |
| 1 | 语言接口 |
| 2 | 未实例化模板资产 |
| 3／4 | 原生对象／自包含归档 |
| 5／6 | bitcode／独立字节码资产 |
| 7／8 | 显式源码资产／许可证与署名 |
| 9 | 单元数据及布局资产 |

标志位 0 表示未知种类的条目可以忽略；其他位保留。被清单引用的接口、实现和许可证必须存在且种类正确。bitcode／字节码标签只标识容器负载，`inspect` 成功不表示这些字节已经通过相应执行后端的验证。`source` 只记录显式源码资产，原生目标缺失时不会自动使用它。

单包最多 256 MiB，清单最多 1 MiB，条目最多 4096 个，包内名字最多 240 字节；文本资产最多 32 MiB 且不能包含 NUL。目标最多 64 项，依赖与各文本集合最多 1024 项。按实际剩余文件长度核对声明长度后才分配负载。这些是编码及有效载荷预算，不是编译器进程总内存上限。

原生负载在已经摘要核对的拥有型字节上继续检查目标头和归档成员边界，不重新打开包来判定实现身份。符号索引、节、重定位和实际原生签名仍由对应工具链及互调契约核对。薄归档、嵌套归档与错误架构拒绝。

## 失败与复现

库包先写入根 `tmp/` 下的独立工作区，成功后才发布最终文件。格式或对象检查失败时保留原产物；命令行封装禁止输出覆盖输入清单或任一资产。读取失败不发布半个恢复结果。摘要、版本、目标或必需能力错误不会静默选择另一个实现。

相关回归：

```sh
python tests/chali_digest.py
python tests/chali_library_package.py --chali PATH
python tests/chali_library_package.py --chali STATIC_PATH --other-chali EXTERNAL_PATH
```

测试独立按公开编码和 `hashlib` 交叉核对摘要，覆盖分块与填充边界、规范编码、损坏、长度预算、可忽略条目、三目标选择和事务发布；包内归档恢复后使用独立链接器实际运行。交叉目标只检查生成与格式，宿主执行单独记录。
