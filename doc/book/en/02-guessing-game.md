# Chapter 2 A Guessing Game

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../02-猜数字.md)
<!-- END LANGUAGE NAVIGATION -->

Let us write a program that answers a guess. It holds the number 42 and responds with too small, too large, or found. A fixed answer makes it possible to compare the same inputs on every run.

## 2.1 Compare once

Place these statements in main:

```chali
int64 answer = 42;
int64 guess = 30;
println("{}", guess < answer);
```

The first two lines store the answer and guess. The comparison produces a bool; the default formatter prints the true result as 1. Change 30 to 60 and it prints 0.

## 2.2 Choose a message

Replace the print with:

```chali
if (guess < answer)
{
    println("small");
}
else
{
    println("not small");
}
```

if evaluates its condition and executes the first body when true. Otherwise it executes the else body. Only one branch runs. This version still combines larger and equal guesses, so we will add another comparison and repeat the process.

## 2.3 Read a guess each round

Save the complete program as guess.chl. Enter 30, 60, and 42 on separate lines. The expected output excludes the terminal's input echo.

<!-- verify-book-input: "30\n60\n42\n" -->
```chali
void main()
{
    int64 answer = 42;
    loop
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
    };
}
```

```text
small
large
found
```

The first section enters main and sets answer once, outside the loop. int64 is the integer type used here; Chapter 3 examines types in more detail.

The second section starts at `loop`. Its body repeats while the condition is true. Each round calls inputInt to obtain a new guess, because the read is inside the loop.

The third section chooses a response. Input 30 takes the smaller branch. Input 60 reaches elif only after the first condition fails. Input 42 satisfies neither comparison and reaches else. There, println runs before break exits the loop. Without break the program keeps waiting. `else if` is another spelling of `elif`.

## 2.4 Count the guesses

A counter must remember previous rounds, so declare it outside the loop, increment after reading, and print it when the guess is correct. Declaring it inside the loop resets it each time.

This first version assumes valid integer input. Chapter 14 handles invalid input and end-of-input as errors rather than incorrect guesses. For now, test all three normal paths.

## 2.5 Exercises

1. What happens if the first input is 42?
2. Add and print a guess counter.
3. Replace elif with else if and repeat the inputs.
4. Why does removing break prevent termination after a correct guess?

### Answers and checks

1. It prints found and exits the loop.
2. Initialize to zero outside the loop and increment after each read; the sample needs three guesses.
3. The output remains small, large, found.
4. The loop condition remains true and its explicit exit has been removed.
