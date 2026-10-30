# Chapter 6 Value semantics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../06-值语义.md)
<!-- END LANGUAGE NAVIGATION -->

Two variables containing equal data do not necessarily share mutable state. Value semantics describes observable independence after copying. References borrow existing objects, while moves transfer ownership.

An experiment with a structure containing a list anchors the discussion of parameters, returns and noncopyable resources. Explain behavior through independent mutation and continued source availability, not a promised physical copy or allocation count.

## 6.1 Modify copied values independently

Suppose an original list must remain available while a second list is edited experimentally. If editing the experiment changes the original, the requirement has failed. Value semantics explains the expected independence without first requiring allocation internals.

The example wraps a List inside Bag and copies the whole Bag. Rather than merely comparing equal contents, append to the copy and inspect both lengths. That operation tests independent mutation directly.

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

### Code walkthrough

Bag owns a List<int64> in its items field. Original first receives one element. `Bag copy = original` copies this copyable model's value; wrapping a list in a structure does not change ordinary assignment into a shared mutable-list contract.

Appending two to copy.items makes its length two while original still has one element, producing `1 2`. This verifies observable independence, not a particular allocation count. Reference fields preserve their reference relationships on copying, and resource-owning noncopyable types require a separate check before applying this pattern.


## 6.2 Value parameters and returns

A function receiving a copyable value modifies its own value. Use a mutable reference for write-back and a readonly reference for borrowed access. Copying a reference field preserves that reference; it does not copy the referent.

Returning local owned values, value exits and proven last uses can use implicit transfer or copy elimination. Reason about availability and independent mutation rather than assuming a particular number of physical copies.

## 6.3 Explicit move and noncopyable types

Move expresses ownership transfer. A moved binding cannot subsequently be read, written or reassigned; a new binding can be declared under the language rules. Closeable structures and corresponding NonCopyable types restrict copying. Ordinary value passing does not bypass those restrictions.

Implicit transfer follows ownership checks and does not invalidate a source that is still required. See the ownership documentation for detailed exit rules.

## 6.4 Separate semantics from cost

First determine which object changes, who owns resources and when a source becomes unavailable. Then choose value or reference interfaces. String and collection growth, copying and cleanup have costs; equivalent transformations can remove copies. Measure a concrete workload to assess runtime or allocation cost.

### Summary

Identify copyable owners, references and noncopyable resources before judging assignment and later use. Reference copies retain their source, moves invalidate their source and ordinary owning copies retain independent mutation semantics. A composite model can contain several relationships.

Chapter 7 explains intentional access to the same object. Choose an interface from the required data relationship before measuring performance.

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
