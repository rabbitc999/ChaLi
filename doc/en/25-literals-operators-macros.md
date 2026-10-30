# Context-typed literals, operator inference and macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../25-literals-operators-macros.md)
<!-- END LANGUAGE NAVIGATION -->

[文档目录](README.md) · [Context-typed literals, operator inference and macros](25-literals-operators-macros.md)

This guide covers integer literals now take their type from context, operator overloading infers more on its own (commutativity, comparison duality, right-hand switches, multiple result types), and the macro DSL has exactly one form. The first two let you write fewer casts and fewer right-hand interfaces; the third leaves exactly one way to write a macro and halves the diagnostic surface.


## Integer literals take their type from context

Literals are no longer always `int64`. The target type of an assignment, initialisation, argument, or `return` flows into the expression, and binary arithmetic and bitwise operators take their type from the other operand. Comparisons and logical operators are excluded; with no context a literal is still `int64`.

```chali
int32 twice(int32 value) { return value * 2; }
void main() {
    int32 a = 1;
    int32 b = a + 1;                 /* 1 is int32 here; no (int32)1 needed */
    int32 sum = 1 + 2;               /* both sides literal: typed by the int32 slot */
    println("{} {} {}", a, b, sum);
}
```

```text
1 2 3
```

A decimal literal that does not fit the target type falls back to the default type and is rejected by the existing "narrowing requires a cast" diagnostic — **it is never silently truncated**:

```chali
void main() {
    int8 x = 200;
    println("{}", x);
}
```

```text
chali: 整数字面量 200 超出 int8 的取值范围（-128..127）
```

Hexadecimal and binary literals are still read as bit patterns, so `int8 x = 0xFF;` is unaffected.

## Operator overloading: commutativity and comparison duality

`+` `*` `==` `!=` `&` `|` `^` are commutative: a type that declares only the left interface also works on the right-hand side, because the compiler swaps the two operands. `-` `/` `%`, ordering comparisons and shifts are not commutative; the reversed form still needs an explicit right-hand interface.

Declaring only `Less` is enough for `5 > a` (dual), `a >= 5` (negated) and `5 <= a` (dual + negated). `a > 5` is not inferred — that would need `int64 < T`, so the type must also declare `Greater`.

```chali
struct Vec : Add<int64, int64>, Less<bool, int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
    bool leftLess(int64 other) { return self.v < other; }
};
void main() {
    Vec a;
    a.v = 10;
    println("{} {}", a + 5, 5 + a);     /* commutativity */
    println("{} {}", a < 5, 5 > a);     /* dual */
    println("{} {}", a >= 5, 5 <= a);   /* negated / dual + negated */
}
```

```text
15 15
0 0
1 1
```

`&&` and `||` are built-in short-circuit operators and **cannot be overloaded**. The diagnostic now says so directly instead of falling through to the generic "cannot find `Add` / `RightAdd`" message: short-circuiting must decide the left operand first, which does not fit the operator-overloading model of evaluating both operands and then calling a method.

## Right-hand switches: `RightXxx` / `#NoRightXxx`

Both commutativity and duality borrow the right operand as the receiver, so a type can state its intent:

- Implement `RightAdd` and the right-hand side is yours: `5 + m` uses `rightAdd` only; if its parameter does not fit, that is an error and it no longer falls back to `Add`.
- Write the compiler directive `#NoRightAdd` before the `struct` (`#NoRight` + the left interface name, no type argument) and the right-hand direction is forbidden outright: `5 + f` fails, while `f + 5` is unaffected.
- `#NoRightXxx` wins over `RightXxx`, and an explicit right interface wins over commutativity inference.

```chali
struct Money : Add<int64, int64>, RightAdd<int64, int64> {
    int64 cents;
    int64 leftAdd(int64 other) { return self.cents + other; }    /* m + 5 */
    int64 rightAdd(int64 other) { return other - self.cents; }   /* 5 + m */
};
#NoRightAdd
struct Forbid : Add<int64, int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
};
void main() {
    Money m;
    m.cents = 120;
    println("{} {}", m + 5, 5 + m);
    Forbid f;
    f.v = 7;
    println("{}", f + 5);
}
```

```text
125 -115
12
```

Writing `5 + f` instead of `f + 5`:

```text
chali: 右向被编译器指令关闭：Forbid 声明了 #NoRightAdd，int64 + Forbid 不再推导；要去掉该指令，或改为实现 RightAdd<R,T>
```

## One operand pair, several result types

`Add<int64,int64>` and `Add<Vec,int64>` can now coexist: two `leftAdd(int64)` overloads that differ only in return type, which ordinary method overloading cannot express. The **target type** of the expression picks one:

```chali
struct Vec { int64 v; };
struct A : Add<int64,int64>, Add<Vec,int64> {
    int64 v;
    int64 leftAdd(int64 other) { return self.v + other; }
    Vec leftAdd(int64 other) {
        Vec r;
        r.v = self.v * other;
        return r;
    }
};
void main() {
    A a;
    a.v = 5;
    int64 x = a + 3;                 /* the int64 one: 5 + 3 */
    Vec y = a + 3;                   /* the Vec one: 5 * 3 */
    println("{} {}", x, y.v);
}
```

```text
8 15
```

With no target type (`let z = a + 3;`) the compiler reports the candidates:

```text
chali: 运算符 + 有多条声明匹配同一对操作数 A / int64，返回类型分别是 int64、Vec（先强转再算；若它们只是返回类型不同，给表达式一个目标类型就能定）
```

**Swapping operands does not change evaluation order.** When `bump() + makeA()` becomes `makeA().leftAdd(bump())`, both operands are still evaluated left to right as written (the left one is parked in a temporary first), so side effects happen in the same order as they would without the swap.

## Compile-time hygienic AST macros

The generated parser builds macro definitions, calls and syntax fragments. Semantic analysis expands hygienic ASTs while resolving type dependencies, before quadruple generation. Expansion does not concatenate source and reparse it. Arguments retain their original spelling, location and caller bindings; ordinary declarations introduced by a template are hygienic, and helper names resolve in the definition module.

```chali
$macro SQUARE(x)
x * x
$end_macro
$macro ANSWER
42
$end_macro
$macro EMPTY()
42
$end_macro
void main() {
    println("{} {} {}", $SQUARE(1 + 2), $ANSWER, $EMPTY());
}
```

Expression arguments are each evaluated once, from left to right, and their saved values are reused inside the macro. `$SQUARE(1 + 2)` is 9; `$SQUARE(next())` calls next once, while two separate next arguments call it twice. Ordinary parameters have value semantics: modifying one changes its copy, without a warning. Copies can be eliminated only when evaluation count, aliasing, ownership and observable behavior remain equivalent. Statement invocations still require a semicolon. Identifier, type and statement parameters retain their respective syntax and source information.

## Groups, variadic fragments and bindings

Use `$macro BUMP_REF(mut &n)` to modify the caller's writable value, and `$macro READ(&n)` for a readonly borrow. With an ordinary `$macro BUMP(n)` containing `n = n + 1;`, `$BUMP(x)` changes only the saved copy and produces no warning. A mutable reference parameter rejects readonly sources; a readonly reference parameter rejects writes. Aliasing and lifetime checks still apply. The compiler may remove a copy only when observable behavior is preserved.

Ordinary parameter types are inferred from their arguments. An otherwise unconstrained integer literal defaults to int64; pass an explicitly typed value, such as `$SQUARE((int32)3)`, when another width is needed. References contained in a copied value retain their original borrow relationships.

`$macro A` requires `$A`; `$macro A()` requires `$A()`. Square, round and curly groups must match in order and shape; repeated groups are allowed. Each group can end with `...rest` or `..rest`, including an empty capture. An applicable fixed signature takes priority over a variadic signature.

```chali
$macro ACCUMULATE(mut &out)[...values]
$for value in values
int64 temporary = value;
out += temporary;
$end_for
$end_macro
void main() {
    int64 temporary = 100;
    int64 total = 0;
    $ACCUMULATE(total)[1, 2, 3];
    println("{} {}", total, temporary);
}
```

This prints `6 100`. Each iteration has distinct declaration identities without an added runtime block. Variadic fragments can be forwarded to another macro or spread in a function argument list. `$for` traverses variadic parameters or statically iterates `$fields<T>()` inside a macro. A parameter in a declaration-name position requires a single identifier; that declaration is visible to subsequent caller code. Member selectors and variable references have separate syntax roles.

## Explicit parameter categories

Fixed and variadic parameters can declare their category explicitly:

| Category | Constraint and expansion behavior |
| --- | --- |
| `x: value` | A complete expression, evaluated once before the expansion body; its saved value copy is reused |
| `x: expr` | A complete expression fragment, evaluated at each placement; unused fragments do not execute |
| `name: ident` | One identifier, usable as a generated declaration or member name |
| `T: type` | A complete type fragment used in a type position |
| `body: stmt` | A complete statement fragment used in a statement position |
| `body: block` | A complete block, following existing statement or block-expression rules |
| `p: pattern` | A match pattern with payloads or top-level alternatives, accepted only in pattern positions |

Value parameters evaluate in declaration order before the expansion body. Expression fragments evaluate wherever that body places them. An expr fragment used as an assignment target can modify the caller's variable. Unannotated parameters retain their existing role inference and ordinary expression arguments still evaluate once. `&n: value` and `mut &n: value` retain readonly and mutable borrowing; legacy reference declarations remain valid. References cannot use expr or other syntax-fragment categories.

```chali
int64 count = 0;
int64 next() { count += 1; return count; }
$macro TwiceValue(x: value)
x + x
$end_macro
$macro TwiceExpr(x: expr)
x + x
$end_macro
void main() {
    int64 value = $TwiceValue(next());
    println("{} {}", value, count);
    count = 0;
    int64 expression = $TwiceExpr(next());
    println("{} {}", expression, count);
}
```

```text
2 1
3 2
```

Category mismatches are rejected before expansion. Value and expr parameters cannot supply declaration names. Each `...xs: value` argument is evaluated once in source order before the body, including unused arguments; nested repetition and forwarding reuse those values. `...xs: expr` runs at each inserted location. Variadics cannot be references. Unannotated variadics retain their syntax-fragment behavior. `p: pattern` accepts a match pattern, including payloads and top-level `|` alternatives, and can only be inserted into a match pattern position. item is a result category, not a parameter category.

## Macro type parameters

Angle brackets are reserved for type parameters:

```chali
$macro CONVERT<T:value>(value)
cast<T>(value)
$end_macro
$macro RESULT<T>(value)
cast<T>(value)
$end_macro
void main() {
    int32 source = 42;
    let same = $CONVERT(source);
    int32 expected = $RESULT(42);
    int64 explicitValue = $RESULT<int64>(42);
}
```

`T:value` uses only that argument's static type. A parameter without a source uses only the expected result type. Explicit types take priority. Inference does not evaluate arguments, insert conversions or switch sources; ordinary value arguments still evaluate once each after expansion, while explicit expr fragments follow their placements. `let x = $RESULT(42)` fails because no expected type is available. Macro value parameters, specialization, partial specialization and arbitrary compile-time function execution are outside this interface.

`#res` explicitly selects the expected result type at the call site and can be combined with an argument source:

```chali
$macro Example<T:#res, U:b>(a, b)
{
    T left = cast<T>(a);
    U right = b;
    yield left + cast<T>(right);
}
$end_macro
void main() {
    int32 result = $Example((int64)3, (int16)4);
    println("{}", result);
}
```

T is int32, U is int16, and the result is 7. Initialization, assignment, function returns and function arguments with a known parameter type can supply the expected result type. Inference does not evaluate either argument. `<T>` retains the same implicit result-source behavior. Explicit type arguments take priority. Without an expected type, `let` does not make T infer from a instead.

## Complete syntax matching

```chali
$macro CLASSIFY(value)
$match value is ident
1
$else is expr
2
$else is type
3
$else is block
4
$else is stmt
5
$else
0
$end_match
$end_macro
```

The five categories are exactly `expr`, `ident`, `type`, `block` and `stmt`. The same generated parser must consume the whole fragment. The first matching branch wins; an identifier may also be an expression. Syntax recognition does not imply valid names or types. Only selected output enters final name, type, capture, borrow and resource checks, while all template syntax must be valid. No match without a fallback emits nothing. A curly argument group is a delimiter rather than an implicit block: pass a complete block as `$A{{ statements; }}`.

## Regular expressions, spelling and origins

```chali
$macro DECIMAL(value)
$regex value is "[0-9]+"
value
$else
0
$end_regex
$end_macro
```

`$regex` uses the full C++ `std::regex` ECMAScript syntax and matches the entire argument spelling after trimming outer whitespace. Internal comments, quotes and newlines remain significant. The language string is decoded once; compiled patterns and invalid patterns are cached. Invalid patterns are compile errors. Capture groups are available inside the regex but do not export variables.

`$stringify(value)` returns original argument text without evaluating or expanding it. `$concat(left, right)` produces a single identifier only. A template-generated identifier keeps its definition context and cannot capture a caller's local binding. Direct source concatenation uses that source context. `$line`, `$file` and `$module` are available directly and refer to the invocation inside a template; forwarding retains original argument origins.

`$replace ADD(a, b) a + b` is a single-line definition using the same AST mechanism. Expansion has recursion and size budgets. Diagnostics retain invocation, definition and argument chains. The editor previews hygienic ASTs one graph per macro call; each graph is a self-contained page that can be embedded in a web page. Type hints come from final static types without exposing internal binding identifiers.

See [AST tests](../../tests/chali_ast_macros.py), [groups and origins](../../tests/chali_macro_groups.py), [static types](../../tests/chali_macro_generics.py) and [syntax and regex branches](../../tests/chali_macro_branches.py).

## Explicit result categories and module declaration macros

A macro header may end in `-> expr`, `-> stmt`, or `-> item`. Unannotated macros retain their existing expansion rules. An expr macro produces an expression result. A stmt macro produces statements and may be invoked as a standalone call in a function body. An item macro is invoked at module declaration position, followed by a semicolon, and produces functions, structures, enums, interfaces, or calls to other item macros.

```chali
$macro RECORD<T>(name:ident) -> item
struct name {T value;}
$end_macro
$macro ANSWER(name:ident) -> item
int64 name(){return 42;}
$end_macro
$RECORD<int64>(Record);
$ANSWER(answer);
void main(){Record record;record.value=answer();assert(record.value==42);}
```

Item macros require explicit type arguments and categorized syntax parameters; runtime value and reference parameters are rejected. Type substitution applies to generated fields, parameters, and return types. Existing `<T:arg>` and `<T:#res>` inference remains available in expression macros. Public names come from ident arguments. Internal helper functions and types receive independent names on each invocation. Generated functions may overload, while duplicate signatures fail. Generated types and bodies pass ordinary name, type, borrow, and resource checks. Item macros cannot generate module declarations, imports, or new macro definitions. Recursive declaration expansion reports its origin chain.

See [declaration macro tests](../../tests/chali_macro_declarations.py).

## Compile-time field metadata

`$fields<T>()` queries a concrete ordinary structure in source declaration order, excluding static fields and methods. It does not create a runtime reflection table or require an import of std::reflect. Indices must be nonnegative integer constants. Write `($fields<T>())[index]` to distinguish indexing from a macro's `[]` argument group.

```chali
struct Item{int64 code;String name;}
void main(){
    assert($fields<Item>().count==2);
    assert(($fields<Item>())[0].name=="code");
    assert(($fields<Item>())[1].type.name=="std::lang::String");
}
```

| Descriptor | Available queries |
| --- | --- |
| `$fields<T>()` | `count`, qualified `name`, type `id`, constant indexing |
| Field | `name`, zero-based `index`, field `id`, `type`, `attributes` |
| Field or annotation-argument `type` | Readable `name`, type `id`; an ordinary structure also provides `fields` |
| `attributes` | `count`, constant indexing |
| Attribute | Qualified `name`, annotation-declaration `id`, `arguments` |
| `arguments` | `count`, constant indexing in annotation parameter declaration order |
| Annotation argument | `name`, exact `type`, bound constant `value` |

Generic instances expose substituted field types. Integer width and signedness, reference permissions, array dimensions, and container parameters remain intact. Continue into nested structures through `($fields<Outer>())[0].type.fields`. Type identities use qualified declarations and concrete arguments; field identities additionally use the owning type and field name. They do not depend on machine addresses or incidental source declaration order. An id is an opaque uint64, not an address, layout offset, or version-independent ABI identifier.

Annotations retain their existing semantic checks and feature switch; annotated programs may explicitly use `--reflection`. For example, after `@annotation Label(text:String,limit:int16=(int16)7)`, a field annotated with `@Label("code")` exposes explicit and default arguments through `.attributes[0].arguments`. Values retain exact types such as int16.

Descriptors cannot be stored in ordinary variables, passed to runtime functions, or converted into runtime objects. Scalar query results use their ordinary language types. The current field entry rejects enums, interfaces, and opcode objects; dynamic indices and layout queries such as `.offset` also fail.

See [field metadata tests](../../tests/chali_macro_metadata.py).

## Pattern parameters and structured repetition

`p: pattern` accepts a complete match pattern. Payload names belong to that branch and are available to a caller-supplied block; free names written in the macro definition retain the definition environment. Top-level `|` alternatives share the branch body and undergo ordinary payload-binding consistency checks. Nested payload positions accept a single pattern. `_` is an unbound wildcard. A pattern cannot supply a runtime expression, type, or declaration name.

```chali
$macro SELECT(value: value, p: pattern, body: block) -> expr
match(value) { p => { yield body; } _ => { 0 } }
$end_macro
enum Box { Value(int64 value), Empty }
void main() {
    Box box = Box::Value(7);
    assert($SELECT(box, Box::Value(x), { yield x + 1; }) == 8);
    assert($SELECT(2, 1 | 2, { yield 9; }) == 9);
}
```

Synchronize variadic groups with `$for (a, b) in (left, right)` and `$end_for`. Variable and source counts must agree, sources must be visible variadics, and their lengths must match. Empty groups are valid; nested repetitions keep separate lexical identities. `$count(xs)` returns the captured element count for optional fragments selected with `$if`. Function argument and tuple separators come from the corresponding AST lists rather than arbitrary text concatenation.

```chali
$macro DOT(...left: value)[...right: value] -> expr
int64 total = 0;
$for (a, b) in (left, right)
total += a * b;
$end_for
total
$end_macro
```

`$DOT(2, 3)[4, 5]` evaluates to 23. Each value argument is evaluated once before the generated body, including unused arguments.

## Field iteration and compile-time conditions

Inside a macro, `$for field in $fields<T>()` generates static code for each field. `$member(object, field)` becomes an ordinary member access and checks the object's concrete type against the descriptor's owner. Normal access, mutation and borrowing rules still apply. Passing `field.type` to a `T: type` parameter preserves the precise field type.

```chali
$macro SUM_INTEGERS<T>(value: value) -> expr
int64 total = 0;
$for field in $fields<T>()
$if field.type.name == "int64"
int64 local = $member(value, field);
total += local;
$end_if
$end_for
total
$end_macro
```

`$if condition`, optional `$else`, and `$end_if` require a compile-time bool constant. Conditions can use metadata properties, variadic counts, and constant arguments. String properties support `==` and `!=` after ordinary language escape decoding. Unselected branches do not undergo ordinary name, type, or borrowing checks; generated branches do. No arbitrary user function or host side effect is executed. Each field iteration has distinct local declaration identities. Empty field collections generate no body, and descriptors cannot escape as runtime values.

Regression entry points: [typed variadics](../../tests/chali_macro_variadics.py), [patterns](../../tests/chali_macro_patterns.py), and [field and structured controls](../../tests/chali_macro_controls.py).

## Compile-time assertions and errors

`$compile_assert(condition)` and `$compile_assert(condition, message)` require a provable bool constant. A successful assertion is a true-valued ordinary expression. As a macro statement it is removed from the generated statement list, including item macros. `$compile_error(message)` stops the current expansion.

Messages must be constant strings. String concatenation, provable named const strings, and field metadata properties are supported. Top-level, module-qualified, and macro-local constants retain their lexical identities. Numeric constants preserve declared width and signedness; a runtime local cannot fall back to a same-named global const. Constant dependency cycles are rejected. An initializer that still calls a user function is not a compile-time value merely because its variable is const. No user function, IO, or runtime formatter executes inside the compiler. Integers cannot replace bool conditions, and an optional message must remain valid even when the assertion succeeds. String leaves and concatenated results have a 1 MiB expansion budget.

```chali
$macro REQUIRE_INT64<T>() -> stmt
$for field in $fields<T>()
$compile_assert(field.type.name=="int64", "unsupported type "+field.type.name);
$end_for
$end_macro
```

Failures retain the call and definition expansion chain and identify the diagnostic argument's source position. Inside field loops they also name each enclosing owner, field, and field index. Errors in unselected `$if` branches do not fire. Existing parameter rules remain in force: value arguments evaluate once, expr arguments are syntax fragments, and compile-time conditions do not make ordinary runtime functions executable in the compiler process.

The [complete field tools example](../../example/reflection/macro_fields.chl) generates printing, validation, and JSON encoding:

```text
chali run example/reflection/macro_fields.chl --no-reflection
```

It preserves field declaration order and encodes String, int64, and bool fields. Other field types produce a compile-time diagnostic at the relevant field. String encoding escapes quotes, backslashes, and control bytes while preserving ordinary UTF-8 bytes; booleans use the JSON true/false tokens. The example imposes its own 64-field assertion, rather than a language limit. JSON parsing is used only to validate the result in main; encoding does not consult runtime field tables. See [diagnostic and field tool tests](../../tests/chali_macro_diagnostics.py).
