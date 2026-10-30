# 字面量定型、运算符推导与宏

<!-- LANGUAGE NAVIGATION -->
[English](en/25-literals-operators-macros.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回文档目录](README.md) · [字面量定型、运算符推导与宏](25-literals-operators-macros.md)

本文介绍：整数字面量按上下文定型、运算符重载会自己推导（交换律 / 比较对偶 / 右向开关 / 多个返回类型），以及宏 DSL 的单一写法。前两项让你少写强转和右接口，第三项让宏只有一套写法、报错面减半。


## 整数字面量按上下文定型

字面量不再一律按 `int64` 走：赋值 / 初始化 / 实参 / `return` 的目标类型会传进表达式；二元算术与位运算按另一侧操作数的类型定型。比较与逻辑不参与这层定型，没有上下文时仍是默认 `int64`。

```chali
int32 twice(int32 value) { return value * 2; }
void main() {
    int32 a = 1;
    int32 b = a + 1;                 /* 1 按 int32 定，不必写 (int32)1 */
    int32 sum = 1 + 2;               /* 两边都是字面量：按 int32 槽位定型 */
    println("{} {} {}", a, b, sum);
}
```

```text
1 2 3
```

十进制字面量超出目标类型范围就退回默认类型，由既有的「缩窄要强转」报错，**不会静默截断**：

```chali
void main() {
    int8 x = 200;
    println("{}", x);
}
```

```text
chali: 整数字面量 200 超出 int8 的取值范围（-128..127）
```

十六进制 / 二进制照旧按位模式看待，`int8 x = 0xFF;` 不受影响。

## 运算符重载：交换律与比较对偶

`+` `*` `==` `!=` `&` `|` `^` 满足交换律：类型只声明了左接口时，右操作数那侧也能用，编译器自动交换两个操作数。`-` `/` `%`、大小比较和移位不交换，反向仍须显式声明右接口。

只声明 `Less` 时，`5 > a`（对偶）、`a >= 5`（取反）、`5 <= a`（对偶 + 取反）都推得出来；`a > 5` 推不出来——那要 `int64 < T`，得给类型补 `Greater`。

```chali
struct Vec : Add<int64, int64>, Less<bool, int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
    bool leftLess(int64 other) { return self.v < other; }
};
void main() {
    Vec a;
    a.v = 10;
    println("{} {}", a + 5, 5 + a);     /* 交换律 */
    println("{} {}", a < 5, 5 > a);     /* 对偶 */
    println("{} {}", a >= 5, 5 <= a);   /* 取反 / 对偶 + 取反 */
}
```

```text
15 15
0 0
1 1
```

`&&` / `||` 是内建短路运算符、**不参与重载**。报错不再落到「找不到 `Add` / `RightAdd`」的通用文案，而是直接说明它们不可重载——短路要先决定左边，与运算符重载「先求两个操作数再调方法」的模型对不上。

## 右向开关：`RightXxx` / `#NoRightXxx`

交换律与比较对偶都要把右操作数借来当接收者，所以类型可以表态：

- 实现 `RightAdd` → 右向归它自己，`5 + m` 只认 `rightAdd`，形参接不住就报错、不再回落到 `Add`；
- 在 `struct` 前写编译器指令 `#NoRightAdd`（`#NoRight` + 左接口名，无类型实参）→ 禁止右向，`5 + f` 直接报错，左向 `f + 5` 不受影响；
- `#NoRightXxx` 优先于 `RightXxx`，显式声明的右接口优先于交换律推断。

```chali
struct Money : Add<int64, int64>, RightAdd<int64, int64> {
    int64 cents;
    int64 leftAdd(int64 other) { return self.cents + other; }    /* m + 5 */
    int64 rightAdd(int64 other) { return other - self.cents; }   /* 5 + m */
};
#NoRightAdd
struct Forbid : Add<int64, int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
};
void main() {
    Money m;
    m.cents = 120;
    println("{} {}", m + 5, 5 + m);
    Forbid f;
    f.v = 7;
    println("{}", f + 5);
}
```

```text
125 -115
12
```

把 `f + 5` 写成 `5 + f`：

```text
chali: 右向被编译器指令关闭：Forbid 声明了 #NoRightAdd，int64 + Forbid 不再推导；要去掉该指令，或改为实现 RightAdd<R,T>
```

## 同一对操作数、多个返回类型

`Add<int64,int64>` 与 `Add<Vec,int64>` 可以共存——两个 `leftAdd(int64)` 只在返回类型上不同，这是普通方法重载做不到的事。挑哪条由表达式的**目标类型**定：

```chali
struct Vec { int64 v; };
struct A : Add<int64,int64>, Add<Vec,int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
    Vec leftAdd(int64 other) {
        Vec r;
        r.v = self.v * other;
        return r;
    }
};
void main() {
    A a;
    a.v = 5;
    int64 x = a + 3;                 /* 返回 int64 的那条：5 + 3 */
    Vec y = a + 3;                   /* 返回 Vec 的那条：5 * 3 */
    println("{} {}", x, y.v);
}
```

```text
8 15
```

没有目标类型（`let z = a + 3;`）会报错并列出候选返回类型：

```text
chali: 运算符 + 有多条声明匹配同一对操作数 A / int64，返回类型分别是 int64、Vec（先强转再算；若它们只是返回类型不同，给表达式一个目标类型就能定）
```

**换边不改变求值顺序**：`bump() + makeA()` 变成 `makeA().leftAdd(bump())` 时，两个操作数仍按源码从左到右求值（左操作数先钳进临时量），副作用顺序与不换边时一致。

## 编译期卫生 AST 宏

`$macro` 定义由生成解析器建立语法树，语义分析按依赖展开，类型推导与展开交错进行，四元式生成前完成。宏不把替换结果拼成源码重新解析。参数保留原文、位置、语法候选及调用处绑定；引入的普通局部声明自动卫生，辅助名称在定义模块查找。

```chali
$macro SQUARE(x)
x * x
$end_macro
$macro ANSWER
42
$end_macro
$macro EMPTY()
42
$end_macro
void main() {
    int64 temporary = 10;
    println("{} {} {}", $SQUARE(1 + 2), $ANSWER, $EMPTY());
}
```

表达式实参按从左到右的顺序各求值一次，再由宏体复用保存的值；`$SQUARE(1 + 2)` 得到 9，`$SQUARE(next())` 只调用一次 next，两个独立实参中的 next 则各调用一次。普通参数按值绑定，宏内修改只影响副本，不产生警告。编译器仅在不改变求值次数、别名、所有权与可观察行为时消除复制。语句宏调用仍写分号；名称、类型、语句等语法参数保留相应的结构和来源。

## 参数组、变长片段及卫生绑定

需要写回调用方时显式声明可变引用；只读引用禁止修改来源：

```chali
$macro BUMP(n)
n = n + 1;
$end_macro
$macro BUMP_REF(mut &n)
n = n + 1;
$end_macro
$macro DOUBLE_READ(&n)
n + n
$end_macro
void main() {
    int64 x = 7;
    $BUMP(x);         // x 仍为 7，修改副本，不警告
    $BUMP_REF(x);     // x 为 8
    assert($DOUBLE_READ(x) == 16);
}
```

引用参数仍受可写性、别名和生命周期规则约束；不可通过宏绕过只读引用检查。

普通参数从实参推导类型，整数字面量缺少其他约束时为 int64；需要其他位宽时传入相应类型的值，例如 `$SQUARE((int32)3)`。副本中已有的引用仍遵守原借用关系。

裸定义 `$macro A` 只能用 `$A`；空组定义 `$macro A()` 只能用 `$A()`。`[]`、`()`、`{}` 按声明顺序严格匹配，允许重复组。每组支持固定形参和一个末尾变长形参 `...rest` 或 `..rest`，空捕获合法。固定形态优先于适用的变长形态。

```chali
$macro ACCUMULATE(mut &out)[...values]
$for value in values
int64 temporary = value;
out += temporary;
$end_for
$end_macro
void main() {
    int64 temporary = 100;
    int64 total = 0;
    $ACCUMULATE(total)[1, 2, 3];
    println("{} {}", total, temporary);
}
```

结果是 `6 100`。每轮展开中的局部声明互相独立，实参 `out` 保留调用者的绑定。变长片段可转发到另一宏，或在函数参数列表中逐项展开；`$for` 可遍历变长形参，也可在宏内静态遍历 `$fields<T>()`。形参出现在声明名位置时必须传入单个标识符，该声明可供后续调用者代码使用。成员名与变量名按各自语法角色处理。

## 显式参数类别

固定形参与变长形参都可以显式声明类别：

| 类别 | 约束与展开行为 |
| --- | --- |
| `x: value` | 完整表达式；进入展开体前求值一次，保存并复用值副本 |
| `x: expr` | 完整表达式语法片段；在每个放置位置求值，未放置则不执行 |
| `name: ident` | 单个标识符，可用作生成声明或成员的名称 |
| `T: type` | 完整类型片段，用于类型位置 |
| `body: stmt` | 完整语句片段，用于语句位置 |
| `body: block` | 完整块片段，可按已有块语义用于语句或表达式位置 |
| `p: pattern` | match 模式片段，只能放入匹配模式位置，支持载荷与顶层 `|` 备选 |

value 参数先按声明顺序求值，再执行展开体；expr 参数的求值位置由展开体决定。expr 是显式语法替换，作为赋值左侧时可以修改调用者变量。未标注形参保持原有按角色推断的兼容规则；普通表达式参数仍单次求值。`&n: value` 和 `mut &n: value` 保留只读／可写借用，未标注引用类别的旧写法也继续有效。引用不能标注为 expr 等语法片段类别。

```chali
int64 count = 0;
int64 next() { count += 1; return count; }
$macro TwiceValue(x: value)
x + x
$end_macro
$macro TwiceExpr(x: expr)
x + x
$end_macro
void main() {
    int64 value = $TwiceValue(next());
    println("{} {}", value, count);
    count = 0;
    int64 expression = $TwiceExpr(next());
    println("{} {}", expression, count);
}
```

```text
2 1
3 2
```

类别不匹配在展开前报错，value／expr 参数不能转作声明名称。`...xs: value` 中每个实参在进入展开体前按源码顺序求值一次，即使未使用也执行；嵌套重复与转发复用这些值。`...xs: expr` 按实际插入位置执行。变参不能标为引用；未标类别的旧变参保留语法片段行为。item 是宏结果类别，不是参数类别。

## 宏泛型

尖括号仅表示宏类型参数，不能作为任意文本参数组：

```chali
$macro CONVERT<T:value>(value)
cast<T>(value)
$end_macro
$macro RESULT<T>(value)
cast<T>(value)
$end_macro
void main() {
    int32 source = 42;
    let same = $CONVERT(source);
    int32 expected = $RESULT(42);
    int64 explicitValue = $RESULT<int64>(42);
}
```

`T:value` 只从指定实参的静态类型推导；未指定来源的 `T` 只从外层期望结果类型推导。显式类型优先，推导阶段不执行参数、不自动转换、不改换推导来源；展开后的普通值参数仍各求值一次，显式 expr 参数按放置位置执行。`let x = $RESULT(42)` 缺少期望类型，编译失败。首版不支持宏泛型值参数、特化、偏特化或任意函数的编译期执行。

`#res` 显式表示从调用位置的预期结果类型推导，可与实参来源组合：

```chali
$macro Example<T:#res, U:b>(a, b)
{
    T left = cast<T>(a);
    U right = b;
    yield left + cast<T>(right);
}
$end_macro
void main() {
    int32 result = $Example((int64)3, (int16)4);
    println("{}", result);
}
```

这里 T 是 int32，U 是 int16，结果是 7。初始化、赋值、函数返回和可确定参数类型的函数实参均可提供结果上下文；推导不执行 a 或 b。`<T>` 保留相同的隐式结果来源语义。显式类型实参优先；缺少结果上下文时使用 `let` 不会自动改从 a 推导。

## 五类完整语法匹配

```chali
$macro CLASSIFY(value)
$match value is ident
1
$else is expr
2
$else is type
3
$else is block
4
$else is stmt
5
$else
0
$end_match
$end_macro
```

只支持 `expr`、`ident`、`type`、`block`、`stmt` 五类，使用同一生成解析器完整消费原始片段。按书写顺序选择首个成立分支；`ident` 也可能是 `expr`，因此顺序有意义。语法成立不代表名称存在或类型合法；只有最终展开的节点进入名称、类型、捕获、借用和资源检查。无匹配且无兜底不生成内容。未选分支的模板结构仍须合法。大括号参数组不等于块片段：传入一个完整块写 `$A{{ statements; }}`。

## 正则、原文与来源

```chali
$macro DECIMAL(value)
$regex value is "[0-9]+"
value
$else
0
$end_regex
$end_macro
```

`$regex` 使用 C++ `std::regex` 的 ECMAScript 语法对去除首尾空白的完整参数原文匹配，保留内部注释、引号和换行。字符串按语言规则解码一次，合法与非法模式均缓存；非法模式报编译错误。正则捕获组仅用于匹配，不导出捕获变量。

`$stringify(value)` 返回实参原文的字符串，不先执行或展开参数。`$concat(left, right)` 只生成单个标识符，不能生成任意代码；模板中生成的名字保持定义处上下文，不能借拼名捕获调用者局部变量。直接源码中的拼名按该源码上下文查找。`$line`、`$file`、`$module` 可直接使用；模板内取调用处来源，嵌套参数转发保留原始来源。

`$replace ADD(a, b) a + b` 是同一 AST 机制的单行定义。宏递归与规模有预算，失败诊断保留调用、定义和参数来源链。编辑器宏预览按宏调用逐张生成卫生 AST 展开图，每张图都是自包含页面，可直接放进网页；类型提示读取最终静态类型，不显示内部绑定编号。

### 跨模块调用与限定名

宏定义登记在**定义它的模块**里，调用按模块可见性解析：

- 未限定写法（`$assert(...)`）：先查调用所在模块，再查 `import` 进来的模块；`std::lang`
  始终可见，所以标准库提供的宏不用 import。可见范围内出现多个同名定义时报错。
- 限定写法（`$std::lang::assert(...)`）：直接点名「模块 + 宏名」，模块路径从最长前缀往下试。
  名字被同名的函数 / 变量挡住，或者想明确指定用哪个模块的宏时用这种写法。限定调用同样
  受可见性约束：`std::lang` 始终可见，其余模块必须先 `import`。
- 两种方式展开出的 AST 完全一致，`$line` / `$file` / `$module` 取的都是调用点。

```chali
module qualified;

void main() {
    int64 n = 3;
    $std::lang::assert(n > 0);              /* 限定名：直接点 std::lang 的宏 */
    $std::lang::assert(n < 10, "n 得是个位数");
}
```

限定名找不到对应定义时报「未定义宏」，位置指向调用点。

回归见 [宏 AST](../tests/chali_ast_macros.py)、[分组与来源](../tests/chali_macro_groups.py)、[静态类型](../tests/chali_macro_generics.py) 和 [语法及正则](../tests/chali_macro_branches.py)。

## 显式结果类别与模块声明宏

宏头可以在参数组后写 `-> expr`、`-> stmt` 或 `-> item`。未标注的旧宏保持原有展开规则。`expr` 需要表达式结果；`stmt` 生成语句，可在函数体中作为独立调用；二者均不能用于模块声明位置。`item` 在模块声明位置调用，调用后写分号，生成函数、结构体、枚举或接口，也可以调用另一个声明宏。

```chali
$macro RECORD<T>(name:ident) -> item
struct name {T value;}
$end_macro

$macro ANSWER(name:ident) -> item
int64 name(){return 42;}
$end_macro

$RECORD<int64>(Record);
$ANSWER(answer);
void main(){Record record;record.value=answer();assert(record.value==42);}
```

声明宏的类型实参须显式提供，形参须标注语法类别，不能使用运行期 value 或引用形参。类型参数替换到生成的字段、形参和返回类型中；`<T:arg>`、`<T:#res>` 在原有表达式宏中继续按既有规则推导。公开名称通过 ident 实参传入；模板内部声明的辅助函数和类型获得独立名称，重复调用不共享这些辅助声明。生成函数可正常重载，同签名冲突报错；生成类型接受普通名称、类型、借用和资源检查。声明宏不能生成 module、import 或新的宏定义，递归声明展开报告来源链。

相关回归见 [声明宏测试](../tests/chali_macro_declarations.py)。

## 编译期字段元数据

`$fields<T>()` 查询具体普通结构体的字段，按源码声明顺序排列，排除 static 字段和方法。查询不会建立运行期反射表，也不要求导入 std::reflect。下标需要非负整数常量；为区分宏的 `[]` 实参组，字段下标写成 `($fields<T>())[index]`。

```chali
struct Item{int64 code;String name;}
void main(){
    assert($fields<Item>().count==2);
    assert(($fields<Item>())[0].name=="code");
    assert(($fields<Item>())[1].type.name=="std::lang::String");
}
```

| 描述符 | 可查询内容 |
| --- | --- |
| `$fields<T>()` | `count`、限定 `name`、类型 `id`，以及常量下标 |
| 字段 | `name`、从零开始的 `index`、字段 `id`、`type`、`attributes` |
| 字段或注解实参的 `type` | 可读类型 `name`、类型 `id`；普通结构体还可查询 `fields` |
| `attributes` | `count`、常量下标 |
| 单个注解 | 限定 `name`、注解声明 `id`、`arguments` |
| `arguments` | `count`、按注解形参声明顺序排列的常量下标 |
| 单个注解实参 | `name`、精确 `type`、已绑定的常量 `value` |

泛型实例返回替换后的字段类型；字段的整数位宽、符号、引用权限、数组维度和容器参数均保留。`($fields<Outer>())[0].type.fields` 可继续查询嵌套结构体。类型身份使用限定声明及具体类型参数，字段身份同时包含所属类型和字段名；身份不依赖机器地址或声明在文件中的偶然顺序。`id` 是不透明的 uint64，不能当作地址、布局偏移或跨版本 ABI 编号使用。

注解沿用语言的语义检查与开关，使用注解的程序可显式加 `--reflection`。例如 `@annotation Label(text:String,limit:int16=(int16)7)` 定义后，字段上的 `@Label("code")` 可通过 `.attributes[0].arguments` 读取显式参数和默认值；`value` 保留 int16 等精确类型，不作隐式扩大。

字段集合、字段、类型、注解及实参描述符只能用于编译期属性查询，不能保存到普通变量、传给运行期函数或转换成运行期对象。查询得到的名称、数量、身份或注解标量常量可按各自普通类型使用。当前字段入口不接受枚举、接口或 opcode 对象，动态下标和 `.offset` 等布局查询也会报错。

回归见 [字段元数据测试](../tests/chali_macro_metadata.py)。

## 模式参数与编译期重复

`p: pattern` 接收完整的 match 模式。模式中的载荷名字属于该分支，调用方传入的块可以使用这些名字；宏定义中的自由名字继续从定义环境解析。

```chali
$macro SELECT(value: value, p: pattern, body: block) -> expr
match(value) { p => { yield body; } _ => { 0 } }
$end_macro
enum Box { Value(int64 value), Empty }
void main() {
    Box box = Box::Value(7);
    int64 result = $SELECT(box, Box::Value(x), { yield x + 1; });
    assert(result == 8);
    assert($SELECT(2, 1 | 2, { yield 9; }) == 9);
}
```

顶层 `|` 生成共享正文的普通备选分支，仍检查各备选的载荷绑定是否一致。嵌套载荷位置只接受一个模式，不能把多条顶层备选塞进一个载荷。`_` 是无绑定通配；与模板已有兜底重复时，按普通 match 规则报错。pattern 不能用于运行期表达式、类型或声明名称位置。

多个变参组可同步重复：

```chali
$macro DOT(...left: value)[...right: value] -> expr
int64 total = 0;
$for (a, b) in (left, right)
total += a * b;
$end_for
total
$end_macro
```

`$DOT(2, 3)[4, 5]` 得到 23。变量与来源数量必须一致，来源必须是同一层可见的变参，且长度相同；空组合法。不同层的重复通过各自的词法作用域绑定。`$count(xs)` 返回变参捕获的元素数，可结合 `$if` 生成可选片段。普通函数实参和元组的逗号仍由对应 AST 列表生成，宏不进行任意文本分隔符拼接。

## 字段遍历与编译期条件

宏中的 `$for field in $fields<T>()` 逐字段生成静态代码。`$member(object, field)` 生成该字段的普通成员访问，检查对象具体类型与描述符所属类型，读写和借用继续遵守普通成员规则。将 `field.type` 传给 `T: type` 形参时，保留字段的精确类型。

```chali
$macro SUM_INTEGERS<T>(value: value) -> expr
int64 total = 0;
$for field in $fields<T>()
$if field.type.name == "int64"
int64 local = $member(value, field);
total += local;
$end_if
$end_for
total
$end_macro
```

`$if condition`、可选的 `$else` 与 `$end_if` 只接受编译期 bool 常量。可使用字段属性、变参数量和常量实参；字符串属性支持 `==` 与 `!=`，按语言转义规则解码后比较。未选分支不参与普通名称、类型和借用检查，实际生成的分支照常检查。这里不会执行任意用户函数或宿主副作用。每轮字段局部声明具有独立身份；字段集合为空时不生成循环正文。描述符仍不能逃逸为运行期值。

专项入口为 [类别变参](../tests/chali_macro_variadics.py)、[模式参数](../tests/chali_macro_patterns.py)与[字段和结构化控制](../tests/chali_macro_controls.py)。

## 编译期断言与错误

`$compile_assert(condition)` 或 `$compile_assert(condition, message)` 检查可证明的 bool 常量。成功时可作为值为 true 的普通表达式使用；作为宏语句时会从生成的语句表中删除，包括 item 宏，不会占据声明位置。`$compile_error(message)` 中止当前展开。

消息必须是常量字符串，支持常量字符串连接、可证明的具名 const 字符串及字段元数据属性。顶层、模块限定和宏局部常量保留各自的词法身份；数值常量按声明位宽与符号归一化，同名运行期局部不会退回读取全局 const。常量依赖环会报错。const 变量的初始化式如果仍需运行用户函数，就不能作为诊断消息或断言条件。原语不执行用户函数、IO 或运行期格式化。条件必须是 bool，不能用整数隐式代替；即使断言成立，消息参数也须合法。单条字符串及连接结果有 1 MiB 的展开预算。

```chali
$macro REQUIRE_INT64<T>() -> stmt
$for field in $fields<T>()
$compile_assert(field.type.name=="int64", "不支持类型 "+field.type.name);
$end_for
$end_macro
```

失败会保留调用与宏定义的展开链，报告实际诊断实参的位置；在字段循环内还会列出所属类型、字段名及字段序号。未选中的 `$if` 分支内的错误不触发。具名宏仍按普通参数规则处理：value 实参求值一次，expr 实参是语法片段，编译期条件不会将普通运行期函数改为可在编译器进程中执行的函数。

[完整字段工具示例](../example/reflection/macro_fields.chl)生成字段打印、校验和 JSON 编码，可运行：

```text
chali run example/reflection/macro_fields.chl --no-reflection
```

示例按定义顺序输出字段，只为 String、int64 和 bool 生成 JSON 编码，其他类型在编译期定位到具体字段。字符串按 JSON 规则转义引号、反斜杠和控制字节，UTF-8 普通字节保留；bool 显式编码为 true／false。64 字段上限是该示例通过断言施加的约束，不是语言的结构体限制。JSON 解析只用于主函数验证生成结果，编码过程不读取运行期字段表。专项见 [编译期诊断与字段工具测试](../tests/chali_macro_diagnostics.py)。

## 类型成员宏

`-> member` 宏在结构体、枚举的方法区或接口的成员位置调用，展开结果参与普通布局、方法、接口和借用检查。它可以生成字段、方法、关联类型及嵌套成员宏调用；接口仍禁止字段，普通类型的方法必须提供方法体。成员宏形参须显式标注语法类别，泛型实参须显式提供，不在类型体求值或借用运行期实参。

```chali
$macro GET(name:ident, field:ident) -> member
int64 name(){return self.field;}
$end_macro
struct Cell {
    int64 value;
    $GET(read, value);
}
```

`name:ident` 明确导出调用方可见的成员名；未由 ident 指定的辅助成员按展开身份隔离，其内部引用同步重命名。旧 item 宏生成类型的原有字段名称规则保持兼容。生成成员与已有成员发生同名字段或相同方法签名冲突时报告错误，合法的方法重载仍由普通检查器处理。成员调用可使用编译期字段循环与条件；读取本类型字段再生成本类型字段会形成依赖环并报错，按字段生成方法不改变布局。专项见 [成员宏测试](../tests/chali_macro_members.py)。

## 展开预览与诊断栈

VSCode 的“预览宏展开”显示当前文件的调用；“当前宏展开一层”和“当前宏全部展开”按光标位置选择调用。一层视图保存生产展开器生成的模板，其中可以保留嵌套宏；全部视图读取完成后续展开和语义检查的同一 AST，不重新执行宏或实参。

树中显示普通值／只读引用／可变引用参数的缓存、已确定的静态类型，以及 `name@序号` 形式的卫生符号。序号仅用于当前树的可读引用关系，不是内部节点或绑定编号；点击符号会高亮同树内的声明与引用。辅助声明不显示内部展开编号，未确定的类型不伪造为具体类型。预览为只读视图，不直接重命名生成代码；需要改名时应修改原声明或显式 ident 实参，不能把内部生成名当作源码声明。

来源按钮跳到调用或原始实参，定义按钮跳到宏定义；导入宏保留其文件，中文文本的位置按编辑器 UTF-16 转换。显式 ident 生成声明的来源落在标识符实参上。字段访问保留所属类型与字段名。预览沿用语言服务的文档与依赖缓存，未保存文档更新或导入内容变化会失效，不向原文件写回内容。

协议 `chali/expandMacro` 接受 `mode: "one" | "all"`（缺省 all）和可选的 LSP `position`；省略 position 返回当前文件的展开记录。每棵树有深度和节点预算，超出部分标为 truncated，总预览数据受容量限制。

命令行使用 `--macro-backtrace short` 仅显示最近一层展开帧，保留实际错误、字段与实参位置；`--macro-backtrace full` 显示完整调用—定义链，缺省 full 保持兼容。专项见 [生产预览与诊断验证](../tests/chali_macro_preview.py)。
