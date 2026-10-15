# 09 工具链使用

AOT 可以显式选择 GCC 工具链，支持 PATH 中的命令、编译器完整路径和工具链目录：

```sh
magel aot app.mgl --gcc-toolchain g++ --out app.exe
magel aot app.mgl --gcc-toolchain "D:/GCC/bin/g++.exe" --out app.exe
magel aot app.mgl --gcc-toolchain "D:/GCC" --out app.exe
```

目录可以是含有 `bin/g++` 与 `bin/gcc` 的安装根目录，或直接指定 `bin` 目录。
`gcc` 命令会推导同目录的 `g++`；也可通过 `--cxx`、`--cc` 分别覆盖。
项目配置 `[magel] gcc_toolchain = "D:/GCC"` 提供默认值，命令行优先。
选择的工具链必须能构建当前桌面 AOT 宿主；无操作系统的 `none-eabi` 目标请使用独立 MCU VM。
路径或命令错误时直接报错，不回退到另一套编译器。

`magel` 是统一入口：解释、导出字节码、AOT、建工程、跑测试都在同一个程序里。

```sh
magel 文件.mgl [程序参数...]
magel 场景 [文件.mgl] [配置词...] [-- 程序参数...]
```

> Windows 上程序名是 `magel.exe`。Git Bash / MSYS 下写 `./magel` 能自动找到 `magel.exe`，光写 `magel` 会走 PATH 查找。

## AOT 库输出

库源码通过 `export` 选择公开的 C 接口，允许没有 `main`：

```magel
export int32 add(int32 left, int32 right) { return left + right; }
```

```sh
# Windows 工具链生成 DLL
magel aot math.mgl --out math.dll
# Linux 工具链生成 ELF 共享库
magel aot math.mgl --out math.so
# 使用所选工具链的目标格式生成可重定位文件
magel aot math.mgl --out math.o
```

扩展名自动选择类型；也可显式使用 `--shared` 或 `--object`。省略后缀时，
共享库按 GCC 目标补 `.dll` / `.so`，目标文件补 `.o`。相互冲突的选项报错。
`.dll` 要求 Windows GCC 目标，`.so` 要求 Linux GCC 目标，修改后缀不能转换平台 ABI。
不支持 `.lib` / `.a` 静态库输出。

三种产物均生成同名 `.h`，至少需要一个 `export`。宿主通过 `magel_create` 建立上下文，
调用 `magel_<函数名>`，最后 `magel_destroy`；状态码非零时用 `magel_last_error` 读取错误。
共享库可用系统动态加载器取得这些 C 符号。`.o` 是可重定位目标文件，需要宿主链接器处理，
不是动态加载器直接加载的共享库。可用同一 GCC 工具链将 `.o` 链成宿主程序或共享库；
最终链接需要 C++ 运行时及所用系统库，Linux 通常加 `-pthread -ldl`。
Windows 工具链若采用动态 C++ 运行时，加载 DLL 时还须能找到它实际引用的
`libstdc++`、`libgcc`、线程库等依赖 DLL；按该工具链的依赖部署或配置加载路径。

`--make` 与 `--separate-modules` 均支持库输出，模块模式会合并本次需要的模块和运行时目标。
库产物不定义宿主 `main`；源码若声明了 `main`，也不会被库初始化自动调用。
完整可执行 C 客户端与无入口源码见 [ABI 示例](examples/abi_contracts/)，回归见 [库输出测试](../tests/magel_aot_libraries.py)。

---

## 1. 场景指令

| 指令 | 作用 |
| --- | --- |
| `run` / `interpret` | 内部解释运行（缺省） |
| `check FILE [json]` | 仅检查，可输出 JSON 诊断 |
| `emit` | 只出 `.quad` / `.data.bin` / `.layout`，不执行 |
| `svmb` | 只导出 `<前缀>.svmb`，不执行 |
| `aot FILE [out EXE] [opt LEVEL] [checked\|unchecked]` | 导出 AOT（默认 `unchecked`） |
| `new DIR [name NAME]` | 新建应用工程（目标目录必须不存在） |
| `gen` | 生成独立编译器工程（转发 `magel-generator compiler`） |
| `clear out DIR` | 清理登记过的中间产物（不重新编译） |
| `lang zh\|en\|ja\|auto` | 选择显示语言 |
| `help` | 显示帮助 |

开发场景：`test tests backend all`、`fmt check src`、`deps lock project . offline`、`lsp`。

**直接给文件时，文件后面的全部内容都作为程序参数传给 `main`**：

```sh
magel app.mgl Alice "two words"
```

需要参数时把入口写成 `int64 main(List<String> arguments)`。

---

## 2. 常用选项

| 选项 | 作用 |
| --- | --- |
| `--out 前缀` | 产物文件名前缀（缺省 `build/magel_out`） |
| `--check` | 仅检查，不执行、不生成产物 |
| `--emit` | 只生成 `.quad` / `.data.bin` / `.layout`，不执行 |
| `--svmb` | 额外写出可由 StackVM 直接加载的 `<前缀>.svmb` |
| `--debug` | 写 `.svmb` 时带调试节；解释执行时打印位置轨迹 |
| `--dap` | 作为调试适配器运行（stdio 收发 DAP） |
| `--compress-symbols` | 写无名变长符号表，生成 PC 映射文件 |
| `--no-compress-symbols` | 覆盖配置，关闭符号压缩 |
| `--symbol-map FILE` | 映射路径（默认 `<前缀>.symbol_map.json`） |
| `--reflection` / `--no-reflection` | 显式要求 / 关闭注解与反射（缺省开启） |
| `--hard-realtime` | 禁止整个编译单元使用反射 |
| `--diagnostics=json` | 检查并输出稳定 JSON 诊断 |
| `--separate-modules` | AOT 按模块缓存本地目标文件并统一链接 |
| `--module-cache DIR` | 指定 AOT 模块目标文件缓存目录 |
| `--memory-size 字节` | 运行时堆容量（缺省 512 MiB，最小 64） |
| `--watchdog` / `--no-watchdog` | 开 / 关软重启看门狗（缺省关） |
| `--max-restarts N` | 最多软重启几次（缺省 3，隐含 `--watchdog`） |
| `--watchdog-watermark N` | 内存水位（字节，隐含 `--watchdog`） |
| `--step-limit N` | 本次执行的指令条数上限 |
| `--threads` | 启用隔离的宿主线程执行 |
| `--incremental` | 跨次复用模块预处理与完整编译产物 |
| `--stats` | 打印运行时及增量文件校验统计 |
| `--raw` | 跳过前处理：源文件原样喂给解析器 |
| `--deps` | 只列出 `%include` 拉进来的文件（构建脚本用） |
| `-I` / `--include-dir` / `--include-path` 目录 | 追加 include 搜索目录 |
| `--import-dir 目录` | 追加 import 搜索目录 |
| `-D` / `--define 符号[=值]` | 定义预处理符号 |
| `--input "文本"` | `input(变量);` 的输入项（空白分隔） |
| `--` | 后面全部作为程序的命令行参数 |

不给源文件时，从标准输入读。

---

## 3. `magel.toml`

项目根目录放 `magel.toml`，命令行参数优先于配置。

```toml
[package]
name = "my-app"
version = "0.1.0"

[magel]
run = "interpret"
opt_level = 2
main = "src/main.mgl"
import_paths = ["."]
```

| 段 | 键 | 作用 |
| --- | --- | --- |
| `[package]` | `name`、`version` | 工程元信息 |
| `[magel]` | `run` | 运行方式（`interpret` 等，也可用 `--mode` 指定） |
| | `opt_level` | 优化级别 |
| | `main` | 工程主文件 |
| | `import_paths` | import 搜索目录 |
| `[embedded]` | `enabled`、`restrict_modules`、`max_memory` | 嵌入式模式，见[跨平台部署](06-deployment.md#3-嵌入式前端配置与独立-vm) |

`[magel] main` 指定工程主文件：**不给源文件**执行时用它当入口（即构建项目）；命令行显式给了文件就是单文件编译，不看主文件。`magel new` 生成的工程默认写入 `main = "src/main.mgl"`。

配置里的相对路径以**配置目录**为基准；CLI 上的相对路径以**启动目录**为基准。

---

## 4. 报错呈现

编译错误给出行列位置，并在源码下面按错误长度画波浪号：

```text
语法错误：hello.mgl 第 2 行第 8 列：期望 标识符 / 常量(const) / 可变(mut) / 新建(new) 等 17 个之一，实际是 实现(impl)
   |
 2 | impl Foo { }
   |        ~~~~
```

- **不再显示 Token 内部名**：`SEMI` → `分号(;)`、`IMPL` → `实现(impl)`、`VARIABLE` → `标识符`。
- 出错位置落在 `%include` 进来的文件里时，指向**真正写错的那个文件**，不是展开后的主文件。
- JSON 诊断（`--diagnostics=json` 或 `check FILE json`）带同样的行列，供编辑器跳转。

### 显示语言

报错文案跟随显示语言。同一处错误的三种呈现：

```text
期望 标识符 / 常量(const) / 可变(mut) / 新建(new) 等 17 个之一，实际是 实现(impl)
expected one of identifier / constant(const) / mutable(mut) / new(new) (17 candidates), found impl(impl)
識別子 / 定数(const) / 可変(mut) / 生成(new) 他 17 件のいずれかを期待しましたが、実際は 実装(impl) です
```

选择方式（优先级从高到低）：

1. `magel lang zh|en|ja|auto`；
2. 环境变量 `MAGEL_LANG`；
3. 系统语言（`LC_ALL` / `LC_MESSAGES` / `LANG`，Windows 用 `GetUserDefaultUILanguage()`）；
4. 都没有 → 英文。

> **语义错误（类型不匹配、未定义符号等）目前仍是中文**——那些文案写在编译器核心里，不在报错呈现层。

---

## 5. 调试

编译器本体就是调试适配器：

```sh
magel --dap app.mgl
```

VS Code 装上 [`vscode/`](../vscode/) 下的插件后，在「运行与调试」里选 **Magel**，即可直接在 `.mgl` 上打断点（插件拉起 `magel --dap <文件>`）。支持断点、继续、步进 / 步过 / 步出与调用栈；程序输出走调试控制台。

命令行侧想看位置轨迹，加 `--debug`。

---

## 6. 工程、测试与格式化

```sh
magel new hello-magel        # 新建工程
magel test tests             # 跑测试
magel fmt check src          # 格式检查
magel fmt write src          # 格式写回
magel lsp                    # 语言服务（编辑器接入）
magel deps lock project . offline
```

`test` / `fmt` / `lsp` / `deps` 均为原生 C++ 实现，运行时无需 Python。构建脚本、文档生成与仓库回归测试仍使用 Python。Windows HTTP 下载使用系统 WinHTTP；其他平台目前调用 curl，Git 来源需要 Git。ZIP64、加密包、特殊文件与不支持的 TAR 扩展条目会明确拒绝。

在线模块依赖在 `magel.toml` 中声明，通过 `magel deps lock` 下载并递归锁定。HTTP/HTTPS 支持 ZIP（Store/Deflate）和 TAR（含 `.tar.gz`）压缩包，来源使用固定 SHA-256；Git 来源使用完整提交号：

```toml
[dependencies]
geometry = "https://example.com/geometry-1.0.0.zip#sha256=填写压缩包的64位SHA256摘要"
# fixed = "git+https://example.com/fixed.git#填写完整40位提交号"
# local = "path:../local"
```

```sh
magel deps lock --project .          # 下载缺失的在线依赖并锁定全部传递依赖
magel deps check --project . --offline
magel run src/main.mgl
```

所有新下载的在线依赖统一存到**生成器可执行文件所在目录的 `libraries/`**，多个项目共用。HTTP 包目录为 `名称-摘要`，Git 包目录为 `名称-提交号`；原有项目 `.magel/deps/` 中的 Git 缓存仍可使用。压缩包根目录（或唯一的顶层目录）必须包含 `magel.toml`，依赖包可以继续声明自己的 `[dependencies]` 和 `import_paths`，源码通过普通 `import` 递归导入模块。

`magel.lock` 记录来源、目录、包版本及内容指纹，应纳入项目版本管理。同盘使用相对目录，Windows 跨盘使用绝对目录；切换生成器位置后重新运行 `deps lock`。编译始终使用已锁定的本地缓存，服务器关闭后仍可运行和导出 AOT。`--offline` 不下载，缺失缓存会明确报错；循环依赖、摘要错误及越界压缩路径也会报错。

本地 HTTP 服务器回归：`python tests/magel_online_dependencies.py`，覆盖嵌套导入、菱形依赖去重、共享缓存、离线 VM / AOT、LSP 注释及错误包处理。

`magel lsp` 提供标准库及项目导入模块的函数补全、悬停文档、重载参数提示与跨文件跳转。补全列表保留完整名称以说明来源，无冲突时插入简短名称；跨模块同名或当前作用域有遮蔽时使用限定名。同模块函数重载共享简短名称。标准库说明与 [API 参考](library/README.md) 共用声明数据，随工具链内嵌，运行时无需文档目录。

项目模块支持声明前的 `/** ... */`、`///` 和 `//!` 文档注释，以及 `@brief`、`@param`、`@return`。搜索遵循当前文件目录、`modules/`、最近的 `magel.toml` 中的 `search_paths` / `import_paths` / `include_paths` 与 `magel.lock` 已锁定依赖。循环导入只索引一次，已打开模块的未保存文本优先于磁盘；外部文件修改在下一次查询时读取。

```magel
module calculation;
/** @brief 返回两个整数之和。
 * @param left 左操作数。
 * @param right 右操作数，默认 1。
 * @return 相加的结果。
 */
int64 add(int64 left, int64 right = 1) { return left + right; }
```

VS Code 的语义着色区分类型、泛型参数、变量、参数、函数和成员，使用当前主题的对应颜色。词法扫描保留字符串、数字和多行注释的边界，也支持尚未闭合的编辑中内容；同名局部变量会遮蔽类型名的着色。`List`、`Set`、`Map` 是 `lang` 中的类型名，可以作为普通标识符使用，并非关键字。

输入 `import std::ma` 可候选 `math`，输入 `std::math::sq` 可候选 `sqrt`。模块候选覆盖标准库与工程模块；选择未导入模块的函数会补入相应 `import`，已有 `module` 声明时插在它之后。输入 `std::sql::` 也可候选模块中的 `Database` 类型。成员补全区分 `::` 静态访问与 `.` 实例访问。

VS Code 在 `(` 和 `,` 后显示参数提示；工作区符号搜索可查询工程其他文件中的函数和方法。输入 `.` 时按接收者的具体类型列出直接方法和字段。除了显式声明和函数参数，还支持从构造、变量赋值、字段及函数/方法返回声明推导 `auto`，并处理泛型接收者的字段和返回类型替换。悬停 `auto` 或变量名时显示已推断的具体类型，例如 `auto item = makeLeaf();` 的提示为 `Leaf item`。成员筛选保留模块限定名并遵守局部变量遮蔽，不混合不同模块中同名类型的成员。未知初始化式和返回类型有歧义时保留 `auto`；完整表达式的类型检查与重载可调用性仍由编译器确定。`magel lsp --macros` 继续提供独立的宏检查。


---

## 7. 构建编译器自身

首次克隆后先生成解析器，再构建：

```sh
sh gen.sh      # 需要 ParseEngine 文法生成器
make           # 构建 magel 与 libparser.a
sh check.sh    # 运行正常与错误样例
```

`sh gen.sh` 生成出来的解析器源码（`magel_parser.cpp` / `magel_parser.hpp` /
`magel_parser_user.h` / `magel_main.cpp`）不入库。首次构建及修改真源后需要生成：

```sh
sh gen.sh      # 用 IELR 从 magel.gram 生成前端（需要 ParseEngine 文法生成器）
```

| 改了什么 | 要不要 `sh gen.sh` |
| --- | --- |
| `source/magel.gram` / `source/magel_token.gram` | 要 |
| `source/magel_epilogue.cpp` / `source/epilogue/*.cpp` | 要（展开进 `magel_parser.cpp`） |
| `source/magel_driver.cpp` | 要（展开进 `magel_main.cpp`） |
| 其他 `.cpp` / `.hpp` | 不要，直接 `make` |

> 提交文法与语义层真源，不提交解析器生成物。
> 忘了跑 `gen.sh` 是最容易踩的坑：`magel_main.cpp` 是 `source/magel_driver.cpp` 的**生成副本**，
> 只 `make` 不会重新生成它，于是你会看到"改了没生效"。

`gen.sh` 需要一份 [ParseEngine](https://gitee.com/rabbitc999/ParseEngine) 文法生成器。最省事是把它放到项目根目录 `./ParseEngine[.exe]`；也可以让它待在旁边的 ParseEngine 仓库里，`gen.sh` 会自动找 `../ParseEngine/build/bin/`。

### 编入注解与反射

```sh
make reflection     # 等价 make MAGEL_WITH_REFLECTION=1 STACKVM_WITH_REFLECTION=1
```

根目录默认构建已同时编入前端与运行时反射，单次编译默认启用。`make reflection` 是显式启用入口，
`--no-reflection` 用于关闭本次编译。向默认无反射的 MCU VM 交付时必须显式关闭；
两项构建宏及独立工程默认值见[当前能力](17-capabilities.md)。

---

## 8. 下一步

| 你想 | 去哪 |
| --- | --- |
| 交付产物怎么加固 | [交付安全](05-delivery-security.md) |
| 部署到具体平台 | [跨平台部署](06-deployment.md) |
| 哪些语义已定、哪些还会动 | [21 语义冻结清单](21-semantics-frozen.md) |

---

## 9. 发布门禁 `ci.sh` 与本机环境

`sh ci.sh` 是发布前的一条命令门禁。它分两类结果，看输出就能分清：

| 输出 | 含义 | 要不要处理 |
| --- | --- | --- |
| `ok` | 检查跑过并通过 | 不用 |
| `失败` | 检查跑了，真没过 | **要**，看它打印的日志路径 |


### 哪些检查会依赖本机环境

构建与回归检查依赖 g++ / gcc / make / cmake / Python 3.11+ 与一份 ParseEngine 文法生成器。字节码加密工具及其 OpenSSL 门禁已删除。

### 文法生成器在哪

`ci.sh` 与 `gen.sh` 用同一套查找顺序：先看**本项目根目录**的 `./ParseEngine[.exe]`，
再退回同级的 ParseEngine 仓库。本仓是独立工程，生成器默认就放在根目录。
也可以用 `ENGINE=<路径> sh ci.sh` 显式指定。

## 多诊断与独立借用检查

编译器在可恢复的语句和函数边界继续检查，收集多个独立错误；普通输出逐条显示，
`--diagnostics=json` 在同一个 JSON 文档的 `diagnostics` 数组中输出全部诊断，并映射到原始文件与行号。
同一位置的相同消息合并，最多保留 100 条，避免级联错误；无法恢复的语法或类型注册错误仍会停止当前阶段。

生命周期检查由独立模块 `source/magel_borrow_checker.hpp` 执行。语义分析只解析类型、记录借用来源与存储操作，
全部函数和泛型实例完成降级后才将事实交给检查器；检查器不访问符号表、不生成代码，也不在语义分析过程中判定逃逸。
保存短命引用、返回局部来源及按值 Any 隐藏非全局引用都会产生 `MGL1204`，可以与其他语义错误一并报告。
任一静态诊断都阻止执行及生成可交付产物。回归入口为 `tests/magel_analysis_diagnostics.py` 和 `tests/magel_reference_escape.py`。

控制流检查由 `source/magel_borrow_flow.hpp` 执行，分支合流与循环不动点分析检测释放后及移动后访问，产生 `MGL1205`。
设计边界、算法测试及运行命令见[借用控制流与算法验证](11-algorithm-validation.md)。

### 原生语言服务与宏预览

LSP 对类型名检查大驼峰（PascalCase），对函数和方法名检查小驼峰（camelCase）。不符合规则时输出警告 `MGL2101` / `MGL2102`，不阻止编译；构造器沿用类型名，内部双下划线协议名不参与风格检查。修正名称后警告自动清除。

`import ` 后只补全模块；`import std::` 隐藏自动导入的 `lang`。普通代码中的 `std::` 显示直接子模块，`a::iter::` 只显示该作用域直接包含的子模块、函数、类型等，`a::iter::Type::` 显示该类型的直接方法与字段，不混入其他作用域的候选。

标准库类型和字段的补全与悬停显示声明及实际含义，与 [类型和字段索引](library/symbol-index.json) 及参考页的说明列共用数据。`pause()` 归属 `std::lang`，可写 `std::lang::pause()` 或自动导入后的裸调用。

`magel lsp` 的 stdio JSON-RPC 服务由 `tools/native_development.cpp` 实现，标准库声明从内嵌 JSON 读取；不释放或调用 Python 脚本。普通语言服务也索引 `$macro`、`$replace` 和 `%define`，支持包含文件中的宏文档、定义及参数提示。VSCode 命令 **Magel: 预览宏展开** 调用 `magel/expandMacro`，返回真实预处理和宏展开后的完整源码，包含导入模块，尚未进行 lambda/泛型降低。预览使用只读文档，修改后可重新执行命令。

验证入口：`python tests/magel_native_development.py --aot`（测试驱动使用 Python，受测程序 PATH 不含 Python）、`python tests/magel_native_lsp_parity.py`、`python tests/magel_online_dependencies.py`。旧 Python 实现保留供文档与回归对照，不参与用户运行流程。
