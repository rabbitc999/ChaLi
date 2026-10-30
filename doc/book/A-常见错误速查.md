# 附录 A 常见错误速查

<!-- LANGUAGE NAVIGATION -->
[English](en/A-common-errors.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

诊断先按发生阶段定位：解析检查语法结构，语义检查类型与借用，链接阶段查找目标库和符号，运行阶段检查实际输入与资源。下面保留可执行的拒绝示例及其关键诊断；文件名、位置和调用栈会随保存位置变化。

## A.1 定位阶段

| 阶段 | 检查内容 | 后续动作 |
| --- | --- | --- |
| 解析 | 标点、括号与声明结构 | 从首个语法错误修正上下文 |
| 类型与借用 | 实参与返回类型、引用来源、资源使用 | 检查声明与存活范围 |
| 代码生成与链接 | 目标架构、ABI、库与符号 | 核对工具链和真实依赖 |
| 应用运行 | 输入、边界、缺省与外部资源 | 建立前提或返回可恢复错误 |

## A.2 除零与运行期前提

先检查分母再执行除法。输入解析成功并不保证它可以作为分母。

<!-- fail -->
```chali
void main()
{
    int64 a = 1;
    int64 b = 0;
    println("{}", a / b);
}
```

```text
chali aot: 整数除零
出错位置：hello.chl:5（main）
调用栈（内 → 外）：
  main
```

## A.3 缩窄转换

确认目标范围，再显式转换；不能仅为消除诊断而截断业务数据。

<!-- fail -->
```chali
void main()
{
    int64 big = 70000;
    int32 small = big;
    // 缩窄：位宽变小
    println("{}", small);
}
```

```text
chali: hello.chl 第 4 行第 5 列：不能隐式转换：int64 -> int32（请显式强转）
```

## A.4 字面量越界

选择能容纳数值的类型，或修正本来就不合理的常量。

<!-- fail -->
```chali
void main()
{
    uint8 b = 256;
    // uint8 只到 255
}
```

```text
chali: hello.chl 第 3 行第 5 列：整数字面量 256 超出 uint8 的取值范围（0..255）
```

## A.5 引用来源

引用必须绑定允许借用的实际对象，数值转换不能制造合法引用。

<!-- fail -->
```chali
void main()
{
    int64 mut& r = 5;
    // 引用要从对象身上取地址，不能绑字面量
}
```

```text
chali: hello.chl 第 3 行第 5 列：不能隐式转换：int64 -> int64 mut&（请显式强转）
```

## A.6 未绑定引用

在声明处绑定对象，并确保其存活覆盖引用的使用。

<!-- fail -->
```chali
void main()
{
    int64 mut& r;
    println("{}", r);
}
```

```text
chali: hello.chl 第 3 行第 5 列：引用声明必须初始化，缺省值请使用 Option<T mut&>
```

## A.7 缺省值取出

先匹配 Option 的状态或检查 isNone，再使用取值接口。需要缺省时显式构造 None；默认构造不应被当作缺省。

<!-- fail -->
```chali
void main()
{
    Option<int64> none = None;
    println("{}", none.unwrap());
}
```

```text
chali aot: panic：called Option.unwrap() on None
```

## A.8 排查顺序

先保存触发问题的最小程序，保留编译器版本、命令、配置和首条诊断。把外部输入、文件、网络或数据库问题与语言检查区分开。改动后重新验证原失败条件和正常路径；未运行的检查不写成通过。更多诊断和接口边界见[语言参考](../04-language-reference.md)、[AOT](../38-aot.md)和[标准库](../library/README.md)。
