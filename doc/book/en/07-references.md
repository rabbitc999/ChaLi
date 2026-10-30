# Chapter 7 References

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../07-引用.md)
<!-- END LANGUAGE NAVIGATION -->

A reference accesses an existing object instead of creating another owning value. Use the score variable to distinguish readonly access and write-back, then relate reference validity to the source lifetime.

## 7.1 Readonly and mutable borrowing

T const& is a readonly reference and T mut& a mutable reference. Supply a source with &value. Mutable borrowing requires writable storage; a readonly reference does not permit mutating methods.

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

## 7.2 A reference needs a source

A reference is bound to a source when declared; ordinary references do not use null. Option<T const&> and Option<T mut&> represent an optional reference, with None indicating absence.

A copied reference still borrows the same object. Alias sharing does not remove writability or source checks; the language does not impose global exclusive mutable borrowing.

## 7.3 Lifetime, invalidation and escape

A returned reference needs a source that survives the call. Local references cannot escape, and a borrowing closure must not outlive its sources. Collection structure changes, release and moves can also invalidate borrows.

CHL1204 reports source and use paths. Typical fixes return an owning value, use caller-owned storage or shorten a borrow. Raw address casts do not repair lifetime errors.

## 7.4 Choose a parameter form

Small copyable values can be passed by value. Use a reference when accessing an existing object and make mutation explicit. An owning return transfers a value to the caller; a reference return continues borrowing a source. Chapter 8 connects this distinction with cleanup.

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
