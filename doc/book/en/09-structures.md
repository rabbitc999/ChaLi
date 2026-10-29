# Chapter 9 Structures

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-结构体.md)
<!-- END LANGUAGE NAVIGATION -->

A structure gives related values a name. Fields should describe one coherent piece of state; functions and methods can then enforce operations on that state.

A named `static T create(...)` entry initializes an implicit `self`. It may fall through, use `return;` for early completion, or explicitly write `return self;`. Returning self ends initialization without copying or moving the receiver; the caller takes the constructed value. Other return expressions are rejected.

```chali
struct Point
{
    int64 x;
    int64 y;
};

void main()
{
    Point first;
    first.x = 3;
    first.y = 4;
    Point second = first;
    second.x = 10;
    println("{} {}", first.x, second.x);
}
```

Structure assignment follows the value semantics of its fields. A field owning a string or container is not silently turned into shared mutable storage. A borrowed field still carries the original lifetime requirement.

Constructors and methods can express valid initialization and operations. Generic structures reuse one shape for several element types. Derivation can generate supported operations such as equality, hashing and cloning; it does not justify inventing arbitrary implementations for unsupported contracts.

Reflection is explicit metadata about declared types, not a reason to bypass their resource rules. The student-system chapter uses a reflected structure as its model. Exercise: add a label to `Point`, copy a value and modify the copy's label independently. Next: [enums and patterns](10-enums-patterns.md).
