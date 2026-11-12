# LLVM 构建

<!-- LANGUAGE NAVIGATION -->
[English](en/38-build.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

`build` 是发布构建命令；原 `aot` 命令已移除。项目配置使用 `[build]`，不再接受 `[aot]`。LLVM 构建由 LLVM 负责优化、目标代码生成与原生链接。

`run`／`build` 成功编译并发布最终产物后，会在标准错误输出“编译完成：产物路径”。run 先提示再执行程序，后续运行失败仍返回失败；程序标准输出不混入提示。`--quiet` 或 `CHALI_QUIET=1` 仅关闭完成提示，不隐藏错误、统计或程序输出。仅检查及 JSON 诊断不发送完成提示。

默认 `run` 与 `build` 的最终产物位于 `.chl` 同级，run 生成的可执行文件也会保留。中间文件使用源码旁 `.chali` 中的独占工作区，Windows 下隐藏，成功或失败结束后清理并删除空目录；既有用户文件保留。显式输出与保留选项继续可用。VSCode 默认同样输出到源码旁；static VSIX 不提供 `Chali › Build: Llvm Path`，external 必须配置，构建和运行均传递 `--llvm`。

`run` 使用 checked 原生构建 编译执行，`build` 默认 unchecked 导出。原生调试见[构建调试](43-native-debugging.md)。

### Windows 应用的控制台

Windows 可执行文件默认使用控制台子系统。GUI 应用可用 `--subsystem windows`，从资源管理器启动时不会为它创建控制台窗口。该选项只选择 PE 子系统，应用窗口由程序自己创建。

```powershell
chali build app.chl --subsystem console --out app-console
chali build app.chl --subsystem windows --out app-gui
```

在 `chali.toml` 的 `[build]` 中设置 `subsystem = "windows"` 可用于项目构建，`run` 和编辑器构建也会读取该设置。命令行选项优先。分步构建时把选项传给 `chali link`；导出的 LLVM IR、汇编或对象尚未设置 PE 子系统。

`chali build input.chl --out build/program` 从语义检查后的 AST 直接生成 LLVM 代码，不经过四元式生成或组装，由 LLVM 优化、生成机器码并链接原生产物。也可使用 `chali --mode build input.chl`。默认源码运行同样使用 LLVM 构建，且开启检查。

Windows x64 依赖通过 [在线下载脚本](../library/llvm/README.md) 准备。外置后端运行 `python tools/fetch_llvm_toolchain.py`；静态后端运行 `python tools/fetch_llvm_sdk.py`，下载预编译 LLVM/LLD 和匹配 GCC，全部缓存位于 `build/`。仓库不保存 SDK 或离线分片；归档按固定 SHA-256 校验。只导出 LLVM IR 不需要代码生成工具链。

LLVM 构建 的 LLVM 优化始终全开：外置 LLVM 使用 `-O3`，静态 LLVM 使用 `default<O3>` 与激进代码生成等级，保留正常浮点语义与存活检查；run 开启数组和指针检查，发布构建缺省关闭这些检查。四元式不做内部优化。批量复制、直接调用和可选计数的适用条件与对照开关见[LLVM 构建 降低开关与计数](40-backend-ownership.md)。

| 参数 | 行为 |
| --- | --- |
| `--out 前缀` | 输出 `.ll` 模块、C ABI 头文件及原生产物；Windows 使用 .exe/.dll，Linux 使用无后缀可执行文件或 .so |
| `--llvm 路径` | 指定外置 LLVM clang 驱动；仅外置变体接受此选项；静态变体报参数错误。外置变体也可用 `CHALI_LLVM_CLANG` 选择工具 |
| `--emit-llvm` | 只生成 LLVM 模块与 C ABI 头文件，不执行目标代码生成或链接 |
| `--allocator malloc\|pool` | 0.32.10 起缺省使用系统 malloc；pool 保留连续 arena 与原有字节堆分配策略 |
| `--checked` | 生成 AST 中已检查的数组边界与指针检查；run 默认启用，发布构建默认关闭 |
| `--unchecked` | 显式关闭这些检查，仅用于调试或性能对照；仍检查对象存活与实际内存访问 |
| `--shared` | 生成动态库 |
| `--export 名称` | 指定对外导出的函数，可重复使用 |
| `--link 库路径` | 添加链接库，可重复使用；标量 ABI 声明中的库自动加入，复杂 ABI 由类型化胶水加载声明的动态库 |

产物自带内存、字符串、列表、打印和 panic 运行时，不依赖编译器、运行期库或 LLVM 动态库。覆盖基本数值运算、分支、循环、直接函数调用、递归和固定数组。列表支持创建、长度、追加、插入、下标读写、删除、弹出、清空、查找、值复制和 for-in 引用；元素覆盖整数、浮点、String、嵌套列表及支持叶子布局的结构体。列表值复制保持独立存储，活跃元素引用期间禁止结构性修改，列表下标和对象存活验证始终执行。

语言 `opcode` 的创建、复制、移动和销毁沿用前端降级的真实调用；成员字段、聚合拥有字段以及 `#NonCopyable` 的静态限制保持一致。用户可以将 `new/drop/copy` 绑定至可达的 ChaLi 函数或显式 `abi` 声明的 C 函数。宿主资源的句柄不转换为机器函数指针。字符串、列表和其他标准库调用复用运行期统一的堆、对象表与胶水注册，语言函数体执行 LLVM 生成的机器码。捕获闭包、函数值和同步标准库回调通过统一调用协议分发；返回值保留整数、浮点与对象句柄的槽类型。隔离线程复用共享的消息、通道、事件和任务控制服务。标量 C ABI 调用使用对应 LLVM 类型和显式转换；结构体、引用、数组、可选引用及同步 C 回调使用统一的类型化胶水，保留窄标量位宽、引用别名、写回、返回数据复制及释放协议。共享库头文件提供初始化、清理及导出函数声明。

复杂 C ABI 签名在导出时还需要与内嵌宿主兼容的 GNU C++17 编译器，默认调用 g++，可用 CHALI_ABI_CXX 指定路径。它只编译宿主封送适配，语言函数体仍直接生成 LLVM 指令；适配使用内嵌头文件，不依赖源码目录。静态 LLVM 变体也遵循这一宿主依赖边界。仅使用标量 ABI 或没有 ABI 声明时无需该步骤；交付的原生产物无需 C++ 编译器，应用声明的动态库仍须提供。

## 运行分配器

0.32.10 起，原生程序默认使用系统 `malloc`，可通过 `--allocator pool` 选择原有内存池。系统模式同时作用于宿主对象与语言字节堆：每块语言存储独立分配，不再从连续字节缓冲区细分。逻辑引用身份、释放撤销、范围验证、零初始化和内存预算继续保留；`--unchecked` 不关闭这些保护。

```sh
chali build app.chl --allocator malloc --out build/app-malloc
chali build app.chl --allocator pool --out build/app-pool
```

项目可在 `[build]` 设置 `allocator = "malloc"` 或 `allocator = "pool"`，显式命令行覆盖清单。此选项选择导出程序的分配后端，不改变编译器自身的容器 ABI。性能比较以系统 malloc 为正式基准，记录 checked/unchecked、LLVM 与分配器配置；不同配置分别保存原始数据。

## 编译器的 LLVM 链接方式

| 构建方式 | 原生产物生成 | 插件与发布包标记 |
| --- | --- | --- |
| `make` 或 `make CHALI_STATIC_LLVM=0` | 调用外置 LLVM clang；工具链位于 `library/llvm/` | `llvm-external` |
| `make llvm-static` | 静态链接 LLVM 优化与 x64 代码生成库、LLD；内嵌 CRT 链接资产，编译时无需 clang 或 LLVM DLL | `llvm-static` |

首次静态构建由 [setup_static.py](../library/llvm/setup_static.py) 自动下载固定版本 LLVM/LLD 预编译开发库与匹配的 GCC，核对 SHA-256 并安装到隔离 SDK 目录。已有开发库时可直接使用 `make CHALI_STATIC_LLVM=1`；Windows x64 的编译器产物为 `chali-1.0.0-llvm-static-windows-x64.exe`，外置变体为 `chali-1.0.0-llvm-external-windows-x64.exe`，文件名版本取自 `VERSION`。`CHALI_LLVM_SDK` 可指定同 ABI 的开发库目录。当前静态分发验收目标为 Windows x64。静态变体使用内置后端，`run`、`build`、`asm`、`ar`、`link` 均拒绝 `--llvm`（退出码 2），工程 `[build].llvm` 也会报错。需要指定外部工具时使用 `llvm-external` 变体；静态版不会用该选项切换后端。只导出 LLVM 模块时，两种变体都不需要运行代码生成工具。

`python tools/release_variants.py` 顺序构建和验收两个程序与插件变体，发布 ZIP 分别保存在 `dist/llvm-static/` 与 `dist/llvm-external/`。插件版本与 VERSION 中的编译器版本一致，VSIX 文件名分别使用 `-llvm-static` 与 `-llvm-external` 后缀；二者安装为同一个扩展的替代变体。单独打包可在 `vscode/` 使用 `npm run package -- --llvm-variant static --compiler 编译器路径`，或将 static 改为 external。发布脚本只生成本地产物，远端发布仍需显式执行。

程序代码直接生成 LLVM 指令、基本块和全局存储，不经过 C/C++ 程序源码。手写的 [原生运行时](../source/aot/runtime.c) 在维护期使用固定版本 LLVM 生成为平台模块，再内嵌进编译器；执行 `sh tools/build_aot_runtime.sh --check` 可验证生成结果同步。当前运行时提供 Windows x64 及 Linux x64、x86、ARM32、ARM64、RISC-V 32/64 和 MIPS32/64 小端布局，Windows x64 已提供配套工具分发。Linux x64 构建机需准备对应宿主的 LLVM clang 和系统链接环境。

`--incremental` 在 LLVM 构建 路径不复用编译产物，ABI 声明来自本次语义分析。功能验证由 [LLVM 构建 回归](../tests/chali_aot.py)、[列表与 opcode 差分回归](../tests/chali_aot_values.py)、[复杂 ABI 值与引用回归](../tests/chali_abi_values.py)（--aot）、[C 回调及返回所有权回归](../tests/chali_abi_contracts.py)（--aot）和[静态 LLVM 隔离回归](../tests/chali_aot_static.py)覆盖。

## 导出无入口的动态库

`export` 放在普通函数定义前，标记供 原生调用方使用的公开入口。例如：

```chali
#export("f_add")
int64 add(int64 left, int64 right)
{
    return left + right;
}
```

执行 `ChaLi build library.chl --shared --out build/library` 生成动态库和 C ABI 头文件，
导出函数名以生成的头文件为准（上例为 `f_add`）。动态库源码可以没有 `main`；
可执行程序仍需要 `main`。调用方先调用 `chali_init()`，再使用导出函数，最后调用
`chali_finish()`；全局初始化与清理分别在这两个入口执行。`--export` 可以额外指定导出函数。
模块限定名和重载使用各自的实际标签；应以生成头文件核对完整签名。

Windows 原生进程入口从 CRT 宽字符参数转换到 UTF-8，再交给 main 的 List<String>；中文、非 BMP 字符、空串、引号及反斜线均保留。入口行为的回归为 python tests/chali_aot_arguments.py，同时检查中文与空格产物路径。

在线工具链下载也支持 Linux x64、Linux ARM64 与 macOS ARM64，可运行 `python tools/fetch_llvm_toolchain.py --list` 查看，或用 `--platform` 显式选择。静态 SDK 自动准备目前限定 Windows x64；下载能力不代表相应平台 LLVM 构建 已验收。

## 交叉编译与项目配置

交叉编译时，编译器运行的平台是宿主，生成程序运行的平台是目标。目标可使用下列简写或规范三元组。

| 简写 | LLVM 三元组 | 目标 ABI |
| --- | --- | --- |
| `windows-x64` | `x86_64-w64-windows-gnu` | MinGW x64 |
| `linux-x64` | `x86_64-unknown-linux-gnu` | x86-64 GNU |
| `linux-arm32` | `armv7-unknown-linux-gnueabihf` | ARMv7-A GNU 硬浮点 |
| `linux-arm64` | `aarch64-unknown-linux-gnu` | AArch64 GNU |
| `linux-x86` | `i686-unknown-linux-gnu` | 32 位 x86 GNU |
| `linux-riscv32` | `riscv32-unknown-linux-gnu` | RV32GC / ILP32D |
| `linux-riscv64` | `riscv64-unknown-linux-gnu` | RV64GC / LP64D |
| `linux-mipsel` | `mipsel-unknown-linux-gnu` | 小端 MIPS32r2 / O32 |
| `linux-mips64el` | `mips64el-unknown-linux-gnuabi64` | 小端 MIPS64r2 / N64 |

先导出目标 LLVM IR 或目标文件，不必准备目标链接库：

```sh
chali build aot_demo.chl --target linux-arm64 --emit-llvm --out build/arm64/demo
chali build aot_demo.chl --target linux-arm64 --emit-obj --out build/arm64/demo
```

`--emit-llvm` 生成目标对应的 `.ll`；`--emit-obj` 继续生成 `.o`，跳过链接。Windows 目标的 `.o` 内容为 COFF，Linux 目标为 ELF，文件格式由目标决定。两种选项不能同时使用。静态 LLVM 变体可通过内置后端生成目标文件；外置变体需要包含对应目标后端的 clang。

完整链接还需要目标平台的 C/C++ 运行库、启动对象、链接器和 ChaLi 运行时。`--sysroot` 指定目标系统根目录，`--runtime-dir` 指定包含目标 `libchali_aot_host.a` 的目录；库的目标架构和 ABI 必须与 `--target` 一致。`--cpu` 可进一步选择目标 CPU。仓库提供在 Linux 或 WSL 中准备 GNU 目标运行时 SDK 的脚本，需要先安装对应的 GNU C/C++ 工具链：

```sh
sh tools/build_cross_runtime.sh linux-arm64 2
```

脚本也接受 `linux-x86`、`linux-arm32`、`linux-riscv32`、`linux-riscv64`、`linux-mipsel` 和 `linux-mips64el`，分别使用 `i686-linux-gnu`、`arm-linux-gnueabihf`、`riscv32-linux-gnu`、`riscv64-linux-gnu`、`mipsel-linux-gnu` 和 `mips64el-linux-gnuabi64` 的 GCC/G++ 工具链。脚本隔离构建目标对象，并在 `build/cross-sdk/<目标简写>/` 生成 `sysroot/`、`lib/`、许可证和摘要。Windows 编译器使用该目录时，应填写 Windows 能访问的实际路径。

完整交叉链接使用外部 clang 和 LLD。若当前 LLVM 发行版缺少所选目标后端，可用 `CHALI_LLVM_CLANG` 指向带该后端的 clang，并让对应的 LLD 位于 `PATH`。MIPS SDK 若含明确要求可执行栈的 GNU 启动对象，准备脚本会记录该要求，链接时按记录设置 ELF 栈权限。

MIPS32 和 MIPS64 可执行文件使用非 PIE 和常规机器码对象链接，保留目标代码优化并跳过 LTO。MIPS32 和 RV32 运行环境需要匹配的 `libatomic.so.1`。

在 `chali.toml` 中集中保存配置：

```toml
[build]
target = "aarch64-unknown-linux-gnu"
cpu = "generic"
sysroot = "build/cross-sdk/linux-arm64/sysroot"
runtime_dir = "build/cross-sdk/linux-arm64/lib"
output = "executable"
```

路径相对于 `chali.toml` 所在目录解析。`output` 可选 `llvm`、`object` 或 `executable`；`llvm` 配置项可指定外置 clang 路径。执行 `chali build` 会读取项目配置，显式命令行参数覆盖对应设置：

```sh
chali build --out build/arm64/app
chali build --target linux-x64 --emit-obj --out build/x64/app
```

生成成功后应在目标机器或相应模拟器中验证参数、标准输入输出、文件、资源清理和错误路径。`run` 不会自动启动模拟器，目标与宿主不同时会拒绝直接运行。导出目标文件成功仅证明代码生成完成，不能据此声称目标链接或实际运行已验证。复杂 C ABI 的跨目标完整链接需要兼容的目标适配器；当前可先导出目标文件，并在目标工具链中完成适配器编译与链接。

## 目标机器汇编

项目 `[build]` 支持 `assembly_sources`（汇编源）、`assembly_includes`（汇编包含目录）、`objects`（对象文件）、`libraries`（链接库）、`search_paths`（模块及包含搜索目录）数组。路径以 `chali.toml` 所在目录为基准；`libraries` 中不含点或路径分隔符的裸库名交给链接器解析。

显式 `--asm`、`--asm-include`、`--link` 分别覆盖清单中的同类列表。清单搜索目录追加在命令行搜索目录之后。对象及汇编输入用于最终链接。

`lto` 默认 `true`（完整 LTO），也接受 `false`、`"off"`、`"full"`、`"thin"`。`--lto-mode off|full|thin` 覆盖项目设置；`--lto` 和 `--no-lto` 分别保留完整 LTO 与关闭语义，多个命令行模式开关以最后一个为准。关闭时先生成机器对象再链接，LLVM 的模块内优化仍按原优化级别执行。静态与外置 LLVM 的 `--debug` 开发调试按所选 LTO 模式使用 O0；`--debug-optimized` 明确保留 DWARF 并使用 O3，详见[原生调试](43-native-debugging.md)。

独立 `chali link --shared --out FILE` 在私有临时目录中使用最终文件名完成链接，成功后再发布。因此 Windows DLL 的导出表与宿主导入表记录实际交付名称，C 宿主可直接链接并加载该文件；失败仍保留原产物。

`chali build app.chl --emit-asm --out build/app` 按实际目标、CPU、检查及优化／调试设置导出 `build/app.s`。这是机器汇编，与 LLVM IR 文本 `.ll` 不同。输出在对象生成和链接之前停止，跨目标导出不要求准备目标运行时链接资产。静态变体使用内置 LLVM，外置变体使用 clang；采用相应后端默认语法（x86 为 AT&T，AArch64 为目标语法），不表示兼容 NASM／MASM。

项目可设置 `[build] output = "assembly"`。显式 `--emit-asm`、`--emit-obj` 或 `--emit-llvm` 覆盖项目输出，三个选项互斥；普通 `run` 仍执行 checked 原生构建。VSCode 输出选择和构建支持汇编及 `.s` 后缀。

同时保留 `.ll` 和适用的 C ABI 头文件；复杂 C ABI 还可能生成 `.abi.cpp` 封送侧文件，单独的汇编文件不等于已链接应用。重新汇编和最终链接需要匹配的目标工具链、运行时及依赖。独立汇编、链接与归档入口见下文。

## 独立汇编器与汇编输入

```sh
chali asm native/add.s --target windows-x64 --out build/add.obj -I native/include
chali asm native/add.s --target linux-x64 --out build/add.o
chali build main.chl --asm native/add.s --asm-include native/include --out build/app
```

`asm` 每次接受一个 `.s` 和完整输出路径。不写 `--out` 时，Windows 默认 `.obj`，Linux 默认 `.o`。支持 `--target`、`--cpu`、`--debug`、`-I DIR`／`-IDIR`、`--include-path DIR` 和仅外置变体支持的 `--llvm CLANG`。源指令必须匹配目标和 CPU；采用 LLVM 的 GAS 兼容语法，不处理 `.S` 预处理、NASM 或 MASM。包含目录先加入源文件所在目录，再按显式目录顺序搜索。主输入最多 64 MiB；该入口是编译工具，不是面向不可信代码的沙箱。

静态变体直接调用 LLVM MC，无需 clang，并拒绝 `--llvm`；外置变体使用该选项指定集成汇编器。对象先生成在独占工作区，汇编成功后才发布，失败保留已有产物，且禁止覆盖自己的主输入。输出父目录必须存在。源文件及包含文件的诊断保留实际位置。

LLVM 构建 可重复指定 `--asm FILE.s` 和 `--asm-include DIR`，每个对象使用语言单元相同的目标和 CPU，参与最终链接。通过既有 C ABI 声明引用汇编符号，汇编须遵循目标调用约定。该输入只适用于可执行文件或共享库输出，不能与 `--emit-asm`、`--emit-obj`、`--emit-llvm` 组合。生成交叉对象不等于已经验证目标系统的可执行交付。

链接的标量 C 函数可声明为 `#abi("C", "@link", "asm_add") int64 asm_add(int64 a, int64 b);`。符号由交给链接器的对象或归档提供。结构体、引用、缓冲区和回调签名使用兼容的动态库，遵循 [C ABI 封送约定](41-unsafe-abi.md)。


## 独立链接与静态归档

```sh
chali ar build/first.o build/second.o --out build/libexample.a
chali link build/main.o build/libexample.a --out build/app.exe
chali link build/main.o -L build --library example --out build/app.exe
chali link build/public.o --shared --export public_entry --out build/example.dll
chali link build/start.o --target linux-arm64 --no-runtime --entry _start --out build/app
```

`ar` 创建带符号索引的确定性自包含归档，接受零个或多个同目标对象；时间、UID 和 GID 不随构建变化。Windows 的 `.lib` 选择 COFF 归档，其他后缀使用 GNU ar 格式。薄归档和嵌套归档不接受。`--out` 指定完整文件名，父目录须已存在。静态变体调用 LLVM ArchiveWriter；外置变体查找 clang 旁的 llvm-ar 或 `CHALI_LLVM_AR`，也可用 `--llvm LLVM-AR` 显式指定；静态变体拒绝该选项。

`link` 接受多个对象／归档，支持 `--target`、`--shared`、重复 `--export SYMBOL`、`--entry SYMBOL`、`--debug`、`-L DIR`／`-LDIR`／`--library-path DIR`、重复 `--library NAME`、`--sysroot`、`--runtime-dir` 和仅外置变体支持的 `--llvm CLANG`。入口及导出使用原始对象符号名。对象和库按输入顺序进入同一链接组，归档按需提取并重复扫描，允许库间循环引用；未提取成员的未定义符号不引发错误。重复定义及已引用的缺失符号由 LLD 报告。共享库只公开显式导出的符号，其他符号隐藏；不提供任意原始链接器参数透传。

静态 Windows x64 变体默认使用内置 LLD、CRT 与 ChaLi 运行时，无需外置 clang；外置变体使用匹配 clang／LLD 和所需系统工具链。完整跨目标链接需要目标运行时与 SDK。显式 `--no-runtime` 不添加 CRT 或 ChaLi 运行时，可用于自备入口与依赖的程序；内置 LLD 可为已登记的 Linux 目标链接此类输入，生成成功不等于已在目标系统运行。普通独立 C 程序也可使用默认运行时链接，未被引用的归档成员不装入应用。

输入先检查对象格式、位宽、架构及归档成员边界；实际符号、节和重定位仍由 LLVM／LLD 检查。这些工具不构成执行不可信代码的沙箱。产物先在私有工作区生成，成功后才替换最终文件；失败保留原文件，禁止输出覆盖输入。链接错误不能作为旧产物仍有效的依据，调用方必须检查返回状态。


### 目标平台与布尔构建选项

`$system` 和 `$cpu` 是不带括号的字符串常量，分别读取目标程序的操作系统和 CPU 架构。Windows、macOS、Linux 对应 `"win"`、`"mac"`、`"linux"`；常见架构名称为 `"amd64"`、`"arm64"`、`"arm32"`。交叉构建读取选定目标，不读取运行编译器的宿主；`--cpu` 的指令调优名称不会改变架构宏。名称映射不表示新增了原生产物支持，当前可构建目标仍以 LLVM 构建章节为准。

```csharp
void main() {
    println("system={}, cpu={}", $system, $cpu);
    println("A={}, B={}", $build[A], $build["B"]);
}
```

命令行 `chali run main.chl -B A -BB` 定义 `A` 和 `B`。也可以在工程清单中声明：

```toml
[build]
build_option = ["A", "B"]
```

命令行与清单中的名称取并集；名称区分大小写，重复定义没有额外效果。`$build[A]` 与 `$build["A"]` 等价：前者的 `A` 是标志名称，不查找同名变量。已定义为 `true`，未定义为 `false`，不支持 `-BA=false`、数值或字符串取值。名称必须非空，只含英文字母、数字、下划线、点或连字符；含点或连字符时使用字符串下标。

在宏模板内，可以写 `$if $build[A]`、`$else`、`$end_if`，也可以用 `$if ($system == "win")` 选择目标代码。普通代码可以直接读取这些常量。`$build` 只接受名称或字符串字面量下标，不接受运行时计算表达式。`-I` 仍用于模块搜索目录。



### 用 TOML 保存构建配置

第 11 章的工程清单可以继续扩展 `[build]`。下面是 Windows x64 发布构建的配置片段；其他目标需要匹配的运行时、SDK 和链接输入。

```toml
[build]
target = "windows-x64"
output = "executable"
lto = "full"
allocator = "malloc"
subsystem = "console"
build_option = ["A", "B"]
assembly_sources = []
assembly_includes = []
objects = []
libraries = []
search_paths = ["src"]
```

| 配置项 | 含义与命令行关系 |
| --- | --- |
| target、cpu | 目标三元组或已支持别名、CPU 调优名称；显式 --target、--cpu 优先 |
| output | executable、llvm、object、assembly；显式 --emit-llvm、--emit-obj、--emit-asm 优先，run 仍编译并执行程序 |
| lto | true、false，或 full、thin、off；显式 LTO 选项优先 |
| allocator、subsystem | malloc／pool 和 Windows console／windows；显式同名选项优先 |
| sysroot、runtime_dir | 目标 SDK 根与目标运行时目录；显式对应选项优先 |
| assembly_sources、assembly_includes | 汇编源与汇编包含目录；命令行已给同类输入时使用命令行集合 |
| objects、libraries | 额外对象与链接库；命令行已给同类输入时使用命令行集合 |
| search_paths | 追加模块和导入搜索目录，以清单所在目录解析相对路径 |
| build_option | 布尔标志名称数组，与 -B 定义取并集；$build[name] 查询存在性 |

`libraries` 中没有路径分隔符和点的裸名称交给链接器解析；其他路径相对清单目录。`cpu` 选择指令调优，不改变 `$cpu` 表示的目标架构。`$system`、`$cpu` 与 `$build` 在宏展开阶段成为常量，具体写法见[第 22 章](book/22-预处理与宏.md)。

静态 LLVM 变体使用内置后端，拒绝 `--llvm` 和 `[build] llvm`。外置变体才允许配置外部工具链路径。不能通过修改 TOML 让静态版切换后端。完整选项见[LLVM 构建参考](38-build.md)，项目入口和搜索规则见[第 11 章](book/11-模块与工程.md)。
