# 当前 O2 参考构建检查

复用四项旧 O2 原始样本：int_chain、float_chain、call、array，来源 aot-1.0.8/results.json。对应 C++ 源码从初始 Git 快照至今没有变化，校验值、宿主及工具主版本一致。旧数据没有当时源码摘要，报告明确 measured_source_sha256=null，不将当前或初始摘要冒充历史实测摘要。

其余九个当前负载仅以 O2 构建，未执行任何参考程序；build-only 日志与二进制摘要在此保留。正式首次测量这些缺少配置另存，已有四项 O2、全部 O3、Rust、Java、Lua 均不重新评测。
