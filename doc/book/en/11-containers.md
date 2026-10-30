# Chapter 11 Collections

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../11-容器.md)
<!-- END LANGUAGE NAVIGATION -->

Choose containers by access patterns: fixed positions, growing sequences, key lookup or duplicate detection. Arrays, List, Map and Set provide distinct structures and contracts rather than interchangeable syntax.

A matrix illustrates boundaries and copying. Any containers then distinguish heterogeneous contents from unchecked extraction. Length, capacity and mutation during iteration prepare the iterator discussion in Chapter 17.

## 11.1 Fixed arrays and nested layout

Some data has a fixed shape, such as a two-by-three table. Arrays record shape alongside element type, and each index selects one level. Identifying rows and columns first makes boundaries clearer than treating the data as an arbitrary growing list.

The example encodes each position numerically and copies the table. Language array layout is not itself a C ABI buffer contract, and checked runtime access has a different role from static type and borrow checking.

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

### Code walkthrough

Matrix has two rows and three columns, both indexed from zero. The outer range chooses rows and the inner range columns. `row * 10 + column` makes positions visible: the rows contain 0,1,2 and 10,11,12. The first output is therefore two and twelve.

Copied receives a value copy of the same-shaped array, then its bottom-right entry becomes 99. Printing twelve from the original and 99 from the copy demonstrates independent mutation for multidimensional arrays. Dimensions bound legal indices; `[2][3]` is not the final element. That position is `[1][2]`.


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

### Code walkthrough

List<Any> stores differing concrete types, but retrieval still returns Any and requires a typed query such as `tryGet<String>`. Three initial entries plus 1.5 make length four. Entry one really contains a string, allowing this example to unwrap two. A mismatched query requires handling failure instead.

Set removes the duplicate string key, so two identical additions plus an integer yield length two. The mixed Map has two key-value entries and returns stored for key. Record fixes its key type to String while retaining Any values, so count is extracted as int64. Finally clear empties only the local saved dynamic value, making isEmpty true; it does not clear the entire mixed map.


## 11.3 Capacity, length and cleanup

Length counts live elements; capacity describes allocated space. Growth must check len + need and allocation bounds, then grow geometrically when needed. Removal, clear and truncation retain buffer capacity while cleaning removed owned elements. Reallocation can reclaim the old block; drop releases the final buffer.

Element size, metadata, indexes, alignment and allocator limits also bound capacity. An integer maximum is not an allocatable element count for every type.

## 11.4 Borrowing and traversal

Avoid structural changes that may move a buffer while element references are live. Copying obtains a value, moving transfers ownership, and iteration obtains successive elements through its protocol. Chapter 17 demonstrates iterator consumption and transformations.

### Summary

Length counts valid elements, capacity describes available storage and an index identifies an access position. Maps and sets additionally depend on equality and hashing, while Any requires confirmation during extraction.

Container copies follow value semantics, iterators retain traversal state and owned elements have cleanup obligations. Keeping these layers separate explains retained data with an exhausted iterator, or unchanged capacity after an element has been removed.

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
