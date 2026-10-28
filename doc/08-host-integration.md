# 08 宿主集成与 C ABI

<!-- LANGUAGE NAVIGATION -->
[English](en/08-host-integration.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 普通执行从语义检查后的 AST 直接生成 LLVM IR，再经 LLVM O3 生成原生代码。和 C 打交道有两个方向——
**ChaLi 调 C 动态库**，和**C 宿主调 ChaLi 编出来的原生库**。两个方向都走平台 C ABI：中间没有字节码容器，
也没有需要宿主注册函数表的运行环境。

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

两个方向都能同步回调，规则一致：

- C 调 ChaLi：把 ChaLi 函数值当函数指针传给 `#abi` 声明的入口，C 侧直接调用；
- ChaLi 调 C：C 侧把函数指针作为参数传给 ChaLi，ChaLi 用它当普通函数值调用。

函数值里的运行时地址**不是**通用机器函数指针，只能在这次同步调用期间、同一线程使用；不要缓存下来
异步回调。回调里的 `panic` 会穿过 C 帧回到顶层，按运行期错误报告，不会变成缺省返回值。

字符串、列表、映射等拥有资源的值不在这套机制里自动转移所有权：要跨边界就显式设计接口，用标量、
引用或自己管理的缓冲区。

## 调试与边界

- 反射默认编入；`--no-reflection` 可关掉，关闭后 `@Reflect` 会报「功能未开启」。
- 调试构建用 `--debug` 保留原生符号与源码行、降低优化级别，配合 GDB 使用，见[原生调试](43-native-debugging.md)。
- `--memory-size` 决定运行时堆容量；子进程边界、线程与看门狗见[运行边界](17-capabilities.md)。
- 这里没有 VM 沙箱：`#abi` 调进来的 C 代码与宿主同权限运行，边界安全由接口设计负责。

参见[工具链](09-toolchain.md)、[AOT 编译](38-aot.md)与[同步回调示例](../example/native-callbacks/README.md)。
