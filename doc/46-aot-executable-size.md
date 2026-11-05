# AOT 可执行文件体积与抽样验证

<!-- LANGUAGE NAVIGATION -->
[English](en/46-aot-executable-size.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

## 测量配置

2026-11-05 在 Windows x64 上使用 ChaLi 1.0.0 静态内置 LLVM 编译器测量。原始发行版对应提交 `76c0c5b`，首次裁剪从提交 `3d9fb60` 重建；后续 C 服务重构继续保留 1.0.0 版本，使用仓库锁定的 LLVM 23.1.4 与 GCC 16.2.0。所有大小均为实际 EXE 文件字节数。

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

## 首次裁剪的历史测量

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

## C 服务阶段的后续测量（版本仍为 1.0.0）

下表复测 15 个程序，共 45 次 AOT 编译，每个产物均核对标准输出、退出码和标准错误。上表保留首次裁剪时的实际提交和配置；下表包含 C 数值与集合服务重构后的结果。

| 程序 | 覆盖内容 | 关闭 LTO | Full LTO | ThinLTO |
| --- | --- | ---: | ---: | ---: |
| `tt.chl` | Hello World | 10,240 | 10,240 | 10,240 |
| `array2d` | 二维数组 | 956,416 | 947,712 | 947,712 |
| `dynamic_array` | 动态数组 | 1,509,376 | 1,499,136 | 1,499,648 |
| `strings` | 字符串 | 1,015,808 | 1,006,080 | 1,006,080 |
| `generic_inference` | 泛型推导 | 1,033,728 | 1,024,000 | 1,024,000 |
| `interface` | 接口 | 1,041,408 | 1,032,192 | 1,032,192 |
| `drop_destructor` | 析构 | 951,296 | 942,080 | 942,080 |
| `recursion` | 递归 | 941,568 | 932,352 | 932,352 |
| `try_custom` | 错误传播 | 1,036,800 | 1,027,584 | 1,027,584 |
| `value_text` | 动态值文本 | 1,572,864 | 1,561,088 | 1,561,088 |
| `float` | 浮点 | 947,712 | 938,496 | 938,496 |
| `thread_channel` | 线程通道 | 1,237,504 | 1,228,288 | 1,228,288 |
| `std_math_clock_random` | 数学、时钟和随机 | 1,088,000 | 1,078,784 | 1,078,784 |
| `std_file_strings` | 文件与字符串 | 1,821,696 | 1,809,920 | 1,809,920 |
| `std_structured_data` | JSON/TOML 与结构化数据 | 2,375,680 | 2,364,416 | 2,364,928 |

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

`std::gui`、`std::audio` 和 `std::xml` 的内置声明不引入原生依赖。可达调用决定所需归档，LLVM 在发出代码前消除不可达定义，LLD 再按符号抽取静态库成员并回收未引用的节。生成的 Windows EXE 直接包含用到的 C 实现，运行时无需解包或查找扩展 DLL。复测入口见 `python tests/chali_optional_namespaces.py --embedded --chali path/to/chali.exe`；[构建与分发说明](45-optional-libraries.md)列出编译工具和平台依赖。

Windows x64 实测仅复制 EXE 到新目录运行。只导入三个模块、没有可达的原生调用时，三种 LTO 模式仍是 10,240 字节。实际调用 GUI、音频与 XML 的组合样例依次为 3,801,600, 3,790,336, 3,790,336 字节，对应关闭 LTO、Full LTO、ThinLTO。

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

## 按需 C 服务与标准库命名

内置模块统一声明为 `std::xxx`，内嵌资产生成时核对声明与文件名一致。GUI、音频、XML 的内置名称分别为 `std::gui`、`std::audio`、`std::xml`。示例、书稿和回归使用相同命名；显式指定外部模块目录时仍可运行动态库兼容测试。

数值 List 的稳定头部仍为数据地址、有效长度与容量，LLVM 可以直接访问；底层存储改由 C 缓冲区管理。通用 List 槽、Map/Set 密集键值、键的聚合字段及语言层哈希槽由 C 对象序列管理，C++ 薄层只提供原有对象的构造、移动和销毁回调。宿主哈希桶另外使用 C 开放寻址索引；分配器身份随存储保存，扩容先成功分配再移动，失败保持已有内容。

套接字平台入口、初始化、地址解析、阻塞切换、端口查询和就绪等待放在独立 C 服务中。Windows 动态加载 Winsock，等待集合使用栈存储；Linux 使用 libc 套接字及 `poll`，等待不受 `FD_SETSIZE` 限制。这些服务随宿主归档参与节级裁剪，未引用的服务不会因源码存在就进入应用。

这部分改动保留语言记录、嵌套值所有权和错误边界的 C++ 适配。CAS 的符号驻留、结构相等、哈希与有理数排序同样使用 C 服务。高层符号规则、表达式 API 和正则等保留 C++ 实现，不能据此把完整运行时称为纯 C。构建与验证按实际引用范围区分最小程序和使用复杂服务的程序。

```powershell
python tests/chali_optional_namespaces.py --chali path/to/chali.exe --desktop build/desktop_audio --xml build/xml
python tests/chali_c_runtime_storage.py
python tests/chali_numeric_list_storage.py --build-root build/llvm-static-gcc16
python tools/run_module_gates.py collections net file --chali path/to/chali.exe --build-root build/llvm-static-gcc16
```

## C 大整数乘法的分层阈值

本轮乘法内核使用 C11，保留 1.0.0 版本号。参考 OpenJDK BigInteger 的分层思路，独立实现学校乘法、专用平方和 Toom-Cook 3，衔接已有 Karatsuba 与双素数 CRT+NTT。没有复制 Java 源码，也不要求 Java 或 C++ 数值运行库；C++ 兼容层负责原有接口、所有权与异常转换。除法仍使用原有 Knuth / Burnikel-Ziegler。

阈值单位均为 **32 位分块数**，不是十进制位数。它们限定候选区间，实际分派继续比较成本。

| 参数 | 原生 128 位乘积路径 | portable 路径 |
| --- | ---: | ---: |
| 学校乘法直接路径，较短操作数上限 | 64 | 128 |
| 学校乘法成本比较，较短操作数候选上限 | 512 | 4096 |
| 专用学校平方候选上限 | 256 | 256 |
| Toom-Cook 3 普通乘法候选下限，较长操作数 | 160 | 160 |
| Toom-Cook 3 平方候选下限 | 256 | 256 |
| CRT+NTT 候选下限，两侧长度之和 | 6144 | 6144 |
| NTT 短侧保护上限 | 96 | 96 |

单分块乘法继续使用原快速路径。Toom 按实际切片比较、不设置固定长短比上限；学校算法、Karatsuba 与 Toom 以预计工作量比较，学校算法保留约一成、Toom 保留 6.25% 的选择裕量，避免收益很小的边界切换。NTT 根据实际补齐变换长度 `t × log2(t)` 比较成本，平方单独计价。所有估计使用整数运算，不引入浮点幂或额外宿主服务。

不能用一个永久切换点描述 NTT：等长操作数从 7680 增至 7681 个分块，15 位数字卷积的补齐变换长度由 32768 翻倍至 65536。此时 Toom 可能重新更快；代码允许按形状退回第一阶段。学校算法也按较短操作数筛选，避免长短悬殊时遗漏更便宜的逐行乘法。两个 NTT 素数均小于 2^31，蝶形加减使用等价无符号掩码，避免 GCC 11 在此处生成数据相关跳转。

正确性测试覆盖 Windows x64、Linux x64 的原生与 portable 路径，各通过 3978 次 Python 完整乘积比较和 1402 次别名/平方检查，合计 15912 次乘积与 5608 次别名/平方检查。范围含零、正负数、全一进位、非平衡长度、二路/三路切分边界、NTT 补齐边界，以及最大 32768 分块的操作数。各算法可在测试构建中单独强制，递归层采用实际生产分派策略；生产构建不含测试入口。

Toom 的普通乘法、平方及覆盖输入操作另在两平台逐点验证分配失败与所有者回滚；同轮 C 大浮点 29 项操作在 Windows/Linux 各通过 211570 个分配失败点。

可复现入口如下，性能结果的原始 CSV 与 JSON 只保留在本地，不提交 Git：

```powershell
python tests/chali_bigint_multiplication.py
python tests/chali_bigint_multiplication.py --portable
python tests/chali_bignum_c_ownership.py --kind integer
python tests/bigint_multiplication_benchmark.py --build --binary tmp/bigint-bench.exe --phase audit --default-policy --paired --samples 11 --target-us 10000 --output tmp/bigint-audit.json
python tests/bigint_multiplication_benchmark.py --build --portable --binary tmp/bigint-bench-portable.exe --phase audit --default-policy --paired --samples 11 --target-us 10000 --output tmp/bigint-audit-portable.json
```

Linux 使用相同入口，输出可执行文件可去掉 `.exe` 后缀；地址与未定义行为检查使用 `python3 tests/chali_bignum_c_ownership.py --kind integer --sanitize`。基准记录编译器版本、编译参数、二进制及源码摘要，同一输入逐轮打乱算法顺序，报告中位数、p10/p90、分配次数与工作区峰值，每个样本核对完整乘积和活动分配归零。只固定基准线程亲和性，不修改系统电源设置或其他进程。

### 阈值测量配置与结果

在同一台 Ryzen 5 5600X 上，以 Windows GCC 16.2.0、Ubuntu 22.04 / WSL GCC 11 测量原生和 portable 乘积。portable 表示在 x64 上关闭原生 128 位乘积，不代表 32 位系统验收。发布宿主实际采用 `-O2`，最终测量使用相同级别、默认 C 分配器；另保留 `-O3` 的校准和留出种子对照。

每组输入逐轮打乱算法顺序，取 11 轮配对耗时比的中位数；候选阈值的加密比较另用 15 轮。表中比例为自动路径相对本轮各强制候选的最大中位耗时比，1.10 表示多耗时 10%。阶段有重叠输入，不将组数相加视为独立样本。原始 p10/p90、CSV、编译参数及摘要保留在本地。

| 系统 | 乘积路径 | 阶段 | 输入形状数 | 比例≤1.10 | 比例≤1.20 | 最大比例 |
| --- | --- | --- | ---: | ---: | ---: | ---: |
| windows | native | boundary | 58 | 57 | 58 | 1.103 |
| windows | native | ntt | 40 | 40 | 40 | 1.024 |
| windows | native | skew | 77 | 77 | 77 | 1.078 |
| windows | portable | boundary | 58 | 58 | 58 | 1.099 |
| windows | portable | ntt | 40 | 39 | 40 | 1.145 |
| windows | portable | skew | 77 | 76 | 77 | 1.122 |
| linux | native | boundary | 58 | 51 | 56 | 1.256 |
| linux | native | ntt | 40 | 40 | 40 | 1.046 |
| linux | native | skew | 77 | 77 | 77 | 1.079 |
| linux | portable | boundary | 58 | 52 | 57 | 1.311 |
| linux | portable | ntt | 40 | 39 | 40 | 1.117 |
| linux | portable | skew | 77 | 76 | 77 | 1.136 |

初轮发现 portable 的 4096×1024 分块输入比强制 Toom 慢约 59%～94%。最终取消固定比例限制，分别估计三次求值及两个端点的实际切片成本，空段费用为零，短最高段按实际长度计价。Windows `-O2` 同轮结果为自动 Toom 约 1.242 ms、强制 Karatsuba 约 2.003 ms。

这些参数是本次硬件、编译器和分配策略下的合理默认值。Linux/WSL 有可见波动；极端不平衡输入仍可能受益于专门分块策略，表中保留实际偏离点。完整数值验证另覆盖 120000 位十进制尾数的 C 大浮点乘法；Linux 原生/portable 的 Toom 所有权检查通过 ASan/UBSan。
