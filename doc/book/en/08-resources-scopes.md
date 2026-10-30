# Chapter 8 Resources and scopes

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-资源与作用域.md)
<!-- END LANGUAGE NAVIGATION -->

Files, buffers and other resources need an owner and a cleanup point. Scope exit, early return and transfer affect cleanup paths. Observe the order before distinguishing automatic owning values from manually allocated storage.

## 8.1 Actions at scope exit

Defer registers an action for leaving the current scope; scope(exit) registers a cleanup block. Exit actions execute in reverse registration order, including paths that leave scopes early.

<!-- verify-book -->
```chali
void main()
{
    {
        defer println("first");
        scope (exit)
        {
            println("second");
        }
        println("body");
    }
}
```

```text
body
second
first
```

## 8.2 Drop and field cleanup

A Drop implementation handles the value's resources before owned fields are cleaned according to their types. Do not transfer or explicitly release self inside drop. Reference fields do not confer ownership of the referent.

Copying a cleanup-bearing value requires an allowed copy operation and a clear resource owner for each copy. Copying a handle as an integer does not create independent ownership.

## 8.3 Closeable and ownership transfer

Closeable expresses a closing protocol and makes its implementing structure noncopyable. Transfer ownership using the move rules and keep sources alive while borrowed. The example observes consumption and close order; ownership is distinct from the numeric id.

<!-- verify-book -->
```chali
struct Handle : Closeable
{
    int64 id;

    void close()
    {
        println("close {}", self.id);
    }
};

void consume(Handle handle)
{
    println("consume {}", handle.id);
}

void main()
{
    Handle first;
    first.id = 1;
    consume(move first);

    Handle second;
    second.id = 2;
    Handle third = move second;
    println("third={}", third.id);
}
```

```text
consume 1
close 1
third=2
close 2
```

## 8.4 Manual allocation boundaries

New T creates manually allocated storage, optionally with constructor arguments. Its release protocol differs from ordinary automatic local values. Do not assume raw storage is an automatic resource wrapper.

Closing, moving and release affect borrows. The compiler can merge known cleanup paths; repeated release is not a test of arbitrary pointer safety. Chapter 23 covers raw storage and unsafe operations.

## 8.5 Exercises

1. Explain why second is printed before first.
2. Does a reference field own its source?
3. How is a Closeable value transferred?
4. Compare local values and new storage.

### Answers and checks

1. Exit actions run in reverse registration order.
2. No; the source follows its own lifetime.
3. Use a valid move and stop using the moved binding.
4. Owning locals use scope cleanup; manual storage follows its release protocol.

Next: [Chapter 9](09-structures.md).
