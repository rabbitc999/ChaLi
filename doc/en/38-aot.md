# LLVM AOT compilation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../38-aot.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, `run` compiles and executes checked AOT code, while `aot` defaults to unchecked export. The compiler no longer embeds a VM execution engine. `.svmb` export has been removed; use native AOT artifacts. See [Native AOT debugging](43-native-debugging.md).

`chali aot input.chl --out build/program` generates LLVM code directly from the semantically checked AST without generating or assembling quadruples. LLVM optimizes the module, emits machine code and links the native product. `chali --mode aot input.chl` selects the same path. Ordinary source execution uses checked AOT.

Windows x64 dependencies use [online setup scripts](../../library/llvm/README.md). Run `python tools/fetch_llvm_toolchain.py` for external LLVM, or `python tools/fetch_llvm_sdk.py` for the prebuilt static LLVM/LLD SDK and matching GCC. Downloads and installations stay under ignored `build/` directories; SDK binaries and offline archive parts are not committed. Archives have pinned SHA-256 checksums. LLVM IR export alone does not require the code-generation toolchain.

All applicable AOT optimizations are enabled by default. External LLVM uses `-O3`; static LLVM uses `default<O3>` and aggressive code generation. Normal floating-point semantics and liveness checks remain; run enables array/pointer checks while aot exports default to omitting them. See [backend ownership optimization](40-backend-ownership.md) for destination storage, temporary transfers, bulk copies, direct calls, optional counters and comparison switches.

| Option | Behavior |
| --- | --- |
| `--out prefix` | Prefix for the `.ll` module, C ABI header and native output; Windows uses .exe/.dll, Linux uses no executable suffix or .so |
| `--llvm path` | Explicit external LLVM clang driver; the static variant defaults to its built-in backend; the external variant also accepts `CHALI_LLVM_CLANG` |
| `--emit-llvm` | Write the LLVM module and C ABI header without code generation or linking |
| `--checked` | Emit array bounds and pointer checks; on for run and off by default for aot exports |
| `--unchecked` | Explicitly omit these checks for debugging or performance comparisons; object lifetime and actual memory accesses remain checked |
| `--shared` | Build a shared library |
| `--export name` | Export a function; repeatable |
| `--link path` | Add a library; repeatable; scalar ABI libraries are linked automatically; typed marshalling loads declared libraries for complex ABI signatures |

The product includes memory, string, list, printing and panic services and does not depend on the compiler, a runtime library or LLVM dynamic libraries. Basic numeric operations, branches, loops, direct calls, recursion and fixed arrays are supported. Lists support creation, length, push, insert, indexed reads and writes, remove, pop, clear, search, value copying and for-in references. Elements include integers, floating-point values, String, nested lists and structs with supported leaf layouts. Copies own independent storage, structural changes are rejected while element references are active, and list bounds and object lifetime validation always run.

Language `opcode` creation, copying, moving and destruction use the calls lowered by the frontend. Member fields, owning aggregate fields and static `#NonCopyable` restrictions are preserved. User `new/drop/copy` bindings may target reachable ChaLi functions or C functions explicitly declared through `abi`; resource handles are never cast to machine function pointers. Strings, lists and other standard library calls reuse the shared runtime heap, object store and glue registrations, while language function bodies execute LLVM-generated machine code. Capturing closures, function values and synchronous standard library callbacks use the shared invocation protocol; results preserve integer, floating-point and object-handle slot types. Isolated threads reuse the shared message, channel, event and task-control services. Scalar C ABI calls use matching LLVM types and explicit conversions. Structs, references, arrays, optional references and synchronous C callbacks use the shared typed glue, preserving narrow scalar widths, aliases, copy-back, returned-data copying and release contracts. Shared-library headers declare initialization, cleanup and exported signatures.

Exporting complex C ABI signatures also requires a GNU C++17 compiler compatible with the embedded host runtime; g++ is the default and CHALI_ABI_CXX selects another path. It compiles only host marshalling adapters; language functions still lower directly to LLVM instructions. Adapter headers are embedded, so no source checkout is required. This host dependency also applies to the static LLVM variant. Scalar-only ABI programs and programs without ABI declarations skip this step. Native products need no C++ compiler at runtime, but their declared dynamic libraries must be provided.

## Runtime allocator

Since 1.0.11, native applications default to system `malloc`. Select `--allocator pool` to retain the previous arena and byte-heap allocation strategy. The malloc backend covers both host objects and individual language allocations, rather than subdividing a continuous byte buffer. Logical reference identities, invalidation on release, range checks, zero initialization and memory budgets remain active, including unchecked exports.

```sh
chali aot app.chl --allocator malloc --out build/app-malloc
chali aot app.chl --allocator pool --out build/app-pool
```

The `[aot]` table accepts `allocator = "malloc"` or `allocator = "pool"`; explicit CLI options override it. The option selects the generated program's allocator without changing the compiler's container ABI. Formal performance baselines use system malloc and record the actual allocator, check mode and LLVM configuration separately.

## LLVM linkage in the compiler

| Build | Native code generation | Plugin and release marker |
| --- | --- | --- |
| `make` or `make CHALI_STATIC_LLVM=0` | External LLVM clang under `library/llvm/` | `llvm-external` |
| `make llvm-static` | Statically linked LLVM optimization, x64 code generation and LLD, with embedded CRT link assets; no clang or LLVM DLL required | `llvm-static` |

The first static build runs [setup_static.py](../../library/llvm/setup_static.py), downloads pinned LLVM sources, verifies SHA-256 and incrementally builds the SDK under `library/llvm/`. With an existing SDK, use `make CHALI_STATIC_LLVM=1`. Windows x64 compiler outputs are named `chali-1.0.8-llvm-static-windows-x64.exe` and `chali-1.0.8-llvm-external-windows-x64.exe`; the version is read from `VERSION`. `CHALI_LLVM_SDK` selects an SDK with the same C++ ABI. The validated static distribution targets Windows x64. Static variants use the built-in backend by default; explicit `--llvm` selects an external driver. LLVM module export requires no code generation tools in either variant.

`python tools/release_variants.py` builds and validates both program and plugin variants in sequence. Release ZIPs are placed in `dist/llvm-static/` and `dist/llvm-external/`. Both plugins retain their independent 1.0.0 version; VSIX filenames use `-llvm-static` and `-llvm-external` suffixes. They are alternative builds of the same extension. To package one, run `npm run package -- --llvm-variant static --compiler COMPILER_PATH` in `vscode/`, or replace static with external. The script creates local artifacts; remote publication requires an explicit action.

Program code is emitted directly as LLVM instructions, basic blocks and global storage. The hand-written [AOT runtime](../../source/aot/runtime.c) is compiled into platform modules during maintenance using the pinned LLVM version and embedded in the compiler. `sh tools/build_aot_runtime.sh --check` verifies synchronization. Runtime layouts cover Windows x64 and Linux x64; the bundled executable toolchain targets Windows x64. Linux x64 build hosts require their native LLVM clang and system linker environment.

AOT does not reuse compiled outputs with `--incremental`, so ABI declarations come from the current semantic analysis. See the [functional regression](../../tests/chali_aot.py), [list and opcode differential checks](../../tests/chali_aot_values.py) and [static LLVM isolation checks](../../tests/chali_aot_static.py).

## Exporting a library without main

Place `export` before a function definition to expose it to native AOT callers:

```chali
#export("f_add")
int64 add(int64 left, int64 right)
{
    return left + right;
}
```

`ChaLi aot library.chl --shared --out build/library` generates a shared library and its C ABI
header. Use the symbols declared in that header (`f_add` in this example). Shared-library
sources may omit `main`; executable sources still require it. Call `chali_init()` before
exported functions and `chali_finish()` afterwards. These entries run global initialization
and cleanup respectively. Repeatable `--export` options can add further exported functions.
Module-qualified and overloaded functions retain their actual labels; consult the generated
header for their complete signatures.

Complex ABI value/reference checks run with python tests/chali_abi_values.py --aot; synchronous C callbacks and return ownership are checked by python tests/chali_abi_contracts.py --aot. Both are included in the full gate.

On Windows, native process arguments are converted from the CRT wide-character argument vector to UTF-8 before reaching main(List<String>). Chinese text, supplementary Unicode characters, empty strings, quotes and backslashes are preserved. Run python tests/chali_aot_arguments.py to check native product paths containing Chinese text and spaces.

Online toolchain setup also covers Linux x64, Linux ARM64 and macOS ARM64. Use `python tools/fetch_llvm_toolchain.py --list` or select `--platform` explicitly. Automatic static SDK setup currently targets Windows x64; downloading a toolchain does not establish AOT support for that platform.

## Cross-compilation and project configuration

The compiler host and the application target may differ. Supported target selections are `windows-x64` (`x86_64-w64-windows-gnu`), `linux-x64` (`x86_64-unknown-linux-gnu`) and `linux-arm64` (`aarch64-unknown-linux-gnu`).

```sh
chali aot hello.chl --target linux-arm64 --emit-llvm --out build/arm64/hello
chali aot hello.chl --target linux-arm64 --emit-obj --out build/arm64/hello
```

`--emit-llvm` exports target LLVM IR; `--emit-obj` generates a target `.o` without linking. These options are mutually exclusive. Windows objects use COFF, Linux objects use ELF. Static LLVM can generate objects using its embedded target backends; external LLVM requires clang with the selected backend.

Full cross-linking additionally requires matching target startup objects, C/C++ libraries, a linker and `libchali_aot_host.a`. Use `--sysroot` for the target system root, `--runtime-dir` for the ChaLi runtime library directory and `--cpu` to select a CPU. On Linux or WSL, `sh tools/build_cross_runtime.sh linux-arm64 2` prepares an isolated SDK with the installed GNU target toolchain under `build/cross-sdk/linux-arm64/`, including library licenses and hashes.

```toml
[aot]
target = "aarch64-unknown-linux-gnu"
cpu = "generic"
sysroot = "build/cross-sdk/linux-arm64/sysroot"
runtime_dir = "build/cross-sdk/linux-arm64/lib"
output = "executable"
```

Paths resolve relative to `chali.toml`. `output` accepts `llvm`, `object` or `executable`; the optional `llvm` key selects an external clang path. Explicit command-line options override project settings:

```sh
chali aot --out build/arm64/app
chali aot --target linux-x64 --emit-obj --out build/x64/app
```

`run` rejects a foreign target instead of starting an emulator. Object generation, full linking and execution are separate validation steps. Test the resulting application on its target or an appropriate emulator. Complex imported C ABI adapters currently require target-toolchain compilation and linking after object export.

## Target machine assembly

The project `[aot]` table accepts arrays named `assembly_sources`, `assembly_includes`, `objects`, `libraries`, and `search_paths`, plus the `library_lock` string for a precompiled-library lock file. Paths are relative to the directory containing `chali.toml`. Bare library names containing neither a dot nor a path separator are resolved by the linker.

Explicit `--asm`, `--asm-include`, and `--link` options replace their corresponding manifest lists. `--library-lock` replaces the manifest lock file. Manifest search paths follow command-line search paths. Object and assembly inputs participate in final linking.

`lto` defaults to `true` (full LTO) and also accepts `false`, `"off"`, `"full"`, and `"thin"`. `--lto-mode off|full|thin` overrides the project setting. `--lto` retains full-LTO behavior and `--no-lto` disables LTO; the last CLI mode flag wins. Disabling it emits a machine object before linking while module optimization retains the selected level. Static and external LLVM debug builds use O0 in the selected mode without upgrading to release optimization.

`library_implementation = "native"` is the default library selection; `"bitcode"` selects LLVM bitcode. `--library-implementation native|bitcode` overrides the manifest for all direct and transitive libraries. Selected bitcode must match the actual target, layout and exact LLVM version. See [library packages](44-library-packages.md#native-and-bitcode-implementation-selection) for mode conversion, missing-implementation errors and limits.

The manifest's `library_lock` also applies to source checking and language-server analysis. The lock and imported `.chlib` files enter the dependency set. The editor watches these files and requests fresh consumer diagnostics when they change. Invalid locks or packages produce errors; restoring valid contents triggers another check.

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

For a symbol supplied by linked objects, declare `#abi("C", "@link", "asm_add") int64 asm_add(int64 a, int64 b);`. `@link` is a compiler marker, never a filename or dynamic-library lookup. Fixed scalars use direct target C ABI calls. Structures, references, arrays, String, synchronous callbacks and declared release functions use the existing C-frame marshaller with linker-resolved native symbols. Complex signatures still require a target-compatible C++ compiler (`CHALI_ABI_CXX`) for platform layout adaptation; the public boundary uses only C APIs. Ordinary host declarations and existing library paths retain their behavior. Explicit empty library paths are rejected. See [raw storage and C ABI directives](41-unsafe-abi.md) for interoperability and ownership contracts.


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
