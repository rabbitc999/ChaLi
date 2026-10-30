# Chapter 15 Testing and Debugging

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../15-测试与调试.md)
<!-- END LANGUAGE NAVIGATION -->

One successful run does not prove that the next edit preserves behavior. We will turn expectations into repeatable checks before building the log-search project.

## 15.1 Write an assertion

<!-- verify-book -->
```chali
int64 addBonus(int64 score, int64 bonus)
{
    return score + bonus;
}

void main()
{
    assert(addBonus(70, 5) == 75);
    assert(addBonus(0, 5) == 5);
    println("checks passed");
}
```

```text
checks passed
```

The first section is a calculation with two inputs and one result. It needs no files, keyboard, or network. In main, the first assertion compares a call with the expected 75. A true condition continues; a false one reports failure. The second assertion checks a different input, and the success message runs only after both pass.

Temporarily change the first expectation to 76 and observe the failure, then restore it. This also checks that your test is exercising the intended behavior.

## 15.2 Give each check a purpose

Related assertions can live in a function such as testOrdinaryScore. Naming it that way does not automatically run it; this style still calls the function from main.

Choose boundaries from the behavior you promise: a text search can find a match, miss it, receive empty text, differ in case, contain Unicode, or end without a newline.

<!-- verify-book -->
```chali
import std::text;

void main()
{
    assert(std::text::Text::contains("ERROR disk", "ERROR"));
    assert(!std::text::Text::contains("INFO ready", "ERROR"));
    println("search checked");
}
```

```text
search checked
```

The first assertion expects a match. The second negates contains and expects no match. Copying a case without changing its input or expectation may only repeat the same observation.

## 15.3 Separate calculation from external work

Which lines match does not depend on the file's directory. A function receiving text and returning hits can be tested with fixed strings. File access separately tests missing paths, read errors, and size limits. Later, request parsing uses fixed text before real TCP tests exercise fragmentation and disconnection.

Keep inputs, expected results, and actual diagnostics. Investigate a disagreement rather than removing a useful boundary check merely to make the suite pass.

## 15.4 Read an error back to its cause

First distinguish syntax checking, type checking, and runtime assertions. Reduce a failure to the necessary data and functions. Temporary prints can expose an intermediate value; remove them after understanding the problem. Native debugging uses a build with debug information; the [documentation index](../../en/README.md) points to the tools.

## 15.5 Exercises

1. Test a negative bonus.
2. Break an expectation and verify the success message disappears.
3. Add Unicode and empty-text search cases.
4. Which part of a log tool can be tested without disk access?

### Answers and checks

1. For example, assert that adding minus five to 70 produces 65.
2. Execution stops at the failed assertion and succeeds again after the fix.
3. Match exact UTF-8 substrings; a nonempty query in empty text is absent.
4. The pure search function can receive a literal string; file errors need separate checks.
