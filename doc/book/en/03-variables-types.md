# Chapter 3 Variables and types

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../03-变量与类型.md)
<!-- END LANGUAGE NAVIGATION -->

Types describe the values a variable may hold and the operations available on it. Signed integer types are `int8`, `int16`, `int32` and `int64`; unsigned counterparts start with `uint`. Floating-point types are `float32` and `float64`. `bool`, `char` and `String` serve different purposes: a byte character is not a complete Unicode string.

```chali
void main()
{
    int64 count = 3;
    const int64 limit = 10;
    float32 ratio = 1.0f;
    uint64 mask = 1u;
    let next = count + 1;
    println("count={}, next={}, limit={}", count, next, limit);
}
```

`let` infers a type; it does not turn the variable into an untyped box. `const` prevents modification through that declaration. Numeric conversions still obey the language's conversion rules; do not assume that every narrowing conversion is implicit.

Fixed arrays carry their declared length. Dynamic arrays and `List<T>` have different allocation and growth behavior. Choose a representation from the data's lifetime and size requirements, then check its API rather than treating all containers as interchangeable.

Exercise: compare `1.0f` with an unsuffixed floating literal and explain why assigning a wider value to a narrower destination can require an explicit cast. Next: [functions](04-functions.md).
