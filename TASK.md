# 1.0.8 收尾任务

## 已完成

- [x] 可达数据池裁剪、稀疏非零映像、局部数组重复零写入消除和可达函数值根追踪。
- [x] run 为 checked AOT，aot 默认 unchecked；保留四元式导出，移除 VM 执行和 .svmb 路径。
- [x] 删除解释核、寄存器执行核、字节码编解码、旧 DAP 及相关专属测试；共享运行时迁移至 source/ircode。
- [x] 更新构建、导出工程、内嵌资产和文档路径；共享运行时实际静态库含 44 个对象，无已删除的 VM／字节码对象。
- [x] 1.0.8 外置 LLVM 编译器生成与构建成功。原生 smoke、真实 GDB 断点／单步／变量／表达式通过。
- [x] 插件与编译器版本统一 1.0.8；支持 static／external 分包，拒绝真实链接方式不匹配的编译器。
- [x] 外置 LLVM VSIX、MSI 已生成，VSIX 版本／变体／编译器字节核对通过。
- [x] 中文书稿 Markdown／LaTeX／PDF 同步；PDF 301 页，仅抽查封面及新原生部署章。
- [x] 文档假名扫描为 0，中英文运行／调试文档已同步。
- [x] 单例体积对照记录 benchmarks/results/aot-data-1.0.8/smoke.json，真实旧版 SHA256 保留；完整历史性能结果不重标。
- [x] AGENTS.md 加入生成验证收尾清理 tmp，提供 tools/clean_tmp.ps1，-WhatIf 预览通过。

## 尚未完成

- [ ] 生成实际 LLVM static 编译器和对应 1.0.8 VSIX：本机 library/llvm/static-sdk/bin/llvm-config.exe 不存在；未伪造静态产物。源码和打包流程已支持，仍需静态 LLVM SDK。
- [x] 已结束生成与验证进程，证据转存 dist/validation/1.0.8 及 benchmarks/results/aot-data-1.0.8；tools/clean_tmp.ps1 实际清理成功，tmp 条目为 0。
未完成项保留，全部完成才删除本文件。Git 提交与 origin/master 推送以最终交付中核对的对象号为准。

## 验证限制

用户要求仅简单验证，本轮未运行完整 CI、完整回归、完整性能或 VM／容器性能套件。最终 smoke 和真实 GDB 结果见 benchmarks/results/aot-data-1.0.8/；本轮不把单例体积变化表述为运行性能提升。
