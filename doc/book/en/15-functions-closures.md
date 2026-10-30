# Chapter 15 Closures and higher-order functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../15-函数值与闭包.md)
<!-- END LANGUAGE NAVIGATION -->

Function values let algorithms accept behavior. Closures also retain an environment. Understand capture modes and environment lifetimes before relying on syntax alone.

## 15.1 Pass behavior as a value

A higher-order function accepts or returns a function value. Establish parameter and result types, then provide a matching function or closure. Equal signatures do not imply equal environments.

<!-- verify-book -->
```chali
int64 level1(int64 value)
{
    return value + 1;
}

int64 level2(int64 fn(int64) callback)
{
    return callback(10);
}

int64 level3(int64 fn(int64 fn(int64)) callback)
{
    return callback(level1);
}

int64 level4(int64 fn(int64 fn(int64 fn(int64))) callback)
{
    return callback(level2);
}

int64 level5(int64 fn(int64 fn(int64 fn(int64 fn(int64)))) callback)
{
    return callback(level3);
}

int64 hello(
    int64 fn(int64 fn(int64 fn(int64 fn(int64 fn(int64))))) pipeline,
    int64 fn(int64) finish)
{
    return finish(pipeline(level4));
}

void main()
{
    int64 factor = 2;
    int64 finish(int64 value)
    {
        return value * factor;
    }
    println("{}", hello(level5, finish));
}
```

```text
22
```

<!-- verify-book -->
```chali
T fn(T) compose<T>(T fn(T) a, T fn(T) b)
{
    return fn copy(T x)
    {
        a(b(x))
    };
}

int64 increment(int64 value)
{
    return value + 1;
}

int64 twice(int64 value)
{
    return value * 2;
}

void main()
{
    let combined = compose(increment, twice);
    println("{}", combined(6));
}
```

```text
13
```

## 15.2 Captures and independent copies

Read-only, mutable and copy captures express different access relationships. Choose copying for an independent snapshot and validate mutable access for shared state. Capture and copy rules determine environment independence.

<!-- verify-book -->
```chali
void main()
{
    int64 n = 1;
    let f = fn mut(x) { n = x + n };
    let g = fn copy(x) { x * n };
    println("{} {} {}", f(1), g(1), n);
}
```

```text
2 1 2
```

<!-- verify-book -->
```chali
void main()
{
    String a = "1";
    String fn() b = fn copy()
    {
        a
    };
    println("{} {}", a, b());
}
```

```text
1 1
```

## 15.3 Return closures with valid environments

A returned closure needs a valid environment throughout later calls. Wrapping a local borrow in a function value does not extend its lifetime. This factory distinguishes separate instances and their counter state.

<!-- verify-book -->
```chali
int64 fn() makeNamedCounter()
{
    int64 current = 0;
    int64 next() move
    {
        current++;
        return current;
    }
    return next;
}

void main()
{
    int64 base = 10;
    int64 read()
    {
        return base;
    }
    int64 increment() mut
    {
        base++;
        return base;
    }
    int64 isolated() copy
    {
        base += 100;
        return base;
    }
    int64 initial = read();
    int64 changed = increment();
    int64 snapshot = isolated();
    println("{} {} {} {}", initial, changed, snapshot, base);

    let counter = makeNamedCounter();
    int64 first = counter();
    int64 second = counter();
    println("{} {}", first, second);

    int64 callback(int64 value);
    callback = int64 fn(int64 value)
    {
        return value + 1;
    };
    println("{}", callback(5));
}
```

```text
10 11 110 11
1 2
6
```

<!-- verify-book -->
```chali
fn makeAdder(int64 mut& amount) -> (int64 fn(int64))
{
    return fn(int64 value)
    {
        value + amount
    };
}
void main()
{
    int64 one = 1;
    int64 ten = 10;
    let plus1 = makeAdder(&one);
    let plus10 = makeAdder(&ten);
    println("{} {}", plus1(5), plus10(5));
}
```

```text
6 15
```

## 15.4 Recursion and named function values

Recursion needs a callable target inside the body and a termination condition. Named function values can carry environments too. After copying, inspect which state belongs to the copy and which is explicitly shared.

<!-- verify-book -->
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

<!-- verify-book -->
```chali
void main()
{
    int64 shift = 100;
    fn addShift(int64 value) -> int64
    {
        return value + shift;
    }
    fn describe()
    {
        println("shift is {}", shift);
    }
    println("{}", addShift(4));
    describe();
}
```

```text
104
shift is 100
```

## 15.5 Call boundaries and cost

Environment creation, copying, cleanup and indirect calls have separate costs. Inlining depends on the known target, environment and optimizer; allocations are not guaranteed to disappear. Synchronous host callbacks use the prescribed function-value protocol, not casts to machine function pointers.

## 15.6 Exercises

1. Pass a processing function to a higher-order function.
2. How do you check state independence after copying a closure?
3. Why cannot a borrow outlive its local scope?
4. Can every closure become a C function pointer?

### Answers and checks

1. Check the signature and compare results.
2. Call both and inspect counters or text under their capture rules.
3. The object has expired; a function-value wrapper does not extend its lifetime.
4. No; environments and calling conventions differ. Use supported synchronous callback bridging.

Next: [Chapter 16](16-errors.md).
