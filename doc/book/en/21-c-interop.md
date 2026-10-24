# Chapter 21 C interoperability

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../21-与C互操作.md)
<!-- END LANGUAGE NAVIGATION -->

An `abi` declaration describes an external C interface. Declare exact parameter and result types and follow the library's ownership rules. C ABI compatibility is about representation and calling conventions, not just matching function names.

Scalar values, structures, references and buffers need their supported marshalling paths. Buffer length, nullability and element widths are part of the contract. A C function returning a pointer into one of its arguments requires the argument storage to remain valid through return-value decoding.

The VM host and AOT host use the shared glue protocol for synchronous callbacks. Validate the actual number of arguments and preserve the original error. A callable language value includes runtime context; do not reinterpret its address as a native machine function pointer.

For AOT, scalar C calls can be emitted directly. Complex ABI shapes use a typed host adapter compiled with a compatible GNU C++17 compiler when exporting the application. The language function bodies still lower directly to LLVM. Application DLL/SO dependencies remain application dependencies after export.

Use the maintained [ABI buffer example](../../examples/abi_buffers/demo.chl) and [host integration guide](../../en/08-host-integration.md) to inspect exact declarations. Exercise: document who allocates and releases each buffer in an interface before writing its glue. Next: [bytecode deployment](22-bytecode.md).
