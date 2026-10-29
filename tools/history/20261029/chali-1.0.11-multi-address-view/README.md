# 四项 LLVM 地址视图

保留原 40 字节视图布局及 chali_host_address_view ABI，新增三项线程局部视图和轮换索引。每项独立核对非零映射代次和完整无符号范围；只有全部未命中才调用既有宿主检查。进入／退出／销毁上下文的既有 reset 入口清空四项视图，保持旧分配不可复活。

CHALI_NO_AOT_MULTI_ADDRESS_CACHE 保留单项模式；CHALI_NO_AOT_ADDRESS_CACHE 仍禁用缓存。查找和轮换直接由 C++ 发射 LLVM 指令，不借 Python 生成 LLVM IR，不增四元式优化或宿主胶水。

集成用例交错访问五个同时活跃的分配，覆盖四项轮换；分配器 malloc／pool 与 multi／single／off 六组模式，在 checked run 和 unchecked 动态库中核对结果。同一持续宿主线程跨每组 64 次初始化／销毁，合计 384 轮、每轮两个不同种子。

底层 C++ 地址视图测试用当前 AOT 核心库及 system_arena.o 链接，覆盖扩容、释放、收窄、堆销毁、零代次永久失效；实际构建及执行退出码均为 0。首次 PowerShell 命令的 -Wl,--gc-sections 未加引号而被逗号语法拒绝，随后使用带引号的真实参数执行成功。

最终全量门禁和最终性能目标尚未完成；本里程碑提交后，仅正式复测 ChaLi 受影响负载，不执行其他语言参考。静态 LLVM 摘录只是核对代码生成，不能代替性能数据。

六组集成及 384 生命周期轮次全部通过；String 缓冲区八组及错误／禁止修改、同步回调与完整 AOT 专项均通过；新增专项已注册 ci.sh，shell 语法及 Python 语法检查通过。
