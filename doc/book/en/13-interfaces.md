# Chapter 13 Interfaces and runtime polymorphism

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../13-接口与运行期多态.md)
<!-- END LANGUAGE NAVIGATION -->

An interface states a contract shared by several concrete types. Runtime polymorphism lets code call that contract on a value whose actual concrete type is selected at runtime. A generic constraint, in contrast, is checked while compiling the relevant specialization.

Use an interface when a caller needs a common operation rather than knowledge of every concrete implementation. Keep the contract small and meaningful. An interface reference still borrows an object; dispatch does not extend that object's lifetime or transfer ownership.

The standard `Error` interface is a practical example. Standard-library results use `Result<T, Error>` while the owned interface payload retains the concrete error, including positions, categories and messages. Copies own independent concrete values and clean up independently. Interface references continue to borrow their source. Interface types can also be generic arguments in Option and containers.

Match an interface using `Concrete(value)`, with `_` or `_(other)` covering unknown implementations. Nested enum patterns combine the outer tag and the dynamic payload type: `Err(E error)`, `Err(F error)`, then `Err(_)` or `Err(_ other)`. Repeated tags with different payload types are tried in source order. An open interface requires a fallback. The canonical Result success tag is Ok; match patterns also accept OK. The [complete multi-tag example](../../../example/samples/interface_payload_match.chl) uses `Result<A, B>` with both success and failure interface payloads.

Do not confuse `Any`, which erases the concrete value type, with an interface contract that exposes specific operations. Choose the representation that gives the consumer the guarantees it needs.

For a single interface payload, `Err::IoError(error)` is shorthand for `Err(IoError error)`. `IoError(error)` may omit the tag only when the tag is unique and no variant-name collision exists. Multiple payloads retain the tag and parenthesized patterns, such as `Pair(A a, B b)` or `Many(A a, B b, text, count)`. Ordinary payloads can bind directly without a type annotation; an untyped interface binding retains its original interface type.

Exercise: design a common display operation for two domain types. Explain which data remains concrete, which operation is dispatched and who owns the object being referenced. Consult the [reference](../../en/04-language-reference.md) for declarations and implementation syntax. Next: [generics](14-generics.md).
