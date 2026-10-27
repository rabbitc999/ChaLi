# LLVM AOT compilation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../38-aot.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, `run` compiles and executes checked AOT code, while `aot` defaults to unchecked export. The compiler no longer embeds a VM execution engine. `.svmb` export has been removed; use native AOT artifacts. See [Native AOT debugging](43-native-debugging.md).

`chali aot input.chl --out build/program` lowers optimized ChaLi quadruples directly to LLVM intermediate representation. LLVM optimizes the module, emits machine code and links the native product. `chali --mode aot input.chl` selects the same path. Ordinary source execution uses checked AOT.

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

The product includes memory, string, list, printing and panic services and does not depend on the VM or LLVM dynamic libraries. Basic numeric operations, branches, loops, direct calls, recursion and fixed arrays are supported. Lists support creation, length, push, insert, indexed reads and writes, remove, pop, clear, search, value copying and for-in references. Elements include integers, floating-point values, String, nested lists and structs with supported leaf layouts. Copies own independent storage, structural changes are rejected while element references are active, and list bounds and object lifetime validation always run.

Language `opcode` creation, copying, moving and destruction use the calls lowered by the frontend. Member fields, owning aggregate fields and static `#NonCopyable` restrictions are preserved. User `new/drop/copy` bindings may target reachable ChaLi functions or C functions explicitly declared through `abi`; resource handles are never cast to machine function pointers. Strings, lists and other standard library host calls reuse the same heap, object store and glue registrations as the VM, while language function bodies execute LLVM-generated machine code. Capturing closures, function values and synchronous standard library callbacks use the shared invocation protocol; results preserve integer, floating-point and object-handle slot types. Isolated threads reuse the shared message, channel, event and task-control services. Scalar C ABI calls use matching LLVM types and explicit conversions. Structs, references, arrays, optional references and synchronous C callbacks use typed host glue shared with VM hosts, preserving narrow scalar widths, aliases, copy-back, returned-data copying and release contracts. Shared-library headers declare initialization, cleanup and exported signatures.

Exporting complex C ABI signatures also requires a GNU C++17 compiler compatible with the embedded host runtime; g++ is the default and CHALI_ABI_CXX selects another path. It compiles only host marshalling adapters; language functions still lower directly to LLVM instructions. Adapter headers are embedded, so no source checkout is required. This host dependency also applies to the static LLVM variant. Scalar-only ABI programs and programs without ABI declarations skip this step. Native products need no C++ compiler at runtime, but their declared dynamic libraries must be provided.

## LLVM linkage in the compiler

| Build | Native code generation | Plugin and release marker |
| --- | --- | --- |
| `make` or `make CHALI_STATIC_LLVM=0` | External LLVM clang under `library/llvm/` | `llvm-external` |
| `make llvm-static` | Statically linked LLVM optimization, x64 code generation and LLD, with embedded CRT link assets; no clang or LLVM DLL required | `llvm-static` |

The first static build runs [setup_static.py](../../library/llvm/setup_static.py), downloads pinned LLVM sources, verifies SHA-256 and incrementally builds the SDK under `library/llvm/`. With an existing SDK, use `make CHALI_STATIC_LLVM=1`. Windows x64 compiler outputs are named `chali-1.0.8-llvm-static-windows-x64.exe` and `chali-1.0.8-llvm-external-windows-x64.exe`; the version is read from `VERSION`. `CHALI_LLVM_SDK` selects an SDK with the same C++ ABI. The validated static distribution targets Windows x64. Static variants use the built-in backend by default; explicit `--llvm` selects an external driver. LLVM module export requires no code generation tools in either variant.

`python tools/release_variants.py` builds and validates both program and plugin variants in sequence. Release ZIPs are placed in `dist/llvm-static/` and `dist/llvm-external/`. Both plugins retain their independent 1.0.0 version; VSIX filenames use `-llvm-static` and `-llvm-external` suffixes. They are alternative builds of the same extension. To package one, run `npm run package -- --llvm-variant static --compiler COMPILER_PATH` in `vscode/`, or replace static with external. The script creates local artifacts; remote publication requires an explicit action.

Program code is emitted directly as LLVM instructions, basic blocks and global storage. The hand-written [AOT runtime](../../source/aot/runtime.c) is compiled into platform modules during maintenance using the pinned LLVM version and embedded in the compiler. `python tools/build_aot_runtime.py --check` verifies synchronization. Runtime layouts cover Windows x64 and Linux x64; the bundled executable toolchain targets Windows x64. Linux x64 build hosts require their native LLVM clang and system linker environment.

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

On Windows, native process arguments are converted from the CRT wide-character argument vector to UTF-8 before reaching main(List<String>). Chinese text, supplementary Unicode characters, empty strings, quotes and backslashes are preserved. Run python tests/chali_aot_arguments.py to compare against the VM and check native product paths containing Chinese text and spaces.

Online toolchain setup also covers Linux x64, Linux ARM64 and macOS ARM64. Use `python tools/fetch_llvm_toolchain.py --list` or select `--platform` explicitly. Automatic static SDK setup currently targets Windows x64; downloading a toolchain does not establish AOT support for that platform.
