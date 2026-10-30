# Chapter 3 Variables and Types

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../03-变量与类型.md)
<!-- END LANGUAGE NAVIGATION -->

Imagine recording an exam score. It starts at 70 and gains five points after review. The program needs a name by which later statements can find the current value.

## 3.1 Name a value

<!-- verify-book -->
```chali
void main()
{
    int64 score = 70;
    score += 5;
    let count = 2;
    const int64 limit = 100;
    println("{} {} {}", score, count, limit);
}
```

```text
75 2 100
```

The first statement in main, `int64 score = 70;`, declares score, selects the int64 type, and initializes it to 70. The right-hand expression supplies the first value for the new variable.

`score += 5;` updates that existing variable. It adds five to the current value and stores the result, 75, without creating a new score or changing its type.

`let count = 2;` asks the compiler to infer a concrete type. It does not make count a dynamically typed box. `const int64 limit = 100;` restricts later modification of limit. Inference and read-only access solve different problems.

The three output placeholders receive score, count, and limit in that order. Change the initial score to 60, predict the new first item, and run again.

## 3.2 Choose a representation

Common integer types include int8, int16, int32, int64, and unsigned uint counterparts. Width and signedness determine their ranges. int64 is a useful starting point for ordinary counters; narrower types become useful for formats, foreign interfaces, and explicit range requirements.

float32 and float64 represent floating-point values; bool holds true or false. `'A'` is a character literal, while `"A"` is a string. Chapter 12 develops text handling.

The following larger example uses two functions as type labels. They share a name but accept different parameter types. Chapter 4 explains functions and overloads in detail.

<!-- verify-book -->
```chali
int64 floatingKind(float32 value)
{
    return 32;
}

int64 floatingKind(float64 value)
{
    return 64;
}

uint64 unsignedValue(uint64 value)
{
    return value;
}

void main()
{
    let one = 1u;
    let single = 1.0f;
    uint8 narrow = 255u;
    uint64 maximum = 18446744073709551615u;
    println("{} {} {} {}", unsignedValue(one), floatingKind(single),
            floatingKind(1.0), narrow);
    println("{}", maximum);
}
```

```text
1 32 64 255
18446744073709551615
```

The function declarations do not execute merely because they appear before main. In main, `1.0f` selects float32 while `1.0` selects float64, so the two observations produce different labels.

The integer declarations demonstrate unsigned values at different widths. A u suffix does not remove the target type's range limit. The first print shows four observations; the second preserves the complete large integer.

Hexadecimal forms such as `0xff` and grouped forms such as `1_000` are other spellings of values. They do not enlarge the target representation. Appendix C collects literal forms and suffixes for later reference.

## 3.3 Expressions produce values

`score + 5` produces a value; `score = score + 5;` uses it to update a variable. Parentheses make evaluation order clear: `(2 + 3) * 4` yields 20. A comparison such as `score >= 60` produces bool.

Conversions also produce values:

<!-- verify-book -->
```chali
void main()
{
    int64 original = 42;
    int32 narrowed = cast<int32>(original);
    println("{}", narrowed);
}
```

```text
42
```

The first declaration preserves the original value. Both conversion forms express a target type without modifying original. The output places the results together for comparison.

The value 42 fits int32. For arbitrary input, a written conversion is not a range check: validate an amount or measurement before narrowing it.

## 3.4 Names have scope

Braces can define a local scope. A name declared inside it is not automatically available afterward. Functions, references, and resources will give this rule more practical uses.

When reading a variable, ask for its name, type, current value, and whether the binding permits modification.

## 3.5 Exercises

1. Change the initial score to 60 and predict the output.
2. Replace count's inferred declaration with an explicit one.
3. Why does const restrict later assignment to limit?
4. Store whether the updated score is at least 60 in a bool.

### Answers and checks

1. The first output becomes 65; the other items remain unchanged.
2. Write `int64 count = 2;`.
3. const imposes a checked read-only restriction on the binding.
4. `bool passed = score >= 60;` stores the comparison result without changing score.
