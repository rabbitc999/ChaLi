# ChaLi 1.0.10 后端与交付验证

本目录保存 2026-10-28 的真实验证证据。程序运行使用原生 AOT；旧测试日志中的“VM”“解释执行”字样沿用测试原有提示，不表示本版本恢复了 VM。

## 全量行为对照

`sample-comparison.json` 记录 603 个样例、基线与验证快照的真实 SHA-256、退出码、输入配置及规范化输出摘要。599 项与 1.0.9 基线一致，4 项由基线失败变为成功，且新输出与对应 `example/expected/` 文件一致：

- `bits`、`numeric_v101`、`unsigned_suffix`：直接 AST 后端支持相应位运算。
- `recursive_reference_storage`：去除重复辅助函数定义导致的无效 LLVM 模块。

原始比较器的 `passed` 为 false，因为它要求全部行为与基线完全相同；补充字段 `regression_free` 为 true，并明确列出上述改善。文本比较只规范化 CRLF 和行尾空白；失败样例比较首行诊断，地址占位只适用于原预期已注明的情况。

正常样例中有 11 项在基线和新快照都失败，不能计为正常样例验收成功：`cpp_glue`、`function_pointer_glue`、`glue_interface_inherit`、`glue_members`、`glue_system_interfaces`、`glue_types`、`pointer_glue`、`reference_glue_optional`、`std_input`、`variadic_glue`、`zip`。其中宿主演示胶水缺失、旧输入夹具以及 zip 类型诊断属于既有问题；本轮没有借改动预期隐藏它们。

全量对照使用隔离的后端验证快照，以便同时构建发行版。随后 C ABI `void` 返回处理的修复通过独立 AOT 专项验证；最终发行版另有标准流、静态隔离、调试、手册和交付字节核对。报告分别保留快照及最终二进制摘要。

## 专项验证

- `aot-final.log`：LLVM 模块、Linux ELF 目标文件、checked/unchecked、C 双向调用、数值位宽、列表、opcode 生命周期及无 main 动态库初始化与清理。
- `release-streams-verification.json`：最终外置／静态编译器分别启用和关闭原语快速路径，验证 5000 个汉字长行、CRLF、空行、末尾裸 CR、浮点去尾零及标准错误流。`standard_streams.chl` 为实际源码。较早专项还覆盖 EOF、零字节拒绝与生成调用路径。
- `native-debug-final.log`：真实 GDB/DAP 断点、单步、调用栈、参数、局部变量及表达式。
- `static-standalone-final.log`：单个静态编译器在没有 clang PATH 与 LLVM DLL 的隔离目录中生成并运行程序及 C ABI 共享库。
- `module-layout-final.log`：39 个独立核心单元，修改单个实现只重编相应核心对象。
- `book-updates-final.log`：59 个完整书稿程序、6 个附录失败示例；`book-consistency-final.log` 核对最终 Markdown 与 LaTeX。PDF 共 301 页，已检查封面、交叉编译和唯一番外页面，最终构建无 overfull 警告。
- 两个 `manual-*-check.log`：发行版导出的全部中英文 Markdown 与当前书稿一致。
- 插件完整测试、ABI 入口表、工具链命令、文本输入、运行时资产同步、库参考与文档链接检查均保留日志。

没有声称仓库所有历史测试均通过。仍读取旧 `.quad`／`.svmb` 的测试不属于本次已执行门禁；正常 CLI 已删除这些产物路径，独立库兼容接口仍保留。

## 交叉编译

实际宿主为 Windows x64。静态 LLVM 后端生成了 Windows x64 COFF、Linux x64 ELF 与 Linux ARM64 ELF 对象；`cross-objects.json` 保存机器类型及摘要。

外置 clang 配合隔离 GNU 目标 SDK 完整链接 Linux x64 和 ARM64 示例，分别在 WSL 与 QEMU 运行。`cross_io.chl` 还在 ARM64 QEMU 中验证了含零字节文件读写、浮点文本、标准输入与标准错误，见 `cross-io-execution.log`。SDK 由 `tools/build_cross_runtime.sh` 使用现有 GNU 工具链构建。对象生成、链接成功与模拟执行的验证范围彼此独立；没有宣称已在 ARM64 实机验收。

项目 TOML 与命令行覆盖、未给源文件时读取项目 main、拒绝直接运行外国目标均有独立记录。完整跨目标链接需要目标 sysroot 与匹配的 ChaLi 运行时；复杂 C ABI 适配器当前需要在目标工具链编译链接。

## 发行资产

`delivery-verification.json` 保存两个最终编译器的 SHA-256。实际编译器版本、VSIX 清单与内嵌版本、MSI 版本均为 1.0.10；已从 MSI 解包并与原编译器逐字节比较，同时核对 VSIX 中的编译器字节。静态和外置对象分别在 `build/llvm-static-gcc16/` 与 `build/` 构建。

交付文件位于仓库根、`dist/installers/` 与 `vscode/dist/`，使用 `chali-1.0.10-llvm-{static,external}-windows-x64` 命名。临时 Python 验证脚本不参与 LLVM IR 生成，也不留在最终实现中；LLVM IR 来自 C++ 后端，运行时平台模块由固定 clang 的 shell 脚本生成。
