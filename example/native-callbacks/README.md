# AOT 同步回调：C 调用 ChaLi 函数

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 侧把具名函数、内联 `fn` 或带捕获的闭包作为参数传给 C，C 侧按普通函数指针直接调用。
[`demo.chl`](demo.chl) 通过 `#abi` 指令声明 [`host.c`](host.c) 里的入口，覆盖具名函数、
内联闭包、copy/move 捕获、`float32`、`bool`、`void`、`int64 mut&`、ChaLi 侧高阶函数以及
ChaLi → C → ChaLi 多层重入，并用断言检查结果。

```sh
gcc -shared -O2 -o example/native-callbacks/host.dll example/native-callbacks/host.c
./chali aot example/native-callbacks/demo.chl --out example/native-callbacks/demo
./example/native-callbacks/demo.exe
```

输出：

```text
callbacks: named inline copy move float32 bool void reference higher-order reentry OK
```

`#abi("C", "./host.dll", "hostApply")` 的路径按**源文件所在目录**解析，产物也要和动态库放在同一
目录：Windows 加载器在进程启动时按 exe 目录和 `PATH` 找库。Linux 上把库名换成 `host.so`、
去掉 exe 后缀即可：

```sh
gcc -shared -fPIC -O2 -o example/native-callbacks/host.so example/native-callbacks/host.c
```

## 边界规则

- **参数只传标量、引用和函数指针**。C 侧写 `int64_t (*)(int64_t)`，ChaLi 侧写
  `int64 fn(int64)`；引用用 `T mut&` 对应 C 的 `T *`，调用处写 `&value`。
- **闭包可以带捕获**，但捕获环境由 ChaLi 运行时随函数值携带，C 侧看不到也不需要解释它。
  只能在这次同步调用期间使用，不能存下来异步回调。
- **函数值不是通用机器指针**：只有 `#abi` 声明里按上述签名出现时才可跨边界；调用失败、
  参数不符或 ChaLi 侧 `panic` 都按运行期错误传播，`panic` 会穿过 C 帧回到顶层。
- 字符串、列表、聚合等拥有资源的值不在此机制内自动转移所有权，需要显式设计接口。

回归命令 `python tests/chali_native_callbacks.py` 构建同一个 C 宿主，逐项验证这些组合，
并额外检查回调内 `panic` 的退出码与诊断文本。
