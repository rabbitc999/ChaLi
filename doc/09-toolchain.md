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
标准库胶水统一位于 `source/glue/std_xxx.{hpp,cpp}`：公共头暴露绑定入口，平台适配、记录布局和
算法实现在对应 `.cpp` 独立编译。`std_binding.hpp` 只提供注册表前置声明；模板需要的
结构化数据与类型转换定义保留在头文件中。VM 核心继续使用通用宿主协议。

已经搬到 C 层的胶水位于 `source/abi/`：一个模块一个文件（`abi_math.c`、`abi_clock.c`、
`abi_random.c`、`abi_print.c`、`abi_string.c`），各自导出按名称升序的 `ChaliAbiRecord` 入口表；
`chali_abi.c` 把各表聚合成一张全局表，首次使用时排序一次，之后二分查找。
`chali_abi.h` 是 C 侧唯一需要包含的头文件（不引任何 C++ 头），
`chali_abi_bridge.cpp` 用 `extern "C"` 把堆、数据区、对象表、错误通道和同步调用暴露给 C 胶水。
直通入口在**绑定期**就由 `Registry::bind` 解析出来（`chali_abi_resolve`），
`chali_host_call` 命中后直接调用 `void (*)(ChaliAbiFrame *)`：实参就是生成代码写好的槽数组，
不构造 `NativeValue` 向量、不拷贝 `NativeContext`、不经过 `std::function` 类型擦除。
新增直通函数只需在对应 `abi_*.c` 里实现入口并加入该模块的表；新增模块才需要动 `chali_abi.c` 的
`kModules` 与 `tools/embed_stdlib.py` 的资产清单。`GLUE_NO_STD_XXX` 对 C 模块同样生效。

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

宿主接入胶水时同时包含 `chali_glue.hpp` 与所需 `std_xxx.hpp`，并链接对应 `.cpp` 或宿主记录库。
只有绑定接口的头文件不会隐式带入全部实现；完整构建命令见[同步回调示例](../example/native-callbacks/README.md)。
已迁到 `source/abi/` 的模块没有 `std_xxx.hpp`，宿主只需把该目录的 `.c` 与 `chali_abi_bridge.cpp`
一起编译；`std_registry.cpp` 末尾的 `bindDirectEntries()` 会把它们补进注册表。

## 标准库内嵌二进制

标准库核心 `std::lang` 的解析产物在构建编译器时生成二进制，并直接编入编译器程序。常规编译从内存恢复语法节点、类型池、标识符池和顶层声明；不需要外置 `.unit` 文件，也不创建或更新 `.chali/chali-stdlib-cache`。

当前预编译范围是标准库核心。可选标准模块和用户模块继续使用常规前端，泛型处理、宏展开、语义与借用检查仍按本次程序执行；恢复核心不跳过这些语言规则。核心内容身份与内嵌资产不匹配时在内存中解析源码，不写入磁盘缓存。

- `make all` 先构建阶段编译器，再由它生成二进制字节数组并重链接最终编译器；生成源为 `tools/build_stdlib_units.py`，资产不包含用户源码、项目路径或进程指针。
- 二进制单元记录格式版本，包记录核心内容及编译器版本身份、长度和校验信息。整包损坏或匹配单元格式不兼容时报告明确错误。
- 宏来源位置按文件与原始行归一，用户输入长度、模块与路径改变时核心二进制保持一致。
- `--no-stdlib-cache` 和 `CHALI_NO_STDLIB_CACHE=1` 保留为兼容开关，含义为关闭内嵌单元、从源码解析；两条路径都不写标准库缓存。
- `--stdlib-cache-dir` 已删除，`CHALI_STDLIB_CACHE_DIR` 不再控制加载目录。已有项目文件和旧目录不会被编译器擅自删除。
- `--stats` 区分“命中内嵌核心”“源码解析”和“未启用”。

反射元数据和具体泛型实例仍在本次编译中处理，不能用核心解析被跳过推断整个标准库已经完成目标代码生成。

## 基础库独立编译单元

`source/ast` 的类型、节点构造、四元式生成和数据导出分别独立编译；`source/bignum` 的整数乘除法、数论、位操作、转换和十进制方根也按职责分离。公开头保留接口，Make 自动依赖记录各单元的实际包含关系。导出编译器和内嵌资产包含全部实现，独立构建使用各库 CMake 目标。具体文件职责见 [AST 库说明](../source/ast/README.md) 与 [大数库说明](../source/bignum/README.md)。
