# 17 Current capabilities and implementation limits

[Documentation index](README.md) · [中文](../17-capabilities.md)

This page describes the current checkout. Read the version from [`VERSION`](../../VERSION) and
release history from [`CHANGELOG.md`](../../CHANGELOG.md). Grammar, standard-library sources, build
configuration and regressions define the implementation. Historical measurements retain the actual
tested version, configuration and date; they do not automatically describe current builds or other targets.

## Language and resources

| Area | Implemented capability and contract |
| --- | --- |
| Types | Fixed-width integers, float32/float64, bool, char, String, big numbers; arrays, tuples, structs, payload enums, containers and function types |
| Abstraction | Overloads, generic monomorphization, interface constraints, associated types, constant generics, default type parameters, default interface methods and runtime interface dispatch |
| Function values | Anonymous functions use `fn`; inline arguments, nested higher-order functions and const/mut/copy/move captures are supported |
| Control flow | Match, guards, destructuring, ranges, if expressions, block expressions with `break value;`, tuple exits with `break a, b;`, implicit moves of direct variables and value breaks across non-value loops, macros and conditional compilation |
| Errors and cleanup | Option, Result, custom error propagation, `?`, panic, defer, scope(exit), Drop and Closeable |
| Ownership | Ordinary owned-value assignment and value arguments copy; move transfers resources. The old binding cannot be read, written or rebound; a complete move permits a new declaration using that name |
| References | Non-null read-only/mutable references; `Option<T const&>` and `Option<T mut&>` represent an optional reference, with None constructing no reference |
| Checking | Independent provenance and control-flow checking tracks locals, fields, aliases, moves and releases; alias sharing is allowed, without a global exclusive-borrow rule |

Owned values have deterministic cleanup without GC. References, host resources and aggregates containing
references retain their individual contracts. Manually allocated storage must be released as specified.
Static checking does not cover arbitrary raw memory, external host behavior or every concurrent invalidation.
String indexing and slicing use UTF-8 bytes; use `std::utf8` for code-point operations.
See the [language reference](04-language-reference.md), [core concepts](03-core-concepts.md) and
[move rules](23-ownership-move.md).

## Execution, optimization and delivery

| Mode | Implementation | Deployment requirements |
| --- | --- | --- |
| Source execution | Preprocess, parse, check types/borrows, lower to quadruples, then execute in the VM | Magel toolchain and required host capabilities |
| Bytecode | Load `.svmb` in the shared `library/stackvm/` core | Compatible feature bits, host glue, data and platform resources |
| AOT | Export quadruples as C++, then compile with the selected toolchain | Runtime services are compiled into the artifact; external drivers, system and ABI libraries remain dependencies when used |

IR means quadruples. The complete IR optimizer and highest available VM execution optimizations are
enabled by default, including the register core, preparation/fusion, jump folding and integer paths.
Source and bytecode execution share the VM; there is no JIT mode.
Library output supports `.dll` / `.so` shared libraries and relocatable `.o` files with C headers.
Libraries require at least one `export` and may omit `main`. Static `.lib` / `.a` output is unsupported.
See [library output](09-toolchain.md#aot-library-output). AOT supports checked/unchecked,
module caches and explicit GCC commands, paths or directories. Symbol compression does not provide
encryption, a complete sandbox or a guarantee against reverse engineering.

Shared capabilities follow the same semantics. C `export` is AOT-only and is rejected by VM execution
and bytecode export. Replacing bytecode updates a program image, without promising live migration of
stacks, closures or global state.

## Host integration and reflection

Magel calls C libraries through `abi` and C++ through a Registry. References, structs, arrays, optional
references, returned-memory release and limited header bindings follow explicit ABI contracts; declarations
must match the actual C signatures. Glue calls named functions, inline fn values and closures through
`NativeCallContext::invoke` or `magel_glue::Function`. VM/AOT share the interface, argument-count checks
and error propagation. Wrappers and contexts are valid only synchronously, on the same thread, during
the enclosing host callback. Resource values retain their Codec ownership requirements.
See [host integration](08-host-integration.md) and the [callback example](../../example/native-callbacks/README.md).

The root toolchain defaults to `MAGEL_WITH_REFLECTION=1` and `STACKVM_WITH_REFLECTION=1`, with reflection
enabled for source compilation; use `--no-reflection` to disable it. Generated compiler projects and MCU
hosts have their own documented build defaults. Reflection supports annotations, explicitly exposed
metadata and restricted dynamic calls; it does not create arbitrary runtime types or offer general field
writes and arbitrary resource-value invocation. It carries no hard-real-time guarantee.
See the [reflection specification](../19-reflection.md).

## Libraries, MCU and tools

Libraries cover containers/algorithms, iterators, math, numerical methods, text/encoding, JSON/TOML/CSV,
files/directories, TCP/basic HTTP, SQL drivers, coroutines, isolated threads, messages, explicit shared
values, events and watchdogs. The [API index](../library/README.md) defines the module list; overview
pages do not maintain a separate module count. HTTP excludes TLS; external databases need their
clients and services. Asynchronous event callbacks have separate lifetime contracts.

Desktop and MCU builds share one VM core. `source/mcu_vm/` supplies a host without exceptions/RTTI,
pools and platform adapters. The front-end `[embedded]` module gate and standalone MCU build macros/glue
are separate configurations. The H743 port provides SDRAM, FatFS, USART1, an LCD selector and keys,
and starts one benchmark by default. Its 2/16/4 MiB pools total 22 MiB, which is not a small-SRAM footprint.
The host can return to its menu after normal exit or language panic; hardware Faults, host corruption
and unrecoverable exhaustion remain platform concerns. See [deployment](06-deployment.md), the
[MCU host](../../source/mcu_vm/README.md) and [H743 project](../../example/mcu/FK-STM32H743XIH6/README.md).

The CLI provides projects, checks, formatting, tests and dependency locks/caches. LSP provides diagnostics,
completion, hover, signatures and cross-file navigation. DAP provides breakpoints, stepping and stacks;
variable inspection and expression evaluation are not implemented. Dependencies support local directories,
fixed Git commits and hashed HTTP/HTTPS archives. Feature implementation, regression evidence and
long-term production validation are separate claims.
