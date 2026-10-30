# The ChaLi Programming Language

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../README.md)
<!-- END LANGUAGE NAVIGATION -->

This book develops programs from checking and local execution through native delivery. Small examples establish types and control flow, then introduce values, references, resources, generics and interfaces before combining them in a database application. It describes implemented 1.0.11 behavior. [VERSION](../../../VERSION) identifies the release; pending plans are not presented as available syntax.

Each chapter introduces a problem, then follows code and output. Code walkthroughs explain key statements, state changes, result origins and common mistakes. Chapter summaries relate examples to later topics. Predict results before consulting the explanation instead of relying only on copying and running examples.

## Reading and running examples

New programmers can start at Chapter 1. Experienced readers can use the topic map; read Chapters 6–8 together for the value, reference and resource rules. Chapters include exercises and answers. This English edition contains technical text and appendices, without the Chinese literary extras or a separate LaTeX/PDF build.

Complete programs include main and expected output. Interactive examples specify fixed input; terminal echo is not program output. Short fragments need their enclosing declaration or expression context. Rejected examples demonstrate errors and are separate from successful programs.

Commands use chali from PATH. Use the actual versioned filename for a Windows source build, or ./chali for an executable in the current Linux directory. Follow the [installation guide](../../en/39-installation-packages.md) and prepare application libraries and databases separately.

## Chapter map

| Chapter | Title | Topics |
| --- | --- | --- |
| [01](01-first-program.md) | Your first program | Tools, checking, running and export |
| [02](02-guessing-game.md) | A guessing game | An interactive loop with fixed input |
| [03](03-variables-types.md) | Variables and types | Declarations, numeric types and casts |
| [04](04-functions.md) | Functions | Parameters, returns and calls |
| [05](05-control-flow.md) | Control flow | Branches, loops and control expressions |
| [06](06-value-semantics.md) | Value semantics | Copies, borrows, moves and observable behavior |
| [07](07-references.md) | References | Reference validity and mutable access |
| [08](08-resources-scopes.md) | Resources and scopes | Scope exit, deferred cleanup and resources |
| [09](09-structures.md) | Structs and methods | Fields, methods and named construction |
| [10](10-enums-patterns.md) | Enums and pattern matching | States, payloads and exhaustive matching |
| [11](11-containers.md) | Collections | Arrays, List, Map, Set and capacity |
| [12](12-strings-text.md) | Strings and formatting | UTF-8, formatting and safe output |
| [13](13-interfaces.md) | Interfaces and concrete-type matching | Behavior, inheritance and concrete-type matching |
| [14](14-generics.md) | Generics | Type parameters, constants and constraints |
| [15](15-functions-closures.md) | Closures and higher-order functions | Captured environments and call boundaries |
| [16](16-errors.md) | Errors and propagation | Option, Result, interface payloads and propagation |
| [17](17-iterators.md) | Iterators | Consumption, composition and mutation |
| [18](18-modules-projects.md) | Projects and modules | Module ownership, directories and configuration |
| [19](19-macros.md) | Hygienic macros | Hygiene, parameter categories and result inference |
| [20](20-reflection.md) | Reflection and serialization | Field metadata and serialization boundaries |
| [21](21-c-interop.md) | C ABI and host interoperation | Prototypes, marshalling, resources and callbacks |
| [22](22-compiler-commands.md) | Directives and derived behavior | Derivation, operators and borrow provenance |
| [23](23-unsafe-code.md) | Unsafe code and raw storage | Addresses, initialization and raw storage |
| [24](24-aot-export.md) | AOT export: delivering native artifacts | LLVM, target configuration and deployment |
| [25](25-student-system.md) | A student information system | Authentication, permissions and school data |

## Appendices and reference

| Entry | Purpose |
| --- | --- |
| [Appendix A](A-common-errors.md) | Diagnose common failures by stage |
| [Appendix B](B-standard-library.md) | Find modules and APIs by task |
| [Language reference](../../en/04-language-reference.md) | Detailed declarations and expressions |
| [Library reference](../../library/README.md) | Signatures, results and limitations |
| [Capabilities](../../en/17-capabilities.md) | Current implementation scope |
| [Runnable examples](../../../example/samples/) | Standalone repository programs |

Identify whether a failure occurs in parsing, type or borrow checking, native linking, or application execution. Check the relevant preconditions and diagnostics instead of merely disabling checks.
