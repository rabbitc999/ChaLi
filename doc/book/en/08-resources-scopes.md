# Chapter 8 Resources and scopes

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-资源与作用域.md)
<!-- END LANGUAGE NAVIGATION -->

Files, connections and buffers carry cleanup obligations as well as data. Acquisition must be followed by a clear account of ownership, transfer and cleanup on every exit path.

Printed exit actions first distinguish registration from execution. A Closeable example then follows ownership through moves. Only after automatic cleanup is clear does the chapter introduce manual allocation with its separate protocol.

## 8.1 Actions at scope exit

Before opening a real file, use output to observe doing something when a block ends. Defer registers an exit action and scope(exit) can register a block of code. Execution continues after registration; actions run when that scope is actually left.

Multiple actions run in reverse registration order, which supports unwinding layered resource acquisition. Early returns and loop exits also cross cleanup boundaries, so inspect more than just natural fall-through.

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

### Code walkthrough

Entering the inner block registers first with defer and then second with scope(exit). Registration does not print either message immediately. The ordinary statement executes first, producing body.

Leaving the block runs exit actions in reverse registration order: second, then first. Main's outer braces describe a different scope; moving registrations there would change their timing. To trace cleanup, list the actions registered in each block and reverse that list when following its exit path.


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

### Code walkthrough

Handle implements Closeable, and close prints its id to expose cleanup timing. After first receives id one, `consume(move first)` transfers ownership to the function parameter. Consume prints its message, then its owner closes resource one on return. The caller can no longer use first.

Resource two moves from second to third without creating another independently owned copy. After printing third's id, main closes resource two when its scope ends. The output therefore follows consume one, close one, observe two, close two. Printing only demonstrates lifetime behavior; a real handle's close method must release its external resource.


## 8.4 Manual allocation boundaries

New T creates manually allocated storage, optionally with constructor arguments. Its release protocol differs from ordinary automatic local values. Do not assume raw storage is an automatic resource wrapper.

Closing, moving and release affect borrows. The compiler can merge known cleanup paths; repeated release is not a test of arbitrary pointer safety. Chapter 23 covers raw storage and unsafe operations.

### Summary

Track registered exit actions separately from the current owner of each resource. A move changes ownership; leaving scope executes exit actions and owning-value cleanup. References do not assume ownership of their sources.

An absence of crashes does not prove correct closure. Verify order, ownership and outcomes on normal and early exits. Chapter 23 extends this reasoning to per-slot initialization and cleanup in raw storage.

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
