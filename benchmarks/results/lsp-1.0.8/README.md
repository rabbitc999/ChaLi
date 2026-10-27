# LSP 1.0.8 增强验证证据

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

日期：2026-10-27。平台：Windows x64，AMD Ryzen 5 5600X，LLVM 外置变体。版本未变更。构建配置及 SHA256 见 [metadata.json](metadata.json)。

本轮只执行 LSP 门禁。闭包门禁使用 --lsp-only，未运行 run、VM、AOT 或书稿程序回归。编译诊断由真实 LSP 触发。中文 PDF 另做生成与页面检查。

## 验收

- 最终 LSP 门禁：16 项通过，0 失败，0 跳过；[结果](gates/results.json)、[控制台输出](gates.log)。
- 基础闭包门禁：42 组提示。风险用例：17 组，另含 6 组赋值上下文（fac、Fibonacci、显式 mut[counter]、普通签名、遮蔽、函数字段）；递归例子检查悬停、补全、内联提示及编译诊断。
- 修复未知／空分支被忽略、高阶返回函数签名被截断、独立赋值无法为省略参数提供类型、参数声明处补全不可见。悬停与补全共用展示类型入口。
- 修复旧原生对照测试依赖不存在的 chali_lsp_api，复用带超时的 Transport，并增加当前可运行的声明、泛型字段与未保存依赖对照。
- [build.log](build.log) 为最终成功构建输出。内嵌书稿资产在文档定稿后生成，再完成编译器重建。
- [attempt-linking](attempt-linking/results.json) 保留一次链接未退出时过早启动门禁的失败记录；前八项为 WinError 32，最终验收采用其后完整成功的一轮。

## 高亮测量

同一脚本、每份文档 1600 个声明、8009 个 token；四次请求中首个单列，后三个取中位数。基线为修改前工作区已有外置编译器，没有重新构建或归档基线二进制。耗时受机器负载影响，仅代表本机该输入。

| 布局 | 修改前首次 ms | 修改后首次 ms | 修改前重复中位 ms | 修改后重复中位 ms |
| --- | ---: | ---: | ---: | ---: |
| 多行 | 25.829 | 18.469 | 13.146 | 7.565 |
| 长行 | 236.409 | 48.666 | 231.318 | 43.492 |

长行重复请求约提速 5.32 倍。实现按源码顺序推进 UTF-16 游标及词法项，避免从行首反复扫描，且跳过重复绑定扫描。测试同时检查 token 编码、非 BMP 字符位置、重复结果及编辑失效。原始值见 [修改前](highlight-before.json) 和 [修改后](highlight-after.json)。

## 书稿

中文 Markdown、LaTeX 与 303 页 PDF 已同步，英文只同步 Markdown。新增 fac、Fibonacci 和 fn mut[counter](x) 计数器，输出示例为 counter=0、fib(3)=2、counter=5；这些是书稿预期输出，本轮未运行程序验证。

PDF 实际第 164、165、166、187 页（正文 149、150、151、172 页）已渲染检查，证据保存在 [book-review](book-review)。生成日志见 [book-generate.log](book-generate.log)、[首轮](book-build-1.log)、[第二轮](book-build-2.log)。最终 PDF 按仓库约定保存在 doc/book/latex/book.pdf，不纳入 Git。

复现：

```powershell
python tools/run_module_gates.py lsp --chali ./chali-1.0.8-llvm-external-windows-x64.exe --jobs 1
```
