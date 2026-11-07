# 安装包与多架构构建

<!-- LANGUAGE NAVIGATION -->
[English](en/39-installation-packages.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

统一构建入口为 [`tools/build.py`](../tools/build.py)。它先刷新文法生成资产与内嵌手册，再构建编译器和安装包；不运行测试门禁。修改书稿、标准库或胶水后，应在文档定稿并刷新内嵌资产之后生成最终发行版。

## 快速安装

Windows 使用 MSI，默认安装到 Program Files 下的 ChaLi 目录，并注册系统 PATH。安装或升级后重新打开终端：

```powershell
msiexec /i .\chali-1.0.0-llvm-static-windows-x64.msi
```

Windows x64 默认使用静态内置 LLVM，编译器 EXE 已链接 LLVM 后端，不需要另装 LLVM 工具链。两类 MSI 同时提供：

| 安装包 | 用途 |
| --- | --- |
| `chali-1.0.0-llvm-static-windows-x64.msi` | 默认安装包，静态内置 LLVM |
| `chali-1.0.0-llvm-external-windows-x64.msi` | 外置 LLVM 工具链版本 |

MSI 支持系统的应用卸载入口，也可以通过 `msiexec /x 安装包.msi` 卸载。相同架构的静态和外置 LLVM 变体使用同一升级系列，选择其中一种。


本版本 Windows x64 交付附件如下；表中大小由最终构件生成后按实际字节数换算为 MiB，重新打包或更换工具链后应同步更新：

| 文件 | 大小 | 用途 |
| --- | ---: | --- |
| `ChaLi-1.0.0-licenses.zip` | 0.21 MiB | ChaLi 与随附第三方组件的 LICENSE、NOTICE 和许可文本 |
| `chali-1.0.0-llvm-external-windows-x64.msi` | 18.78 MiB | Windows x64 安装包，使用外部 LLVM/Clang 工具链；适合已有工具链的环境 |
| `chali-1.0.0-llvm-static-windows-x64.msi` | 52.31 MiB | Windows x64 安装包，内置 LLVM 工具链；推荐一般用户选择 |
| `ChaLi-1.0.0-reference.chm` | 未生成（缺少 hhc.exe） | Windows 离线参考手册，集中查阅语法、用法、教程与示例 |
| `chali-editor-1.0.0-llvm-external.vsix` | 21.93 MiB | VS Code 扩展，内嵌外置 LLVM 版编译器，构建需要外部工具链 |
| `chali-editor-1.0.0-llvm-static.vsix` | 64.55 MiB | VS Code 扩展，内嵌静态 LLVM 版编译器，提供语言服务与开发入口 |
Debian/Ubuntu 使用 DEB；以下名称以 Ubuntu 22.04 amd64 构建为例：

```sh
sudo apt install ./chali-1.0.0-llvm-external-ubuntu-22.04-x64.deb
```

CentOS Stream 使用该发行版环境生成的 RPM：

```sh
sudo dnf install ./chali-1.0.0-llvm-external-centos-9-x64.rpm
```

`llvm-external` 版本在执行 `run` / `build` 时需要匹配的 Clang 和 LLD。CentOS Stream 9 可使用 `sudo dnf install clang lld`；仅安装 Clang 而缺少 `ld.lld` 时，`-fuse-ld=lld` 会报告链接器不可用。编译器内嵌 GUI、音频、XML、密码学的原生归档，生成程序无需额外部署 ChaLi、raylib、Expat 或 libsodium 共享库。

无包管理器时可解压对应发行版的 tar.gz，再将其中的 `usr/bin/chali` 放入自己的 PATH。安装包安装的是编译器，不会替应用启动数据库、复制应用的静态页面或安装其自定义 C ABI 动态库。

## 文件名与架构

文件名前缀为 `chali-版本-llvm-链接方式-系统或发行版-架构`：

| 目标 | 文件名前缀示例 | 包内架构 |
| --- | --- | --- |
| Windows amd64 | `chali-1.0.0-llvm-external-windows-x64` | PE x64 |
| Ubuntu amd64 | `chali-1.0.0-llvm-external-ubuntu-22.04-x64` | DEB amd64、RPM x86_64 |
| Ubuntu ARM64 | `chali-1.0.0-llvm-external-ubuntu-22.04-arm64` | DEB arm64、RPM aarch64 |
| CentOS Stream 9 amd64 | `chali-1.0.0-llvm-external-centos-9-x64` | RPM x86_64 |

静态 LLVM 使用 `llvm-static`，外置工具链使用 `llvm-external`。发行版名称读取实际构建环境的 `/etc/os-release`，不是把一个发行版的二进制改名成另一个。安装包生成器读取 PE/ELF 机器类型，拒绝架构不符的输入；相邻 JSON 记录编译器和安装包的 SHA256。

表格列出构建目标及命名规则，实际可生成项取决于本机工具链。交叉编译成功不等于已在目标机器运行验收。当前不提供 x86 安装包：32 位编译可以完成，但运行时未通过启动验证。ARM64 包提供编译器前端和语言服务，当前原生 AOT 运行时布局面向 x64。

## 自动构建

Windows 需要受支持的 GNU C++17 工具链、Python 3.11 以上、Git Bash，以及 WiX 的 `wix` 命令用于生成 MSI：

```powershell
python tools/build.py --jobs 4
python tools/build.py --variant external --jobs 4
python tools/build.py --wsl never --jobs 4
```

入口自动检测可运行 Linux 命令的 WSL 发行版；没有可用 WSL 时正常保留本机产物。默认安装包使用静态内置 LLVM，`--variant external` 选择外置工具链版本。x86 目标不进入发布构建，也不能通过安装包生成器打包。

WSL/Linux 使用本机工具链及已安装的交叉工具链。在 x64 Ubuntu 上构建 ARM64 包需准备 `g++-aarch64-linux-gnu` 和 `qemu-user`，构建器用 QEMU 和目标运行库执行阶段编译器来生成内嵌标准库。DEB 需要 `dpkg-deb`，RPM 需要 `rpmbuild`。构建器先探测编译链接与 ELF 机器类型。自动模式会跳过缺少交叉工具链或模拟器的目标。

```sh
python3.11 tools/build_linux_packages.py --available --jobs 4
python3.11 tools/build_linux_packages.py --arch amd64 arm64 --jobs 4
```

各目标源码副本、对象和中转文件放在仓库的 `tmp/` 内，产物位于 `dist/installers/`。Linux 副本复用本次宿主生成的解析器，因此首次构建前要先在具备 ParseEngine 的宿主执行 `sh gen.sh`。内嵌标准库及原生服务资产仍按各目标重新生成。

## CentOS 构建环境

仓库提供固定基础镜像摘要的 [CentOS Stream 9 构建容器](../tools/packaging/centos-stream9.Dockerfile)，启用 CRB 取得静态开发依赖。使用 Docker 或兼容环境构建该镜像后，在实际 CentOS 用户空间运行构建器：

```sh
docker build -f tools/packaging/centos-stream9.Dockerfile -t chali-centos-stream9-builder tools/packaging
docker run --rm -v "$PWD:/workspace" chali-centos-stream9-builder
```

容器默认构建其中可用的架构，输出 RPM 和 tar.gz。CentOS RPM 的系统依赖由 RPM 工具提取；DEB 的最低 glibc 要求读取 ELF 符号版本。Ubuntu 22.04 构建不承诺兼容所有更旧发行版，CentOS Stream 9 构建也不等于 CentOS 7 兼容。

## 离线参考手册（CHM）

`ChaLi-1.0.0-reference.chm` 整合语言参考、常见语法与用法、完整中文书稿（含附录）、标准库 API、引用的 ChaLi 示例源码和插图，提供目录、关键字索引和全文搜索。独立 HTML 使用 `ChaLi-1.0.0-reference-html.zip` 压缩后作为发行附件；插图已内嵌，解压后只需一个 HTML 文件。

生成脚本为 [`tools/build_reference_chm.py`](../tools/build_reference_chm.py)。在 Windows 上准备 Microsoft HTML Help Workshop 的 `hhc.exe`，然后运行：

```powershell
python -m pip install -r tools/requirements_chm.txt
python tools/build_reference_chm.py --compiler "C:\Program Files (x86)\HTML Help Workshop\hhc.exe" --output dist/release/ChaLi-1.0.0-reference.chm
python tools/export_library_html.py --output tmp/ChaLi-1.0.0-reference.html --zip-output dist/release/ChaLi-1.0.0-reference-html.zip
```

编译器信息见 [Microsoft HTML Help](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads)；[Help+Manual 提供原版 Workshop 下载入口](https://www.helpandmanual.com/downloads_mscomp.html)。脚本也会检测 PATH、常见安装目录和 `build/dependencies/html-help-workshop/`。便携编译器目录包含 `hhc.exe`、`hha.dll`、`itcc.dll` 时，脚本自动准备免注册 COM 清单，无需修改系统注册表。

CHM 工程的中间 HTML、目录和索引只放在仓库 `tmp/` 中。`--prepare-only` 可在任意平台准备工程，实际 CHM 编译需要 Windows；单独下载任一脚本时用 `--repo 仓库路径` 指定完整源码目录。两种格式共享同一套原文，默认都包括书稿与示例；HTML 脚本的 `--api-only` 选项可仅导出 API。

版本、反射默认配置和平台能力见[当前能力](17-capabilities.md)，内置与外置 LLVM 的区别见 [发布构建](38-aot.md)。
