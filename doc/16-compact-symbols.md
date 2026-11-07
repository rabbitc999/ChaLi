# 原生产物符号

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](16-compact-symbols.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

AOT 发布构建默认进行 LLVM 优化、LTO、死代码消除和符号裁剪；反射、回调及导出根按实际引用保留。调试构建通过 `--debug` 保留原生符号与源码行，并降低优化级别。

当前选项见[LLVM 构建](38-build.md)和[原生调试](43-native-debugging.md)。
