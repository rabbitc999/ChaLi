# Chapter 11 Containers

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../11-容器.md)
<!-- END LANGUAGE NAVIGATION -->

Choose a container from the operations the program needs. `List<T>` stores an ordered growable sequence. `Map<K, V>` associates keys with values. `Set<T>` stores distinct keys. Their default implementations have different lookup, insertion and allocation behavior.

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

Exercise: count repeated words in a map and collect unique words in a set. Verify an empty input and repeated insertion. The [performance report](../../en/12-vm-performance.md) compares actual VM, AOT, C++, Python and Lua workloads. Next: [strings](12-strings-text.md).
