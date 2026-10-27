# ChaLi 四元式与原生共享运行时（`source/ircode`）

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本目录由 `source/stackvm` 迁移而来。从 1.0.8 起，`run` 使用 checked AOT，`aot` 默认生成 unchecked 原生产物。解释执行器、寄存器执行器、旧 DAP 服务和 `.svmb` 编解码及命令行入口已删除。

保留的实现负责四元式程序模型、优化与分析，以及 AOT 共用的内存、对象、所有权、反射、线程、宿主函数和同步回调服务。代码要求 C++17 和受支持的操作系统服务，不承诺裸机支持。

`aot_sources.txt` 是编译与内嵌资产的源文件清单，`core_assets.hpp` 由 `tools/stackvm_sources.py --build-assets` 生成。根目录构建和导出工程使用同一清单。

现有 `stackvm` C++ 命名空间、`Machine` 类型及兼容构建目标名称暂时保留，供共享运行时与宿主胶水使用；它们不再提供解释执行入口。目录迁移不改变这些接口的 ABI。

原生运行及调试见 [AOT](../../doc/38-aot.md) 和 [原生调试](../../doc/43-native-debugging.md)。
