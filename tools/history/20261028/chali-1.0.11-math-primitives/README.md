# 数学函数 LLVM 原语验证

本轮 Windows x64 外置 LLVM 编译器 SHA256：`79a3762111f3569f151a0346b944406edda560425aff7b155e5655932419d98e`。源码为 C++，LLVM 模块由编译器生成，验证过程中的 Python 只调用产物和检查文本，没有生成 LLVM IR；临时验证输入与产物清理后不保留验证脚本。

`std::math` 的 `__sqrt64`、`__floor64`、`__ceil64`、`__round64`、`__trunc64` 分别生成对应 LLVM 内建；`__fmod64` 使用 frem；`__isNaN64` 使用无序浮点比较；`__isInf64` 使用绝对值位模式判定。没有启用 fast-math。原有 memcpy／memset 已使用 LLVM 内建，本次没有把已有路径重复计作新增。

语义依据：[LLVM 数学内建](https://llvm.org/docs/LangRef.html#standard-c-c-library-intrinsics)、[frem](https://llvm.org/docs/LangRef.html#frem-instruction)。采用语言数值接口的默认浮点环境；LLVM 数学原语不提供 C errno 接口。需要看门狗或显式设置 `CHALI_NO_AOT_RAW_INTRINSICS=1` 时继续使用既有原生边界。

fixture 为 `tests/fixtures/aot_math_primitives.chl`。八组分别覆盖 malloc／pool、checked／unchecked、原语开启／关闭，全部退出零且输出相同；另与修改前 `7bc6c40` 编译器输出比较一致。用动态数组实参验证有限值、正负零、舍入中点、无穷、安静及信号 NaN、最大有限值、最小次正规数和 float32 包装。NaN 比较以类别核对，不约定 NaN 载荷位模式。

validation.json 与原始 stdout 保存全部八组；llvm-primitives.txt 保存生成模块中的原语调用证据。完整 `tests/chali_aot.py` 专项亦通过，原始日志见 math-aot-suite.log。本轮不声称新增数学操作已完成独立性能测量，也不将此前七项成绩重标为本次成绩；静态 LLVM 与交叉运行时不在本轮验证范围。
