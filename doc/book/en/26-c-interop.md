# Chapter 26 C ABI and host interoperation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../26-与C互操作.md)
<!-- END LANGUAGE NAVIGATION -->

C libraries provide databases, system services and mature algorithms. Crossing the boundary requires agreement on widths, layout, pointers, results and resource responsibility. Finding a symbol does not establish that a call is correct.

Build the interface through prototypes, marshalling, resource/error handling and synchronous callbacks. A declaration fragment still needs a real compatible library to run, and deployed applications retain their actual library dependencies.

## 26.1 Declare the actual host prototype

Begin with the library's real C prototype rather than guessing a declaration that compiles. Identify its name, result and parameters, then verify widths, pointer directions and structure layouts. Symbol existence is only the first step.

The ABI directive connects the declaration to a convention, library and exported symbol. Int64 is not a universal substitute for platform pointers or integers, and a declaration cannot repair an incompatible native implementation.

See the [C ABI buffer example](../../examples/abi_buffers/demo.chl), which requires building its native.c library.

Declare the actual C prototype: an int32_t return is different from void even when both use an int32_t pointer argument. This fragment corresponds to void bump(int32_t*), called with &value:

```chali
#abi("C", "./native.dll", "bump")
void bump(int32 mut& value);
```

The ABI directive associates the following prototype with C symbol bump in the specified dynamic library. Its strings identify convention, library path and symbol. The trailing semicolon declares a prototype without a ChaLi body. The actual native implementation must match the signature and platform ABI.

Mut& expresses possible writes to the supplied integer source, while void supplies no ordinary result. This fragment requires a real native.dll, compatible exports and calling code before it can run. Renaming a library suffix does not complete a port, and ChaLi logical addresses cannot simply be passed as C pointers.


Library paths are relative to the source declaring the function, and local names may differ from exported symbol names. The linked project includes complete struct, array and optional-reference examples.

An existing host registry can also associate implementations with impl Type : Interface = host_symbol;. See demo_counter_from_uint in [cpp_glue.chl](../../../example/samples/cpp_glue.chl). This requires an existing host implementation rather than treating a symbol name as an arbitrary C function pointer.

## 26.2 References, arrays and marshalling

A single-object reference and an array parameter are different declarations. Host arrays require a length and valid buffer; returned arrays need the supported length annotation. A language array object is not itself a machine buffer address. Establish who allocates, copies, mutates and frees variable-sized data.

## 26.3 Resources and failure boundaries

Check status codes and close handles on every return path. Copy borrowed host buffers when the library reuses them under its contract. External binaries can violate prototypes or write out of bounds; static language checks cannot establish all of their behavior.

## 26.4 Function values and synchronous callbacks

Use the supported host context and function-value protocol for synchronous callbacks. Validate arguments, returns, captures, reentry and failure propagation. A captured function value is not a machine function pointer. Deployment also needs the external libraries and their licenses.

### Summary

Review agreement of prototypes, data transfer, ownership and failure reporting. Distinguish input from output pointers, object references from contiguous arrays, and borrowed buffers from owned copies.

Environment-bearing functions require supported callback protocols. Pointer casts cannot remove environment or failure-propagation obligations. Contain low-level complexity behind small, checkable interfaces for business code.

## 26.5 Exercises

1. What must be checked before declaring an external function?
2. Are array parameters and single-object references interchangeable?
3. What must a failure path do after opening a handle?
4. Does embedded LLVM remove application library dependencies?

### Answers and checks

1. Check prototypes, layout, lengths, ownership, threading and errors.
2. No; match the actual ABI.
3. Close successfully acquired resources under the library contract.
4. No; toolchain and application dependencies are separate.

Next: [Chapter 24](24-compiler-directives.md).
