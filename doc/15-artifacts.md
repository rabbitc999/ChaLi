# 构建产物与中间文件

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](15-artifacts.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

`run`／`aot` 成功编译并发布最终产物后，会在标准错误输出“编译完成：产物路径”。run 先提示再执行程序，后续运行失败仍返回失败；程序标准输出不混入提示。`--quiet` 或 `CHALI_QUIET=1` 仅关闭完成提示，不隐藏错误、统计或程序输出。仅检查及 JSON 诊断不发送完成提示。

ChaLi 只有一条执行路径：语义检查后的 AST 直接生成 LLVM 代码，再由 LLVM AOT 生成原生可执行文件。这一页说明**文件都落在哪里、
哪些会留下、哪些用完就删**。命令行开关见[工具链](09-toolchain.md)，AOT 选项见[AOT 编译](38-aot.md)。

## 三类产物

| 命令 | 产物 | 缺省位置 |
| --- | --- | --- |
| `chali run app.chl` | 原生可执行文件 + 运行结果 | 源码同级的 `app.exe`（Linux 为 `app`），运行结束保留 |
| `chali aot app.chl` | 原生可执行文件（Windows 为 `.exe`，Linux 无后缀） | `.chali/` 临时前缀，最终产物位于源码同级 |
| `chali aot app.chl --emit-llvm --out build/app` | `.ll` | 指定的 `build/app` 前缀 |
| `chali aot app.chl --target linux-arm64 --emit-obj --out build/app` | ARM64 ELF `.o` | 指定的 `build/app` 前缀 |

`emit` 模式已删除。`run` 与 `aot` 的中间文件默认留在 `.chali/` 下；显式 `--out` 将其保留在指定前缀。

## `.chali/` 目录

不给 `--out` 时，`run` 与 `aot` 在源码旁的 `.chali/build-唯一编号/` 中生成中间文件。每次构建独占子目录，不覆盖已有用户文件；Windows 上 `.chali` 保留其他属性并添加隐藏属性，资源管理器默认不显示。最终 exe、对象、机器汇编、LLVM IR 及必要 ABI 头／封送源码发布到 `.chl` 同级，成功后或失败返回时清理本次子目录，再删除空的 `.chali`。已有用户文件或其他构建仍在使用该目录时保留其内容；`.chali` 被普通文件、符号链接或重解析点占用时明确拒绝。

```text
.chali/build-唯一编号/
  app.ll           LLVM IR
  app.llvm.log     后端诊断
  app.abi.cpp      `#abi` 桥接源码（用到外部符号时才生成）
  app.h            导出头（带 `--export` 时生成，属交付物）
```

标准库核心二进制直接内嵌在编译器中，常规运行不创建标准库磁盘缓存。默认工作区在编译失败、工具失败和运行结束后均清理：

- `run`：先发布源码旁的可执行文件，再运行；执行结束清理工作区，最终可执行文件保留；
- `aot`：发布请求的最终格式并清理工作区；带 `--export` 时 `.h` 同样发布到源码旁。编译／链接失败保留既有最终文件。

## 什么时候不删

| 情形 | 保留原因 |
| --- | --- |
| `--keep-artifacts` | 显式要求保留，排查后端或看 IR 时用 |
| `--incremental` | 下次运行要复用缓存与产物 |
| `--emit-llvm` | `.ll` 是源码旁的最终产物，工作区仍清理 |
| `--emit-obj`／`--emit-asm` | `.o`／`.s` 是源码旁的最终产物，工作区仍清理 |
| 显式 `--out 前缀` | 用户点名了路径，编译器不动他的目录 |

显式输出时增量缓存目录是 `<前缀>.chali-cache`，默认输出使用源码旁的 `.chali/cache`，由 `--incremental` 生成；它按模块内容与编译开关做签名，
开关一变就重新预处理。

## 交付时需要带什么

`aot` 产物是原生可执行文件，**运行时不需要 ChaLi 编译器**。Windows 标准交付静态链接运行库；Linux 目标还应核对 sysroot 所用的系统运行库版本。应用自己的依赖包括：

- 程序用了 `#abi` 外部库：动态库要和产物放在同一目录（Windows 加载器按 exe 目录和 `PATH` 找库），
  相对路径按 `#abi` 声明所在的源文件目录解析；
- 交付 `--shared` 内核：连同生成的 `.h` 与运行时一起给对方，接口约定见[宿主集成](08-host-integration.md)。

其余情况只交付那一个可执行文件；`.chali/` 不用带，也不该带。

## 旧格式

1.0.8 删除了 `.svmb` 字节码容器与符号压缩。旧格式的说明、测量与逆向评估保留在 Git 历史里，
迁移要点见[原生产物符号与旧格式迁移](16-compact-symbols.md)。
