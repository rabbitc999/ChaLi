LLVM O3 小测，实际编译器源码 cf62694、驱动 b2f6fd7；测量启动时工作区干净。四项冻结负载各两种分配器、三进程各五样本，共 120 样本，校验全部一致；全部构建在计时前完成，进程按轮交错 malloc／pool，第二轮反转先后。数据和产物摘要见 results.json。

pool 在 List、Map、Set 小测分别较快，malloc 在 String concat 明显较快；四项等权几何 pool/malloc 为 1.101631，按预先记录规则选择统一 malloc。小样本不能证明微小差异稳定，不将 Map 的小差距推广为全面优胜。此综合值只选择分配器，最终目标仍逐项验收。现有性能参考及其他语言工具版本未执行；历史 malloc 结果未改名。

O3 核对：source/chali_aot_llvm.cpp 使用 default<O3>,globaldce 与 LLVMCodeGenLevelAggressive，链接使用 --lto-O3／--lto-CGO3；外置驱动使用 -O3；计时壳也以 -O3 构建。C++ O2 仍是当前最终门槛。
