# AOT 降低开关与计数

<!-- LANGUAGE NAVIGATION -->
[English](en/40-backend-ownership.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 前端完成类型、借用和确定性清理的降低后，四元式直接交给 LLVM AOT，LLVM 负责全部优化。普通赋值的独立值语义，以及 `Copy<T>` 与 `#NonCopyable` 的互斥规则保持不变。

AOT 的 LLVM 优化始终全开：静态 LLVM 使用 `default<O3>` 与激进代码生成等级，外置 LLVM 使用 `-O3`。保持常规浮点语义，不启用 fast-math。安全检查与回退仍然生效，计数插桩默认关闭。

## 保留的 AOT 快速路径

降低层只保留 LLVM 无法替代的 ChaLi 专属快速路径：

- 聚合槽批量复制：整个源、目标范围都活跃且不重叠时批量搬运。重叠或范围证明失败时，回到原顺序的逐槽先读后写，保留越界、悬空及部分写入行为。零长度不访问内存。
- 数组复制：64 位整数与 64 位浮点的平凡元素可走槽复制助手；其他布局和超大计数保留原循环。新数组仍经原有受检分配与初始化，不引入未初始化存储语义。
- 地址验证：仅在直线路径、地址符号和精确访问宽度不变时复用验证结果。调用、分配、释放、控制流边界和内存写入使相应证明失效。
- 函数调用：对适用的极小内部 AOT 函数提供内联提示；直线路径内已知且无捕获环境的函数值可直接调用。未知目标、闭包环境、重入后失效的事实及间接调用协议保留兼容路径。

看门狗路径保守停用不能证明逻辑安全点等价的快速路径。除零、溢出和移位越界等既有运算语义不因这些快速路径而关闭。

## 对照开关

以下环境变量在编译时非空即保留对应原路径，用于同版本对照，不修改源码：

| 环境变量 | 关闭的快速路径 |
| --- | --- |
| `CHALI_NO_ARRAY_COPY_BULK` | 64 位平凡数组的槽复制路径 |
| `CHALI_NO_AOT_BULK_COPY` | AOT 聚合槽批量复制助手 |
| `CHALI_NO_AOT_ADDRESS_CACHE` | AOT 已验证地址复用 |
| `CHALI_NO_AOT_INLINE_HINT` | AOT 极小函数内联提示 |
| `CHALI_NO_AOT_DIRECT_CALLS` | 已知无环境函数值直接调用 |

专项入口：[语言用例](../example/samples/backend_ownership.chl)、[生成证据与复制边界测试](../tests/chali_backend_ownership.py)。

## 可选计数

设置 `CHALI_BACKEND_PROFILE=1` 后编译 AOT 程序，生成的程序会在所有权相关四元式位置记录调用、分配请求、移动及清理。运行时用 `CHALI_BACKEND_PROFILE_OUTPUT` 指定 JSON 输出，正常进程退出后保存所有线程的累计计数。

```powershell
$env:CHALI_BACKEND_PROFILE='1'
chali aot example/samples/backend_ownership.chl --out tmp/ownership-profile
Remove-Item Env:CHALI_BACKEND_PROFILE
$env:CHALI_BACKEND_PROFILE_OUTPUT="$PWD/tmp/ownership-profile.json"
& ./tmp/ownership-profile.exe
python tools/backend_profile.py tmp/ownership-profile.json --top 20
```

默认编译不插入计数调用。插桩产物用于定位成本，正式耗时使用另行构建的未插桩产物。设置输出环境变量不会给普通产物添加插桩。

记录包含操作、函数、源码位置、优化后的四元式序号、执行计数与已知的请求字节数。这些计数统计的是降低后仍在执行的操作，不是源语言层面的语义复制；已被消除的操作不会出现。分配字节数只覆盖四元式 `alloc` 请求，不含宿主对象内部的每次分配、堆驻留或峰值内存。递归清理与底层释放是不同层次，不得当作用户析构调用次数相加。裸字搬运与拥有字段的深复制分别计数。
