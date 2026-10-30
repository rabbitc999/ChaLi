# Chapter 16 Errors and propagation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../16-错误处理.md)
<!-- END LANGUAGE NAVIGATION -->

Absence and failure are ordinary outcomes that programs must express. Reusing a special integer or empty string for every case risks confusing valid data with status. Option and Result keep status and payload together in the type.

Start with Value and None in lookup, then inspect interface errors, nested payloads, question-mark propagation and a custom Try protocol. Determine the outer state before the payload type to decide whether execution continues or returns early.

## 16.1 Branch on Option and Result

Searching a score list can find an integer or nothing. Zero cannot distinguish them because it might be valid data. Option<int64> separates Value from None so the caller can respond to the actual state.

FindFirst establishes the two return paths. Then compare unwrap with known presence, unwrapOr with a fallback and match with explicit handling of both alternatives.

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

### Code walkthrough

FindFirst returns Option<int64>: Value contains a found item and None represents absence. A match returns immediately, ending the scan. None is returned only after exhausting the list. The list is passed by value and only read here.

Searching for seven succeeds, so unwrap returns seven. Nine is absent, making unwrapOr(-1) choose its fallback. Searching for three selects the Value arm and binds v. Unwrap requires presence, whereas match and unwrapOr explicitly handle absence. Minus one is a fallback chosen by the caller, not a value stored inside None.


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

### Code walkthrough

ReadDemo does not access a real file. It directly builds Err with an IoError payload inside an Error interface. Both matches inspect that failure, so neither Ok arm executes.

The first uses `Err::std::io::IoError(e)` to state the failure branch and concrete type together. The second uses the supported shorter type pattern. Both extract the same error text with different prefixes, exposing the chosen arms. Err(_) remains necessary for other Error implementations; one IoError pattern does not cover every possible failure.


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

### Code walkthrough

Response::Failed carries a code followed by Error. Its specialized arm binds 503 and requires the second payload to be IoError. The `_ other` fallback binds other error implementations for interface use.

The second part deliberately constructs `Result<Error, Error>::Ok(...)`. Although the success payload has type Error, the outer state remains Ok. It matches the IoError pattern within Ok, not Err. For nested patterns, determine the outer success/failure state or enum variant before checking the inner concrete type. An error-valued payload does not by itself mean the operation failed.


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

### Code walkthrough

Leaf always returns the IoError::Closed failure. In readName, `leaf()?` propagates Err out of the current function, skipping Ok(name). Widening to the Error interface retains the identifiable IoError, allowing main's concrete match.

OptionalError instead returns Value containing an error object. WidenedError's question mark therefore extracts a present value, widens it to Error and wraps it in Value again. Main prints the option-prefixed message. The question mark examines the container branch, not the payload's name: Err and None are the respective early-exit states.


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

### Code walkthrough

Step implements Try<int64, NoneResidual>. Its branch method translates business states into the propagation protocol: Value produces Continue(value), while Empty produces Break(NoneResidual::None). The final panic is not an expected path because the match handles both variants.

In addOne, step? follows that protocol. Value(4) extracts four and returns Option::Value(5). Empty exits early with absence, skipping addition. Main unwraps the former to five and confirms the latter with isNone. Custom question-mark behavior requires a complete, type-compatible protocol rather than merely a similarly named method.


### Summary

Option expresses presence, Result success or failure, and interface payloads describe concrete contents. Unwrap depends on established premises, match exposes handling paths and question-mark propagation returns a noncontinuing state according to its protocol.

Choose deliberately where to supply defaults and where to retain error information. An Error-valued successful payload remains success. Iterator examples next reuse Option to express traversal exhaustion.

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
