# Chapter 5 Control flow

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../05-控制流.md)
<!-- END LANGUAGE NAVIGATION -->

Control flow selects statements, repetition and the path that produces a value. Filter scores with conditions and accumulate with loops, checking which statements run after an exit.

## 5.1 Conditional chains and expressions

If, elif and else inspect conditions in order; else if is equivalent to elif. Conditions are bool. A conditional expression requires compatible branch result types, while a statement does not require branch values.

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

## 5.2 Loops and ranges

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

## 5.3 Obtain a result from a loop

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

## 5.4 Exit levels and cleanup

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
        }
        visited += 100;
    }

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

## 5.5 Exercises

1. Count range(0,5) iterations.
2. Change it to rangeEq.
3. Provide a no-match search result.
4. Compare break level 2 and return.

### Answers and checks

1. Five iterations, with values 0 through 4.
2. It includes 5, giving six iterations.
3. Use a compatible result in else.
4. The first exits two loops; the second exits the function.

Next: [Chapter 6](06-value-semantics.md).
