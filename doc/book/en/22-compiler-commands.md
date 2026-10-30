# Chapter 22 Directives and derived behavior

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../22-编译器指令.md)
<!-- END LANGUAGE NAVIGATION -->

Directives identify special behavior for the compiler. Use each for a concrete purpose and understand its interaction with ordinary values, references and operators.

## 22.1 Derive equality, hashing and cloning

Use #derive to request supported capabilities such as Equal, Hash and Clone. Derived behavior follows type structure and depends on field semantics. It is separate from deferred declaration-macro features.

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

## 22.2 Operator replacement and receivers

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

## 22.3 Describe borrow provenance

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

## 22.4 Choose the smallest special mechanism

Prefer ordinary methods when they express the business rule. Chapter 8 covers cleanup, Chapter 20 reflection and Chapter 21 C ABI. Check each mechanism at its own boundary instead of stacking directives and guessing their meaning from one run.

## 22.5 Exercises

1. What should be checked alongside derived Hash?
2. Which types must custom operators specify?
3. Can #borrow extend a destroyed object’s lifetime?
4. When should ordinary methods be preferred?

### Answers and checks

1. Equality, key mutability and index use.
2. Operands, receiver and result type.
3. No; it describes valid borrow relationships.
4. When no compiler-specific mechanism is needed.

Next: [Chapter 23](23-unsafe-code.md).
