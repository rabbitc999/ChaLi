# 默认字符串基准

四项负载比较 ChaLi 默认 `String`、C++ `std::string`、Rust `String` 与官方 Lua VM 完成相同字符串操作的成本。ChaLi 显式使用系统 `malloc`；参考语言使用标准分配器。C++ 为 GCC `-O2`，Rust 为 `rustc -O`，ChaLi 为 unchecked LLVM AOT 默认 O3；实际工具版本、二进制摘要、命令和环境随结果保存。

完整代码在 [cases.chl](cases.chl)、[cases.cpp](cases.cpp)、[cases.rs](cases.rs) 和 [cases.lua](cases.lua)。[run.py](run.py) 的 Python `bytes` 模型独立核对五个种子，不生成 LLVM IR。

输入由 256 个 UTF-8 单元构成。每个单元是 `ChaLi-字符串-🙂-0123456789|` 加一个随单元序号变化的 ASCII 字母，单元长 33 字节，总长 8448 字节。初始化不计时。读取、切片及查找的种子和归并值影响后续位置；拼接结果的全部字节进入校验，避免仅观察长度。

| 负载 | 工作函数内的操作 |
| --- | --- |
| `string_read` | 131072 次依赖索引的无符号字节读取，包含 ASCII、汉字及 emoji 的 UTF-8 字节 |
| `string_concat` | 从空字符串开始追加 2048 个单元，不预留结果容量，随后归并完整 67584 字节 |
| `string_slice` | 1024 次建立两个拥有型子串并比较；端点落在完整单元边界，长度为一至四单元，比较覆盖相同和不同内容 |
| `string_find` | 256 次从单元边界开始查找，三种命中模式和一种未命中模式；返回值统一为零基字节位置，未命中为负一 |

字节读取使用 ChaLi `byteAt(...).unwrap()`、C++ 无符号字节访问、Rust `as_bytes()` 和 Lua `string.byte`。ChaLi 的普通字符串下标会检查 UTF-8 字符边界，本项使用明确的字节 API。切片保持完整 UTF-8，因此 Rust 使用标准 `String` 子串复制，未改成字节数组。查找分别使用各语言的标准字符串 API，保留其查找算法。

逐次拼接分别使用 ChaLi `s = s + chunk`、Lua `s = s .. chunk`、C++ `+=` 和 Rust `push_str`。各实现的可变性、容量增长、复制和短字符串策略是测量的一部分。Lua 另测收集片段后 `table.concat` 的常用实现，最终 Lua 门槛取这两个实现中更快的中位数。两个 Lua 实现都归并完整结果，输入、输出及计时范围一致。

每个配置运行五个独立进程，每进程先预热一次，再测五个种子。C++ 和 ChaLi 共享 [C 计时壳](../features/timing.c)，分离编译且不对 C++ 开启 LTO；工作函数通过 volatile 函数指针调用。Rust 使用 [原有计时壳](../features/timing.rs) 和 `black_box`，四个工作函数禁止内联进计时壳。Lua 使用 [官方 VM 单调时钟宿主](../perf/luabench.c) 和 [timing.lua](timing.lua)，保持默认 GC。分配、拼接、子串创建和工作函数内的清理计入耗时，构建及输入准备不计时。

先做无性能样本的预检：

```powershell
python benchmarks/strings/run.py --compiler-source-commit <实际编译器源码提交> --preflight --out tmp/strings-preflight
```

源码定稿并提交后，首次建立参考。该模式拒绝覆盖已有参考目录：

```powershell
python benchmarks/strings/run.py --compiler-source-commit <实际编译器源码提交> --establish-baseline --runs 5 --out benchmarks/results/1.0.11-string-baseline
```

后续只复测 ChaLi，验证冻结源码及计时壳的摘要，直接读取参考；不执行或查询 C++、Rust、Lua 程序：

```powershell
python benchmarks/strings/run.py --compiler-source-commit <实际编译器源码提交> --reference benchmarks/results/1.0.11-string-baseline/results.json --runs 5 --out benchmarks/results/<新的字符串里程碑>
```

正式验收逐项要求 ChaLi 快于保存的 Lua 参考，并且耗时不超过 C++ O2 中位数的 1.05 倍。Rust 作为独立比较，不以平均值抵消未达标项。原有 [1.0.7 字符串记录](../results/string-1.0.7/README.md) 保持原版本、配置和数据来源，与这套跨语言负载分开保存。
