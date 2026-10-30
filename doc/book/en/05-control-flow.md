# Chapter 5 Control flow

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../05-控制流.md)
<!-- END LANGUAGE NAVIGATION -->

Results depend on which statements execute, in what order and how often. Control flow selects branches, repeats operations and exits structures. This chapter connects each decision to concrete state changes.

Begin with if-produced values, then range accumulation, loop results and nested exits. Track both variable values and nesting depth, especially the distinct scopes affected by yield, break, continue and return.

## 5.1 Begin with a classification

<!-- verify-book -->
```chali
String classify(int64 value)
{
    if (value < 0)
    {
        return "负数";
    }
    elif (value == 0)
    {
        return "零";
    }
    else
    {
        return "正数";
    }
}

void main()
{
    println("{} {} {}", classify(-3), classify(0), classify(8));
}
```

```text
负数 零 正数
```

classify returns at the first matching branch. main calls it for -3, 0 and 8, producing one string each. Change the final input to zero before moving on to expression-valued branches.

## 5.2 Conditional chains and expressions

Begin with selecting a boolean based on whether p is odd. An if can choose a result on an assignment's right-hand side as well as choose statements. Both result sources then appear in one expression instead of separate later assignments.

Conditions produce bool and result arms supply compatible types. If, elif and else are tried in order, stopping after a match; else if is equivalent to elif. The example first uses final expressions, then yield after local steps.

<!-- verify-book -->
```chali
void main()
{
    int32 p = 5;
    bool odd = if (p % 2 == 1)
    {
        true
    }
    else
    {
        false
    };
    int32 category = if (p % 3 == 1)
    {
        1
    }
    elif (p % 3 == 2)
    {
        2
    }
    else
    {
        3
    };
    odd = if (p % 2 == 0)
    {
        false
    }
    else
    {
        bool value = true;
        yield value;
    };
    println("{} {}", odd, category);
}
```

```text
1 2
```

P is 5, so its remainder modulo two is one and the first if produces true for odd. For category, the remainder modulo three is two: the first condition fails, elif succeeds and supplies 2. The else branch does not execute.

The later assignment to odd tests evenness, fails and enters else. That block declares a local value and explicitly yields it. Earlier branches use a final expression; this block uses statements followed by yield. Odd ends true either way. Boolean formatting here displays true as 1, giving `1 2`. Branch results must still have compatible types.


## 5.3 Loops and ranges

For-in traverses a range or iterator protocol. Range excludes its upper bound, while rangeEq includes it. While tests before each iteration; loop relies on exits in its body. Define endpoints and state updates before counting iterations.

<!-- verify-book -->
```chali
void main()
{
    int64 total = 0;
    for (int64 k in range(0, 5))
    {
        total = total + k;
    }
    println("{}", total);
}
```

```text
10
```

Total is initialized outside the loop so every iteration updates the same accumulator. `range(0, 5)` yields 0, 1, 2, 3 and 4, excluding five. Each yielded value is added to total.

The successive totals are 0, 1, 3, 6 and 10. Println runs once after iteration, not once per element. Moving the declaration inside would reset the accumulator each time. Whether an interval includes its upper endpoint comes from the API contract rather than an informal reading of “to five.”


## 5.4 Obtain a result from a loop

A value-producing loop uses yield. An else branch handles completion without such a result. A valueless loop cannot serve as an integer expression. The example covers both a found value and a no-match path.

<!-- verify-book -->
```chali
void main()
{
    int64 found = for (int64 value in range(0, 10))
    {
        if (value > 5)
        {
            yield value;
        }
    }
    else
    {
        yield -1;
    };
    String label = while (false)
    {
        "运行"
    }
    else
    {
        "结束"
    };
    println("{} {}", found, label);
}
```

```text
6 结束
```

The first for appears on an assignment's right-hand side. It examines zero through nine; the first value greater than five is six. Yield supplies six as the for expression's result and ends the search. Else supplies minus one only when the body has not produced a result.

The while condition is false immediately, so its body never supplies its string. Else supplies the skipped label instead, yielding the shown output. Here loop else completes the path that produced no body result; its execution should be understood from loop-expression semantics, not guessed from an ordinary if.


## 5.5 Exit levels and cleanup

Continue advances an iteration and break exits a loop. Nested loops support break level N and break all; a value exit needs a receiver. Leaving scopes triggers cleanup, covered in Chapter 8.

Increment and compound assignment mutate their target. Split dependent reads and writes into ordered statements.

<!-- verify-book -->
```chali
void main()
{
    int64 visited = 0;
    loop
    {
        loop
        {
            visited++;
            break level 2;
        };
        visited += 100;
    };

    int64 total = 0;
    List<int64> values;
    values.push(2);
    values.push(4);
    values.push(6);
    for (item in values)
    {
        total += item;
    }
    println("{} {}", visited, total);
}
```

```text
1 12
```

Visited begins at zero. The inner loop increments it to one and `break level 2` exits both enclosing loops. Consequently the outer body's addition of 100 is skipped. Two denotes loop nesting depth, not two separate break operations or two function returns.

The second part builds a list containing two, four and six, then adds each item to total to obtain twelve. The output combines independent results: exit scope explains visited equal to one, and container iteration explains total equal to twelve. Trace both state updates and the nesting boundaries crossed by exits.


### Summary

Ask when the condition is tested, where state changes and where each exit leads. For value-producing structures also ask whether every reachable path supplies a compatible result. This separates missing updates, endpoint mistakes and missing-result paths.

Exits also leave scopes. Chapter 8 adds cleanup actions to these same paths once their control flow is clear.

## 5.6 Exercises

1. Count range(0,5) iterations.
2. Change it to rangeEq.
3. Provide a no-match search result.
4. Compare break level 2 and return.

### Answers and checks

1. Five iterations, with values 0 through 4.
2. It includes 5, giving six iterations.
3. Use a compatible result in else.
4. The first exits two loops; the second exits the function.

Next: [Chapter 7](07-value-semantics.md).
