# LLVM AOT 编译

<!-- LANGUAGE NAVIGATION -->
[English](en/38-aot.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

从 1.0.8 起，`run` 使用 checked AOT 编译执行，`aot` 默认 unchecked 导出；编译器不再内置 VM 执行器。`.svmb` 导出已删除，新项目使用 AOT 原生产物。原生调试见[原生 AOT 调试](43-native-debugging.md)。

`chali aot input.chl --out build/program` 将前端生成并优化的四元式直接降低为 LLVM 中间表示，由 LLVM 优化、生成机器码并链接原生产物。也可使用 `chali --mode aot input.chl`。默认源码运行同样使用 AOT，且开启检查。

Windows x64 依赖通过 [在线下载脚本](../library/llvm/README.md) 准备。外置后端运行 `python tools/fetch_llvm_toolchain.py`；静态后端运行 `python tools/fetch_llvm_sdk.py`，下载预编译 LLVM/LLD 和匹配 GCC，全部缓存位于 `build/`。仓库不保存 SDK 或离线分片；归档按固定 SHA-256 校验。只导出 LLVM IR 不需要代码生成工具链。

AOT 的 LLVM 优化始终全开：外置 LLVM 使用 `-O3`，静态 LLVM 使用 `default<O3>` 与激进代码生成等级，保留正常浮点语义与存活检查；run 开启数组和指针检查，aot 导出缺省关闭这些检查。四元式不做内部优化。批量复制、直接调用和可选计数的适用条件与对照开关见[AOT 降低开关与计数](40-backend-ownership.md)。

| 参数 | 行为 |
| --- | --- |
| `--out 前缀` | 输出 `.ll` 模块、C ABI 头文件及原生产物；Windows 使用 .exe/.dll，Linux 使用无后缀可执行文件或 .so |
| `--llvm 路径` | 指定外置 LLVM clang 驱动；静态变体缺省使用内置后端，外置变体也可用 `CHALI_LLVM_CLANG` 选择工具 |
| `--emit-llvm` | 只生成 LLVM 模块与 C ABI 头文件，不执行目标代码生成或链接 |
| `--checked` | 生成四元式的数组边界与指针检查；run 默认启用，aot 导出默认关闭 |
| `--unchecked` | 显式关闭这些检查，仅用于调试或性能对照；仍检查对象存活与实际内存访问 |
| `--shared` | 生成动态库 |
| `--export 名称` | 指定对外导出的函数，可重复使用 |
| `--link 库路径` | 添加链接库，可重复使用；标量 ABI 声明中的库自动加入，复杂 ABI 由类型化胶水加载声明的动态库 |

产物自带内存、字符串、列表、打印和 panic 运行时，不依赖编译器、运行期库或 LLVM 动态库。覆盖基本数值运算、分支、循环、直接函数调用、递归和固定数组。列表支持创建、长度、追加、插入、下标读写、删除、弹出、清空、查找、值复制和 for-in 引用；元素覆盖整数、浮点、String、嵌套列表及支持叶子布局的结构体。列表值复制保持独立存储，活跃元素引用期间禁止结构性修改，列表下标和对象存活验证始终执行。

语言 `opcode` 的创建、复制、移动和销毁沿用前端降级的真实调用；成员字段、聚合拥有字段以及 `#NonCopyable` 的静态限制保持一致。用户可以将 `new/drop/copy` 绑定至可达的 ChaLi 函数或显式 `abi` 声明的 C 函数。宿主资源的句柄不转换为机器函数指针。字符串、列表和其他标准库调用复用运行期统一的堆、对象表与胶水注册，语言函数体执行 LLVM 生成的机器码。捕获闭包、函数值和同步标准库回调通过统一调用协议分发；返回值保留整数、浮点与对象句柄的槽类型。隔离线程复用共享的消息、通道、事件和任务控制服务。标量 C ABI 调用使用对应 LLVM 类型和显式转换；结构体、引用、数组、可选引用及同步 C 回调使用统一的类型化胶水，保留窄标量位宽、引用别名、写回、返回数据复制及释放协议。共享库头文件提供初始化、清理及导出函数声明。

复杂 C ABI 签名在导出时还需要与内嵌宿主兼容的 GNU C++17 编译器，默认调用 g++，可用 CHALI_ABI_CXX 指定路径。它只编译宿主封送适配，语言函数体仍直接生成 LLVM 指令；适配使用内嵌头文件，不依赖源码目录。静态 LLVM 变体也遵循这一宿主依赖边界。仅使用标量 ABI 或没有 ABI 声明时无需该步骤；交付的原生产物无需 C++ 编译器，应用声明的动态库仍须提供。

## 编译器的 LLVM 链接方式

| 构建方式 | 原生产物生成 | 插件与发布包标记 |
| --- | --- | --- |
| `make` 或 `make CHALI_STATIC_LLVM=0` | 调用外置 LLVM clang；工具链位于 `library/llvm/` | `llvm-external` |
| `make llvm-static` | 静态链接 LLVM 优化与 x64 代码生成库、LLD；内嵌 CRT 链接资产，编译时无需 clang 或 LLVM DLL | `llvm-static` |

首次静态构建由 [setup_static.py](../library/llvm/setup_static.py) 自动下载固定版本 LLVM 源码、核对 SHA-256，再在 `library/llvm/` 内增量构建静态开发库。已有开发库时可直接使用 `make CHALI_STATIC_LLVM=1`；Windows x64 的编译器产物为 `chali-1.0.8-llvm-static-windows-x64.exe`，外置变体为 `chali-1.0.8-llvm-external-windows-x64.exe`，文件名版本取自 `VERSION`。`CHALI_LLVM_SDK` 可指定同 ABI 的开发库目录。当前静态分发验收目标为 Windows x64。静态变体默认使用内置后端；显式 `--llvm` 可选择外部工具。只导出 LLVM 模块时，两种变体都不需要运行代码生成工具。

`python tools/release_variants.py` 顺序构建和验收两个程序与插件变体，发布 ZIP 分别保存在 `dist/llvm-static/` 与 `dist/llvm-external/`。插件版本均为独立的 1.0.0，VSIX 文件名分别使用 `-llvm-static` 与 `-llvm-external` 后缀；二者安装为同一个扩展的替代变体。单独打包可在 `vscode/` 使用 `npm run package -- --llvm-variant static --compiler 编译器路径`，或将 static 改为 external。发布脚本只生成本地产物，远端发布仍需显式执行。

程序代码直接生成 LLVM 指令、基本块和全局存储，不经过 C/C++ 程序源码。手写的 [AOT 运行时](../source/aot/runtime.c) 在维护期使用固定版本 LLVM 生成为平台模块，再内嵌进编译器；执行 `python tools/build_aot_runtime.py --check` 可验证生成结果同步。当前运行时提供 Windows x64 与 Linux x64 布局，Windows x64 已提供配套工具分发。Linux x64 构建机需准备对应宿主的 LLVM clang 和系统链接环境。

`--incremental` 在 AOT 路径不复用编译产物，ABI 声明来自本次语义分析。功能验证由 [AOT 回归](../tests/chali_aot.py)、[列表与 opcode 差分回归](../tests/chali_aot_values.py)、[复杂 ABI 值与引用回归](../tests/chali_abi_values.py)（--aot）、[C 回调及返回所有权回归](../tests/chali_abi_contracts.py)（--aot）和[静态 LLVM 隔离回归](../tests/chali_aot_static.py)覆盖。

## 导出无入口的动态库

`export` 放在普通函数定义前，标记供 AOT 原生调用方使用的公开入口。例如：

```chali
#export("f_add")
int64 add(int64 left, int64 right)
{
    return left + right;
}
```

执行 `ChaLi aot library.chl --shared --out build/library` 生成动态库和 C ABI 头文件，
导出函数名以生成的头文件为准（上例为 `f_add`）。动态库源码可以没有 `main`；
可执行程序仍需要 `main`。调用方先调用 `chali_init()`，再使用导出函数，最后调用
`chali_finish()`；全局初始化与清理分别在这两个入口执行。`--export` 可以额外指定导出函数。
模块限定名和重载使用各自的实际标签；应以生成头文件核对完整签名。

Windows 原生进程入口从 CRT 宽字符参数转换到 UTF-8，再交给 main 的 List<String>；中文、非 BMP 字符、空串、引号及反斜线均保留。入口行为的回归为 python tests/chali_aot_arguments.py，同时检查中文与空格产物路径。

在线工具链下载也支持 Linux x64、Linux ARM64 与 macOS ARM64，可运行 `python tools/fetch_llvm_toolchain.py --list` 查看，或用 `--platform` 显式选择。静态 SDK 自动准备目前限定 Windows x64；下载能力不代表相应平台 AOT 已验收。
