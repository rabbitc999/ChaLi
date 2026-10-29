# 立即消费 String 字节 Option 的标量路径

标准 String 的 byteAt(i).unwrap() 在稳定变量／字面量接收者及变量／整数 int64 下标条件下，直接生成对 ChaLi 标量内核的调用。精确核对标准 String、Option<uint8> 载荷；不改变保存的 Option、自定义 byteAt、带调用的接收者／下标或 debug／step 行为。关闭开关为 CHALI_NO_STRING_BYTE_UNWRAP。

有效路径先保留字符串长度与边界检查，随后读取字节；越界保持 called Option.unwrap() on None。没有新增宿主胶水、四元式优化或 Python LLVM IR 生成。Python 只执行验证并摘录 clang 实际优化的函数。

四组分配器／开关各自通过 checked run 与默认 unchecked AOT，覆盖 UTF-8、保存 Option、自定义方法、引用、独立复制、求值次数；负下标、末尾、空字符串在两种执行形态共 24 个错误验证通过。

byte-unwrap-llvm-comparison.json 仅统计 LLVM O3 探测函数本身：直接宿主分配调用 1→0，静态调用点 22→12，不包含被调用函数，不等同于全程序动态调用数或性能计时。

初次测试错误地给 String const& 参数添加 C ABI export，被现有规则正确拒绝；移除多余导出标记后重新通过。初次统计匹配了不存在的 chali_alloc 名称，已改为实际 chali_host_allocate，并保存真实函数摘录供核对；不将错误统计当作证据。

最终全量门禁及最终性能目标尚未完成，本轮提交后仅正式测 ChaLi 字符串；其他语言基线不执行。

完整 AOT、String 缓冲区八组配置、String 自举、资源生命周期 24 项、借用流 45 拒绝／41 允许及多诊断通过。API 目录 43 模块／1230 重载、47 页链接和 CI shell 语法检查通过。借用测试原有 build 工作路径改为根 tmp；本轮旧位置的源码已归档并清理。
