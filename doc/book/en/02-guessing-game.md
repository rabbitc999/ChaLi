# Chapter 2 A guessing game

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../02-猜数字.md)
<!-- END LANGUAGE NAVIGATION -->

The previous program ran straight through. Guessing introduces decisions: retain an answer, read guesses repeatedly, compare them and choose when to stop. It forms a complete small problem for learning control flow.

Fix the answer at 42 to make execution reproducible. Track each input, selected branch and return to the loop head. Only then add randomness or attempt limits, so the effect of each new requirement remains clear.

## 2.1 Separate the interaction into steps

Describe one round before writing syntax: read a guess, compare it and give feedback. End on a match; otherwise repeat. Establishing the actions first makes choosing language constructs easier.

The answer persists across rounds and belongs outside the loop. A fresh guess is used only in its current round. A later attempt counter also needs cross-round state, so the same placement reasoning applies.

## 2.2 Implement and run

The complete program reads integers with inputInt. Enter 30, 60 and 42 on separate lines. The output below excludes terminal echo of the input.

<!-- verify-book-input: "30\n60\n42\n" -->
```chali
void main()
{
    int64 answer = 42;
    while (true)
    {
        int64 guess = inputInt();
        if (guess < answer)
        {
            println("small");
        }
        elif (guess > answer)
        {
            println("large");
        }
        else
        {
            println("found");
            break;
        }
    }
}
```

```text
small
large
found
```

### Code walkthrough

The answer is set to 42 before iteration and remains fixed. `while (true)` has no condition that ends it automatically. Each iteration reads a fresh guess with `inputInt()` and chooses exactly one of the less-than, greater-than or equal branches. An `elif` is tested only after the preceding condition fails.

Trace the supplied input in order: a lower guess prints small and a higher guess prints large, after which both paths read again. A matching guess prints found and `break` exits the nearest while. Removing break would leave the program waiting for more input even after a match. This demonstrates control flow with valid integers, not complete handling of malformed input or end of input.


## 2.3 Branches and state

If, elif and else form one branch chain. Else if is equivalent to elif. Break exits the current loop. To count guesses, keep a counter outside the loop and update it at one defined point after reading.

A while(true) loop needs an explicit exit path. Without break it requests further input, so tests should exercise the terminating branch.

## 2.4 Handle input errors at the appropriate layer

This example assumes input accepted by the integer reader. Text tools commonly read a whole line, parse it with std::parse and handle errors through Result. Chapter 12 introduces text and Chapter 16 error returns. Invalid input and an incorrect guess are different outcomes. Keep fixed-answer tests when adding random answers.

### Summary

A round reads an integer, compares it, prints feedback and either continues or exits. The answer persists across rounds while guess belongs to one round. Declaration placement follows the required lifetime; an attempt counter likewise needs an explicit persistence scope.

This chapter assumes valid integer input. Chapter 3 explains representation types, and Chapter 16 supplies ways to express parsing failure and absent results.

## 2.5 Exercises

1. What is printed for input 42?
2. Count guesses.
3. Replace elif with else if.
4. Why is the answer outside the loop?

### Answers and checks

1. Only found is printed.
2. Initialize outside and increment after each read.
3. Preserve branch order and conditions.
4. The answer persists across iterations; a guess belongs to one iteration.

Next: [Chapter 3](03-variables-types.md).
