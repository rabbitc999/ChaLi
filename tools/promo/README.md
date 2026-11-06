# ChaLi 1.0.0 无音频宣传片

使用 ManimGL 1.7.2（`manimlib`，不是 Manim Community）和 FFmpeg。从仓库根目录运行，确保 FFmpeg 位于 PATH，安装 Microsoft YaHei 和 Consolas 字体：

```powershell
python -m manimlib tools/promo/chali_promo.py ChaLiPromo -w --hd --config_file tools/promo/config.yml --video_dir tmp/promo
```

场景直接读取 `doc/images/chali-poster.png` 和 `chali-logo.png`，不使用音频、LaTeX 或 Python 绘制的旧标志。画面为 1920×1080、30 fps，时长约 64.2 秒。ManimGL 1.7.2 的 `--fps` 参数可能按字符串处理，帧率在 YAML 中设置。

导出时将实际生成的 MP4 路径传给 FFmpeg，移除任何音轨并设置流式播放索引：

```powershell
ffmpeg -i <生成的MP4> -an -c:v copy -movflags +faststart <发行MP4>
```

Run the command from the repository root with ManimGL 1.7.2, FFmpeg on PATH, Microsoft YaHei and Consolas installed. The scene uses the shared kitten artwork and produces a silent 1080p, 30 fps film of approximately 64.2 seconds. Set the frame rate through YAML. Use the final FFmpeg command to remove audio tracks and enable fast start.
