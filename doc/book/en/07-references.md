# Chapter 7 References

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../07-引用.md)
<!-- END LANGUAGE NAVIGATION -->

A reference borrows an existing value. `const&` provides read-only access; `mut&` permits mutation under the borrowing rules. Creating a reference is explicit, and its source must stay alive for every use of the reference.

```chali
void increment(int64 mut& value)
{
    value = value + 1;
}

void main()
{
    int64 number = 41;
    increment(&number);
    println("{}", number);
}
```

Returning a reference to a local variable would let the reference outlive its storage, so the compiler rejects it. The same rule applies when a reference is hidden in an aggregate or captured by a closure. Adding another layer of syntax does not extend the source object's lifetime.

A lifetime contract written before a function declaration states in the signature where a borrowed result comes from. `#borrow(out, in)` means `out` borrows from `in`, and the list form `#borrow(out, [in1, in2, in3])` borrows from each of them. `#ret` is a reserved name for the return value, so `#borrow(#ret, [a])` on `int64 mut& first(int64 mut& a, int64 mut& b)` says the same thing as Rust's `fn first<'a, 'b>(a: &'a mut i64, b: &'b mut i64) -> &'a mut i64`:

```chali
#borrow(#ret, [a])
int64 mut& first(int64 mut& a, int64 mut& b)
{
    return a;
}
```

Without the contract the checker has to be conservative at the call site and fold in every reference argument, so returning `first(&x, &local)` is rejected because `local` dies at the end of the block. With it, only `a` propagates and the return is accepted. `#static(a)` declares that a's origin is `'static`, which means callers must pass a global. A contract is also an obligation: returning or storing a reference from an undeclared source is an error.

Use a reference to describe access, not ownership. If a result needs independent storage after the callee returns, return an owned value. If a callback must survive its defining scope, select a legal capture mode and check the lifetime of every captured member.

## A lifetime metaphor

ChaLi's borrow checker can be pictured as a “dead-object detector”: it follows reference origins through control flow and asks whether an origin has been moved, destroyed, freed, or left its valid scope at a particular use. The Chinese nickname “死人遥测程序” describes compile-time origin and state analysis, not background telemetry. Existing runtime checks cover cases that static analysis cannot establish; unsafe callers remain responsible for raw-pointer initialization and ownership requirements.

Calling Rust a “life-history checker” can suggest its additional inference of borrow regions and access permissions, but should not imply that it tracks every instant from an object's creation to destruction. Rust's non-lexical lifetimes also use control flow and actual uses. ChaLi currently permits multiple mutable aliases and checks origins, escapes and accesses after invalidation; it does not enforce Rust's exclusive mutable-borrow rule. The metaphor describes an emphasis, not equivalent safety guarantees. See the [official Rust borrow-checker description](https://rustc-dev-guide.rust-lang.org/borrow-check.html).

Exercise: write a read-only helper and attempt to modify its parameter. Then design a function that returns a value instead of a reference to a local. Add `#borrow(#ret, [a])` to a two-parameter function returning a reference and see what the compiler says when you return the other parameter. Next: [resources and scopes](08-resources-scopes.md).
