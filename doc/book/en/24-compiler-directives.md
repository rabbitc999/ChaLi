# Chapter 24 Directives and derived behavior

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../24-编译器指令.md)
<!-- END LANGUAGE NAVIGATION -->

Some behavior requires compiler participation, including derived equality/hashing and connecting a returned reference to selected parameter sources. Directives expose these mechanisms while retaining ordinary type and ownership rules.

Observe derived values, compare operator replacement directions and trace returned borrow sources. Identify the behavior a directive changes rather than treating its presence as an exemption from other checks.

## 24.1 Derive equality, hashing and cloning

Equality, cloning and hashing often repeat a model's field structure. Supported derive directives supply those implementations for protocol-based use.

Request Equal, Hash and Clone for Point, compare its copy and then change one field. This connects directives to observable value semantics rather than syntax alone.

<!-- verify-book -->
```chali
#derive(Equal, Hash, Clone)
struct Point
{
    int64 x;
    int64 y;
    String label;
};

void main()
{
    Point origin;
    origin.x = 0;
    origin.y = 0;
    origin.label = "O";

    Point copy = origin.clone();
    println("equal={}", origin == copy);
    println("sameHash={}", origin.hash(origin) == copy.hash(copy));

    copy.label = "O2";
    println("afterChange={}", origin == copy);
    println("original={}", origin.label);
}
```

```text
equal=1
sameHash=1
afterChange=0
original=O
```

All three Point fields participate in derived behavior. Origin starts with coordinates zero, zero and label O. Cloning preserves those contents, making equal and sameHash true. The latter compares hash results rather than checking a fixed numeric hash.

Changing copy.label to O2 changes overall equality even though coordinates still match. AfterChange becomes false while origin.label stays O, demonstrating independent mutation. Equal objects require consistent hashes, but equal hashes alone do not prove equality or provide persistent identity.

Fields here mean instance fields. Static fields belong to shared type state and do not participate in these derives. Chinese and emoji labels are hashed as complete UTF-8 bytes; positive and negative zero also hash consistently for float32/64 fields. Nested types use their own protocols: if handwritten equality compares only id, handwritten hashing needs the same meaning.


### Deriving your own interface

A same-name interface/member-macro pair can share an implementation across types. This example counts instance fields during expansion; the resulting method does not query a runtime reflection table.

<!-- verify-book -->
```chali
interface Count
{
    int64 count();
}

$macro Count<T>() -> member
int64 count()
{
    int64 result = 0;
    $for field in $fields<T>()
    result += 1;
    $end_for
    return result;
}
$end_macro

#derive(Count)
struct Record
{
    int64 number;
    String text;
}

void main()
{
    Record record;
    Count counted = record;
    println("fields={}", counted.count());
}
```

```text
fields=2
```

The interface requires an instance method returning an integer; a handwritten implementation can satisfy it too. In the macro header, T is a type parameter, the empty group accepts no ordinary arguments, and `member` generates type members. `#derive(Count)` supplies Record as T. The interface and macro belong to the same module.

The method creates result, and the static loop emits one increment for each of Record's two instance fields. It reads no field values and copies no String payload. Adding a third field and recompiling emits a third increment.

Finally, assigning Record to Count and invoking count checks the actual interface implementation. Signature-matching methods become public while helpers remain hygienic. Derivation does not overwrite same-signature methods or add layout fields. Imported pairs can be selected as `#derive(derive_tools::Count)`; generic owners expand per instance, and enums can derive ordinary methods using Chapter 22's variant/payload descriptors.

## 24.2 Operator replacement and receivers

Custom operators need explicit operand roles, return types and receiver mutation rules. These examples inspect behavior when compound or right-operand replacement is disabled. Familiar symbols do not imply built-in numeric conversions for arbitrary types.

<!-- verify-book -->
```chali
#NoAddReplace
struct Vec : Add<int64, int64>
{
    int64 v;

    int64 leftAdd(int64 other)
    {
        return self.v + other;
    }
};

void main()
{
    Vec a;
    a.v = 10;
    println("left={}", a + 5);
}
```

```text
left=15
```

Vec implements Add<int64, int64>. LeftAdd receives the right-hand integer and returns an integer. For a+5 it reads a.v equal to ten and returns fifteen without converting a itself to int64.

NoAddReplace restricts the associated replacement behavior without removing this explicitly implemented left addition. The body only reads self.v, so a.v is unchanged. A valid a+5 does not establish that a+=5, 5+a or other combinations are valid; those have their own interface and directive requirements.


<!-- verify-book -->
```chali
#NoRightAdd
struct Chain : Add<int64, int64>
{
    int64 v;

    int64 leftAdd(int64 other)
    {
        return self.v + other;
    }
};

void main()
{
    Chain chain;
    chain.v = 7;
    println("chain + 1 = {}", chain + 1);
}
```

```text
chain + 1 = 8
```

Chain also supplies leftAdd. With chain.v equal to seven, chain+1 returns eight. NoRightAdd restricts right-operand replacement; Chain is on the left here and retains its implemented left addition.

The example deliberately executes only that direction and does not establish a valid 1+chain. Swapping operands is not automatically a permitted algebraic transformation for a custom type. Receiver, result type and effects come from the implementation, so a familiar plus sign does not promise commutativity.


## 24.3 Describe borrow provenance

A #borrow annotation describes the relationship between a returned borrow and its input source. It does not keep destroyed objects alive or cancel restrictions on overlapping mutable access.

<!-- verify-book -->
```chali
#borrow(#ret, [a])
int64 mut& first(int64 mut& a, int64 mut& b)
{
    return a;
}

int64 mut& outer(int64 mut& x)
{
    int64 local = 5;
    return first(&x, &local);
}

void main()
{
    int64 number = 30;
    int64 mut& selected = outer(&number);
    selected = 31;
    println("{} {}", number, selected);
}
```

```text
31 31
```

The borrow directive states that first's returned borrow comes only from a. Although b is also accepted, the body returns a, so the result's lifetime need not depend on every parameter source.

Outer passes an external x and local temporary. The returned source remains x, so destroying local does not invalidate it. Selected therefore refers to number in main, and writing 31 updates both observations. A source declaration must agree with the implementation; it cannot disguise a local reference as an external borrow.


## 24.4 Choose the smallest special mechanism

Prefer ordinary methods when they express the business rule. Chapter 8 covers cleanup, Chapter 20 reflection and Chapter 21 C ABI. Check each mechanism at its own boundary instead of stacking directives and guessing their meaning from one run.

### Summary

Derives reduce repeated protocol implementations, operator directives limit replacement and borrow declarations identify result sources. Check each against its body and call site instead of generalizing one output to every combination.

Use ordinary methods for business behavior and the smallest special mechanism when needed. Unsafe operations next also require explicit premises, with more proof obligations assigned to their author.

## 24.5 Deriving Comparison and Hashing for Nested Values

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

The inner value declares derived capabilities, and the enclosing type composes them from its fields. Equal contents compare equal; using them as Map keys also checks compatible hashing. Derivation uses field behavior, not merely a type name, and missing field capabilities remain compilation errors.

### Generating JSON encoding and decoding rules

Program field names need not match external keys. This example keeps the language member id, publishes student_id, and excludes an internal string.

<!-- verify-book -->
```chali
import std::json;

#derive(Json)
struct Profile
{
    @std::json::Field(name:"student_id")
    int64 id;
    String name;
    @std::json::Field(skip:true)
    String secret;
}

void main()
{
    Profile profile;
    profile.id = 7;
    profile.name = "Ada";
    profile.secret = "internal";
    println("{}", profile.toJson().unwrap());

    let decoded = Profile::fromJson("{\"student_id\":7,\"name\":\"Ada\"}");
    match (decoded)
    {
        Ok(value) => { println("{} {}", value.id, value.name); }
        Err::MessageError(error) => { println("{}", error.toString()); }
        Err(_) => { println("unavailable"); }
    }
}
```

```text
{"student_id":7,"name":"Ada"}
7 Ada
```

Import std::json first. Json declares toJson; derivation also generates a static fromJson on the concrete type. Both return Result with Error: encoding produces String and decoding produces an independently owned Profile.

The name annotation changes the JSON key, not profile.id. skip excludes secret from encoding and input reconstruction, leaving its default-constructed value after decoding. This annotation is restricted to instance fields of structures that derive Json; functions, static fields and unrelated types reject it.

main constructs the value and prints its encoding in declaration order. The second half decodes external text. Ok receives the owned value, MessageError handles a concrete diagnostic, and Err(_) covers other Error implementations. A real application must still validate the identifier and enforce account permissions.

Missing required fields, unknown keys, wrong types and out-of-range values return Err. A known skipped input key is ignored. Renamed fields use their new key. Duplicate mapped names, repeated field annotations and ASCII controls in key names are compile-time errors. Derived enum tag objects also reject unknown fields.

Supported values include all base numeric types, bool, char, String, List, Set, String-keyed Map and recursively supported Option/Result. char encodes as a byte number. uint64 above the signed 64-bit range encodes as a decimal string to preserve precision. Floats must be finite and decoding checks the destination width. Enums use objects with $variant and $values in payload declaration order.

Nested user types must also derive Json. References, functions, dynamic interfaces, arrays, noncopyable resources and other unsupported types fail at compile time, including skipped fields. Existing toJson/fromJson methods are never replaced. The implementation generates ordinary concrete methods and currently requires reflection code generation; basic JSON parsing remains available without it.

Json derivation does not expose runtime fields through @Reflect. These are separate choices. Existing reflected types without Json derivation retain their previous serialization policy. Debug in Chapter 22 is for inspection, not JSON persistence.

## 24.6 Exercises

1. What should be checked alongside derived Hash?
2. Which types must custom operators specify?
3. Can #borrow extend a destroyed object’s lifetime?
4. When should ordinary methods be preferred?

### Answers and checks

1. Equality, key mutability and index use.
2. Operands, receiver and result type.
3. No; it describes valid borrow relationships.
4. When no compiler-specific mechanism is needed.

Next: [Chapter 25](25-unsafe.md).
