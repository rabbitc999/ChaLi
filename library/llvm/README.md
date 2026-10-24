# LLVM 工具链

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi AOT 从四元式直接生成 LLVM 中间表示，使用 LLVM clang 驱动完成优化、目标代码生成及链接。此目录固定 LLVM **23.1.4**，Windows x64 分发来自 [llvm-mingw 20261020](https://github.com/mstorsjo/llvm-mingw/releases/tag/20261020)。

[packages](packages/) 保存仓库持有的离线 ZIP 归档分片，每片不超过 8 MiB。归档包含 LLVM 与 Clang 共享库、clang 驱动、LLD 链接器、compiler-rt builtins，以及 x64 MinGW-w64 UCRT 头文件、启动对象和导入库。未包含其他目标架构、调试器和 sanitizer 运行库。来源、上游归档、完整归档和各分片的 SHA-256 记录在 [manifest.json](manifest.json)；LLVM 许可证见 [LICENSE.TXT](LICENSE.TXT)，依赖许可证见 [licenses](licenses/README.md)。

在仓库根执行：

```sh
python library/llvm/setup.py
```

脚本只校验和解包本地归档，不下载依赖，生成的 `toolchain/` 不入库。`chali aot` 优先查找当前工作目录或编译器同目录下的 `library/llvm/toolchain/bin/clang.exe`，也可使用 `--llvm 路径` 或 `CHALI_LLVM_CLANG` 指定 LLVM clang。VM 运行与 `--emit-llvm` 不需要运行 LLVM 工具。

维护者可从 `manifest.json` 的上游地址取得完整分发，再执行 `python library/llvm/vendor.py 上游归档路径`。脚本先校验上游哈希，再按固定清单、顺序和时间戳重建本地归档，并核对本地哈希。LLVM 源码提交为 `7a941dfb2877a0b8cf30323fefa85ad019e0b0f5`，构建配方位于同一 llvm-mingw 标签下；仓库使用该分发的二进制库，不要求每次构建 ChaLi 时从头编译 LLVM。

LLVM 模块内嵌入编译器，原生产物无需安装 LLVM 或 ChaLi。C ABI 头文件仍用于宿主集成。手写运行时位于 [source/aot/runtime.c](../../source/aot/runtime.c)，用 [tools/build_aot_runtime.py](../../tools/build_aot_runtime.py) 生成 Windows x64 与 Linux x64 模块；`--check` 核对同步。运行时生成必须使用固定版本，编译用户程序直接输入 `.ll`，不生成用户程序的 C/C++ 源码。

Linux x64 构建机可通过 `--llvm` 指定该宿主的 LLVM 23.1.4 clang，使用系统 C 运行库和链接器。仓库提供的可执行工具包面向 Windows x64；跨宿主工具需在对应宿主构建或安装，并存放于本目录下。当前 LLVM 模块的目标布局明确限定 x64，其他宿主应增加匹配的运行时资产和回归后再启用。

## 静态开发库与内置后端

`make llvm-static` 自动运行 [setup_static.py](setup_static.py)，下载官方固定版本源码并核对 SHA-256，随后使用与 ChaLi 一致的 GCC/G++ ABI 构建 LLVM 的 x64 优化、目标代码生成库和 LLD。需要 CMake、Ninja 与 GCC/G++；Ninja 可通过 `python -m pip install ninja` 安装。首次源码构建耗时较长，后续按文件依赖增量构建。

源码 URL 与固定 SHA-256 写在脚本内：`819ecf7351f1a66f76b2d701d8942faaf5faeb324cb90c9d5b090de142434bb2`。下载缓存位于 `cache/`，提取源码、构建目录、SDK 分别位于 `static-source/`、`static-build/`、`static-sdk/`，均不入库。SDK 清单记录源码身份、宿主编译器及各静态库摘要；复用前逐项核对。已有源码归档可使用 `--source-archive 路径`，已有完整 SDK 可由 `CHALI_LLVM_SDK` 指定。

静态编译选项为 `CHALI_STATIC_LLVM=1`，外置变体为 `0`。LLVM 和 LLD 以真实静态库代码进入最终 exe，不把 DLL 包进 exe 后再加载。内置后端直接读取 LLVM 模块、执行 O2 优化、生成对象并调用进程内 LLD；[链接资产生成器](../../tools/build_aot_linker_assets.py)将必要 CRT 静态库和导入库固化进编译器，在单次链接的私有临时目录恢复。编译用户程序不需要外部 clang、LLVM DLL 或磁盘上的仓库工具链。相关 LLVM、MinGW-w64、compiler-rt 与 libunwind 许可证仍随本目录维护。

两个程序与插件变体通过 `python tools/release_variants.py` 一起构建、验收和打包；参见 [AOT 文档](../../doc/41-aot.md)。静态隔离验收见 [chali_aot_static.py](../../tests/chali_aot_static.py)。
