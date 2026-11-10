# Appendix C Syntax and Operators

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../C-语法与操作符.md)
<!-- END LANGUAGE NAVIGATION -->

Use this appendix after the introductory chapters to locate less common forms.

## C.1 Names, Literals and Basic Types

Identifiers are case-sensitive. // comments end at the line boundary; /* ... */ delimits block comments. Double quotes form strings and single quotes form char values. Literal suffixes and expected types both affect numeric typing.

| Types | Use |
| --- | --- |
| int8, int16, int32, int64 | Signed integers |
| uint8, uint16, uint32, uint64 | Unsigned integers |
| float32, float64 | Floating-point numbers |
| bool, char, void | Boolean, character and no result |
| String | Owned UTF-8 bytes |

Boolean formatting may print 1 and 0. JSON encoding must explicitly produce true and false text.

## C.2 Declarations, Functions and Type Shapes

Explicit declarations use T name = value, inferred declarations use let, and constants use const. `type Name = T;` introduces an opaque host type. `type alias Name<T> = T;` declares a transparent alias, with optional generic parameters.

| Form | Meaning | Chapter |
| --- | --- | --- |
| int64 add(int64 a, int64 b) | Leading return type | 4 |
| fn add(int64 a, int64 b) -> int64 | Trailing return type | 19 |
| static T create(...) | Named factory; return self is supported | 6, 18 |
| R fn(A, B) | Function-value type | 19 |
| T mut&, T const&, T & | Mutable or read-only references | 8 |
| (A, B), value[0] | Tuple type and positional access | This appendix |
| T values[2][3] | Fixed multidimensional array | 13 |
| T values[] = new[3] | Runtime-sized array | 13 |

Default arguments fill omitted call positions. Ordinary T... values parameters differ from macro capture lists. main returns void and accepts either no arguments or a List<String>.

## C.3 Find Operators by Purpose

| Purpose | Operators |
| --- | --- |
| Arithmetic | +, -, *, /, % |
| Comparison | ==, !=, <, <=, >, >= |
| Short-circuit logic | !, &&, \|\| |
| Bit operations | &, \|, ^, ~, <<, >> |
| Assignment | = and arithmetic/bitwise compound assignments |
| Increment | ++, -- |
| Access and calls | ., ::, (...), [...] |
| Ownership and propagation | move value, result? |
| Conversion | cast<T>(value), (T)value |

Precedence descends through unary, multiplicative, additive, shifts, relations, equality, bitwise AND/XOR/OR and logical AND/OR. Use parentheses when uncertain. Logical operators short-circuit and cannot be overloaded.

An incremented target cannot also be read or written by another operation in the same full expression. Compound-assignment locations are evaluated once. Struct arithmetic uses interfaces such as Add and Less, with explicit right-side interfaces and inference controls described in Chapters 17 and 24.

## C.4 Loops, Matching and Exits

if, elif and else select branches; else if is equivalent to elif. while checks before each iteration, while loop expresses unconditional repetition. Standalone loop statements may omit their semicolon. `for (int64 i = 0; i < n; i++)` scopes its counter to the loop; continue runs the update first. All three parts may be empty, and initialization and update expressions support comma-ordered execution.

for-in uses the iteration protocol. range excludes its upper bound; rangeEq includes it. match supports variants, constants, ranges, concrete interface types and unions. _ is an unbound wildcard.

break and continue default to the nearest loop. break additionally supports explicit levels and all-loop exits; continue has no level annotation. Loop else supplies a value when the body did not produce a result. Value exits and yield belong to their respective structures; return exits the current function. The following complete example collects advanced control expressions here instead of introducing all of them in the first function chapter:

<!-- verify-book -->
```chali
int64 fromBlock()
{
    return
    {
        int64 local = 3;
        yield local;
    };
}

int64 choose(bool first)
{
    return if (first)
    {
        yield 5;
    }
    else
    {
        yield 6;
    };
}

int64 fromMatch(int64 item)
{
    return match (item)
    {
        0 => { yield 7; }
        _ => { yield 8; }
    };
}

int64 fromLoop()
{
    return loop
    {
        yield 9;
    };
}

void main()
{
    println("{} {} {} {}", fromBlock(), choose(false), fromMatch(0), fromLoop());
}
```

```text
3 6 7 9
```

## C.5 Nested Function Types

After learning R fn(A), replace A with another function type. The following is a reading exercise, not a preferred everyday API shape.

level1 adds one; level2 supplies ten. Further levels pass functions inward until the result is eleven. hello passes it to finish, which reads factor=2 and yields twenty-two. Passing a name does not call it.

<!-- verify-book -->
```chali
int64 level1(int64 value)
{
    return value + 1;
}

int64 level2(int64 fn(int64) callback)
{
    return callback(10);
}

int64 level3(int64 fn(int64 fn(int64)) callback)
{
    return callback(level1);
}

int64 level4(int64 fn(int64 fn(int64 fn(int64))) callback)
{
    return callback(level2);
}

int64 level5(int64 fn(int64 fn(int64 fn(int64 fn(int64)))) callback)
{
    return callback(level3);
}

int64 hello(
    int64 fn(int64 fn(int64 fn(int64 fn(int64 fn(int64))))) pipeline,
    int64 fn(int64) finish)
{
    return finish(pipeline(level4));
}

void main()
{
    int64 factor = 2;
    int64 finish(int64 value)
    {
        return value * factor;
    }
    println("{}", hello(level5, finish));
}
```

```text
22
```

## C.6 Combine Generics, Interfaces and Matching

Type parameters, constant parameters, constraints and associated types express different static relationships. Generic closures retain captured state; specialization is not runtime replacement.

Result<A,B> matches success and error labels independently. Typed payload patterns filter concrete interface implementations. Fail(_) binds nothing. Single-payload sugar supports Fail::IoError(e), or IoError(e) when unique. Multiple payloads retain the outer variant and internal type annotations.

Propagation extracts a successful value or returns through the relevant protocol. Concrete errors can widen to implemented interfaces. Option and custom Try still obey their protocols; a payload named Error does not itself select Fail.

## C.7 Macros, Annotations and Compiler Directives

$macro and $replace define hygienic macros. Parameter categories constrain inputs; result categories constrain output positions. Type inference can use #res or an argument's static type. Compile-time controls and field generation are explained in Chapter 22.

$for, $if, $match and $regex repeat or select templates. $elif and $else_if are equivalent additional branches of one $if chain, closed by a single $end_if. $fields and $member generate static field operations; $compile_assert and $compile_error report compile-time conditions. Chapter 22 includes a complete JSON_FIELDS example using both branch spellings.

Use $if, $elif, $else and $end_if inside macros for compile-time template selection, and ordinary if for runtime conditions. Import modules with import. @annotation declares annotations and @Reflect selects runtime metadata. Hash-prefixed compiler directives such as #borrow, #abi and #NonCopyable have specific compiler-defined contracts.

## C.8 Unsafe Operations and Host Boundaries

unsafe admits specified raw operations without removing type and ownership rules. Address, size, alignment and lifetime remain obligations. Raw pointers differ from safe references.

#abi declares C entry points. Prepare representations according to that ABI and do not cast a language closure to a machine pointer. Exported applications still need their application-specific dynamic libraries. Consult Chapters 25–27 for concrete boundaries.

## C.9 Simplified forms and chapter index

These forms require sufficient name or type context and preserve type, ownership and borrow checking. Read the complete programs in the referenced chapters, then use this index to review boundaries.

| Form | Conditions and boundaries | Chapters |
| --- | --- | --- |
| Short imported names | Unambiguous functions, constants, interfaces, enums and structs; qualify conflicts | 11 |
| Inferred function type arguments | Actual arguments and available context determine types; declarations retain necessary parameters | 18 |
| Short variants such as Ok, Fail, Value and None | Enum context or a unique candidate determines construction and matching | 10, 14 |
| Direct single-payload values | Known target enum and one best variant in initialization, assignment, arguments or returns; name ambiguous variants | 10, 14 |
| #borrow(a,b) | Destination a borrows from b; #ret names a return target and lists describe multiple sources; generic and cross-module direct calls are supported | 24 |
| Three-part for | Loop-local initialization declarations, continue through the update, optional parts and comma-ordered initialization/update expressions | 5 |
| Else on for and for-in | Loop variables remain visible; empty input uses ordinary value defaults, invalid references cannot be read and borrowed fallback elements need a copyable snapshot | 5 |
| Standalone loop without a semicolon | Statement, function-end and value-block-end positions; expressions retain the enclosing statement terminator | 5 |
| Counters scoped by the loop | Avoid blocks used solely to scope counters; retain scopes needed for cleanup and borrow lifetimes | 5, 9 |
| Target and build-option constants | $system and $cpu describe the target; -B and TOML build_option define boolean presence queried with $build[name] or a string literal | 22 |

See [Appendix D](D-language-limitations.md) for implementation budgets, conservative analysis and platform boundaries.
