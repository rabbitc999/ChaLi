# Chapter 17 Iterators

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../17-迭代器.md)
<!-- END LANGUAGE NAVIGATION -->

An iterator separates how values are produced from how they are consumed. `for-in` is often the clearest consumer. Iterator adapters can transform, select and combine values without forcing every caller to reproduce traversal logic.

```chali
void main()
{
    int64 total = 0;
    for (int64 value in range(0, 5))
    {
        total = total + value;
    }
    println("{}", total);
}
```

This half-open range produces zero through four. Endpoint conventions matter when the result becomes an index. Integer range operations also check their documented overflow and endpoint behavior.

Borrowed iteration and value iteration have different costs and permissions. A borrowed element must not outlive the source container. Structural changes during a traversal must follow the container and iterator contracts. A callback that inspects an element should use the appropriate read-only parameter shape rather than unnecessarily copy a large object.

Use adapters when they clarify the pipeline; use an explicit loop when error propagation, resource handling or early exit is easier to see there. Exercise: sum a range and compare the result with a closed-form formula, including an empty range. Next: [modules and projects](18-modules-projects.md).
