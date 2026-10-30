# Chapter 4 Functions

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../04-函数.md)
<!-- END LANGUAGE NAVIGATION -->

A bonus calculation is small, but repeating it in three places invites inconsistent edits. A function gives the work a name, accepts inputs, and returns a result.

## 4.1 Follow a call

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

Start in main. Its first statement creates score with value 70. The next calls `addBonus(score, 5)`, temporarily entering the earlier function with arguments 70 and 5.

In the function header, the first int64 names the result type, addBonus names the function, and the declarations in parentheses are parameters. Arguments match them by position, not by matching variable names.

The first body statement updates this call's score. `return score;` then returns 75 and ends the call. main saves that value in result and continues to println.

The original score remains 70 because ordinary integers are passed by value. An identical parameter name does not establish shared modification. Chapter 8 introduces explicit mutable references.

## 4.2 Supply a default

If five is the usual bonus, keep the body and change the header:

```chali
int64 addBonus(int64 score, int64 bonus = 5)
```

`addBonus(70)` uses five, while `addBonus(70, 10)` uses the supplied ten. A default fills an omitted argument rather than replacing an explicit one.

Overloads instead distinguish functions by parameter types or counts. A result type alone does not distinguish overloads. Chapter 3's floatingKind functions used different parameter types.

## 4.3 No result, or several related values

A function that performs work without returning a business value uses void. A bare return can end it early. A function returning int64 must supply an appropriate result on its normal exit paths.

Related values can be grouped in a tuple:

```chali
let pair = (70, 80);
let (first, second) = pair;
```

The first line creates a pair. The second destructures it, assigning 70 and 80 to the two names. Functions can return tuples too; Appendix C collects the full type and destructuring forms.

Variadic parameters accept several arguments of one type. First identify the parameter's element type and how it is consumed; there is no need to combine every function feature in one first example.

## 4.4 Make the work clear

The name addBonus and its parameters explain a calculation. Hidden file access or unrelated mutation would make its behavior harder to predict. Later chapters express such relationships through explicit parameters and error results.

return accepts a computed expression. After Chapter 5 introduces control flow, Appendix C shows compact combinations with returning control expressions.

## 4.5 Exercises

1. What is main's score after addBonus(70, 5)?
2. Rename the parameter score to original. Which references must change?
3. Compare an omitted bonus with an explicit bonus of ten.
4. How does returning 75 differ from printing 75?

### Answers and checks

1. It remains 70; result receives 75.
2. Change the header and references in that function body, not main's variable.
3. The default produces 75 and the explicit ten produces 80.
4. A returned value can be saved and calculated with; printed output does not substitute for it.
