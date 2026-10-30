# Chapter 10 Enums and pattern matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../10-枚举与模式匹配.md)
<!-- END LANGUAGE NAVIGATION -->

An enum records alternative states in its type. A variant may carry no data, one payload or several payloads. Match chooses a branch and binds its payloads.

## 10.1 States and payloads

List mutually exclusive states before choosing their data. A request might be waiting, completed with a body, or failed with an error. Identify the variant before reading a payload, and cover reachable alternatives in a match expression.

<!-- verify-book -->
```chali
enum Packet
{
    Data(String text), Empty
};
void main()
{
    Packet packet = Packet::Data("original");
    String label = match (packet)
    {
        Packet::Data(text) =>
        {
            text = "copy";
            yield text;
        }
        Packet::Empty =>
        {
            "empty"
        }
    };
    String original = match (packet)
    {
        Packet::Data(text) =>
        {
            text
        }
        Packet::Empty =>
        {
            "empty"
        }
    };
    println("{} {}", label, original);
}
```

```text
copy original
```

## 10.2 Multiple payloads and wildcards

Write multiple payloads in declaration order. Bind ordinary payloads directly, or use _ when their values are unnecessary. Interface payload refinement uses Type value inside an explicit outer variant; Chapter 13 explains the concrete-type test.

<!-- verify-book -->
```chali
enum Event
{
    Idle,
    Message(String sender, String text)
};
void main()
{
    let event = Event::Message("Ada", "ready");
    match (event)
    {
        Event::Idle => { println("idle"); }
        Event::Message(sender, text) => { println("{}: {}", sender, text); }
    }
    match (event)
    {
        Event::Idle => { println("idle"); }
        Event::Message(_, text) => { println("{}", text); }
    }
}
```

```text
Ada: ready
ready
```

## 10.3 Copying and derived operations

Payloads retain their own value, borrow and resource rules. Copying a String-bearing enum does not make later value mutations affect the original. Reference payloads still express borrowing. An enum wrapper does not bypass ownership checks.

<!-- verify-book -->
```chali
#derive(Equal, Hash, Clone)
enum BookPacket
{
    Empty,
    Text(String text)
}

void main()
{
    BookPacket first = BookPacket::Text("value");
    BookPacket second = first.clone();
    assert(!(first == BookPacket::Empty));
    println("{} {}", first == second, first.hash(first) == second.hash(second));
}
```

```text
1 1
```

## 10.4 Computing with match

Match can produce a value when its branches satisfy a common result type. Keep payload-dependent work in its matching branch instead of asserting a guessed variant later. Option and Result apply this structure to absence and failure.

## 10.5 Exercises

1. Add a disconnected state to Event.
2. Print only Message text.
3. When is a wildcard necessary?
4. Is a reference payload an independent copy?

### Answers and checks

1. Update every exhaustive match.
2. Use Message(_, text).
3. Keep a fallback for remaining alternatives or interface implementations.
4. No; lifetime and access restrictions remain.

Next: [Chapter 11](11-containers.md).
