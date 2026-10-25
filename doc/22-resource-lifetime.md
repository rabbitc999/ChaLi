# 资源生命周期与借用控制流

<!-- LANGUAGE NAVIGATION -->
[English](en/22-resource-lifetime.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

## 无墓碑的分配身份

`source/stackvm/allocation_index.hpp` 保存活跃逻辑区间，以及每个曾使用物理块首的最新代次和偏移位宽。释放立即移除活跃区间，不保存墓碑或清扫边界。重复释放失败；物理内存复用不会复用逻辑身份，旧引用不能访问或释放新对象。代次耗尽时拒绝回绕，长期复用同一块首只需要一个身份槽。

## 资源构造与回收模块

显式释放统一调用 `value.drop()`，包括堆引用、数组及拥有资源的值；不提供 `delete` 或独立 `drop` 语句。普通类型自定义的同名方法仍按普通调用处理。

`source/semantics/resource_lifetime.cpp` 集中维护资源构造后的登记、Closeable/Drop 调用、聚合析构、拥有槽清理与作用域出口。显式终结、自动清理和移动使用一致的拥有状态；释放执行后清空拥有槽，分支上的释放不会从其他路径的清理清单中删除资源。宿主 close 只关闭操作系统资源；记录壳仍由其拥有值的 drop 回收，不将已释放记录保存为墓碑。

实现 `Closeable` 的结构体自动按不可复制处理（`MStructDef::nonCopyable`，与 `#NonCopyable opcode` 共用 `TypeNonCopyable` 判定）：复制一份就是两份所有权，`close()` 会跑两遍。复制初始化、赋值、按值传参和返回值复制全部拒绝并给出“实现 Closeable 或声明了 #NonCopyable”的诊断；转移用 `move x`，共用改传引用，返回函数内局部拥有值不需要额外 `move`。这条规则沿字段与元素传递：含不可复制字段的聚合整体不可复制，数组按元素复制、容器按值存放同类元素同样被拒（容器请存引用或托管值）。

`source/chali_resource_cleanup.cpp` 是独立编译的资源合并模块。它消费借用控制流的来源与初始化事实，按精确存储身份合并 drop：所有到达路径均已终结时移除后续操作，只有部分路径终结时保留执行路径守卫。重新初始化、重绑和分配更新证明条件。动态下标、来源不唯一或不透明的跨调用副作用不作为合并证明。运行时释放保持非幂等，无法合并的重复释放报告 CHL1205；普通读写仍检查释放后使用。

已命名聚合的显式 close 使用编译器局部关闭标志，实际关闭后更新标志，每次构造时重置；自动关闭和显式 drop 复用此标志，分支不会取消其他路径的清理。该标志保存在调用帧中，不增加运行时墓碑接口。

## 完整表达式临时所有权

打印、格式化和文件写入借用新建字符串时，编译器生成内部拥有型临时变量，先求值、再调用、最后在完整表达式结束时逆序清理。结果转移给命名变量、返回值或其他拥有临时变量时清空源槽，循环不会再次释放上一轮对象。字面量与空槽不产生额外拥有者；拼接链仍使用一次 join 分配。

## VM 与软重启

默认 VM 堆在 `source/stackvm/runtime_heap.cpp` 中首次适配空闲块、拆分余量并在释放时合并相邻空闲区间。watchdog 重启结束全部旧执行上下文，再重置 arena 和分配身份；没有清扫 API，也没有隐式垃圾回收。

## fn 闭包借用规则的参考与实现

参考 Rust 编译器 [upvar.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_typeck/src/upvar.rs) 的捕获分析流程，以及 [constraint_conversion.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/type_check/constraint_conversion.rs) 的寿命约束转换。ChaLi 使用自己的作用域来源事实与控制流检查器，不引入 Rust 的类型系统。0.15.0 默认捕获为 `const&`，显式 `mut` 使用可写借用；`copy` 和 `move` 持有内部状态。复制引用捕获的闭包不延长来源寿命。

捕获发现位于 `source/semantics/generic_inference.cpp::CheckLambdaCaptures`；地址存储在 `source/semantics/artifacts.cpp` 预登记，经 `source/semantics/boxing.cpp` 建立和恢复闭包环境；来源寿命经 `source/semantics/borrow_facts.cpp` 传播到赋值、返回与聚合字段。`LowerAssign` 必须先完成右值推导、发现 lambda 捕获，再记录保存约束。

异步事件回调可以被保存到全局登记表，因此单独要求静态来源；事件函数签名携带该契约，调用时拒绝借用局部环境的闭包，模块内部转送保留契约。含回调参数的事件登记入口不能转换成擦除该契约的普通函数指针。该限制不要求同步算法回调具有全局寿命。

## 借用检查与 Rust 源码对照

本轮参考 Rust 编译器的三个入口，采用适合 ChaLi 现有语义的规则：

| Rust 源码 | ChaLi 中的对应处理 |
| --- | --- |
| [move_paths/mod.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_mir_dataflow/src/move_paths/mod.rs)：记录存储位置、移动与初始化 | `Place` 使用变量存储身份加字段、下标和解引用路径；整体移动与局部字段移动分别影响对应路径 |
| [places_conflict.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/places_conflict.rs)：逐层比较投影的重叠关系 | 不同结构体字段和已知不同下标互不影响；动态下标保守视为可能重叠 |
| [dataflow.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/dataflow.rs)：借用与初始化状态的数据流分析 | 独立控制流图迭代到不动点；分支合并保留可能失效和借用来源的并集，同名重新定义要求所有到达路径均已完整移动 |

闭包建立时将捕获来源的读取和 move 捕获写入同一张控制流图。引用槽与其所指对象分别检查：移动引用变量使其槽失效；重新绑定只恢复该槽，不复活旧的已释放对象。copy/move 捕获的内部引用仍须通过寿命检查，拥有闭包不会延长外部被引用对象的寿命。

这是一套 ChaLi 自身的来源与移动分析，并未实现 Rust 的完整 NLL 区域推导、可变借用唯一性或 Polonius。引用资料用于规则对照，现有 ChaLi 允许的别名关系保持其语言契约。


## 闭包后续使用的来源检查

捕获发现和后续使用分别建模。`FlowState::captures` 保存函数值及聚合字段持有的外部借用来源；复制、赋值、元组和枚举载荷传递该集合，拥有捕获中的引用字段仍关联外部对象。调用函数值时读取其存储路径，并检查捕获来源是否在某条可达路径上已释放、移动或销毁。实参求值完成后再次检查闭包本身和捕获来源，避免 `callback(move source)` 在调用前使环境失效。返回函数值的调用保守继承接收者和实参已知的外部借用来源。重新赋值替换原有捕获集合，闭包最后一次使用后的销毁允许通过。

该处理参考 Rust 的 [borrow_set.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/borrow_set.rs) 对借用的显式登记、[upvar.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_typeck/src/upvar.rs) 对捕获方式的区分，以及 [places_conflict.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/places_conflict.rs) 的存储路径重叠分析。实现沿用 ChaLi 的字段路径和控制流检查器，不移植 Rust 源码。

检查覆盖当前函数内可见的来源失效与传播；不透明宿主代码、原始内存写入及被擦除的跨函数副作用仍需要对应接口的生命周期契约。此分析不等于消除了所有未定义行为。

## 显式生命周期契约 `#borrow` / `#static`

借用检查器默认保守：调用点把**全部**引用实参的来源都算进结果，于是 `first(&x, &local)` 的结果被当成也借用了 `&local`，返回它会被 CHL1204 拒掉。函数声明前写出契约，即可声明结果只借用哪些来源——等价于 Rust 把生命周期参数写在签名上：

| ChaLi | Rust |
| --- | --- |
| `#borrow(#ret, [a])` 的 `int64 mut& first(int64 mut& a, int64 mut& b)` | `fn first<'a, 'b>(a: &'a mut i64, b: &'b mut i64) -> &'a mut i64` |
| `#static(slot)` 的 `int64 mut& install(int64 mut& slot)` | `fn install(slot: &'static mut i64) -> &'static mut i64` |

三种写法，都写在函数声明之前，可连写多条：

- `#borrow(out, in)` —— `out` 借用 `in`；列表写法 `#borrow(out, [in1, in2, in3])` 表示借用其中每一个。
- `#static(a)` —— `a` 的来源按 `'static` 处理，可跨函数返回与长期保存；调用点必须真的传 `'static` 来源（全局变量）。
- `#borrow(#ret, [a, b])` —— `#ret` 是函数返回值，声明返回值借用 `a`、`b`。

名字只能是本函数的形参或局部变量，`#ret` 的来源只能是形参。契约带来两件事：一是调用点传播只按声明的那几个实参算来源（不再取全部引用实参的最大值），`first(&x, &local)` 的结果因此能够返回；二是契约本身成为义务——`#borrow(#ret, [a])` 的函数不能返回来自别处的引用，`#borrow(out, [a])` 的 `out` 不能保存来自别处的引用。

| 诊断 | 触发条件 |
| --- | --- |
| CHL1210 | 契约引用了不存在的名字；或 `#borrow(#ret, ...)` 的来源不是形参 |
| CHL1211 | `#borrow(#ret, ...)` 的函数返回了声明之外的来源 |
| CHL1212 | `#borrow(out, ...)` 的 `out` 保存了声明之外的来源 |
| CHL1213 | `#static(...)` 标的是局部变量 |
| CHL1214 | 调用点给 `#static` 形参传了非 `'static` 实参 |

本轮参考 Rust 的两处入口，采用适合 ChaLi 现有语义的规则：

| Rust 源码 | ChaLi 中的对应处理 |
| --- | --- |
| [wfcheck.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_analysis/src/check/wfcheck.rs)：签名上显式生命周期约束的良构性（`region_known_to_outlive` / `ty_known_to_outlive`） | 契约名字逐个核对作用域归属；`#static` 形参在调用点核对实参来源深度为 0 |
| [region_infer/mod.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/region_infer/mod.rs)：区域约束的求解 | 带 `#borrow(#ret, ...)` 的调用只按声明的实参传播来源，取代保守的"取最大值" |

实现沿用 ChaLi 自己的借用来源深度与借用流图，不移植 Rust 源码，也不实现 Rust 的完整区域推导与生命周期变体分析。
