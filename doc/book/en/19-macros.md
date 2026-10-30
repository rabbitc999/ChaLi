# Chapter 19 Hygienic macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../19-预处理与宏.md)
<!-- END LANGUAGE NAVIGATION -->

Macros compose syntax during compilation. Hygiene prevents accidental capture of caller names, while parameter categories determine whether an argument is evaluated as a value or inserted as syntax.

## 19.1 Value parameters, references and hygiene

Unannotated parameters use single-evaluation value semantics. One argument in $A(next()) is evaluated once; the two arguments in $A(next(), next()) are each evaluated once. Modifying a value parameter changes its copy, leaves the caller unchanged, and does not warrant a warning. Copies may be optimized when behavior is preserved.

Explicit &n borrows read-only and mut &n permits caller mutation, subject to static borrowing. Internal temporary names remain hygienic.

## 19.2 Parameter categories control expansion

x:value keeps single evaluation. x:expr accepts expression syntax evaluated where inserted: two placements may execute twice and no placement does not execute it. name:ident, T:type, body:stmt and body:block require their corresponding syntax categories.

Value arguments are evaluated in argument order before expr placements execute in the expanded body. Do not assume expr preserves single evaluation for side effects.

<!-- verify-book -->
```chali
int64 count = 0;
int64 next() { count += 1; return count; }
$macro TwiceValue(x: value)
x + x
$end_macro
$macro TwiceExpr(x: expr)
x + x
$end_macro
void main() {
    int64 value = $TwiceValue(next());
    println("{} {}", value, count);
    count = 0;
    int64 expression = $TwiceExpr(next());
    println("{} {}", expression, count);
}
```

```text
2 1
3 2
```

## 19.3 Generic parameters and result context

Macros may declare type parameters. T:#res infers from the statically expected result type, while U:b infers from argument b. Neither runs the argument expression during compilation. Explicit arguments take priority; missing context or conflicting constraints require more type information.

For example, $macro Example<T:#res, U:b>(a, b) combines result and argument information. Its inferred types still must satisfy the body and call-site checks.

<!-- verify-book -->
```chali
$macro Example<T:#res, U:b>(a, b)
{
    T left = cast<T>(a);
    U right = b;
    yield left + cast<T>(right);
}
$end_macro
void main() {
    int32 result = $Example((int64)3, (int16)4);
    println("{}", result);
}
```

```text
7
```

## 19.4 Standard macros and extension boundaries

When using existing standard macros, retain the element types, evaluation counts and return semantics. This tutorial uses implemented categories and expansion forms. Declaration macros are deferred; declaration generation, arbitrary result categories and unsupported repetition matching are not available features.

<!-- verify-book -->
```chali
$macro vec<T:first>[first,...rest]
{
    List<T> result;
    result.push(first);
    $for element in rest
    result.push(element);
    $end_for
    yield result;
}
$end_macro

void main()
{
    List<int64> result;
    result.push(99);
    List<int64> values = $vec[1, 2, 3];
    println("{} {} {}", values.len(), values.at(2), result.at(0));
}
```

```text
3 3 99
```

## 19.5 Diagnostics and checking

For a macro diagnostic, distinguish definition and call sites, then inspect categories, type arguments, scope and borrowing. The following hygiene example is rejected because an internal temporary is not a caller-visible name.

<!-- verify-book-error: temp -->
```chali
$macro broken(value)
{
    temp.push(value);
    yield temp;
}
$end_macro

void main()
{
    List<int64> temp;
    List<int64> values = $broken(1);
}
```

## 19.6 Exercises

1. Compare next call counts with value and expr.
2. Does changing an unannotated parameter change the caller?
3. Does T:#res come from a runtime return value?
4. Why cannot a caller read internal temp?

### Answers and checks

1. Value evaluates once per argument; expr follows placements and execution.
2. No; it changes a copy. Use an explicit mutable reference to change the caller.
3. No; it comes from the statically expected result type.
4. Hygiene prevents capture; internal names do not become caller declarations.

Next: [Chapter 20](20-reflection.md).
