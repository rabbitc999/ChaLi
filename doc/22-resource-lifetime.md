# 资源生命周期与借用控制流

<!-- LANGUAGE NAVIGATION -->
[English](en/22-resource-lifetime.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

## 无墓碑的分配身份

`source/ircode/allocation_index.hpp` 保存活跃逻辑区间，以及每个曾使用物理块首的最新代次和偏移位宽。释放立即移除活跃区间，不保存墓碑或清扫边界。重复释放失败；物理内存复用不会复用逻辑身份，旧引用不能访问或释放新对象。代次耗尽时拒绝回绕，长期复用同一块首只需要一个身份槽。

## 资源构造与回收模块

显式释放统一调用 `value.drop()`，包括堆引用、数组及拥有资源的值；不提供 `delete` 或独立 `drop` 语句。普通类型自定义的同名方法仍按普通调用处理。

`source/semantics/resource_lifetime.cpp` 集中维护资源构造后的登记、Closeable/Drop 调用、聚合析构、拥有槽清理与作用域出口。显式终结、自动清理和移动使用一致的拥有状态；释放执行后清空拥有槽，分支上的释放不会从其他路径的清理清单中删除资源。宿主 close 只关闭操作系统资源；记录壳仍由其拥有值的 drop 回收，不将已释放记录保存为墓碑。

实现 `Closeable` 的结构体自动按不可复制处理（`MStructDef::nonCopyable`，与 `#NonCopyable opcode` 共用 `TypeNonCopyable` 判定）：复制一份就是两份所有权，`close()` 会跑两遍。复制初始化、赋值、按值传参和返回值复制全部拒绝并给出“实现 Closeable 或声明了 #NonCopyable”的诊断；转移用 `move x`，共用改传引用，返回函数内局部拥有值不需要额外 `move`。这条规则沿字段与元素传递：含不可复制字段的聚合整体不可复制，数组按元素复制、容器按值存放同类元素同样被拒（容器请存引用或托管值）。

`source/chali_resource_cleanup.cpp` 是独立编译的资源合并模块。它消费借用控制流的来源与初始化事实，按精确存储身份合并 drop：所有到达路径均已终结时移除后续操作，只有部分路径终结时保留执行路径守卫。重新初始化、重绑和分配更新证明条件。动态下标、来源不唯一或不透明的跨调用副作用不作为合并证明。运行时释放保持非幂等，无法合并的重复释放报告 CHL1205；普通读写仍检查释放后使用。

已命名聚合的显式 close 使用编译器局部关闭标志，实际关闭后更新标志，每次构造时重置；自动关闭和显式 drop 复用此标志，分支不会取消其他路径的清理。该标志保存在调用帧中，不增加运行时墓碑接口。

经可变引用执行 `move` 时，失效的是所指拥有值；宏的 `mut &` 缓存与普通引用声明遵循相同规则。已知具体函数及其转发调用会把引用形参可能移动的路径传播到调用方，原拥有者与旧别名随后读取或再次移动均报告 CHL1205；分支合流保留可能失效状态。引用槽仍可重新绑定到另一个有效对象，重新声明拥有绑定则要求所有到达路径均已完整移动旧绑定。

`defer` 登记回调时检查捕获，随后在实际执行清理的正常退出、return、break 与 continue 路径上再次检查借用来源。来源在登记后被消费，不能在延迟回调中继续读取；未执行的登记及已经结束的内层清理不会把借用延长到外层。原生语言函数正常返回时恢复调用方的源码位置，嵌套 `close()` 不会把之后的运行时错误归到已经返回的清理函数。

`defer` 延迟执行后面的语句，正文默认借用所用变量。`defer f();` 会在退出时调用 `f`；`defer f;` 只延迟求值该表达式，不会立即复制并登记一个函数值。若退出前移动或销毁被借用的 `f`，必须拒绝。需要独立函数快照时，可先建立 `let saved = void fn copy[f](){f();};`，再写 `defer saved();`；快照所包含的外部引用仍必须有效。

## 完整表达式临时所有权

打印、格式化和文件写入借用新建字符串时，编译器生成内部拥有型临时变量，先求值、再调用、最后在完整表达式结束时逆序清理。结果转移给命名变量、返回值或其他拥有临时变量时清空源槽，循环不会再次释放上一轮对象。字面量与空槽不产生额外拥有者；拼接链仍使用一次 join 分配。

## 运行时与软重启

运行时堆由 `source/ircode/runtime_heap.cpp` 管理。系统 malloc 模式直接申请和归还有效载荷，pool 模式拆分空闲区间并在释放时合并相邻区间。`new` 创建的拥有值由普通作用域管理，释放立即撤销分配身份；没有隐式垃圾回收。

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

检查覆盖可见的来源失效与传播，并用具体函数摘要传播可解析的引用移动。不透明宿主代码、原始内存写入及被擦除的跨函数副作用仍需要对应接口的生命周期契约。此分析不等于消除了所有未定义行为。

## 显式生命周期契约 `#borrow` / `#static`

借用检查器默认保守：调用点把**全部**引用实参的来源都算进结果，于是 `first(&x, &local)` 的结果被当成也借用了 `&local`，返回它会被 CHL1204 拒掉。函数声明前写出契约，即可声明结果只借用哪些输入来源，具体约束以下文语义为准：

| ChaLi | Rust |
| --- | --- |
| `#borrow(#ret, [a])` 的 `int64 mut& first(int64 mut& a, int64 mut& b)` | `fn first<'a, 'b>(a: &'a mut i64, b: &'b mut i64) -> &'a mut i64` |
| `#static(slot)` 的 `int64 mut& install(int64 mut& slot)` | `fn install(slot: &'static mut i64) -> &'static mut i64` |

三种写法，都写在函数声明之前，可连写多条：

- `#borrow(a,b)` —— 短写，等价于 `#borrow(a,[b])`：目标 `a` 保存的引用只能来自 `b`。多个可能来源写成 `#borrow(a,[b,c])`。
- `#static(a)` —— `a` 的来源按 `'static` 处理，可跨函数返回与长期保存；调用点必须真的传 `'static` 来源（全局变量）。
- `#borrow(#ret,a)` —— 返回值只允许借用 `a`；`#borrow(#ret,[a,b])` 声明可能来自 `a` 或 `b`，调用点保留来源并集。

名字只能是本函数的形参或局部变量，`#ret` 的来源只能是形参。契约带来两件事：一是调用点传播只按声明的那几个实参算来源（不再取全部引用实参的最大值），`first(&x, &local)` 的结果因此能够返回；二是契约本身成为义务——`#borrow(#ret, [a])` 的函数不能返回来自别处的引用，`#borrow(out, [a])` 的 `out` 不能保存来自别处的引用。

### 契约的作用与边界

`#borrow(a,b)` 的第一个名字是保存引用的目标，不是返回值来源列表；返回值必须明确写 `#ret`。契约写在普通或泛型函数声明之前，可连续声明，单来源短写与方括号形式等价。初始化、重新绑定、结构字段、返回别名及分支／循环合流按已绑定存储的实际来源核对，不把别名的新名字误当成新来源，也不能把重新绑定后的参数仍冒充原始输入。

来源列表是“可能来源”的上界，不要求每条执行路径同时使用全部来源。多列一个未使用的引用实参，调用方仍须保守考虑它的寿命；需要更窄的返回寿命时，只声明真正可能返回的来源。全局与新分配的独立存储遵循原有来源规则；契约不会把函数的按值形参、局部槽或已经移动的对象变成长期存储。

保存借用与复制数值不同：对标量引用执行 `a=b` 是普通值写入，不能据此认定保存了 `b` 的引用；重新绑定用 `a=&b`。`#borrow(out,b)` 还会在调用点核对可保存引用的输出聚合：较短寿命的输入不能经调用写入寿命更长的对象。它不是互斥借用锁，不提供 Rust 式独占可变引用、线程同步或额外运行时所有权。

控制流分析保守合并各可达来源，不证明任意数值条件互补。需要所有分支切换到同一来源时，优先写明确的 `if/else`；单独的循环和后续条件可能仍保留初始来源。

泛型同样支持契约，包括自动推断类型参数的直接调用。例如 `#borrow(#ret,a)` 后声明 `T mut& pick<T>(T mut& a,T mut& b){return a;}`，调用 `pick(&x,&y)` 时无需显式写出 `<int64>`。具体函数体在实例化时核对来源；将返回值改为未声明的 `b` 必须被拒绝。尚未实例化的泛型声明不等于所有可能类型组合都已通过检查。

契约随函数声明跨模块生效，普通调用与泛型实例均按实际选中的声明检查。同名函数在不同模块声明不同来源时，限定调用分别保留各自契约，不合并为同一条规则；类型参数、接口约束和指令中的形参名也按声明模块解析。见[跨模块契约样例](../example/samples/borrow_contract_modules.chl)，以及错误样例中的 `borrow_contract_module_*`。

当前函数值类型不携带生命周期契约。因此，带 `#borrow`／`#static` 的函数必须直接调用，不能先取成函数值再绕过调用点要求；需要回调时可在闭包内直接调用，并接受普通捕获和实参检查。外部宿主函数的契约是受信任的边界，编译器无法验证宿主实现；`unsafe` 原始地址转换也不因此获得完整的静态别名证明。checked／unchecked 原生路径仍保留各自既有的存活、移动和资源协议检查，契约不等同于对任意宿主代码的内存安全保证。

可复现验证见 [借用契约压力测试](../tests/chali_borrow_contract_stress.py)、[引用移动与延迟清理对照](../tests/chali_borrow_move_defer.py) 和 [嵌套 Result 传播压力测试](../tests/chali_nested_result_stress.py)。覆盖单来源短写、合法和非法来源、泛型实例、长别名链、大形参表、重复契约、清理与多层错误传播；运行平台和执行范围按实际验收记录说明。

| 诊断 | 触发条件 |
| --- | --- |
| CHL1210 | 契约引用了不存在的名字；返回来源不是形参；或把带契约函数转换成不能携带契约的函数值 |
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

枚举自动包装仍服从载荷的复制能力：复制现有不可复制资源不能因省略构造器而变成隐式移动，含这类字段的枚举／结构体也不可复制。标准 `Option<T>`、`Result<T,E>` 的复制式 API 与传播协议在实例化时仍要求对应载荷可复制；不可复制资源可使用引用载荷，或使用明确移交所有权的自定义枚举。覆盖赋值应先求出新值，再清理旧值的活动载荷；资源计数回归覆盖普通变量、引用和字段。

链表等遍历可以先保存下一节点，再释放当前节点；这不等于在释放后继续读取当前节点。含释放的循环在分析图中分开首轮与后续迭代，额外节点预算按当前函数的源码规模限制，避免嵌套循环放大分析成本。静态检查不证明任意动态堆图无环，也不穷举未知字段引用的所有别名；checked 路径的存活检查仍是这类动态结构的运行时边界。拥有数组从值表达式返回时按移交／复制规则形成新的结果拥有者，元素包含的借用仍须满足来源与逃逸检查。

存储契约不仅检查赋值，也在实际求值路径检查 List／Set／Map 的引用元素写入、Any.set 和带显式 #borrow 输出契约的调用；接收者别名解析回原存储。Map 键的普通值复制不算借用写入。未声明契约的任意被调方容器内部副作用不做完整跨函数证明，宿主与擦除类型仍是受信任边界。含借用元素的数组经值表达式返回仍有保守精度限制，已验证的具名数组 return 替代写法及复现见[已知限制](34-compiler-safety.md)。
