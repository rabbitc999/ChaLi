# Chapter 23 Unsafe code and raw storage

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../23-不安全代码.md)
<!-- END LANGUAGE NAVIGATION -->

Unsafe operations assign particular proof obligations to the programmer. Keep them in small functions, establish storage state, bounds, alignment, ownership and cleanup, and expose ordinary typed interfaces.

## 23.1 An address is not automatically an object

std::lang::address obtains an address representation. Address arithmetic needs correct units, offsets, allocations and target types. Nearby numbers do not prove object membership, and runtime checks do not establish every condition for external memory.

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

## 23.2 Raw capacity and initialization state

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

## 23.3 Copy bytes versus copying values

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

## 23.4 Encapsulation and invariants

Validate counts and bounds at the safe entry, maintain initialized regions internally and clean only live elements on exit. rawSize describes its raw-storage query; it does not establish host ABI layout, every alignment requirement or a serialization format. See [unsafe storage and ABI](../../en/41-unsafe-abi.md).

## 23.5 Exercises

1. Can readAt directly follow takeAt on the same slot?
2. Does dropAt free the entire buffer?
3. Why cannot memcpy replace every copy?
4. Which state should a raw-storage wrapper track?

### Answers and checks

1. No; initialize the slot again first.
2. No; it cleans the element and retains capacity.
3. Owned values carry ownership and cleanup semantics.
4. Capacity, initialized regions, live elements and borrow lifetimes.

Next: [Chapter 24](24-aot-export.md).
