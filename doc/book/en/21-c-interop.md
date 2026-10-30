# Chapter 21 C ABI and host interoperation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../21-与C互操作.md)
<!-- END LANGUAGE NAVIGATION -->

C ABI exchanges data under a fixed external contract. Language values, borrows, contiguous buffers and host pointers have distinct boundaries. Establish prototypes and ownership before declaring calls.

## 21.1 Declare the actual host prototype

Use #abi to specify calling convention, library and symbol. Widths, signedness, struct layout and return rules must match the library. int64 is not a universal substitute for pointers and integers on every platform.

See the [C ABI buffer example](../../examples/abi_buffers/demo.chl), which requires building its native.c library.

Declare the actual C prototype: an int32_t return is different from void even when both use an int32_t pointer argument. This fragment corresponds to void bump(int32_t*), called with &value:

```chali
#abi("C", "./native.dll", "bump")
void bump(int32 mut& value);
```

Library paths are relative to the source declaring the function, and local names may differ from exported symbol names. The linked project includes complete struct, array and optional-reference examples.

An existing host registry can also associate implementations with impl Type : Interface = host_symbol;. See demo_counter_from_uint in [cpp_glue.chl](../../../example/samples/cpp_glue.chl). This requires an existing host implementation rather than treating a symbol name as an arbitrary C function pointer.

## 21.2 References, arrays and marshalling

A single-object reference and an array parameter are different declarations. Host arrays require a length and valid buffer; returned arrays need the supported length annotation. A language array object is not itself a machine buffer address. Establish who allocates, copies, mutates and frees variable-sized data.

## 21.3 Resources and failure boundaries

Check status codes and close handles on every return path. Copy borrowed host buffers when the library reuses them under its contract. External binaries can violate prototypes or write out of bounds; static language checks cannot establish all of their behavior.

## 21.4 Function values and synchronous callbacks

Use the supported host context and function-value protocol for synchronous callbacks. Validate arguments, returns, captures, reentry and failure propagation. A captured function value is not a machine function pointer. Deployment also needs the external libraries and their licenses.

## 21.5 Exercises

1. What must be checked before declaring an external function?
2. Are array parameters and single-object references interchangeable?
3. What must a failure path do after opening a handle?
4. Does embedded LLVM remove application library dependencies?

### Answers and checks

1. Check prototypes, layout, lengths, ownership, threading and errors.
2. No; match the actual ABI.
3. Close successfully acquired resources under the library contract.
4. No; toolchain and application dependencies are separate.

Next: [Chapter 22](22-compiler-commands.md).
