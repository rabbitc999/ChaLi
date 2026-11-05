# AOT 可执行文件体积与抽样验证

<!-- LANGUAGE NAVIGATION -->
[English](en/46-aot-executable-size.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

## 测量配置

2026-11-05 在 Windows x64 上使用 ChaLi 1.0.0 静态内置 LLVM 编译器测量。原始发行版对应提交 `76c0c5b`，新编译器从包含体积裁剪改动的提交 `3d9fb60` 重建，使用仓库锁定的 LLVM 23.1.4 与 GCC 16.2.0。所有大小均为实际 EXE 文件字节数。

新编译器在正常 `chali aot` 入口完成编译、优化和链接。可执行文件在 Windows 本机运行，使用系统 C 运行库。

## Hello World 基线

参考程序如下。

```chali
void main()
{
    println("Hello World");
}
```

原始发行版再次编译得到 4,938,752 字节。新编译器在关闭 LTO、Full LTO 和 ThinLTO 三种模式下均得到 10,240 字节，即 10 KiB。缩减比例按 `(4,938,752 - 10,240) / 4,938,752` 计算，约为 99.79%。

三种新产物分别以无参数、`--help` 和两个普通参数运行，均输出 `Hello World` 与换行，退出码为零，标准错误为空。

## 其他程序的三种 LTO 模式

表中包含 9 个程序、27 次 AOT 编译。其余 8 个程序直接使用仓库中的样例源码，每个产物均与 `example/expected/<样例名>.txt` 对照标准输出，并检查退出码为零和标准错误为空。

| 程序 | 覆盖内容 | 关闭 LTO | Full LTO | ThinLTO |
| --- | --- | ---: | ---: | ---: |
| `tt.chl` | Hello World | 10,240 | 10,240 | 10,240 |
| `example/samples/array2d.chl` | 二维数组 | 2,148,864 | 2,139,136 | 2,139,136 |
| `example/samples/dynamic_array.chl` | 动态数组 | 2,701,312 | 2,690,560 | 2,691,072 |
| `example/samples/strings.chl` | 字符串 | 2,208,256 | 2,198,016 | 2,198,016 |
| `example/samples/generic_inference.chl` | 泛型推导 | 2,225,664 | 2,216,448 | 2,216,448 |
| `example/samples/interface.chl` | 接口 | 2,233,344 | 2,224,128 | 2,224,128 |
| `example/samples/drop_destructor.chl` | 析构 | 2,143,232 | 2,134,016 | 2,134,016 |
| `example/samples/recursion.chl` | 递归 | 2,134,016 | 2,124,800 | 2,124,800 |
| `example/samples/try_custom.chl` | 自定义错误传播 | 2,229,248 | 2,219,008 | 2,219,008 |

10 KiB 和 99.79% 的结果适用于本次 Hello World 基准。表中的复杂程序需要语言宿主服务，其产物大小按各自的实际依赖确定。

## 运行时与失败路径抽样

新编译器另执行了以下 12 个样例的 checked `run` 与 AOT 对照，全部编译成功，产物退出码为零、标准输出一致、AOT 标准错误为空。

- 闭包与捕获模式包括 `lambda_callbacks`、`lambda_capture_modes`。
- 错误、结构化数据、反射序列化、二进制与 UTF-8 包括 `std_errors`、`std_structured_data`、`reflect_serialization`、`std_binary`、`std_utf8`。
- 集合算法包括 `std_collections_algorithms`。
- 线程消息、通道、线程池和定时事件包括 `thread_messages`、`thread_channel`、`thread_pool`、`event_timer`。

运行时回归还核对了整数除零、整数除法溢出、越界移位和浮点除零。checked `run` 与 AOT 均在预期位置失败，保留失败前的输出并给出对应诊断。带有 1,000 步限制的无限循环也按预期停止，两条路径的退出码、输出和诊断一致。

```powershell
python tests/chali_aot_runtime.py --chali path/to/chali.exe --samples lambda_callbacks lambda_capture_modes std_errors std_structured_data reflect_serialization std_binary std_utf8 std_collections_algorithms thread_messages thread_channel thread_pool event_timer
```

这些结果覆盖本页列出的 Windows x64 样例和失败路径。

## 裁剪条件

Windows 静态 LLVM 的普通 EXE 构建会复制 LLVM 模块，试着去掉宿主生命周期调用，再由 LLVM 消除不可达定义。只有剩余的外部引用全部属于允许的 C 标准输出接口或 LLVM 内建函数时，才采用轻量模块并省去通用宿主归档。

需要对象、动态存储、原生胶水或线程服务的模块会保留宿主引用，继续使用完整运行时。显式导出、外部链接对象或库、共享库、调试构建及仅导出对象或汇编的请求沿用对应的完整构建流程。当前轻量路径的实现范围为 Windows 静态内置 LLVM 编译器。

## 复现命令

在仓库根目录运行，`chali.exe` 指向本次重建的静态 LLVM 编译器。

```powershell
chali.exe aot tt.chl --lto-mode off --out tmp/tt-off
chali.exe aot tt.chl --lto-mode full --out tmp/tt-full
chali.exe aot tt.chl --lto-mode thin --out tmp/tt-thin
Get-Item tmp/tt-off.exe,tmp/tt-full.exe,tmp/tt-thin.exe |
    Select-Object Name,Length
chali.exe aot example/samples/dynamic_array.chl --lto-mode full --out tmp/dynamic-array
./tmp/dynamic-array.exe
```

`tests/chali_aot_static.py` 持续核对三种 LTO 模式下基础输出程序的运行结果与体积上限，并覆盖容器计算和 C ABI 共享库。运行时样例的 checked `run` 与 AOT 对照入口为 `tests/chali_aot_runtime.py`。
