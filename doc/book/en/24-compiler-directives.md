# Chapter 24 Compiler Directives

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../24-编译器指令.md)
<!-- END LANGUAGE NAVIGATION -->

This chapter uses directives to define operator replacement, describe where a returned reference comes from, and suppress naming warnings in one source file. [Chapter 22](22-macros.md) covers interface derivation through member macros.

## 24.1 Operator replacement and receivers

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


## 24.2 Describe borrow provenance

A #borrow annotation describes the relationship between a returned borrow and its input source. It does not keep destroyed objects alive or cancel restrictions on overlapping mutable access.

<!-- verify-book -->
```chali
#borrow(#ret, a)
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


A single source can use #borrow(#ret,a). In contrast, #borrow(a,b) means that destination a saves a reference from b, exactly like #borrow(a,[b]); it is not shorthand for two return sources. Use #borrow(#ret,[a,b]) for either possible return source. The list is an upper bound, and even an unused listed source constrains a caller.

The same contract applies to generic functions, whose direct calls may still infer type arguments. Contracts travel with module function signatures to importers. Cross-module generic calls check concrete origins, and forwarding inside a module must preserve the relationship. Bodies are checked at concrete instantiation; an unused declaration does not verify every type combination. Branch and loop joins conservatively retain possible origins without proving arbitrary numerical conditions complementary. Prefer an explicit if/else when every branch must change the origin. The executable [generic borrowing and control-flow sample](../../../example/samples/generic_borrow_control_flow.chl) covers integer, floating-point and String instances.

Contracts check initialization, aliases, rebinding and joins; they neither extend local storage nor revive moved values. Function-value types currently cannot carry contracts, so functions with #borrow or #static must be called directly. A closure may make such a direct call while retaining normal capture and argument checks. Host implementations and unsafe raw addresses do not acquire a complete safety proof from this directive. See the [resource lifetime reference](../../en/22-resource-lifetime.md) for output borrows and other limits.

Without an explicit return-source contract, an ordinary function call conservatively combines the possible sources of all reference arguments. The assigned result must stay within every source lifetime. The following example is rejected even though first's body returns a.

<!-- verify-book-error: CHL1204 -->
```chali
int64 mut& first(int64 mut& a, int64 mut& b) { return a; }

int64 mut& escape(int64 mut& stable)
{
    int64 temporary = 2;
    int64 mut& selected = first(&stable, &temporary);
    return selected;
}

void main() {}
```

```text
CHL1204
```

## 24.3 Choose the smallest special mechanism

Prefer ordinary methods when they express the business rule. Chapter 9 covers cleanup, Chapter 23 reflection and Chapter 26 C ABI. Check each mechanism at its own boundary instead of stacking directives and guessing their meaning from one run.

### Suppress naming warnings in one source file

Write `#NoNamingCheck` on its own line when a source file needs to keep names that differ from the naming rules. A trailing line comment is allowed. The directive applies to the entire file. For example, the module name `legacy::OldName` can be retained in that file. Imported files and other warning rules retain their own settings. The editor uses the same compiler check.

### Suppress naming checks in source

This file keeps a legacy module name and still runs as a complete program.

<!-- verify-book -->
```chali
#NoNamingCheck
module LegacyName;

void main() {
    println("legacy");
}
```

```text
legacy
```

### Summary

Operator directives limit replacement, and borrow declarations identify result sources. Check each against its body and call site instead of generalizing one output to every combination.

Use ordinary methods for business behavior and the smallest special mechanism when needed. Unsafe operations next also require explicit premises, with more proof obligations assigned to their author.

Next: [Chapter 25](25-unsafe.md).
