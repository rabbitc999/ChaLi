# Chapter 9 Structs and methods

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-结构体.md)
<!-- END LANGUAGE NAVIGATION -->

Model students, courses and coordinates as named groups of fields. Structs store state, methods maintain relationships, and construction establishes an initial value.

## 9.1 Fields and member methods

A struct declares statically typed fields. Methods use self to access the receiver and may modify it only when its access permits mutation. Field names express meaning without positional indexes.

This counter illustrates member calls and chaining. Ordinary methods follow their declared value or reference return type; named construction has its own initialization rules.

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

## 9.2 Named construction

A named static T create entry may initialize self and return self. Call it through its type. Generic arguments can be inferred when sufficient context exists. This self is the result being initialized, not a global receiver available to every static function.

<!-- verify-book -->
```chali
struct Cell<T>
{
    T value;
    static Cell<T> create(T value)
    {
        self.value = value;
        return self;
    }
};
void main()
{
    let cell = Cell::create((int64)42);
    println("{}", cell.value);
}
```

```text
42
```

## 9.3 Value operations and derivation

Assignment, parameter passing and collection storage follow Chapter 6. Declare supported derivations such as Equal, Hash and Clone explicitly. Mutating fields used by hashing does not preserve an existing index. This example uses a key in a set and compares nested values after copying.

<!-- verify-book -->
```chali
#derive(Equal, Hash, Clone)
struct BookKey
{
    String label;
    int64 id;
}

#derive(Equal, Hash, Clone)
struct BookRecord
{
    BookKey key;
}

void main()
{
    BookRecord original;
    original.key.label = "left";
    original.key.id = 7;
    BookRecord copied = original.clone();
    Map<BookKey, int64> cache;
    cache.put(original.key, 10);
    println("{} {} {}", original == copied,
            original.hash(original) == copied.hash(copied),
            cache.has(original.key) != 0);
    copied.key.label = "changed";
    println("{} {}", original.key.label, copied.key.label);
}
```

```text
1 1 1
left changed
```

## 9.4 Organizing a domain model

Define invariants before exposing operations. Ages and scores need range validation; a class reference must identify an existing record. Static field types do not replace database constraints or business checks. Chapter 13 introduces interface abstractions and Chapter 20 covers reflection and serialization.

## 9.5 Exercises

1. Add a reading method to Cell.
2. Use a String payload in Cell.
3. Explain two contexts for returning self.
4. List three student-model invariants.

### Answers and checks

1. Return the field according to the declared value or reference type.
2. Call Cell::create("name") and inspect its field.
3. Construction initializes a result; ordinary methods follow their return type.
4. For example, unique IDs, valid ages and existing classes; validate on writes.

Next: [Chapter 10](10-enums-patterns.md).
