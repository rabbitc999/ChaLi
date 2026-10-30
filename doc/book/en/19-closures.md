# Chapter 19 Function Values and Closures

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../19-函数值与闭包.md)
<!-- END LANGUAGE NAVIGATION -->

Pass a predicate to separate traversal from selection. A closure also retains the environment needed by that behavior.

## 19.1 Pass a Predicate

<!-- verify-book -->
```chali
List<int64> filter(List<int64> xs, bool fn(int64) keep)
{
    List<int64> out;
    for (int64 x in xs)
    {
        if (keep(x))
        {
            out.push(x);
        }
    }
    return out;
}

void main()
{
    List<int64> xs;
    xs.push(3);
    xs.push(8);
    xs.push(15);
    xs.push(42);
    List<int64> evens = filter(xs, fn(int64 x)
    {
        x % 2 == 0
    });
    List<int64> big = filter(xs, fn(int64 x)
    {
        x > 10
    });
    println("{} {}", evens[0], evens[1]);
    println("{}", big.len());
}
```

```text
8 42
2
```

filter accepts a list and a bool fn(int64) predicate. Its loop calls keep and collects matching values. main supplies an even-number predicate, then a greater-than-ten predicate, producing 8 and 42, then length 2. Constructing the anonymous function does not execute the traversal.

## 19.2 Four Capture Modes

| Form | Stored relationship | Updates | Source afterward |
| --- | --- | --- | --- |
| fn const or plain fn | Read-only reference | Cannot write through capture | Must remain alive; later values are visible |
| fn mut | Mutable reference | Writes the source | Shares the same source |
| fn copy | Owned copy | Updates independent state | Original remains usable |
| fn move | Transferred value | Updates owned state | Original binding is unusable |

Copying a reference copies the reference, not its target, and does not extend target lifetime.

## 19.3 const Reads the Source

<!-- verify-book -->
```chali
void main()
{
    int64 n = 1;
    let read = int64 fn() { n };
    let explicitRead = int64 fn const() { n };
    n = 2;
    println("{} {}", read(), explicitRead());
}
```

```text
2 2
```

Both closures borrow n read-only. After n becomes 2, both read 2. const restricts writing; it is not a snapshot. A closure cannot outlive a borrowed source.

## 19.4 mut Writes Back

<!-- verify-book -->
```chali
void main()
{
    int64 n = 1;
    let tick = int64 fn mut(int64 step)
    {
        n += step;
        return n;
    };
    println("{}", tick(3));
    println("{}", n);
}
```

```text
4
4
```

tick borrows n mutably. Its call adds the parameter 3 to the source 1, stores 4 and returns it. Reading n afterward also produces 4. The body executes on invocation, not construction. Borrow checks still apply, and copying a borrowing closure does not duplicate n.

## 19.5 copy Retains Independent State

<!-- verify-book -->
```chali
void main()
{
    int64 n = 5;
    let counter = int64 fn copy(int64 step)
    {
        n += step;
        return n;
    };
    n = 100;
    println("{} {}", counter(2), n);
    let second = counter;
    println("{} {}", second(3), counter(1));
}
```

```text
7 100
10 8
```

counter captures 5 before the caller changes n to 100. Its first call advances its own state to 7. second then copies that current environment: it advances to 10 independently, while counter advances to 8. Calls do not recopy the original initial value.

Strings and collections follow their copy rules. Closures containing noncopyable resources cannot freely be copied. References inside an owned environment retain their lifetime requirements.

## 19.6 move Transfers Ownership

<!-- verify-book -->
```chali
void main()
{
    String text = "inside";
    let take = String fn move()
    {
        text += "!";
        return text;
    };
    String text = "outside";
    println("{} {}", take(), text);
    println("{}", take());
}
```

```text
inside! outside
inside!!
```

take owns the original string and appends a marker on every call. The new String text declaration creates a distinct binding containing outside. Merely assigning to the moved original would be rejected.

Move capture can transfer noncopyable resources into the environment. Their lifetime follows that owner. Borrowing closures cannot access a moved or destroyed source; their last use may precede the move if analysis permits.

## 19.7 Per-Variable Capture Lists

<!-- verify-book -->
```chali
void main()
{
    int64 a = 2;
    int64 b = 3;
    int64 c = 4;
    int64 d = 5;
    let mixed = int64 fn copy[a] mut[b] const[c] move[d](int64 step)
    {
        a += step;
        b += step;
        d += step;
        return a + b + c + d;
    };
    a = 100;
    c = 10;
    int64 d = 200;
    println("{}", mixed(1));
    println("{} {} {} {}", a, b, c, d);
}
```

```text
23
100 4 10 200
```

a is copied, b mutably borrowed, c read-only borrowed and d moved. After construction, changing external a does not change its snapshot; changing c remains visible; redeclaring d creates a new binding.

The call uses internal a=3, source b=4, source c=10 and internal d=6, yielding 23. External output remains 100, 4, 10, 200.

Unlisted locals default to const references. A list can be followed by one default mode: fn copy[a, b] mut(int64 x) copies a and b and mutably borrows other captures. copy[] applies no overrides, not a copy-all default. Reject duplicate, unknown or parameter-conflicting names. Explicit list entries capture even if unused. Array and resource rules still apply.

## 19.8 Return Closures with Valid Environments

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

makeNamedCounter moves its local current into the returned next function. In main, read borrows base, increment writes it through mut and isolated owns a copy. Named-function suffixes express the same capture relationships as anonymous-function prefixes. Owned environments can still contain references whose targets must survive.

## 19.9 Composition and Recursion

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

compose returns a function and copies a and b so its environment survives the call. combined(6) executes twice first, then increment, yielding 13.

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

Declare fac before assigning the recursive body. Arguments decrease to zero, whose branch supplies 1, then multiplication yields 6. This example covers small nonnegative inputs, not overflow or excessive recursion.

## 19.10 Generic and Named Function Forms

let identity = fn<T>(T value) { value }; supports inferred or explicit type arguments. Specializations are selected at compile time, sharing one captured state across typed calls. Copying the closure applies its environment copy rules.

Named functions support leading result types or fn name(...) -> Type. Consult Appendix C for deeply nested signatures. Environment construction, copying and indirect calls have costs; use the supported host function-value protocol rather than casting a closure into a machine function pointer.

## 19.11 Exercises

1. Does default capture take a snapshot?
2. Does each copy-closure call recopy the caller's value?
3. Why redeclare String text after moving?
4. Does copying a borrowing closure copy its target?

### Answers and checks

1. No; the default is a read-only reference.
2. No; calls retain and update the existing environment.
3. The original binding was moved; the declaration creates another binding.
4. No; the same source and lifetime requirements remain.
