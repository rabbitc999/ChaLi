# 引用非空 LLVM 原语与泛型探查

Windows x64 外置 LLVM 编译器 SHA256：`275ff2a421920331a3ba84e60bd469c3b576df45a262ad4efbfa3201b68402db`。

__std_reference 的正常非零路径改为直接调用同一 LLVM 模块中的小型校验原语，由 LLVM O3 内联及消除可证明的重复检查；不再构造宿主槽向量、查找函数或查询结果 TLS。零值保留原共享运行时错误“引用尚未绑定有效元素；缺省值请使用 Option”。CHALI_NO_AOT_REFERENCE_PRIMITIVES=1 或显式步数限制保留原路径。

运行时 LLVM 模块由 tools/build_aot_runtime.sh 使用固定 clang 从 C 源生成，Windows、Linux x64、Linux ARM64 三个模块同步；正常程序仍由 C++ 直接从已检查 AST 生成 LLVM。Python 仅调度和验证。

variants.json 记录修改前编译器及八组 malloc／pool、checked／unchecked、原语开关的泛型拥有值／借用行为和未绑定引用错误，共 18 个程序产物。完整 AOT、同步 C 回调与泛型单态化专项通过，显式 --step-limit 回退亦通过。未在本轮声称静态 LLVM、交叉目标执行或安装包验收。

新增 aot_generic_codegen.chl 核对标量整数／浮点泛型、字符串拥有值副本、泛型借用读取及嵌套 List 副本。将实际生成模块交给固定 clang -O3 -S -emit-llvm 后，genericScalar 和 concreteScalar 的函数体均为 33 行且没有调用，原引用原语调用也全部内联消失；摘要与两个函数体在本目录。这只说明该标量探查，没有据此宣布所有泛型无开销。

后端设计参考：[Rust operand.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_codegen_ssa/src/mir/operand.rs) 的标量／二元标量／地址表示和字段／判别值提取，[Clang CGExpr.cpp](https://github.com/llvm/llvm-project/blob/main/clang/lib/CodeGen/CGExpr.cpp) 的左值与字段地址生成，以及 [HotSpot klassVtable.cpp](https://github.com/openjdk/jdk/blob/master/src/hotspot/share/oops/klassVtable.cpp) 的继承表与接口表初始化。这里只参考表示与生成策略，不复制其他项目实现；main／master 树是设计资料，不当作本轮固定 Rust／Java 二进制的源码快照。

下一步仅复测 ChaLi，复用基础负载及新特征已有原始参考；性能收益尚未测量。最终要求逐项快于 Lua且耗时不超过 C++ O3 的 1.05 倍，不能以本次中间提交代替最终达标。
