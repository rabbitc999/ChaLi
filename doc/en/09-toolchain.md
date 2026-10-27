# 09 Toolchain and commands

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-toolchain.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, `run` compiles and executes checked AOT code, while `aot` defaults to unchecked export. The compiler no longer embeds a VM execution engine. `.svmb` export has been removed; use native AOT artifacts. See [Native AOT debugging](43-native-debugging.md).

ChaLi executes programs through checked AOT by default. The front end preprocesses, parses and checks source, then lowers it to quadruples and native LLVM code. The explicit [AOT entry](38-aot.md) can generate LLVM modules and native products.

```sh
chali run app.chl
chali check app.chl
chali check app.chl json
chali emit app.chl out build/app
chali aot app.chl --out build/app
```

A direct source file also runs; following arguments are passed to the program. `emit` writes `.quad`, `.data.bin` and `.layout`; Bytecode export has been removed. The compiler embeds the standard library and its required assets.

## Development tools

```sh
chali new demo name demo
chali test tests backend interpret
chali fmt check src
chali deps lock project . offline
chali man List
chali lsp
chali gen compiler out build/compiler
chali clear out build/compiler
```

The generator creates standalone compiler projects. Hosts integrate through the shared runtime, host registries and the native AOT entry points.

Reflection is a fixed, non-trimmable part of the language: the front end and the runtime always build it in and compilation always runs the full reflection pass; `--no-reflection` only decides whether this artifact carries reflection metadata and reduces no workload. Quadruples are not optimized internally at all — every optimization is left to LLVM, and native release builds default to O3, LTO and dead-code elimination. `--memory-size` and `--step-limit` configure runtime resource budgets; isolated threads are on by default and `--no-threads` turns them off.

Back-end debugging switches change diagnostics or the execution path, never program semantics:

| Switch | Effect |
| --- | --- |
| `CHALI_DUMP_FRAME=1` | Print each function's frame layout (symbol → slot) plus the allocator self-check result |
| `CHALI_NO_SLOT_REUSE=1` | Fall back to "parameters first, then one slot per declaration in order" |
| `CHALI_ONE_SLOT_PER_SYMBOL=1` | One slot per symbol but still through the allocator — separates aliasing bugs from numbering bugs |

AOT fast-path comparison switches are listed in [AOT lowering switches and counters](40-backend-ownership.md); the internal quad optimizer and the register execution core were removed in 1.0.8, so their switches no longer exist.

Instruction-selection coverage and frame slots before/after reuse are both visible in `--stats`:

```text
指令选择：目标指令 3062 条，覆盖栈指令 10492/26865（39.1%，通用回退 16373 条）
帧槽分配：362 个函数，符号 1622 个 → 槽 1346 个（省掉 276 个）
```

Project execution uses run; interpret remains only as an AOT alias, and svmb has been removed. See [projects](13-projects.md), [deployment](06-deployment.md) and the [SVMB specification](../15-svmb-format.md).

## Source modules and incremental builds

C/C++ filenames under `source/` use descriptive module names with underscores. Existing class, member and
function names are retained. Standard bindings live in `source/glue/std_xxx.{hpp,cpp}`: headers expose registration
interfaces, while platform adapters, record layouts and algorithms compile in their own `.cpp` files.
`std_binding.hpp` provides a registry forward declaration. Templates for structured data and value conversion
remain in headers where their definitions are required. The VM core continues to use generic host protocols.

Generic expansion, closures, project manifests and diagnostics compile through `chali_generic.cpp`,
`chali_lambda.cpp`, `chali_generic_closure.cpp`, `chali_package.cpp` and `chali_report.cpp`.
`macro/` independently compiles hygienic binding, structural substitution, registration, regex matching,
original fragments and editor header models. `syntax/` provides ordinary source grouping and constant
expressions; `editor/` writes real compiler analysis. AST declarations live in `chali_ast.hpp`.
The authoritative grammar assembles registration, hygienic expansion, generic closures and lowering
from `semantics/`. Language services live in `lsp/`; the compiler generator lives in `generator/`.

After grammar changes, run `sh gen.sh`, then `make -j4`. Compiler-generated `.d` files track actual include
relationships for ordinary C++ changes. `make` automatically checks standard-library, glue and manual inputs;
The generators keep `.sha256` manifests in `build/embedded-sha256/`. Before compression or generation,
they compare SHA-256 digests of inputs, generator scripts and existing outputs. Matching digests skip
generation and preserve output timestamps; missing or damaged outputs, changed contents and changed
asset indexes require regeneration. This also covers the manual, bilingual messages, runtime asset lists
and AOT linker assets. `gen.sh` forces UTF-8 for Python subprocesses so Chinese output remains valid on Windows.
Builds, tests, AOT asset extraction, LSP checks, ABI bridges and compiler-project generation keep temporary
directories under the repository's `tmp/`, using independent subdirectories. They do not create a sibling `tmp`.
Repository compiler binaries locate this directory even when started from a child or parent directory;
Python and Node tests and build subprocesses inherit the same temporary path.
`tools/embed_stdlib.py` generates the assets, which compile once as `chali_stdlib_embedded.o`. Public headers
no longer include the compressed data implementation. A generation stamp tracks inputs, and unchanged output
is not rewritten. Exported compiler projects also link these implementations as separate CMake sources.

A host includes `chali_glue.hpp` and the required `std_xxx.hpp`, then links the matching `.cpp` files or host-record
library. Registration headers do not implicitly include all implementations. A complete command is provided in
the [synchronous callback example](../../example/native-callbacks/README.md).

## Independently compiled library modules

Function hover shows the overload matching the current call. Signature help labels switchable candidates
as `Function [1/4]` and narrows them using entered argument counts and inferred types, retaining viable
default, variadic, generic and incomplete calls. A selection is preserved while it remains valid.
Macros use `Macro [1/4]`, with ordered bracket groups, counts and variadic groups following compiler rules.
Fragment categories and type inference follow the macro body's `$match` and generic rules; they do not
create type overloads for identical macro headers. A `$` prefix completes only macros, including imported
modules and `$module::name`. VS Code discards cancelled requests and responses for changed document versions.

`source/ast` compiles types, node construction, quadruple generation and data export separately. `source/bignum` separates integer multiplication, division, number theory, bit operations, conversion and decimal roots. Public headers retain the interfaces; Make dependency files track actual includes. Embedded assets and exported compiler projects carry all implementation units. Each library also provides a CMake target for independent builds. See the [AST library](../../source/ast/README.md) and [big-number library](../../source/bignum/README.md) for file responsibilities.
