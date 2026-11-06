# Lua 性能对照依赖

当前统一测试由 [benchmarks/current/run.py](../../benchmarks/current/run.py) 构建 Lua 宿主。源码从官方 `lua/lua` 仓库的 `v5.5.1` 获取，固定提交为 `7579fc9d7ed90240487251dfb69168f8e64e9294`，校验提交与工作区后使用 GCC O3/LTO 构建。源码和构件保存在忽略的 `build/dependencies/lua/`，不入库。

宿主只提供单调纳秒时钟，算法和 table 操作由官方 Lua 解释器完成，使用默认 GC。普通 ChaLi 构建不下载或链接 Lua。

[manifest.json](manifest.json) 保留官方发布归档及 SHA-256；[tools/fetch_lua.py](../../tools/fetch_lua.py) 和 `library/lua/setup.py` 可单独准备该归档。许可见 [LICENSE.txt](LICENSE.txt)。
