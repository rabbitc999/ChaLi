# 对象描述符标量读取入口

实际编译器 SHA256 bdcc1398f4ec24b9ff3254e1f3d7b468679fea950d5478827185d6152a3801bc。__std_obj_name / __std_obj_addr 直接调用 chali_host_object_scalar，仍使用 nativeObject 和 nativeRefreshViews，保留类别／存活检查、原诊断、memoryFault 优先、异常封装及安全点。CHALI_NO_AOT_OBJECT_SCALARS=1、关闭原语或显式步数限制退回旧通路。没有标记为纯函数或跳过视图刷新。

修改前同一新用例输出一致。malloc/pool × checked/unchecked × 开关八组通过；三个现有继承／视图所有权／对象组合样例两条路径通过；只读运行错误与未初始化引用编译拒绝在旧版、开启及关闭之间一致。完整 Any／泛型专项、完整 AOT 与同步回调专项通过。Any 专项仅在内存中将临时工作目录从 build/chali-any-generics 替换为 tmp/object-scalars-any，断言及源码未改。

两份 LLVM 模块均通过 LLVM O3；开启的单态接口工作模块有两处直接对象标量调用，关闭时零处。原始及优化 LLVM IR 保留；由 C++ 编译器和 LLVM 工具生成，Python 仅负责验证。性能在提交后另存，不重复参考语言。

verify-initial 日志记录测试预期错误：未初始化引用本来在编译期拒绝，初版脚本却预期运行期非空诊断；旧版确认后改为原有编译拒绝文案。初次失败未记作通过，修正后全组通过。
