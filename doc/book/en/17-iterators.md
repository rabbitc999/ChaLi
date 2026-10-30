# Chapter 17 Iterators

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../17-迭代器.md)
<!-- END LANGUAGE NAVIGATION -->

An iterator abstracts the next item. It supports traversal and combinations of filtering, mapping and terminal operations. Correct reuse depends on consumption state and borrowing.

## 17.1 Consumption and early termination

An iterator may retain its current position. Terminal operations such as all and any consume elements and can short-circuit. A second operation need not restart. Create a fresh iterator or copy its state only where supported.

<!-- verify-book -->
```chali
import std::iter;

void main()
{
    List<int64> xs;
    xs.push(1);
    xs.push(2);
    xs.push(3);
    xs.push(4);
    let first = xs.iter();
    println("第一次 all: {}", first.all(fn(int64 const& x)
    {
        x > 0
    }));
    println("第二次 any: {}", first.any(fn(int64 const& x)
    {
        x > 2
    }));
    let second = xs.iter();
    println("新建后 any: {}", second.any(fn(int64 const& x)
    {
        x > 2
    }));
}
```

```text
第一次 all: 1
第二次 any: 0
新建后 any: 1
```

## 17.2 Build a pipeline from a sequence

Identify the source, compose transformations and finally collect or reduce. Building a pipeline and traversing it fully are different operations. Captured environments must remain valid throughout actual iterator use.

<!-- verify-book -->
```chali
import std::iter;

void main()
{
    List<int64> xs;
    xs.push(2);
    xs.push(4);
    xs.push(6);
    int64 total = fromList<int64>(xs).fold(0, fn(int64 acc, int64 x)
    {
        acc + x
    });
    println("{} {}", total, xs.len());
}
```

```text
12 3
```

## 17.3 Traverse Map and Set

Map iteration yields key-value items and Set yields keys. Sort explicitly or use an ordered structure when order matters. One observed hash-table traversal is not a storage-format contract.

<!-- verify-book -->
```chali
import std::iter;

void main()
{
    Set<int64> s;
    s.add(1);
    s.add(2);
    s.add(3);
    let si = s.iter();
    int64 sum = 0;
    loop
    {
        match(si.next())
        {
            Value(v) => { sum = sum + v; }
            None => { break; }
        }
    }
    println("sum = {} len = {}", sum, s.len());
    Map<int64, int64> m;
    m.put(1, 10);
    m.put(2, 20);
    let mi = m.iter();
    int64 doubled = 0;
    loop
    {
        match(mi.next())
        {
            Value(pair) => { doubled = doubled + pair.first * pair.second; }
            None => { break; }
        }
    }
    println("doubled = {} len = {}", doubled, m.len());
}
```

```text
sum = 6 len = 3
doubled = 50 len = 2
```

## 17.4 Structural mutation and invalidation

Growth, deletion or reordering can invalidate traversal state and element borrows. Use supported mutation operations. For batch changes, collect stable identifiers and update in a second phase. Early loop exit still cleans iterator state and owned temporaries.

## 17.5 Exercises

1. Does a second any always restart?
2. When does a pipeline actually execute?
3. Is Map iteration an ordering operation?
4. How can matching elements be removed in a batch?

### Answers and checks

1. Do not assume so; inspect consumption or create a fresh iterator.
2. Consumption or terminal operations advance it under its protocol.
3. No; sort explicitly when needed.
4. Use supported APIs or collect identifiers before mutation without retaining invalid references.

Next: [Chapter 18](18-modules-projects.md).
