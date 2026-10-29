# 数值 List 最终槽直接构造

数值追加原语已完成句柄、借用、布局和容量检查后，直接在最终元素槽 emplace_back，再写入原始整数、浮点标记和 double 值。原路径先构造 NativeStoredValue 临时快照，再将其中字符串和字段容器移动到最终槽；本次删除这一临时快照，不改变现有宽槽布局。

仅应用于已经排除文本与拥有值的数值追加入口，所有基础类型沿原实参转换，String 和拥有型值继续走原路径。原 nativeValue 在 false/0 布局下只写入这些标量字段，没有可能失败的资源操作；先完成容量预留再构造，保留原错误顺序。

实际验证产物：chali-1.0.11-llvm-external-windows-x64.exe，SHA256 b259ebe19a3d16762f0011676eeaa127a729feb96a85376e4b73206b62d9802c。

- make all -j6：重建记录库、内嵌资产和编译器，通过。
- tests/chali_numeric_list_append.py：malloc/pool × 直接/自举回退 × 标量/通用返回 ABI，8 种模式；40 条执行记录包含预期失败的失效句柄和活跃借用诊断。全基础类型、Map/Set 配套、浮点位模式、容量和复制验证通过。
- tests/chali_buffer_growth.py：8 种配置和 7 项容量诊断通过。
- tests/chali_collection_glue.py：集合传参、返回值、回调及复制验证通过。

同目录保留原始日志、矩阵源码和结果。尚未将减少临时构造视为已测性能收益；最终全量门禁尚未执行。
