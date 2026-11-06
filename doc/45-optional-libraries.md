# 可选宿主库

<!-- LANGUAGE NAVIGATION -->
[English](en/45-optional-libraries.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

`std::gui`、`std::audio` 和 `std::xml` 的模块声明内置于编译器，无需 `-I`。包含原生归档的编译器在链接时选取程序调用到的服务，将 C 库代码直接写入最终程序。Windows 分发只需一个 EXE，不必携带 ChaLi、raylib 或 Expat 的 DLL；图形与音频仍使用操作系统服务。应用另行读取的字体、图片或音频素材需要按应用的资源路径提供。

从源码构建 Windows x64 静态 LLVM 版本时，准备 CMake、Ninja 和 SDK，再执行：

```powershell
mingw32-make CHALI_STATIC_LLVM=1 native-extensions
mingw32-make CHALI_STATIC_LLVM=1 -j8
```

Linux 源码构建使用 `make native-extensions` 后再运行 `make`，需准备 raylib 对应的桌面开发头文件。带字符串或结构体等复杂 C ABI 的程序在编译时还需同架构 C++ 编译器，可用 `CHALI_ABI_CXX` 指定；生成的程序不需该工具链。外置 LLVM 版按配置传入 `--llvm`。

如需覆盖内置实现或测试外部库，`tools/setup_desktop_gui.py` 和 `tools/setup_xml.py` 不带 `--static` 时仍提供动态模块。显式 `-I` 指向包含 `std/gui.chl`、`std/audio.chl` 或 `std/xml.chl` 的安装根；采用此方式时，部署应保留声明指定的共享库。密码学和 HTTP 流式模块仍使用各自的安装脚本。

## 桌面窗体和音频

raylib 提供窗口、事件和绘制，raygui 提供控件。默认构建关闭音频、三维模型和内置图标。`--audio` 启用独立 `std::audio` 模块，`--icons` 加入图标。脚本校验归档摘要，`--offline` 只读已下载的有效归档。

```powershell
chali build example/desktop-gui/demo.chl --out build/gui-demo
```
运行 `gui-demo.exe` 后，`Count` 按钮累加计数，关闭窗体时输出 `clicks=N`，未点击时为 `clicks=0`。在创建窗口的线程上完成绘制与事件循环，退出时调用 `close()`。音频代码独立按需链接。

```powershell
chali run example/desktop-gui/audio.chl --out build/audio-demo -- path/to/tone.wav
```
成功播放输出 `playback finished`。没有可用设备时输出 `audio device unavailable`；无效或无法解码的文件输出 `audio file could not be loaded`。显式关闭会停止并释放仍登记的声音。


### 控件和回调

使用锁定的 raygui 4.0 全部 **34 种控件**，每种控件都有同名的小写工厂。下表列出 ChaLi 工厂与上游函数的对应关系；`controlCount()` 返回 34，`controlName(index)` 返回工厂名。

| ChaLi 工厂 | raygui 控件 |
| --- | --- |
| `windowBox` | `GuiWindowBox` |
| `groupBox` | `GuiGroupBox` |
| `line` | `GuiLine` |
| `panel` | `GuiPanel` |
| `tabBar` | `GuiTabBar` |
| `scrollPanel` | `GuiScrollPanel` |
| `label` | `GuiLabel` |
| `button` | `GuiButton` |
| `labelButton` | `GuiLabelButton` |
| `toggle` | `GuiToggle` |
| `toggleGroup` | `GuiToggleGroup` |
| `toggleSlider` | `GuiToggleSlider` |
| `checkBox` | `GuiCheckBox` |
| `comboBox` | `GuiComboBox` |
| `dropdownBox` | `GuiDropdownBox` |
| `spinner` | `GuiSpinner` |
| `valueBox` | `GuiValueBox` |
| `textBox` | `GuiTextBox` |
| `slider` | `GuiSlider` |
| `sliderBar` | `GuiSliderBar` |
| `progressBar` | `GuiProgressBar` |
| `statusBar` | `GuiStatusBar` |
| `dummyRec` | `GuiDummyRec` |
| `grid` | `GuiGrid` |
| `listView` | `GuiListView` |
| `listViewEx` | `GuiListViewEx` |
| `messageBox` | `GuiMessageBox` |
| `textInputBox` | `GuiTextInputBox` |
| `colorPicker` | `GuiColorPicker` |
| `colorPanel` | `GuiColorPanel` |
| `colorBarAlpha` | `GuiColorBarAlpha` |
| `colorBarHue` | `GuiColorBarHue` |
| `colorPickerHsv` | `GuiColorPickerHSV` |
| `colorPanelHsv` | `GuiColorPanelHSV` |

工厂接收文本并返回 `Widget`。`draw(Rect::at(x, y, width, height))` 绘制并返回事件，每帧同一模型只处理一次。`onEvent`、`onClick`、`onChange`、`onSubmit`、`onClose` 注册同步的 ChaLi 回调；回调由语言对象持有，C 库只保存文本和控件状态。`clearCallbacks()` 释放注册的捕获，`close()` 同时销毁模型并清空回调。回调可关闭控件或窗口，派发会在对象失效后停止。

`textBox`、`textInputBox` 使用 C 分配的可编辑 UTF-8 缓冲，`setTextCapacity` 可调整容量，失败保持原文本。复选/切换状态使用 `checked`，选择使用 `selected`，整数编辑使用 `number`，滑块使用 `value` 和 `setRange`，颜色使用 `color` 或 HSV。列表通过 `setItems(List<String>)` 设置最多 128 项，每项不得包含分号或换行；这些字符是 raygui 的项目分隔符。`setContent`、`viewport`、`scrollOffset` 与 `beginContent` 支持滚动面板和裁剪。

`run(draw, onEvent)` 管理帧循环和退出清理；绘制完成后轮询窗口事件，文本控件优先消耗字符，窗口 `TextEntered` 只接收剩余字符。事件包括打开、关闭、尺寸、焦点、键盘、字符、鼠标和滚轮。关闭按钮中优先调用 `requestClose()`，循环会派发一次 `Closing` 再清理窗口。直接 `close()` 用于立即释放。关闭窗口会释放全部 C 控件，旧控件 ID 在重开后仍失效；语言回调捕获的寿命由语言对象和 `clearCallbacks` 管理。

所有窗口、状态、绘制与事件操作必须在创建窗口的线程执行；其他线程的 UI 操作被拒绝。`openWindow(width, height, title, options)` 的选项位为 1 可调整大小、2 隐藏、4 垂直同步。`loadFont(path, size, glyphs)` 可加载包含所需字符的字体；默认字体不保证覆盖中文。旧版 `open`、即时 `label`、即时 `button` 接口仍可使用。

[回调示例](../example/desktop-gui/demo.chl) 展示按钮、复选框、文本框、滑块和进度条。原生宿主使用 C11 与平台 C API，不要求 C++ 运行库；音频仍按需独立构建。

### 绘图、自定义控件与命名事件

绘图在 `beginFrame()` / `endFrame()` 之间或 `run` 的绘制回调中执行，颜色使用 `0xRRGGBBAA`。`drawPixel`、`drawLine`、`drawRectangle`、`drawCircle`、`drawEllipse`、`drawTriangle`、`drawPolygon`、`drawBezier`、`drawRing` 和 `drawText` 覆盖二维基本图形；矩形支持圆角及描边，`measureText` 返回文字尺寸。`beginClip` / `endClip` 支持嵌套裁剪，`clipBounds()` 返回当前裁剪范围。

`loadTexture(path)` 读取 PNG，返回窗口拥有的 `Texture`；`draw` 和 `drawRegion` 支持目标区域、旋转及着色。纹理 `close()` 后失效，窗口关闭时释放余下纹理，旧句柄不会在新窗口复用。字体和图片的 Windows UTF-8 路径通过 C/WinAPI 文件入口读取。

`customWidget()` 不增加内置控件种类；它提供自定义绘制和输入行为。`onDraw(fn(Rect))` 使用屏幕矩形，`onEvent(fn(Event))` 与 `on(name, fn(Event))` 接收本控件事件。把窗口事件交给 `handle(event)`；指针事件位置转换为控件局部坐标，支持按下后的鼠标捕获和焦点管理。命名事件包括 `pointer.down`、`pointer.up`、`pointer.move`、`pointer.wheel`、`key.down`、`key.up`、`text.input`、`focus.gained`、`focus.lost` 以及窗口事件。回调可关闭自身控件或窗口。

`customEvent(name, text, code, value)` 创建拥有自身文本的事件。`EventBus.on`、`once`、`off`、`emit` 提供同步订阅，`post` 与 `pump` 提供有界队列。一次性监听器在调用前注销，回调中新注册的监听器延后到下一次派发；回调新增的排队事件留给下一次 `pump`。队列上限为 4096，默认单次处理 256 项，递归派发深度上限为 64，调用者应检查返回值。

参考[自定义滑块示例](../example/desktop-gui/custom.chl)与[生命游戏项目](../example/desktop-gui/life.chl)。[书稿第 30 章](book/30-标准库初探.md)提供完整模型、暂停左键切换细胞、右键拖动视口及模拟帧率控制，并附实际运行截图。

## XML

```powershell
chali run example/xml/demo.chl --out build/xml-demo
```

```text
XML OK
```
`std::xml::parse` 读取 UTF-8 文档并返回根节点。元素名和属性名保留源码中的限定拼写，文本片段和子元素按混合内容顺序读取。`escapeText` 与 `escapeAttribute` 分别用于元素文本和带引号的属性值。输入上限为 16 MiB，另有限制嵌套和中间数据。解析错误包含从 1 开始的行列；`DOCTYPE` 被拒绝，解析器不访问外部实体。相邻字符片段可能分成多项。

## 密码学

```powershell
chali run example/crypto/demo.chl --out build/crypto-demo
```

```text
crypto OK
```
`std::crypto` 的模块与原生静态归档随支持原生服务的发行版内嵌，无需 `-I` 或另行部署 DLL。`randomHex`、`sha256`、`hmacSha256`、`verifyEd25519` 分别提供安全随机、SHA-256、HMAC-SHA-256 和 Ed25519 分离签名验证。随机字节数为 1–65536；摘要和随机字节使用小写十六进制，验签接收固定长度的十六进制公钥和签名。构建 Windows x64 使用锁定的 libsodium 归档，Linux 使用系统静态开发包。完整签名见[密码学 API 参考](library/std-crypto.md)。

## HTTP 流式传输

```powershell
python tools/setup_http_transfer.py
```
导入 `std::net::http` 后，`HttpClient::open()` 创建会话，`downloadTo` 发起 GET 并写入新文件，`uploadFile` 以 PUT 流式读取源文件。同一会话重复请求时，libcurl 在协议允许的情况下复用连接。超时覆盖完整请求；同步回调收到已下载和已上传的字节数，返回非零可取消。`keepGoing` 只使用超时。HTTP 状态与传输失败分开返回，下载成功且响应为 2xx 才保留本次创建的文件。调用者使用 `close()` 关闭会话。

Windows 可运行 [GUI 与音频](../example/desktop-gui)、[XML 示例](../example/xml/demo.chl) 和 [密码学示例](../example/crypto/demo.chl)。实现使用 [raylib](https://github.com/raysan5/raylib)、[raygui](https://github.com/raysan5/raygui)、[Expat](https://github.com/libexpat/libexpat)、[libsodium](https://doc.libsodium.org/) 与 [libcurl](https://curl.se/libcurl/)。

[下载示例](../example/http-transfer/download.chl)。传入 URL 和尚不存在的目标路径。成功获取 200 响应时输出 HTTP 200，传输失败时输出具体错误。
