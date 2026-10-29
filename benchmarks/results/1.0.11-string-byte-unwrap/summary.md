# 默认 String：逐项性能对照

| 负载 | ChaLi / ms | C++ O2 / ms | Rust / ms | 较快 Lua / ms | Lua 配置 | 当前门槛 |
| --- | ---: | ---: | ---: | ---: | --- | --- |
| string_read | 13.9147 | 0.5427 | 0.5437 | 6.0014 | lua | 未通过 |
| string_concat | 22.6670 | 0.1012 | 0.0699 | 4.1247 | lua-builder | 未通过 |
| string_slice | 12.1186 | 0.1329 | 0.1323 | 0.3515 | lua | 未通过 |

每格五个样本／进程，实际进程数见 results.json；Lua 拼接取逐次拼接和 table.concat 中更快者。参考语言首次建立后仅复用。最终门槛逐项要求快于 Lua，且不超过 C++ O2 的 1.05 倍。
