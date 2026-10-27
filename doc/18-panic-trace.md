# panic、源码行号与调用栈

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](18-panic-trace.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

先运行 `samples-error/assert_failure.chl` 看一次预期失败，再比较默认构建与 `--debug` 构建的诊断差异。错误样例应以失败退出，它不是正常程序回归里的成功用例。

## 一次最小失败

[`assert_failure.chl`](../example/samples-error/assert_failure.chl) 的完整输入为：

```mc
void main() {
    assert(2 < 1);
}
```

从仓库根目录执行，命令预期返回非零：

```sh
build/work/chali/chali run chali/example/samples-error/assert_failure.chl --debug --out build/doc-example/assert-failure
```

[正式错误期望](../example/expected-error/assert_failure.txt) 的消息为 `chali aot: panic：assert(x) 的 x 为假`。`--debug` 还会输出执行轨迹与失败位置，所以不能把完整 stderr 与这一行做严格相等比较；专项测试分别检查消息、位置与帧顺序。

## 消息、出错位置、调用栈是三件事

一次 panic 的诊断由三部分组成，来源各不相同：

| 部分 | 来源 | 缺什么时候没有 |
| --- | --- | --- |
| `chali aot: panic：<消息>` | 用户 `std::lang::panic` 的文本或运行期检查生成的文本 | 永远有 |
| `出错位置：文件:行（函数）` | 前端写进 `.quad` 的 `# chali-source` 元数据，随产物一起编入 | 四元式没有来源元数据时退化为四元式行号 |
| `调用栈（内 → 外）` | 运行期帧采集，受 `STACKVM_WITH_PANIC_TRACE` 控制 | 关闭该宏时只剩前两行 |

一次真实的默认构建输出：

```
chali aot: panic：nested failure
出错位置：nested.chl:1（program::inner）
调用栈（内 → 外）：
  program::inner
```

同一个程序加 `--debug` 重新导出，调用栈补全为 `program::inner` / `program::outer` / `main`：

```
chali aot: panic：nested failure
出错位置：nested.chl:1（program::inner）
调用栈（内 → 外）：
  program::inner
  program::outer
  main
```

差别不在「有没有栈」，而在内联。默认构建 `default<O3>` 会把 `outer` 直接内联进 `main`，那一刻机器上确实只有 `inner` 和 `main` 两个栈帧，帧采集如实反映硬件现实；`--debug` 走 `default<O0>,globaldce`，函数边界都在，三层帧就都能看到。**不要把默认构建的短栈当成丢帧 bug**。

用户主动写进错误消息的文本始终原样保留，不受构建模式影响。

## 运行期帧采集

`STACKVM_WITH_PANIC_TRACE` 是编译运行期库时的宏，默认 `1`：

| 配置 | 消息 | 位置 | 自动调用栈 |
| --- | --- | --- | --- |
| 默认（`STACKVM_WITH_PANIC_TRACE=1`） | 保留 | 保留 | 按真实机器帧记录 |
| `-DSTACKVM_WITH_PANIC_TRACE=0` 或 `-DNO_CHALI_PANIC_TRACE` | 保留 | 保留 | 关闭采集 |

```sh
# 关掉帧采集重新构建编译器（运行期随编译器一起编进去）
make CXXFLAGS="-DNO_CHALI_PANIC_TRACE"
```

宏、`include`、`import`、嵌套调用和多线程终止不靠这个最小程序推断，分别由 `tests/test_chali_source_lines.py` 与 `tests/test_chali_panic.py` 的专用输入检查。

## panic 与线程

任一活动线程 panic，整个进程以失败退出：主线程、忙循环的兄弟线程，以及阻塞在 `std::thread::receive`、`std::thread::wait`、`std::thread::state` 轮询上的线程都会停止，等待者被唤醒。程序退出时自动回收未显式等待的线程。

诊断里报告的是**首个** panic 的线程栈。帧采集关闭不影响终止语义。

## 编译期行号

前端记录语法节点起始行，降级与发射期间把原始位置写成 `.quad` 的 `# chali-source: 行号 "文件"` 元数据。AOT 后端把它编进运行期位置表，因此 panic 直接显示 `.chl:行号`；没有来源元数据的旧四元式退化为 `.quad` 行号。

`import` 引入的模块使用各自文件中的原始行号。宏正文指向定义文件的正文行，并显示 `[宏 名称]`；嵌套宏指向实际执行的内层宏。退出展开后恢复调用文件行号，`$line` / `$file` 等宏元变量在调用处的语义保持不变。

需要按机器指令定位、看变量或单步时，用 `--debug` 加 GDB，见[原生 AOT 调试](43-native-debugging.md)。

## 回归

| 用例 | 自动验证 |
| --- | --- |
| 嵌套函数 panic（默认构建） | 首行 `chali aot: panic：`，含 `调用栈` 与 `inner` |
| 嵌套函数 panic（`--debug`） | 三层帧按内到外排列 |
| 多线程全局停止 | 主线程 panic、忙循环兄弟、消息等待、显式等待及自动回收均停止 |
| 行级来源 | 宏、`import`、嵌套调用的行号与文件正确 |

```sh
python tests/test_chali_panic.py --chali chali-1.0.8-llvm-external-windows-x64.exe
python tests/test_chali_source_lines.py --chali chali-1.0.8-llvm-external-windows-x64.exe
```
