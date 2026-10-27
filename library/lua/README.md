# Lua 性能对照依赖

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

源码仓库仅保留 [manifest.json](manifest.json)、下载入口和 [许可证](LICENSE.txt)，不保存 Lua 源码归档或展开的源码。

运行 `python benchmarks/perf/build_lua.py` 时按需调用 [tools/fetch_lua.py](../../tools/fetch_lua.py)，下载官方 Lua 5.5.1，核对固定 SHA-256，再以 GCC `-O2 -DNDEBUG` 构建性能宿主。普通 ChaLi 构建不下载或链接 Lua。

归档缓存位于 `build/downloads/lua/`，源码位于 `build/dependencies/lua/lua-5.5.1/`，均不入库。直接准备依赖可运行 `python tools/fetch_lua.py`，离线复用为 `python tools/fetch_lua.py --offline`。`python library/lua/setup.py` 为兼容入口。

性能宿主仅注册单调纳秒时钟并装载 `.lua` 文件；算法、循环、函数调用和 table 读写由官方 Lua VM 执行，不使用 LuaJIT。构建后核对宿主报告的实际版本；历史测量保留原始版本与摘要，不因下载方式改变而重标。
