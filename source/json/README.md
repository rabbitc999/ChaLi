# ChaLi 内置 JSON 库副本

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

这里保留编译器内部的 C++ JSON 模型、解析与序列化实现，本轮运行时重构没有修改这些实现文件。
应用程序的 `std::json` 使用 `source/runtime_c/chali_json.c` 和 `source/abi/abi_json.c`，解析器为锁定版本的 yyjson C 库；两条路径独立构建。
编译器模型继续使用 C++ 标准容器。修改本目录实现时运行对应库测试；应用程序的生命周期和格式协议另由 C 服务及标准库回归验证。
ChaLi 接口与生命周期见 [std::json](../../doc/library/std-json.md)。
