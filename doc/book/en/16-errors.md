# Chapter 16 Error handling

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../16-错误处理.md)
<!-- END LANGUAGE NAVIGATION -->

`Result<T, E>` represents success with a value of type `T` or failure with an error of type `E`. Check the state before extracting the corresponding value. The `?` operator propagates a compatible error from a function instead of making every caller repeat the same branch.

```chali
Result<int64, Error> divide(int64 left, int64 right)
{
    if (right == 0)
    {
        return Result<int64, Error>::Err(MessageError::from("division by zero"));
    }
    return Result<int64, Error>::Ok(left / right);
}

void main()
{
    let result = divide(9, 3);
    if (result.isErr())
    {
        println("{}", result.error().toString());
        return;
    }
    println("{}", result.unwrap());
}
```

Standard-library errors implement `Error`. Concrete types retain module-specific information; the common interface gives a readable diagnostic. `MessageError::from` is appropriate for application errors that only need a message. A generic API can preserve its error parameter instead of unnecessarily converting it.

Standard-library failure signatures use `Result<T, Error>` while preserving the concrete dynamic error. If IoError implements Error, `?` can propagate `Result<T, IoError>` into a function returning `Result<U, Error>`. Custom Try, TryMove and FromResidual protocols support the same legal residual conversion, and an interface value or reference may provide the source protocol dynamically. Exact receivers take priority; ambiguous conversions are rejected. Option preserves its distinct None residual. Match concrete errors with `Err(IoError error)`, followed by `Err(_)` or `Err(_ other)` for remaining implementations.

Distinguish expected operation failures from violated runtime checks. An invalid file path is an operation failure; accessing an invalid index violates the indexing contract. Do not convert every problem into an unchecked default value.

Exercise: compose two result-returning helpers with `?`, preserving the first failure's original diagnostic. Next: [iterators](17-iterators.md).
