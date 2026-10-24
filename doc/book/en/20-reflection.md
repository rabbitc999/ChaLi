# Chapter 20 Annotations and reflection

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../20-注解与反射.md)
<!-- END LANGUAGE NAVIGATION -->

Annotations associate explicit metadata with declarations. Reflection exposes supported type and member information to language programs and host services. Keep the model declaration authoritative instead of maintaining a second hand-written list of the same fields.

```chali
import std::reflect;

@Reflect
struct Student
{
    int64 id;
    String name;
};

void main()
{
    let information = std::reflect::getStructInfo<Student>();
    if (!information.isNone())
    {
        println("Student metadata is available");
    }
}
```

Reflection is built into the normal compiler and runtime configurations. A single program compilation can explicitly request `--no-reflection` when it does not need emitted reflection metadata; omitting a reflection flag does not mean reflection is disabled.

Serialization and model-driven SQL can consume the same metadata, but they remain fallible operations. Check unsupported shapes, invalid input and host errors through their declared result types. An annotation is not permission to trust external data.

Exercise: add a field to the student model and inspect which consumers derive their field lists automatically. Then determine which business validation rules still require explicit application code. Next: [C interoperability](21-c-interop.md).
