# Raw storage and C ABI directives

**English** · [简体中文](../44-unsafe-abi.md)

## Unsafe scopes and storage

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

These types and operations belong to the implicit `std::lang` core; namespace qualification is optional. `memcpy`/`memset` produce dedicated quads and VM instructions, with LLVM intrinsics in AOT, avoiding language wrappers and generic native dispatch. Necessary dynamic raw-storage checks use a scalar kernel, small pointer wrappers are preferred for inlining, and bare dereferences directly extract the address.

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

## Reproducing experiments

```text
python tests/chali_unsafe_storage.py --aot
python tests/chali_abi_directives.py
python benchmarks/perf/bootstrap_containers.py --raw-storage --aot-only --operations 1024 --runs 3 --out tmp/raw-containers.json
python benchmarks/perf/bootstrap_string.py --size 512 --runs 3 --out tmp/string-comparison.json
```

String reports retain compiler/source SHA256 hashes, correctness results, raw timings and checksums. The three sides are the old native String, the current default self-hosted String and an experimental raw byte string. Conversion into experimental storage through a byte list is a separate workload. Timed workloads include their construction, allocation and cleanup; compilation is excluded from internal timings. Interpret comparisons alongside the algorithms used by each workload.
