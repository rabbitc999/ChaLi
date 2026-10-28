# Chapter 23 Unsafe code

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../23-不安全代码.md)
<!-- END LANGUAGE NAVIGATION -->

Ordinary values, references and standard containers let the compiler check types, borrowing and cleanup relationships. Implementing an allocator, container or host adapter can also require working with addresses and capacity that does not yet contain constructed elements. This chapter introduces those operations and the obligations that accompany them, before the student-system project.

The complete API reference is [Raw storage and C ABI directives](../../en/41-unsafe-abi.md).

`RawStorage`, `RawPtr`, `BarePtr`, `memcpy` and `memset` belong to the implicitly available `std::lang` core. Examples omit `std::lang::` and require no extra import for these names.

## 23.1 Unsafe scopes and functions

`unsafe { ... }` grants permission for unsafe operations within a scope. An `unsafe` function requires an unsafe calling context, and its body has the same permission.

<!-- verify-book -->
```chali
unsafe int64 addOffset(int64 address, int64 bytes) {
    return address + bytes;
}

void main() {
    unsafe {
        println("{}", addOffset(100, 8));
    }
}
```

```text
108
```

Moving this call outside the block is rejected during compilation. A closure establishes its own function permission scope; creating it inside an unsafe block does not grant permission to its body. Unsafe function values cannot implicitly become ordinary function values.

Ordinary array, arithmetic and allocation-lifetime checks keep their existing behavior. The keyword permits operations whose extra preconditions must be established by the programmer.

## 23.2 Capacity and constructed elements

A container with capacity 100 and length 3 needs three constructed elements. Its other 97 slots are spare space. Default-constructing all spare resource elements can change both behavior and cost.

`RawStorage<T>` owns such capacity and records layout and per-slot initialization state. `RawPtr<T>` stores allocation identity and an element index without owning the element or extending its lifetime.

| Operation | Before | After |
| --- | --- | --- |
| `initAt` | Uninitialized destination | Destination owns the supplied value |
| `readAt` | Initialized source | Independent value copy; source remains live |
| `borrowAt` | Initialized source | Borrowed reference; ownership unchanged |
| `takeAt` | Initialized source | Returned owned value; source uninitialized |
| `dropAt` | Initialized source | Element cleaned; capacity retained |
| `close` | Any valid initialization state | Live elements cleaned and allocation released |

<!-- verify-book -->
```chali

void main() {
    RawStorage<String> slots = RawStorage<String>::allocate(4);
    unsafe {
        initAt<String>(slots.pointer(0), "hello");
        String result = takeAt<String>(slots.pointer(0));
        assert(!slots.isInitialized(0));
        println("{} / {}", result, slots.capacity());
    }
    slots.close();
}
```

```text
hello / 4
```

Double initialization, uninitialized reads, out-of-range accesses and accesses after release report errors. A pointer at `capacity` is a valid one-past position, but cannot be dereferenced. Explicit generic arguments also make the intended element type clear.

## 23.3 Copying, moving and cleanup

`readAt` preserves ordinary ChaLi value semantics. An unsafe scope does not turn a String handle into an independent copy. Use `takeAt` to transfer ownership, `initAt` to establish a destination owner, and `dropAt` to discard a live element.

`relocateNonOverlapping<T>(destination, source, count)` validates the complete ranges and slot states before moving elements. Destinations must be uninitialized and the ranges disjoint. Successful moves leave source slots uninitialized. Moving zero elements returns immediately.

A `Closeable` resource cannot also declare `Copy<T>` or bypass `#NonCopyable`. An explicitly named independent-copy method must establish each new resource owner correctly. `close()` cleans initialized slots only; deleting elements can retain capacity without destroying moved-out values again. Borrowed references must stop being used before move-out, cleanup or release.

## 23.4 Bare pointers contain addresses

`BarePtr<T>` stores a ChaLi logical byte address without a runtime type tag, initialization bitmap or owner. Its static type determines layout and arithmetic stride.

| Representation | Information | Typical use |
| --- | --- | --- |
| Ordinary reference | Type and language borrowing relationship | Everyday value access |
| `RawPtr<T>` | Allocation identity and slot index | Checked construction, reading, move-out and cleanup |
| `BarePtr<T>` | Byte address | Low-level kernels without runtime type checks |

`barePointer<T>` obtains an address from raw storage, including uninitialized positions. `pointerFromAddress<T>` interprets a numeric address as a pointer. `dereference<T>` interprets an address as `T mut&`. All three require `unsafe`; the caller must establish layout, alignment, initialization, lifetime and ownership correctness.

Logical addresses are managed by ChaLi and cannot be passed directly as machine C pointers. Use the adapters from [chapter 21](21-c-interop.md) for host interoperability.

## 23.5 Pointer arithmetic uses element strides

`p.castTo<U>()` requires `unsafe` and changes a `BarePtr<T>` into a `BarePtr<U>` view at the same address. Cast to `uint8` for byte-wise dereferencing and one-byte offsets. The cast neither checks the target type nor allocates or copies storage. The caller must ensure layout, alignment, initialization, lifetime and access ranges.

IEEE754 bit-pattern codecs also use `castTo` and dereferencing. Both `float32` and `float64` occupy eight-byte floating-point slots in ChaLi. The external four-byte binary32 format must be reconstructed from the slot representation rather than reading its first four bytes. AOT lowers supported conversions to LLVM bitcasts and floating-point width conversions, and eliminates pointer-view wrappers during optimization. The codecs preserve the specified handling of signed zero, subnormals, infinities and NaNs.

For `BarePtr<int64>`, `p + 1` advances eight bytes. For `BarePtr<uint8>`, it advances one byte.

<!-- verify-book -->
```chali

void main() {
    RawStorage<int64> slots = RawStorage<int64>::allocate(3);
    unsafe {
        BarePtr<int64> p = barePointer<int64>(slots.pointer(0));
        BarePtr<int64> q = p + 2;
        assert(q.address - p.address == 16);
        assert((2 + p).address == q.address);
        q = q - 1;
        println("{} / {}", q - p, q.address - p.address);
    }
    slots.close();
}
```

```text
1 / 8
```

Supported forms are `p + n`, `n + p`, `p - n` and same-type `p - q`. Pointer differences return integral element distances; the byte difference must be a whole multiple of the stride. Callers should calculate differences within one allocation, a condition two bare addresses cannot prove.

Address arithmetic itself accesses no memory. A computable address, including a one-past position, is not necessarily dereferenceable. Integer overflow and other arithmetic retain ordinary language behavior.

## 23.6 memcpy and memset use byte counts

`memcpy<T,U>(destination, source, bytes)` allows different static source and destination element types. The implementation does not check their types. `memset<T>(destination, value, bytes)` repeats one byte across the destination range.

Their function-like syntax lowers to dedicated AST memory operations, from which AOT directly generates LLVM `memcpy`/`memset` intrinsics. Necessary address and complete-range checks still run before any write.

Lengths count bytes. Filling eight bytes through `BarePtr<int64>` covers one element. Repeating `0x01` across all eight bytes does not store the integer value 1.

<!-- verify-book -->
```chali

void main() {
    RawStorage<int64> source = RawStorage<int64>::allocate(1);
    RawStorage<uint8> target = RawStorage<uint8>::allocate(8);
    unsafe {
        BarePtr<int64> from = barePointer<int64>(source.pointer(0));
        BarePtr<uint8> to = barePointer<uint8>(target.pointer(0));
        memset<int64>(from, (uint8)42, 8);
        memcpy<uint8,int64>(to, from, 8);
        uint8 mut& first = dereference<uint8>(to);
        uint8 mut& last = dereference<uint8>(to + 7);
        println("{} / {}", (int64)*first, (int64)*last);
        assert(!source.isInitialized(0));
    }
    source.close();
    target.close();
}
```

```text
42 / 42
```

Nonempty ranges must lie within live allocations. A copy validates both complete ranges before writing, and overlapping ranges are rejected. Zero lengths allow null pointers. Moving overlapping container regions requires a separate design for direction and ownership transfer.

## 23.7 Writing bytes does not construct resource values

The example writes bytes but leaves `source.isInitialized(0)` false. Bare byte operations do not read or change slot initialization bits and invoke no copy, move or cleanup hooks.

This permits whole-buffer byte copies. Copying String or file-handle bits does not establish another owner; independently cleaning both copies could release the same resource twice. Overwriting a live resource with zero bytes also fails to clean its old value. Use the ownership-aware operations for resource initialization, transfer and destruction.

Bare pointers contain no allocation generation. Stop using them when their allocation is released; an address becoming accessible later does not establish that it still identifies the original object.

## 23.8 Encapsulating unsafe operations

A byte buffer can expose checked `pushByte`, `getByte` and `reserve` methods while keeping address operations in small internal unsafe blocks. Its callers use the buffer without managing address/capacity relationships.

Growth allocates new capacity, copies live contents, releases the old allocation after success, and then updates the pointer and capacity. Resource elements additionally need correct initialization-state and ownership transitions. Do not release the source before reading it or publish a destination before the transfer succeeds.

The complete [bootstrap String experiment](../../../benchmarks/perf/bootstrap_string.chl) implements owned byte storage, growth, append, access, search and explicit independent copying. It preserves Chinese text and emoji as UTF-8 bytes. It does not implement the entire standard String Unicode API or automatically establish code-point boundaries for byte indices.

Write down invariants such as `0 <= length <= capacity`, a pointer into the currently owned allocation, and valid ranges for every dereference. Each unsafe block should have a clear explanation of how those conditions are satisfied.

## 23.9 Correctness and measurement

Test empty capacity, boundaries, double initialization, reads after move-out, growth, deletion with retained capacity, released allocations and exactly-once resource cleanup. Byte operations also need zero-length, cross-type, overlap, full-range and fill-byte tests.

Separate compilation from execution. Compare identical inputs with independently checked outputs. Record construction, incremental append, reading, search, copying and conversion from native String separately. Bulk operations can remove per-byte calls without guaranteeing that every workload becomes faster.

The [String comparison report](../../../benchmarks/results/string-1.0.7/README.md) retains actual source and compiler hashes. The earlier checked-slot implementation is preserved as `bootstrap_string_checked.chl`; its measurements must not be relabeled as the bare-pointer implementation. Record results per compiler version and target separately.

## 23.10 Summary

| Tool | Main caller obligation |
| --- | --- |
| Unsafe scope or function | Establish the interface's extra preconditions |
| `RawStorage<T>` | Maintain initialization and ownership transitions |
| `BarePtr<T>` | Maintain address, layout, alignment, lifetime and valid ranges |
| Pointer arithmetic | Distinguish element stride from byte distance |
| `memcpy` and `memset` | Supply correct byte counts and ownership-safe, disjoint ranges |
| Safe abstraction | Maintain length, capacity and cleanup invariants internally |

Use ordinary references and standard containers in everyday application code. Storage kernels benefit from small, independently testable unsafe interfaces. Continue to [chapter 24, LLVM AOT export](24-aot-export.md), followed by the student information system.

## 23.11 Exercises

1. Move the first unsafe call outside its block, then create an ordinary closure containing that call inside the block. Inspect both diagnostics.
2. Read a raw String slot after `takeAt`. Explain why leftover bytes do not represent an owned String.
3. Change the pointer-arithmetic example to `uint8`. Predict element and byte distances.
4. Fill an `int64` range with byte 1. Explain why its integer value differs from 1.
5. Test partially overlapping copies and zero-length copies with null pointers.
6. Add self-append to a byte buffer and test both spare-capacity and growing cases, independent copies and final cleanup.

### Reference answers and acceptance criteria

1. Both ordinary function contexts lack permission; closures do not inherit creation-site unsafe scope.
2. Ownership has transferred and the source slot is uninitialized. A checked read must fail.
3. Advancing two elements advances two bytes; subtracting one element leaves both distances equal to one.
4. Every byte becomes `0x01`, a different full-width bit pattern from integer 1.
5. Nonempty overlap is rejected. Zero-length operations access no bytes and allow null pointers.
6. Retain the original length, use valid updated pointers after growth, and publish new storage only after the transfer succeeds. Check both growth paths and ownership independently.

Next: [LLVM AOT export](24-aot-export.md).
