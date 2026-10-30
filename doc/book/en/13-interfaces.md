# Chapter 13 Interfaces and concrete-type matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../13-接口与运行期多态.md)
<!-- END LANGUAGE NAVIGATION -->

An interface describes callable behavior implemented by concrete structs or enums. Callers can depend on that behavior or match a concrete type when domain handling differs.

## 13.1 Organize calls around behavior

Put common operations in an interface, such as area, error display, reading or writing. Interface consumers depend on these operations rather than implementation fields. Concrete values retain their own ownership and resource rules.

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

## 13.3 Generic interfaces and inheritance

Interfaces can take type parameters and inherit other interfaces. Type arguments identify data used by their behavior, while inheritance combines required capabilities. It does not inherit class field layout or let casts invent missing implementations. The next chapter develops generic constraints.

## 13.4 Matching versus calling

Call the interface directly when every implementation supplies the required behavior. Refine types when recovery, payload access or handling actually differs. Avoid duplicating identical toString calls for every known implementation. Chapter 16 handles interface payloads inside Result and propagation.

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
