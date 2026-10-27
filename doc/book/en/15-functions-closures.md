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

## Recursive closures and assignment context

Declaring a function variable before assigning a closure that refers to the same binding is the supported way to write recursive closures: the declaration supplies the complete function signature, the closure body resolves `fac` to that declared binding, and the call happens after assignment. In this factorial example, the assignment target supplies the complete function signature, so every occurrence of the omitted parameter `x` has type `int64`.

```chali
void main()
{
    int64 fn(int64) fac;
    fac = fn(x)
    {
        if (x >= 1)
        {
            fac(x - 1) * x
        }
        else
        {
            1
        }
    };
    println("fac(3)={}", fac(3));
}
```

```text
fac(3)=6
```

## Fibonacci with the same feature

The Fibonacci recurrence uses the same declaration and assignment pattern. Its base cases are `fib(0)=0` and `fib(1)=1`; consequently `fib(3)=2`. LSP provides parameter hover, completion details and inlay hints from the assignment target.

```chali
void main()
{
    int64 fn(int64) fib;
    fib = fn(x)
    {
        if (x == 0)
        {
            0
        }
        elif (x == 1)
        {
            1
        }
        else
        {
            fib(x - 1) + fib(x - 2)
        }
    };
    println("fib(3)={}", fib(3));
}
```

```text
fib(3)=2
```

## Observe deferred execution with a counter

A closure stores its body and environment when created; its body executes when invoked. A counter makes this deferred execution observable — this is not the iterator laziness of chapter 17, only a matter of call timing: the body runs each time the closure is called. `fn mut[counter](x)` explicitly borrows only `counter` mutably, while `fib` retains its default read-only capture. It computes a local result, increments the counter and returns that result, counting completed calls. The parameter `x` still receives `int64` from the target signature.

```chali
void main()
{
    int64 counter = 0;
    int64 fn(int64) fib;
    fib = fn mut[counter](x)
    {
        int64 result = if (x == 0)
        {
            0
        }
        elif (x == 1)
        {
            1
        }
        else
        {
            fib(x - 1) + fib(x - 2)
        };
        counter += 1;
        return result;
    };
    println("counter={}", counter);
    println("fib(3)={}", fib(3));
    println("counter={}", counter);
}
```

```text
counter=0
fib(3)=2
counter=5
```

The counter is zero immediately after assignment. Evaluating `fib(3)` enters the body for `3`, `2`, `1`, `0` and `1`, five times in total. Further calls continue to increment it: results are not automatically memoized. Arguments still follow ordinary call evaluation rules; the deferred part is execution of the closure body.

For an explicit higher-order closure, `int64 fn(int64) fn(int64 fn(int64) a, int64 fn(int64) b) { a }` has the complete type `(int64 fn(int64)) fn(int64 fn(int64), int64 fn(int64))`. LSP retains this outer signature and unwraps one result function at each call.

Exercise: reverse the composition order and predict the result before running. Then explain which captures must own their data if the composed function is stored for later use. Next: [error handling](16-errors.md).
