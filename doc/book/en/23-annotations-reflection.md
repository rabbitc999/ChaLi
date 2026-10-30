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
    Option<std::reflect::StructInfo> info = std::reflect::getStructInfo<Point>();
    println("{}", info.isValue());
    std::reflect::StructInfo s = info.unwrap();
    println("{} {}", s.name, s.fields.len());
}
```

```text
1
program::Point 2
```

@Reflect requests runtime metadata for Point. getStructInfo returns an Option; this registered example can unwrap it to read the name and field count. When registration is not guaranteed, inspect the Option before unwrapping.

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

void main()
{
    Profile profile;
    profile.name = "原名";
    profile.score = 80;
    assert(std::reflect::setField<Profile, String>(&profile, "name", "新名").unwrap());
    assert(std::reflect::getField<Profile, String>(&profile, "score").isErr());
    assert(std::reflect::getField<Profile, int64>(&profile, "missing").isErr());
    int64 score = std::reflect::getField<Profile, int64>(&profile, "score").unwrap();
    println("{} {}", profile.name, score);
}
```

```text
新名 80
```

Reflect exposes Profile's model information. SetField's type arguments specify the model and written value type; `&profile` supplies the object, and name selects its string field. Unwrap and assert confirm the known-success operation here, changing the name to the shown replacement.

The next queries intentionally fail: score is not String, and missing is not a field. IsErr checks both. Querying score correctly as int64 returns eighty. Only the selected field is updated; failed queries do not silently supply defaults. With unknown external input, handle Result explicitly rather than unwrapping blindly.


## 23.4 Serialization boundaries

Parse external data and validate structure and field constraints. An integer age can still be invalid, and an existing class ID can still exceed a teacher’s scope. Select response fields explicitly; reflected password hashes must not be exported automatically.

## 23.5 Metadata and generation responsibilities

Use reflection for repetitive type mappings and keep business decisions at explicit function boundaries. Chapter 25 separates field validation, related records and authorization in a database-backed application.

### Place metadata inside a complete data flow

The Profile example has three independent conditions: the type exposes reflection information, the field name identifies a real field, and the requested value type matches the field. An existing score does not permit reading it as String. A missing field does not establish that score metadata is broken. Preserve these distinctions when handling failures to identify incorrect API calls, external field names or unavailable model information.

Separate external parsing, validation, business-object mutation and output selection. For a grade update, parse an integer, check its range, verify the student and course, confirm the account's authority, and only then update and persist. Reflection reduces mapping code but does not choose validation order or transaction boundaries.

Annotations require an interpreter in application logic. Route defines declaration data; directly calling health does not demonstrate HTTP route registration. A service still needs to consume the information, establish dispatch and handle requests. Declaring metadata and executing business behavior are separate actions.

Runtime reflection and compile-time generation also have different roles. The getField and setField examples request typed access by field name at runtime. They do not demonstrate compiler iteration over heterogeneous fields to generate static code. That capability belongs to macro and compile-time reflection extensions not yet delivered. Use existing derive directives under their current contracts; reflection APIs do not imply arbitrary custom derives. See the [reflection reference (Chinese)](../../19-reflection.md) for interfaces.

### Summary

Metadata still requires interpretation, querying, validation and failure handling before business behavior occurs. Confirm exposed information, field identity and requested type before permitted access. External output additionally requires selected public fields and encoding.

Runtime reflection and compile-time derivation have separate supported scopes. Host integration next makes explicit type, layout and ownership contracts even more important.

## 23.6 Exercises

1. Does an annotation execute a route by itself?
2. How should a wrong-type field read be handled?
3. Why not serialize an entire account structure?
4. List two business checks reflection does not replace.

### Answers and checks

1. No; code must interpret and dispatch it.
2. Check the result rather than treating failure as a value.
3. Sensitive fields could leak; select output fields explicitly.
4. For example class existence and the current teacher’s access scope.

Next: [Chapter 26](26-c-interop.md).
