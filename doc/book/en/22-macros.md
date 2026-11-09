# Chapter 22 Macros

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
    Pair pair = Values(7, 8);
    int64 total = $SELECT(pair, Values(x, y), { yield x + y; });
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
std::core::String
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

### Chaining Conditions with $elif and $else_if

Use $elif or $else_if within one $if to select among several implementations. The two commands are equivalent, and each takes a compile-time condition. This example generates different JSON field text for String, int64 and bool fields.

<!-- verify-book -->
```chali
struct Record
{
    String name;
    int64 count;
    bool active;
}
$macro JSON_FIELDS<T>(&value:value) -> expr
$compile_assert(($fields<T>()).count > 0, "JSON 生成至少需要一个实例字段");
String result = "{";
$for field in $fields<T>()
$if field.index > 0
result = result + ",";
$end_if
$if field.type == String
result = result + "\"" + field.name + "\": \"" + $member(value, field) + "\"";
$else_if field.type == int64
result = result + "\"" + field.name + "\": " + $member(value, field).toString();
$elif field.type == bool
result = result + "\"" + field.name + "\": " + $member(value, field).toString();
$else
$compile_error("JSON_FIELDS 不支持该字段类型: " + field.type.name);
$end_if
$end_for
result + "}"
$end_macro
void main()
{
    Record record;
    record.name = "Ada";
    record.count = 7;
    record.active = true;
    println("{}", $JSON_FIELDS<Record>(record));
}
```

```text
{"name": "Ada","count": 7,"active": true}
```

The first true branch is selected in source order. Later conditions are not evaluated after selection. Unselected branches are not expanded, so their field operations do not enter runtime type checking. The final $else handles types that matched none of the earlier conditions; adding a float64 field produces a compile-time error naming that type.

The int64 branch uses $else_if and the bool branch uses $elif to demonstrate that both spellings can appear in one chain. Either spelling can replace the other. A single $end_if closes the whole type-selection chain, avoiding three nested $if blocks and separate terminators. At most one $else is allowed, and it must come last. Neither $elif nor $else_if can stand alone or follow $else.

Field iteration, type selection and diagnostics happen during compilation; the generated string concatenations still execute at runtime. The example uses strings that need no escaping. Complete JSON encoding also handles quotes, backslashes and control characters inside strings.

### Child Fields and Recursive JSON Generation

The single field of JsonType<T> exposes type metadata for T. Records recurse through their fields. List and Set produce arrays, while a Map with String keys produces an object. Element and value types come from typeArguments, so collections can contain records or other collections. Empty collections produce [] or {}.

<!-- verify-book -->
```chali
import std::json;

String jsonText(String value) {
    String result = "\"";
    String digits = "0123456789abcdef";
    int64 start = 0;
    int64 i = 0;
    bytes: while (i < value.length()) {
        int64 byte = match (value.byteAt(i)) {
            Value(item) => { (int64)item }
            None => { break bytes; }
        };
        if (byte < 32 || byte == 34 || byte == 92) {
            result += value.slice(start, i);
            if (byte == 34) { result += "\\\""; }
            elif (byte == 92) { result += "\\\\"; }
            else { result += "\\u00" + digits.substring(byte / 16, 1) + digits.substring(byte % 16, 1); }
            start = i + 1;
        }
        i += 1;
    }
    return result + value.slice(start, value.length()) + "\"";
}
String jsonBool(bool value) { if (value) { return "true"; } return "false"; }

struct JsonType<T> { T item; }

$macro JSON_VALUE(T:type)[value:value] -> expr
String result = "";
$for info in $fields<JsonType<T>>()
$if info.type == String
result = jsonText(value);
$elif info.type == int64
result = value.toString();
$elif info.type == bool
result = jsonBool(value);
$elif info.type == List<?> || info.type == Set<?>
result = "[";
int64 i = 0;
while (i < value.len()) {
    if (i > 0) { result += ","; }
    let item = value.at(i);
    result += $JSON_VALUE(info.typeArguments[0])[item];
    i += 1;
}
result += "]";
$elif info.type == Map<String, ?>
result = "{";
int64 i = 0;
while (i < value.len()) {
    if (i > 0) { result += ","; }
    let item = value.valAt(i);
    result += jsonText(value.keyAt(i)) + ":" + $JSON_VALUE(info.typeArguments[1])[item];
    i += 1;
}
result += "}";
$elif info.type == Map
$compile_error("JSON Map 的键必须是 String: " + info.type.name);
$else
result = "{";
$for field in info.chlids
$if field.index > 0
result += ",";
$end_if
result += jsonText(field.name) + ":" + $JSON_VALUE(field.type)[$member(value, field)];
$end_for
result += "}";
$end_if
$end_for
result
$end_macro

struct Empty {}
struct Leaf { int64 count; String text; bool enabled; Empty empty; }
struct Middle<T> { T inner; }
struct Root {
    Middle<Leaf> middle;
    List<int64> items;
    Set<String> unique;
    Map<String, List<Leaf>> byName;
    List<int64> emptyList;
    Set<int64> emptySet;
    Map<String, bool> emptyMap;
}
Result<T, MessageError> requireValue<T>(Option<T> value, String description) {
    match (value) {
        Value(item) => { return Ok(item); }
        None => {
            MessageError error = MessageError::from(description);
            return Err(error);
        }
    }
}
Result<bool, Error> verifyJson(String encoded) {
    let parsed = parse(encoded)?;
    let items = requireValue(parsed.find("items"), "missing items")?;
    let names = requireValue(parsed.find("byName"), "missing byName")?;
    let bucket = requireValue(names.find("b\"ox"), "missing bucket")?;
    let item = requireValue(items.at(1), "missing item")?;
    assert(requireValue(item.asInt(), "item must be int")? == 8);
    let leaf = requireValue(bucket.at(0), "missing leaf")?;
    let count = requireValue(leaf.find("count"), "missing count")?;
    assert(requireValue(count.asInt(), "count must be int")? == 42);
    return Ok(true);
}
Result<bool, Error> demonstrateJson() {
    Root value;
    value.middle.inner.count = 42;
    value.middle.inner.text = "a\"\\\n";
    value.middle.inner.enabled = true;
    value.items.push(5);
    value.items.push(8);
    value.unique.add("only");
    value.unique.add("only");
    List<Leaf> bucket;
    bucket.push(value.middle.inner);
    value.byName.put("b\"ox", bucket);
    String encoded = $JSON_VALUE(Root)[value];
    assert(verifyJson(encoded)?);
    println("{}", encoded);
    assert(value.items.len() == 2 && value.unique.len() == 1 && value.byName.len() == 1);
    return Ok(true);
}
void main() {
    match (demonstrateJson()) {
        Ok(_) => {}
        Err(error) => { println("{}", error.toString()); }
    }
}
```

```text
{"middle":{"inner":{"count":42,"text":"a\"\\\u000a","enabled":true,"empty":{}}},"items":[5,8],"unique":["only"],"byName":{"b\"ox":[{"count":42,"text":"a\"\\\u000a","enabled":true,"empty":{}}]},"emptyList":[],"emptySet":[],"emptyMap":{}}
```

This general helper keeps Err(error): T can itself be MessageError, making the success and error payload types identical. The explicit variant selects failure in that case. Naming the error avoids nested constructors; other error returns can still use a direct payload when the target has a unique best match.

requireValue maps a missing required field to MessageError, and verifyJson uses ? to propagate parsing, missing-field, and type failures. Option and Result carry different residual states, so mapping absence requires an explicit application error. Main matches the final result.

Field names, map keys, and string values share one escaping function for quotes, backslashes, and control bytes. List preserves element order, and Set follows its iteration order. Map keys must be String.

For a single level, write $for child in field.chlids and use $member($member(value, field), child) to access the child. To dispatch on a generic family, match $if field.type == Map<String, ?> and read the value type through field.typeArguments[1], or use $for arg in field.typeArguments to visit the concrete types. Use Map as shorthand for Map<?, ?> when only the family matters.

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

## 22.12 Reading Enum Variants and Payloads

Fields describe object data; variants describe an enum's alternatives. Print the declaration structure without creating a Message value.

<!-- verify-book -->
```chali
enum Message
{
    Quit,
    Text(String text),
    Move(int64 x, int64 y)
};
$macro DESCRIBE<T>() -> stmt
$for variant in $variants<T>()
println("{}: {}", variant.name, variant.payloads.count);
$for payload in variant.payloads
println("  {}: {}", payload.name, payload.type.name);
$end_for
$end_for
$end_macro
void main()
{
    $DESCRIBE<Message>();
}
```

```text
Quit: 0
Text: 1
  text: std::core::String
Move: 2
  x: int64
  y: int64
```

The declaration has three variants. Quit has no payload, Text has one String and Move has two int64 values. Payload names and order come from the declaration.

DESCRIBE obtains descriptors with `$variants<T>()`. The outer compile-time loop emits one output statement for each variant. The inner loop visits that variant's payloads and emits their names and types. Quit has an empty payload collection, so it produces no indented lines.

`payload.type.name` is canonical, hence std::core::String for text. Generic enums substitute concrete arguments before exposing payload types: a T payload in MyBox<int16> has type int16. Descriptors do not read an enum object; use ordinary match to inspect a Message value.

main invokes the macro. Iteration happens during compilation; the generated println statements execute at runtime. Variants and payloads expose name, index and id. Variants additionally expose payloads, and payloads expose type. A payload is not a struct field and cannot be passed to `$member`.

## 22.13 Querying Methods and Interfaces

Method metadata describes declared operations; interface metadata describes inherited contracts.

<!-- verify-book -->
```chali
interface Base
{
    int64 read();
}
interface Named: Base
{
    String label();
}
struct Counter: Named
{
    int64 read()
    {
        return 7;
    }
    String label()
    {
        return "counter";
    }
}
void main()
{
    println("{}", $methods<Counter>().count);
    println("{}", ($methods<Counter>())[0].name);
    println("{}", $interfaces<Counter>().count);
    println("{}", ($interfaces<Counter>())[0].isDirect);
    println("{}", ($interfaces<Counter>())[1].isDirect);
}
```

```text
2
read
2
1
0
```

Base declares read. Named inherits Base and adds label. Counter implements both methods and therefore satisfies Named and its base.

The first two output statements query methods: the count is 2 and the first source method is read. The parameters collection excludes the implicit receiver; each parameter exposes name, type, hasDefault and isVariadic. Methods also expose result, declaringType, isStatic, isConstructor, hasBody and isInherited. Compiler-supplied toString is excluded.

The remaining statements inspect interfaces. Counter explicitly lists Named and inherits Base through it, so the count is 2. Traversal is source-order depth-first, deduplicating repeated bases. Named is direct and Base is inherited. Boolean output is 1 and 0, as elsewhere in this book.

Use these collections in `$for method in $methods<T>()` or `$for contract in $interfaces<T>()`. A nested `$for parameter in method.parameters` reads parameters. Queries do not create callable machine pointers. Indices must be compile-time nonnegative constants. Collections and descriptors cannot escape into runtime storage; id only distinguishes identities within the current compilation.

### Receivers and associated types

The interface declaring a method and the object receiving a call are separate pieces of information. An associated type likewise distinguishes an interface requirement from an implementation's definition.

<!-- verify-book -->
```chali
interface Source
{
    type Item;
    Item get();
}
struct Numbers: Source
{
    type Item = int64;
    Item get() { return 7; }
}
void main()
{
    println("{} {}", ($interfaces<Numbers>())[0].associatedTypes[0].name,
        ($interfaces<Numbers>())[0].associatedTypes[0].type.name);
    println("{}", ($methods<Numbers>())[0].receiverType.name);
    assert(($interfaces<Numbers>())[0].associatedTypes[0].sourceLine > 0);
}
```

```text
Item int64
Numbers
```

Source requires an Item definition and uses it as get's result type. Numbers supplies int64, so get returns an integer.

Main first reads the associated type from Numbers' first implemented interface: name is Item and type.name is int64. An interface requirement without an implementation has hasDefinition set to false. Test it in a compile-time condition before reading type. Associated types also support `$for item in contract.associatedTypes`.

The second statement reads get's receiver, producing Numbers. An inherited default method retains the queried receiverType and its original declaringType. Static factories have no caller receiver; check hasReceiver before querying receiverType.

Methods also expose hasMutableParameters, returnsMutableReference, usesUnsafe and hasPropagation. The first pair describes writable references in the signature. The latter pair observes direct unsafe and question-mark propagation syntax in the current body, excluding nested functions and closures. They do not prove an entire call chain free of side effects. Generated code still undergoes ordinary type, borrow and resource checks.

The final assertion confirms that Item retains its declaration line. Descriptors expose sourceFile, sourceLine and sourceColumn for diagnostics; fields, payloads, methods and parameters each have their own location. Generic instances retain template lines, and cross-module queries retain the original module's file. The preview's Metadata declaration button uses this provenance even after name or id folds into an ordinary constant.

## 22.14 Reading a Complete Field Example

[macro_fields.chl](../../../example/reflection/macro_fields.chl) applies these operations to printing, validation and JSON. Read it in four sections:

1. quoteJson and boolText handle scalar encoding. The first escapes quotes, backslashes and control bytes. The second emits JSON true and false instead of numeric boolean formatting.
2. PRINT_FIELDS generates output statements. VALIDATE_FIELDS generates type-specific nonempty and nonnegative checks.
3. JSON_FIELDS checks field count and emits keys, punctuation and encoded values. Unsupported types receive a compile-time error.
4. main prepares a Student and calls the three macros. A JSON parser verifies the encoded result; field access does not depend on runtime reflection.

Start with printing and validation before adding encoding. Add a bool field and inspect which statements are generated.

## 22.15 Keeping Control Targets at Their Source

A break passed to a macro still targets the caller's loop. A loop introduced by the template must not intercept it.

<!-- verify-book -->
```chali
$macro RUN_ONCE(body:block) -> stmt
int64 round = 0;
while (round < 1)
{
    round += 1;
    body;
}
$end_macro
void main()
{
    int64 visited = 0;
    loop
    {
        visited += 1;
        $RUN_ONCE({ break; });
        visited += 100;
        break;
    }
    println("{}", visited);
}
```

```text
1
```

RUN_ONCE takes a complete block fragment. Its private round variable controls one while iteration, then the template places body inside the loop. The private binding does not accidentally resolve to a caller variable with the same spelling.

main creates the outer loop and increments visited from 0 to 1. The supplied break belongs to that loop, so the addition of 100 is never executed. The result is 1. Selecting the physically nearest loop after expansion would instead exit the template's while and change the result.

Continue follows the same origin rule. A return or ? in a supplied fragment belongs to the caller's function; yield belongs to the caller's existing value exit. Template caches and value blocks do not intercept them. Loops and functions declared inside the supplied fragment retain their own local boundaries.

A template's ordinary control statements first search its loops or value exits. An explicit `-> stmt` macro may transfer control to the caller when there is no local target. Legacy macros without a result category retain that use. Named exits resolve within their own syntax origin. An explicit `-> expr` template stops at its own boundary: it cannot itself return or propagate errors into the caller, nor escape the expression through break or continue. Returns inside functions declared by that template still belong to those functions.

Explicit caller fragments may leave the caller, but cannot cross a newly generated function and then return to the original function. Exited scopes still execute defer actions and owned-value cleanup. Return and propagation values retain ordinary type, borrowing and move checks. Read the statement category, fragment categories and invocation's exit condition together before examining expansion nesting.

## 22.16 Inspecting Expansions and Diagnosing Errors

In VS Code, use ChaLi's macro expansion preview to inspect one level or the complete expansion. A one-level preview may retain nested macro calls; the complete preview expands them further. Source mappings identify definitions and invocations. Preview output is not a stable source export format.


A generic directive may have several concrete instances. Each shows its field types and generated methods while retaining the original directive as its source. One-level and complete views can differ after further name, type and macro resolution. Previewing does not execute user code again.
The summary lists direct effects: result category, introduced template bindings, shared or mutable parameter borrows, and control exits. Full expansion distinguishes caller control from local control using actual targets; one-step previews list unresolved targets separately. Creating a closure does not execute its body, so that body's return is excluded from the current macro's exits. The summary does not analyze effects along an entire call chain. An empty list still requires ordinary signature, type and borrow checks.

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

```text
引用了没声明的变量：temp
```

Broken uses temp without declaring it in the template's scope or receiving its source as a parameter. Main's equally spelled temp belongs to the caller. Hygienic name resolution does not connect those identities automatically.

The example therefore fails during compilation and has no normal runtime output. Repair the interface by explicitly supplying the needed object, or declare a genuine local list inside the macro. A hidden caller variable of the same name is not a repair. Value supplies an element, not a list source.


### Counterexample: writing the caller’s matching name

The template writes to total directly. The caller’s matching declaration supplies no binding to it. Pass the target explicitly through a mutable reference parameter such as mut &target. The diagnostic excerpt below uses the compiler’s Chinese language setting.

<!-- verify-book-error: check -->
```chali
$macro ADD(amount:value) -> stmt
total = total + amount;

$end_macro

void main() {
    int64 total = 7;
    $ADD(2);
}
```

```text
给没声明的变量赋值：total
```

### Expansion and execution occur at different stages

Expansion composes program structure during compilation; next in the examples still executes at runtime. Knowing an expression's static type does not execute that expression. T:#res obtains information from an expected type supplied by contexts such as assignment or return; it does not observe a runtime result. Supply explicit types when context is insufficient rather than depending on incidental literal inference.

Expanded code undergoes ordinary semantic checking and then reaches the LLVM native backend. Macros cannot remove cleanup obligations, bypass borrowing or make an invalid expression executable. Performance depends on generated operations and LLVM optimization; fewer template lines do not establish less runtime work. For diagnostics, first express the intended operation as ordinary code, verify types and resource relationships, then inspect whether the macro preserves them.


## 22.17 Single-Line and Parameterless Macros

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

## 22.18 Source Text, Names and Locations

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


### Target platform and boolean build options

`$system` and `$cpu` are bare string constants describing the target program's operating system and architecture. Windows, macOS and Linux use `"win"`, `"mac"` and `"linux"`; common architecture names are `"amd64"`, `"arm64"` and `"arm32"`. Cross builds use the selected target rather than the compiler host. CPU tuning through `--cpu` does not change the architecture macro. Name normalization does not add native output targets; consult the LLVM build chapter for supported targets.

```csharp
void main() {
    println("system={}, cpu={}", $system, $cpu);
    println("A={}, B={}", $build[A], $build["B"]);
}
```

`chali run main.chl -B A -BB` defines both names. A project can also declare:

```toml
[build]
build_option = ["A", "B"]
```

CLI and manifest names form a union. Names are case sensitive; duplicates have no additional effect. `$build[A]` and `$build["A"]` are equivalent: the bare name is a literal flag name, not a variable lookup. Defined names yield `true`, undefined names yield `false`. Values such as `-BA=false`, numbers and strings are unsupported. Names must be nonempty and contain only ASCII letters, digits, underscores, dots or hyphens; quote names containing dots or hyphens.

Inside macro templates, use `$if $build[A]`, `$else`, `$end_if` or `$if ($system == "win")` to select code. Ordinary expressions can read the same constants. `$build` accepts a literal name or string, not a computed runtime key. `-I` keeps its module-search-directory meaning.

See [Chapter 11](11-modules-projects.md) for the complete manifest context and [Chapter 27](27-aot-export.md) for target and link settings.

## 22.19 Selecting Templates by Syntax or Text

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

## 22.20 Groups and Nested Forwarding

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
    $assert($FORWARD() == 0);
}
```

```text
9
```

SUM generates additions over captured arguments. FORWARD places the capture list into a nested argument position, forwarding three separate arguments rather than a runtime List. The first call sums 2, 3 and 4 to 9. Empty forwarding retains a valid empty group and produces 0.

Single evaluation for value parameters survives forwarding. Forwarded expr parameters remain syntax and do not automatically become cached values.

$std::core::assert explicitly qualifies a macro by module. Use visible imported names or qualify them when the source needs to be explicit. Ordinary function calls and macro calls retain their respective call syntax.

### Interface derivation in a member macro

This member macro generates a method that counts instance fields. The static loop expands at compile time, and the method executes the generated statements.

<!-- verify-book -->
```chali
interface Count
{
    int64 count();
}

$macro Count<T>(method:ident) -> member
$implement Count;
int64 method()
{
    int64 result = 0;
    $for field in $fields<T>()
    result += 1;
    $end_for
    return result;
}
$end_macro

struct Record
{
    int64 number;
    String text;
    $Count<Record>(count);
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

The `Count` interface requires an instance method returning an integer. The macro’s `$implement Count;` adds that interface to the host type. The call `$Count<Record>(count);` supplies the type `Record` and the method name `count` explicitly. The `method:ident` parameter uses the caller’s name for the generated method, while local variables inside the macro remain hygienic.

The static loop emits an increment for each of the two instance fields. It reads no field values and copies no string. The call through an interface value verifies that the generated method satisfies the interface signature.


In a generic member macro, `$implement Count<T>;` uses the macro's type argument, and `$implement Count<String>;` fixes it to String, provided the declared Count interface accepts a type parameter. The host must provide all required methods after expansion.


### Adding an interface from a member macro

The macro registers `Read<U>` with the host and generates the required method. At `$AUTO<int64>(read, label)`, the interface becomes `Read<int64>`. Assignments to `Read<int64>` and `Named` exercise ordinary interface dispatch.

<!-- verify-book -->
```chali
interface Read<T> { T read(); }
interface Named { String label(); }
$macro AUTO<U>(reader:ident, labeler:ident) -> member
$implement Read<U>;
$implement Named;
U reader() { return self.value; }
String labeler() { return "auto"; }
$end_macro
struct Entry {
    int64 value;
    $AUTO<int64>(read, label);
}
void main() {
    Entry entry;
    entry.value = 42;
    Read<int64> reader = entry;
    Named named = entry;
    println("{} {}", reader.read(), named.label());
}
```

```text
42 auto
```

### Inspecting values with Debug

Debug uses `($type<T>()).qualifiedName` to display the declaring module and concrete generic arguments. It omits the default `program::` prefix.

The standard Debug interface declares String debug(). Call `$std::macro::Debug<T>(debug);` in the body of a structure or enumeration to generate the implementation from its instance fields or active variant payloads.

<!-- verify-book -->
```chali
import std::macro;

struct Reading
{
    String sensor;
    int64 value;
    $std::macro::Debug<Reading>(debug);
}

enum Event
{
    Idle,
    Updated(Reading reading, bool saved);
    $std::macro::Debug<Event>(debug);
};

void main()
{
    Reading reading;
    reading.sensor = "温度";
    reading.value = 23;
    println("{}", reading.debug());
    Event event = Updated(reading, true);
    println("{}", event.debug());
}
```

```text
Reading { sensor: "温度", value: 23 }
Event::Updated(reading: Reading { sensor: "温度", value: 23 }, saved: true)
```

The first declaration generates Reading.debug in field order. Strings appear in double quotes, integers as numbers, and static fields are excluded because they belong to the type rather than the inspected instance.

Event has an empty variant and a variant with two payloads. Updated displays only its active reading and saved values, calling Reading.debug for the nested value. Idle produces Event::Idle without reading any other payload. Additional payloads retain declaration order.

main constructs the reading and then the event. Each println receives an ordinary String returned by debug. Generated methods follow the same type, borrowing and cleanup rules as handwritten methods; they do not walk runtime field tables.

Newlines, carriage returns, tabs, quotes and backslashes are escaped. Other ASCII control bytes use `\xNN`; complete UTF-8 spans preserve Chinese text and emoji. Characters use single quotes and booleans use true or false. The default program:: module prefix is omitted, while other module names and concrete generic identities distinguish types.

Nested user types must implement Debug. A handwritten implementation can omit sensitive fields or shorten output. Derivation rejects an existing debug method instead of replacing it. Arrays, references, function values and containers without a Debug contract are rejected at compile time. An owned resource can provide a borrowing Debug method without becoming copyable.

Debug output is for inspection, has no cross-version format guarantee, and is not a JSON or persistence format. User-facing error descriptions still use Error.toString. The generated method follows the ordinary interface, type and borrow checks.

### Generate equality, hashing and cloning with member macros

Equality, cloning and hashing often follow a model’s field structure. Standard member macros generate the methods for those interfaces.

Point declares the three interfaces and calls the corresponding member macros in its type body. The program compares a copy with its source, then changes one field and compares them again.

<!-- verify-book -->
```chali
import std::macro;

struct Point
{
    int64 x;
    int64 y;
    String label;
    $std::macro::Equal<Point>(leftEqual);
    $std::macro::Hash<Point>(hash);
    $std::macro::Clone<Point>(clone);
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

The three member macros add the Equal, Hash, and Clone interfaces to Point. All three instance fields participate in the generated methods. Origin starts with coordinates zero, zero and label O. Cloning preserves those contents, making equal and sameHash true. The latter compares hash results rather than checking a fixed numeric hash.

Changing copy.label to O2 changes overall equality even though coordinates still match. AfterChange becomes false while origin.label stays O, demonstrating independent mutation. Equal objects require consistent hashes, but equal hashes alone do not prove equality or provide persistent identity.

Fields here mean instance fields. Static fields belong to shared type state and do not participate in these derives. Chinese and emoji labels are hashed as complete UTF-8 bytes; positive and negative zero also hash consistently for float32/64 fields. Nested types use their own protocols: if handwritten equality compares only id, handwritten hashing needs the same meaning.


### Generating comparison and hashing for nested values

<!-- verify-book -->
```chali
import std::macro;

struct BookKey
{
    String label;
    int64 id;
    $std::macro::Equal<BookKey>(leftEqual);
    $std::macro::Hash<BookKey>(hash);
    $std::macro::Clone<BookKey>(clone);
}

struct BookRecord
{
    BookKey key;
    $std::macro::Equal<BookRecord>(leftEqual);
    $std::macro::Hash<BookRecord>(hash);
    $std::macro::Clone<BookRecord>(clone);
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

struct Profile
{
    @std::json::Field(name:"student_id")
    int64 id;
    String name;
    @std::json::Field(skip:true)
    String secret;
    $Json<Profile>(toJson, fromJson);
}

void main()
{
    Profile profile;
    profile.id = 7;
    profile.name = "Ada";
    profile.secret = "internal";
    match (profile.toJson()) {
        Ok(encoded) => { println("{}", encoded); }
        Err(error) => { println("{}", error.toString()); return; }
    }

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

## 22.21 Choosing a Form

Start with functions and generics. Choose a macro when repetition concerns syntax positions, declarations or field sets. Decide what each input represents and where the result belongs; encode both decisions in the definition header.

Generated code still undergoes name, type, borrow and resource checks. Start with a field-printing macro before adding serialization rules to a project.

### Summary

Prefer a regular function when it expresses the requirement; choose macros for necessary syntax composition. Identify which arguments become values, which remain fragments, where names originate and how result types are determined.

Expansion does not execute arbitrary user runtime logic or legalize invalid borrowing and resource use. Runtime reflection next addresses metadata-based object access, a different mechanism from compile-time syntax generation.

Next: [Chapter 23](23-annotations-reflection.md).
