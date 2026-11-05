# ChaLi 内置 TOML 库副本

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

这里的 C++ 解析、值模型与序列化用于编译器及工具内部，`toml.cpp` 和 `toml_source.cpp` 分别编译并链接。应用程序的 `std::toml` 已改用 `source/runtime_c/chali_toml.c` 和 `source/abi/abi_toml.c`，解析器为锁定版本及兼容补丁的 tomlc17 C 库；导出工程包含对应实现。

解析得到的根值保留源文本及注释。未修改时 dump() 逐字节保留原文，包括 BOM、换行和空白；已有值的等形编辑保留周围注释与布局。增删成员、改变容器形状或替换含注释的整体值时，全部原注释按原顺序保存为文档开头的独立注释，再输出当前值。结构编辑保证注释内容保留，位置可能改变。再次解析和保存不会重复或丢失注释；字符串里的 # 不视为注释。

手工构造的值及 inlineDump() 使用规范序列化。子节点仅保存值；原文布局保留适用于解析文档的根值。本目录的工具模型使用 C++ 标准容器；应用程序 C 服务保留相同的标准库格式协议。更新时运行 test_toml.cpp、C 服务差分及 ChaLi 结构化数据回归。

ChaLi 接口与生命周期见 [std::toml](../../doc/library/std-toml.md)。
