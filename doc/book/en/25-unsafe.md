# Chapter 25 Unsafe code and raw storage

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../25-不安全代码.md)
<!-- END LANGUAGE NAVIGATION -->

Containers and host adapters must distinguish reserved storage from constructed values. Confusing capacity, initialization, address strides and cleanup can mistake byte writes for construction or destroy extracted values twice.

Follow unsafe call contexts, RawStorage slot states, BarePtr arithmetic and byte versus value copying. Every example identifies its premises so the low-level operations can eventually sit behind a safe external interface.

## 25.1 An address is not automatically an object

An address can be represented numerically, but a number alone does not prove a readable object exists there. Correct arithmetic still leaves allocation, range, layout and alignment obligations.

Begin with an unsafe function that only adds integers to demonstrate call context. Address queries such as std::lang::address must also be read through their contracts. Nothing here dereferences the result; later examples introduce allocation-backed storage and pointers gradually.

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

AddOffset is declared unsafe, requiring an unsafe calling context. Main supplies that context and passes integers 100 and eight. The body only adds them, so it prints 108.

Nothing dereferences 108 or establishes that address 100 belongs to a valid allocation. The arithmetic demonstrates call permission rather than safe real-address access. Unsafe assigns extra obligations to the author rather than disabling every type, boundary or lifetime rule in the program.


## 25.2 Raw capacity and initialization state

RawStorage<T> allocates capacity without constructing every element. initAt initializes an empty slot, takeAt moves a value out and leaves it uninitialized, and dropAt cleans a live value while retaining capacity. Do not destroy an owned value twice.

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

Allocate(4) reserves capacity for four String slots without constructing four strings. InitAt constructs hello in slot zero. TakeAt transfers that owned value into result and marks the slot uninitialized again, which the assertion verifies.

Taking a value does not reduce capacity, so the output is hello and four. Closing slots releases the allocation without destroying the moved-out string again; result has its own owning lifetime. Repeating takeAt or reading before reinitialization violates the state contract. Track capacity, initialized elements and extracted values separately.


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

P forms a BarePtr<int64> view from an allocation-aware pointer. Adding two advances by elements rather than two bytes. The int64 stride here is eight bytes, so the address difference is sixteen. The second assertion verifies the corresponding integer-left addition.

Subtracting one leaves q one element and eight bytes beyond p, producing `1 / 8`. No uninitialized integer is read; this is address arithmetic only. BarePtr does not own storage or extend slots' lifetime, and its addresses cannot remain valid access sources after close.


## 25.3 Copy bytes versus copying values

Memory primitives follow their byte-level contracts and cannot generally replace owned-value copying. Copying integer bytes differs from copying String ownership metadata, which can corrupt ownership or cleanup state.

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

Source reserves one int64 slot and target eight uint8 slots. Memset uses 42 as a per-byte fill and eight as a byte count; it does not construct an integer with numeric value 42. Memcpy copies those eight bytes, so reading the first and last uint8 yields 42.

Writing bytes does not establish source's typed initialization state, which the assertion confirms remains false. This distinguishes raw memory writes from constructing T. The example concerns integer/byte views, not owned objects such as String. Both storage allocations still require closing at the end.


## 25.4 Encapsulation and invariants

Validate counts and bounds at the safe entry, maintain initialized regions internally and clean only live elements on exit. rawSize describes its raw-storage query; it does not establish host ABI layout, every alignment requirement or a serialization format. See [unsafe storage and ABI](../../en/41-unsafe-abi.md).

### Summary

Track allocation range, element layout, slot state and borrows. InitAt constructs, takeAt extracts, dropAt destroys an element and close releases storage. Raw byte operations do not automatically maintain those typed states.

An address alone does not prove valid typed access. Safe entry points should check what they can, internal code must preserve remaining invariants and verification should cover failure and exit paths. AOT delivery next checks the resulting program together with its environment.

## 25.5 Exercises

1. Can readAt directly follow takeAt on the same slot?
2. Does dropAt free the entire buffer?
3. Why cannot memcpy replace every copy?
4. Which state should a raw-storage wrapper track?

### Answers and checks

1. No; initialize the slot again first.
2. No; it cleans the element and retains capacity.
3. Owned values carry ownership and cleanup semantics.
4. Capacity, initialized regions, live elements and borrow lifetimes.

Next: [Chapter 27](27-aot-export.md).
