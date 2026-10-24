# Chapter 13 Interfaces and runtime polymorphism

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../13-接口与运行期多态.md)
<!-- END LANGUAGE NAVIGATION -->

An interface states a contract shared by several concrete types. Runtime polymorphism lets code call that contract on a value whose actual concrete type is selected at runtime. A generic constraint, in contrast, is checked while compiling the relevant specialization.

Use an interface when a caller needs a common operation rather than knowledge of every concrete implementation. Keep the contract small and meaningful. An interface reference still borrows an object; dispatch does not extend that object's lifetime or transfer ownership.

The standard `Error` interface is a practical example. Concrete errors retain their useful data, such as a position, category or underlying message, while callers can request a readable description through the common contract. Standard-library results use concrete error types implementing `Error`; they do not erase every error into a string.

Do not confuse `Any`, which erases the concrete value type, with an interface contract that exposes specific operations. Choose the representation that gives the consumer the guarantees it needs.

Exercise: design a common display operation for two domain types. Explain which data remains concrete, which operation is dispatched and who owns the object being referenced. Consult the [reference](../../en/04-language-reference.md) for declarations and implementation syntax. Next: [generics](14-generics.md).
