# 09 工具链与命令行

<!-- LANGUAGE NAVIGATION -->
[English](en/09-toolchain.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

从 1.0.8 起，`run` 使用 checked AOT 编译执行，`aot` 默认 unchecked 导出；编译器不再内置 VM 执行器。`.svmb` 导出已删除，新项目使用 AOT 原生产物。原生调试见[原生 AOT 调试](43-native-debugging.md)。

ChaLi 默认通过 checked AOT 执行程序。源码经过前处理、语法与语义检查，再降级为四元式；支持单独导出四元式，也可显式使用 [AOT 编译入口](38-aot.md)生成 LLVM 模块与原生产物。

## 执行与检查

```sh
chali run app.chl
chali check app.chl
chali check app.chl json
chali emit app.chl out build/app
chali aot app.chl --out build/app
```

直接传入 `.chl` 文件也会运行，文件后的参数传给程序；显式 `run` 可配置产物前缀。
`emit` 输出 `.quad`、`.data.bin` 和 `.layout`；字节码导出已删除。运行时资产与标准库内嵌在编译器中。

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
配置见[工程与包管理](13-projects.md)，旧格式迁移见[迁移说明](15-svmb-format.md)。

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

## 标准库预编译单元

`import` 标准库时，标准库（`std::lang` 加本次实际 import 的 `std::xxx`）在合并源码里占绝大部分，
而这部分每次编译都一样。编译器在标准库段末尾插一行分界标记，把整份源码切成「标准库段 + 用户段」
两次解析，并把标准库段那份解析产物（语法节点、类型池、标识符池、顶层声明）序列化成**预编译单元**
写进缓存目录；下次遇到同一份标准库段时直接装回竞技场，跳过标准库段的词法 / LR 解析与节点构建。

- 缓存目录默认 `.chali/chali-stdlib-cache`；`--stdlib-cache-dir DIR` 或环境变量
  `CHALI_STDLIB_CACHE_DIR` 指定别处。这个目录是 `.chali/` 里唯一会留下来的东西：运行与导出
  结束后其余中间产物都删除，`--keep-artifacts` 才保留。
- `--no-stdlib-cache` 或 `CHALI_NO_STDLIB_CACHE=1` 关闭；关闭后与一次性解析整段源码等价。
- 单元键 = 标准库段文本哈希 + 编译器版本 + 构建戳（`__DATE__ __TIME__`）：改了 `source/std/*.chl`
  （重建内嵌资产后）或换了编译器二进制，缓存自动失效。写盘走原子替换；读不回来只是"这份缓存用
  不了"，静默退回全量解析，结果与不用缓存完全一致。
- `--stats` 打印「命中 / 回写 / 未启用」。

反射是语言的固定组成、不可裁剪，缓存也不会跳过它：元数据与查询源码那一轮每次编译都照跑，
所以 `import std::json` 这类程序的总耗时仍主要由反射决定（见[当前能力](17-capabilities.md)）。

## 基础库独立编译单元

`source/ast` 的类型、节点构造、四元式生成和数据导出分别独立编译；`source/bignum` 的整数乘除法、数论、位操作、转换和十进制方根也按职责分离。公开头保留接口，Make 自动依赖记录各单元的实际包含关系。导出编译器和内嵌资产包含全部实现，独立构建使用各库 CMake 目标。具体文件职责见 [AST 库说明](../source/ast/README.md) 与 [大数库说明](../source/bignum/README.md)。
