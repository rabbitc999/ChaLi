# 特征基线的 LLVM 模块快照

这些模块由 `6c9cc3e` 源码编译器直接生成，SHA256 为 `af3bbacec00aa5d2fc9858f517b6e99f3d8c765c226fee09076bf4e9632b6d3c`，对应 `benchmarks/results/1.0.11-feature-baseline/` 的工作负载，不由 Python 生成。

初步线索：enum_payload 的热循环调用两次 __std_reference，并为编译器生成的引用存储分配及释放 8 字节。接口与嵌套结构体也保留包装和地址访问边界。下一步需要按实际执行路径核对不逃逸性、空引用错误、所有权与析构，再逐项减少开销；目前仅保存诊断证据。

参考实现资料：[Rust 类型与载荷枚举布局](https://doc.rust-lang.org/stable/reference/type-layout.html)、[Rust OperandValue](https://doc.rust-lang.org/stable/nightly-rustc/rustc_codegen_ssa/mir/operand/enum.OperandValue.html)、[Clang RValue 的标量／聚合表示](https://clang.llvm.org/doxygen/CGValue_8h_source.html)。Rust 默认 enum 布局不作稳定 ABI 假设，不能把 repr(C) 的示意布局当作本次自然 Rust 布局保证。相关改造仍直接从已检查 AST 生成 LLVM，不恢复四元式或添加额外中间转换。
