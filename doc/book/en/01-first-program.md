# Chapter 1 Your first program

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../01-第一个程序.md)
<!-- END LANGUAGE NAVIGATION -->

Begin with a repeatable minimum workflow: write a source file, check it, run it and explain what appears. A single output call separates tool setup from application logic. Later input, data and resource examples build on the same workflow.

Distinguish source code, terminal commands and recorded output. They serve different purposes and do not belong in one source file. By the end, identify main, explain which command executes it and state what successful checking actually establishes.

## 1.1 Prepare the compiler and toolchain

Follow the [installation guide](../../en/39-installation-packages.md) for platform requirements. Commands use chali from PATH. A Windows source build has a versioned filename such as chali-1.0.11-llvm-external-windows-x64.exe; use its actual path. On Linux, an executable in the current directory is invoked as ./chali.

The external LLVM variant needs a configured toolchain. The embedded variant carries its corresponding compiler components. Compiler availability and supported export targets are separate checks.

## 1.2 A source file and its entry point

Create a text file named `hello.chl` and save the complete example below. Its extension helps tools recognize source, while its contents determine behavior. Leave out extra inputs and functions for now so the first run has one output to explain.

Execution enters main, performs the call inside its braces and leaves the function. Observe that sequence before separating the roles of return type, name, string and semicolon.

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

### Code walkthrough

Read `void main()` first: `void` means no business value is returned, `main` is the execution entry, and the empty parentheses declare no parameters. The body contains one call, so execution prints once and then leaves the function.

The quotes delimit the string argument and are not printed. `println` adds a newline after `Hello, ChaLi!`, explaining the single output line. The semicolon ends the call statement; braces delimit the function body. With no input or branches, the result follows directly from this one call.


## 1.3 Check and run

Run the commands below in the source directory. Check performs frontend analysis without executing the application. Run compiles checked native code and executes it on the host. Input, file access and application side effects occur during execution. When checking fails, address the first diagnostic before assessing later messages.

```sh
chali check hello.chl
chali run hello.chl
```

### Code walkthrough

These are terminal commands, executed in order, rather than statements to place in hello.chl. `check` examines the file without printing the greeting. `run` compiles the same input and, if compilation succeeds, executes main to produce it.

The source path is relative to the terminal's working directory. A missing-file error calls for checking that directory and the saved filename; an unknown chali command calls for checking the compiler path. Only the second command runs user code, so the first command's success does not validate the output.


## 1.4 Compilation stages

The frontend parses declarations and expressions and checks types, borrowing, resources and control flow. Normal run/aot paths generate LLVM IR from the checked AST, then optimize, generate machine code and link. LLVM IR differs from ChaLi quadruples; neither needs manual editing for ordinary programs.

Chapter 24 covers export targets and application dependencies. VERSION identifies the compiler release. This book describes implemented behavior rather than planned syntax.

![Source, checking, LLVM optimization and native artifacts](../images/chali-compilation-en.png)

## 1.5 Reading and diagnosing

Complete examples execute from main, with supporting declarations before it. Interactive examples specify input. Rejection examples illustrate a diagnostic and are separate from successful programs. Use the [language reference](../../en/04-language-reference.md) for detailed syntax.

### Summary

Read complete examples as entry, calls and result: locate main, follow execution and compare the output. When something fails, identify checking, compilation/linking or execution before changing the relevant input. Changing code and tools simultaneously makes the cause harder to establish.

Chapter 2 introduces waiting for input. Results then depend on the supplied sequence as well as source, making recorded input part of a reproducible example.

## 1.6 Exercises

1. Run hello.chl and inspect its newline.
2. Which command executes println?
3. Remove the semicolon and inspect the first diagnostic.
4. Distinguish compiler and application dependencies.

### Answers and checks

1. The output is one line: Hello, ChaLi!.
2. Run executes the program; check does not.
3. Restore it and check again; line numbers depend on the source.
4. LLVM supplies compilation tools; application libraries follow their linkage requirements.

Next: [Chapter 2](02-guessing-game.md).
