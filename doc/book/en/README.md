# Learning ChaLi

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../README.md)
<!-- END LANGUAGE NAVIGATION -->

This English Markdown edition follows the chapter order of the [Chinese book](../README.md). It explains the core language, practical examples and deployment workflow in English. The Chinese edition contains additional extended discussions and chapter extras. This edition does not generate LaTeX or PDF.

The compiler embeds both editions. Export them without downloading documentation:

```sh
chali man book
chali man book 1 --lang en
chali man book 24 --lang en
chali man book A --lang en
```

The complete export uses `book/` for Chinese Markdown, `book/en/` for English Markdown and `book/images/` for shared images. Reading a chapter does not require Python or a source checkout. Source examples retain ChaLi syntax; changing the display language does not translate keywords or identifiers.

| Chapter | Topic |
| --- | --- |
| [1](01-first-program.md) | Your first program |
| [2](02-guessing-game.md) | A guessing game |
| [3](03-variables-types.md) | Variables and types |
| [4](04-functions.md) | Functions |
| [5](05-control-flow.md) | Control flow |
| [6](06-value-semantics.md) | Value semantics |
| [7](07-references.md) | References |
| [8](08-resources-scopes.md) | Resources and scopes |
| [9](09-structures.md) | Structures |
| [10](10-enums-patterns.md) | Enums and pattern matching |
| [11](11-containers.md) | Containers |
| [12](12-strings-text.md) | Strings and text |
| [13](13-interfaces.md) | Interfaces and runtime polymorphism |
| [14](14-generics.md) | Generics |
| [15](15-functions-closures.md) | Function values and closures |
| [16](16-errors.md) | Error handling |
| [17](17-iterators.md) | Iterators |
| [18](18-modules-projects.md) | Modules and projects |
| [19](19-macros.md) | Preprocessing and hygienic macros |
| [20](20-reflection.md) | Annotations and reflection |
| [21](21-c-interop.md) | C interoperability |
| [22](22-native-deployment.md) | Native deployment |
| [23](23-compiler-commands.md) | Compiler commands |
| [24](24-aot-export.md) | LLVM AOT export |
| [25](25-unsafe-code.md) | Unsafe code, raw storage, bare pointer arithmetic and byte operations |
| [26](26-student-system.md) | A student information system |
| [A](A-common-errors.md) | Common errors |
| [B](B-standard-library.md) | Standard-library reference |

For precise API signatures, use `chali man api` and the [API index](../../library/README.md). For measured VM and AOT performance, see the [performance report](../../en/12-vm-performance.md).
