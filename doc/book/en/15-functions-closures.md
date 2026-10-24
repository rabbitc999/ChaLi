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

Exercise: reverse the composition order and predict the result before running. Then explain which captures must own their data if the composed function is stored for later use. Next: [error handling](16-errors.md).
