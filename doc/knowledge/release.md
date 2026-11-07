# 构建与验证手册

从 [知识库索引](README.md) 返回。

1. 用 `git status --short` 记录源码状态，核对 [VERSION](../../VERSION) 与 [扩展清单](../../vscode/package.json)。按[安装包文档](../39-installation-packages.md)准备目标工具链。静态和外置 LLVM、不同 CPU 的对象目录分别构建。
2. 文法修改后运行 `sh gen.sh`。标准库源码由构建阶段生成内嵌单元，手册可用 `python tools/embed_manual.py --check` 核对。书稿修改时运行 `python doc/book/latex/tools/md2tex.py`，重新排版并检查改动页。构建入口见 [Makefile](../../Makefile) 和 [生成脚本](../../gen.sh)。
3. Windows x64 使用 `python tools/release_variants.py` 构建两种 LLVM 变体及 VSIX，使用 [安装包工具](../../tools/package_installers.py)生成 MSI。Ubuntu x64／ARM64 的外置 LLVM 构建从 [Linux 打包脚本](../../tools/build_linux_packages.py)进入。ARM64 交叉构建使用 QEMU 运行阶段编译器，目标系统实测需要单独记录。CentOS RPM 使用相应[构建容器](../../tools/packaging/centos-stream9.Dockerfile)。
4. 先跑直接覆盖改动的专项，再按发行范围执行 `sh ci.sh --full --jobs 4`。回归清单见 [ci.sh](../../ci.sh)，样例入口为 [check.sh](../../check.sh)。保留首轮失败和复验各自的结果。
5. 检查交付 EXE 或 ELF 的系统和机器类型，运行 `--build-info` 核对版本和 LLVM 变体。解包比较 MSI、DEB、RPM 或 VSIX 中的编译器字节，记录 SHA-256。对 VSIX 另核对扩展版本、`chaliCompilerVersion` 和实际宿主。分开报告目标对象、模拟运行、原生运行和包内容检查。

| 改动范围 | 定向入口 |
| --- | --- |
| LLVM 构建、静态 LLVM、目标对象与链接 | [LLVM 构建 回归](../../tests/chali_aot.py)、[静态后端](../../tests/chali_aot_static.py)、[汇编](../../tests/chali_aot_assembly.py)、[Windows PE 子系统](../../tests/chali_windows_subsystem.py) |
| LSP、命名与补全 | [原生 LSP 专项](../../tests/chali_lsp_naming_scopes.py)、[扩展测试](../../vscode/test/) |
| 书稿与正式文档 | [书稿例程及 LaTeX](../../tests/chali_book_updates.py)、[双语结构](../../tests/chali_book_rewrite.py)、[链接检查](../../tools/check_doc_links.py) |
| 临时目录与默认产物路径 | [临时目录专项](../../tests/chali_temporary_paths.py)、[产物文档](../15-artifacts.md) |

Windows ARM64 和 Linux 静态 LLVM 的平台接入应从[目标表](../../source/chali_aot_target.cpp)、[构建规则](../../local.mk)及[内置链接入口](../../source/chali_aot_llvm.cpp)核对。构建支持还取决于对应 CRT、目标运行时、SDK 和真实运行环境。
