# Chapter 8 Resources and scopes

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../08-资源与作用域.md)
<!-- END LANGUAGE NAVIGATION -->

Owned resources are cleaned up when their lifetime ends. Scope is therefore part of program behavior: leaving a block through normal completion, `return` or an error path must preserve the resource protocol.

Use `heap T` or `heap T(arguments)` for the language's heap construction form. An owning handle is different from a borrowed reference. Moving a handle transfers responsibility; copying is only available when the type defines legal copy behavior.

`defer` expresses an action to perform on scope exit. `scope` and `drop` provide the documented lifetime controls. Use them deliberately and avoid manually destroying something that another owning value will also destroy. The compiler's lifetime analysis and runtime live-memory checks cover different parts of this contract.

For files, sockets and database connections, inspect opening errors before using the resource. Keep the resource in the smallest useful scope. Borrow it into helpers rather than constructing competing owners around one external handle.

Exercise: open a file, inspect a failure without unwrapping it, and ensure that every successful path closes or relinquishes the resource correctly. See [error handling](16-errors.md) for propagating failures and [the Chinese chapter](../08-资源与作用域.md) for the scope-control examples. Next: [structures](09-structures.md).
