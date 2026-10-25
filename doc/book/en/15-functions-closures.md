# Chapter 15 Function values and closures

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../15-函数值与闭包.md)
<!-- END LANGUAGE NAVIGATION -->

A function value can be passed, stored and returned. Its type includes both the return type and every parameter type. A closure additionally carries access to values from its defining environment. Capture mode decides whether that access borrows, copies or moves state.

```chali
T fn(T) compose<T>(T fn(T) first, T fn(T) second)
{
    return fn copy(T value)
    {
        first(second(value))
    };
}

int64 addOne(int64 value)
{
    return value + 1;
}

int64 doubleValue(int64 value)
{
    return value * 2;
}

void main()
{
    let combined = compose(addOne, doubleValue);
    println("{}", combined(6));
}
```

The result is `13`. The returned closure uses `fn copy` because the callback parameters must remain available after `compose` returns. A default borrowed capture cannot safely borrow those local parameter values beyond their lifetime. Explicit generic arguments do not make an illegal borrow legal.

Inference considers the callback's complete shape. Overloads can be narrowed by known argument types and expected function signatures. A remaining ambiguity should be resolved explicitly. Nested function values and containers of functions retain their type and ownership rules.

Use mutable capture only when changing captured state is intended. For synchronous C callbacks, follow the glue protocol and its lifetime; a runtime function value is not a machine function pointer.

Parameter names inside a trailing function type are erased, so `fn(int64 a, int64 b) -> int64` and `int64 fn(int64, int64)` are one type; the names are notes for the reader only. The leading form stays strict and rejects names outright. Neither form accepts a default value.

A declaration may also put `fn` before the name and move the result type behind the parameter list with `->`. Omitting the arrow makes the result `void`, so `fn show()` is `void show()`:

```chali
fn add(int64 left, int64 right) -> int64
{
    return left + right;
}

fn show()
{
    println("no result");
}
```

The same shape works for generics: `T add<T>(T a, T b)` may be written `fn add<T>(T a, T b) -> T`. It also reads well for functions that return a function — parameters first, then what you get back:

```chali
fn makeAdder(int64 mut& amount) -> (int64 fn(int64))
{
    return fn(int64 value)
    {
        value + amount
    };
}
```

`(int64 fn(int64))` is the ordinary function type; its arrow forms are `fn(int64) -> int64` and the parenthesised `(fn(int64) -> int64)` — all three mean the same function. The trailing function type is only written behind `->`; a variable declaration keeps the leading form `int64 fn(int64, int64) combine;`, because `fn` immediately followed by `(` is the anonymous lambda. The two share one prefix and one token of lookahead cannot tell them apart, so the grammar yields to the lambda that existed first.

In a function body the same shape names a closure, exactly like the statement-level named closures above:

```chali
void main()
{
    int64 shift = 100;
    fn addShift(int64 value) -> int64
    {
        return value + shift;
    }
    println("{}", addShift(4));
}
```

It prints `104`. A named closure written this way takes no capture keyword — `const`, `mut`, `copy` and `move` right after `fn` always mean an anonymous lambda capture — so it always borrows its environment read-only. Parameter names inside a function type are documentation only and never enter type identity. The arrow return type of an anonymous lambda itself was removed in 0.12.0 and is still rejected: `fn (int64 x) -> int64 { ... }` fails, and `int64 fn(int64 x) { ... }` is the annotated form. These shapes are real grammar productions shared by top-level functions, statement-level named closures and function types — not a text rewrite.

Exercise: reverse the composition order and predict the result before running. Then explain which captures must own their data if the composed function is stored for later use. Next: [error handling](16-errors.md).
