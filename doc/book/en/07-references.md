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

Use a reference to describe access, not ownership. If a result needs independent storage after the callee returns, return an owned value. If a callback must survive its defining scope, select a legal capture mode and check the lifetime of every captured member.

Exercise: write a read-only helper and attempt to modify its parameter. Then design a function that returns a value instead of a reference to a local. Next: [resources and scopes](08-resources-scopes.md).
