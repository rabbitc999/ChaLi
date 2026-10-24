# Chapter 6 Value semantics

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../06-值语义.md)
<!-- END LANGUAGE NAVIGATION -->

Assignment copies a value. For an owning aggregate, copying means independent owned storage rather than an invisible shared mutable object. This makes local reasoning easier: changing a copy should not unexpectedly change its source.

```chali
void main()
{
    String original = "cat";
    String copied = original;
    copied[0] = 'b';
    println("{} {}", original, copied);
}
```

The result is `cat bat`. A reference, by contrast, explicitly borrows existing storage. Choose a copy for independent state and a reference for an operation on an existing value. Do not choose a reference solely to imitate the behavior of a different language.

`move` transfers an owning value where supported. Write `move value`; it is an operator. A pointer or reference operand is dereferenced by one level according to the language rules. A moved value must not subsequently be used as though it still owned its original resource.

Some resource types are noncopyable. Their creation, movement and destruction protocols protect external resources such as files and database connections. Exercise: repeat the string experiment with a list, then use an explicit mutable reference to intentionally change the original. Next: [references](07-references.md).
