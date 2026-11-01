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

The compiler carries a binary parse unit for the `std::lang` core. Normal compilation restores its nodes, type pool, identifier pool and top-level declarations from memory. It requires neither an external `.unit` file nor a `.chali/chali-stdlib-cache` directory. The assets include ordinary declarations, hygienic macro ASTs, uninstantiated generic-template records, and generated range/algorithm declarations. Separate compressed units load in import order, relocating node, type, interface-name, macro-invocation and source identities into the current session. User dependencies retain their order. Matching generic declarations restore parameters, constraints, defaults and source information, with internal macro references rebound to the current session. Concrete instances, interface implementations, expansion and semantic/borrow checks remain local to the current program. Preprocessing and name normalization may still scan embedded text; unmatched content uses the source path without writing a disk cache.

`make all` builds a separate standard-library stage compiler, generates each binary twice with `tools/build_stdlib_units.py` to check determinism, atomically updates the assets, and builds the final compiler. The package contains no user source, project paths or process pointers. Parse-unit format 7 preserves the complete type-reference graph and lifetime contracts. Template-record format 1 stores no user instances or process addresses. Package format 2 validates content/compiler identity, reflection configuration, lengths and integrity. Core source locations are canonicalized so unrelated user input lengths do not change the binary. A missing content profile uses in-memory source parsing; a damaged matching asset produces an error.

`--no-stdlib-cache` and `CHALI_NO_STDLIB_CACHE=1` remain compatibility controls for source parsing. Neither path writes a standard-library cache. `--stdlib-cache-dir` has been removed, and `CHALI_STDLIB_CACHE_DIR` no longer chooses a loading directory. Existing project files are preserved. `--stats` reports core, module-AST and template-record hits and source fallbacks, including matched module names. `std::__range` identifies a generated standard-library asset; it is not a user-importable module.

## Warning configuration

Warnings have stable codes. `CHL1601` (`overlapping-pattern`) is enabled by default. Naming rules `CHL2101`/`CHL2102` (`type-naming`/`function-naming`) and `CHL2201` (`unused-local`) are opt-in. Constructors and generated macro declarations are exempt. Unused-local analysis uses the checked AST and conservatively treats cleanup or borrowing reads as uses.

```console
chali --check main.chl --warn=unused-local
chali --check main.chl --warn=naming --warnings-as-errors
chali --diagnostics=json main.chl --deny=CHL2201
```

`--warn=RULE`, `--allow=RULE`, and `--deny=RULE` select warning, disabled, and error levels. Use a code, rule name, or the `naming`/`all` group. `--warnings-as-errors` promotes enabled warnings only. Unknown rules are rejected. Project configuration:

```toml
[warnings]
unused-local = "warn"
CHL2101 = "allow"
```

Levels are `allow`, `warn`, and `error`. The editor preset enables naming; project settings override the preset, and explicit CLI rules override project settings in command-line order. A real line comment `// chali: allow-next-line CHL2201` suppresses the next line; `// chali: allow CHL2201` suppresses its own line. Strings and block comments do not act as directives.

CLI JSON and LSP share codes, severity, original locations, and safe fix data. The current fix inserts a local suppression and preserves initialization and cleanup. Errors prevent execution. Ordinary macro parameter copies and compiler copy elimination produce no warning.

## Language-service analysis reuse

Requests retain immutable open-document versions and freeze closed modules read during that request. These module texts are passed to the compiler as source overlays. Lexical indexes serve completion and highlighting directly; diagnostics, type hints, and macro previews share full semantic reports. Concurrent semantic requests for one entry are merged while waiters remain cancellable. Compiler frontend globals are not shared between concurrent analyses.

The memory report cache holds at most 64 entries and an estimated 32 MiB of serialized JSON; records larger than 8 MiB are not retained. This estimate is not the JSON objects’ actual heap size or a process-wide memory cap. Every hit validates input, language, module graph, actual dependencies, and project/lock-file fingerprints. Valid memory reports remain usable without disk reports. File watching also invalidates entries when a previously missing manifest or lock file is created.

An edit crossing a full semantic check cancels the old request and reschedules diagnostics. Captured versions are checked again before publication. JSON excerpts for unsaved documents come from the checked source. `chali/lspStats` exposes `checks`, `diagnosticHits`, `memoryReportHits`, `mergedChecks`, `reportEntries`, and `reportBytes` to distinguish checks, reuse, and retained memory.

## Language-service scheduling and timing

Six independent worker lanes remain available. At most two compiler subprocesses perform full semantic checks at once. Lexical completion and hover do not wait for this budget; waiting interactive semantic requests take priority over background diagnostics. Each lane retains at most 128 pending tasks and a conservative 16 MiB snapshot estimate. An empty lane may retain one larger snapshot; newer work evicts older queued tasks. This is not a whole-process memory cap: open documents, active analyses, and bounded index/report caches have separate lifetimes.

Editing or closing a document cancels its old requests and releases queued snapshots. Every request still receives a result or cancellation response. Shutdown first cancels background diagnostics, gives request lanes a shared five-second drain deadline, then cancels remaining work and joins workers. Mutable frontend state remains isolated in compiler subprocesses. `analysisActive`, `analysisPeak`, and `analysisWaiting` in `chali/lspStats` expose the semantic budget.

LSP timing checks CPU usage before and after measurement. At 40% or above, or without a reliable counter, only functional checks run and no performance numbers are reported. Set `CHALI_SKIP_LSP_TIMING=1` to request functional checks explicitly. Comparisons record actual compiler SHA256 values, process startup/first diagnostics with empty LSP caches, and P50/P95 for edit diagnostics, completion, and hover. Latency includes JSON-RPC client round trips; process-cold measurements do not flush OS file caches. Other languages' runtime baselines are not remeasured.

## VSCode fixes, standard sources, and project targets

Warning lightbulbs use the compiler's current report. The current safe fix inserts a line-local suppression while retaining initialization, cleanup, and other statements. Document versions are checked before application; further edits invalidate old actions.

Go to Definition opens a `chali-stdlib:` read-only document for standard declarations that have embedded language source. Locations come from actual declaration indexes. Source and indexes are loaded on demand into memory, without source copies or standard-library disk indexes. Standard declarations cannot be renamed. Backend intrinsics without a language declaration do not receive invented bodies. Open standard views refresh after server restart.

Select Project Target, Select AOT Output, and Open Project Configuration use the compiler's TOML parser and source-preserving serializer. Other fields, comments, and multiline strings remain intact. Changes stay in an undoable buffer, and the status bar marks unsaved configuration. Builds save the source and an open dirty manifest before querying the actual target:

```toml
[aot]
target = "linux-arm64"
output = "object"
allocator = "malloc"
```

Build Current File respects executable, `.o`, or `.ll` output. Linux executables have no `.exe` suffix. Build Executable requires `output = "executable"` and does not rewrite the manifest. The status bar shows the target, output kind, and actual compiler LLVM variant. Choices include only registered Windows x64, Linux x64, and Linux ARM64 targets. Object generation does not validate target linking or execution; cross executables still require the corresponding SDK, runtime, and link dependencies.

P7 rename edits include source fingerprints. Before returning edits, the extension checks open versions and the current text of closed dependencies. Standard sources are read-only; ambiguous bindings or macro-template references reject the complete rename.

Code actions can fill directly listed non-generic interface methods and add a fallback to an incomplete match. Generated bodies contain explicit `TODO panic` placeholders that need implementation. Each candidate must pass an isolated compiler check without entering the normal report or preprocessing cache. Other errors, generics, or ambiguous declarations can prevent an action. The macro preview's “Expansion origins and bindings” summary lists compiler-recorded source, definition, metadata and control-target locations with navigation; it does not infer transitive purity.

## Verifying both distribution variants

Static and external LLVM builds use separate build directories and versioned artifact names. Before packaging, check `--build-info`, extension versions, exact VSIX compiler bytes and the actual variant's build configuration. Do not substitute the other variant's configuration. `tools/release_variants.py --package-only` accepts `--static-build-root` and `--external-build-root` for these directories. Check installer and ZIP executables, versions, LICENSE/NOTICE and dependency licenses separately. Record cross-object generation separately from execution on the target system.

The [final P3-to-P5 LSP comparison](../../benchmarks/results/lsp-1.0.12-final/README.md) is frozen by actual compiler hashes; it does not describe later P6-to-P8 rebuild bytes. Subsequent artifact checks do not relabel or repeat that measurement.

## Target machine assembly

`chali aot app.chl --emit-asm --out build/app` exports `build/app.s` using the current target, CPU, check mode and optimization/debug settings. This is machine assembly, distinct from LLVM IR text `.ll`. It stops before object generation and linking, so cross-target assembly does not require target runtime link assets. The static variant uses its built-in LLVM backend; the external variant uses clang. Output uses the selected backend's default syntax (x86 AT&T, AArch64 target syntax); no NASM/MASM compatibility is implied.

The project setting `[aot] output = "assembly"` selects the same output for AOT builds. An explicit `--emit-asm`, `--emit-obj`, or `--emit-llvm` overrides the project output; these three options are mutually exclusive. Ordinary `run` retains checked AOT execution. VSCode's output selector and build command recognize assembly and its `.s` suffix.

The `.ll` and any C ABI header remain available alongside `.s`. Complex C ABI signatures can also produce a `.abi.cpp` marshalling sidecar; assembly alone is not a complete linkable application. Reassembly and final linking require a matching target toolchain, runtime and dependencies. See the standalone assembler, linker and archiver below.

## Standalone assembler and assembly inputs

```sh
chali asm native/add.s --target windows-x64 --out build/add.obj -I native/include
chali asm native/add.s --target linux-x64 --out build/add.o
chali aot main.chl --asm native/add.s --asm-include native/include --out build/app
```

`asm` accepts one `.s` file and an exact output path. Without `--out`, it replaces the input suffix with `.obj` for Windows and `.o` for Linux. `--target`, `--cpu`, `--debug`, `-I DIR` / `-IDIR`, `--include-path DIR` and `--llvm CLANG` are supported. Target and CPU must match the source instructions. GAS-compatible LLVM syntax is used; `.S` preprocessing and NASM/MASM are not supported. Include search adds the source directory before the explicitly listed directories. Input files are limited to 64 MiB; this is a compiler tool, not an untrusted-code sandbox.

The static variant uses LLVM MC in process without clang; explicit `--llvm` selects the external integrated assembler. Objects are first produced in a private workspace and published only after successful assembly. A failed assembly preserves an existing output and cannot overwrite its own primary input. The output parent directory must exist. Source/include diagnostics retain their original positions.

AOT accepts repeated `--asm FILE.s` and `--asm-include DIR` options. Every object uses the same target and CPU as the language unit and participates in the final link. Use the established C ABI declaration to reference an assembly symbol; the assembly must follow that target's calling convention. These inputs require executable/shared-library output and cannot be combined with `--emit-asm`, `--emit-obj` or `--emit-llvm`. Assembling a cross-target object does not verify executable deployment on that target.

For a symbol supplied by linked objects, declare `#abi("C", "@link", "asm_add") int64 asm_add(int64 a, int64 b);`. `@link` is a compiler marker, never a filename or dynamic-library lookup. Its current signatures support fixed scalar parameters/results (integer, boolean, floating point, and void result); aggregate, pointer, string, callback and release-function forms are rejected. It remains a direct call when other declarations need typed C marshalling. Ordinary host declarations and existing library paths retain their behavior; an empty library string does not select direct linkage.


## Standalone linking and static archives

```sh
chali ar build/first.o build/second.o --out build/libexample.a
chali link build/main.o build/libexample.a --out build/app.exe
chali link build/main.o -L build --library example --out build/app.exe
chali link build/public.o --shared --export public_entry --out build/example.dll
chali link build/start.o --target linux-arm64 --no-runtime --entry _start --out build/app
```

`ar` writes deterministic, indexed, self-contained archives from zero or more objects of the selected target. Timestamps, UID and GID are fixed. Windows `.lib` uses COFF archives; other suffixes use GNU ar format. Thin and nested archives are rejected. `--out` is an exact filename and its parent must exist. The static variant uses LLVM ArchiveWriter. The external variant finds llvm-ar beside clang or through `CHALI_LLVM_AR`; `--llvm LLVM-AR` selects an explicit archiver.

`link` accepts multiple objects/archives and supports `--target`, `--shared`, repeated `--export SYMBOL`, `--entry SYMBOL`, `--debug`, `-L DIR` / `-LDIR` / `--library-path DIR`, repeated `--library NAME`, `--sysroot`, `--runtime-dir` and `--llvm CLANG`. Entry/export names are raw object symbols. Inputs enter one link group in their supplied order. Archives extract members on demand and are rescanned for cyclic references; unresolved symbols in unextracted members are irrelevant. LLD diagnoses duplicate definitions and referenced missing symbols. Shared libraries expose only explicit exports and hide other symbols. Arbitrary raw linker flags are not forwarded.

The static Windows x64 variant uses embedded LLD, CRT and ChaLi runtime assets without external clang. The external variant requires matching clang/LLD and the applicable system toolchain. Full cross-target linking requires a target runtime and SDK. Explicit `--no-runtime` omits CRT and the ChaLi runtime for programs supplying their own entry and dependencies. Embedded LLD can link such inputs for the supported Linux x64/ARM64 targets; successful generation is distinct from execution on the target. Ordinary standalone C programs may use default runtime linkage; unreferenced archive members are omitted.

Input checks cover object format, width, architecture and archive-member bounds; LLVM/LLD performs the final symbol, section and relocation validation. These tools are not an untrusted-code sandbox. Output is generated in a private workspace and published only on success. Failures preserve an existing output, and output cannot overwrite an input. Callers must check the exit status rather than treat an old output as evidence of a successful link.
