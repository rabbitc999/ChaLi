# 无守卫枚举匹配标签快照

仅调整检查后 AST 的降级：没有守卫且存在具体变体时读取一次标签，让 LLVM 自行合并 switch；任何守卫、仅兜底的 match 保留原路径。CHALI_NO_ENUM_TAG_SNAPSHOT=1 关闭。未增加四元式优化，也未用 Python 生成 LLVM IR。

编译器 SHA256：54f66f4f5e96ae60cb6a49087576dc0099110ee01df6a2b265add832ec4cf268。代码在本目录所属提交，验证脚本和逐次结果保留。修改前 a619614 的同一新用例通过；八组 malloc/pool × checked/unchecked × 开关通过，七个已有样例开启/关闭均与期望一致，六类枚举拒绝诊断与旧编译器一致。完整 tests/chali_aot.py 和 24 项资源生命周期通过。

LLVM O3 的枚举 work 函数由 366 行变为 265 行，静态原子读取位置 9→6，switch 0→1；这些是生成结构证据，不代替性能测量。性能在提交之后另测，参考语言不重跑。

enum-tag-opt.log 保存首次误用输出文件名造成的检查失败；--out 会附加 .ll 后缀，纠正为实际 .ll.ll 文件后，enum-tag-opt-corrected.log 对应两份 LLVM O3 优化成功。失败没有计作通过。
