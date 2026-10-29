# Chapter 5 Control flow

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../05-控制流.md)
<!-- END LANGUAGE NAVIGATION -->

`if`, `elif` and `else` select a branch. `while` repeats while its condition holds; `do` executes a body before testing. `for-in` traverses an iterable. Keep the update step visible so that the termination condition can be checked.

`else if` is equivalent to `elif`; both may appear in the same chain. Statements and value expressions use the same branch semantics and evaluate only the conditions needed to select a branch. Every branch still requires braces, and a value expression requires a final else.

```chali
void main()
{
    int64 value = 0;
    int64 total = 0;
    while (value < 5)
    {
        total = total + value;
        value = value + 1;
    }
    println("{}", total);
}
```

`break` leaves the selected loop and `continue` starts its next iteration. Nested control flow can use the language's explicit level forms; prefer a small helper function when the relationship becomes hard to read. `return` exits a function, not just the nearest branch.

ChaLi also has value-producing control expressions, including loop and block forms with `yield` or valued exits. Their types must agree with the surrounding use. An expression form does not relax ownership, lifetime or missing-return checks.

Exercise: sum only the even integers below ten, then rewrite the traversal using an integer range. Check the exact endpoint convention in the range API. Next: [value semantics](06-value-semantics.md).
