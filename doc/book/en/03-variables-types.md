# Chapter 3 Variables and types

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../03-变量与类型.md)
<!-- END LANGUAGE NAVIGATION -->

A variable connects a name, a type and a current value. Decide whether a score is an integer or a measured quantity before selecting its representation. Inference removes repeated type spelling; it does not choose a data model for the application.

## 3.1 Declarations, assignment and inference

Place an explicit type before a name. Let infers from an initializer; const restricts mutation through the binding. An inferred variable still has a concrete type. Ordinary const and compile-time evaluation are distinct: array bounds and similar uses impose additional evaluation rules.

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

## 3.2 Numbers, booleans and text

Integer types differ in width and signedness; float32/float64 represent floating-point values. Bool is used for conditions, char for character values, and String for UTF-8 bytes. Byte count, character count and numeric range are different properties. Floating-point arithmetic has representation error; use the appropriate library for exact or arbitrary-precision requirements.

Unconstrained integer literals normally use int64. Suffixes, expected types and overloads can select other types.

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

## 3.3 Conversions at representation boundaries

Saving int64 as int32 can lose range, so narrowing is explicit: use a cast expression or cast<int32>. Consult the reference for implicit conversions and literal range checks. An explicit cast does not replace application input validation.

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

## 3.4 Scope and later use

A name can denote different bindings in different scopes. Initialize a variable before reading it; ownership rules additionally govern use after a move. Function parameter and return types extend these checks across calls.

## 3.5 Exercises

1. Represent the score as float64.
2. Compare let and Any.
3. Why consider range when converting to int32?
4. Does const imply compile-time execution?

### Answers and checks

1. Change the type and initializer and inspect output.
2. Let infers a type; Any is an explicit erased value.
3. The target range is smaller; validate before converting.
4. No. Compile-time evaluation has its own expression and budget limits.

Next: [Chapter 4](04-functions.md).
