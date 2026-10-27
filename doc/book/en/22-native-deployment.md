# Chapter 22 Native deployment

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../22-部署原生产物.md)
<!-- END LANGUAGE NAVIGATION -->

Since 1.0.8, ChaLi runs and deploys through AOT. `run` compiles and executes checked machine code; `aot` defaults to unchecked export. The `.svmb` format and export commands have been removed.

## Development and delivery

```sh
chali run app.chl
chali aot app.chl --out build/app
chali aot app.chl --checked --out build/app-checked
```

Development execution requires a compiler and its LLVM toolchain. Native applications need neither ChaLi, a VM nor an LLVM DLL at runtime. Ship the executable for its actual platform and architecture, together with application resources and declared C ABI dynamic libraries. Windows uses `.exe`; Linux executables have no suffix. Both checked and unchecked exports retain ownership, actual-memory and liveness protections.

A compiler built for an architecture does not establish AOT support for that target. Check the actually supported and tested configurations in [LLVM AOT compilation](../../en/41-aot.md).

## Debugging and release

```sh
chali aot app.chl --checked --debug --out build/app-debug
```

Debug builds retain DWARF source locations, parameters and locals at O0. VS Code uses GDB for breakpoints, stepping and variable inspection. Release builds keep highest optimization, LTO and dead-code elimination. See [Native AOT debugging](../../en/46-native-debugging.md).

Before delivery, verify arguments, input/output, C ABI dependencies, resources and error paths. Record the compiler version, actual target and artifact hashes. LLVM modules and quadruple dumps are analysis artifacts rather than runtime dependencies.

## Summary and exercises

`run` uses checked AOT; `aot` defaults to unchecked export, with explicit `--checked` available. Deliver native applications and their dependencies. For practice, describe these defaults, produce a `--checked --debug` build, and explain why cross-compiling the compiler does not validate a native target.
