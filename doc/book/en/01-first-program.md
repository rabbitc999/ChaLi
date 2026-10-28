# Chapter 1 Your first program

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../01-第一个程序.md)
<!-- END LANGUAGE NAVIGATION -->

After semantic checks, ChaLi generates LLVM IR directly from the checked AST. LLVM performs optimization and native code generation; normal `run` and `aot` skip ChaLi quadruple generation, serialization and assembly. `run` uses checked AOT and executes the program in the same process. Diagnostics point directly at the program you are editing.

Install the MSI on Windows, DEB on Ubuntu/Debian or RPM on CentOS, then use the `chali` command on PATH. A Windows source build instead produces `chali-1.0.8-llvm-external-windows-x64.exe` (or `llvm-static`); invoke that actual path when using an uninstalled build. See [installation and architecture options](../../en/39-installation-packages.md).

Save this as `hello.chl`:

```chali
void main()
{
    println("Hello, ChaLi!");
}
```

Run `chali hello.chl`. `main` is the program entry point. Braces delimit its body, and a semicolon terminates the call. `println` writes a line. The format string stays part of the source; the compiler's display language only changes tool messages.

![Compilation pipeline](../images/chali-compilation.png)

`run` and `aot` both produce native programs: `run` compiles with checked AOT and executes immediately, while `aot` exports an executable or shared library unchecked by default. Deployment and AOT export, including cross-compilation, are covered in chapter 24.

Use `chali --check hello.chl` to check a program without running it. A successful syntax check does not prove that every possible runtime input is valid: indexing, division and resource operations still have runtime checks.

Exercise: print two lines and then introduce a missing semicolon. Read the diagnostic location before correcting it. Continue with [a guessing game](02-guessing-game.md).
