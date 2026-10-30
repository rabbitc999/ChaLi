# Chapter 1 Getting Started

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../01-第一个程序.md)
<!-- END LANGUAGE NAVIGATION -->

Our first program displays one message. It has no input and creates no files, so its result is easy to check.

## 1.1 Save a source file

Follow [installation](../../en/39-installation-packages.md), then create a UTF-8 text file named hello.chl.

<!-- verify-book -->
```chali
void main()
{
    println("Hello, ChaLi!");
}
```

```text
Hello, ChaLi!
```

The first line, `void main()`, declares the entry function. main is its name; empty parentheses mean this example takes no parameters. void means it does not return a value to a caller. Chapter 4 will introduce functions that calculate and return results.

The opening brace starts the function body and the closing brace ends it. The statement between them is what this program executes.

The third line calls println. The quoted text is its argument; the quotes mark the text and are not printed. println adds a newline. The final semicolon ends the statement, while the closing brace ends the body.

## 1.2 Run it in a terminal

Open a terminal in the directory containing hello.chl:

```sh
chali run hello.chl
```

chali names the compiler, run asks it to compile and execute, and hello.chl names the input file. The resulting output should match the example.

If the command cannot be found, check PATH or use the compiler's full path. If the source cannot be found, check the terminal directory and saved filename. Neither problem is caused by the text passed to println.

## 1.3 Make two small changes

Replace the greeting with your name. Run again, then add another statement below the first:

```chali
    println("This is my second line.");
```

The calls execute from top to bottom. Remove the second semicolon deliberately, read the first diagnostic, and restore it. Change one thing at a time so you can connect a result to its cause.

## 1.4 Check without running

```sh
chali check hello.chl
```

A successful check does not print the greeting: it checks the program without executing main. run executes after checking and compiling. This distinction matters once programs read input, open files, or use networks.

For now, know that run compiles a native program and executes it. Chapter 27 explains export and compilation tools; you do not need LLVM knowledge to continue.

## 1.5 Exercises

1. Print your name and one learning goal on separate lines.
2. Put a semicolon inside the quoted text and compare its role with the final semicolon.
3. Record the output of check and run.
4. Misspell main, read the diagnostic, and fix it.

### Answers and checks

1. Use two println statements in the same function body.
2. The quoted semicolon is data; the unquoted one ends a statement.
3. A successful check prints no greeting; run executes the calls.
4. A valid entry is required. Check again before comparing runtime output.

The next chapter responds to input. Keep this small file as a way to check your tools when a larger program fails.
