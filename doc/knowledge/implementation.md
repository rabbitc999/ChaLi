# 实现导航

从 [知识库索引](README.md) 返回。

| 问题 | 从哪里追踪 |
| --- | --- |
| 语法、宏展开、类型和借用 | [文法](../../source/chali.gram) → [语义实现](../../source/semantics/) → [语义冻结清单](../21-semantics-frozen.md) |
| `run`、`build` 的产物和目标 | [AOT 驱动](../../source/chali_aot.cpp) → [目标三元组和布局](../../source/chali_aot_target.cpp) → [运行时资产](../../source/chali_aot_runtime_assets.cpp) |
| 静态 LLVM、汇编与独立链接 | [内置 LLVM／LLD](../../source/chali_aot_llvm.cpp) → [汇编器](../../source/chali_assembler_llvm.cpp) → [链接入口](../../source/chali_linker.cpp) |
| 分配器、引用身份和宿主调用 | [共享运行时](../../source/ircode/) → [分配器](../../source/allocator/) → [C ABI 说明](../41-unsafe-abi.md) |
| 标准库源文件、预编译单元和内嵌文档 | [标准库源码](../../source/std/) → [阶段编译器生成单元](../../tools/build_stdlib_units.py) → [资产嵌入](../../tools/embed_stdlib.py) → [手册生成](../../tools/embed_manual.py) |
| 编辑器诊断、跳转、补全 | [原生语言服务](../../source/lsp/) → [VS Code LSP 客户端](../../vscode/src/lspClient.js) → [编辑器提供器](../../vscode/src/providers.js) |
| 断点、单步、变量和远程调试 | [编译器调试元数据](../../source/chali_aot_debug.cpp) → [VS Code 原生调试适配器](../../vscode/src/nativeDebugAdapter.js) → [调试协议与边界](../43-native-debugging.md) |

诊断编译故障时，先用最小 `.chl` 文件跑 `chali check`。检查通过后用 `chali build --emit-llvm` 查看 LLVM IR，用 `chali build --emit-obj` 把链接从问题中分离。原生程序再用 `chali run` 验证 checked 执行，用 `chali build` 验证发行配置。ChaLi 四元式与 LLVM IR 应在记录中分别命名。

遇到标准库缓存或补全差异，记录默认路径与 `--no-stdlib-cache` 的结果，再对比源码、嵌入单元和实际启动的编译器。编辑器问题还要核对 VSIX 内编译器、设置中的 LLVM 变体，以及客户端发给服务器的实际请求。
