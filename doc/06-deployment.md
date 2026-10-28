# 06 原生部署

<!-- LANGUAGE NAVIGATION -->
[English](en/06-deployment.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 1.0.8 起只有一条执行路径：前端把源码降成四元式，LLVM 做 AOT，出的是目标平台的原生机器码。没有共享 VM、没有 `.svmb`，所以「部署」就是把编译产物搬到目标机器并补齐它依赖的系统库。

## 源码直接运行

```sh
chali run app.chl
```

`run` 走 checked AOT：`default<O3>` 编译后在进程内直接执行，运行期检查（边界、空值、类型）保持开启。适合开发与验收，不是交付形态。

## 交付可执行文件

```sh
chali aot app.chl --out build/app
```

`aot` 是 unchecked AOT：同样是 `default<O3>`，但去掉运行期检查，产物是一个自包含的可执行文件（Windows 带 `.exe`）。把它和程序用到的动态库、数据文件一起拷到目标机器即可。`--debug` 切换成 `default<O0>,globaldce` 并保留调试信息，用于定位问题。

启动时不需要宿主注册胶水、设置数据区或步数上限——这些是 VM 时代的接口。程序要调外部 C 函数时用 `#abi("C", "库路径", "符号")` 声明，路径按声明所在源文件目录解析，动态库要和产物放在同一目录。

## 嵌入既有宿主

需要被 C/C++ 程序调用时导出动态库与头文件：

```sh
chali aot lib.chl --shared --export add --out build/lib
```

宿主按 `chali_init()` → `f_add(...)` → `chali_finish()` 三段顺序调用；异常以 `panic` 穿 C 帧回到顶层。约定与示例见[宿主集成与 C ABI](08-host-integration.md)。

## 只出中间产物

```sh
chali aot app.chl --emit-llvm --out build/app
```

只写 `.quad` / `.data.bin` / `.layout`，不链接也不执行，用于检查四元式或自行装配。`.quad` 不是机器码。

## 资源与边界

编译器内嵌标准库；实际用到的操作系统接口、动态库、网络、数据库和外设仍由目标环境提供。默认构建会清理 `.chali/` 下的中间文件，只有显式 `--keep-artifacts`、`--incremental`、`--emit-llvm` 或显式指定 `--out` 时才保留，详见[构建产物与中间文件](15-artifacts.md)。

嵌入式 Linux 用同一套后端，但产物要按目标架构重新编译：LLVM 目标三元组、C++17 运行时和系统服务都要逐项核对。「能在嵌入式 Linux 上跑」不等于「一次构建、到处交付」。

程序更新是整包替换可执行文件，不涉及字节码版本兼容；运行中的栈、闭包与全局状态不会跨版本迁移。

详细接口见[宿主集成](08-host-integration.md)、[工具链](09-toolchain.md)和[产物布局](15-artifacts.md)。
