# std::audio — 音频播放

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-audio.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::audio;
```

## 公共约定与错误边界

先 open 初始化音频设备，再 load 加载声音并用返回的正整数句柄播放。无效句柄和设备不可用通过 false 或零报告。unload 释放单项声音，close 释放所有声音及设备；不要继续使用已释放的句柄。需要可用的音频设备和有效输入文件。

## 参考代码

完整程序：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

```powershell
chali run example/desktop-gui/audio.chl
```

交互示例需要图形会话或音频设备；音频示例的输入文件及参数见程序入口。

<details>
<summary>展开完整参考代码</summary>

```chali
import std::audio;
import std::thread;
void main(List<String> args) {
    if (args.len() != 1) { println("pass one audio file path"); return; }
    if (!open()) { println("audio device unavailable"); return; }
    int64 sound = load(args[0]);
    if (sound == 0) { close(); println("audio file could not be loaded"); return; }
    assert(play(sound));
    while (isPlaying(sound)) { sleepFor(10); }
    assert(unload(sound));
    close();
    println("playback finished");
}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [open](#api-0) | `bool open()` |
| [load](#api-1) | `int64 load(String path)` |
| [play](#api-2) | `bool play(int64 id)` |
| [stop](#api-3) | `bool stop(int64 id)` |
| [isPlaying](#api-4) | `bool isPlaying(int64 id)` |
| [unload](#api-5) | `bool unload(int64 id)` |
| [close](#api-6) | `void close()` |

<a id="api-0"></a>

### open（1）

**声明**

```chali
bool open();
```

**行为**

初始化音频设备，成功返回 true。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。初始化音频设备，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
open();
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:4](../../library/desktop_gui/desktop_audio.chl.in#L4)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-1"></a>

### load（2）

**声明**

```chali
int64 load(String path);
```

**行为**

加载声音文件，成功返回正整数句柄，失败返回零。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 输入文件路径。 | 无 |

**返回值**

`int64`。加载声音文件，成功返回正整数句柄，失败返回零。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
load(path);
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:6](../../library/desktop_gui/desktop_audio.chl.in#L6)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-2"></a>

### play（3）

**声明**

```chali
bool play(int64 id);
```

**行为**

播放指定声音，成功返回 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。播放指定声音，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
play(id);
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:8](../../library/desktop_gui/desktop_audio.chl.in#L8)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-3"></a>

### stop（4）

**声明**

```chali
bool stop(int64 id);
```

**行为**

停止指定声音播放，成功返回 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。停止指定声音播放，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
stop(id);
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:10](../../library/desktop_gui/desktop_audio.chl.in#L10)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-4"></a>

### isPlaying（5）

**声明**

```chali
bool isPlaying(int64 id);
```

**行为**

查询指定声音是否正在播放。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。查询指定声音是否正在播放。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
isPlaying(id);
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:12](../../library/desktop_gui/desktop_audio.chl.in#L12)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-5"></a>

### unload（6）

**声明**

```chali
bool unload(int64 id);
```

**行为**

停止并卸载指定声音，成功返回 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。停止并卸载指定声音，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
unload(id);
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:14](../../library/desktop_gui/desktop_audio.chl.in#L14)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。

<a id="api-6"></a>

### close（7）

**声明**

```chali
void close();
```

**行为**

卸载全部声音并关闭音频设备。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
close();
```

**源码**：[library/desktop_gui/desktop_audio.chl.in:16](../../library/desktop_gui/desktop_audio.chl.in#L16)。

**完整示例**：[example/desktop-gui/audio.chl](../../example/desktop-gui/audio.chl)。
