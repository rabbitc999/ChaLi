# 原生产物符号与旧格式迁移

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](16-compact-symbols.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

1.0.8 删除 `.svmb` 后，不再提供字节码符号压缩和 PC 映射交付。旧格式细节保留在 Git 历史。

AOT 发布构建默认进行 LLVM 优化、LTO、死代码消除和符号裁剪；反射、回调及导出根按实际引用保留。调试构建通过 `--debug` 保留原生符号与源码行，并降低优化级别；调试符号与字节码符号表是不同机制。

当前选项见[AOT 编译](38-aot.md)和[原生调试](43-native-debugging.md)。
