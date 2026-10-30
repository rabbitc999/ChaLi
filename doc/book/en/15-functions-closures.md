# Chapter 15 Closures and higher-order functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../15-函数值与闭包.md)
<!-- END LANGUAGE NAVIGATION -->

Comparators, predicates and completion actions may vary while an algorithm's structure remains stable. Function values pass that varying behavior as data. Closures additionally retain the environment needed for later calls.

Read nested function types first, then trace composition and captures. Returned closures, borrowed sources, named functions and recursion follow. For every example identify both the invoked function and the environment it reads.

## 15.1 Pass behavior as a value

Treat `int64 fn(int64)` as a type describing behavior with an integer input and result. A function accepting it decides where to invoke that behavior without prescribing its implementation. This separates a fixed workflow from a varying step.

The first example nests this relationship several times. Read level1 and level2 before adding outer layers. Compose then applies the same idea to ordinary function composition. Even long nested signatures can be traced as a sequence of normal calls.

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

### Code walkthrough

Read the innermost `int64 fn(int64)` as a function taking an integer and returning an integer. Each outer fn layer accepts the previous function type while still ultimately returning int64. Level2 supplies the numeric argument ten; the other levels pass suitable functions onward. Declaring or passing a function does not execute it.

Main passes level5 and finish to hello. The chain evaluates level5(level4), level4(level3), level3(level2), level2(level1), then level1(10), producing eleven. That result returns through the chain to finish, which captures factor equal to two and produces 22. Separate passing a function from invoking it rather than interpreting level numbers as recursion counts.


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

### Code walkthrough

Compose returns `T fn(T)`, a function value rather than an immediate T result. Copy capture retains a and b in the returned closure's environment after compose returns. Its final expression calls b first and passes that result to a.

Main supplies increment as a and twice as b. Creating combined performs no arithmetic on six. Calling combined(6) computes twice(6)=12 and then increment(12)=13. Swapping the arguments changes composition order. The generic T must agree across both functions' input and output types.


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

### Code walkthrough

F uses mut capture to update the external n. G uses copy capture to retain the value one that n had when g was created. Their environments are established before invocation; g does not take a fresh external snapshot on every call.

The output first calls f(1), updating external n to two and producing the shown result two. G(1) still multiplies by its captured one, giving one, and the final read observes external n equal to two. Hence `2 1 2`. This demonstrates distinct capture relationships rather than universal sharing between closures.


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

### Code walkthrough

`String fn() b` declares a function-valued variable taking no arguments and returning String. The fn copy initializer retains a's value, and the final a expression supplies each call's result. Copy capture does not move away the external a, so main can still print it.

`b()` invokes the function; b alone denotes its value. Printing the external string and the captured result gives one twice. Equal text does not establish shared mutation: the capture mode determines that relationship.


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

### Code walkthrough

In main, read observes external base, increment mutates it through mut capture, and isolated retains the value ten through copy capture at creation. Sequential calls give initial=10 and changed=11. Isolated adds 100 to its own copy, yielding 110 while external base remains eleven. This explains the first line.

MakeNamedCounter moves current into the returned next environment, keeping owned state alive after the factory exits. Calling the same counter twice changes zero to one and then two rather than reinitializing it. Finally callback is declared by signature and assigned an increment closure, so callback(5) returns six. These parts demonstrate capture relationships, returned-environment lifetime and function-valued assignment.


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

### Code walkthrough

MakeAdder accepts a caller-provided reference and returns a closure using that source. One and ten are different objects in main, so plus1 and plus10 have the same function type but different associated sources.

Calling both with five reads one and ten respectively, producing six and fifteen. Returning a function value does not remove the source's lifetime requirements; both sources remain alive throughout these calls. This pattern does not permit returning a borrow of a destroyed factory local or treating an environment-bearing function value as a C function pointer.


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

### Code walkthrough

Fac's function type is declared before assigning the closure, giving its body a named callable target. Starting at three, x decreases until zero. The recursive arm multiplies fac(x-1) by x; the else arm returns one, the multiplicative identity and termination case.

The calls expand as fac(3)=fac(2)*3, fac(2)=fac(1)*2 and fac(1)=fac(0)*1, yielding six. Without a terminating path recursion would continue. Fac must also receive a valid function value before use. The example covers small nonnegative input, not large-integer overflow or excessive recursion depth.


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

### Code walkthrough

AddShift uses an fn prefix and an explicit int64 after the arrow. Its body reads shift from main, so this named function still carries environment information. Describe returns no business value and prints that same source.

Calling addShift(4) yields 104; calling describe afterward prints shift is 100. The addition computed a result without changing shift. Interpret each declaration through its signature rather than treating an omitted business result as permission to return an arbitrary type.


## 15.5 Call boundaries and cost

Environment creation, copying, cleanup and indirect calls have separate costs. Inlining depends on the known target, environment and optimizer; allocations are not guaranteed to disappear. Synchronous host callbacks use the prescribed function-value protocol, not casts to machine function pointers.

### Summary

Function types specify inputs and results, capture modes specify environmental relationships and lifetimes determine valid later calls. Matching signatures do not settle capture or lifetime questions. Copy retains independent values, mut permits corresponding writes and move transfers owned state into the environment.

Intermediate results clarify complex call ordering. Check sources before returning closures and use supported protocols at host boundaries rather than casting language function values to machine callbacks. Next, make absence and failure explicit.

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
