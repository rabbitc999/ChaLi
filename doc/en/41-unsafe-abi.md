# Raw storage and C ABI directives

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../41-unsafe-abi.md)
<!-- END LANGUAGE NAVIGATION -->

## Unsafe scopes and storage

`inline unsafe T f(...)` combines module visibility with the unsafe calling contract; the body retains unsafe permission.

`unsafe { ... }` permits unsafe calls and explicit integer/reference conversions. An `unsafe` function requires an unsafe calling context and grants that permission to its body. Closures and other functions are checked independently. An unsafe function value cannot become an ordinary function value implicitly.

```chali
import std::memory;

void main() {
    std::lang::RawStorage<String> slots = std::lang::RawStorage<String>::allocate(8);
    unsafe {
        std::lang::initAt<String>(slots.pointer(0), "hello");
        String value = std::lang::takeAt<String>(slots.pointer(0));
        assert(value == "hello");
    }
    slots.close();
}
```

`RawStorage<T>` owns uninitialized capacity and implements `Closeable`. Spare slots do not construct elements. `RawPtr<T>` holds allocation identity and an element index, rather than a host machine pointer. Do not modify internal handles. A pointer at `capacity` is a valid one-past position, but cannot be dereferenced. `__rawSize(T)` and `__rawAlign(T)` query language storage layout inside unsafe scopes. Direct array types and host aliases are unsupported raw elements; wrap arrays in a named struct.

The public `std::lang::rawSize<T>()` queries the same value layout from safe code. For example, `rawSize<uint8>() == 1` and `rawSize<int64>() == 8`. Aggregate sizes include their language storage slots. Owning values such as String report the value slot size, excluding separately allocated contents and capacity. This query does not access an object or describe the host C ABI `sizeof`. Void, direct arrays and host aliases are unsupported.

| Operation | Behavior |
| --- | --- |
| `initAt<T>(pointer, value)` | Requires an uninitialized slot; moves the supplied value into it |
| `readAt<T>(pointer)` | Reads a live slot using ordinary independent value semantics |
| `borrowAt<T>(pointer)` | Borrows a live slot; the caller must prevent use after cleanup, move-out or release |
| `takeAt<T>(pointer)` | Transfers ownership and marks the source uninitialized |
| `dropAt<T>(pointer)` | Cleans a live element while retaining capacity |
| `relocateNonOverlapping<T>(to, from, count)` | Validates the full ranges and states before moving; destination must be uninitialized and ranges disjoint; zero elements return immediately |
| `RawStorage<T>::close()` | Cleans only initialized elements, then frees storage; repeated close does nothing |

Type identity, capacity overflow, alignment, initialization state and allocation lifetime checks remain. Supported raw alignment is at most 16 bytes. Unsafe operations impose extra obligations on the caller; these interfaces are not a safe reference API that bypasses ownership rules.

### Type-erased bare pointers and byte operations

`BarePtr<T>` stores only a ChaLi logical byte address, without runtime type tags, initialization state or ownership. `T` determines the static layout and arithmetic stride. `p + n`, `n + p` and `p - n` offset by `n * sizeof(T)`; `p - q` returns an integral element distance. Callers should calculate distances within one allocation and dereference only valid positions. Address arithmetic itself accesses no memory; dereferences and byte operations require `unsafe`.

```chali
unsafe {
    std::lang::BarePtr<int64> p = std::lang::barePointer<int64>(slots.pointer(0));
    std::lang::BarePtr<int64> q = p + 1;
    assert(q - p == 1);
    std::lang::memset<int64>(p, (uint8)0, 8);
    int64 mut& value = std::lang::dereference<int64>(p);
    *value = 42;
}
```

Here `slots` is a `RawStorage<int64>`. `barePointer<T>` obtains an address, allowing uninitialized and one-past positions without checking the actual slot type. `pointerFromAddress<T>` forms a pointer from a numeric address, and `dereference<T>` interprets it with the static type. These operations require `unsafe`. Logical addresses cannot be passed directly as machine C pointers.

Lengths in `memcpy<T,U>(destination, source, bytes)` and `memset<T>(destination, uint8 value, bytes)` are byte counts. Their implementation checks no element types, changes no raw-storage initialization bits, and invokes no copy, move or cleanup hooks. Nonempty ranges must lie within live allocations. Both complete copy ranges are validated before writing, and overlapping copies are rejected. Zero lengths allow null pointers. Bare pointers contain no allocation generation; callers must manage identity when freed addresses are reused.

These types and operations belong to the implicit `std::lang` core; namespace qualification is optional. `memcpy`/`memset` lower directly from the checked AST to LLVM memory intrinsics, avoiding language wrappers and generic native dispatch. Necessary dynamic raw-storage checks use a scalar kernel, small pointer wrappers are preferred for inlining, and bare dereferences directly extract the address.

Byte operations suit byte buffers and data the caller has established can be handled bitwise. Resource initialization, transfer and cleanup belong to operations such as `initAt`, `takeAt` and `dropAt`; copying handle bytes does not establish independent ownership. The bare-pointer String experiment uses bulk copy/fill. The earlier checked-slot implementation is retained as `bootstrap_string_checked.chl`, and reports distinguish the actual sources measured.

Experimental implementations live in `benchmarks/perf/raw_containers.chl` and `benchmarks/perf/bootstrap_string.chl`. List deletion cleans elements and retains capacity; Map owns key/value/state slots, and Set owns an independent key array. The String experiment stores UTF-8 bytes and implements capacity, append, byte access, naive search and explicit independent copying. It does not implement the full standard String Unicode API. Resource-owning types do not implement `Copy<T>`.

## Pointer views and IEEE754 bit patterns

Within an `unsafe` scope, `BarePtr<T>.castTo<U>()` returns a `BarePtr<U>` at the same address. It changes the element view without numerical conversion, allocation, or a target-type check. The caller must ensure the required range, alignment, initialization, and lifetime. Cast to `uint8` for byte-wise arithmetic and dereferencing; pointer offsets use the resulting element layout.

IEEE754 codecs use this method to reinterpret and dereference storage rather than obtaining bit patterns through numerical integer/float casts. Both `float32` and `float64` occupy eight-byte floating-point slots in the language. The four-byte external binary32 representation is therefore reconstructed from the slot's binary64 bit pattern, rather than reading its first four bytes as binary32. AOT lowers the supported conversions directly to LLVM `bitcast` and floating-point width conversions, and inlines pointer-view wrappers. Codec arguments explicitly select byte order.

## Default self-hosted standard library

Default List, Map and Set growth, lookup, insertion, deletion, copying and clearing live in `source/std/lang.chl`. List removal and clearing retain capacity. Map combines dense key/value entries, occupied-slot encodings and cached HashKey values; Set owns independent slots. Existing `Equal`/`Hash` contracts and opaque public storage representations preserve host interoperability.

Linked lists, stacks, queues, deques, ring buffers, binary search trees, red-black trees, general trees, min-heaps, graphs and union-find use ChaLi implementations in their standard modules. List traversal, function-value callbacks, folds, binary search, extremes and stable index-merge sorting also run in the language. Algorithm leases retain the source and prevent structural movement during callbacks. String ownership, search, comparison, classification, UTF-8 decoding, slicing, transformations, splitting, joining and byte conversion, together with IEEE754 codecs and KMP, are implemented in ChaLi.

Remaining host entry points cover these boundaries rather than the migrated container and text traversal algorithms:

- Allocation, release, address/length layout access, liveness and range checks, and raw-storage initialization state.
- Opaque single-slot reservation, swapping, reads, owned-value marshalling, key comparison/Hash callbacks and iterator leases, preserving existing host Codecs and record ABI.
- I/O, numeric display formatting, operating-system services, reflection, synchronous callbacks and C ABI. Big-number and CAS support retain their existing runtimes.

Existing resource-copy restrictions on safe containers remain in force. Experimental raw-storage containers separately verify moves and exactly-once cleanup of `Closeable` elements. The experimental byte string remains separate; performance reports distinguish the actual old native implementation, the current default self-hosted implementation and the experiment.

## Imports and exports

```chali
#abi("C", "./native.dll", "calc")
int32 nativeCalc(int32 x);

#export("calc")
int32 calc(int32 x) { return (int32)(x + (int32)1); }
```

Imports are declarations without bodies. The arguments specify calling convention, library path and native symbol; currently the convention is `"C"`. The former `abi "library" { ... }` syntax has been removed. `#export("symbol")` selects the native symbol; `chali aot FILE --shared --out OUTPUT.dll` selects the output library. Export names must be valid, unique C identifiers outside runtime-reserved prefixes.

Imports retain synchronous callback and return-ownership adapters; release functions and array-length contracts are covered in [Host integration](08-host-integration.md). Imported function values use adapters with the language calling convention. The bridge performs the actual C call, without casting a runtime function value to a machine function pointer.

`#export` supports scalars and resource-free value structs. Generated C headers describe the platform C ABI, and the C++ compiler adapts by-value struct arguments and returns. String, containers, closures, references and structs requiring cleanup cannot cross this value-only export boundary directly. Hosts use the generated header and follow the `chali_init`/`chali_finish` lifecycle.

## Demand-driven AOT linking

AOT selects standard-library host modules from reachable native calls. The linker then collects unused function sections within selected modules. An unused source import does not force all its host code into the executable. Exports, reflection, callbacks and initialization/cleanup retain their reachability roots.

The external LLVM path defaults to O3 and full LTO, eliminating unreachable LLVM definitions during linking before section collection and symbol stripping. The static LLVM path is configured for O3, global dead-code elimination and LTO; release reports identify the platforms actually validated. `CHALI_AOT_FULL_STDLIB=1` forces selection of every standard host module for same-version comparisons.

On MinGW, the CAS 64 MiB zero-initialized backing pool occupies BSS instead of embedding the entire zero buffer in the file. Runtime pool capacity remains unchanged. File size and runtime memory use are separate measurements.

The AOT data image retains only data referenced by `addr` in reachable functions and records needed by function values, removing unreachable initializers and layout entries. Startup zeroes the logical image and copies nonzero segments; identical payloads share file storage. Binary layouts use an explicit byte length, supporting empty tables and embedded zero bytes. Logical addresses, object identity, array length headers and the heap boundary remain unchanged, so a smaller file does not imply a smaller runtime data area. `CHALI_AOT_KEEP_DATA=1` retains the complete image and layout for debugging comparisons.

## C frames for host entries

The standard library's direct C entries use `source/abi/chali_abi.h`, a separate boundary from external signatures declared with `#abi`. A `ChaliAbiFrame` contains 64-bit argument slots and a separate kind array. Integer slots also carry checked language handles; floating slots hold double bit patterns. A handle is not a dereferenceable host pointer.

A module calls `chali_abi_local_info(requiredCapabilities)` using its own compiler, and the host checks that descriptor with `chali_abi_check`. It records the ABI version, target architecture and platform, pointer width, byte order, C calling convention, and frame/record sizes, alignments and field offsets. Different versions, architectures/platforms, target layouts, packing or unsupported required capabilities are rejected. Copying the host descriptor does not describe a module's actual layout. Compatibility does not establish AOT execution support for a cross target.

`chali_abi_check_module` validates a `ChaliAbiModuleDescriptor` before registration, including missing names/functions, invalid arities and duplicate names. It does not register anything. `chali_abi_check_frame` validates the argument count, arrays and slot kinds before a call; `chali_abi_check_result` checks the returned kind. The native direct-entry path performs these checks as well. Callers must still supply accessible, sufficiently large memory: these functions cannot prove arbitrary pointers are readable.

`ChaliAbiError` uses a caller-owned buffer without allocating or transferring ownership. `status` holds a stable error code; `required` includes the terminating zero byte, or is zero on success. Truncated output remains terminated, and zero capacity supports a length query. Protocol diagnostics use fixed text independent of locale. Run `python tests/chali_c_api_contract.py` for independently compiled C11/C++17 interoperability and rejection of incompatible packing.

A language failure inside a synchronous C callback returns to the checked invoker. Its trampoline stores the failure and returns a zero value to C. After the C function returns, the adapter releases its owned return pointer before reporting the original failure; a C++ exception does not escape through the user's C function. Callback pointers are valid only for that synchronous call and cannot be retained for asynchronous invocation. A panic outside a callback still terminates the process.

## Reproducing experiments

```text
python tests/chali_unsafe_storage.py --aot
python tests/chali_abi_directives.py
python benchmarks/perf/bootstrap_containers.py --raw-storage --aot-only --operations 1024 --runs 3 --out tmp/raw-containers.json
python benchmarks/perf/bootstrap_string.py --size 512 --runs 3 --out tmp/string-comparison.json
```

String reports retain compiler/source SHA256 hashes, correctness results, raw timings and checksums. The three sides are the old native String, the current default self-hosted String and an experimental raw byte string. Conversion into experimental storage through a byte list is a separate workload. Timed workloads include their construction, allocation and cleanup; compilation is excluded from internal timings. Interpret comparisons alongside the algorithms used by each workload.

## Current primitive lowering

| Operation | LLVM path | Types and retained rules |
| --- | --- | --- |
| `memcpy` / `memset` | `llvm.memcpy` / `llvm.memset` after range preparation | Byte counts; validate complete ranges before writing, reject overlapping copies, permit null for zero length. |
| Logical addresses and bare-pointer arithmetic | Address values and integer arithmetic, with small wrappers inlined by O3 | Element strides use language layout; logical addresses are not machine pointers. |
| Scalar raw-slot access | LLVM loads/stores or C-signature scalar primitives for supported layouts | bool, char, signed/unsigned integer widths and float32/64; preserve narrowing, signs, negative zero and floating-point bits. |
| `rawSize<T>()` | Language-layout constant in each generic instance | One byte for uint8/char, eight-byte ordinary value slots; aggregate language layout excludes dynamic allocations. |
| `initAt` / `readAt` / `takeAt` / `dropAt` | Fixed semantic lowering plus initialization-state and cleanup kernels | Owned values follow their type protocols; bit copies do not establish ownership. |
| C entry results | Return checked C-frame bits and result kind directly | Errors, memory faults, step limits and exit requests are checked first; scalar results avoid generic-value conversion. |

Complex layouts, object lookup, resource protocols, callbacks and stale-access diagnostics retain their C runtime services. Checked and unchecked keep their documented array/pointer settings; static borrowing and ownership cleanup remain in force. The C++ backend generates production LLVM IR. Python tests invoke the compiler and verify results, IR and target files.


## Directly linked C ABI

Use `@link` for fixed scalar C signatures supplied by a linked object or archive.

The compiler preserves scalar widths and uses the target C calling convention. Use a compatible dynamic library for structures, references, buffers and synchronous callbacks. The existing marshalling rules above describe those signatures and ownership.

Run `python tests/chali_link_abi.py --chali PATH` to check scalar calls, target declarations, rejection of complex direct-link signatures and exports to a C host.
