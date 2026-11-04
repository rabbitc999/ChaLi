# 09 工具链与命令行

<!-- LANGUAGE NAVIGATION -->
[English](en/09-toolchain.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

从 1.0.8 起，`run` 使用 checked AOT 编译执行，`aot` 默认 unchecked 导出；编译器不再内置 VM 执行器。`.svmb` 导出已删除，新项目使用 AOT 原生产物。原生调试见[原生 AOT 调试](43-native-debugging.md)。

ChaLi 默认通过 checked AOT 执行程序。源码经过前处理、语法与语义检查，直接从已检查的 AST 生成 LLVM IR；正常路径绕过四元式生成及组装，可显式使用 [AOT 编译入口](38-aot.md)生成 LLVM 模块与原生产物。

## 执行与检查

```sh
chali run app.chl
chali check app.chl
chali check app.chl json
chali aot app.chl --emit-llvm out build/app
chali aot app.chl --out build/app
```

直接传入 `.chl` 文件也会运行，文件后的参数传给程序；显式 `run` 可配置产物前缀。
`aot --emit-llvm --out 前缀` 直接生成 `.ll`，不生成四元式中间文件；字节码导出已删除。运行时资产与标准库内嵌在编译器中。

## 开发工具

```sh
chali new demo name demo
chali test tests backend interpret
chali fmt check src
chali deps lock project . offline
chali man List
chali lsp
chali gen compiler out build/compiler
chali clear out build/compiler
```

生成器用于独立编译器工程；宿主集成通过共享运行时、宿主注册表与原生 AOT 入口完成。
可恢复的运行失败、调用栈及宿主错误按运行时的统一协议传播。

## 默认构建与优化

反射是语言的固定组成、不可裁剪：前端及运行时始终编入，编译流程始终执行完整的反射处理；`--no-reflection` 只决定这次产物写不写反射元数据，不减少工作量。四元式不做内部优化，优化全部交给 LLVM；原生发布构建默认 O3、LTO 与死代码消除。
`--memory-size` 与 `--step-limit` 设置 VM 资源预算；宿主线程默认启用，`--no-threads` 关闭隔离线程。编译器帮助列出当前支持的选项。

后端调试开关只影响诊断输出与执行路径，不改变程序语义：

| 开关 | 作用 |
| --- | --- |
| `CHALI_DUMP_FRAME=1` | 打出每个函数的帧布局（符号 → 槽号）与分配自检结论 |
| `CHALI_NO_SLOT_REUSE=1` | 退回「形参在前 + 声明顺序一人一槽」，不看活跃性 |
| `CHALI_ONE_SLOT_PER_SYMBOL=1` | 一人一槽但仍走分配器，用来分辨「别名」与「编号」两类问题 |

AOT 快速路径的对照开关见[AOT 降低开关与计数](40-backend-ownership.md)；四元式内部优化与寄存器执行核在 1.0.8 已删除，对应的旧开关不再存在。

指令选择覆盖率与帧槽复用前后都能在 `--stats` 里看到：

```text
指令选择：目标指令 3062 条，覆盖栈指令 10492/26865（39.1%，通用回退 16373 条）
帧槽分配：362 个函数，符号 1622 个 → 槽 1346 个（省掉 276 个）
```

项目配置使用 `run`，旧 interpret 仅作为 AOT 运行的别名；svmb 配置已删除。
配置见[工程与包管理](13-projects.md)，旧格式迁移见[迁移说明](16-compact-symbols.md)，产物位置见[构建产物](15-artifacts.md)。

## 源码模块与增量构建

语言服务的函数悬停只显示当前调用匹配的重载详情；输入时的签名帮助用“函数 [1/4]”显示
可切换的候选，并根据已输入参数的数量和可推断类型缩减候选。默认参数、变长参数、泛型和
尚未完成的实参保留仍可能匹配的声明；修改参数后，服务保留仍有效的选中项，否则选择剩余候选。
宏使用“宏 [1/4]”，按有序括号分组、参数数量和变长组筛选；片段类别与类型推导继续遵循宏体的
`$match` 和泛型规则，不把不同片段类别虚构为同一宏头的类型重载。输入 `$` 只补全宏，包含已导入模块的宏，
支持 `$模块::宏名`。VS Code 丢弃已取消请求或文档版本变化后的响应。

`source/` 的 C/C++ 文件按模块职责使用下划线名称；类、成员和函数保持既有命名。
标准库上层策略保留在 `source/std/` 的 ChaLi 源码中，宿主入口位于 `source/abi/`。
公开 C11 头 `chali_abi.h` 定义调用帧、固定宽度槽、不透明注册句柄与错误缓冲；
`chali_standard.h` 提供标准库登记。实现文件可以使用 C 或 C++17，公开边界均使用 C 类型。

模块提供只读的 `ChaliAbiRecord` 入口表。`glue/aot_modules.inc` 统一维护模块名与取表函数，
集中标准库登记排除 demo，AOT 根据实际入口名称选择需要链接的模块。
每张入口表通过 `chali_abi_registry_register` 原子登记，失败保留首个诊断；
注册完成后封存。同一运行时只使用一张函数表，不另建类型化绑定表。

`chali_host_call` 把槽数组交给 C 入口。标量 C ABI 可以由 LLVM 直接调用；
结构体、数组、引用、可空引用及同步回调由生成的 C 入口适配，
私有封送实现只通过 C 帧服务读写语言存储、创建暂存和调用语言函数值。
`chali_abi_bridge.cpp` 复用原有堆、对象表、错误及同步调用服务，
`GLUE_NO_STD_XXX` 继续控制相应模块的编译裁剪。

泛型、闭包、工程清单和诊断分别由 `chali_generic.cpp`、`chali_lambda.cpp`、
`chali_generic_closure.cpp`、`chali_package.cpp` 和 `chali_report.cpp` 编译。
`macro/` 独立编译卫生绑定、结构化替换、宏注册、正则匹配、原始片段和编辑器宏头模型；
`syntax/` 提供普通源码分组及常量求值，`editor/` 输出真实编译分析。
AST 声明位于 `chali_ast.hpp`；`semantics/` 中的符号登记、卫生展开、泛型闭包和程序降级
由权威文法装配进生成解析器。语言服务位于 `lsp/`，工程生成器位于 `generator/`。

修改文法后执行 `sh gen.sh`，然后执行 `make -j4`。普通 C++ 实现变化由 `.d` 文件追踪实际包含关系。
`make` 自动检查标准库、胶水及手册的内嵌输入，内容由 `tools/embed_stdlib.py` 生成并单独编译为
`chali_stdlib_embedded.o`；压缩数据不再被公共头包含。生成器在 `build/embedded-sha256/` 保存各组资产的
`.sha256` 清单，生成前检查输入、生成脚本和现有产物的 SHA-256；全部一致时直接跳过压缩和源码生成，
保持产物时间戳。文件缺失或损坏、输入内容或资产索引变化会重新生成。此检查也用于手册、中英文消息、
运行时资产清单和 AOT 链接资产。`gen.sh` 统一 Python 子进程的 UTF-8 编码，保证 Windows 下的中文提示
与脚本输出编码一致。独立导出的编译器工程也将这些实现作为单独的 CMake 源文件链接。

构建、测试、AOT 资产解包、LSP 检查、ABI 调用桥和编译器工程生成器的临时文件夹统一放在
仓库根目录的 `tmp/` 内，各任务使用独立子目录；不在仓库同级创建 `tmp`。从子目录或上级目录启动
仓库内的编译器时也定位到同一目录。Python、Node 测试和构建子进程继承统一临时路径。

C 宿主包含 `chali_abi.h`，需要集中登记标准库时另包含 `chali_standard.h`，并链接匹配配置的运行时与宿主记录库。
宿主入口及其代码需要覆盖注册表和调用的寿命；函数值回调用 `chali_abi_invoke`，不能将语言句柄强转成机器函数指针。
完整构建命令见 [C 宿主示例](../example/host-c-api/README.md)及[同步回调示例](../example/native-callbacks/README.md)。

## 标准库内嵌二进制

标准库核心 `std::core` 的解析产物在构建编译器时生成二进制，并直接编入编译器程序。常规编译从内存恢复语法节点、类型池、标识符池和顶层声明；不需要外置 `.unit` 文件，也不创建或更新 `.chali/chali-stdlib-cache`。

内嵌资产包括核心和可选标准模块的普通声明、卫生宏 AST、未实例化泛型模板记录，以及 range／算法辅助声明。模块分别压缩并按实际导入顺序恢复；节点、类型、接口名称、宏调用和来源在当前会话重定位，用户依赖的位置保持不变。泛型模板在完整声明匹配后恢复参数、约束、默认类型和来源，内部宏编号重新绑定；具体实例、接口实现表、宏展开、语义与借用检查仍属于本次程序。预处理和名字归一仍可扫描内嵌文本，内容不匹配时使用源码路径，不写入磁盘缓存。

- `make all` 先构建独立的标准库阶段编译器，再由它两次生成并核对确定性，原子更新二进制字节数组并构建最终编译器；生成源为 `tools/build_stdlib_units.py`，资产不包含用户源码、项目路径或进程指针。
- 解析单元格式为 7，保留完整类型引用闭包及生命周期契约；模板记录格式为 1，不保存用户实例或进程地址；包格式为 2，记录内容与编译器身份、反射配置、长度和校验信息。整包损坏或匹配单元格式不兼容时报告明确错误。
- 宏来源位置按文件与原始行归一，用户输入长度、模块与路径改变时核心二进制保持一致。
- `--no-stdlib-cache` 和 `CHALI_NO_STDLIB_CACHE=1` 保留为兼容开关，含义为关闭内嵌单元、从源码解析；两条路径都不写标准库缓存。
- `--stdlib-cache-dir` 已删除，`CHALI_STDLIB_CACHE_DIR` 不再控制加载目录。已有项目文件和旧目录不会被编译器擅自删除。
- `--stats` 分别报告核心、模块 AST 和模板记录的命中／源码回退，并列出命中的模块名称。`std::__range` 是编译器生成的标准辅助资产，不是可供用户导入的模块。

反射元数据和具体泛型实例仍在本次编译中处理，不能用核心解析被跳过推断整个标准库已经完成目标代码生成。

## 基础库独立编译单元

`source/ast` 的类型、节点构造、四元式生成和数据导出分别独立编译；`source/bignum` 的整数乘除法、数论、位操作、转换和十进制方根也按职责分离。公开头保留接口，Make 自动依赖记录各单元的实际包含关系。导出编译器和内嵌资产包含全部实现，独立构建使用各库 CMake 目标。具体文件职责见 [AST 库说明](../source/ast/README.md) 与 [大数库说明](../source/bignum/README.md)。

## 警告配置

警告使用稳定代码。`CHL1601`（`overlapping-pattern`）默认启用；命名规则 `CHL2101`／`CHL2102`／`CHL2103`（`type-naming`／`function-naming`／`module-naming`）和 `CHL2201`（`unused-local`）默认关闭。命名只检查已有真实声明位置的类型、函数、方法和模块，构造器及宏生成声明不报告；未使用变量检查只报告已检查 AST 中没有读取的用户局部，清理或借用产生的读取会保守地视为使用。

```console
chali --check main.chl --warn=unused-local
chali --check main.chl --warn=naming --warnings-as-errors
chali --diagnostics=json main.chl --deny=CHL2201
```

`--warn=规则`、`--allow=规则`、`--deny=规则` 分别选择警告、关闭、错误；可以使用稳定代码、规则名以及 `naming`／`all` 规则组。`--warnings-as-errors` 只提升已启用的规则。未知规则报错。项目清单可配置：

```toml
[warnings]
unused-local = "warn"
CHL2101 = "allow"
```

级别为 `allow`、`warn` 或 `error`。编辑器预设启用命名规则，项目配置覆盖预设，命令行显式规则最后覆盖；同一命令行按出现顺序应用。逐行抑制写在真实行注释中：

在源码文件中写 `#NoNamingCheck` 可关闭该文件的类型、函数、方法和模块命名警告。命令行和 LSP 采用同一规则；导入文件及其他警告独立处理。

```chali
// chali: allow-next-line CHL2201
int64 reserved = prepare();
```

也可在目标行末写 `// chali: allow CHL2201`。字符串或块注释中的相同文本不生效。JSON 和 LSP 共用代码、级别、原始位置及安全修复数据；当前修复仅插入局部抑制，不删除初始化或清理。提升为错误时不执行程序。正常宏参数副本和编译器消除复制的行为不产生警告。

## 语言服务分析复用

语言服务为请求保存不可变的打开文档版本；分析中读取的关闭模块也在该请求内固定，并以源码覆盖表交给编译器。词法索引可直接服务补全和着色，诊断、类型提示及宏预览共享完整语义报告。同一入口同时请求语义检查时合并执行，等待者仍可取消；不会并行访问编译器前端的全局可变状态。

内存最多保留 64 个入口的报告，按序列化 JSON 字节估计合计 32 MiB，单项超过 8 MiB 不驻留。该数字不是 JSON 对象实际堆占用或整个进程的内存硬上限。每次命中仍核对输入、语言、模块图、实际依赖和项目／锁文件指纹，磁盘报告不可用时仍能复用有效内存结果。项目清单或锁文件从不存在变为存在，也会通过文件监视使入口失效。

完整语义检查跨过一次编辑时，旧请求被取消，诊断重新排队；结果发布前再次核对捕获版本。未保存文档的 JSON 摘录来自本次检查原文。`chali/lspStats` 提供 `checks`、`diagnosticHits`、`memoryReportHits`、`mergedChecks`、`reportEntries` 和 `reportBytes`，可区分真实检查、复用和驻留预算。

## 语言服务调度与计时

语言服务保留六条独立工作通道，完整语义编译最多同时启动两个子进程。补全、悬停等词法路径不会等待这个语义预算；等待空位的交互语义请求优先于后台诊断。每条通道最多排队 128 项，并按文档快照保守估算限制在 16 MiB；空队列允许一个超过预算的大快照，后来的快照会淘汰旧排队项。此预算不等于整个进程的内存上限：当前打开的文档、活跃分析和有界索引／报告缓存分别持有内存。

编辑或关闭文档立即取消它的旧请求，并释放尚未启动的快照；每个请求仍收到结果或取消响应。关闭时先取消后台诊断，请求通道共用五秒排空期限，随后取消剩余请求并回收线程。前端可变上下文继续隔离在编译器子进程中。`chali/lspStats` 的 `analysisActive`、`analysisPeak` 和 `analysisWaiting` 用于核对子进程预算。

LSP 计时前后检查 CPU，占用达到 40% 或读不到可靠计数时只验证功能，不输出性能数字。可用 `CHALI_SKIP_LSP_TIMING=1` 显式只运行功能检查。正式对照记录实际编译器 SHA256、空语言服务缓存下的进程启动／首次诊断，以及编辑诊断、补全和悬停的 P50／P95；延迟包含 JSON-RPC 客户端往返，进程冷启动不代表清空操作系统文件缓存。其他语言的运行性能基线不在这里重测。

## VSCode：修复、源码与项目目标

安装当前版本的扩展后，警告灯泡中的修复来自编译器当前报告。当前安全修复在目标行前插入逐行抑制；初始化、清理及其他语句保留。应用前检查文档版本，期间继续编辑会使旧操作失效。

在具有内嵌语言源码的标准库声明上使用“转到定义”，会打开 `chali-stdlib:` 只读文档。位置来自实际声明索引，源码和索引按需保存在内存；扩展不生成源码副本，也不允许重命名标准库声明。没有语言源码声明的后端内建入口不会伪造函数体。重启服务后，已打开的标准库视图会刷新。

命令面板提供“选择项目目标”“选择 AOT 输出”和“打开项目配置”。配置编辑使用编译器的 TOML 解析／保留来源序列化，保留其他字段、注释和多行字符串；变更先留在可撤销的编辑缓冲区，状态栏显示未保存状态。构建时先保存源文件和已打开的脏清单，再读取实际目标。例如：

```toml
[aot]
target = "linux-arm64"
output = "object"
allocator = "malloc"
```

“构建当前文件”按清单输出可执行文件、`.o` 或 `.ll`；Linux 可执行文件没有 `.exe` 后缀。“构建可执行文件”要求项目输出已设为 `executable`，不会暗中改写清单。状态栏显示规范目标、输出类别及当前编译器 LLVM 变体。目标列表只包含后端登记的 Windows x64、Linux x64 和 Linux ARM64；对象生成不代表该目标的链接环境或运行平台已验证，交叉可执行文件仍需相应 SDK、运行时和链接依赖。

重命名编辑附带当前源码指纹。扩展在返回编辑前核对已打开文档的版本及关闭依赖的实际文本。标准库只读，无法唯一确认绑定或存在宏模板歧义时拒绝整个重命名。

代码操作支持补齐直接列出的非泛型接口方法，以及为缺失分支的 match 添加兜底。生成体使用明确的 `TODO panic`，需要自行实现；每个候选先通过独立的编译器检查，不写入正式报告或预处理缓存。其他编译错误、泛型或不明确声明可能使候选不提供。宏预览中的“展开来源与绑定”汇总编译器记录的来源、定义、元数据及控制出口，点击位置可跳转；它不推断传递纯度。

## 双变体交付核对

静态与外置 LLVM 使用独立构建目录和同版本命名。打包前核对 `--build-info`、插件版本、VSIX 内嵌 EXE 字节和实际构建配置；不要用另一变体的配置文件代替核对。`tools/release_variants.py --package-only` 可用 `--static-build-root`／`--external-build-root` 指定本次配置目录。安装包和 ZIP 另核对 EXE、版本及 LICENSE／NOTICE／依赖许可；生成交叉对象与在目标系统执行分别记录。

语言服务的性能对照按参与测量的编译器摘要保存。后续产物核对沿用这组数据的原始标识。[语言服务性能对照](../benchmarks/results/lsp-1.0.12-final/README.md)

## 目标机器汇编

`chali aot app.chl --emit-asm --out build/app` 按实际目标、CPU、检查及优化／调试设置导出 `build/app.s`。这是机器汇编，与 LLVM IR 文本 `.ll` 不同。输出在对象生成和链接之前停止，跨目标导出不要求准备目标运行时链接资产。静态变体使用内置 LLVM，外置变体使用 clang；采用相应后端默认语法（x86 为 AT&T，AArch64 为目标语法），不表示兼容 NASM／MASM。

项目可设置 `[aot] output = "assembly"`。显式 `--emit-asm`、`--emit-obj` 或 `--emit-llvm` 覆盖项目输出，三个选项互斥；普通 `run` 仍执行 checked AOT。VSCode 输出选择和构建支持汇编及 `.s` 后缀。

同时保留 `.ll` 和适用的 C ABI 头文件；复杂 C ABI 还可能生成 `.abi.cpp` 封送侧文件，单独的汇编文件不等于已链接应用。重新汇编和最终链接需要匹配的目标工具链、运行时及依赖。独立汇编、链接与归档入口见下文。

## 独立汇编器与汇编输入

```sh
chali asm native/add.s --target windows-x64 --out build/add.obj -I native/include
chali asm native/add.s --target linux-x64 --out build/add.o
chali aot main.chl --asm native/add.s --asm-include native/include --out build/app
```

`asm` 每次接受一个 `.s` 和完整输出路径。不写 `--out` 时，Windows 默认 `.obj`，Linux 默认 `.o`。支持 `--target`、`--cpu`、`--debug`、`-I DIR`／`-IDIR`、`--include-path DIR` 和 `--llvm CLANG`。源指令必须匹配目标和 CPU；采用 LLVM 的 GAS 兼容语法，不处理 `.S` 预处理、NASM 或 MASM。包含目录先加入源文件所在目录，再按显式目录顺序搜索。主输入最多 64 MiB；该入口是编译工具，不是面向不可信代码的沙箱。

静态变体直接调用 LLVM MC，无需 clang；显式 `--llvm` 使用外置集成汇编器。对象先生成在独占工作区，汇编成功后才发布，失败保留已有产物，且禁止覆盖自己的主输入。输出父目录必须存在。源文件及包含文件的诊断保留实际位置。

AOT 可重复指定 `--asm FILE.s` 和 `--asm-include DIR`，每个对象使用语言单元相同的目标和 CPU，参与最终链接。通过既有 C ABI 声明引用汇编符号，汇编须遵循目标调用约定。该输入只适用于可执行文件或共享库输出，不能与 `--emit-asm`、`--emit-obj`、`--emit-llvm` 组合。生成交叉对象不等于已经验证目标系统的可执行交付。

链接的标量 C 函数可声明为 `#abi("C", "@link", "asm_add") int64 asm_add(int64 a, int64 b);`。符号由交给链接器的对象或归档提供。结构体、引用、缓冲区和回调签名使用兼容的动态库，遵循 [C ABI 封送约定](41-unsafe-abi.md)。


## 独立链接与静态归档

```sh
chali ar build/first.o build/second.o --out build/libexample.a
chali link build/main.o build/libexample.a --out build/app.exe
chali link build/main.o -L build --library example --out build/app.exe
chali link build/public.o --shared --export public_entry --out build/example.dll
chali link build/start.o --target linux-arm64 --no-runtime --entry _start --out build/app
```

`ar` 创建带符号索引的确定性自包含归档，接受零个或多个同目标对象；时间、UID 和 GID 不随构建变化。Windows 的 `.lib` 选择 COFF 归档，其他后缀使用 GNU ar 格式。薄归档和嵌套归档不接受。`--out` 指定完整文件名，父目录须已存在。静态变体调用 LLVM ArchiveWriter；外置变体查找 clang 旁的 llvm-ar 或 `CHALI_LLVM_AR`，也可用 `--llvm LLVM-AR` 显式指定。

`link` 接受多个对象／归档，支持 `--target`、`--shared`、重复 `--export SYMBOL`、`--entry SYMBOL`、`--debug`、`-L DIR`／`-LDIR`／`--library-path DIR`、重复 `--library NAME`、`--sysroot`、`--runtime-dir` 和 `--llvm CLANG`。入口及导出使用原始对象符号名。对象和库按输入顺序进入同一链接组，归档按需提取并重复扫描，允许库间循环引用；未提取成员的未定义符号不引发错误。重复定义及已引用的缺失符号由 LLD 报告。共享库只公开显式导出的符号，其他符号隐藏；不提供任意原始链接器参数透传。

静态 Windows x64 变体默认使用内置 LLD、CRT 与 ChaLi 运行时，无需外置 clang；外置变体使用匹配 clang／LLD 和所需系统工具链。完整跨目标链接需要目标运行时与 SDK。显式 `--no-runtime` 不添加 CRT 或 ChaLi 运行时，可用于自备入口与依赖的程序；内置 LLD 可为受支持 Linux x64／ARM64 目标链接此类输入，生成成功不等于已在目标系统运行。普通独立 C 程序也可使用默认运行时链接，未被引用的归档成员不装入应用。

输入先检查对象格式、位宽、架构及归档成员边界；实际符号、节和重定位仍由 LLVM／LLD 检查。这些工具不构成执行不可信代码的沙箱。产物先在私有工作区生成，成功后才替换最终文件；失败保留原文件，禁止输出覆盖输入。链接错误不能作为旧产物仍有效的依据，调用方必须检查返回状态。
