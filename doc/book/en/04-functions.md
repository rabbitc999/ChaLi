# Chapter 4 Functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../04-函数.md)
<!-- END LANGUAGE NAVIGATION -->

Functions separate score calculations so their results and parameter behavior can be checked independently. This chapter uses ordinary numeric parameters; Chapters 7 and 8 introduce references and resources.

## 4.1 Parameters and return types

A declaration specifies a return type, name and parameter list. Arguments follow parameter order and the result can initialize another value. For copyable values, mutation of a value parameter does not write back to the caller binding.

<!-- verify-book -->
```chali
int64 addBonus(int64 score, int64 bonus)
{
    score += bonus;
    return score;
}

void main()
{
    int64 score = 70;
    int64 result = addBonus(score, 5);
    println("{} {}", score, result);
}
```

```text
70 75
```

## 4.2 Overloads, defaults and variadics

Overloads are selected by argument types and counts, not return type alone. Defaults fill omitted trailing arguments. Variadic declarations accept variable argument counts under their declared element rules. Resolve ambiguity with types or a clearer interface rather than declaration order.

## 4.3 Multiple results and value-producing control flow

Several results can form a tuple and be destructured by the caller. Return can consume a value-producing block, condition or loop. Yield supplies the receiving control expression; return exits the function. Distinguish these exits before reasoning about cleanup and subsequent statements.

<!-- verify-book -->
```chali
int64 fromBlock()
{
    return
    {
        int64 local = 3;
        yield local;
    };
}

int64 choose(bool first)
{
    return if (first)
    {
        yield 5;
    }
    else
    {
        yield 6;
    };
}

int64 fromMatch(int64 item)
{
    return match (item)
    {
        0 => { yield 7; }
        _ => { yield 8; }
    };
}

int64 fromLoop()
{
    return loop
    {
        yield 9;
    };
}

void main()
{
    println("{} {} {} {}", fromBlock(), choose(false), fromMatch(0), fromLoop());
}
```

```text
3 6 7 9
```

## 4.4 Make the call contract explicit

Distinguish readonly inputs, writable targets and fallible computations using value/reference parameters and Option/Result returns. An executable main returns void, either with no parameters or a List<String> argument list. Native library entry points are covered in Chapters 21 and 24.

## 4.5 Exercises

1. What is score after addBonus?
2. Add a float64 overload and select it with typed arguments.
3. Return a total and count as a tuple.
4. Identify the receivers of yield and return.

### Answers and checks

1. Score remains 70; the result is 75.
2. Use distinct parameter types and check overload selection.
3. Declare a tuple result and destructure it at the call site.
4. Yield supplies a control expression; return exits a function.

Next: [Chapter 5](05-control-flow.md).
