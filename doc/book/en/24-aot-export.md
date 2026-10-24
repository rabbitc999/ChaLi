# Chapter 24 LLVM AOT export

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../24-AOT导出.md)
<!-- END LANGUAGE NAVIGATION -->

AOT exports native machine code from a ChaLi program. The frontend checks the program and optimizes the quadruple representation, then lowers reachable language functions to LLVM. LLVM performs optimization and target code generation before the linker combines the application with its runtime.

```text
ChaLi source
    |
    v
Frontend checks and quadruple optimization
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

`--emit-llvm` stops at the LLVM module. A compiler with static LLVM embeds its compiler-side LLVM facilities; an external variant uses its configured LLVM toolchain. The exported application does not require a VM or LLVM DLL merely because the compiler used LLVM. Its own declared C libraries must still be deployed.

Mark public functions with `export`. A dynamic library can omit `main`:

```chali
export int64 add(int64 left, int64 right)
{
    return left + right;
}
```

```sh
chali aot library.chl --shared --out build/library
```

Use the generated C header for actual exported names and signatures. Call `chali_init()` before exported operations and `chali_finish()` when finished. Complex imported C ABI shapes require the compatible host adapter compiler described in [chapter 21](21-c-interop.md).

Cross-building the ChaLi compiler for another CPU is separate from supporting AOT export on that CPU. Current AOT target code and toolchain assumptions must be checked; an ARM64 compiler package alone does not prove ARM64 AOT support. VM/AOT execution performance is measured separately in the [performance report](../../en/12-vm-performance.md). Next: [the student system](25-student-system.md).
