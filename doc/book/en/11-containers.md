# Chapter 11 Collections

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../11-容器.md)
<!-- END LANGUAGE NAVIGATION -->

Arrays, List, Map and Set serve fixed positions, variable sequences, keyed lookup and uniqueness. Choose by access pattern before examining capacity and traversal cost.

## 11.1 Fixed arrays and nested layout

Arrays have element types and bounds. Access multidimensional arrays within every dimension. Language arrays, nested field layout and C ABI contiguous buffers have distinct contracts. Checked execution includes runtime checks; unchecked defaults do not remove static typing or borrowing.

Initialize a two-dimensional array, then copy it and modify the copy. Each index has its own dimensional bound.

<!-- verify-book -->
```chali
void main()
{
    int64 matrix[2][3];
    for (int64 row in range(0, 2))
    {
        for (int64 column in range(0, 3))
        {
            matrix[row][column] = row * 10 + column;
        }
    }
    println("{} {}", matrix[0][2], matrix[1][2]);
    int64 copied[2][3] = matrix;
    copied[1][2] = 99;
    println("{} {}", matrix[1][2], copied[1][2]);
}
```

```text
2 12
12 99
```

## 11.2 Choosing List, Map and Set

List preserves sequence positions, Map associates keys with values, and Set keeps unique keys. Map and Set keys need hashing and equality. Any accepts different static types but requires recovery or matching on read. Prefer concrete generic arguments when known.

<!-- verify-book -->
```chali
void main()
{
    List<Any> values = [1, "two", true];
    values.push(1.5);
    Any text = values[1];
    println("{} {}", values.len(), tryGet<String>(&text).unwrap());

    Set<Any> unique;
    Any key = "key";
    unique.add(key);
    unique.add(key);
    unique.add(2);
    println("{}", unique.len());

    Map<Any, Any> mixed;
    mixed.put(key, "stored");
    mixed.put(3, true);
    Any saved = mixed[key];
    println("{} {}", mixed.len(), tryGet<String>(&saved).unwrap());

    Map<String, Any> record = {"count": 3, "name": "ChaLi"};
    Any count = record["count"];
    println("{}", tryGet<int64>(&count).unwrap());
    saved.clear();
    println("{}", saved.isEmpty());
}
```

```text
4 two
2
2 stored
3
1
```

## 11.3 Capacity, length and cleanup

Length counts live elements; capacity describes allocated space. Growth must check len + need and allocation bounds, then grow geometrically when needed. Removal, clear and truncation retain buffer capacity while cleaning removed owned elements. Reallocation can reclaim the old block; drop releases the final buffer.

Element size, metadata, indexes, alignment and allocator limits also bound capacity. An integer maximum is not an allocatable element count for every type.

## 11.4 Borrowing and traversal

Avoid structural changes that may move a buffer while element references are live. Copying obtains a value, moving transfers ownership, and iteration obtains successive elements through its protocol. Chapter 17 demonstrates iterator consumption and transformations.

## 11.5 Exercises

1. Choose collections for scores, student-ID lookup and unique subjects.
2. What happens to capacity and resources after clear?
3. Why must key hashing agree with equality?
4. List checks before growing a collection.

### Answers and checks

1. Usually List, Map and Set, subject to actual access patterns.
2. Retain capacity and clean removed owned elements.
3. Equal keys must follow consistent indexing rules.
4. Check addition and growth overflow, limits and allocation failure.

Next: [Chapter 12](12-strings-text.md).
