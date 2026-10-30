# Chapter 22 Preprocessing and Macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../22-预处理与宏.md)
<!-- END LANGUAGE NAVIGATION -->

Some repetition concerns program structure rather than repeated calls to one function. Macros compose syntax during compilation to reuse that structure, making evaluation count, name origins and type context essential to the interface.

Begin with evaluation counts, then generate expressions, statements, module declarations and members. Finally, iterate over struct fields to generate different operations. Separate the structure produced during compilation from the statements executed at runtime.

## 22.1 Value parameters, references and hygiene

Start with an argument next() that changes state. A macro reusing one result should evaluate once and reuse that value. A macro accepting expression syntax may execute at each placement. Similar-looking calls therefore need an interface that makes the difference explicit.

Unannotated parameters use single evaluation per argument: one next() argument runs once, while two separate arguments each run once. Mutating the parameter copy does not update the caller, and copy elimination must preserve behavior. The next section makes value versus expr visible with a counter.

Explicit &n borrows read-only and mut &n permits caller mutation, subject to static borrowing. Internal temporary names remain hygienic.


### Updating a Copy or the Source

<!-- verify-book -->
```chali
$macro Bump(n) -> stmt
n = n + 1;
$end_macro
$macro RefBump(mut &n:value) -> stmt
n = n + 1;
$end_macro
void main()
{
    int64 x = 7;
    $Bump(x);
    println("{}", x);
    $RefBump(x);
    println("{}", x);
}
```

```text
7
8
```

Bump has no reference annotation. Its assignment changes the parameter copy, so the first output remains 7. This does not produce a copy-mutation warning.

RefBump explicitly borrows a mutable source with mut &n. Its assignment updates x, producing 8. Use &n for a read-only borrow. The compiler may remove an unnecessary copy while preserving these observable results; it cannot turn a private update into a write to x.

## 22.2 Parameter categories control expansion

x:value keeps single evaluation. x:expr accepts expression syntax evaluated where inserted: two placements may execute twice and no placement does not execute it. name:ident, T:type, body:stmt and body:block require their corresponding syntax categories.

Value arguments are evaluated in argument order before expr placements execute in the expanded body. Do not assume expr preserves single evaluation for side effects.

<!-- verify-book -->
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

Each next call increments the global count and returns its new value. TwiceValue evaluates next() once to obtain one, then reuses that value in both additions. Its result is two while count is one, explaining `2 1`.

After resetting count, TwiceExpr places expression syntax at both use sites. Runtime calls return one and two, yielding three with count two. Thus `3 2`. Identical-looking x+x bodies have different evaluation behavior because their categories differ, affecting input reads, state updates and resource acquisition.


### Choose categories by evaluation behavior

Decide whether the macro should reuse a computed value or place syntax at a new execution point. Input reads, counter increments and resource acquisition need explicit evaluation counts; ordinary value parameters are usually suitable. Choose expr when the expanded control structure must determine execution placement, and document that execution may be skipped or repeated. Copy costs and effects still follow the argument type's copy rules.

Ident carries a name, type carries type syntax, and stmt and block carry statements or blocks. These categories describe syntax positions. A generic parameter T and a parameter written T:type have different roles. Category matching does not exempt names, types, borrows or ownership from ordinary checking, and an identifier does not grant permission to mutate its object.

Hygiene associates names with their origins. A template's result can coexist with the caller's result without overwriting it. An undeclared temp in the template cannot be repaired by an unrelated caller variable of that name. Reusable macros therefore do not require callers to avoid a hidden list of temporary names. Interfaces should state explicitly when they affect caller objects.

## 22.3 Generic parameters and result context

Macros may declare type parameters. T:#res infers from the statically expected result type, while U:b infers from argument b. Neither runs the argument expression during compilation. Explicit arguments take priority; missing context or conflicting constraints require more type information.

For example, $macro Example<T:#res, U:b>(a, b) combines result and argument information. Its inferred types still must satisfy the body and call-site checks.

<!-- verify-book -->
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

```text
7
```

T:#res obtains int32 from result's expected type. U:b obtains int16 from the second argument. Although the first argument is int64, cast<T> converts it for left. Right initially keeps U and is converted to T at the addition.

The sum therefore fits the result context and yields seven. Inference uses static information rather than executing b to discover its type. If context is missing or constraints conflict, supply explicit types or correct the call instead of expecting runtime adaptation.


## 22.4 Argument groups and variadic parameters

Fixed parameters describe individual positions. Variadic parameters collect related inputs. First build a list, then annotate whole groups with a syntax category.

<!-- verify-book -->
```chali
$macro vec<T:first>[first,...rest]
{
    List<T> result;
    result.push(first);
    $for element in rest
    result.push(element);
    $end_for
    yield result;
}
$end_macro

void main()
{
    List<int64> result;
    result.push(99);
    List<int64> values = $vec[1, 2, 3];
    println("{} {} {}", values.len(), values.at(2), result.at(0));
}
```

```text
3 3 99
```

Vec uses a bracket call group. First provides both the initial item and T's inference source, while rest collects later arguments. Its internal List<T> receives first, and compile-time $for generates a push for each remaining argument. Those pushes execute when the generated program runs; yield supplies the completed list.

The call produces three entries, and at(2) returns three because indexing starts at zero. The caller's separate result contains 99. Hygiene keeps the macro's result distinct, so the last output remains 99. Compile-time $for differs from runtime for, and this supported repetition does not imply arbitrary pattern matching.



## 22.5 Explicit Result Categories

An expression computes a value. A module declaration introduces a function or type. The category after the arrow tells the compiler where the expansion belongs.

| Result category | Position | Typical use |
| --- | --- | --- |
| -> expr | Expression | Compute or construct a value |
| -> stmt | Statement in a function | Update an object or print lines |
| -> item | Module declaration | Generate a function, struct, enum or interface |
| -> member | Type member | Generate fields or methods |

Parameter and result categories answer different questions: name:ident accepts a name, while -> item produces module declarations.

<!-- verify-book -->
```chali
$macro FUNCTION(name:ident) -> item
int64 name() { return 42; }
$end_macro
$macro RECORD<T>(name:ident) -> item
struct name { T value; }
$end_macro
$FUNCTION(answer);
$RECORD<int64>(Score);
void main()
{
    Score score;
    score.value = answer();
    println("{}", score.value);
}
```

```text
42
```

FUNCTION places its identifier parameter in a function-name position. Its invocation generates an ordinary answer function. Calling answer() at runtime returns 42; it does not invoke the expander.

RECORD has a separate type parameter T. Its invocation supplies Score as the struct name and int64 as the field type. main declares that type, stores the function result and prints the field.

These invocations belong outside main. An item result cannot be used as the right side of an assignment. Duplicate public names still conflict. Names explicitly supplied through ident form the caller-facing interface; template-local helpers retain hygienic identities.

## 22.6 Generating Members Inside a Type

<!-- verify-book -->
```chali
$macro GET(name:ident, field:ident) -> member
int64 name() { return self.field; }
$end_macro
struct Counter
{
    int64 value;
    $GET(read, value);
}
void main()
{
    Counter counter;
    counter.value = 9;
    println("{}", counter.read());
}
```

```text
9
```

Read the definition first: name supplies the method name and field supplies the accessed field. self belongs to the generated method's receiver.

Counter declares value, then generates read in the same type body. This is equivalent in intent to an ordinary int64 read() { return self.value; } method. main stores 9 and reads it back through that method.

The return type is fixed to int64. Add a type parameter when supporting other types. Generated methods undergo normal field, return-type and borrow checks, and conflict with hand-written methods of the same name. Interface member generation can introduce method declarations or defaults, but cannot add instance fields.

## 22.7 Categorized Variadic Arguments

<!-- verify-book -->
```chali
int64 calls = 0;
int64 next() { calls += 1; return calls; }
$macro SUM(...values:value) -> expr
int64 total = 0;
$for value in values
total += value + value;
$end_for
total
$end_macro
void main()
{
    int64 result = $SUM(next(), next());
    println("{} {}", result, calls);
    println("{}", $SUM());
}
```

```text
6 2
0
```

...values:value collects zero or more value arguments. next increments a counter so that evaluation remains visible.

The first call obtains 1 and 2 in source order. The compile-time loop generates two additions that reuse those values: (1+1)+(2+2)=6, with two actual calls. An empty invocation generates no additions and keeps total at 0.

Changing the category to expr inserts syntax at each use and changes the evaluation contract. Variadic ident, type, stmt and block parameters collect the corresponding fragments. A variadic parameter must end its own group. Empty groups are valid, but the macro must define a useful empty result.

## 22.8 Passing a Pattern and Its Branch

<!-- verify-book -->
```chali
enum Pair
{
    Values(int64 first, int64 second),
    Empty
}
$macro SELECT(value:value, p:pattern, body:block) -> expr
match(value)
{
    p => { yield body; }
    _ => { 0 }
}
$end_macro
void main()
{
    int64 x = 100;
    Pair pair = Pair::Values(7, 8);
    int64 total = $SELECT(pair, Pair::Values(x, y), { yield x + y; });
    println("{} {}", total, x);
}
```

```text
15 100
```

Pair has a variant with two payloads. SELECT evaluates its value once, inserts p before the branch arrow, and executes body on a match. The wildcard produces 0 otherwise.

Pair::Values(x, y) binds payloads 7 and 8. The accompanying block uses these bindings to produce 15. The outer x remains 100: a pattern-local binding does not update it.

Union patterns such as 1 | 2 and interface-payload patterns such as Err::IoError(e) use the normal matching rules from Chapters 10 and 17. For interface filtering in multiple payload positions, keep the type annotations inside the variant parentheses.

## 22.9 Reading Field Metadata

Runtime reflection inspects objects. Here, a concrete type supplies field information during compilation.

<!-- verify-book -->
```chali
struct Record
{
    int64 score;
    static int64 shared;
    String name;
}
void main()
{
    println("{}", $fields<Record>().count);
    println("{}", ($fields<Record>())[0].name);
    println("{}", ($fields<Record>())[1].type.name);
}
```

```text
2
score
std::lang::String
```

shared is static and is excluded from instance fields. The count is therefore 2, in source order: score, then name. The three output statements use the count, the first name and the second field's canonical type name.

Metadata is processed during compilation; the program prints generated scalar values. Fields also expose index, id, type and attributes. Types expose nested fields, and annotations expose their names and arguments. Identity values distinguish entities in the current compilation; do not persist them as stable cross-version identifiers.

A metadata collection is not a runtime List, cannot escape into ordinary runtime storage and requires compile-time indices. This interface covers ordinary struct fields; enum and method reflection are separate capabilities.

## 22.10 Generating Operations by Field Type

<!-- verify-book -->
```chali
struct Record
{
    int64 first;
    String name;
    int64 second;
}
$macro SUM_FIELDS<T>(&value:value) -> expr
int64 total = 0;
$for field in $fields<T>()
$if field.type.name == "int64"
total += $member(value, field);
$end_if
$end_for
total
$end_macro
void main()
{
    Record record;
    record.first = 3;
    record.name = "Ada";
    record.second = 4;
    println("{}", $SUM_FIELDS<Record>(record));
}
```

```text
7
```

Record contains two integers and one string. &value:value explicitly borrows the object read-only.

The macro creates a runtime accumulator. Its compile-time loop visits the three descriptors, and its compile-time condition selects only int64 fields. The name field generates no addition and is not type-checked as an operand of that addition.

$member turns the descriptor into direct field access. This invocation generates operations equivalent to total += value.first; total += value.second;. There is no runtime name-based lookup loop; the additions themselves still execute at runtime.

main initializes 3, Ada and 4, then prints 7. Writing fields requires a mutable reference parameter and writable fields. The descriptor must belong to the object's concrete type; identical spelling in another type does not grant access.

## 22.11 Grouped Iteration and Compile-Time Diagnostics

<!-- verify-book -->
```chali
$macro DOT(...left:value)[...right:value] -> expr
$compile_assert($count(left) == $count(right), "两组长度必须相同");
int64 total = 0;
$for (a, b) in (left, right)
total += a * b;
$end_for
total
$end_macro
void main()
{
    println("{}", $DOT(2, 3)[4, 5]);
    println("{}", $DOT()[]);
}
```

```text
23
0
```

The definition has two ordered groups: parentheses for left and brackets for right. Invocations preserve both shape and order.

$count returns capture counts. $compile_assert checks equality during expansion. The paired loop generates 2*4 and 3*5, producing 23. Empty lists produce 0. The loop itself requires equal lengths rather than truncating either list.

`$DOT(2, 3)[4]` must fail during compilation. A selected $compile_error("reason") directly rejects unsupported input, such as a field type that a JSON generator cannot encode.

$if requires a compile-time condition, such as metadata or a capture count. An ordinary runtime function cannot decide template structure. $match x is ident classifies complete syntax, and $regex x is "[0-9]+" matches complete source text. Neither is the runtime match statement or a runtime regex library.

## 22.12 Reading a Complete Field Example

[macro_fields.chl](../../../example/reflection/macro_fields.chl) applies these operations to printing, validation and JSON. Read it in four sections:

1. quoteJson and boolText handle scalar encoding. The first escapes quotes, backslashes and control bytes. The second emits JSON true and false instead of numeric boolean formatting.
2. PRINT_FIELDS generates output statements. VALIDATE_FIELDS generates type-specific nonempty and nonnegative checks.
3. JSON_FIELDS checks field count and emits keys, punctuation and encoded values. Unsupported types receive a compile-time error.
4. main prepares a Student and calls the three macros. A JSON parser verifies the encoded result; field access does not depend on runtime reflection.

Start with printing and validation before adding encoding. Add a bool field and inspect which statements are generated.

## 22.13 Inspecting Expansions and Diagnosing Errors

In VS Code, use ChaLi's macro expansion preview to inspect one level or the complete expansion. A one-level preview may retain nested macro calls; the complete preview expands them further. Source mappings identify definitions and invocations. Preview output is not a stable source export format.

Check parameter and result categories first, followed by type inference, descriptor ownership and name origin. The temp example below demonstrates the last issue.

### A Hygiene Error

For a macro diagnostic, distinguish definition and call sites, then inspect categories, type arguments, scope and borrowing. The following hygiene example is rejected because an internal temporary is not a caller-visible name.

<!-- verify-book-error: temp -->
```chali
$macro broken(value)
{
    temp.push(value);
    yield temp;
}
$end_macro

void main()
{
    List<int64> temp;
    List<int64> values = $broken(1);
}
```

Broken uses temp without declaring it in the template's scope or receiving its source as a parameter. Main's equally spelled temp belongs to the caller. Hygienic name resolution does not connect those identities automatically.

The example therefore fails during compilation and has no normal runtime output. Repair the interface by explicitly supplying the needed object, or declare a genuine local list inside the macro. A hidden caller variable of the same name is not a repair. Value supplies an element, not a list source.


### Expansion and execution occur at different stages

Expansion composes program structure during compilation; next in the examples still executes at runtime. Knowing an expression's static type does not execute that expression. T:#res obtains information from an expected type supplied by contexts such as assignment or return; it does not observe a runtime result. Supply explicit types when context is insufficient rather than depending on incidental literal inference.

Expanded code undergoes ordinary checking and then reaches the quadruple IR and LLVM backend. Macros cannot remove cleanup obligations, bypass borrowing or make an invalid expression executable. Performance depends on generated operations and LLVM optimization; fewer template lines do not establish less runtime work. For diagnostics, first express the intended operation as ordinary code, verify types and resource relationships, then inspect whether the macro preserves them.


### Summary

Prefer a regular function when it expresses the requirement; choose macros for necessary syntax composition. Identify which arguments become values, which remain fragments, where names originate and how result types are determined.

Expansion does not execute arbitrary user runtime logic or legalize invalid borrowing and resource use. Runtime reflection next addresses metadata-based object access, a different mechanism from compile-time syntax generation.


## 22.14 Single-Line and Parameterless Macros

Use $replace for a one-line template. It shares hygienic AST substitution and needs no $end_macro.

<!-- verify-book -->
```chali
$replace ADD(a, b) a + b
$replace LIMIT 100
void main()
{
    println("{}", $ADD(2, 3) * 10);
    println("{}", $LIMIT + 1);
}
```

```text
50
101
```

ADD is invoked with parentheses. Its complete addition expression produces 5 before multiplication by 10, so the result is 50. LIMIT has no parameter group and is invoked without parentheses. A definition with an empty () group requires that group at the call. Use $macro and $end_macro for multi-line control structures.

## 22.15 Source Text, Names and Locations

<!-- verify-book -->
```chali
module macroTutorial;
$macro SHOW(x)
$stringify(x)
$end_macro
$macro MAKE(a, b)
$concat(a, b)
$end_macro
int64 itemCount = 9;
void main()
{
    println("{}", $SHOW(1 + 2));
    println("{}", $MAKE(item, Count));
    println("{}", $module);
}
```

```text
1 + 2
9
macroTutorial
```

SHOW stringifies the argument's original spelling. It prints 1 + 2 rather than computing 3; comments can also be retained. Reading spelling does not evaluate it as an ordinary value.

MAKE concatenates identifier fragments into itemCount, resolving to the global variable with value 9. $concat must produce one valid identifier. It cannot manufacture arbitrary statements and is not runtime string concatenation.

$module gives the module name. $file and $line provide source information for diagnostics. Paths vary across machines, so do not freeze a local absolute path as portable expected output. Follow source mappings through nested expansion rather than treating preview line numbers as original source positions.

## 22.16 Selecting Templates by Syntax or Text

<!-- verify-book -->
```chali
$macro CLASSIFY(value) -> expr
$match value is ident
1
$else is expr
2
$else
0
$end_match
$end_macro
$macro NUMBER(value) -> expr
$regex value is "[0-9]+"
value
$else
0
$end_regex
$end_macro
void main()
{
    println("{} {}", $CLASSIFY(name), $CLASSIFY(1 + 2));
    println("{} {}", $NUMBER(42), $NUMBER(noSuchFunction()));
}
```

```text
1 2
42 0
```

CLASSIFY checks a complete identifier before a complete expression. name matches the first branch and produces 1. 1 + 2 matches the second and produces 2. This classifies syntax rather than reading a runtime variable named name.

NUMBER fully matches source spelling with an ECMAScript regular expression. The digits 42 remain in the selected template; noSuchFunction() selects 0, so no call to that function is generated. Unselected templates do not enter final semantic checking. This does not permit executing unresolved names.

$match supports ident, expr, type, block and stmt, ordered $else is branches and $end_match. $regex ends with $end_regex and matches the entire spelling: abc123 does not match [0-9]+.

## 22.17 Groups and Nested Forwarding

Ordered groups preserve their delimiter shapes. For ``$macro ADD[a](b){c}`` with body a + b + c, call `$ADD[1](2){3}`, not $ADD(1, 2, 3). Each group can end with its own variadic parameter; ..rest is a compatibility spelling for ...rest.

<!-- verify-book -->
```chali
$macro SUM(...values:value) -> expr
int64 total = 0;
$for value in values
total += value;
$end_for
total
$end_macro
$macro FORWARD(...values:value) -> expr
$SUM(values)
$end_macro
void main()
{
    println("{}", $FORWARD(2, 3, 4));
    $std::lang::assert($FORWARD() == 0);
}
```

```text
9
```

SUM generates additions over captured arguments. FORWARD places the capture list into a nested argument position, forwarding three separate arguments rather than a runtime List. The first call sums 2, 3 and 4 to 9. Empty forwarding retains a valid empty group and produces 0.

Single evaluation for value parameters survives forwarding. Forwarded expr parameters remain syntax and do not automatically become cached values.

$std::lang::assert explicitly qualifies a macro by module. Use visible imported names or qualify them when the source needs to be explicit. Ordinary function calls and macro calls retain their respective call syntax.

## 22.18 Choosing a Form

Start with functions and generics. Choose a macro when repetition concerns syntax positions, declarations or field sets. Decide what each input represents and where the result belongs; encode both decisions in the definition header.

Generated code still undergoes name, type, borrow and resource checks. Start with a field-printing macro before adding serialization rules to a project.

## 22.19 Exercises

1. Compare next call counts with value and expr.
2. Does changing an unannotated parameter change the caller?
3. Does T:#res come from a runtime return value?
4. Why cannot a caller read internal temp?

### Answers and checks

1. Value evaluates once per argument; expr follows placements and execution.
2. No; it changes a copy. Use an explicit mutable reference to change the caller.
3. No; it comes from the statically expected result type.
4. Hygiene prevents capture; internal names do not become caller declarations.

Next: [Chapter 23](23-annotations-reflection.md).
