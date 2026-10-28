# 08 Host integration and C ABI

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-host-integration.md)
<!-- END LANGUAGE NAVIGATION -->

ChaLi has a single execution path: the front end emits quads and LLVM AOT produces native code. So
talking to C has exactly two directions — **ChaLi calling a C shared library**, and **a C host calling
a native library built from ChaLi**. Both go through the platform C ABI: no virtual machine, no
bytecode, no host runtime with a function registry to populate.

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

Both directions support synchronous callbacks under the same rules:

- C calling ChaLi: pass a ChaLi function value to an `#abi` entry and C calls it directly;
- ChaLi calling C: C hands a function pointer to ChaLi, which calls it like any function value.

The runtime address inside a function value is **not** a general machine function pointer. Use it only
during that synchronous call, on the same thread; do not cache it for an asynchronous callback. A
`panic` inside a callback crosses the C frame back to the top level and is reported as a runtime error
rather than a default return value.

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
