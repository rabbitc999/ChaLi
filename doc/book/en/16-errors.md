# Chapter 16 Errors and propagation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../16-错误处理.md)
<!-- END LANGUAGE NAVIGATION -->

Absence and failure are distinct. Option expresses presence, Result expresses success or error, and an error interface unifies boundaries while retaining concrete errors for recovery.

## 16.1 Branch on Option and Result

A default-constructed Option follows its default variant and value; use Option<T>::None for explicit absence. Match Value/None before reading an optional value and Ok/Err before reading a result. Use unwrap only after establishing its precondition, not as external input validation.

<!-- verify-book -->
```chali
Option<int64> findFirst(List<int64> xs, int64 target)
{
    for (int64 x in xs)
    {
        if (x == target)
        {
            return Value(x);
        }
    }
    return None;
}

void main()
{
    List<int64> xs;
    xs.push(3);
    xs.push(7);
    println("{}", findFirst(xs, 7).unwrap());
    println("{}", findFirst(xs, 9).unwrapOr(-1));
    match (findFirst(xs, 3))
    {
        Value(v) => { println("found = {}", v); }
        None => { println("no"); }
    }
}
```

```text
7
-1
found = 3
```

## 16.2 Match an interface inside an enum

For Result<String, Error>, bind ordinary success data with Ok(text). Refine an error implementation with Err(IoError e), or use Err::IoError(e) for a single-payload variant. When resolution is unique and does not conflict with variants or candidates, IoError(e) may omit the outer variant. Err(_) or Err(_ other) handles remaining errors.

These abbreviations apply only to single-payload variants. With multiple payloads retain the outer variant and explicit type annotations inside its parentheses.

<!-- verify-book -->
```chali
import std::io;
Result<String, Error> readDemo()
{
    return Result<String, Error>::Err(std::io::IoError::ReadFailed("demo"));
}
void main()
{
    let result = readDemo();
    match (result)
    {
        Ok(text) => { println("{}", text); }
        Err::std::io::IoError(e) => { println("qualified: {}", e.toString()); }
        Err(_) => { println("other error"); }
    }
    match (result)
    {
        Ok(_) => { println("ok"); }
        std::io::IoError(e) => { println("short: {}", e.toString()); }
        Err(_) => { println("other error"); }
    }
}
```

```text
qualified: 读取失败：demo
short: 读取失败：demo
```

With two payloads keep Failed(code, Type value). When both Ok and Err can carry the same implementation, explicit variants distinguish the alternatives without ambiguity.

<!-- verify-book -->
```chali
import std::io;
enum Response
{
    Pending,
    Failed(int64 code, Error error)
};
void main()
{
    let response = Response::Failed(503, std::io::IoError::Closed);
    match (response)
    {
        Response::Pending => { println("pending"); }
        Response::Failed(code, std::io::IoError e) => { println("{}: {}", code, e.toString()); }
        Response::Failed(code, _ other) => { println("{}: {}", code, other.toString()); }
    }
    Result<Error, Error> result = Result<Error, Error>::Ok(std::io::IoError::Closed);
    match (result)
    {
        Ok(std::io::IoError e) => { println("ok: {}", e.toString()); }
        Ok(_) => { println("other success"); }
        Err(std::io::IoError e) => { println("err: {}", e.toString()); }
        Err(_) => { println("other failure"); }
    }
}
```

```text
503: 文件已关闭
ok: 文件已关闭
```

## 16.3 Promote interface implementations while propagating

On success, ? obtains the inner value; on failure or absence it returns through the propagation protocol. If IoError implements the required Error, Result<..., IoError> can propagate into Result<..., Error>. Option interface payloads and custom propagation must likewise satisfy their target types and protocol constraints; ? does not bypass typing.

The concrete IoError from leaf propagates through ? into Error. Option success payloads can also become interface values; its absence path follows the Option propagation protocol.

<!-- verify-book -->
```chali
import std::io;
Result<String, std::io::IoError> leaf()
{
    return Result<String, std::io::IoError>::Err(std::io::IoError::Closed);
}
Result<String, Error> readName()
{
    String name = leaf()?;
    return Result<String, Error>::Ok(name);
}
Option<std::io::IoError> optionalError()
{
    return Option<std::io::IoError>::Value(std::io::IoError::Closed);
}
Option<Error> widenedError()
{
    let error = optionalError()?;
    return Option<Error>::Value(error);
}
void main()
{
    match (readName())
    {
        Ok(name) => { println("{}", name); }
        Err(std::io::IoError e) => { println("result: {}", e.toString()); }
        Err(_) => { println("other error"); }
    }
    match (widenedError())
    {
        Value(std::io::IoError e) => { println("option: {}", e.toString()); }
        Value(_) => { println("other value"); }
        None => { println("absent"); }
    }
}
```

```text
result: 文件已关闭
option: 文件已关闭
```

## 16.4 Custom propagation and recovery boundaries

A custom Try protocol defines both successful output and residual conversion. Recover where a useful decision can be made: retry file operations, report field errors, and display unknown implementations through their common interface. Early returns still follow cleanup rules.

<!-- verify-book -->
```chali
enum Step: Try<int64, NoneResidual>
{
    Value(int64 value), Empty;
    TryBranch<int64, NoneResidual> branch()
    {
        match (self)
        {
            Step::Value(value) =>
            {
                return TryBranch<int64, NoneResidual>::Continue(value);
            }
            Step::Empty =>
            {
                return TryBranch<int64, NoneResidual>::Break(NoneResidual::None);
            }
        }
        panic("unreachable");
    }
};
Option<int64> addOne(Step step)
{
    int64 value = step?;
    return Option<int64>::Value(value + 1);
}
void main()
{
    println("{} {}", addOne(Step::Value(4)).unwrap(), addOne(Step::Empty).isNone());
}
```

```text
5 1
```

## 16.5 Exercises

1. Name an Err(_) fallback.
2. When may the outer Err be omitted?
3. How do you refine an interface in a two-payload branch?
4. Does ? turn an unrelated type into Error?

### Answers and checks

1. Use Err(_ other) and call other.toString().
2. Only for one payload with unique, nonconflicting resolution.
3. Keep the variant, for example Failure(code, IoError error).
4. No; implementation and propagation requirements still apply.

Next: [Chapter 17](17-iterators.md).
