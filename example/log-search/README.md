# 日志检索工具

第 16 章的教学工程。按行匹配文本，输出原行号和原文；默认区分大小写。

在本目录运行：

```sh
chali run src/main.chl -- ERROR sample.log
chali run src/main.chl -- --ignore-case ERROR sample.log
chali run src/main.chl -- --ignore-case --count ERROR sample.log
```

最后一条输出 `2`。应用参数中的 `--` 结束选项识别，可用于以两个连字符开头的查询：`chali run src/main.chl -- -- --example sample.log`。

查询不能为空。文件最多四 MiB，必须是没有零字节的文本；行尾支持 LF 与 CRLF，忽略大小写只处理 ASCII 字母。无匹配不是错误，普通模式没有输出，计数模式输出零。输入或文件错误由库函数返回 Result，再由入口报告为运行失败；本例没有实现 Unix grep 的完整退出码协议。

核心搜索、参数解析与文件访问分开，便于单独测试。完整项目验收入口为 `python tests/chali_book_projects.py`。

Windows 当前文件打开路径应使用系统能识别的字符；中文 UTF-8 文件内容与查询不受此说明限制。
