# Chapter 7 Value Semantics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../07-值语义.md)
<!-- END LANGUAGE NAVIGATION -->

Suppose we want to preserve a score while trying an adjustment. Copying the student record and modifying only the copy gives us an observable introduction to values without allocator details.

## 7.1 Modify the copy

<!-- verify-book -->
```chali
struct Student
{
    String name;
    int64 score;
}

void main()
{
    Student original;
    original.name = "Ada";
    original.score = 70;
    Student copy = original;
    copy.name = "Bob";
    copy.score = 90;
    println("{} {}", original.name, original.score);
    println("{} {}", copy.name, copy.score);
}
```

```text
Ada 70
Bob 90
```

The first operations establish original and its fields. The copy declaration creates another record from this copyable owning value. The next assignments select copy, so the final two prints show independent records.

That observable independence includes the String field. We rely on which modifications are visible, not on a particular allocation count. The compiler may remove copies when behavior is unchanged; that does not change ownership rules.

## 7.2 Values across calls

Chapter 4's addBonus changed its parameter to 75 while main's score stayed 70. A struct passed by value likewise follows its copying rules rather than silently becoming a shared reference.

A method's result type matters too:

<!-- verify-book -->
```chali
struct Counter
{
    int64 total;

    Counter add(int64 x)
    {
        self.total = self.total + x;
        return self;
    }
};

void main()
{
    Counter c;
    c = c.add(1).add(2).add(3);
    println("{}", c.total);
}
```

```text
6
```

The type section declares total and the add method. add updates its receiver and returns a Counter value. This ordinary method return follows its result type; it is distinct from finishing a named construction body.

In main, follow the chain from left to right: one, then three, then six. The outer assignment preserves the final result. Always examine whether another method returns a value, a reference, or no result.

## 7.3 Transfer ownership

`move value` explicitly transfers ownership rather than creating another owner through copying. After the transfer, the old binding cannot continue reading or assigning as though it still owned the value. A name remaining in the source does not mean that it remains usable.

Some types cannot be copied at all. Chapter 9 develops resource ownership; the next chapter introduces references, which use an object without copying it or taking ownership.

### Observe the source after copying

Modify the copy and print both values to observe value semantics.

<!-- verify-book -->
```chali
void main() {
    String original = "tea";
    String copied = original;
    copied.append("!");
    println("{} {}", original, copied);
}
```

```text
tea tea!
```
