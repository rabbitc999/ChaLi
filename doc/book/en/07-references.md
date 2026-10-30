# Chapter 7 References

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../07-引用.md)
<!-- END LANGUAGE NAVIGATION -->

Some functions need to update the caller's counter or read an existing large object rather than compute from a value copy. References encode access to that same object while requiring a valid source throughout use.

A writing function and a reading function introduce mut& and const&, followed by source binding, absence and escape. Their convenience and restrictions have the same cause: borrowed data is not independently owned by the reference.

## 7.1 Readonly and mutable borrowing

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

### Code walkthrough

Increment's mut& parameter permits mutation through a borrow, and `&score` explicitly supplies the source object. Updating value therefore changes score from three to four rather than changing an integer copy.

Read accepts const&, allowing observation through that interface. It returns the integer it reads, so println prints four. The calls borrow the same live score in sequence; this does not authorize overlapping conflicting mutable access. Read both the reference parameter type and the call-site borrowing syntax to determine which object a function affects.


## 7.2 A reference needs a source

A reference is bound to a source when declared; ordinary references do not use null. Option<T const&> and Option<T mut&> represent an optional reference, with None indicating absence.

A copied reference still borrows the same object. Alias sharing does not remove writability or source checks; the language does not impose global exclusive mutable borrowing.

## 7.3 Lifetime, invalidation and escape

A returned reference needs a source that survives the call. Local references cannot escape, and a borrowing closure must not outlive its sources. Collection structure changes, release and moves can also invalidate borrows.

CHL1204 reports source and use paths. Typical fixes return an owning value, use caller-owned storage or shorten a borrow. Raw address casts do not repair lifetime errors.

## 7.4 Choose a parameter form

Small copyable values can be passed by value. Use a reference when accessing an existing object and make mutation explicit. An owning return transfers a value to the caller; a reference return continues borrowing a source. Chapter 8 connects this distinction with cleanup.

### Summary

Trace each reference from use back to its source, checking writability, lifetime and any move or release. Ordinary references need sources; Option expresses absence instead of an uninitialized reference.

The language permits alias relationships without permitting every overlapping access. Interpret diagnostics through the actual access and source path rather than importing another language's global exclusivity model. Next, connect these sources to cleanup on exit.

## 7.5 Exercises

1. Change increment to a value parameter.
2. Can const storage be passed as mut&?
3. How is an optional reference represented?
4. Why cannot a local reference escape?

### Answers and checks

1. Score is not changed by mutation of the copy.
2. No; change the interface or provide writable storage.
3. Use an Option reference type and handle None.
4. The local source ends when the function returns.

Next: [Chapter 8](08-resources-scopes.md).
