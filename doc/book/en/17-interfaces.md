# Chapter 17 Interfaces and concrete-type matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../17-接口与运行期多态.md)
<!-- END LANGUAGE NAVIGATION -->

Different layouts can offer the same operations. Interfaces collect the behavior a caller needs, allowing one processing function to work across implementations. Shapes and errors provide concrete examples.

First dispatch through Shape references, then identify concrete Error types when handling genuinely differs. Direct dispatch shares behavior; concrete-type matching supports implementation-specific handling.

## 17.1 Implement a common contract

<!-- verify-book -->
```chali
interface Shape
{
    int64 area();
    String describe();
};

struct Circle: Shape
{
    int64 r;

    int64 area()
    {
        return 3 * self.r * self.r;
    }
    String describe()
    {
        return "圆";
    }
};

void main()
{
    Circle c;
    c.r = 2;
    println("{}，面积 {}", c.describe(), c.area());
}
```

```text
圆，面积 12
```

Shape declares area and describe. Circle supplies both methods and a radius field. The formula uses 3 as an educational approximation, not an accurate value of pi. main sets the radius to two and obtains the description and area 12 before the next section introduces an interface-typed value.

## 17.2 Organize calls around behavior

A report that reads r or side directly must know every shape layout and change when a shape is added. Shape instead requires area and description behavior, leaving fields to each implementation.

Report accepts an interface reference and invokes those common methods. Circle and Square differ internally but share the calling contract. Borrowing existing objects exposes dispatch while retaining the concrete model's value, resource and lifetime rules.

<!-- verify-book -->
```chali
interface Shape
{
    int64 area();
    String describe();
};

struct Circle: Shape
{
    int64 r;
    int64 area()
    {
        return 3 * r * r;
    }
    String describe()
    {
        return "圆";
    }
};

struct Square: Shape
{
    int64 side;
    int64 area()
    {
        return side * side;
    }
    String describe()
    {
        return "正方形";
    }
};

String report(Shape const& shape)
{
    return shape.describe() + "：" + format("{}", shape.area());
}

void main()
{
    Circle c;
    c.r = 2;
    Square s;
    s.side = 3;
    Shape const& shape1 = &c;
    Shape const& shape2 = &s;
    println("{}", report(shape1));
    println("{}", report(shape2));
}
```

```text
圆：12
正方形：9
```

Shape declares area and describe without prescribing fields such as r or side. Circle and Square implement these behaviors. Report needs only Shape const&, with no branch on the concrete shape and no ownership transfer of the original object.

Main borrows two live objects through interface references. The circle uses integer three as an illustrative coefficient, giving twelve for radius two; the square gives nine for side three. Report dispatches to each implementation and joins its description with the formatted area. This demonstrates interface dispatch rather than precise geometry, and the references cannot outlive c or s.


## 17.3 Recognize a concrete error

IoError implements Error. Match can recognize that concrete value and bind it to e. A wildcard handles other implementations, and _(other) names the fallback value. An open interface generally needs a fallback because listed implementations do not exhaust future ones.

<!-- verify-book -->
```chali
import std::io;
void describe(Error error)
{
    match (error)
    {
        std::io::IoError(e) => { println("io: {}", e.toString()); }
        _ => { println("other: {}", error.toString()); }
    }
}
void main()
{
    describe(std::io::IoError::Closed);
    describe(MessageError::from("invalid score"));
}
```

```text
io: 文件已关闭
other: invalid score
```

Describe accepts Error statically, but the enclosed concrete implementation can vary. IoError::Closed matches the std::io::IoError type pattern and binds e, producing the io-prefixed message.

MessageError does not match that type, so the wildcard fallback uses the Error interface's toString to print invalid score. Keeping a fallback allows other implementations. When every branch would merely call the same interface method, concrete-type matching is usually unnecessary; the differing prefixes here deliberately expose the two paths.


## 17.4 Generic interfaces and inheritance

Interfaces can take type parameters and inherit other interfaces. Type arguments identify data used by their behavior, while inheritance combines required capabilities. It does not inherit class field layout or let casts invent missing implementations. The next chapter develops generic constraints.

## 17.5 Matching versus calling

Call the interface directly when every implementation supplies the required behavior. Refine types when recovery, payload access or handling actually differs. Avoid duplicating identical toString calls for every known implementation. Chapter 16 handles interface payloads inside Result and propagation.

### Summary

Interfaces promise callable behavior rather than a particular field layout. Borrowed implementations still need live sources, and value payloads retain concrete copy and resource semantics. Refine types only when recovery or extra data requires it.

Chapter 14 applies behavior requirements to generic constraints and links implementations to results through associated types. Chapter 16 places interface errors inside Result states.

## 17.6 Refining Errors Inside Enum Payloads

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

The first example matches an IoError held through Error, using both explicit variant/type sugar and an unambiguous shorthand, with Err(_) as fallback.

The second retains the outer variant for multiple payloads, with code and typed error positions. Result<Error,Error> can hold an error object in Ok; payload type alone does not determine success.

The third widens concrete errors through propagation. Option propagation tests Value versus None, independently of whether the stored object implements Error. Read the outer tag before the concrete payload type.

## 17.7 Exercises

1. Add another error implementation and check fallback.
2. How do you write a wildcard without a binding?
3. Does interface inheritance inherit fields?
4. When should you call instead of matching?

### Answers and checks

1. Retain _ and confirm the fallback executes.
2. Write _ => {...}.
3. No; it combines behavioral requirements.
4. When the interface fully expresses the operation without concrete payload access.

Next: [Chapter 18](18-generics.md).
