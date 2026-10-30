# Chapter 9 Structs and methods

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../09-结构体.md)
<!-- END LANGUAGE NAVIGATION -->

Structures name fields that jointly describe an entity and group methods around that state. Compared with parallel arrays, the type directly expresses that these values belong to one record.

Start with counter methods, proceed to named creation of a generic model, then examine nested cloning, equality and hashing. Representation, creation and valid later updates belong in the same design.

## 9.1 Fields and member methods

A counter combines a total with an operation that changes it. The structure types the field and add places that operation on the model. Self identifies the receiver, keeping field ownership explicit.

Returning Counter allows another member call on the result. Trace individual additions before reading the chain. Ordinary return-self behavior follows the method's type; the initializing self in a named creation entry is introduced separately next.

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

### Code walkthrough

Counter groups total with add, whose self denotes the current receiver. Starting from zero, each call adds its argument and returns a Counter, allowing another add call. The return type is a counter value rather than an integer.

Reading left to right gives totals one, three and six. The outer assignment retains the final result in c, which prints six. That assignment explicitly keeps the chain's result. For other methods, inspect whether they return a new value, return self or only mutate state instead of inferring behavior from dot syntax.


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

### Code walkthrough

Cell<T> uses T as its field type. Create is a static entry associated with the type: it fills the result's self.value and returns that Cell<T>. It does not require the caller to construct an instance first.

The explicitly typed int64 argument in `Cell::create((int64)42)` supplies information for inferring T. Let then infers the full returned type for cell. Its value field prints 42. This is static type inference, not a runtime change to the field's type.


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

### Code walkthrough

Both BookKey and BookRecord request equality, hashing and cloning. The inner model can therefore serve as a Map key, while the full record can be cloned. After initialization, the first output checks equal fields, matching hashes for equal values and presence of the inserted key.

Changing only copied.key.label produces left and changed on the second line, demonstrating value semantics for the owned string inside the nested model. Matching hashes alone do not prove equality. Fields that determine an indexed key's equality and hash should not be arbitrarily mutated while in use. This example queries the original key rather than modifying an indexed key.


## 9.4 Organizing a domain model

Define invariants before exposing operations. Ages and scores need range validation; a class reference must identify an existing record. Static field types do not replace database constraints or business checks. Chapter 13 introduces interface abstractions and Chapter 20 covers reflection and serialization.

### Summary

Structures group fields, methods govern changes, creation establishes initial state and supported derives supply repeated value operations. All retain the fields' copy, borrow and resource rules.

A model with mutually exclusive states and different required data can make a flat field collection ambiguous. Chapter 10 uses enums to represent that choice in the type itself.

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
