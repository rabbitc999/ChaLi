# Lua 性能对照依赖

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本目录保存官方 [Lua 5.5.1](https://www.lua.org/ftp/lua-5.5.1.tar.gz) 源码归档，用于 ChaLi、LLVM AOT 与其他语言的性能对照，不链接进 ChaLi 编译器。

来源、版本与官方 SHA-256 见 [manifest.json](manifest.json)，许可证见 [LICENSE.txt](LICENSE.txt)。`python library/lua/setup.py` 校验并离线解包到 `source/`；`python benchmarks/perf/build_lua.py` 使用 GCC `-O2 -DNDEBUG` 编译官方 VM 与提供单调纳秒时钟的微型宿主。

算法、循环、函数调用与 table 读写均由官方 Lua VM 执行，宿主仅注册 `now_ns()` 并装载 `.lua` 文件。保留标准 64 位整数、双精度浮点与默认垃圾回收，不使用 LuaJIT。每轮测量核对宿主报告的实际 Lua 版本与校验值。
