# Chapter 11 Containers

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../11-容器.md)
<!-- END LANGUAGE NAVIGATION -->

Choose a container from the operations the program needs. `List<T>` stores an ordered growable sequence. `Map<K, V>` associates keys with values. `Set<T>` stores distinct keys. Their default implementations have different lookup, insertion and allocation behavior.

Default container growth, lookup, insertion, deletion and copying are implemented in ChaLi. The host supplies single-slot storage, ownership marshalling and safety checks. Map uses occupied slots and cached HashKey values; Set owns independent slots. Ordinary assignment retains independent value semantics.

```chali
void main()
{
    List<int64> values;
    values.push(10);
    values.push(20);
    int64 total = 0;
    for (int64 value in values)
    {
        total = total + value;
    }
    println("{}", total);
}
```

Value copying, borrowing and iteration are separate decisions. Copying a list creates independent values. Mutable iteration intentionally changes the source and must obey the borrow rules. Indexing retains bounds and live-memory checks in the default VM and AOT paths.

Map keys need the required equality and hashing behavior. Do not mutate a key in a way that invalidates its placement. Keep failed lookups distinct from a stored default value by using the API's declared result type.

Exercise: count repeated words in a map and collect unique words in a set. Verify an empty input and repeated insertion. The [performance report](../../en/12-performance.md) compares actual VM, AOT, C++, Python and Lua workloads. Next: [strings](12-strings-text.md).
### Fixed multidimensional and growable arrays

Write `int64 matrix[2, 3];` or `int64 cube[2, 3, 4];`. Chained brackets such as `int64 cube[2][3][4];` and mixed forms are equivalent, including array fields. Extents are positive compile-time integers. Storage is contiguous and row-major. `cube[i,j,k]` and `cube[i][j][k]` evaluate each index once and check every dimension; all indices are required. Assignment and value parameters preserve independent copies.

`T a[] = new[n];` allocates a fixed-length runtime array on the heap. For growable storage, import `std::array` and use `std::array::DynamicArray<T>`, implemented in ChaLi:

```chali
import std::array;
void main() {
    std::array::DynamicArray<int64> values;
    values.reserve(8);
    values.push(7);
    values.resize(3);
    values.set(1, 9);
    assert(values[0] == 7 && values[1] == 9 && values[2] == 0);
    assert(values.len() == 3 && values.capacity() >= 8);
}
```

`reserve(n)` only increases capacity; `push(value)` doubles capacity when full. `resize(n)` appends default values or releases removed values; `clear()` retains capacity. Read with `[]` / `at`, write with `set`. Invalid indices and negative lengths or capacities fail. `pop()` returns `Option<T>`; `toArray()` returns an independent runtime array. Copies own independent elements and buffers, cleaned up at scope exit. Growth replaces storage; do not retain references into the backing buffer. Growth currently follows runtime-array value copying; no speed advantage over builtin List is promised.


### Explicit copying with `Copy<T>`

`Copy<T>` declares `T copy();`, returning an independent value. For example: `struct Packet: Copy<Packet> { String text; Packet copy() { return self; } };`. Returning `self` follows existing value semantics: strings, owning fields and arrays are copied independently; reference fields remain borrowed and retain their lifetime limits. `DynamicArray<T>` implements `Copy<DynamicArray<T>>` and preserves length and capacity in the copy.

Generic code may use `T duplicate<T: Copy<T>>(T const& value) { return value.copy(); }`; interface references can dispatch `copy()` too. An explicit interface declaration is required; a same-named method alone does not satisfy the constraint. This interface does not change assignment or value parameters, is not an implicit-copy eligibility marker, and does not override `#NonCopyable` or `Closeable` restrictions. `Copy<T>` and `#NonCopyable` are mutually exclusive, including inherited interfaces and host `impl` declarations. `Closeable` types receive the noncopyable flag automatically and cannot implement `Copy<T>`, even if the method would return a fresh object. Existing `Clone<T>::clone()` behavior and the supported `#derive` traits remain unchanged.
