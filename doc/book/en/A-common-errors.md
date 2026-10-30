# Appendix A Common errors

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../A-常见错误速查.md)
<!-- END LANGUAGE NAVIGATION -->

Locate the failing stage before explaining the violated premise. Parsing, type checking, linking and execution consume different inputs; treating every failure as a runtime crash leads to misplaced fixes.

This appendix pairs minimal failing examples with representative diagnostics and repairs. Each walkthrough distinguishes legal declarations, the failing operation and whether later statements execute. Paths and line numbers identify a particular saved file rather than fixed text across environments.

## A.1 Locate the stage

| Stage | Checks | Next action |
| --- | --- | --- |
| Parsing | Delimiters and declaration structure | Fix the first syntax error in context |
| Types and borrowing | Arguments, returns, reference sources and resources | Inspect declarations and lifetimes |
| Code generation and linking | Target, ABI, libraries and symbols | Verify toolchain and dependencies |
| Execution | Inputs, bounds, absence and resources | Establish preconditions or return recoverable errors |

## A.2 Division by zero and runtime preconditions

Check a denominator before division; successful parsing does not make it nonzero.

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

### Code walkthrough

Both integer initializations are legal. The failure arises when b equal to zero becomes a divisor. Println must evaluate a/b before formatting, so no quotient is printed normally.

The diagnostic represents a runtime failure and source location; line numbers vary with the saved file. Handle b==0 before division by returning an error or requesting new input. Changing the format string does not establish a valid divisor, nor does merely parsing an integer.


## A.3 Narrowing conversions

Check the target range before an explicit cast; do not truncate business data merely to suppress a diagnostic.

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

### Code walkthrough

Big is int64 while small requires int32. Without an explicit conversion, the assignment violates implicit narrowing rules even though 70000 itself fits int32. The rule concerns the source expression's type rather than guessing whether its current value happens to be small enough.

Checking fails before println executes. When narrowing is intended, verify range and business meaning and request conversion explicitly. If the application needs the full int64 range, retain a sufficiently wide destination instead.


## A.4 Out-of-range literals

Choose a type that can represent the value or correct an invalid constant.

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

### Code walkthrough

Uint8 permits zero through 255, so the literal 256 cannot satisfy its destination type. Unlike the preceding example, the initializer itself visibly exceeds the range; a missing conversion symbol is not the only issue.

Correct the constant or choose a type that can hold it. Relying on truncation changes the business value rather than simply removing a diagnostic. This fails during compilation and has no normal program output to compare.


## A.5 Reference sources

Bind a reference to an actual borrowable object; a numeric cast does not create one.

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

### Code walkthrough

Mut& needs an actual writable, borrowable source object. The literal five supplies an integer value, not that relationship. A correctly spelled reference declaration cannot turn a literal into a mutable long-lived source.

Declare an owning integer variable first, borrow it under the language rules and keep it alive while the reference is used. Numeric casts or fabricated addresses do not replace source validation. This example is rejected when r is initialized.


## A.6 Unbound references

Bind an object at declaration and keep it alive through the reference use.

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

### Code walkthrough

An ordinary reference declaration requires a valid source. `int64 mut& r;` supplies none and cannot create an empty reference to bind later. The declaration is already invalid before println.

Bind a live object at declaration. If absence is meaningful, use a type or host interface designed to express it rather than treating an uninitialized reference as None. A later use cannot retroactively complete the declaration.


## A.7 Extracting an absent value

Match Option or check isNone before extraction. Construct None explicitly for absence; default construction is not an absence test.

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

### Code walkthrough

Option<int64> permits an integer to be present, but none explicitly holds None. Unwrap requires Value. A correct container type therefore does not guarantee successful extraction, and this call fails on absence.

Unlike the uninitialized reference case, the Option declaration itself is valid; failure occurs when unwrap executes. Handle both arms with match, select an unwrapOr fallback or propagate absence instead of assuming a different variable name can establish presence.


## A.8 Investigation order

Keep a minimal reproducer, compiler version, command, configuration and first diagnostic. Separate external input, files, networking and databases from language checks. Verify both the original failure and a successful path after a change; do not report unexecuted checks as passed. Consult the [language reference](../../en/04-language-reference.md), [AOT reference](../../en/38-aot.md) and [library](../../library/README.md).
