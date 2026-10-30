# Chapter 13 Interfaces and concrete-type matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../13-接口与运行期多态.md)
<!-- END LANGUAGE NAVIGATION -->

Different layouts can offer the same operations. Interfaces collect the behavior a caller needs, allowing one processing function to work across implementations. Shapes and errors provide concrete examples.

First dispatch through Shape references, then identify concrete Error types when handling genuinely differs. Direct dispatch shares behavior; concrete-type matching supports implementation-specific handling.

## 13.1 Organize calls around behavior

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

### Code walkthrough

Shape declares area and describe without prescribing fields such as r or side. Circle and Square implement these behaviors. Report needs only Shape const&, with no branch on the concrete shape and no ownership transfer of the original object.

Main borrows two live objects through interface references. The circle uses integer three as an illustrative coefficient, giving twelve for radius two; the square gives nine for side three. Report dispatches to each implementation and joins its description with the formatted area. This demonstrates interface dispatch rather than precise geometry, and the references cannot outlive c or s.


## 13.2 Recognize a concrete error

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

### Code walkthrough

Describe accepts Error statically, but the enclosed concrete implementation can vary. IoError::Closed matches the std::io::IoError type pattern and binds e, producing the io-prefixed message.

MessageError does not match that type, so the wildcard fallback uses the Error interface's toString to print invalid score. Keeping a fallback allows other implementations. When every branch would merely call the same interface method, concrete-type matching is usually unnecessary; the differing prefixes here deliberately expose the two paths.


## 13.3 Generic interfaces and inheritance

Interfaces can take type parameters and inherit other interfaces. Type arguments identify data used by their behavior, while inheritance combines required capabilities. It does not inherit class field layout or let casts invent missing implementations. The next chapter develops generic constraints.

## 13.4 Matching versus calling

Call the interface directly when every implementation supplies the required behavior. Refine types when recovery, payload access or handling actually differs. Avoid duplicating identical toString calls for every known implementation. Chapter 16 handles interface payloads inside Result and propagation.

### Summary

Interfaces promise callable behavior rather than a particular field layout. Borrowed implementations still need live sources, and value payloads retain concrete copy and resource semantics. Refine types only when recovery or extra data requires it.

Chapter 14 applies behavior requirements to generic constraints and links implementations to results through associated types. Chapter 16 places interface errors inside Result states.

## 13.5 Exercises

1. Add another error implementation and check fallback.
2. How do you write a wildcard without a binding?
3. Does interface inheritance inherit fields?
4. When should you call instead of matching?

### Answers and checks

1. Retain _ and confirm the fallback executes.
2. Write _ => {...}.
3. No; it combines behavioral requirements.
4. When the interface fully expresses the operation without concrete payload access.

Next: [Chapter 14](14-generics.md).
