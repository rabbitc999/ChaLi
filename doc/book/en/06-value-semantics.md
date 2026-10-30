# Chapter 6 Value semantics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../06-值语义.md)
<!-- END LANGUAGE NAVIGATION -->

Distinguish copying, borrowing and transfer by observable behavior. A copied owning value can be modified independently, while reference fields still refer to their source and resource types retain copy restrictions. Value semantics does not require a physical memory copy at every assignment.

## 6.1 Modify copied values independently

Ordinary List assignment has value semantics, including when the list is a structure field. Append to the copy and inspect the two lengths.

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

## 6.2 Value parameters and returns

A function receiving a copyable value modifies its own value. Use a mutable reference for write-back and a readonly reference for borrowed access. Copying a reference field preserves that reference; it does not copy the referent.

Returning local owned values, value exits and proven last uses can use implicit transfer or copy elimination. Reason about availability and independent mutation rather than assuming a particular number of physical copies.

## 6.3 Explicit move and noncopyable types

Move expresses ownership transfer. A moved binding cannot subsequently be read, written or reassigned; a new binding can be declared under the language rules. Closeable structures and corresponding NonCopyable types restrict copying. Ordinary value passing does not bypass those restrictions.

Implicit transfer follows ownership checks and does not invalidate a source that is still required. See the ownership documentation for detailed exit rules.

## 6.4 Separate semantics from cost

First determine which object changes, who owns resources and when a source becomes unavailable. Then choose value or reference interfaces. String and collection growth, copying and cleanup have costs; equivalent transformations can remove copies. Measure a concrete workload to assess runtime or allocation cost.

## 6.5 Exercises

1. What is the original list length after the copy changes?
2. Does copying a reference field copy its referent?
3. Why not reassign a moved binding?
4. How can a readonly function avoid a value copy?

### Answers and checks

1. It remains 1; the copy has length 2.
2. The reference is preserved and its lifetime still matters.
3. The binding is in the moved state; use a new declaration or redesign the interface.
4. Use const& where appropriate and keep the source alive.

Next: [Chapter 7](07-references.md).
