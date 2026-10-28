# Chapter 24 LLVM AOT export

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../24-AOT导出.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, run uses checked AOT, and aot defaults to unchecked export. --debug retains native source and variable information. VS Code breakpoints, stepping and variable inspection use GDB; see [Native debugging](../../en/43-native-debugging.md). The .svmb export has been removed.

AOT exports native machine code from a ChaLi program. After type, borrow and lifetime checks, the C++ backend generates LLVM IR directly from the checked AST. Normal run and AOT paths skip ChaLi quadruple generation, serialization and assembly. LLVM performs optimization and target code generation before the linker combines the application with its runtime.

```text
ChaLi source
    |
    v
Frontend checks and checked AST
    |
    v
LLVM module
    |
    +-- Embedded static LLVM --+
    |                         |
    +-- External LLVM tools ---+
                              |
                              v
             Object code + AOT runtime + declared C libraries
                              |
                              v
                    Native executable or library
```

```sh
chali aot hello.chl --out build/hello
chali aot hello.chl --out build/hello --emit-llvm
```

Dependency setup downloads pinned archives into ignored `build/` caches. `python tools/fetch_llvm_sdk.py` prepares prebuilt static LLVM/LLD and its matching GCC; static objects use an isolated build directory. SDK binaries and archive parts are not stored in source control.

`--emit-llvm` stops at the LLVM module. A compiler with static LLVM embeds its compiler-side LLVM facilities; an external variant uses its configured LLVM toolchain. The exported application does not require a VM or LLVM DLL merely because the compiler used LLVM. Its own declared C libraries must still be deployed.

Mark public functions with `export`. A dynamic library can omit `main`:

```chali
#export("f_add")
int64 add(int64 left, int64 right)
{
    return left + right;
}
```

```sh
chali aot library.chl --shared --out build/library
```

Use the generated C header for actual exported names and signatures. Call `chali_init()` before exported operations and `chali_finish()` when finished. Complex imported C ABI shapes require the compatible host adapter compiler described in [chapter 21](21-c-interop.md).

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

## Development and delivery

```sh
chali run app.chl
chali aot app.chl --out build/app
chali aot app.chl --checked --out build/app-checked
```

Development execution requires a compiler and its LLVM toolchain. Native applications need neither ChaLi, a VM nor an LLVM DLL at runtime. Ship the executable for its actual platform and architecture, together with application resources and declared C ABI dynamic libraries. Windows uses `.exe`; Linux executables have no suffix. Both checked and unchecked exports retain ownership, actual-memory and liveness protections.

A compiler built for an architecture does not establish AOT support for that target. Check the actually supported and tested configurations in [LLVM AOT compilation](../../en/38-aot.md).

## Debugging and release

```sh
chali aot app.chl --checked --debug --out build/app-debug
```

Debug builds retain DWARF source locations, parameters and locals at O0. VS Code uses GDB for breakpoints, stepping and variable inspection. Release builds keep highest optimization, LTO and dead-code elimination. See [Native AOT debugging](../../en/43-native-debugging.md).

Before delivery, verify arguments, input/output, C ABI dependencies, resources and error paths. Record the compiler version, actual target and artifact hashes. LLVM modules and object files are analysis artifacts rather than runtime dependencies.

## Summary and exercises

`run` defaults to checked AOT; `aot` defaults to unchecked export. Ship the native executable, application resources and declared C ABI libraries. A compiler's embedded LLVM does not remove an application's own library dependencies.

1. Export a checked debug build and an unchecked release build. Record their target and hashes.
2. Explain which tools are needed for an ARM64 object and which additional assets are needed for an ARM64 executable.
3. Configure `[aot]`, then override the target with `--target linux-x64 --emit-obj`.

Answers: use `--checked --debug` for the debug build; object generation needs the target LLVM backend, while linking also needs the target SDK and ChaLi runtime. Command-line target and output options take precedence over TOML. Validate execution separately on the target environment.

Next: [the student information system](25-student-system.md).
