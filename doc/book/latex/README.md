# LaTeX 书本

本目录维护《玩转 Magel 语言》的 [PDF](book.pdf)，语言版本以根目录 [`VERSION`](../../../VERSION) 为准。章节源稿在 [doc/book](../README.md)，正文章节由 `tools/md2tex.py` 生成；`book.tex`、`preamble.tex` 和封面源文件手写维护。

在仓库根运行：

```sh
python doc/book/latex/tools/md2tex.py
python tests/magel_book_updates.py
```

然后进入 `doc/book/latex`，使用已安装的 TeX Live 与中文字体构建两次，使目录和页码稳定：

```sh
xelatex -interaction=nonstopmode -halt-on-error book.tex
xelatex -interaction=nonstopmode -halt-on-error book.tex
```

生成的 `.tex` 章节和 `book.pdf` 入库，编译日志及辅助文件不入库。完整程序用 `<!-- verify-book -->` 标记，紧接 Magel 代码和预期输出；[书稿回归](../../../tests/magel_book_updates.py) 执行这些程序并检查全部生成章节一致。

正文包括表达式、插值、关联类型、整数泛型、作用域清理、宏补全、捕获错误诊断、行输入、文件文本读取和终端手册，调整表格列宽及行内代码换行，避免长签名挤出页边。
