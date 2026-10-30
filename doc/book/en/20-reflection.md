# Chapter 20 Reflection and serialization

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../20-注解与反射.md)
<!-- END LANGUAGE NAVIGATION -->

Reflection exposes type and field metadata; serialization converts values to external formats. They reduce mapping duplication but do not establish authorization, database relations or input trust.

## 20.1 Expose metadata explicitly

Expose the required type and field metadata before querying it. An annotation stores declaration data that application code must interpret; attaching it does not automatically implement routing or validation.

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

## 20.2 Read and write fields

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

## 20.3 Serialization boundaries

Parse external data and validate structure and field constraints. An integer age can still be invalid, and an existing class ID can still exceed a teacher’s scope. Select response fields explicitly; reflected password hashes must not be exported automatically.

## 20.4 Metadata and generation responsibilities

Use reflection for repetitive type mappings and keep business decisions at explicit function boundaries. Chapter 25 separates field validation, related records and authorization in a database-backed application.

### Place metadata inside a complete data flow

The Profile example has three independent conditions: the type exposes reflection information, the field name identifies a real field, and the requested value type matches the field. An existing score does not permit reading it as String. A missing field does not establish that score metadata is broken. Preserve these distinctions when handling failures to identify incorrect API calls, external field names or unavailable model information.

Separate external parsing, validation, business-object mutation and output selection. For a grade update, parse an integer, check its range, verify the student and course, confirm the account's authority, and only then update and persist. Reflection reduces mapping code but does not choose validation order or transaction boundaries.

Annotations require an interpreter in application logic. Route defines declaration data; directly calling health does not demonstrate HTTP route registration. A service still needs to consume the information, establish dispatch and handle requests. Declaring metadata and executing business behavior are separate actions.

Runtime reflection and compile-time generation also have different roles. The getField and setField examples request typed access by field name at runtime. They do not demonstrate compiler iteration over heterogeneous fields to generate static code. That capability belongs to macro and compile-time reflection extensions not yet delivered. Use existing derive directives under their current contracts; reflection APIs do not imply arbitrary custom derives. See the [reflection reference](../../en/19-reflection.md) for interfaces.

## 20.5 Exercises

1. Does an annotation execute a route by itself?
2. How should a wrong-type field read be handled?
3. Why not serialize an entire account structure?
4. List two business checks reflection does not replace.

### Answers and checks

1. No; code must interpret and dispatch it.
2. Check the result rather than treating failure as a value.
3. Sensitive fields could leak; select output fields explicitly.
4. For example class existence and the current teacher’s access scope.

Next: [Chapter 21](21-c-interop.md).
