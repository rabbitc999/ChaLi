# 原生 AOT 调试

<!-- LANGUAGE NAVIGATION -->
[English](en/43-native-debugging.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 1.0.8 的 `run` 使用 checked AOT 编译执行，`aot` 默认生成 unchecked 原生产物。编译器不再内置 VM 执行器；`.svmb` 导出已删除，新项目使用 AOT 原生产物。调试使用真实机器码、DWARF 源码行和帧槽，不依赖 VM 断点或解释执行状态。

## VS Code

安装 ChaLi 扩展与目标平台的 GDB，打开 `.chl` 文件，按 F5 启动原生调试。扩展先执行 checked AOT 调试构建，再通过 GDB/MI 提供源码断点、继续、单步进入／越过／跳出、调用栈、参数／局部变量和表达式查看。

```json
{
  "type": "chali",
  "request": "launch",
  "name": "ChaLi 原生调试",
  "program": "${file}",
  "cwd": "${fileDirname}",
  "debuggerPath": "gdb",
  "stopOnEntry": false,
  "args": []
}
```

`debuggerPath` 指定 GDB 路径；未设置时依次使用 `CHALI_GDB` 和 PATH 中的 `gdb`。调试器架构应与产物匹配。编译器、原生适配器和 GDB 子进程继承同一工程 `tmp/`；临时原生产物保留在该目录便于复核。断点解析失败会显示未验证及诊断，不回退到 VM。

调试构建使用 `-O0`，保留原生符号与源码行，避免最高优化把变量和步骤消除。普通发布构建继续默认最高优化、LTO 和死代码消除。调试结果不能直接作为发布构建性能数据。

## 会话控制

构建、配置、运行、暂停和结束是不同状态。断点安装与启动、继续和单步等控制请求按顺序处理；调用栈、变量和表达式只接受当前暂停状态的引用。恢复执行后，旧帧和变量引用失效；迟到的读取结果报告过期，不展示上次暂停的数据。

支持取消调试构建与查询；客户端支持进度显示时，构建进度也可取消。停止或断开会回收该会话的编译器和 GDB 进程树。重启会重新构建程序并按原始源码位置重装断点，不沿用旧 GDB 的断点编号；构建期间也可以重启。正常退出、取消与重复关闭只发送一次结束事件。

DAP 消息头最多 8 KiB，消息体和单条 MI 记录最多 16 MiB；并发 DAP／MI 请求各最多 256 项。非法长度、截断或错误 JSON 不继续分派。MI 查询默认超时 20 秒；超时和取消后移除请求，迟到响应不能满足后续请求。控制操作一旦交给目标执行，取消不代表撤销已发生的程序行为；状态无法确认时结束会话。

当前尚未提供条件／日志断点、语言容器展开和运行期错误过滤器；没有对应能力时不会宣称支持。调试专项使用真实 GDB 核对单步、返回位置、旧帧拒绝、重启断点及取消清理：

```sh
python tools/run_module_gates.py debug --chali PATH --random-recheck 1
```

## 命令行

```text
chali aot app.chl --checked --debug --out build/app-debug
gdb build/app-debug.exe
```

Linux 使用无 `.exe` 后缀的产物。可以设置 `break app.chl:行号`、`run`、`next`、`step`、`finish`、`backtrace`、`info locals` 和 `print 表达式`。`--debug` 保留调试信息，`--checked` 控制数组及指针检查，两者独立。

目前原生变量描述覆盖参数和局部标量槽；拥有型聚合和容器的槽值仍是 ChaLi 逻辑句柄，不能把它当成机器 C 指针解引用。表达式使用 GDB 接受的原生表达式语法。

源码级调试需要源码文件仍在记录的位置。调试适配器使用现有编译器与工具链生成机器码；不需要安装单独的 VM，也不把运行时函数值强转成机器函数指针。
