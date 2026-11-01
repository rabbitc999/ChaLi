# 09 Toolchain and commands

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-toolchain.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, `run` compiles and executes checked AOT code, while `aot` defaults to unchecked export. The compiler no longer embeds a VM execution engine. `.svmb` export has been removed; use native AOT artifacts. See [Native AOT debugging](43-native-debugging.md).

ChaLi executes programs through checked AOT by default. The front end preprocesses, parses and checks source, then generates LLVM code directly from the checked AST. The explicit [AOT entry](38-aot.md) can generate LLVM modules and native products.

```sh
chali run app.chl
chali check app.chl
chali check app.chl json
chali aot app.chl --emit-llvm out build/app
chali aot app.chl --out build/app
```

A direct source file also runs; following arguments are passed to the program. `aot --emit-llvm --out PREFIX` generates `.ll` directly from the AST; Bytecode export has been removed. The compiler embeds the standard library and its required assets.

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

Project execution uses run; interpret remains only as an AOT alias, and svmb has been removed. See [projects](13-projects.md), [deployment](06-deployment.md) and the [migration note](../16-compact-symbols.md) and [build artifacts](../15-artifacts.md).

## Source modules and incremental builds

C/C++ filenames under `source/` use descriptive module names with underscores. Existing class, member and
function names are retained. Standard-library policy remains in ChaLi sources under `source/std/`,
while host entry tables live under `source/abi/`. The C11 header `chali_abi.h` defines frames, fixed-width
slots, opaque registry handles and caller-owned error buffers; `chali_standard.h` exposes standard registration.
Implementations may use C or C++17, and their public boundaries use C types.

`glue/aot_modules.inc` holds module names and C table providers. Standard registration excludes demo;
AOT selects modules by actual reachable entry names. Each table registers atomically, and the first failure
stops registration with its diagnostic. The runtime shares one function table and seals it after setup.

`chali_host_call` forwards slot arrays to C entries. LLVM can call scalar C ABI functions directly.
Generated C entries adapt aggregates, arrays, references, optional references and synchronous callbacks;
private marshalling accesses language storage and function values only through C frame services.
`chali_abi_bridge.cpp` reuses the heap, object store, error channel and synchronous invocation services.
`GLUE_NO_STD_XXX` continues to control module trimming.

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

A C host includes `chali_abi.h` and optionally `chali_standard.h`, then links matching runtime and host-record
libraries. Entry code must outlive its registration and calls. Invoke language function values through
`chali_abi_invoke`; a language handle is not a machine function pointer. Complete commands are in the
[C host](../../example/host-c-api/README.md) and [synchronous callback](../../example/native-callbacks/README.md) examples.

## Independently compiled library modules

Function hover shows the overload matching the current call. Signature help labels switchable candidates
as `Function [1/4]` and narrows them using entered argument counts and inferred types, retaining viable
default, variadic, generic and incomplete calls. A selection is preserved while it remains valid.
Macros use `Macro [1/4]`, with ordered bracket groups, counts and variadic groups following compiler rules.
Fragment categories and type inference follow the macro body's `$match` and generic rules; they do not
create type overloads for identical macro headers. A `$` prefix completes only macros, including imported
modules and `$module::name`. VS Code discards cancelled requests and responses for changed document versions.

`source/ast` compiles types, node construction, quadruple generation and data export separately. `source/bignum` separates integer multiplication, division, number theory, bit operations, conversion and decimal roots. Public headers retain the interfaces; Make dependency files track actual includes. Embedded assets and exported compiler projects carry all implementation units. Each library also provides a CMake target for independent builds. See the [AST library](../../source/ast/README.md) and [big-number library](../../source/bignum/README.md) for file responsibilities.

## Embedded standard-library binary

The compiler carries a binary parse unit for the `std::lang` core. Normal compilation restores its nodes, type pool, identifier pool and top-level declarations from memory. It requires neither an external `.unit` file nor a `.chali/chali-stdlib-cache` directory. Optional modules carry separate compressed units for ordinary declarations and hygienic macro ASTs. They load in import order, relocating node, type, interface-name, macro-invocation and source identities into the current session. User dependencies retain their order. Generic templates still pass through the text frontend; concrete instantiation, macro expansion, semantic checks and borrow checks remain active.

`make all` builds a separate standard-library stage compiler, generates each binary twice with `tools/build_stdlib_units.py` to check determinism, atomically updates the assets, and builds the final compiler. The package contains no user source, project paths or process pointers. Parse-unit format 7 preserves the complete type-reference graph and lifetime contracts. Package format 2 validates content/compiler identity, reflection configuration, lengths and integrity. Core source locations are canonicalized so unrelated user input lengths do not change the binary. A missing content profile uses in-memory source parsing; a damaged matching asset produces an error.

`--no-stdlib-cache` and `CHALI_NO_STDLIB_CACHE=1` remain compatibility controls for source parsing. Neither path writes a standard-library cache. `--stdlib-cache-dir` has been removed, and `CHALI_STDLIB_CACHE_DIR` no longer chooses a loading directory. Existing project files are preserved. `--stats` distinguishes an embedded-core hit, source parsing and a disabled unit, and reports separate binary-hit and source-fallback counts for optional modules.
