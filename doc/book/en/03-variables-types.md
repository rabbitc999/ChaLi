# Chapter 3 Variables and types

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../03-变量与类型.md)
<!-- END LANGUAGE NAVIGATION -->

Data needs both a name and a type explaining its representation. A number might represent a score, count or capacity, with different valid ranges and mutation rules. Choose types from the data's meaning before choosing concise syntax.

The chapter proceeds through declarations, updates, literal typing, overload observation and explicit conversion. Track the declared type separately from the changing value rather than treating inference as unrestricted runtime type changes.

## 3.1 Declarations, assignment and inference

Start with a score. Declaration gives it a name and int64 type; initialization supplies seventy. Later assignment changes the current value rather than selecting a new type, explaining why the same score can later hold seventy-five.

Use let when the initializer provides enough type information. Use const to restrict changes through the binding. Inference and read-only access solve different problems, and ordinary const does not automatically meet every compile-time evaluation requirement, such as an array dimension.

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

### Code walkthrough

`int64 score = 70` supplies a type, name and initial value. `+= 5` reads that value, adds five and stores the result, so score is 75 before printing. `let count = 2` asks the compiler to infer a type from the initializer; omitting an explicit type does not omit initialization.

`const int64 limit = 100` prevents ordinary reassignment of that binding. The three `{}` placeholders receive score, count and limit in order, yielding 75, 2 and 100. The placeholder braces are formatting syntax rather than printed data. Distinguish declaration, mutation and read-only constraints when tracing the result.


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

### Code walkthrough

The floatingKind overloads differ by parameter type and return fixed markers, 32 and 64, to reveal overload selection. They do not calculate a float's size. The f suffix gives `1.0f` type float32, so single selects that overload; unsuffixed `1.0` selects float64.

The u suffix demonstrates unsigned literal typing. The destination in `uint8 narrow = 255u` also imposes a range: a u suffix does not allow an arbitrarily large value in uint8. Maximum is the largest uint64 value, and the second output verifies its full decimal representation. The first line contains the unsigned identity result, two overload markers and narrow, in that order.


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

### Code walkthrough

Original is int64 and narrowed is int32. `cast<int32>(original)` explicitly requests narrowing on the right-hand side, after which the result initializes narrowed. It does not change original's declared type.

Because 42 fits the destination range, the output remains 42. This input demonstrates explicit conversion syntax, not that every int64 value is suitable. Validate external values against business requirements and the destination range before converting; cast is not an input-validation function.


## 3.4 Scope and later use

A name can denote different bindings in different scopes. Initialize a variable before reading it; ownership rules additionally govern use after a move. Function parameter and return types extend these checks across calls.

### Summary

Check a variable's name, type, current value and writability separately. Let omits inferable type text, const restricts mutation and cast requests conversion. Valid representation may still need a narrower business range for age or grades.

The next chapter places these contracts at function boundaries. Parameter and result types retain the same compatibility and conversion rules during calls.

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
