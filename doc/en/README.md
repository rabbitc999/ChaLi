# ChaLi Documentation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../README.md)
<!-- END LANGUAGE NAVIGATION -->

See [current capabilities and limits](17-capabilities.md) for implemented behavior and build defaults. Tutorials develop language usage, while reference pages provide interface details. Verification reports identify programs, configurations and platforms actually exercised.

ChaLi uses LLVM AOT to generate native code. Prepare target runtimes, C ABI libraries, link assets and runtime resources for the actual platform; consult the module and AOT references for their conditions.

Read the current language/toolchain version from [`VERSION`](../../VERSION).

For AI-assisted ChaLi programming, use the repository's [ChaLi programming skill](../../skills/chali-programming/SKILL.md), covering syntax choices, API lookup, macros and execution checks.
Changes: [`CHANGELOG.md`](../../CHANGELOG.md).

> This is the English edition. The Chinese edition is the default and is the one maintained first —
> see [中文文档](README.md). Code snippets are identical in both editions and are executed before
> they are published.

---

## What do you want to do?

| Who you are | Start here |
| --- | --- |
| Learning step by step | [The ChaLi Programming Language](../book/en/README.md): 29 chapters with log search, HTTP file service and a student system |
| New to ChaLi | [Vision](00-vision.md) → [5-minute quickstart](01-quickstart.md) |
| Evaluating it for a project | [Vision](00-vision.md) (includes "who should not use it") → [Cross-platform deployment](06-deployment.md) |
| Ready to write code | [Language tour](02-language-tour.md) → [Language reference](04-language-reference.md) |
| Want to understand value semantics and references | [Core concepts](03-core-concepts.md) |
| Deploying the program to a target host | [Delivery security](05-delivery-security.md) → [Cross-platform deployment](06-deployment.md) |
| Want to know what the standard library has | [Standard library overview](07-stdlib-overview.md) |
| Looking for something to run right away | [Example projects](13-projects.md) (student records, graphics, host VM…) |
| Need to talk to a C/C++ host | [Host integration and C ABI](08-host-integration.md) |
| Looking up commands, config, tests | [Toolchain](09-toolchain.md) |
| Querying APIs in a terminal or checking editor caches | [Command line manual](../man/en/README.md) → [LSP incremental hints and caching](../man/en/lsp.md) |
| Looking for the authoritative grammar | [EBNF grammar reference](../14-grammar-ebnf.md) |
| Comparing 1.0.10 and 1.0.11, allocators, and frozen C++/Rust/Lua/Java JIT references across 17 workloads | [Version and language performance baselines](12-performance.md) |
| Installing MSI/DEB/RPM packages or building another CPU architecture | [Installation packages and architecture builds](39-installation-packages.md) |
| Want to contribute / see the roadmap | [Roadmap and ecosystem](10-roadmap.md) |

---

## Document structure

| No. | Document | Audience | What it covers |
| --- | --- | --- | --- |
| [00](00-vision.md) | Vision | Decision makers, prospective users | The three dilemmas of software delivery, ChaLi's answers, who it fits / who it does not |
| [01](01-quickstart.md) | 5-minute quickstart | First-time users | Install, first program, command cheat-sheet |
| [02](02-language-tour.md) | Language tour | Experienced developers | Walk through what ChaLi looks like with runnable snippets |
| [03](03-core-concepts.md) | Core concepts | Users going deeper | Value semantics, non-null references, resource management without a GC |
| [04](04-language-reference.md) | Language reference | Day-to-day writers | Syntax, semantics, examples and common errors by topic, including generic nesting, default types and monomorphisation limits |
| [05](05-delivery-security.md) | Delivery security | Teams shipping outward | Delivery forms, the four steps, reverse-engineering cost, boundary statement |
| [06](06-deployment.md) | Cross-platform deployment | People doing deployment | Source execution and bytecode deployment, desktop / embedded Linux, resources and limits |
| [07](07-stdlib-overview.md) | Standard library overview | Anyone asking "what can it do" | Existing modules and core APIs by capability |
| [08](08-host-integration.md) | Host integration and C ABI | Integrators | Glue bindings, `abi` blocks, type mapping, nullable references |
| [09](09-toolchain.md) | Toolchain | Daily `chali` users | Commands, `chali.toml`, compile options, tests, LSP, display language |
| [10](10-roadmap.md) | Roadmap and ecosystem | Contributors, long-term users | Short/mid/long-term goals and verifiable milestones |
| [13](13-projects.md) | Example projects | Anyone who wants to run something | Application and capability examples in the repository: location, commands, verification and prerequisites |
| [14](../14-grammar-ebnf.md) | EBNF grammar reference | Anyone checking grammar details | Every production and token declaration generated from `source/chali.gram`; the readable form of the authoritative grammar |
| [15](../15-artifacts.md) | Build artifacts and intermediates | Anyone wondering where files land | `.chali/` layout, cleanup rules, what to ship |
| [16](../16-compact-symbols.md) | Nameless compact symbols | Anyone tracking size and diagnostic privacy | `--compress-symbols` format, tooling and space comparison |
| [17](17-capabilities.md) | Current capabilities | Evaluation and integration | Language, VM, borrowing, reflection, libraries, embedded Linux and tooling contracts |
| [18](../18-panic-trace.md) | Panic and source lines | Anyone debugging crashes | Failure output, call stack, source location, compact symbol-hiding policy |
| [19](../19-reflection.md) | Annotations and reflection | Anyone using annotations / dynamic calls | Declaration metadata, typed queries, closed-value dynamic invocation |
| [20](../20-memory-allocator.md) | Memory allocator | Anyone wiring host memory | Pool and capacity stats, allocation range checks, external buffers |
| [22](22-resource-lifetime.md) | Resource lifetimes and borrow control flow | Runtime contributors | Allocation identities, resource cleanup and coalescing, temporary ownership, closure borrowing |

## More entry points

## Companion resources

- [`example/samples/`](../../example/samples/) — runnable formal samples; expected stdout in [`example/expected/`](../../example/expected/).
- [`example/samples-error/`](../../example/samples-error/) — samples expected to fail; diagnostics in [`example/expected-error/`](../../example/expected-error/).
- [`doc/examples/`](../examples/) — examples that need a companion project layout (ABI, SQLite, GUI).
- The companion projects under [`doc/examples/`](../examples/) are catalogued in [13 Example projects](13-projects.md).
- [Project README](../../README.en.md) — build, quick usage, debugging, configuration (English;
  [中文版](../../README.md)).
- [Full grammar reference (EBNF)](../14-grammar-ebnf.md) — the authoritative form of the grammar, generated from [`source/chali.gram`](../../source/chali.gram).

- [11 Borrow control flow and algorithm validation](11-algorithm-validation.md): independent dataflow checks, heap structures, DFS/BFS and NOIP2015 brute-force comparisons.

- [Any, explicit generics and data utilities](23-any-generics-data.md)

- [Ownership transfer](24-ownership-move.md)

- [Context-typed literals, operator inference and macros](25-literals-operators-macros.md)

- [Execution and deployment](26-execution-delivery.md)

- [Custom error propagation and if expressions](27-error-propagation-if.md)

- [Matching, interpolation, generics and scope cleanup](28-match-interpolation-scope.md)

- [Return expressions, named closures and field derivation](29-declaration-sugars.md)
- Value breaks and loop traversal: [language reference](04-language-reference.md#block-expressions-and-interpolated-output), [complete sample](../../example/samples/break_values.chl), [VM regressions](../../tests/chali_block_expression.py).

- Implicit moves in value breaks: [semantics and cleanup order](04-language-reference.md#moving-and-cleaning-up-value-breaks), [resource identity and destructor sample](../../example/samples/break_move.chl).

- [Threads, channels and explicit events](30-thread-concurrency.md)

- [Exact symbolic computation and high precision mathematics](31-symbolic-computation.md)
- [Standard library ownership and string methods](32-opcode-values-strings.md)
- [Reflection serialization and dynamic SQL](33-reflection-serialization-sql.md)

The [29-chapter English book](../book/en/README.md) contains technical chapters, exercises and appendices. The Chinese edition also includes literary extras and a PDF. Chapter 29 follows the actual school application with architecture and browser screenshots.

- [Compiler safety boundaries and focused regression tests](34-compiler-safety.md)

- [Opcode and standard-library host records](../35-opcode-storage.md)

- [Name and related capabilities](37-name-freeze.md)

- [AOT compilation](38-aot.md)
- [AOT lowering switches and counters](40-backend-ownership.md)
- [Raw storage and C ABI directives](41-unsafe-abi.md)

- [Downloaded build dependencies](42-build-dependencies.md): pinned SQLite/miniz downloads, checksums, caches, offline reuse and licensing.

- [Native AOT debugging](43-native-debugging.md): GDB breakpoints, stepping and variables.
