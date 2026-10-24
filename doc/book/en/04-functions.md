# Chapter 4 Functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../04-函数.md)
<!-- END LANGUAGE NAVIGATION -->

A function declares its result type, name and parameters. Each call must match an applicable overload. The body should make every returning path clear; a loop that might exit does not excuse a missing return.

```chali
int64 add(int64 left, int64 right)
{
    return left + right;
}

void main()
{
    println("{}", add(20, 22));
}
```

Parameters passed by value follow value semantics. For a large object that only needs inspection, an explicit reference can avoid copying. Mutable references state that the function may modify the caller's object. Those choices affect borrowing and lifetime checks as well as performance.

Overloads are selected from parameter types and the arguments supplied. Default arguments belong to the declared interface; they should make a call clearer rather than create ambiguous short forms. In the editor, signature help shows one selected overload and narrows candidates as arguments become known.

Exercise: add a function that returns the larger of two integers. Verify both branches and the equal-input case. Later, [function values and closures](15-functions-closures.md) explain storing and returning callable values. Next: [control flow](05-control-flow.md).
