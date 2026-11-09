# Chapter 9 Resources and scopes

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-资源与作用域.md)
<!-- END LANGUAGE NAVIGATION -->

Files, connections and buffers carry cleanup obligations as well as data. Acquisition must be followed by a clear account of ownership, transfer and cleanup on every exit path.

Printed exit actions first distinguish registration from execution. A Closeable example then follows ownership through moves. Smart pointers then illustrate different ways to own a heap value. The chapter ends with the release protocol for manual allocation.

## 9.1 Start with a pair of braces

<!-- verify-book -->
```chali
void main()
{
    int64 outer = 1;
    {
        int64 inner = 2;
        println("里面能看到 {} 和 {}", outer, inner);
    }
    println("外面只剩 {}", outer);
}
```

```text
里面能看到 1 和 2
外面只剩 1
```

outer belongs to main and remains visible in the nested block. inner belongs only to that block. The first print can use both names; the final print uses only outer. Reading inner afterward should fail during compilation. Apply these same boundaries to resource cleanup.

## 9.2 Actions at scope exit

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

Entering the inner block registers first with defer and then second with scope(exit). Registration does not print either message immediately. The ordinary statement executes first, producing body.

Leaving the block runs exit actions in reverse registration order: second, then first. Main's outer braces describe a different scope; moving registrations there would change their timing. To trace cleanup, list the actions registered in each block and reverse that list when following its exit path.


## 9.3 Drop and field cleanup

A Drop implementation handles the value's resources before owned fields are cleaned according to their types. Do not transfer or explicitly release self inside drop. Reference fields do not confer ownership of the referent.

Copying a cleanup-bearing value requires an allowed copy operation and a clear resource owner for each copy. Copying a handle as an integer does not create independent ownership.

## 9.4 Closeable and ownership transfer

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

Handle implements Closeable, and close prints its id to expose cleanup timing. After first receives id one, `consume(move first)` transfers ownership to the function parameter. Consume prints its message, then its owner closes resource one on return. The caller can no longer use first.

Resource two moves from second to third without creating another independently owned copy. After printing third's id, main closes resource two when its scope ends. The output therefore follows consume one, close one, observe two, close two. Printing only demonstrates lifetime behavior; a real handle's close method must release its external resource.


## 9.5 Smart pointers and heap ownership

| Type | Ownership | Common operations |
| --- | --- | --- |
| `Box<T>` | One box owns a heap value exclusively | `Box<T>::newBox(value)` boxes the value. `take()` moves it out and consumes the box. |
| `Rc<T>` | Multiple strong references in one thread own a value together | `rc(value)` creates the first strong reference. `clone()` adds another. |
| `Weak<T>` | Observes whether the value managed by an `Rc<T>` is still alive | `downgrade()` creates a weak reference. `upgrade()` tries to acquire a strong reference. |

`Box<T>` comes from the automatically loaded `std::core` module. Use `move` to transfer a noncopyable value into the box. Both `Box.get()` and `Rc.get()` return a copy and require a copyable value. Import `Rc<T>` and `Weak<T>` from `std::memory` to manage ownership within a single thread.

<!-- verify-book -->
```chali
import std::memory;

void main()
{
    let boxed = Box<int64>::newBox(42);
    println("box={}", boxed.take());

    let first = rc(7);
    let observer = first.downgrade();
    {
        let second = first.clone();
        first.close();
        let upgraded = observer.upgrade();
        if (!upgraded.isEmpty()) {
            println("shared={}", upgraded.get());
        }
    }

    let expired = observer.upgrade();
    println("expired={}", expired.isEmpty());
}
```

```text
box=42
shared=7
expired=1
```

`boxed.take()` moves out `42` and frees the box storage. When `first.close()` releases the first strong reference, `second` still owns `7`, so `observer.upgrade()` acquires another strong reference. At the end of the inner block, `upgraded` and `second` are released in that order. The last strong reference destroys the value. `observer` can still check its status, and the next upgrade returns an empty `Rc<int64>`.

## 9.6 Manual allocation boundaries

New T creates manually allocated storage, optionally with constructor arguments. Its release protocol differs from ordinary automatic local values. Do not assume raw storage is an automatic resource wrapper.

Closing, moving and release affect borrows. The compiler can merge known cleanup paths; repeated release is not a test of arbitrary pointer safety. Chapter 25 covers raw storage and unsafe operations.

### Observe block exit order

Compare output from the block, cleanup and outer scope to see when cleanup runs.

<!-- verify-book -->
```chali
void main() {
    {
        defer {
            println("cleanup");
        }
        println("body");
    }
    println("after");
}
```

```text
body
cleanup
after
```

### Summary

Track registered exit actions separately from the current owner of each resource. A move changes ownership; leaving scope executes exit actions and owning-value cleanup. References do not assume ownership of their sources.

An absence of crashes does not prove correct closure. Verify order, ownership and outcomes on normal and early exits. Chapter 25 extends this reasoning to per-slot initialization and cleanup in raw storage.

Next: [Chapter 10](10-enums-patterns.md).
