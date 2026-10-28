# Chapter 21 C interoperability

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../21-与C互操作.md)
<!-- END LANGUAGE NAVIGATION -->

A per-function `#abi("C", "library", "native_symbol")` compiler directive describes an imported C interface. The old ABI block syntax has been removed. Declare exact parameter and result types and follow the library's ownership rules. C ABI compatibility is about representation and calling conventions, not just matching function names.

Scalar values, structures, references and buffers need their supported marshalling paths. Buffer length, nullability and element widths are part of the contract. A C function returning a pointer into one of its arguments requires the argument storage to remain valid through return-value decoding.

A ChaLi function value can cross the boundary as a native function pointer: declare a function-typed parameter and the C side receives `int64_t (*)(int64_t)` it can call directly, with captures carried by the runtime. The address is valid only for that call on that thread, so never cache it for a later or asynchronous callback. A `panic` inside the callback crosses the C frame and is reported as a runtime error rather than a default return value. Validate the actual argument count and preserve the original error.

Use `#export("native_symbol")` on a function implementation to expose scalars or resource-free value structures using the target C ABI. The AOT build command selects the output DLL/SO, and the generated C header declares its actual types.

For AOT, scalar C calls can be emitted directly. Complex ABI shapes use a typed host adapter compiled with a compatible GNU C++17 compiler when exporting the application. The language function bodies still lower directly to LLVM. Application DLL/SO dependencies remain application dependencies after export.

Use the maintained [ABI buffer example](../../examples/abi_buffers/demo.chl), the [synchronous callback example](../../../example/native-callbacks/README.md) and the [host integration guide](../../en/08-host-integration.md) to inspect exact declarations. Exercise: document who allocates and releases each buffer in an interface before writing its glue. Next: [compiler commands](22-compiler-commands.md).
