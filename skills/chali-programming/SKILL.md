---
name: chali-programming
description: 编写、修改与调试 ChaLi (.chl) 程序，依据实际文法和标准库选择语法、宏、值语义、借用与错误处理方式，并使用 ChaLi 工具链验证。用于 ChaLi 应用代码和示例，不替代编译器或运行时内部实现的仓库规范。
---

# 编写 ChaLi 程序

本 Skill 面向使用 MCP 编写 ChaLi 的 AI。目标是交付可检查、可运行的完整 `.chl` 程序，而不是只给出看起来像 C++、Rust 或 JavaScript 的代码。

## MCP 工作流

如果当前会话提供了 ChaLi MCP，优先使用它完成文件操作和验证：

1. 需要陌生语法、关键字或标准库 API 时，先调用 `chali_man` 查询实际编译器手册。
2. 使用 `chali_write` 写入完整源码；默认让工具立即执行 `chali check`，先修复诊断再继续。
3. 需要验证行为时调用 `chali_run` 的 `run` 模式，并依据真实 stdout、stderr 和退出码修改程序。
4. 用户需要可交付可执行文件时调用 `chali_run` 的 `build` 模式；不要把 `check` 的成功误报为构建成功。
5. 用户要求新工程时调用 `chali_new`，再用 `chali_write` 创建或修改源文件。

工具不可用时才使用当前环境中的 `chali` / `ChaLi.exe` 命令，并明确报告实际使用的编译器路径和未执行的检查。不要自行实现编译器规则，也不要凭记忆发明标准库 API。

## 交付习惯

- 先给出最小完整程序，再按需求增加模块、错误处理和性能优化。
- 每次修改后至少重新 `check`；涉及输出、断言或资源生命周期时再 `run`。
- 报告验证结果时引用实际退出码和关键输出；编译失败时保留原始诊断中的文件和行号。
- 源码使用 UTF-8，不在 `.chl` 文件中混入 Markdown fences。
- 除非用户明确要求，不修改文法、解析器、类型系统或运行时实现；本 Skill 只指导 ChaLi 应用代码。

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](SKILL.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

目标是交付能被实际 ChaLi 编译器检查、运行的 `.chl` 程序。ChaLi 使用 C 风格声明和块，但类型、所有权与运行时行为由 ChaLi 自己定义，不要照搬 C++、Rust 或 JavaScript 的泛型、所有权与函数语法。

## 先查询随程序分发的手册

定位已安装的 `chali`（Windows 为 `ChaLi.exe`）。语言用法优先通过程序内嵌手册查询，无需查找仓库 `doc/`，也不依赖 Python 或网络。先执行 `chali man help` 确认可用命令，按任务读取相关条目：

| 任务 | 命令 |
| --- | --- |
| 导出书稿与查看章节 | `chali man book`、`chali man book 6` |
| 关键字语法和用法 | `chali man keyword break`、`chali man keyword move` |
| 标准库模块 | `chali man api std::text` |
| 类型、方法及参数 | `chali man api std::text::Text::split` |
| 搜索 API | `chali man api --search delimiter` |
| 机器可读结果 | `chali man keyword break --json` |
| 获取这份 AI SKILL | `chali man skill` |
| 工具链选项 | `chali --help` |

`man book` 是导出命令，会写入当前工作目录的 `book/`；它更新书稿同名文件，保留其他文件。只需查询时使用 `man book 章号`。书稿可按章号、文件名或标题查找；附录使用 `A`、`B`。手册中的示例可能是片段，不把它们未经检查当作完整程序。中英文界面使用 `--lang zh` 或 `--lang en`；`man book 章号 --lang en` 查询英文书稿，整本导出包含中文和 `book/en/` 下的英文 Markdown。若安装版本缺少对应手册，明确说明工具版本边界；只有任务本身涉及编译器实现或修改文档时，再查实际仓库的文法、源码、回归和 `AGENTS.md`。

本 Skill 的权威文件为 `skills/chali-programming/SKILL.md`，同一内容由 `man skill` 和 VS Code 插件分发；无需为了使用它克隆 ChaLi 仓库。

## 第一个能跑的程序

```chali
void main() {
    int64 total = 0;
    int64 i = 1;
    while (i <= 10) {
        total = total + i;
        i = i + 1;
    }
    assert(total == 55);
    println("total={}", total);
}
```

```text
total=55
```

程序入口必须返回 `void`，可写 `void main()` 或 `void main(List<String> args)`；写 `int32 main()` 会报「main 返回类型必须为 void」。结构体、枚举和接口声明在 `}` 结束，尾分号可选；普通函数写 `int32 add(int32 a, int32 b) { return a + b; }`。

## 常见决策

- 核心 `std::core` 自动加载，可直接使用 `String`、`List<T>`、`println`、`assert`。其他模块按实际文档导入，查到真实 API 再调用；无歧义的名称优先用短名，泛型函数调用能从实参推断时省略显式类型参数；`chali man api` 没有的东西不要凭印象写。
- 标准库公开复合类型使用大驼峰（`DateTime`、`XmlNode`），方法和模块级函数使用小驼峰（`childCount`、`isPlaying`、`formatUtc`）；模块使用小写下划线（`std::date_time`、`std::number_theory`），`std::gui`、`std::audio`、`std::xml` 等单词模块保持小写。
- `int32`、`int64`、`uint64`、`float32` 等是具体类型。未标记整数字面量可按上下文定型；浮点无后缀默认 `float64`，`1.25f` 为 `float32`，`1u` 为无符号整数。`-1u` 和 `-(1u)` 是编译错误，不用它们表示回绕；按需求选择有符号值、显式转换或无符号减法。
- 普通赋值和按值传参复制值；字符串、列表和聚合也遵守值语义。要避免大对象复制就传引用（`T&` 只读、`T mut&` 可写），要转移资源就写 `move x`。
- 用 `if (condition) { value } else { value }` 取条件值，简单分支的值不带分号。多语句分支用 `yield value;`。不要生成 `?:`、`choose` 或 `chooseLazy`。
- 三段式 `for (int64 i = 0; i < n; i++) { ... }` 支持循环内初始化声明、空段和逗号顺序执行的初始化／更新表达式；条件须为 bool，continue 先更新。计数循环优先用 for 或 for-in，避免额外独立块包住计数变量。独立 `loop { ... }` 可省略分号；作为值表达式时按外层语句收尾。 两种 for 的 else 均能访问循环变量：三段式读取最终初始化状态，for-in 读取最后一个值；空输入使用普通默认值，无来源引用禁止读取。借用迭代器的 else 元素保存独立副本，要求值可复制。
- 非空独立块可作为表达式：`int32 a = { int32 b = 3; yield b; };`，以及 `a = { int32 b = 4; yield b; };`。每条正常路径必须给出值；空 `{}` 是集合字面量。`yield value;` 穿透无值循环，交给最近有接收者的表达式；内部可写变量优先移动，外部变量复制。无接收者时报错。`break;` 退出最近循环；在 `while`、`for-in`、三段式 `for` 或语句 `loop` 前声明标签，例如 `outer: while (true) { break outer; }`，即可用 `break 标签;` 退出该循环。标签不能跨函数边界，活动标签不能重复；`all` 也只是普通标签名。`continue;` 继续最近循环，块表达式本身不建立循环。
- 用 `println("value={}", value)` 做格式输出，或直接 `println(f"value={value}")`。后者可包含表达式、嵌套插值，使用 `\{`、`\}` 表示字面花括号。不要给插值输出附加传统格式参数。
- `List<int32> values;` 已经是空列表，`values.push(element)` 追加，`len(values)` 取长度。窄类型元素需要合适的类型上下文，可先声明 `int32 element = 1;` 再 push，不假设所有容器方法都会替字面量推导类型。
- 局部拥有数组可写 `int64[] values = new[2];` 或 `int64 values[] = new[2];`；形参也接受 `T[] values` 和 `T values[]`。数组分配后长度不变，按值传参独立复制，借用用 `T[] const&` / `T[] mut&`；两种局部写法都必须初始化，并遵守相同的作用域清理与移动限制。全局拥有数组仍写 `T values[]`。
- 元组使用编译期整数常量下标，例如 `pair[0]`、`pair[1]`，数组中的元组写 `pairs[0][0]`。下标从零开始并在编译期检查范围；元素只读，更新需给整个元组赋值。
- 函数类型及匿名函数返回类型前置，例如 `int64 fn(int64) callback`、`int64 fn(int64 x) { return x; }`；匿名函数使用 `fn`，捕获修饰和列表写在 `fn` 后。
- 处理可能失败的操作时查 `Option`、`Result` 的当前接口；不要自行发明 Rust 风格的方法名或泛型默认参数。

## 值语义与 `move`

普通拥有值赋值时**深拷贝**其资源，副本与原值独立；引用仍然只复制借用关系，不延长来源寿命。只有确实要把资源交出去时才用 `move`，它不复制、直接把源槽清空：

```chali
struct Buffer : Drop {
    String text;
    void drop() { println("释放 {}", self.text); }
}

void main() {
    Buffer first;
    first.text = "alpha";
    Buffer second = first;
    println("{}/{}", first.text, second.text);

    Buffer third = move second;
    println("third={}", third.text);
}
```

```text
alpha/alpha
third=alpha
释放 alpha
释放 alpha
```

只有两行「释放」：`second` 的值已经被 `move` 走，它的析构被跳过。几条硬规矩：

- `move` 是**一元运算符**，写 `move x`；操作数直接使用变量或引用，不接受额外的取地址表达式。
- 操作数是**位置表达式**：变量、`self`、成员 `move obj.field`、下标 `move list[i]`，或者指针 / 引用本身。指针与引用**自动解引用一层**，所以写 `move p` 而不是 `move *p`（后者在文法上就是语法错误）。
- `move` 只做一层：`T**` 的 `move p` 移动的是那个 `T*`。标量、字符串、容器、`Any`、闭包和具体聚合可从可写位置移动；字面量、临时值及其他右值表达式不能作为来源。数组和不透明宿主类型不支持直接移动；数组中的具体元素可以移动。泛型在具体类型确定后按同一规则检查。
- 移动后不能读取旧值、再次移动或用普通复制赋值恢复。可二次声明建立新绑定；已消费的拥有对象也可通过 `owner = move replacement` 明确补入新的拥有值。经可变引用移动时，原拥有者与旧别名同样失效；把引用重新绑定到其他存活对象只会选择新的来源。非法读取报告 `CHL1205`。

`#borrow(#ret,a)` 声明返回借用的来源；`#borrow(a,b)` 声明目标 a 保存来自 b 的借用，等价于 `#borrow(a,[b])`。普通函数、泛型实例和跨模块直接调用都检查所选声明的契约。指令不延长局部值寿命，也不恢复已移动值；带契约函数当前须直接调用，不能取成丢失契约的函数值。

## `Option` 与 `Result`

可能没有值用 `Option<T>`，可能失败用 `Result<T, E>`。类型上下文明确或名称唯一时，优先写 `Value(x)` / `None`、`Ok(x)` / `Fail(e)`，泛型参数由上下文推断；其他枚举也优先使用短变体名。名称冲突且上下文不足时再保留必要的 `类型::变体` 限定。判断与取值用 `isValue()` / `isNone()` / `isOk()` / `isErr()`，取值用 `take()`（转移载荷）、`unwrap()`、`unwrapOr(兜底)`、`takeError()`：

```chali
Option<int64> look(List<int64> values, int64 index) {
    if (index < 0 || index >= len(values)) {
        return None;
    }
    return Value(values[index]);
}

Result<int64, MessageError> divide(int64 a, int64 b) {
    if (b == 0) {
        return Fail(MessageError::from("除数为零"));
    }
    return Ok(a / b);
}

void main() {
    List<int64> values;
    values.push(7);
    values.push(9);

    match (look(values, 1)) {
        Value(value) => { println("找到 {}", value); }
        None => { println("越界"); }
    }

    Result<int64, MessageError> ok = divide(9, 3);
    if (ok.isOk()) { println("商 {}", ok.take()); }
    Result<int64, MessageError> bad = divide(1, 0);
    if (bad.isErr()) { println("错误 {}", bad.takeError()); }
}
```

```text
找到 9
商 3
错误 除数为零
```

## 卫生宏

宏是**编译期 AST 替换**，用 `$` 标记，和普通标识符不共享命名空间。类型参数放尖括号（`<T:first>` 表示类型由首个实参推导），值参数放方括号（`...rest` 是变长）：

```chali
$macro vec<T:first>[first,...rest]
{
    List<T> result;
    result.push(first);
    $for element in rest
    result.push(element);
    $end_for
    yield result;
}
$end_macro

void main() {
    List<int64> values = $vec[1, 2, 3];
    assert(values.len() == 3 && values.at(2) == 3);
    println("count={}", values.len());
}
```

```text
count=3
```

- 宏体里的名称是卫生的：调用方也有 `result` 也不会撞车；实参保留调用处的绑定。
- 普通宏值参数在进入宏体前按源码顺序各求值一次，宏体重复引用复用该值，例如 `$SQUARE(next())` 只调用一次 `next()`。显式标成 `expr` 的语法片段按插入位置执行，重复插入时才可能重复求值；需要一次求值时使用值参数。
- 补全里宏与普通标识符同口径：输入 `$` 就出候选；只有一个模块可见时是 `$宏名`，跨模块同名时显示 `$模块::宏名` 并插入限定名，唯一的宏优先用不带模块链接的短名。

## 验证交付

对新增或修改的程序执行 `chali check path/to/program.chl`，再 `chali run path/to/program.chl`，比较输出并检查断言。命令以安装工具链的实际 `--help` 为准；Windows 仓库可用 `python tools/compiler_path.py` 定位正式命名的编译器，或显式指定本阶段产物。无法执行时说明未验证，不能把推测写成实测结果。

在 ChaLi 仓库内，默认编入前端与运行时反射。普通 run／build 从检查后的 AST 直接生成 LLVM IR；IR（四元式）保留独立导出与兼容输入，不做内部优化。优化由 LLVM 执行，原生发布默认 O3 与完整 LTO；`--debug` 使用 O0，`--debug-optimized` 保留调试信息并使用 O3。编写文档时使用功能说明，发布变更放根目录 `CHANGELOG.md`，`doc/` 不新增版本发布页。

单独执行 `chali man` 与 `chali man help` 均显示用法；`chali man api` 显示 API 分类，`chali man api XXX` 查询内嵌 `doc/library/` 的 API。

标准库错误类型实现 `Error.toString()`；通用处理接收 `Error const&` 或约束 `<E: Error>`。文本错误使用 `MessageError::from(text)`，保留错误原文。

目标枚举已确定时，返回、初始化、赋值与实参可以直接使用唯一匹配的单载荷值，例如 `return MessageError::from("failed");` 自动成为 `Fail(...)`。相同最佳匹配必须显式选择变体；无载荷、多载荷和未确定的泛型参数不靠此规则猜测。资源移动与引用生命周期检查仍然适用。
