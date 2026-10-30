# The ChaLi Programming Language

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../README.md)
<!-- END LANGUAGE NAVIGATION -->

Start with a four-line program, run it, and change its message. Variables, branches, and functions enter the story when that small program needs them. Later chapters build on what you have already used rather than asking you to learn the whole language at once.

Short examples explain individual statements and show where each result comes from. Larger programs are explained in functional sections: their inputs, work, results, and error paths. Try changing one input before moving on. Predict the result first, then run the program.

The guessing game is an early guided exercise. Chapter 16 builds a log-search command, beginning with a few lines of text and adding files, options, and diagnostics. Chapter 28 develops a local HTTP file server one working step at a time. Chapter 29 follows a request through the existing student-management application, from login and permissions to database operations and responses.

Complete examples have a main function and expected output. Interactive examples also specify their input. Fragments explain their required context, and intentionally rejected programs are marked as such. Detailed syntax belongs in appendices and reference pages; the main text explains why a feature is useful and how it combines with earlier ideas.

## Contents

| Chapter | Topic |
| --- | --- |
| [01](01-getting-started.md) | Getting Started |
| [02](02-guessing-game.md) | A Guessing Game |
| [03](03-variables-types.md) | Variables and Types |
| [04](04-functions.md) | Functions |
| [05](05-control-flow.md) | Control Flow |
| [06](06-structs.md) | Structs |
| [07](07-value-semantics.md) | Value Semantics |
| [08](08-references.md) | References |
| [09](09-resources-scopes.md) | Resources and Scopes |
| [10](10-enums-patterns.md) | Enums and Pattern Matching |
| [11](11-modules-projects.md) | Modules and Projects |
| [12](12-strings-text.md) | Strings and Text |
| [13](13-collections.md) | Collections |
| [14](14-error-handling.md) | Error Handling |
| [15](15-testing-debugging.md) | Testing and Debugging |
| [16](16-log-search.md) | A Log Search Tool |
| [17](17-interfaces.md) | Interfaces and Runtime Polymorphism |
| [18](18-generics.md) | Generics |
| [19](19-closures.md) | Function Values and Closures |
| [20](20-iterators.md) | Iterators |
| [21](21-concurrency.md) | Concurrency and Threads |
| [22](22-macros.md) | Preprocessing and Macros |
| [23](23-annotations-reflection.md) | Annotations and Reflection |
| [24](24-compiler-directives.md) | Compiler Directives |
| [25](25-unsafe.md) | Unsafe Code |
| [26](26-c-interop.md) | Interoperating with C |
| [27](27-aot-export.md) | AOT Export |
| [28](28-http-file-server.md) | An HTTP File Server |
| [29](29-student-management.md) | Student Information Management |

See [Appendix A](A-common-errors.md) for common errors, [Appendix B](B-standard-library.md) for library tasks, and [Appendix C](C-syntax-operators.md) for syntax and operators. [Installation](../../en/39-installation-packages.md) explains how to run the compiler. The Chinese edition also includes the original literary extras and a [PDF](../latex/book.pdf); this English edition contains the technical text.

The organization takes inspiration from [the Rust Book's reading sequence](https://doc.rust-lang.org/book/ch00-00-introduction.html#how-to-use-this-book), alternating concepts and projects. ChaLi examples and rules are based on this repository's implementation; prior knowledge of Rust is not required.
