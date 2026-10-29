# 数值 List 追加的融合原语

编译器 SHA256：e91d57cfe4097de0cd2ac37a0eeaa23679d4c5dba47c633e1fe7ecd1c6070c34。

对标准 __bootListPushI／F 且文本和拥有布局标志均为零的静态调用，LLVM 直接调用现有标量 ABI 的新增操作 18，一次边界完成列表身份／迭代借用检查、有界几何增长和单槽追加。容量足够不查询分配上限；不足时保持 max(required,cap*2)、min(8,maxCap) 和原上界诊断。先预留再封送数值，保持原错误顺序。底层仍是通用 NativeStoredValue 槽，紧凑 LLVM 存储尚未实现，不把减少边界等同于最终布局优化。

替代普通函数时保留原形参转换：F 的数值参数转为 double，I 的参数按 i64 传递并清除不透明浮点标记。扩展用例在修改前原型失败，自举回退通过；修正后八组全部通过，保留 probe 记录。CHALI_NO_AOT_NUMERIC_LIST_APPEND 关闭融合；文本／拥有值、未知标志、debug 和显式步数路径沿原实现。

两分配器 × 融合／自举 × 标量／旧 ABI，共八组 checked run 与 unchecked AOT 通过。覆盖 bool、char、int8／16／32／64、uint8／16／32／64、float32／64；同类型 Map／Set 回归；浮点负零、NaN、无穷、次正规位模式；整数到浮点转换和不透明槽标记；容量 33→66、清空复用、独立复制、String 拥有值回退、无效句柄及活跃迭代借用拒绝。保存函数 LLVM 摘录，确认 eligible 调用使用操作 18、关闭后回退。

最终版本的完整 AOT、八组容量／七类诊断、集合复制、同步回调和数值自举通过；运行时资产及编译器重建。源码仅后端及集合运行时，没有修改文法、类型或借用规则。实际平台 Windows x64；其他目标 SDK 与最终发行核对仍按 TASK.md 进行。

此目录不含正式性能计时；当前仍须实测是否快于 Lua。最终全量门禁未执行。

设计调研另已查阅 LLVM 官方 SmallString／StringRef 源码及 LangRef，后续字符串方案仍待实现和测量。在线文档显示 24.0.0git，仅作设计参考，不重标当前实际 LLVM 23.1.4 测量：
- https://llvm.org/doxygen/SmallString_8h_source.html
- https://llvm.org/doxygen/StringRef_8h_source.html
- https://llvm.org/docs/LangRef.html
