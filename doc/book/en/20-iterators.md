# Chapter 20 Iterators

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../20-迭代器.md)
<!-- END LANGUAGE NAVIGATION -->

An iterator encapsulates what remains to be visited, supporting traversal, predicates and reductions. It is distinct from its container: retained data does not mean an already consumed iterator restarts automatically.

All and any expose position changes, fold follows an accumulator and manual next loops cover sets and maps. Understand state before composing longer chains so reuse is not mistaken for fresh traversal.

## 20.1 Start with the shortest pipeline

<!-- verify-book -->
```chali
import std::iter;

void main()
{
    let xs = std::iter::range(1, 6).collect();
    println("{} {}", xs.len(), xs[4]);
}
```

```text
5 5
```

range(1,6) describes values one through five, excluding six. collect consumes them into a list. The result has five elements and its last element is 5. Distinguish describing a sequence from consuming it before adding filter or map.

## 20.2 Consumption and early termination

Consider checking that a list is entirely positive, then checking for a value greater than two. Both succeed when starting afresh. Reusing one iterator instead resumes from the position left by the previous operation.

All and any consume that state rather than merely storing predicates. Compare reuse of first with creation of second to explain the differing results directly from execution.

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

First is an iterator with a current position. All tests positivity, succeeds for one through four and consumes this iterator to its end. Any on the same first then has no remaining items, so it returns false even though the original list contains values greater than two.

Calling xs.iter again creates second with a fresh position, allowing any to find a matching item and return true. Consuming iteration state does not delete list elements. All need not always traverse everything: it can stop at a failed predicate. This input reaches the end because every value is positive.


## 20.3 Build a pipeline from a sequence

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

FromList<int64>(xs) supplies a sequence for fold, whose initial accumulator is zero. The closure receives the previous accumulator as acc and the current element as x. Returning acc+x yields successive totals two, six and twelve.

The fold result initializes total while the original list retains three entries, producing `12 3`. Accumulator and callback result types must agree. A different reduction operation may need a different initial value; zero is not universal for every fold.


## 20.4 Traverse Map and Set

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
    };
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
    };
    println("doubled = {} len = {}", doubled, m.len());
}
```

```text
sum = 6 len = 3
doubled = 50 len = 2
```

The first loop calls a set iterator's next manually. Each Option indicates either an item or exhaustion. Value(v) adds to the sum; None breaks. The sum is six and the original set still has length three. Addition here does not depend on traversal order.

Map next returns a pair, with the key in first and value in second. The actual formula is 1*10 plus 2*20, giving fifty rather than merely summing the values; the variable name doubled does not change it. Both loops read their containers. Structural mutation during traversal must follow iterator restrictions and is not authorized by this read-only example.


## 20.5 Structural mutation and invalidation

Growth, deletion or reordering can invalidate traversal state and element borrows. Use supported mutation operations. For batch changes, collect stable identifiers and update in a second phase. Early loop exit still cleans iterator state and owned temporaries.

### Summary

Traversal obtains a source, advances, handles items and ends. All, any and fold consume current state; next expresses availability through Option. A new container iterator starts a fresh traversal.

Check each interface for source mutation, value versus borrowed elements and invalidating modifications. An order-independent sum does not establish stable ordering. Chapter 18 extends the discussion from individual algorithms to modules and project dependencies.

## 20.6 Exercises

1. Does a second any always restart?
2. When does a pipeline actually execute?
3. Is Map iteration an ordering operation?
4. How can matching elements be removed in a batch?

### Answers and checks

1. Do not assume so; inspect consumption or create a fresh iterator.
2. Consumption or terminal operations advance it under its protocol.
3. No; sort explicitly when needed.
4. Use supported APIs or collect identifiers before mutation without retaining invalid references.

Next: [Chapter 11](11-modules-projects.md).
