有界增长计算在 ChaLi 实现，List 接入；八组配置和五个错误诊断通过。完整 AOT、集合传参／返回／深复制验证通过。首次测试错误地使用 List.opcode，后改为已有 address 取得原始槽；第二次错误地将语言引用传入 barePointer，最终采用正确 address。引用不可直接用固定 C ABI 导出，IR 检查采用普通语言函数。所有初始失败保留，未放宽诊断或安全规则。

LLVM O3 地址函数正常路径无调用，仅空引用失败路径 chali_panic；核对记录见 buffer-address-verification.json。List 上限使用容器捕获分配器预算／实际槽大小及 vector max_size；仍可能因其他存活分配导致申请失败。String／Map／Set 缓冲区和最终性能验收尚未完成。
