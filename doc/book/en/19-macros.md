# Chapter 19 Hygienic macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../19-预处理与宏.md)
<!-- END LANGUAGE NAVIGATION -->

Some repetition concerns program structure rather than repeated calls to one function. Macros compose syntax during compilation to reuse that structure, making evaluation count, name origins and type context essential to the interface.

Compare value parameters with expression fragments, then inspect result inference, variadic loops and hygiene errors. Examples use delivered behavior; declaration generation and compile-time field iteration remain boundaries rather than usable syntax.

## 19.1 Value parameters, references and hygiene

Start with an argument next() that changes state. A macro reusing one result should evaluate once and reuse that value. A macro accepting expression syntax may execute at each placement. Similar-looking calls therefore need an interface that makes the difference explicit.

Unannotated parameters use single evaluation per argument: one next() argument runs once, while two separate arguments each run once. Mutating the parameter copy does not update the caller, and copy elimination must preserve behavior. The next section makes value versus expr visible with a counter.

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

### Code walkthrough

Each next call increments the global count and returns its new value. TwiceValue evaluates next() once to obtain one, then reuses that value in both additions. Its result is two while count is one, explaining `2 1`.

After resetting count, TwiceExpr places expression syntax at both use sites. Runtime calls return one and two, yielding three with count two. Thus `3 2`. Identical-looking x+x bodies have different evaluation behavior because their categories differ, affecting input reads, state updates and resource acquisition.


### Choose categories by evaluation behavior

Decide whether the macro should reuse a computed value or place syntax at a new execution point. Input reads, counter increments and resource acquisition need explicit evaluation counts; ordinary value parameters are usually suitable. Choose expr when the expanded control structure must determine execution placement, and document that execution may be skipped or repeated. Copy costs and effects still follow the argument type's copy rules.

Ident carries a name, type carries type syntax, and stmt and block carry statements or blocks. These categories describe syntax positions. A generic parameter T and a parameter written T:type have different roles. Category matching does not exempt names, types, borrows or ownership from ordinary checking, and an identifier does not grant permission to mutate its object.

Hygiene associates names with their origins. A template's result can coexist with the caller's result without overwriting it. An undeclared temp in the template cannot be repaired by an unrelated caller variable of that name. Reusable macros therefore do not require callers to avoid a hidden list of temporary names. Interfaces should state explicitly when they affect caller objects.

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

### Code walkthrough

T:#res obtains int32 from result's expected type. U:b obtains int16 from the second argument. Although the first argument is int64, cast<T> converts it for left. Right initially keeps U and is converted to T at the addition.

The sum therefore fits the result context and yields seven. Inference uses static information rather than executing b to discover its type. If context is missing or constraints conflict, supply explicit types or correct the call instead of expecting runtime adaptation.


## 19.4 Standard macros and extension boundaries

When using existing standard macros, retain the element types, evaluation counts and return semantics. This tutorial uses implemented categories and expansion forms. Declaration macros are not yet delivered; declaration generation, arbitrary result categories and unsupported repetition matching are not available features.

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

### Code walkthrough

Vec uses a bracket call group. First provides both the initial item and T's inference source, while rest collects later arguments. Its internal List<T> receives first, and compile-time $for generates a push for each remaining argument. Those pushes execute when the generated program runs; yield supplies the completed list.

The call produces three entries, and at(2) returns three because indexing starts at zero. The caller's separate result contains 99. Hygiene keeps the macro's result distinct, so the last output remains 99. Compile-time $for differs from runtime for, and this supported repetition does not imply arbitrary pattern matching.


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

### Code walkthrough

Broken uses temp without declaring it in the template's scope or receiving its source as a parameter. Main's equally spelled temp belongs to the caller. Hygienic name resolution does not connect those identities automatically.

The example therefore fails during compilation and has no normal runtime output. Repair the interface by explicitly supplying the needed object, or declare a genuine local list inside the macro. A hidden caller variable of the same name is not a repair. Value supplies an element, not a list source.


### Expansion and execution occur at different stages

Expansion composes program structure during compilation; next in the examples still executes at runtime. Knowing an expression's static type does not execute that expression. T:#res obtains information from an expected type supplied by contexts such as assignment or return; it does not observe a runtime result. Supply explicit types when context is insufficient rather than depending on incidental literal inference.

Expanded code undergoes ordinary checking and then reaches the quadruple IR and LLVM backend. Macros cannot remove cleanup obligations, bypass borrowing or make an invalid expression executable. Performance depends on generated operations and LLVM optimization; fewer template lines do not establish less runtime work. For diagnostics, first express the intended operation as ordinary code, verify types and resource relationships, then inspect whether the macro preserves them.

The fixed parameter categories and existing variadic loop shown here are available. Explicit result categories, module-level declaration generation, compile-time field iteration, member macros and custom derive extensions are not yet delivered. Proposed forms are not current examples. The next chapter's runtime field queries have different stages and interfaces from compile-time AST generation over fields.

### Summary

Prefer a regular function when it expresses the requirement; choose macros for necessary syntax composition. Identify which arguments become values, which remain fragments, where names originate and how result types are determined.

Expansion does not execute arbitrary user runtime logic or legalize invalid borrowing and resource use. Runtime reflection next addresses metadata-based object access, a different mechanism from compile-time syntax generation.

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
