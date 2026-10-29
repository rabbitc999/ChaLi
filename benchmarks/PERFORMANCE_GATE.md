# 最终保存数据的性能判定

`evaluate_saved.py` 只重算已保存样本，不编译、不计时，也不运行参考语言。当前条件为：有冻结 Lua 参考的项目耗时严格更低，单态接口不超过 40 ms、多态不超过 400 ms，均含等号。拼接取较快的 Lua table.concat。C++ O2 只作参考，其他四项特征没有独立门限。

最终数据集中在 `results/1.0.11-final/`：真实 1.0.10 pool、1.0.11 pool、1.0.11 malloc 各 17 项，五进程、每进程五样本；参考语言的最终冻结数据在 `references/`。当前分支删除历史测量目录，旧提交历史保持不变。

对同一个完整分配器配置判定：

```sh
python benchmarks/evaluate_saved.py \
  --reports benchmarks/results/1.0.11-final/results.json \
  --configuration 1.0.11-pool \
  --out tmp/recheck-pool
```

用 `--configuration 1.0.11-malloc` 检查 malloc。`--compiler` 可另行核对实际被测编译器字节；文档和内嵌资产重建后的编译器摘要可能不同，应使用保留的真实测量产物，不能重标报告。

本轮 pool 在 13 项条件中通过 8 项，malloc 通过 7 项，四项未设门限；两组整体均未全部通过。返回 1 表示有效数据中仍有未达标项；参数、摘要、样本或来源错误返回 2；所有已选条件满足且存在可判定项目时返回 0。输出目录必须尚不存在。

版本报告按一个统一配置投影，不混用单项最佳值。工具检查 25 个正数纳秒样本、重算中位数、拒绝失败／带 stderr 进程，校验原始基准 Git 快照、冻结用例与计时壳、实际编译器和分配器。支持旧式格式以便从 Git 复核历史；单测使用临时构造的坏输入，不依赖被删除的历史测量。

`--partial` 明确只分析部分项目；`--allow-mixed-compilers` 仅用于有标记的历史分析，均不能产生完整候选标记。`full_ci_executed` 始终为 false，因为该工具未执行功能门禁；全量门禁另行运行和记录。
