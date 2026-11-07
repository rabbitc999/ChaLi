# Chapter 8 References

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-引用.md)
<!-- END LANGUAGE NAVIGATION -->

Some functions need to update the caller's counter or read an existing large object rather than compute from a value copy. References encode access to that same object while requiring a valid source throughout use.

A writing function and a reading function introduce mut& and const&, followed by source binding, absence and escape. Their convenience and restrictions have the same cause: borrowed data is not independently owned by the reference.

## 8.1 A second way to access one value

<!-- verify-book -->
```chali
void main()
{
    int64 value = 7;
    int64 mut& alias = &value;
    // 造一个「指向 value」的可变引用
    alias = 9;
    // 写引用 = 写 value
    println("{} {}", value, alias);
}
```

```text
9 9
```

value starts at 7. &value binds alias as an int64 mut& reference, not another independent integer. Writing 9 through alias updates the source, so both reads produce 9. Compare this with copying a struct in the previous chapter.

## 8.2 Readonly and mutable borrowing

Independent copies suit experiments, but incrementing a counter often needs to change the original. Express that distinction in the interface: mut& borrows writable access and const& borrows read access. The call's &value identifies the source.

Increment changes score and read subsequently observes it. They work on the same integer with different permissions. A read-only reference cannot invoke mutating operations, and a mutable parameter still requires a writable source.

<!-- verify-book -->
```chali
void increment(int64 mut& value)
{
    value += 1;
}

int64 read(int64 const& value)
{
    return value;
}

void main()
{
    int64 score = 3;
    increment(&score);
    println("{}", read(&score));
}
```

```text
4
```

Increment's mut& parameter permits mutation through a borrow, and `&score` explicitly supplies the source object. Updating value therefore changes score from three to four rather than changing an integer copy.

Read accepts const&, allowing observation through that interface. It returns the integer it reads, so println prints four. The calls borrow the same live score in sequence; this does not authorize overlapping conflicting mutable access. Read both the reference parameter type and the call-site borrowing syntax to determine which object a function affects.


## 8.3 A reference needs a source

A reference is bound to a source when declared; ordinary references do not use null. Option<T const&> and Option<T mut&> represent an optional reference, with None indicating absence.

A copied reference still borrows the same object. Alias sharing does not remove writability or source checks; the language does not impose global exclusive mutable borrowing.

## 8.4 Lifetime, invalidation and escape

A returned reference needs a source that survives the call. Local references cannot escape, and a borrowing closure must not outlive its sources. Collection structure changes, release and moves can also invalidate borrows.

CHL1204 reports source and use paths. Typical fixes return an owning value, use caller-owned storage or shorten a borrow. Raw address casts do not repair lifetime errors.

### Counterexample: returning a local borrow

The local storage ends when the function returns, leaving the reference without its source. The diagnostic excerpt below uses the compiler’s Chinese language setting.

<!-- verify-book-error: check -->
```chali
int64 const& bad() {
    int64 local = 7;
    return &local;
}
void main() {
}
```

```text
[CHL1204] 返回值中的引用不能超过局部来源的生命周期
```

## 8.5 Choose a parameter form

Small copyable values can be passed by value. Use a reference when accessing an existing object and make mutation explicit. An owning return transfers a value to the caller; a reference return continues borrowing a source. Chapter 9 connects this distinction with cleanup.

### Summary

Trace each reference from use back to its source, checking writability, lifetime and any move or release. Ordinary references need sources; Option expresses absence instead of an uninitialized reference.

The language permits alias relationships without permitting every overlapping access. Interpret diagnostics through the actual access and source path rather than importing another language's global exclusivity model. Next, connect these sources to cleanup on exit.

Next: [Chapter 9](09-resources-scopes.md).
