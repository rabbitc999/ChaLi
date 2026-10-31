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

### 26.4.1 Providing host capabilities through a C function table

The earlier `#abi` declarations call existing C prototypes from ChaLi. A host can also register functions for the runtime to call by name. The public header is `source/abi/chali_abi.h`. See the complete [C11 host program](../../../example/host-c-api/main.c) and its [build instructions](../../../example/host-c-api/README.md).

Begin with its addition entry:

```c
/**
 * @brief 把两个已经检查的整数槽相加。
 * @param frame 本次同步调用帧。
 */
static void CHALI_ABI_CALL add(ChaliAbiFrame *frame)
{
    int64_t left = 0, right = 0;
    if (!chali_abi_argument_integer(frame, 0, &left)) return;
    if (!chali_abi_argument_integer(frame, 1, &right)) return;
    chali_abi_return_integer(frame, left + right);
}
```

`ChaliAbiFrame` describes this call, with arguments stored in typed 64-bit slots. `chali_abi_argument_integer` checks the index and slot kind. Return immediately when it fails so that the computation does not continue with an invalid argument. The last line writes an integer result. The example passes 20 and 22 and receives 42.

Registration also requires a target descriptor and argument count. This fragment uses the previously declared `add`:

```c
ChaliAbiInfo abi = chali_abi_local_info(0);
const ChaliAbiRecord records[] = { { "host::add", add, 2 } };
const ChaliAbiModuleDescriptor module = {
    sizeof(module), &abi, records, 1
};
```

The first line obtains version, target, calling convention and layout from the current C compiler rather than copying numbers from another target. The record associates a name and entry with two arguments. The module descriptor passes the entire table to `chali_abi_registry_register`; inspect its returned status before continuing.

The complete program creates a registry with `chali_abi_registry_create`, then registers, seals and calls it, and finally releases it with `chali_abi_registry_destroy`. Registration copies names, while entry code must remain alive. A name collision or allocation failure rolls back all new entries from that module. A sealed table supports lookup and calls but no further mutations.

Errors use the caller's bounded `ChaliAbiError` buffer together with a separate status code. A C entry reports failure with `chali_abi_fail`; partial output is not returned as a successful result. Host implementations must not throw C++ exceptions across the public C boundary.

### 26.4.2 Byte views, owned values and callback contexts

Addition does not access the language heap, so its example can use a null `frame.runtime`. Strings, logical memory, host records and language function values need a context supplied by the runtime. Obtain opaque handles through their corresponding interfaces instead of fabricating a context in C.

`chali_abi_bytes` returns a byte view and length without guaranteeing a terminating zero. Stop using the view after the synchronous borrow. Allocation, growth, release or reentry into language code invalidates the previous pointer. Keep a copy in your own valid storage when stable text is needed, and process its actual length.

`chali_abi_string` copies input bytes into a language string; pair the resulting owned handle with `chali_abi_string_release`. Temporary logical buffers use `chali_abi_temporary` and `chali_abi_temporary_release`. These handles and pointers returned by `malloc` have different owners; use the release operation belonging to the creator.

To invoke a captured ChaLi function value, use `chali_abi_invoke`, provide slot kinds and check the result kind. The runtime maintains the environment, handles synchronous reentry and propagates failure. Machine callbacks in `#abi` prototypes use synchronous adapter trampolines. A runtime function handle itself cannot be cast to a machine function pointer.

Follow the [actual C host](../../../tests/fixtures/c_api_library_host.c) and [C callback library](../../../tests/fixtures/c_api_owned_callback.c) for a full round trip. A C function table calls an exported ChaLi function, which captures an offset in a closure. The C library invokes its synchronous trampoline and returns owned text. The language adapter releases that C buffer under its contract before propagating the result or failure. The normal result is 42, and the buffer is released once.

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

Next: [Chapter 27](27-aot-export.md).
