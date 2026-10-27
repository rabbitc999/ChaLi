# 原生 AOT 调试

[English](en/46-native-debugging.md) · **简体中文**

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

## 命令行

```text
chali aot app.chl --checked --debug --out build/app-debug
gdb build/app-debug.exe
```

Linux 使用无 `.exe` 后缀的产物。可以设置 `break app.chl:行号`、`run`、`next`、`step`、`finish`、`backtrace`、`info locals` 和 `print 表达式`。`--debug` 保留调试信息，`--checked` 控制数组及指针检查，两者独立。

目前原生变量描述覆盖参数和局部标量槽；拥有型聚合和容器的槽值仍是 ChaLi 逻辑句柄，不能把它当成机器 C 指针解引用。表达式使用 GDB 接受的原生表达式语法。

源码级调试需要源码文件仍在记录的位置。调试适配器使用现有编译器与工具链生成机器码；不需要安装单独的 VM，也不把运行时函数值强转成机器函数指针。
