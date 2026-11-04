# 独立编译器工程生成器

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

工程生成器从权威文法、前端实现和共享原生运行时 生成可单独构建的 ChaLi 编译器工程。

```sh
chali gen compiler out build/compiler
chali clear out build/compiler
```

生成工程提供语法和语义检查、checked AOT 运行、LLVM AOT 构建，以及 ChaLi 四元式、LLVM IR、对象和机器汇编导出。标准库与运行时资产在构建时内嵌，运行编译器不读取源码仓库。
反射默认同时编入前端和原生运行时；优化交给 LLVM。宿主胶水、外设和动态库仍按实际目标配置。

构建依赖 C++17 工具链和解析器生成器。`clear` 只清理工程生成器清单中的文件，保留用户文件。
