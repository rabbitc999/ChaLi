# ChaLi 四元式与原生共享运行时（`source/ircode`）

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本目录提供 ChaLi 四元式及原生共享运行时。`run` 使用 checked AOT，`aot` 默认生成 unchecked 原生产物。

保留的实现负责四元式程序模型、优化与分析，以及 AOT 共用的内存、对象、所有权、反射、线程、宿主函数和同步回调服务。代码要求 C++17 和受支持的操作系统服务，不承诺裸机支持。

`aot_sources.txt` 是编译与内嵌资产的源文件清单，`core_assets.hpp` 由 `tools/stackvm_sources.py --build-assets` 生成。根目录构建和导出工程使用同一清单。

`stackvm` C++ 命名空间、`Machine` 类型和相关构建目标供共享运行时与宿主胶水使用。

原生运行及调试见 [AOT](../../doc/38-aot.md) 和 [原生调试](../../doc/43-native-debugging.md)。
