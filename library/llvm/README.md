# LLVM 在线依赖与静态 SDK

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

源码仓库仅保留下载脚本、固定版本清单和许可证；不保存 LLVM SDK、工具链二进制或离线归档分片。所有下载归档位于 `build/downloads/llvm/`，安装结果位于 `build/dependencies/llvm/`，均不入库。

## 静态 LLVM 与匹配编译器

```powershell
python -m pip install -r tools/requirements_llvm.txt
python tools/fetch_llvm_sdk.py
make CHALI_STATIC_LLVM=1 -j4
```

脚本从 MSYS2 官方仓库下载 LLVM/LLD **23.1.4** 的预编译静态开发库及匹配的 MinGW GCC **16.2.0-6** 工具链，共 26 个锁定包，约 187 MiB。每包的文件名、版本、字节数及 SHA-256 记录在 [tools/llvm_sdk.lock.json](../../tools/llvm_sdk.lock.json)。不从源码构建 LLVM，不改动系统 PATH 或全局编译器。

SDK 安装到 `build/dependencies/llvm/static-sdk/`。静态构建自动使用该目录的 `gcc.exe`、`g++.exe` 和 `ar.exe`，显式设置 sysroot 并使用 SDK 的 LLD 链接器，全部中间对象隔离到 `build/llvm-static-gcc16/`。不要将旧 GCC 对象或 MSVC SDK 与该 SDK 混用。CMake 导出工程须在独立目录使用 SDK 的 `bin/gcc.exe`、`bin/g++.exe` 配置，并设置 `CHALI_STATIC_LLVM=ON` 和 `CHALI_LLVM_SDK`。

安装完成后校验全部安装文件、运行 llvm-config 和 GCC、确认真实 `.a` 静态库，再静态链接并运行一个同时引用 LLVM C API 与 LLD MinGW 驱动的最小程序。探测程序在不含 SDK 的 PATH 下运行，防止把依赖第三方 DLL 的结果误判为完整静态链接。锁定的 LLD 包包含多目标支持，链接其所需的 LLVM target 组件；这不代表 ChaLi 已支持其他 AOT 架构。

MSYS2 预编译 LLVM 的 WindowsManifest 对象以 dllimport 声明 libxml2，项目通过 [llvm_static_imports.cpp](../../tools/llvm_static_imports.cpp) 提供类型一致、指向静态 libxml2 符号的导入地址槽；不加载 DLL，也不修改 SDK 二进制。zstd 和 libxml2 的依赖选择静态库。

```powershell
python tools/fetch_llvm_sdk.py --check   # 仅核对已安装 SDK
python tools/fetch_llvm_sdk.py --offline # 使用校验通过的缓存恢复
make llvm-static                      # 自动准备 SDK 后构建
```

兼容入口 `python library/llvm/setup_static.py` 调用同一下载脚本。已下载归档可重复使用；损坏文件会从校验通过的归档恢复。下载或校验失败保留原 SDK。

## 外置 LLVM 工具链

```powershell
python tools/fetch_llvm_toolchain.py
```

从 [llvm-mingw 20261020](https://github.com/mstorsjo/llvm-mingw/releases/tag/20261020) 下载 LLVM **23.1.4**，核对 [manifest.json](manifest.json) 的固定上游 SHA-256，再安装 AOT 所需的 clang、LLD、compiler-rt 与 Windows x64 CRT 文件到 `build/dependencies/llvm/toolchain/`。兼容入口是 `python library/llvm/setup.py`；`--offline` 仅使用已有校验通过的下载缓存。

外置 AOT 默认查找该缓存，也可使用 `--llvm` 或 `CHALI_LLVM_CLANG` 指定工具。只导出 LLVM IR 不需要 clang；`run` 和生成原生程序需要可用后端。

下载临时文件仅写入仓库根 `tmp/`。每轮生成和验证结束、转存所需证据后，运行 `pwsh -File tools/clean_tmp.ps1`；清理不会删除 `build/downloads/` 或已安装依赖。

LLVM 使用 [Apache-2.0 WITH LLVM-exception](LICENSE.TXT)。匹配编译工具链、运行库和静态依赖的许可证保留在 [licenses/](licenses/README.md)，下载 SDK 也保留其 `share/licenses/`；交付仍需保留相应许可证与署名。原始来源是 [MSYS2 官方仓库](https://repo.msys2.org/mingw/mingw64/)及 [MSYS2 LLVM 包配方](https://github.com/msys2/MINGW-packages/tree/master/mingw-w64-llvm)。

## Linux 与 macOS 下载

`python tools/fetch_llvm_toolchain.py` 自动识别宿主，也可用 `--platform` 只下载其他平台分发。`--list` 不下载，列出清单中的版本与摘要。

| 平台 | 固定分发 | 安装位置 |
| --- | --- | --- |
| Windows x64 | llvm-mingw 20261020，LLVM 23.1.4 | `build/dependencies/llvm/toolchain/` |
| Linux x64 | 官方 LLVM 23.1.4 Linux X64 | `build/dependencies/llvm/linux-x64/toolchain/` |
| Linux ARM64 | 官方 LLVM 23.1.4 Linux ARM64 | `build/dependencies/llvm/linux-arm64/toolchain/` |
| macOS ARM64 | 官方 LLVM 23.1.4 macOS ARM64 | `build/dependencies/llvm/macos-arm64/toolchain/` |

```sh
python tools/fetch_llvm_toolchain.py --list
python tools/fetch_llvm_toolchain.py --platform linux-x64
python tools/fetch_llvm_toolchain.py --platform linux-arm64
python tools/fetch_llvm_toolchain.py --platform macos-arm64
```

Linux/macOS 归档来自 [LLVM 23.1.4 官方发布](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.4)，URL、长度和 SHA-256 固定在 manifest.json，不在安装时追踪 latest。保留执行权限及安全的包内链接；只在匹配宿主执行 clang 版本探测。对应系统仍须满足官方包的系统运行库和平台 SDK 要求。本轮仅在 Windows x64 实际执行下载和工具验证，其他平台只检查了选择逻辑和固定发布元数据。

当前静态 SDK 自动下载与匹配 GCC 构建方案限定 Windows x64；其他平台下载能力不代表静态后端、跨架构 AOT 或 macOS 运行时已经验收。没有锁定分发的平台会明确报错，可通过 `CHALI_LLVM_CLANG` 指定系统 LLVM。
