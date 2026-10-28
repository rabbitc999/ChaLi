# 集合基准约定

正式对照使用系统 malloc、unchecked LLVM AOT、O3/LTO。C++ 为标准容器的 O3 配置，Rust 为标准容器的 `-O` 配置，Lua 为官方 VM 默认 GC。配置和工具版本均写入结果 JSON；不预留容量，不修改参考语言的默认哈希策略。

| 负载 | 元素数 | 操作 |
| --- | ---: | --- |
| list_growth | 400000 | 从空容器追加整数，再按下标顺序归并 |
| map_growth | 20000 | 空表插入、读取原值并覆盖、命中／缺失查询、每隔三个键删除、重新插入、按原始键序归并 |
| set_growth | 20000 | 空表插入、重复插入、命中／缺失查询、每隔三个键删除、重新插入、按原始键序查询归并 |

Map、Set 的键为 `(index * 65537 + seed * 17) % 1000000007 + 1000000007`，在本轮下标范围中唯一且分散，避免 Lua table 的连续数组区。缺失查询使用这些键的相反数。不同集合的元素数不同，只能在同一负载内比较语言效率。

Map 的三轮写入值依次为 `index * 3 + seed`、`index * 5 + seed` 和被重新插入项的 `index * 7 + seed`。Set 的 Lua 实现以 table 的 true 值表示存在。校验值不依赖哈希遍历次序；不为计数额外遍历某一种语言的容器。

每个进程先用 seed=0 预热，再用 seed=0、1、2、3、4 测量。五轮计时和预热返回值合并后的预期 sink：Map 为 `6004096376`，Set 为 `2373508172`。驱动在跨语言比较之外，使用不构建哈希容器的算术模型核对这两个结果。编译、模块初始化与模型核对不计时；局部容器的创建、扩容及销毁计时。

C++ 使用 `std::unordered_map/set`，Rust 使用默认 `HashMap/HashSet` 和 RandomState，Lua 使用 table，哈希实现不同。C 另附系统 calloc/free 的开放寻址整数表参考，它不是 C 标准库容器；其头文件摘要同样记录。ChaLi 的 C 计时壳只复用时钟和调用协议，不调用 C 参考哈希表。

示例命令：

```powershell
python benchmarks/perf/run.py --runs 5 --allocator malloc `
  --compiler-source-commit <编译器来源提交> `
  --only chali-aot,lua,cpp-o3,rust `
  --cases int_chain,float_chain,call,array,list_growth,map_growth,set_growth `
  --out benchmarks/results/<新里程碑目录>
```

补测历史编译器时，CHALI 指向保留的真实二进制，`--compiler-source-commit` 指定该二进制的源码提交。JSON 中 `git_commit` 表示本轮基准与脚本所在的工作区提交，`chali_compiler.source_commit` 表示被测编译器来源，SHA256 核对实际二进制。未显式提供来源时记录 null，不根据文件名推断。历史结果不改写。

`--cases` 可单独选择 Map、Set；未实现这些负载的其他语言配置不应加入本轮 `--only`。旧 `list` 的预分配参考继续保留，匹配动态增长的正式对照使用 `list_growth`。

其他语言基线已经完成后，后续优化轮次按用户授权使用 `--only chali-aot`，不重复运行 Lua、C++、Rust。报告直接引用保留结果中的参考列及其来源时间、配置和原始样本；先核对负载源码、计时壳和参数保持一致。新结果 JSON 只包含实际执行的配置，不把复用列写成新测量。负载或计时协议变化时，仅重测受影响的参考项。
