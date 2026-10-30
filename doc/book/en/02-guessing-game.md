# Chapter 2 A guessing game

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../02-猜数字.md)
<!-- END LANGUAGE NAVIGATION -->

Connect input, comparison and branching in an interactive loop. Fix the answer at 42 so a sequence of inputs produces reproducible output. Randomization can be added after the control flow is understood.

## 2.1 Separate the interaction into steps

Each iteration reads an integer, compares it with the answer, prints small or large, or prints found and exits. The answer belongs to the outer scope; each guess belongs to its iteration.

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

## 2.3 Branches and state

If, elif and else form one branch chain. Else if is equivalent to elif. Break exits the current loop. To count guesses, keep a counter outside the loop and update it at one defined point after reading.

A while(true) loop needs an explicit exit path. Without break it requests further input, so tests should exercise the terminating branch.

## 2.4 Handle input errors at the appropriate layer

This example assumes input accepted by the integer reader. Text tools commonly read a whole line, parse it with std::parse and handle errors through Result. Chapter 12 introduces text and Chapter 16 error returns. Invalid input and an incorrect guess are different outcomes. Keep fixed-answer tests when adding random answers.

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
