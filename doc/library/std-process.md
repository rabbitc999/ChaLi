# std::process — 宿主进程

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-process.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::process;
```

## 公共约定与错误边界

命令通过 C 帧入口交给宿主 shell；processRun 创建独占读取管道，processRead 每次读取最多 511 字节并移除尾部换行，processClose 关闭并等待。drop 关闭尚未关闭的管道，显式关闭后仍保留记录直至 drop。system 同步执行命令，返回宿主状态码。程序自行构造和信任 command 的内容；不将外部输入直接拼入 shell 命令。

## 参考代码

完整程序：[example/samples/std_process_probe.chl](../../example/samples/std_process_probe.chl)。

```powershell
chali run example/samples/std_process_probe.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::process;
import std::probe;

void main() {
    Process process = processRun("echo chali-process-ok");
    String output = processRead(&process);
    int64 status = processClose(&process);
    assert(status == 0);
    assert(output == "chali-process-ok");
    Probe value;
    value.value = twice(21);
    println("process={} probe={}", status, value.value);
}
```

</details>

<details>
<summary>示例输出</summary>

```text
process=0 probe=42
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [processRun](#api-0) | `Process processRun(String command)` |
| [processRead](#api-1) | `String processRead(Process const& process)` |
| [processClose](#api-2) | `int64 processClose(Process const& process)` |
| [system](#api-3) | `int64 system(String command)` |

<a id="api-0"></a>

### processRun（1）

**声明**

```chali
Process processRun(String command);
```

**行为**

通过 shell 启动读取管道，返回进程句柄。

命令管道通过 C 帧和不透明记录服务接入宿主。显式关闭返回平台退出状态； 离开作用域仍会释放记录并关闭尚未关闭的管道。每次读取最多 511 字节并移除尾部 CR／LF。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `command` | `String command` | 交给宿主 shell 的命令文本 | 无 |

**返回值**

`Process`。通过 shell 启动读取管道，返回进程句柄。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
processRun(command);
```

**源码**：[source/std/process.chl:5](../../source/std/process.chl#L5)。

**完整示例**：[example/samples/std_process_probe.chl](../../example/samples/std_process_probe.chl)。

<a id="api-1"></a>

### processRead（2）

**声明**

```chali
String processRead(Process const& process);
```

**行为**

读取进程管道的输出文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `process` | `Process const& process` | 进程管道句柄 | 无 |

**返回值**

`String`。读取进程管道的输出文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
processRead(process);
```

**源码**：[source/std/process.chl:6](../../source/std/process.chl#L6)。

**完整示例**：[example/samples/std_process_probe.chl](../../example/samples/std_process_probe.chl)。

<a id="api-2"></a>

### processClose（3）

**声明**

```chali
int64 processClose(Process const& process);
```

**行为**

关闭管道并等待进程，返回宿主状态码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `process` | `Process const& process` | 进程管道句柄 | 无 |

**返回值**

`int64`。关闭管道并等待进程，返回宿主状态码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
processClose(process);
```

**源码**：[source/std/process.chl:7](../../source/std/process.chl#L7)。

**完整示例**：[example/samples/std_process_probe.chl](../../example/samples/std_process_probe.chl)。

<a id="api-3"></a>

### system（4）

**声明**

```chali
int64 system(String command);
```

**行为**

同步执行 shell 命令并返回宿主状态码。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `command` | `String command` | 交给宿主 shell 的命令文本 | 无 |

**返回值**

`int64`。同步执行 shell 命令并返回宿主状态码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
system(command);
```

**源码**：[source/std/process.chl:8](../../source/std/process.chl#L8)。

**完整示例**：[example/samples/std_process_probe.chl](../../example/samples/std_process_probe.chl)。
