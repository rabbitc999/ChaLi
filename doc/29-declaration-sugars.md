# 返回表达式、具名闭包与字段派生

<!-- LANGUAGE NAVIGATION -->
[English](en/29-declaration-sugars.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[文档索引](README.md) · [English](en/29-declaration-sugars.md)

权威产生式在 [`source/chali.gram`](../source/chali.gram)。ParseEngine 只负责由文法生成解析器；语义动作调用 ChaLi 自己的 AST 构造与降级函数。以下 IR 均指四元式。

## return 接控制表达式

```chali
String make() {
    return {
        String local = "owned";
        yield local;
    };
}
int32 choose(bool yes) { return if (yes) { 1 } else { 2 }; }
int64 count() { return loop { yield 3; }; }
```

`return` 仍接受普通表达式。块、`loop`、`if`、`match` 复用已有值出口；复杂分支使用 `yield value;`，单表达式分支已有的隐式值出口继续有效。返回控制表达式的所有正常完成路径必须给出值，分支最终类型必须完全一致，不能靠分支之间的数值提升或用户转换消除差异。整数字面量仍接受函数返回类型提供的上下文。

语法入口与 AST 脱糖：

```text
stmt := RETURN expr SEMI { M_ReturnValue(expr) }

Return(ControlExpr E)
  => Block(
       Declare hidden : FunctionResult = E,
       Return(hidden))
```

AST 阶段创建独立且不可被用户命名碰撞的结果绑定，声明组和控制表达式标记 `returnExpression`。`LowerLocalDecl` 把当前函数返回类型传入隐藏声明，`LoopValueType` 检查各分支的精确类型；控制表达式本身仍由 `LowerLoop` 分配单独的汇合槽。表达式只求值一次，先保存结果，再清理来源作用域，最后交给普通返回路径。

返回控制表达式内禁止嵌套 `return`、`break level N`、`break all`；会提前返回的 `?` 传播也要求改为显式匹配后 `break`。检查停在新的 `fn` 或具名函数边界，因此表达式内部声明的独立闭包可以拥有自己的 `return`。不会把内部函数的出口错误归给外层函数。


泛型函数使用同一条路径：`T take<T>(T value) { return { T local = value; yield local; }; }`。loop、if 和 match 同样可返回 T；单态化先把 T 替换为具体类型，之后按该类型建立隐藏槽、检查分支并执行移动或复制。泛型参数本身不会豁免分支类型一致、借用与清理约束。示例覆盖整数、String 及包含 String 的结构体。

## 编译期所有权与状态追踪

不增加 GC，也不把普通赋值改成移动。移动判定针对 `break` 实际交给哪个值出口，而不是仅看语法上最近的大括号。

| 信息 | 当前实现中的载体 | 用途 |
| --- | --- | --- |
| 唯一存储身份与字段投影 | `MVar.emitted`、`Place.root/path` | 区分同名重声明、字段、数组下标及引用别名 |
| 声明所属作用域 | `g_scope` 与 `MBlockContext.valueScopeDepth` | 判断变量位于当前接收结果表达式内部还是外部 |
| 拥有者与待清理项 | `g_ownedScopes` | 维护 Drop、Closeable、字段及延迟清理顺序 |
| 引用来源集合 | 借用控制流的 Bind/Allocate 与 `Fact` | 阻止结果、字段或闭包把局部引用带出作用域 |
| 移动与释放状态 | `FlowState.invalid` 的 Moved/Freed/Dropped | 检查移动后读取、写入、别名访问和重复销毁 |
| 分支与循环合流 | `checkFlow` 的不动点分析 | 任意到达路径上失效的存储都不能被后续路径无条件访问 |

`PrepareBreakValue` 对元组逐项处理。对拥有值的直接局部变量，在当前接收结果的表达式内部声明时生成编译器消费操作；外部变量保持复制，临时表达式由接收方接管。内部只读拥有绑定在退出处也可被编译器消费，这个内部标记不开放给源码；普通用户 `move` 仍要求 `T mut&`。引用不是被引用资源的拥有者，不能借此消费外部资源；所有引用结果继续经过来源寿命检查。

控制流伪代码：

```text
lowerBreak(value, receiver):
    for each leaf in tuple-or-value:
        binding = resolveDirectBinding(leaf)
        if binding owns value and declaredInside(binding, receiver):
            read binding exactly once
            transfer storage into receiver.joinSlot
            mark source Moved; clear source storage without Drop
        else:
            copy value into receiver.joinSlot
        check referenced sources outlive receiver
    cleanup scopes above receiver in reverse order
    jump receiver.exit

lowerReturnControl(E):
    hidden = evaluate E exactly once through its joinSlot
    transfer hidden to ordinary return slot
    cleanup remaining function scopes, retaining the returned owner
    return ordinary return slot
```

这里的 transfer/copy 是编译器伪操作，不新增 VM 操作码。已有 `LowerLanguageMove` 使用存活内存检查、地址读取、源槽清零及聚合移动标记；复制仍使用现有字符串、对象与结构体复制路径。Drop 保留在相应控制流上，由移动状态保护，不能在遇到一条移动分支时全局删除另一条分支的清理。清理回调若仍访问已消费的来源，必须由借用检查拒绝，不能靠悬垂别名访问搬走的资源。

核心代码见 [`M_ReturnValue`](../source/semantics/return_expression_ast.cpp)、[`PrepareBreakValue` 与 `LowerLanguageMove`](../source/semantics/boxing.cpp)、[`LowerLoop`](../source/semantics/lowering.cpp) 和[借用控制流](../source/chali_borrow_flow.hpp)。

运行时数组也沿用该规则：`return { String values[] = new[2]; ... yield values; };` 的内部数组连同长度头一起转移，源句柄清零，清理时跳过源元素。外部运行时数组则重新分配长度相同的存储，字符串、对象和聚合元素逐项深复制。隐藏结果槽接受带长度的块、if、match 与 loop 结果，不再只接受 new 或函数调用。此编译器内部消费不开放为源码中的 `move array`；定长数组仍按原有存储和返回类型约束处理，不透明宿主类型也不会被推断成拥有资源。

## 语句级具名函数

```chali
void main() {
    int64 value = 2;
    int64 read(int64 amount) { return value + amount; }
    int64 change(int64 amount) mut { value += amount; return value; }
    int64 snapshot() copy { return value; }
    int64 owned() move { return value; }
    int64 later(int64 amount);
    later = int64 fn(int64 amount) { return amount * 2; };
}
```

局部定义只加入 `decl_stmt`，不加入 `primary`、`unary` 或其他表达式产生式。`a + int64 f() { ... }` 因此是语法错误。

```text
decl_stmt := type name '(' param_list ')' block
           | type name '(' param_list ')' MUT block
           | type name '(' param_list ')' COPY block
           | type name '(' param_list ')' MOVE block
           | type name '(' param_list ')' ';'

NamedFunction(T, name, params, body, capture)
  => Declare name : FnType(T, params)
       = Lambda(T, params, body, capture)
```

返回类型是类型锚点，不从调用处猜测。默认捕获 const&；`mut` 显式允许修改借用来源，`copy` 建立独立状态，`move` 接管状态所有权。闭包构造结果由新绑定接管，后续普通函数值赋值仍遵守值复制规则。没有函数体的局部声明是函数值变量，可随后赋值；顶层 `T f(...);` 继续表示已有宿主函数声明。

闭包捕获与函数值生命周期复用已有 `fn` 实现。返回或保存一个仍借用局部变量的具名闭包不会延长来源寿命，必须编译报错。核心入口为 [`M_NamedFunction`](../source/semantics/named_function_ast.cpp)；没有另建一套闭包运行时。

## fn 前缀与返回类型后置

```chali
fn add(int64 left, int64 right) -> int64 { return left + right; }
fn main() { println("{}", add(3, 4)); }
```

声明可以省略返回类型写成 `fn 名(形参表)`，此时返回类型为 `void`；也可以把返回类型用 `->` 写在形参表之后。泛型、返回闭包与局部具名函数同样适用：

| 新写法 | 等价的既有写法 |
| --- | --- |
| `fn main() { }` | `void main() { }` |
| `fn calc(int64 x) -> int64 { }` | `int64 calc(int64 x) { }` |
| `fn calc<T>(T x) -> T { }` | `T calc<T>(T x) { }` |
| `fn make<T>(T x) -> (T fn(T v)) { }` | `(T fn(T v)) make<T>(T x) { }` |
| `fn make<T>(T x) -> fn(T v) -> T { }` | 与上一行相同，后置不加括号 |
| `fn make<T>(T x) -> (fn(T v) -> T) { }` | 与上两行相同，后置带括号，读起来是一个整体 |

```text
fn 名<类型形参>(形参) -> 返回类型  =>  返回类型 名<类型形参>(形参)
fn 名<类型形参>(形参)             =>  void 名<类型形参>(形参)
```

写法直接落在权威文法 [`chali.gram`](../source/chali.gram) 里，没有独立的文本预处理趟：`fn_result` 给出返回类型（省略 `->` 取 `void`），`fn_named` 描述具名函数——顶层引用它当函数，语句位置引用它落成具名闭包。四条约束：

1. 后置函数类型只写在返回类型位置，可加括号也可不加：`-> fn(T v) -> T` 与 `-> (fn(T v) -> T)` 等价；`fn` 后面直接跟 `(` 一律是 `fn (形参) { 体 }` 的 lambda。两者共用同一个前缀，单记号前瞻区分不开，文法上让给既有的 lambda。声明与变量类型位置仍写 `int64 fn(int64, int64)` 或 `(T fn(T v))`。
2. 后置写法的形参名会自动擦掉，`fn(T v) -> T` 与 `fn(T) -> T` 类型身份相同；默认值仍被拒绝。前置写法保持严格：函数类型的形参只写类型，`int64 fn(int64 v)` 会报"函数类型的形参只写类型"。
3. lambda 可推导返回类型。需要标注时，写成 `int64 fn(int64 x) { ... }`。
4. 具名函数在语句位置就是具名闭包：`fn read(int64 v) -> int64 { ... }` 在函数体内等价于 `int64 read(int64 v) { ... }`，默认只读借用外层变量。`fn` 后紧跟的关键字（`const` / `mut` / `copy` / `move`）一律当捕获方式，函数名只接受标识符。

核心入口为 [`fn_named` / `fn_result`](../source/chali.gram) 与 [`M_NamedFunctionOf`](../source/semantics/named_function_ast.cpp)。

## 成员宏自动实现接口

成员宏把接口添加到宿主类型，并生成接口方法。生成的签名由普通接口检查核对。

```chali
interface Count { int64 count(); }
$macro Count<T>(method:ident) -> member
$implement Count;
int64 method() {
    int64 result = 0;
    $for field in $fields<T>()
    result += 1;
    $end_for
    return result;
}
$end_macro
struct Record { int64 number; String text; $Count<Record>(count); }
void main() { Record value; Count counted = value; println("fields={}", counted.count()); }
```

```text
fields=2
```

## Closeable 与运算符推导开关

两个指令式约定：实现 `Closeable` 的结构体自动获得 `#NonCopyable`；声明前写 `#No<接口>Replace`
关掉该接口的自动推导。

```chali
struct Handle : Closeable { int64 id; void close() { println("close {}", self.id); } }

#NoAddReplace
#NoLessReplace
struct Vec : Add<int64, int64>, Less<bool, Vec> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
    bool leftLess(Vec const& other) { return self.v < other.v; }
}
```

```text
replace_directives := NO_REPLACE | replace_directives NO_REPLACE
top_decl := replace_directives struct_decl type_decl_end
NO_REPLACE := "#No" [A-Za-z][A-Za-z0-9]* "Replace"

NoOperatorReplace(interface, declaration) => declaration.noReplace += 接口名去掉 #No 与 Replace
```

`Closeable` 是资源关闭协议，复制一份就是两份所有权、`close()` 会跑两遍，因此凡是实现了它的
结构体一律按不可复制处理，与 `#NonCopyable opcode` 同一条检查路径：复制初始化、赋值、按值传参
与返回值复制都报错；`move x`、引用传参、返回局部拥有值照常。诊断文案会同时点出这两条来路
（“实现 Closeable 或声明了 #NonCopyable”）。

两类指令逐条登记：`#No<接口>Replace` 进 `noReplace` 集合，`#NoRight<接口>` 进 `noRight` 集合。
语义层核对三件事：接口名确实存在、是二元运算符接口、该接口本来就带有可取消的推导（前者要
求有换边 / 对偶 / 取反中的任意一条，后者要求存在右向写法）；写错、重复或对着没有推导的接口
写都直接报错。检查落在 `FindOperator` 的推导分支上：

```text
FindOperator(op, left, right):
    direct match               -> 直接用（指令不影响直接写法）
    right-direction (RightXxx) -> 照旧
    symmetry swap (交换律)      -> 仅当接收者未被 #No<接口>Replace 关闭
    duality / negation         -> 仅当 via 接口未被 #No<接口>Replace 关闭
```

因此 `#NoAddReplace` 与 `#NoRightAdd` 分工明确：后者只堵右向那一条，前者关掉这个接口的全部
推导，但直接写出的左向 / 右向调用都不受影响。两者都是**编译器指令**（写在声明前、不进继承
列表）。泛型模板收集时保存这些指令，实例化时把它们加回具体声明之前。



## 生命周期契约指令

函数声明前可以连写生命周期契约。`#borrow(out, in)` 表示 `out` 借用 `in`，列表写法
`#borrow(out, [in1, in2, in3])` 表示借用其中每一个；`#ret` 指返回值，`#borrow(#ret, [a, b])`
与 Rust 的 `fn f<'a, 'b>(a: &'a T, b: &'b T) -> &'a T` 同形；`#static(a)` 把 a 的来源按
`'static` 处理。文法：

```text
lifetime_directive := "#borrow" "(" lifetime_target "," lifetime_name ")"
                    | "#borrow" "(" lifetime_target "," "[" lifetime_names "]" ")"
                    | "#static" "(" lifetime_name ")"
lifetime_target := 标识符 | "#ret"
lifetime_names := 标识符 { "," 标识符 }
top_decl := lifetime_directives func_def
          | lifetime_directives "inline" func_def
          | lifetime_directives "export" func_def
```

指令随函数声明一起登记，早于函数体分析——返回检查在函数体里做，调用点传播要在别的函数体里
查这张表。语义层核对四件事：名字确实存在（是本函数的形参或局部变量）、`#ret` 的来源只能是
形参、`#static` 标的不能是局部变量、调用点给 `#static` 形参传的实参来源深度为 0。剩下的两条
是义务：声明了 `#borrow(#ret, [...])` 的函数，每条 `return` 的借用根必须落在声明来源内；
声明了 `#borrow(out, [...])` 的 `out`，写入它的引用必须来自声明来源。

没有契约时调用点保守取全部引用实参来源的最大值，因此 `first(&x, &local)` 的结果会被当成
也借用了 `&local`；写了 `#borrow(#ret, [a])` 之后只按 a 传播，`outer` 就能把它返回。

- [生命周期契约](../example/samples/lifetime_contract.chl)；报错例 [`lifetime_return_source.chl`](../example/samples-error/lifetime_return_source.chl)、[`lifetime_unknown_name.chl`](../example/samples-error/lifetime_unknown_name.chl)、[`lifetime_static_argument.chl`](../example/samples-error/lifetime_static_argument.chl)、[`lifetime_store_source.chl`](../example/samples-error/lifetime_store_source.chl)、[`lifetime_static_local.chl`](../example/samples-error/lifetime_static_local.chl)。
- 详见[资源生命周期与借用控制流](22-resource-lifetime.md)。


- [返回表达式与资源清理](../example/samples/return_expression.chl)、[具名闭包](../example/samples/named_function.chl)、[结构体和枚举派生](../example/samples/derive.chl)。
- 运算符推导开关：[`no_replace.chl`](../example/samples/no_replace.chl) 与报错例 [`operator_no_add_replace.chl`](../example/samples-error/operator_no_add_replace.chl)；Closeable 自动不可复制：[`closeable_move.chl`](../example/samples/closeable_move.chl) 与报错例 [`closeable_copy.chl`](../example/samples-error/closeable_copy.chl)。
- [语义及生命周期门禁](../tests/chali_declaration_sugars.py)检查分支类型、嵌套返回、层级跳转、只读捕获、引用逃逸、表达式位置声明、Drop 派生和缺失字段协议。
