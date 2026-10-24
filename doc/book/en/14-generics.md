# Chapter 14 Generics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../14-泛型.md)
<!-- END LANGUAGE NAVIGATION -->

A generic function or structure describes operations for several concrete types. The compiler infers type arguments where the available arguments and constraints determine them, and specializes the reachable implementation. Explicit type arguments are useful when the intended type cannot be determined uniquely.

```chali
T identity<T>(T value)
{
    return value;
}

void main()
{
    println("{}", identity<int64>(42));
}
```

Inference must preserve the whole type shape. A function parameter has parameter types and a return type; a container has its element or key/value types. Matching only the outer name would lose information needed to select legal calls.

Overloaded callback arguments are selected against the expected function shape and types inferred from the other arguments. If more than one candidate remains applicable, write enough type information to make the call unambiguous. An ambiguous call should produce a diagnostic rather than silently select a different implementation.

Generics do not remove borrowing or resource restrictions. A specialized body still has to be legal for its concrete types. Exercise: infer an identity call from an integer, then pass a function value into a generic helper. Next: [function values and closures](15-functions-closures.md).
