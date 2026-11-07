# Chapter 13 Collections

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../13-容器.md)
<!-- END LANGUAGE NAVIGATION -->

Choose containers by access patterns: fixed positions, growing sequences, key lookup or duplicate detection. Arrays, List, Map and Set provide distinct structures and contracts rather than interchangeable syntax.

A matrix illustrates boundaries and copying. Any containers then distinguish heterogeneous contents from unchecked extraction. Length, capacity and mutation during iteration prepare the iterator discussion in Chapter 20.

## 13.1 Track list length across mutations

<!-- verify-book -->
```chali
void main()
{
    List<int64> scores;
    scores.push(30);
    scores.push(10);
    scores.push(20);

    println("len={} at1={} has10={} idx={}",
        scores.len(), scores.at(1), scores.has(10), scores.indexOf(20));

    scores.insert(0, 7);
    println("insert 后：{} len={}", scores[0], scores.len());

    int64 last = scores.pop();
    println("pop={} len={}", last, scores.len());

    scores.remove(0);
    println("remove(0) 后：{} len={}", scores[0], scores.len());

    scores.clear();
    println("clear 后：len={}", scores.len());
}
```

```text
len=3 at1=10 has10=1 idx=2
insert 后：7 len=4
pop=20 len=3
remove(0) 后：30 len=2
clear 后：len=0
```

Push creates [30,10,20]. at(1) selects the second element, has finds 10, and indexOf returns 2 for 20. Inserting 7 at the front grows the length to four. pop removes 20, remove(0) then removes 7, and clear leaves length zero. Clearing elements differs from releasing the final buffer.

## 13.2 Fixed arrays and nested layout

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

Matrix has two rows and three columns, both indexed from zero. The outer range chooses rows and the inner range columns. `row * 10 + column` makes positions visible: the rows contain 0,1,2 and 10,11,12. The first output is therefore two and twelve.

Copied receives a value copy of the same-shaped array, then its bottom-right entry becomes 99. Printing twelve from the original and 99 from the copy demonstrates independent mutation for multidimensional arrays. Dimensions bound legal indices; `[2][3]` is not the final element. That position is `[1][2]`.


Local runtime-length arrays accept brackets on either side: `T[] a = new[n];` and `T a[] = new[n];` are equivalent. Parameters also accept both `T[] a` and `T a[]`. Local owning arrays require initialization; copies and value parameters own independent elements. Borrow with `T[] const&` or `T[] mut&`. This example passes both local forms to both parameter forms and changes the copy independently:

<!-- verify-book -->
```chali
int64 firstType(int64[] values)
{
    return values[0];
}
int64 firstName(int64 values[])
{
    return values[0];
}
void main()
{
    int64[] original = new[2];
    original[0] = 7;
    int64 copied[] = original;
    copied[0] = 9;
    println("{} {}", firstName(original), firstType(copied));
}
```

```text
7 9
```

Read tuple elements with zero-based constant subscripts. Tuples inside arrays also use consecutive brackets: `pairs[0][0]` selects the first array element, then its first tuple element. Tuple elements are read-only; assign a complete tuple to update the value.

<!-- verify-book -->
```chali
void main()
{
    (int64, String)[] pairs = new[1];
    pairs[0] = (3, "tuple");
    println("{} {}", pairs[0][0], pairs[0][1]);
}
```

```text
3 tuple
```

## 13.3 Choosing List, Map and Set

List preserves sequence positions, Map associates keys with values, and Set keeps unique keys. Map and Set keys need hashing and equality. Any accepts different static types but requires recovery or matching on read. Prefer concrete generic arguments when known.

<!-- verify-book -->
```chali
void main()
{
    List<Any> values = [1, "two", true];
    values.push(1.5);
    Any text = values[1];
    match (tryGet<String>(&text)) {
        Value(value) => { println("{} {}", values.len(), value); }
        None => { println("类型不匹配"); }
    }

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
    match (tryGet<String>(&saved)) {
        Value(value) => { println("{} {}", mixed.len(), value); }
        None => { println("类型不匹配"); }
    }

    Map<String, Any> record = {"count": 3, "name": "ChaLi"};
    Any count = record["count"];
    match (tryGet<int64>(&count)) {
        Value(value) => { println("{}", value); }
        None => { println("类型不匹配"); }
    }
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

List<Any> stores differing concrete types, but retrieval still returns Any and requires a typed query such as `tryGet<String>`. Three initial entries plus 1.5 make length four. Entry one contains a string, so match selects Value and reads two. A mismatched query selects None and is handled explicitly.

Set removes the duplicate string key, so two identical additions plus an integer yield length two. The mixed Map has two key-value entries and returns stored for key. Record fixes its key type to String while retaining Any values, so count is extracted as int64. Finally clear empties only the local saved dynamic value, making isEmpty true; it does not clear the entire mixed map.


## 13.4 Capacity, length and cleanup

Length counts live elements; capacity describes allocated space. Growth must check len + need and allocation bounds, then grow geometrically when needed. Removal, clear and truncation retain buffer capacity while cleaning removed owned elements. Reallocation can reclaim the old block; drop releases the final buffer.

Element size, metadata, indexes, alignment and allocator limits also bound capacity. An integer maximum is not an allocatable element count for every type.

## 13.5 Borrowing and traversal

Avoid structural changes that may move a buffer while element references are live. Copying obtains a value, moving transfers ownership, and iteration obtains successive elements through its protocol. Chapter 20 demonstrates iterator consumption and transformations.

### Check whether a key exists

Handle present and missing keys explicitly before using their values.

<!-- verify-book -->
```chali
void main() {
    Map<String, int64> scores;
    scores.put("Ada", 42);
    println("{} {}", scores.getOption("Ada").isValue(), scores.getOption("Lin").isNone());
}
```

```text
1 1
```

### Summary

Length counts valid elements, capacity describes available storage and an index identifies an access position. Maps and sets additionally depend on equality and hashing, while Any requires confirmation during extraction.

Container copies follow value semantics, iterators retain traversal state and owned elements have cleanup obligations. Keeping these layers separate explains retained data with an exhausted iterator, or unchanged capacity after an element has been removed.

## 13.6 Copying a Struct Containing a List

<!-- verify-book -->
```chali
struct Bag
{
    List<int64> items;
}

void main()
{
    Bag original;
    original.items.push(1);
    Bag copy = original;
    copy.items.push(2);
    println("{} {}", original.items.len(), copy.items.len());
}
```

```text
1 2
```

The example creates a Bag, fills its list and copies the aggregate. Updating the copied list preserves the original owned contents. Compare the simple Student from Chapter 7: adding an owned collection requires value semantics throughout the fields, not a shared writable list address.

Next: [Chapter 14](14-error-handling.md).
