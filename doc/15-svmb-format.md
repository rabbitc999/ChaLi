# 旧字节码格式迁移

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](15-svmb-format.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 1.0.8 已删除 `.svmb` 格式及相关导出、执行和编辑器构建入口。旧格式说明与测量保留在 Git 历史中，不属于当前语言的交付方式。

使用 `chali run app.chl` 进行 checked AOT 编译执行，使用 `chali aot app.chl --out build/app` 导出原生产物；后者默认 unchecked，可显式加 `--checked`。源码调试使用 `--debug` 及 GDB。

详见[原生部署](book/22-部署原生产物.md)、[AOT 编译](41-aot.md)和[原生调试](46-native-debugging.md)。
