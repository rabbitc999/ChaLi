# Appendix A Common errors

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../A-常见错误速查.md)
<!-- END LANGUAGE NAVIGATION -->

Locate the failing stage first: parsing checks syntax, semantic analysis checks types and borrowing, linking resolves target libraries and symbols, and execution uses actual input and resources. These rejection examples retain the significant diagnostics. Filenames, positions and stacks depend on where the programs are saved.

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

## A.8 Investigation order

Keep a minimal reproducer, compiler version, command, configuration and first diagnostic. Separate external input, files, networking and databases from language checks. Verify both the original failure and a successful path after a change; do not report unexecuted checks as passed. Consult the [language reference](../../en/04-language-reference.md), [AOT reference](../../en/38-aot.md) and [library](../../library/README.md).
