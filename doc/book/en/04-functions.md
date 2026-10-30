# Chapter 4 Functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../04-函数.md)
<!-- END LANGUAGE NAVIGATION -->

A function packages repeated score calculations with parameters for varying inputs and a return value for the result. This supports reuse and isolated checking without duplicating implementation at every call site.

Start with integer value parameters, then examine blocks and control expressions that produce return values. Understanding an ordinary call provides the baseline for later reference parameters, ownership transfer and function values.

## 4.1 Parameters and return types

Treat adding a bonus as a separate task. The caller supplies the original score and bonus; the function returns the calculation. Its signature records two parameters and a result type, while the body provides the steps.

These integer parameters are passed by value. The score inside the call is a different binding from main's score, and result retains the returned value. Printing both makes that relationship observable.

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

### Code walkthrough

Main starts with score equal to 70. In addBonus, the arguments initialize the local value parameters score and bonus. Updating that parameter produces 75, and return supplies the value used to initialize result.

Execution resumes at main's println. The caller's score is still 70 while result is 75, explaining the two different numbers. Changing the caller requires an explicit mutable-reference interface; giving a parameter the same name as a caller variable does not establish shared storage.


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

### Code walkthrough

The four functions use a block, if, match and loop as value-producing expressions, then return their results. Yield supplies a value to the current expression structure; function return acts at a different level. FromBlock yields the local integer 3.

`choose(false)` selects else and produces 6. `fromMatch(0)` selects the zero arm and produces 7. FromLoop yields 9 on its first iteration, ending that loop expression. Main therefore prints 3, 6, 7 and 9. Every produced value must still satisfy the declared int64 return type; control expressions do not bypass type checking.


## 4.4 Make the call contract explicit

Distinguish readonly inputs, writable targets and fallible computations using value/reference parameters and Option/Result returns. An executable main returns void, either with no parameters or a List<String> argument list. Native library entry points are covered in Chapters 21 and 24.

### Summary

For each call identify arguments, parameters, local changes and the return point. A change inside the function does not establish caller mutation; inspect whether the interface passes a value or a borrow and whether writes are permitted.

When a return expression contains if, match or a loop, first determine that expression's result and then the function return. Chapter 5 follows these control structures and their result receivers in detail.

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
