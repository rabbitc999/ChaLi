# 可选宿主库

<!-- LANGUAGE NAVIGATION -->
[English](en/45-optional-libraries.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

这些模块按需构建。先准备 CMake、Ninja 和 C/C++ 工具链，再运行对应脚本。Windows 上将示例产物与所用 DLL 放在同一目录。Linux 按相同模块目录部署对应的共享库。使用外置 LLVM 时按工具链配置传入 `--llvm`。各构建目录保留实际使用的上游许可证。

## 桌面窗体和音频

raylib 提供窗口、事件和绘制，raygui 提供控件。默认构建关闭音频、三维模型和内置图标。`--audio` 启用独立 `desktop_audio` 模块，`--icons` 加入图标。脚本校验归档摘要，`--offline` 只读已下载的有效归档。

```powershell
python tools/setup_desktop_gui.py
chali aot example/desktop-gui/demo.chl -I build/desktop_gui --out build/desktop_gui/demo.exe
```
运行 `demo.exe` 后，`Count` 按钮累加计数，关闭窗体时输出 `clicks=N`，未点击时为 `clicks=0`。在创建窗口的线程上完成绘制与事件循环，退出时调用 `close()`。音频构建使用独立输出目录。

```powershell
python tools/setup_desktop_gui.py --audio --out build/desktop_audio
chali run example/desktop-gui/audio.chl -I build/desktop_audio --out build/desktop_audio/audio.exe -- path/to/tone.wav
```
成功播放输出 `playback finished`。没有可用设备时输出 `audio device unavailable`；无效或无法解码的文件输出 `audio file could not be loaded`。显式关闭会停止并释放仍登记的声音。

## XML

```powershell
python tools/setup_xml.py
chali run example/xml/demo.chl -I build/xml --out build/xml/demo.exe
```

```text
XML OK
```
`xml::parse` 读取 UTF-8 文档并返回根节点。元素名和属性名保留源码中的限定拼写，文本片段和子元素按混合内容顺序读取。`escape_text` 与 `escape_attribute` 分别用于元素文本和带引号的属性值。输入上限为 16 MiB，另有限制嵌套和中间数据。解析错误包含从 1 开始的行列；`DOCTYPE` 被拒绝，解析器不访问外部实体。相邻字符片段可能分成多项。

## 密码学

```powershell
python tools/setup_crypto.py
chali run example/crypto/demo.chl -I build/crypto --out build/crypto/std/demo.exe
```

```text
crypto OK
```
`std::crypto` 提供安全随机、SHA-256、HMAC-SHA-256 和 Ed25519 分离签名验证。摘要和随机字节使用小写十六进制，签名验证接收固定长度的十六进制公钥和签名。Windows x64 使用锁定的 libsodium 归档，Linux 从系统开发包取得 libsodium。摘要可用于协议互操作，密码存储需采用专用的密码哈希方案。

## HTTP 流式传输

```powershell
python tools/setup_http_transfer.py
```
导入 `std::net::http` 后，`HttpClient::open()` 创建会话，`download_to` 发起 GET 并写入新文件，`upload_file` 以 PUT 流式读取源文件。同一会话重复请求时，libcurl 在协议允许的情况下复用连接。超时覆盖完整请求；同步回调收到已下载和已上传的字节数，返回非零可取消。`keep_going` 只使用超时。HTTP 状态与传输失败分开返回，下载成功且响应为 2xx 才保留本次创建的文件。调用者使用 `close()` 关闭会话。

Windows 可运行 [GUI 与音频](../example/desktop-gui)、[XML 示例](../example/xml/demo.chl) 和 [密码学示例](../example/crypto/demo.chl)。实现使用 [raylib](https://github.com/raysan5/raylib)、[raygui](https://github.com/raysan5/raygui)、[Expat](https://github.com/libexpat/libexpat)、[libsodium](https://doc.libsodium.org/) 与 [libcurl](https://curl.se/libcurl/)。

[下载示例](../example/http-transfer/download.chl)。传入 URL 和尚不存在的目标路径。成功获取 200 响应时输出 HTTP 200，传输失败时输出具体错误。
