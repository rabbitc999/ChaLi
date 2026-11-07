# Chapter 23 Reflection and serialization

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../23-注解与反射.md)
<!-- END LANGUAGE NAVIGATION -->

Metadata avoids repeated mappings when tools need field names, types or declaration attributes. Annotations describe declarations, reflection queries or accesses objects through supported APIs, and serialization encodes external representations.

Route and Profile separate these responsibilities before field access enters a validation workflow. Application code still decides authorization, ranges and exposed fields even when metadata removes repetitive mapping.

## 23.1 Query one reflected type

<!-- verify-book -->
```chali
import std::reflect;

@Reflect
struct Point
{
    int64 x;
    int64 y;
};

void main()
{
    Option<StructInfo> info = getStructInfo<Point>();
    println("{}", info.isValue());
    match (info) {
        Value(description) => { println("{} {}", description.name, description.fields.len()); }
        None => { println("类型未暴露反射信息"); }
    }
}
```

```text
1
program::Point 2
```

@Reflect requests runtime metadata for Point. getStructInfo returns an Option, and match handles metadata and absence separately. Point is registered here, so Value provides its name and field count; None handles a type without exposed metadata.

## 23.2 Expose metadata explicitly

Attach route information to a function first. Path describes an association and level data an application may interpret; neither is the function's input or output. An annotated function still executes its ordinary body.

An annotation declaration specifies the data's shape and an application supplies its values. The example deliberately calls the function directly to distinguish metadata from execution. Actual routing requires logic that consumes that metadata.

<!-- verify-book -->
```chali
@annotation Route(path: String, level: int32 = 2)

@Route("/health", level: 1)
int64 health()
{
    return 200;
}

void main()
{
    println("{}", health());
}
```

```text
200
```

The annotation declaration defines Route's metadata: path requires a String and level is int32 with default two. The annotation on health supplies a path and explicitly overrides level with one. These values describe the declaration.

Main calls health directly, its body returns 200 and println prints that integer. No HTTP server or metadata-driven request dispatch appears here. The value's resemblance to an HTTP status does not establish that a network response was sent.


## 23.3 Read and write fields

Reflected accesses retain concrete types. Handle missing fields, incompatible types and forbidden writes through their failure results. Reflection does not bypass read-only access or ownership.

<!-- verify-book -->
```chali
import std::reflect;

@Reflect
struct Profile
{
    String name;
    int64 score;
};

Result<bool, Error> updateProfile(Profile mut& profile)
{
    assert(setField<Profile, String>(profile, "name", "新名")?);
    assert(getField<Profile, String>(profile, "score").isErr());
    assert(getField<Profile, int64>(profile, "missing").isErr());
    int64 score = getField<Profile, int64>(profile, "score")?;
    println("{} {}", profile.name, score);
    return Ok(true);
}
void main()
{
    Profile profile;
    profile.name = "原名";
    profile.score = 80;
    match (updateProfile(&profile)) {
        Ok(_) => {}
        Err(error) => { println("{}", error.toString()); }
    }
}
```

```text
新名 80
```

Reflect exposes Profile's model information. SetField's type arguments specify the model and written value type; `&profile` supplies the object, and name selects its string field. updateProfile propagates failures with ? and asserts the successful result, changing the name to the shown replacement. Main matches the result and displays a failed operation.

The next queries intentionally fail: score is not String, and missing is not a field. IsErr checks both. Querying score correctly as int64 returns eighty. Only the selected field is updated; failed queries do not silently supply defaults. With unknown external input, handle Result explicitly rather than unwrapping blindly.


## 23.4 Serialization boundaries

Parse external data and validate structure and field constraints. An integer age can still be invalid, and an existing class ID can still exceed a teacher’s scope. Select response fields explicitly; reflected password hashes must not be exported automatically.

Chapter 22 uses `$std::json::Json<T>(toJson, fromJson);` and @std::json::Field to select external keys and skipped fields. That choice does not grant runtime field-query access: @Reflect remains independent. Successful decoding still needs business and permission checks.

## 23.5 Metadata and generation responsibilities

Use reflection for repetitive type mappings and keep business decisions at explicit function boundaries. Chapter 29 separates field validation, related records and authorization in a database-backed application.

### Place metadata inside a complete data flow

The Profile example has three independent conditions: the type exposes reflection information, the field name identifies a real field, and the requested value type matches the field. An existing score does not permit reading it as String. A missing field does not establish that score metadata is broken. Preserve these distinctions when handling failures to identify incorrect API calls, external field names or unavailable model information.

Separate external parsing, validation, business-object mutation and output selection. For a grade update, parse an integer, check its range, verify the student and course, confirm the account's authority, and only then update and persist. Reflection reduces mapping code but does not choose validation order or transaction boundaries.

Annotations require an interpreter in application logic. Route defines declaration data; directly calling health does not demonstrate HTTP route registration. A service still needs to consume the information, establish dispatch and handle requests. Declaring metadata and executing business behavior are separate actions.

Runtime reflection and compile-time generation operate at different stages. getField and setField request typed access by name at runtime; Chapter 22 uses $fields and $for to inspect fields at compile time, and member macros can use the results to generate ordinary methods and implement interfaces. Generated code follows type, borrowing and resource checks. Runtime field queries still require explicit exposure; compile-time generation does not use runtime field-name lookup. See the [reflection reference (Chinese)](../../19-reflection.md).

### Check for struct metadata first

Check whether a type is registered before accessing its metadata.

<!-- verify-book -->
```chali
import std::reflect;

@Reflect
struct Entry {
    int64 count;
}
void main() {
    println("{}", getStructInfo<Entry>().isValue());
}
```

```text
1
```

### Summary

Metadata still requires interpretation, querying, validation and failure handling before business behavior occurs. Confirm exposed information, field identity and requested type before permitted access. External output additionally requires selected public fields and encoding.

Runtime reflection and compile-time derivation have separate supported scopes. Host integration next makes explicit type, layout and ownership contracts even more important.

Next: [Chapter 24](24-compiler-directives.md).
