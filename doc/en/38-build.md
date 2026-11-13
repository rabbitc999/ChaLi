# LLVM build

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../38-build.md)
<!-- END LANGUAGE NAVIGATION -->

`build` is the release-build command; the former `aot` command has been removed. Project configuration uses `[build]`; `[aot]` is rejected. LLVM handles optimization, object-code generation, and native linking.

Successful `run` and `build` compilation reports `Compilation completed: OUTPUT` on standard error after publishing the final artifact. Run reports completion before executing the program, so a later runtime failure remains a failure. Standard output contains only program output. `--quiet` or `CHALI_QUIET=1` suppresses this notice without hiding diagnostics, statistics or program output. Check-only and JSON-diagnostic requests do not emit completion notices.

Default `run` and `build` publish final outputs beside the `.chl` source, including executables produced by run. Intermediate files use a unique workspace under the source-adjacent `.chali` directory, hidden on Windows and removed when empty after success or failure. Existing user files are preserved. Explicit output/retention settings remain available. VS Code defaults to source-adjacent output; static VSIX packages omit `Chali › Build: Llvm Path`, while external packages require it and pass `--llvm` for both build and run.

`run` compiles and executes checked native code. `build` performs a release build with checks disabled by default. See [build debugging](43-native-debugging.md).

### Windows application consoles

Windows executables use the console subsystem by default. Use `--subsystem windows` for GUI applications to launch from File Explorer without a console window. The option selects the PE subsystem. The program creates its own application windows.

```powershell
chali build app.chl --subsystem console --out app-console
chali build app.chl --subsystem windows --out app-gui
```

Set `subsystem = "windows"` under `[build]` in `chali.toml` to apply it to project builds, `run`, and editor builds. Command-line options take precedence. When building in stages, pass the option to `chali link`. Exported LLVM IR, assembly, and object files acquire a PE subsystem at final link time.

`chali build input.chl --out build/program` generates LLVM code directly from the semantically checked AST without generating or assembling quadruples. LLVM optimizes the module, emits machine code and links the native product. `chali --mode build input.chl` selects the same path. Ordinary source execution uses checked native compilation.

Windows x64 dependencies use [online setup scripts](../../library/llvm/README.md). Run `python tools/fetch_llvm_toolchain.py` for external LLVM, or `python tools/fetch_llvm_sdk.py` for the prebuilt static LLVM/LLD SDK and matching GCC. Downloads and installations stay under ignored `build/` directories; SDK binaries and offline archive parts are not committed. Archives have pinned SHA-256 checksums. LLVM IR export alone does not require the code-generation toolchain.

All applicable native compilation optimizations are enabled by default. External LLVM uses `-O3`; static LLVM uses `default<O3>` and aggressive code generation. Normal floating-point semantics and liveness checks remain; run enables array/pointer checks while release builds default to omitting them. See [backend ownership optimization](40-backend-ownership.md) for destination storage, temporary transfers, bulk copies, direct calls, optional counters and comparison switches.

| Option | Behavior |
| --- | --- |
| `--out prefix` | Prefix for the `.ll` module, C ABI header and native output; Windows uses .exe/.dll, Linux uses no executable suffix or .so |
| `--llvm PATH` | External variant only: select the LLVM clang driver; static variants report an option error. External variants can also use `CHALI_LLVM_CLANG`. |
| `--emit-llvm` | Write the LLVM module and C ABI header without code generation or linking |
| `--checked` | Emit array bounds and pointer checks; on for run and off by default for release builds |
| `--unchecked` | Explicitly omit these checks for debugging or performance comparisons; object lifetime and actual memory accesses remain checked |
| `--shared` | Build a shared library |
| `--export name` | Export a function; repeatable |
| `--link path` | Add a library; repeatable; scalar ABI libraries are linked automatically; typed marshalling loads declared libraries for complex ABI signatures |

The product includes memory, string, list, printing and panic services and does not depend on the compiler, a runtime library or LLVM dynamic libraries. Basic numeric operations, branches, loops, direct calls, recursion and fixed arrays are supported. Lists support creation, length, push, insert, indexed reads and writes, remove, pop, clear, search, value copying and for-in references. Elements include integers, floating-point values, String, nested lists and structs with supported leaf layouts. Copies own independent storage, structural changes are rejected while element references are active, and list bounds and object lifetime validation always run.

Language `opcode` creation, copying, moving and destruction use the calls lowered by the frontend. Member fields, owning aggregate fields and static `#NonCopyable` restrictions are preserved. User `new/drop/copy` bindings may target reachable ChaLi functions or C functions explicitly declared through `abi`; resource handles are never cast to machine function pointers. Strings, lists and other standard library calls reuse the shared runtime heap, object store and glue registrations, while language function bodies execute LLVM-generated machine code. Capturing closures, function values and synchronous standard library callbacks use the shared invocation protocol; results preserve integer, floating-point and object-handle slot types. Isolated threads reuse the shared message, channel, event and task-control services. Scalar C ABI calls use matching LLVM types and explicit conversions. Structs, references, arrays, optional references and synchronous C callbacks use the shared typed glue, preserving narrow scalar widths, aliases, copy-back, returned-data copying and release contracts. Shared-library headers declare initialization, cleanup and exported signatures.

Exporting complex C ABI signatures also requires a GNU C++17 compiler compatible with the embedded host runtime; g++ is the default and CHALI_ABI_CXX selects another path. It compiles only host marshalling adapters; language functions still lower directly to LLVM instructions. Adapter headers are embedded, so no source checkout is required. This host dependency also applies to the static LLVM variant. Scalar-only ABI programs and programs without ABI declarations skip this step. Native products need no C++ compiler at runtime, but their declared dynamic libraries must be provided.

## Runtime allocator

Since 0.32.10, native applications default to system `malloc`. Select `--allocator pool` to retain the previous arena and byte-heap allocation strategy. The malloc backend covers both host objects and individual language allocations, rather than subdividing a continuous byte buffer. Logical reference identities, invalidation on release, range checks, zero initialization and memory budgets remain active, including unchecked exports.

```sh
chali build app.chl --allocator malloc --out build/app-malloc
chali build app.chl --allocator pool --out build/app-pool
```

The `[build]` table accepts `allocator = "malloc"` or `allocator = "pool"`; explicit CLI options override it. The option selects the generated program's allocator without changing the compiler's container ABI. Formal performance baselines use system malloc and record the actual allocator, check mode and LLVM configuration separately.

## LLVM linkage in the compiler

| Build | Native code generation | Plugin and release marker |
| --- | --- | --- |
| `make` or `make CHALI_STATIC_LLVM=0` | External LLVM clang under `library/llvm/` | `llvm-external` |
| `make llvm-static` | Statically linked LLVM optimization, x64 code generation and LLD, with embedded CRT link assets; no clang or LLVM DLL required | `llvm-static` |

The first static build runs [setup_static.py](../../library/llvm/setup_static.py), downloads pinned prebuilt LLVM/LLD development libraries and the matching GCC, verifies SHA-256, and installs them into an isolated SDK directory. With an existing SDK, use `make CHALI_STATIC_LLVM=1`. Windows x64 compiler outputs are named `chali-1.0.0-llvm-static-windows-x64.exe` and `chali-1.0.0-llvm-external-windows-x64.exe`; the version is read from `VERSION`. `CHALI_LLVM_SDK` selects an SDK with the same C++ ABI. The validated static distribution targets Windows x64. Static variants use the built-in backend. Their `run`, `build`, `asm`, `ar`, and `link` commands reject `--llvm` with exit code 2; project `[build].llvm` is also rejected. Use the `llvm-external` variant to select external tools. LLVM module export requires no code generation tools in either variant.

`python tools/release_variants.py` builds and validates both program and plugin variants in sequence. Release ZIPs are placed in `dist/llvm-static/` and `dist/llvm-external/`. Plugin versions match the compiler version in `VERSION`; VSIX filenames use `-llvm-static` and `-llvm-external` suffixes. They are alternative builds of the same extension. To package one, run `npm run package -- --llvm-variant static --compiler COMPILER_PATH` in `vscode/`, or replace static with external. The script creates local artifacts; remote publication requires an explicit action.

Program code is emitted directly as LLVM instructions, basic blocks and global storage. The hand-written [native runtime](../../source/aot/runtime.c) is compiled into platform modules during maintenance using the pinned LLVM version and embedded in the compiler. `sh tools/build_aot_runtime.sh --check` verifies synchronization. Runtime layouts cover Windows x64 and Linux x64, x86, ARM32, ARM64, RISC-V 32/64 and little-endian MIPS32/64; the bundled executable toolchain targets Windows x64. Linux x64 build hosts require their native LLVM clang and system linker environment.

Native compilation does not reuse compiled outputs with `--incremental`, so ABI declarations come from the current semantic analysis. See the [functional regression](../../tests/chali_aot.py), [list and opcode differential checks](../../tests/chali_aot_values.py) and [static LLVM isolation checks](../../tests/chali_aot_static.py).

## Exporting a library without main

Place `export` before a function definition to expose it to native callers:

```chali
#export("f_add")
int64 add(int64 left, int64 right)
{
    return left + right;
}
```

`ChaLi build library.chl --shared --out build/library` generates a shared library and its C ABI
header. Use the symbols declared in that header (`f_add` in this example). Shared-library
sources may omit `main`; executable sources still require it. Call `chali_init()` before
exported functions and `chali_finish()` afterwards. These entries run global initialization
and cleanup respectively. Repeatable `--export` options can add further exported functions.
Module-qualified and overloaded functions retain their actual labels; consult the generated
header for their complete signatures.

Complex ABI value/reference checks run with python tests/chali_abi_values.py --aot; synchronous C callbacks and return ownership are checked by python tests/chali_abi_contracts.py --aot. Both are included in the full gate.

On Windows, native process arguments are converted from the CRT wide-character argument vector to UTF-8 before reaching main(List<String>). Chinese text, supplementary Unicode characters, empty strings, quotes and backslashes are preserved. Run python tests/chali_aot_arguments.py to check native product paths containing Chinese text and spaces.

Online toolchain setup also covers Linux x64, Linux ARM64 and macOS ARM64. Use `python tools/fetch_llvm_toolchain.py --list` or select `--platform` explicitly. Automatic static SDK setup currently targets Windows x64; downloading a toolchain does not establish native compilation support for that platform.

## Cross-compilation and project configuration

The compiler host and the application target may differ. Select a target by alias or canonical triple.

| Alias | LLVM triple | Target ABI |
| --- | --- | --- |
| `windows-x64` | `x86_64-w64-windows-gnu` | MinGW x64 |
| `linux-x64` | `x86_64-unknown-linux-gnu` | x86-64 GNU |
| `linux-arm32` | `armv7-unknown-linux-gnueabihf` | ARMv7-A GNU hard-float |
| `linux-arm64` | `aarch64-unknown-linux-gnu` | AArch64 GNU |
| `linux-x86` | `i686-unknown-linux-gnu` | 32-bit x86 GNU |
| `linux-riscv32` | `riscv32-unknown-linux-gnu` | RV32GC / ILP32D |
| `linux-riscv64` | `riscv64-unknown-linux-gnu` | RV64GC / LP64D |
| `linux-mipsel` | `mipsel-unknown-linux-gnu` | little-endian MIPS32r2 / O32 |
| `linux-mips64el` | `mips64el-unknown-linux-gnuabi64` | little-endian MIPS64r2 / N64 |

```sh
chali build hello.chl --target linux-arm64 --emit-llvm --out build/arm64/hello
chali build hello.chl --target linux-arm64 --emit-obj --out build/arm64/hello
```

`--emit-llvm` exports target LLVM IR; `--emit-obj` generates a target `.o` without linking. These options are mutually exclusive. Windows objects use COFF, Linux objects use ELF. Static LLVM can generate objects using its embedded target backends; external LLVM requires clang with the selected backend.

Full cross-linking additionally requires matching target startup objects, C/C++ libraries, a linker and `libchali_aot_host.a`. Use `--sysroot` for the target system root, `--runtime-dir` for the ChaLi runtime library directory and `--cpu` to select a CPU. On Linux or WSL, `sh tools/build_cross_runtime.sh linux-arm64 2` prepares an isolated SDK with the installed GNU target toolchain under `build/cross-sdk/linux-arm64/`, including library licenses and hashes. The script also accepts `linux-x86`, `linux-arm32`, `linux-riscv32`, `linux-riscv64`, `linux-mipsel` and `linux-mips64el` with the corresponding `i686-linux-gnu`, `arm-linux-gnueabihf`, `riscv32-linux-gnu`, `riscv64-linux-gnu`, `mipsel-linux-gnu` and `mips64el-linux-gnuabi64` GCC/G++ toolchains.

Full cross-linking uses external clang and LLD. If the installed LLVM distribution lacks the target backend, set `CHALI_LLVM_CLANG` to a clang that includes it and put the matching LLD on `PATH`. The SDK script records when MIPS GNU startup objects explicitly require an executable stack, and the linker uses that record to set the ELF stack permission.

MIPS32 and MIPS64 executables use non-PIE machine-code linking with target optimization and LTO disabled. MIPS32 and RV32 runtime environments need a matching `libatomic.so.1`.

```toml
[build]
target = "aarch64-unknown-linux-gnu"
cpu = "generic"
sysroot = "build/cross-sdk/linux-arm64/sysroot"
runtime_dir = "build/cross-sdk/linux-arm64/lib"
output = "executable"
```

Paths resolve relative to `chali.toml`. `output` accepts `llvm`, `object` or `executable`; the optional `llvm` key selects an external clang path. Explicit command-line options override project settings:

```sh
chali build --out build/arm64/app
chali build --target linux-x64 --emit-obj --out build/x64/app
```

`run` rejects a foreign target instead of starting an emulator. Object generation, full linking and execution are separate validation steps. Test the resulting application on its target or an appropriate emulator. Complex imported C ABI adapters currently require target-toolchain compilation and linking after object export.

## MCU firmware

`mcu-cortex-m4` uses Thumb on Cortex-M4 with the soft-float ABI. `mcu-rv32` uses RV32IMAC with ILP32. Linking uses external clang and LLD with the matching backend and writes an `.elf` file. Supply startup objects, runtime objects, and the board's `.ld` linker script with repeated `--link` arguments.

```sh
chali build app.chl --target mcu-cortex-m4 --llvm clang \
  --link board.ld --link startup.o --link board-runtime.a \
  --out build/firmware
```

Compile the C sources under `source/aot/mcu/` into the board runtime. A bare-metal port provides `HAL_Chali_Print`, `HAL_Chali_Allocate`, and `HAL_Chali_Free`. For an RTOS build, compile the HAL forwarding layer with `CHALI_MCU_RTOS` and provide the corresponding `RTOS_HAL_Chali_*` symbols. Both modes require the board to supply `HAL_Chali_Panic`. The print hook receives a byte pointer and length. The allocation hook returns a 16-byte-aligned address.

The MCU core omits file, process, and standard input entry points, and rejects imports of `std::reflect`. Dynamic strings and collections still require board runtime primitives that are unavailable. Bare-metal print samples ran in instruction emulators for Cortex-M4 and RV32. The Cortex-M4 [FreeRTOS test script](../../tests/chali_mcu_freertos.py) downloads and verifies a pinned kernel and uses QEMU to check that a scheduled task prints, allocates, and frees memory through the HAL.

## Target machine assembly

The project `[build]` table accepts arrays named `assembly_sources`, `assembly_includes`, `objects`, `libraries`, and `search_paths`. Paths are relative to the directory containing `chali.toml`. Bare library names containing neither a dot nor a path separator are resolved by the linker.

Explicit `--asm`, `--asm-include`, and `--link` options replace their corresponding manifest lists. Manifest search paths follow command-line search paths. Object and assembly inputs participate in final linking.

`lto` defaults to `true` (full LTO) and also accepts `false`, `"off"`, `"full"`, and `"thin"`. `--lto-mode off|full|thin` overrides the project setting. `--lto` retains full-LTO behavior and `--no-lto` disables LTO; the last CLI mode flag wins. Disabling it emits a machine object before linking while module optimization retains the selected level. With static and external LLVM, `--debug` uses O0 in the selected LTO mode. `--debug-optimized` explicitly retains DWARF with O3; see [native debugging](43-native-debugging.md).

Standalone `chali link --shared --out FILE` links under the final basename inside a private temporary directory before publishing. Windows DLL export tables and host import tables therefore record the delivered name, allowing C hosts to link and load the actual file. Failures still preserve the previous product.

`chali build app.chl --emit-asm --out build/app` exports `build/app.s` using the current target, CPU, check mode and optimization/debug settings. This is machine assembly, distinct from LLVM IR text `.ll`. It stops before object generation and linking, so cross-target assembly does not require target runtime link assets. The static variant uses its built-in LLVM backend; the external variant uses clang. Output uses the selected backend's default syntax (x86 AT&T, AArch64 target syntax); no NASM/MASM compatibility is implied.

The project setting `[build] output = "assembly"` selects the same output for native builds. An explicit `--emit-asm`, `--emit-obj`, or `--emit-llvm` overrides the project output; these three options are mutually exclusive. Ordinary `run` retains checked native execution. VSCode's output selector and build command recognize assembly and its `.s` suffix.

The `.ll` and any C ABI header remain available alongside `.s`. Complex C ABI signatures can also produce a `.abi.cpp` marshalling sidecar; assembly alone is not a complete linkable application. Reassembly and final linking require a matching target toolchain, runtime and dependencies. See the standalone assembler, linker and archiver below.

## Standalone assembler and assembly inputs

```sh
chali asm native/add.s --target windows-x64 --out build/add.obj -I native/include
chali asm native/add.s --target linux-x64 --out build/add.o
chali build main.chl --asm native/add.s --asm-include native/include --out build/app
```

`asm` accepts one `.s` file and an exact output path. Without `--out`, it replaces the input suffix with `.obj` for Windows and `.o` for Linux. `--target`, `--cpu`, `--debug`, `-I DIR` / `-IDIR`, `--include-path DIR` and the external-only `--llvm CLANG` are supported. Target and CPU must match the source instructions. GAS-compatible LLVM syntax is used; `.S` preprocessing and NASM/MASM are not supported. Include search adds the source directory before the explicitly listed directories. Input files are limited to 64 MiB; this is a compiler tool, not an untrusted-code sandbox.

The static variant uses LLVM MC in process without clang and rejects `--llvm`; the external variant accepts that option to select its integrated assembler. Objects are first produced in a private workspace and published only after successful assembly. A failed assembly preserves an existing output and cannot overwrite its own primary input. The output parent directory must exist. Source/include diagnostics retain their original positions.

`build` accepts repeated `--asm FILE.s` and `--asm-include DIR` options. Every object uses the same target and CPU as the language unit and participates in the final link. Use the established C ABI declaration to reference an assembly symbol; the assembly must follow that target's calling convention. These inputs require executable/shared-library output and cannot be combined with `--emit-asm`, `--emit-obj` or `--emit-llvm`. Assembling a cross-target object does not verify executable deployment on that target.

Declare a linked scalar C function as `#abi("C", "@link", "asm_add") int64 asm_add(int64 a, int64 b);`. The object or archive supplied to the linker provides the symbol. Signatures with structures, references, buffers or callbacks use a compatible dynamic library and the [C ABI marshalling rules](41-unsafe-abi.md).


## Standalone linking and static archives

```sh
chali ar build/first.o build/second.o --out build/libexample.a
chali link build/main.o build/libexample.a --out build/app.exe
chali link build/main.o -L build --library example --out build/app.exe
chali link build/public.o --shared --export public_entry --out build/example.dll
chali link build/start.o --target linux-arm64 --no-runtime --entry _start --out build/app
```

`ar` writes deterministic, indexed, self-contained archives from zero or more objects of the selected target. Timestamps, UID and GID are fixed. Windows `.lib` uses COFF archives; other suffixes use GNU ar format. Thin and nested archives are rejected. `--out` is an exact filename and its parent must exist. The static variant uses LLVM ArchiveWriter. The external variant finds llvm-ar beside clang or through `CHALI_LLVM_AR`; `--llvm LLVM-AR` selects an explicit archiver in the external variant; static variants reject this option.

`link` accepts multiple objects/archives and supports `--target`, `--shared`, repeated `--export SYMBOL`, `--entry SYMBOL`, `--debug`, `-L DIR` / `-LDIR` / `--library-path DIR`, repeated `--library NAME`, `--sysroot`, `--runtime-dir` and the external-only `--llvm CLANG`. Entry/export names are raw object symbols. Inputs enter one link group in their supplied order. Archives extract members on demand and are rescanned for cyclic references; unresolved symbols in unextracted members are irrelevant. LLD diagnoses duplicate definitions and referenced missing symbols. Shared libraries expose only explicit exports and hide other symbols. Arbitrary raw linker flags are not forwarded.

The static Windows x64 variant uses embedded LLD, CRT and ChaLi runtime assets without external clang. The external variant requires matching clang/LLD and the applicable system toolchain. Full cross-target linking requires a target runtime and SDK. Explicit `--no-runtime` omits CRT and the ChaLi runtime for programs supplying their own entry and dependencies. Embedded LLD can link such inputs for the registered Linux targets; successful generation is distinct from execution on the target. Ordinary standalone C programs may use default runtime linkage; unreferenced archive members are omitted.

Input checks cover object format, width, architecture and archive-member bounds; LLVM/LLD performs the final symbol, section and relocation validation. These tools are not an untrusted-code sandbox. Output is generated in a private workspace and published only on success. Failures preserve an existing output, and output cannot overwrite an input. Callers must check the exit status rather than treat an old output as evidence of a successful link.


### Target platform and boolean build options

`$system` and `$cpu` are bare string constants describing the target program's operating system and architecture. Windows, macOS and Linux use `"win"`, `"mac"` and `"linux"`; common architecture names are `"amd64"`, `"arm64"` and `"arm32"`. Cross builds use the selected target rather than the compiler host. CPU tuning through `--cpu` does not change the architecture macro. Name normalization does not add native output targets; consult the LLVM build chapter for supported targets.

```csharp
void main() {
    println("system={}, cpu={}", $system, $cpu);
    println("A={}, B={}", $build[A], $build["B"]);
}
```

`chali run main.chl -B A -BB` defines both names. A project can also declare:

```toml
[build]
build_option = ["A", "B"]
```

CLI and manifest names form a union. Names are case sensitive; duplicates have no additional effect. `$build[A]` and `$build["A"]` are equivalent: the bare name is a literal flag name, not a variable lookup. Defined names yield `true`, undefined names yield `false`. Values such as `-BA=false`, numbers and strings are unsupported. Names must be nonempty and contain only ASCII letters, digits, underscores, dots or hyphens; quote names containing dots or hyphens.

Inside macro templates, use `$if $build[A]`, `$else`, `$end_if` or `$if ($system == "win")` to select code. Ordinary expressions can read the same constants. `$build` accepts a literal name or string, not a computed runtime key. `-I` keeps its module-search-directory meaning.



### Persisting build settings in TOML

Extend the `[build]` table introduced in Chapter 11. This fragment targets Windows x64; other targets require matching runtime, SDK and linker inputs.

```toml
[build]
target = "windows-x64"
output = "executable"
lto = "full"
allocator = "malloc"
subsystem = "console"
build_option = ["A", "B"]
assembly_sources = []
assembly_includes = []
objects = []
libraries = []
search_paths = ["src"]
```

| Setting | Meaning and CLI precedence |
| --- | --- |
| target, cpu | Supported triple or alias and CPU tuning name; explicit --target and --cpu win |
| output | executable, llvm, object or assembly; explicit --emit-llvm, --emit-obj and --emit-asm win; run still compiles and executes |
| lto | true, false, full, thin or off; explicit LTO options win |
| allocator, subsystem | malloc/pool and Windows console/windows; explicit matching options win |
| sysroot, runtime_dir | Target SDK root and runtime directory; explicit matching options win |
| assembly_sources, assembly_includes | Assembly sources and include paths; an explicitly supplied CLI list replaces the corresponding manifest list |
| objects, libraries | Extra objects and libraries; an explicitly supplied CLI list replaces the corresponding manifest list |
| search_paths | Appends module and import lookup paths, resolved from the manifest directory |
| build_option | Boolean flag names, unioned with -B definitions and queried with $build[name] |

Bare library names without dots or path separators are resolved by the linker; other paths are relative to the manifest. CPU tuning does not change the target architecture reported by `$cpu`. `$system`, `$cpu` and `$build` become constants during macro expansion; see [Chapter 22](../book/en/22-macros.md).

The static LLVM variant uses its embedded backend and rejects `--llvm` and `[build] llvm`. External toolchain paths are supported by the external variant. A manifest cannot switch a static compiler to an external backend. Consult the [LLVM build reference](38-build.md) for the complete interface and [Chapter 11](../book/en/11-modules-projects.md) for entry and search rules.
