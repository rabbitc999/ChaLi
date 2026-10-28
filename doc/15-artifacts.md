# 构建产物与中间文件

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](15-artifacts.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 只有一条执行路径：语义检查后的 AST 直接生成 LLVM 代码，再由 LLVM AOT 生成原生可执行文件。这一页说明**文件都落在哪里、
哪些会留下、哪些用完就删**。命令行开关见[工具链](09-toolchain.md)，AOT 选项见[AOT 编译](38-aot.md)。

## 三类产物

| 命令 | 产物 | 缺省位置 |
| --- | --- | --- |
| `chali run app.chl` | 临时可执行文件 + 运行结果 | `.chali/chali_out*`，运行结束即删除 |
| `chali aot app.chl` | 原生可执行文件（Windows 为 `.exe`，Linux 无后缀） | `.chali/` 临时前缀，最终产物位于源码同级 |
| `chali aot app.chl --emit-llvm --out build/app` | `.ll` | 指定的 `build/app` 前缀 |
| `chali aot app.chl --target linux-arm64 --emit-obj --out build/app` | ARM64 ELF `.o` | 指定的 `build/app` 前缀 |

`emit` 模式已删除。`run` 与 `aot` 的中间文件默认留在 `.chali/` 下；显式 `--out` 将其保留在指定前缀。

## `.chali/` 目录

不给 `--out` 时，`run` 与 `aot` 的中间产物前缀是 `.chali/chali_out`。首次使用时自动创建；在
Windows 上该目录带隐藏属性，资源管理器默认不显示。

```text
.chali/
  chali_out.ll           LLVM IR
  chali_out.llvm.log     后端诊断
  chali_out.abi.cpp      `#abi` 桥接源码（用到外部符号时才生成）
  chali_out.h            导出头（带 `--export` 时生成，属交付物）
  chali-stdlib-cache/    标准库预编译单元缓存，跨次复用
```

**只有 `chali-stdlib-cache/` 是长期驻留的。** 其余中间文件在命令成功后删除：

- `run`：删掉 `.ll`、`.llvm.log`、`.abi.cpp`，连临时可执行文件一起删；
- `aot`：删掉同一组中间文件，保留导出的 `app.exe`；带 `--export` 时保留 `.h`。

## 什么时候不删

| 情形 | 保留原因 |
| --- | --- |
| `--keep-artifacts` | 显式要求保留，排查后端或看 IR 时用 |
| `--incremental` | 下次运行要复用缓存与产物 |
| `--emit-llvm` | 用户要的就是 `.ll` |
| `--emit-obj` | 用户要的就是目标 `.o` |
| 显式 `--out 前缀` | 用户点名了路径，编译器不动他的目录 |

增量缓存目录是 `<前缀>.chali-cache`，由 `--incremental` 生成；它按模块内容与编译开关做签名，
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
