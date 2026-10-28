# 1.0.11 同步调用借用验证

本轮仅验证外置 LLVM 的 Windows x64 开发构建；静态 LLVM 和交叉运行时尚未重建，不将此前版本的验证算作本轮结果。性能测量另存于 benchmarks/results，保留初始 malloc 基线。

编译器 SHA256：`7ed9644ca11446b2d0b6dbee20d3778238b8913f39a13f6826695681c02c7676`。

## 改动与边界

- AOT 同步原生调用借用执行器已有的调用器，集合状态直接转发 `NativeCallContext::invoke`，避免创建两个转发闭包。保留原有拥有式构造器，任意可变 C++ 调用器仍可取得独立捕获副本。
- 五个存储入口直接引用当前上下文；数组地址访问使用执行器自身建立的具体 Heap，移除重复 RTTI，继续经过原来的范围及存活验证。未修改公开 HeapAllocator ABI。
- 这不是语言层面的隐式移动实现；临时值、返回值和最后使用点的所有权优化仍列于 TASK.md。

## 核对入口

`tests/chali_native_invoker_borrow.cpp` 在断言开启时验证借用适配器和集合状态均不增加分配计数、借用不复制闭包、拥有式副本隔离可变状态以及失败诊断传播。使用 g++ C++17/O2/pthread 和实际 AOT core、records、system_arena 对象链接，无替代实现。

其余原始输出随本目录保存：完整 AOT 专项、嵌套同步 C ABI 回调、checked/unchecked 集合复制与返回，以及 malloc/pool 下的闭包、数组下标、图、树、线程通道、看门狗和自定义哈希碰撞模型。
