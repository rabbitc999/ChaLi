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
