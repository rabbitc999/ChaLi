# Chapter 19 Preprocessing and hygienic macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../19-预处理与宏.md)
<!-- END LANGUAGE NAVIGATION -->

Macros transform program structure during compilation. They are useful when repeated syntax expresses one stable operation, but ordinary functions remain the clearer choice for most runtime calculations.

ChaLi's hygienic macros preserve binding relationships so that an internal temporary does not accidentally capture a caller's variable. Expression arguments are evaluated once each, in source order, and their saved values are reused: `$A(next())` calls next once and `$A(next(), next())` calls it twice. Ordinary parameters have value semantics. Assigning `n = n + 1;` changes the parameter's copy without modifying the caller or producing a warning. The compiler may eliminate a copy only when observable behavior stays equivalent.

Declare `mut &n` for a writable reference parameter or `&n` for a readonly reference. The former writes through to the caller; the latter rejects mutation. Identifier, type, statement and token-group parameters retain their distinct syntax roles. Macro arguments should not be treated as one unstructured string.

Macro declarations and invocations use the lowercase `$macro` system. Parameter groups and repetition describe how source fragments are matched. Regex branches use the documented ECMAScript behavior. Consult the [macro reference](../../en/25-literals-operators-macros.md) for the exact declaration forms rather than copying another language's macro syntax.

Generated code still undergoes semantic checking. Expansion does not bypass types, references, resource ownership or the compiler's bounded fragment budgets. When debugging, inspect the expansion and its source mapping. The VS Code expansion view groups related results into tabs, with searchable macro names and a tree of expansion levels.

`#res` explicitly selects the expected result type at the call site and can be combined with an argument source:

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

T is int32, U is int16, and the result is 7. Initialization, assignment, function returns and function arguments with a known parameter type can supply the expected result type. Inference does not evaluate either argument. `<T>` retains the same implicit result-source behavior. Explicit type arguments take priority. Without an expected type, `let` does not make T infer from a instead.

Exercise: identify a repeated source pattern, first implement it as a function, then explain which additional requirement would justify a macro. Review variable names at the call site to understand why hygiene matters. Next: [reflection](20-reflection.md).
