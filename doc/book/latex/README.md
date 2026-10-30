# LaTeX 书本

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本目录维护《ChaLi 程序设计语言》的 [PDF](book.pdf)，语言版本以根目录 [`VERSION`](../../../VERSION) 为准。章节源稿在 [doc/book](../README.md)，正文章节由 `tools/md2tex.py` 生成；`book.tex`、`preamble.tex` 和封面源文件手写维护。

在仓库根运行：

```sh
python doc/book/latex/tools/md2tex.py
python tests/chali_book_rewrite.py
python tests/chali_book_updates.py
```

然后进入 `doc/book/latex`，使用已安装的 TeX Live 与中文字体先生成封面，再构建正文两次，使目录和页码稳定：

```sh
mkdir -p ../../../tmp/book-cover ../../../tmp/book-latex
xelatex -interaction=nonstopmode -halt-on-error -output-directory=../../../tmp/book-cover figure/cover.tex
cp ../../../tmp/book-cover/cover.pdf figure/cover.pdf
xelatex -interaction=nonstopmode -halt-on-error -output-directory=../../../tmp/book-latex book.tex
xelatex -interaction=nonstopmode -halt-on-error -output-directory=../../../tmp/book-latex book.tex
cp ../../../tmp/book-latex/book.pdf book.pdf
```

生成的 `.tex` 章节入库，`book.pdf` 与其它 LaTeX 产物（`.aux` / `.log` / `.toc` / `.synctex.gz` 等）不入库，需要时按上面的命令构建；跑文档链接门禁前先构建出 PDF，`doc/book/README.md` 的电子书链接指向它。完整程序用 `<!-- verify-book -->` 标记，紧接 ChaLi 代码和预期输出；[书稿回归](../../../tests/chali_book_updates.py) 执行这些程序并检查全部生成章节一致。

正文包括表达式、插值、关联类型、整数泛型、作用域清理、宏补全、捕获错误诊断、行输入、文件文本读取和终端手册，调整表格列宽及行内代码换行，避免长签名挤出页边。


## 维护与校验

读者入口只说明阅读方式；生成流程和回归命令在本页维护。书稿完整程序及 Markdown 与 LaTeX 的一致性由 [书稿回归](../../../tests/chali_book_updates.py) 检查。相关功能回归还包括[返回表达式、具名闭包与派生](../../../tests/chali_declaration_sugars.py)、[多括号宏与源码映射](../../../tests/chali_macro_groups.py)、[泛型单态化](../../../tests/chali_generic_monomorphization.py)和[文本输入](../../../tests/chali_text_readers.py)。


## 示例排版

ChaLi 代码使用四空格缩进，赋值及二元运算符两侧、逗号后保留空格；函数体和控制分支展开为多行，避免将声明、赋值与退出语句压在同一行。插值字符串的文字部分及宏的原始文本须保持原意。PDF 代码样式使用 `columns=fullflexible` 和 `keepspaces=true` 保留可见的缩进与词间空格，调整后须检查实际页面，并执行书稿程序回归。

所有 `lstlisting` 允许自动换行和跨页，不用 `minipage`／`samepage` 整块保留短示例。列表项后的代码独立排版，不嵌套在 `enumerate` 或 `itemize` 中；拆开后的有序列表保留原编号。书稿回归同时检查这些结构约束。

标题的井号后需要空格，正文中的 `#borrow` 等指令名不作为标题。图片同时限制宽度与页高并保留比例，图注与图片保持同页；修改后需要实际渲染 PDF 核对，不能只检查 LaTeX 是否返回成功。

## 运行图与保留原文

[make_students_diagram.py](../../../tools/make_students_diagram.py) 生成第 25 章中英文 SVG 图源与 PNG，渲染需要 Playwright 和 Edge。运行截图由 [students_server.py](../../../tests/students_server.py) 连接独立 MySQL 测试库、启动真实程序并通过浏览器操作生成；从已通过验收的结果中复制需要的图片，保留角色、数据范围和应用摘要。

[book_extras_manifest.json](../../../tests/book_extras_manifest.json) 记录本次重写之前的番外原文摘要。检查忽略换行风格和章末小节序号调整，核对标题、故事与技术对照原文；中英文完整例程的源码、输入和输出也需对应。交互程序使用 verify-book-input 的 JSON 字符串记录固定输入。

构建前让 TMPDIR、TMP 和 TEMP 指向仓库根目录 tmp；上面的 shell 命令在 Git Bash 或支持相同命令的环境执行。生成和验收结束后，先保留最终产物与证据，再按仓库约定运行 tools/clean_tmp.ps1。
