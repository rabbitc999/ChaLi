# 08 宿主集成与 C ABI

<!-- LANGUAGE NAVIGATION -->
[English](en/08-host-integration.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 普通执行从语义检查后的 AST 直接生成 LLVM IR，再经 LLVM O3 生成原生代码。和 C 打交道有两个方向——
**ChaLi 调 C 动态库**，和**C 宿主调 ChaLi 编出来的原生库**。两个方向都走平台 C ABI：中间没有字节码容器，
这两种互操作由生成代码完成封送。扩展宿主内核时，还可以使用下面的 C 函数表接口。

## C 宿主函数表

公开头为 `source/abi/chali_abi.h`，标准库宿主入口的集中注册另见 `chali_standard.h`。头文件可由 C11 编译器独立包含。运行时实现可以使用 C++，公开边界使用 C 结构、固定宽度槽、字节缓冲和不透明句柄。完整例子见 [C 宿主程序](../example/host-c-api/main.c)及其[构建说明](../example/host-c-api/README.md)。

先用 `chali_abi_registry_create` 创建拥有的函数表，再构造 `ChaliAbiModuleDescriptor`。其中的 ABI 描述由模块自己的编译器调用 `chali_abi_local_info` 生成；宿主检查版本、目标架构／平台、调用约定、布局和必需能力。Windows x86 的公开入口显式使用 C 调用约定，ARM32 描述区分普通与硬浮点 ABI。

`chali_abi_registry_register` 原子登记整模块。名称会复制，入口代码仍须存活；重名、稳定符号冲突或任何分配失败都不会留下部分入口，也不复制已有回调状态。拥有的 C 函数表使用宿主堆，不借用登记时的语言临时 arena。`chali_abi_registry_seal` 封存同一张运行时表，C 和私有实现的注册／移除路径都遵循封存状态。封存后可并发查询，入口自身的共享状态仍按其线程约定处理。

`chali_abi_registry_call` 检查实参数量、数组、类别和返回值，恢复输入帧并在失败时清空部分输出；`chali_abi_registry_resolve` 只返回直接 C 入口。实参按整数或 double 位模式编码，带状态的 `chali_abi_argument_integer`／`chali_abi_argument_real` 拒绝类别不符，入口应在读取失败后返回。纯标量例子允许 `frame.runtime` 为空；访问语言堆、记录或函数值需要运行时提供的上下文，不能自行构造不透明句柄。`#abi` 外部调用继续按其真实 C 原型封送，并不自动接收这个内部调用帧。

`chali_abi_read`／`chali_abi_write` 访问受检查的逻辑地址；它们检查范围与存活，不产生新的类型化所有权。`chali_abi_string` 创建拥有的 String，未转交语言调用方时用 `chali_abi_string_release` 成对释放描述符与缓冲区。`chali_abi_bytes` 同时识别长度头和容量描述符，借用视图只有效到下一次可能分配、扩容、释放或重入之前；需要保留时先复制。原始块由 `temporary` 创建，可用 `temporary_release` 提前释放。

`CHALI_ABI_CAP_BINARY` 表示原始字符串与字节列表服务。`chali_abi_string_bytes` 按显式长度复制内容，保留零字节及非 UTF-8 数据；旧 `chali_abi_string` 仍拒绝内嵌零字节。两种构造共用存储与释放协议。

`chali_abi_list_bytes` 以空目标、零容量查询长度，再向调用方缓冲区复制；它验证元素为 `0..255` 的整数，短缓冲或非法元素不会写入目标。`chali_abi_list_from_bytes` 创建独立拥有的 `List<uint8>`，完整构造后才发布句柄。未交还语言时，用 `chali_abi_record_release` 配对释放；交还语言拥有值后由语言清理。模块需要这些入口时，在所需能力中声明 `CHALI_ABI_CAP_BINARY`。


`chali_abi_record_make_clonable` 登记 C 复制回调，使拥有记录可参与容器深复制及线程快照。回调必须返回独立记录，不得引用源运行时拥有的句柄；失败返回空，不得抛出异常。同名记录的销毁及自动复制回调必须一致，登记后不能更换。需要此服务时声明 `CHALI_ABI_CAP_RECORD_CLONE`；旧 `chali_abi_record_make` 创建的记录仍不支持自动复制。

记录创建成功后转移原指针；返回零时调用方仍负责原指针。克隆登记失败会清理新副本，原记录保持存活。类型名称拥有独立副本，同名类型不能更换销毁协议；销毁回调不得抛异常，代码要存活到记录释放。同步函数值调用使用 `chali_abi_invoke`，保留捕获环境及检查，借用调用器而不复制其状态；函数句柄不是机器函数指针。

错误通过 `ChaliAbiError` 的状态与有界缓冲返回。服务异常不会越过 C 边界；已有运行期错误与内存故障保留优先级，模块的首条显式诊断可以补充通用服务失败。内建入口目录并发发布完整只读表，超容量或非法目录整体失败。验证入口为 `python tests/chali_c_api_contract.py` 与 `python tests/chali_c_api_registry.py`。

## 从 ChaLi 调用 C 动态库

每个函数一条 `#abi` 指令：

```chali
#abi("C", "./host.dll", "hostApply")
int64 hostApply(int64 fn(int64) function, int64 value);
```

第三个参数是库里真实导出的符号名，声明用的是 ChaLi 侧的本地名字，两者可以不同。

- **路径**按**声明所在的源文件目录**解析，导入模块里的声明同样如此；路径分隔符会归一化。
- **产物要和动态库放在同一目录**：Windows 加载器在进程启动时按 exe 目录和 `PATH` 找库。Linux 用
  `host.so`，其余照旧。
- **类型映射**：标量按值；`T mut&` / `T const&` 对应 C 的 `T *` / `const T *`；`String` 对应
  `const char *`；结构体按值且默认 C 布局，`packed` 对接 `#pragma pack`；数组用 `T[]` 且长度另给；
  `Option<T &>` 是唯一能把空指针送过边界的通道。
- **函数值可以过边界**：`int64 fn(int64)` 传到 C 侧就是 `int64_t (*)(int64_t)`，C 可以直接调用它，
  捕获环境由 ChaLi 运行时带着走。完整例子见[同步回调示例](../example/native-callbacks/README.md)。

C 侧看到的只有签名，所以声明必须和真实的 C 函数逐项对上：返回类型、参数宽度、调用约定、结构体
字段顺序。

## C 宿主调用 ChaLi

用 `--shared --export` 把选定的函数导出成动态库，并生成配套头文件：

```sh
chali aot app.chl --out build/app --shared --export add
```

`build/app.h` 里给出 C 声明：

```c
void chali_init(void);
void chali_finish(void);
int64_t f_add(int64_t p0, int64_t p1);
```

约定很短，但顺序不能错：

1. `chali_init()` 分配运行时内存、装入数据区、执行全局初始化；
2. 调 `f_名字(...)`；
3. `chali_finish()` 做全局清理、刷新输出、释放运行时内存。

`chali_finish()` 之后原有句柄一律失效。导出函数只接收和返回 C 标量；需要传结构体时按值传，
需要原地修改时用 `T *` 对应的引用形参。不给 `--export` 时只有 `chali_init` / `chali_finish`。

`--shared` 的产物是动态库，宿主负责加载和生命周期；普通的 `chali aot app.chl` 则是自包含的可执行
文件，属于交付给最终用户的形态，见[构建产物](15-artifacts.md)。

## 同步回调

`#abi` 的函数值参数由适配器连接到同步 C 跳板，C 库调用的是与其真实原型匹配的机器回调；适配器保留
ChaLi 函数值的捕获环境和错误通道。C API 模块调用运行时函数值时使用 `chali_abi_invoke`，显式提供槽类别并
检查返回类别。运行时函数句柄**不是**通用机器函数指针，不得通过强转跳过适配器。

回调只在约定的这次同步调用期间、同一线程使用，不缓存为异步入口。回调发生 `panic` 时，受检查跳板记录
失败，C 函数返回后先执行约定的返回缓冲区释放，再传播原始运行期错误；部分返回值不会成为成功结果。

字符串、列表、映射等拥有资源的值不在这套机制里自动转移所有权：要跨边界就显式设计接口，用标量、
引用或自己管理的缓冲区。

## 调试与边界

- 反射默认编入；`--no-reflection` 可关掉，关闭后 `@Reflect` 会报「功能未开启」。
- 调试构建用 `--debug` 保留原生符号与源码行、降低优化级别，配合 GDB 使用，见[原生调试](43-native-debugging.md)。
- `--memory-size` 决定运行时堆容量；子进程边界与线程见[运行边界](17-capabilities.md)。
- 这里没有 VM 沙箱：`#abi` 调进来的 C 代码与宿主同权限运行，边界安全由接口设计负责。

参见[工具链](09-toolchain.md)、[AOT 编译](38-aot.md)与[同步回调示例](../example/native-callbacks/README.md)。
