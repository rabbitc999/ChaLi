# 08 Host integration and C ABI

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-host-integration.md)
<!-- END LANGUAGE NAVIGATION -->

Ordinary ChaLi execution lowers the checked AST directly to LLVM IR and uses LLVM O3 to produce native code. So
talking to C has exactly two directions — **ChaLi calling a C shared library**, and **a C host calling
a native library built from ChaLi**. Both go through the platform C ABI: no virtual machine, no
bytecode interpreter. Generated code marshals these calls. A host kernel can also use the C registry interface below.

## C host registry

The public headers are `source/abi/chali_abi.h` and, for standard-library host-entry registration, `chali_standard.h`. C11 clients can include them independently. The implementation may use C++, while the boundary contains C structures, fixed-width slots, byte buffers and opaque handles. See the [C host example](../../example/host-c-api/main.c) and its [build instructions](../../example/host-c-api/README.md).

Create an owned table with `chali_abi_registry_create`, then supply a `ChaliAbiModuleDescriptor`. A module uses its own compiler to generate `chali_abi_local_info`; the host checks versions, architecture/platform, calling convention, layouts and required capabilities. Windows x86 public entries explicitly use the C calling convention. ARM32 descriptors distinguish ordinary and hard-float ABIs.

`chali_abi_registry_register` commits a complete module atomically. Names are copied and entry code must remain alive. Duplicate names, symbol collisions and allocation failure leave no partial entries and do not copy existing callback state. Owned C registries use the host heap rather than a temporary language arena. Sealing applies to the same runtime table through both C and private implementation paths. Queries may then run concurrently; entry-owned shared state follows that entry's thread contract.

`chali_abi_registry_call` checks arguments and results, restores input fields and clears partial output on failure. `chali_abi_registry_resolve` returns only direct C entries. Slots hold integers or double bit patterns; checked `chali_abi_argument_integer`/`chali_abi_argument_real` accessors reject a wrong kind, and an entry should return after a failed read. Pure scalar entries allow a null runtime. Language memory, records and function values require a runtime-provided context, never a fabricated opaque handle. External `#abi` calls retain their actual C prototypes rather than automatically receiving this frame.

`chali_abi_read`/`chali_abi_write` check logical ranges and liveness; raw bytes do not acquire typed ownership. Release an owned String with `chali_abi_string_release` if it is not transferred to the language caller. Byte views recognize both length headers and capacity descriptors and expire before allocation, growth, release or reentry; copy bytes that must be retained. Pair temporary raw blocks with `temporary_release` when releasing early.

`CHALI_ABI_CAP_BINARY` identifies raw-string and byte-list services. `chali_abi_string_bytes` copies an explicit byte length and preserves zero bytes and non-UTF-8 data; `chali_abi_string` still rejects embedded zero bytes. Both constructors share storage and release rules.

Query a byte-list length with `chali_abi_list_bytes` using a null output and zero capacity, then copy into a caller-owned buffer. Elements must be integers in `0..255`; invalid elements or a short buffer leave the output untouched. `chali_abi_list_from_bytes` creates an independent owned `List<uint8>` and publishes its handle only after construction. Pair it with `chali_abi_record_release` unless ownership has been returned to the language. Declare `CHALI_ABI_CAP_BINARY` among the module's required capabilities when using these services.


A successful record creation transfers its pointer; a zero result leaves it with the caller. Failed clone registration destroys the new copy and preserves the source. Type names are copied, destruction protocols cannot change under the same name, and destruction callbacks must not throw or outlive their code. `chali_abi_invoke` calls checked language function values with their captures and borrows the invoker without copying its state; handles are not machine function pointers.

`ChaliAbiError` carries a status and bounded caller-owned buffer. Service exceptions cannot escape the C boundary. Existing runtime errors and memory faults keep priority; the first explicit module diagnostic may refine a generic service failure. Built-in entry catalogs publish complete immutable data concurrently and reject invalid or oversized input as a whole. Run `python tests/chali_c_api_contract.py` and `python tests/chali_c_api_registry.py` to reproduce checks.

## Calling a C shared library from ChaLi

One `#abi` directive per function:

```chali
#abi("C", "./host.dll", "hostApply")
int64 hostApply(int64 fn(int64) function, int64 value);
```

The third argument is the symbol the library actually exports; the declaration uses the local ChaLi
name, and the two may differ.

- **Paths** resolve from the directory of the **source file holding the declaration**, including
  declarations in imported modules; separators are normalized.
- **The product must sit next to the library**: the Windows loader searches the executable directory
  and `PATH` at process start. On Linux use `host.so` and drop the extension from the product.
- **Type mapping**: scalars by value; `T mut&` / `T const&` map to `T *` / `const T *`; `String` maps to
  `const char *`; structs by value with C layout, `packed` for `#pragma pack`; arrays as `T[]` with a
  separate length; `Option<T &>` is the only channel for a null pointer.
- **Function values cross the boundary**: `int64 fn(int64)` arrives in C as `int64_t (*)(int64_t)` and
  C may call it directly; the captured environment travels with the value. See the
  [synchronous callback example](../../example/native-callbacks/README.md).

C only sees a signature, so the declaration must match the real C function item by item: return type,
argument widths, calling convention and struct field order.

## Calling ChaLi from a C host

Build a shared library with `--shared --export` and use the generated header:

```sh
chali aot app.chl --out build/app --shared --export add
```

`build/app.h` contains the C declarations:

```c
void chali_init(void);
void chali_finish(void);
int64_t f_add(int64_t p0, int64_t p1);
```

The contract is short but the order matters:

1. `chali_init()` allocates the runtime memory, loads the data area and runs global initializers;
2. call `f_name(...)`;
3. `chali_finish()` runs global cleanup, flushes output and releases the runtime memory.

After `chali_finish()` every previously obtained handle is invalid. Exported functions only take and
return C scalars; pass structs by value and use the reference form (`T *`) when the callee should mutate
in place. Without `--export` the header only exposes `chali_init` and `chali_finish`.

The `--shared` product is a shared library whose loading and lifetime belong to the host. A plain
`chali aot app.chl` is a self-contained executable — the form you ship to end users; see
[build artifacts](../15-artifacts.md).

## Synchronous callbacks

Function-value parameters in `#abi` declarations use synchronous C adapter trampolines. The C library
receives a machine callback matching its actual prototype, while the adapter retains the ChaLi capture
environment and error channel. C API modules invoke runtime function values through `chali_abi_invoke`,
with explicit slot kinds and a checked return kind. A runtime handle is **not** a general machine
function pointer and must not be cast to bypass this adapter.

Use callbacks only on the same thread during the contracted synchronous call. Do not retain them as
asynchronous entries. If a callback panics, the checked trampoline records failure. After the C function
returns, the adapter releases its return buffer under the declared contract before propagating the
original runtime error. Partial output does not become a successful result.

Strings, lists, maps and other resource-owning values do not transfer ownership through this mechanism.
Design the interface explicitly with scalars, references or a buffer you manage yourself.

## Debugging and boundaries

- Reflection is compiled in by default; `--no-reflection` switches it off, after which `@Reflect`
  reports that the feature is disabled.
- Use `--debug` to keep native symbols and source lines and to lower the optimization level, then
  attach GDB — see [native debugging](43-native-debugging.md).
- `--memory-size` sets the runtime heap budget; process boundaries and threads are in
  [capabilities](17-capabilities.md).
- There is no VM sandbox here: C code reached through `#abi` runs with the host's privileges, so the
  interface design is the security boundary.

See [toolchain](09-toolchain.md), [AOT compilation](38-aot.md) and the
[callback example](../../example/native-callbacks/README.md).

`chali_abi_record_make_clonable` registers a C clone callback for container deep copies and thread snapshots. The callback must return an independent record without handles owned by the source runtime, return null on failure, and never throw. Records with the same type name must retain identical destroy and automatic-clone callbacks. Require `CHALI_ABI_CAP_RECORD_CLONE` when using this service. Records created by `chali_abi_record_make` remain unavailable for automatic copying.

`chali_abi_record_probe` borrows a live record with a matching type. Zero, unknown, stale, and mismatched handles return null without replacing an existing error. It requires a valid frame and uses the usual record borrow lifetime. Declare `CHALI_ABI_CAP_RECORD_PROBE` when using this optional query. Use `chali_abi_record_borrow` for a strict borrow that reports invalid records.

`chali_abi_list_append_text` copies borrowed bytes directly into an independent `List<String>` element, preserving zero bytes without a generic native-value wrapper. Callers must provide a matching list type. The runtime checks the handle, active iterator borrows, and input length, and appends only after constructing the element. Require `CHALI_ABI_CAP_LIST_TEXT`; ownership of the input buffer stays with the caller.
