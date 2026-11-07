# Chapter 10 Enums and pattern matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../10-枚举与模式匹配.md)
<!-- END LANGUAGE NAVIGATION -->

A request may be pending, successful or failed, with different data required for each exclusive state. Enums encode that choice instead of combining unrelated flags and optional fields into potentially invalid states.

Construct payload variants, extract local data with match and examine copying, wildcards and derives. Identify the variant before using its payload so each branch's operations have an explicit premise.

## 10.1 States and payloads

A packet may contain text or be empty. A String alone cannot clearly distinguish no payload from a payload that happens to be empty. An extra flag would require keeping two fields consistent. An enum lists the alternatives directly.

Data owns its String payload and Empty needs none. Match identifies the state before text is used. A value-producing match must handle reachable states rather than assuming one payload is always available.

<!-- verify-book -->
```chali
enum Packet
{
    Data(String text), Empty
};
void main()
{
    Packet packet = Data("original");
    String label = match (packet)
    {
        Data(text) =>
        {
            text = "copy";
            yield text;
        }
        Empty =>
        {
            "empty"
        }
    };
    String original = match (packet)
    {
        Data(text) =>
        {
            text
        }
        Empty =>
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

Packet has a Data variant carrying a String and an Empty variant carrying none. Packet currently holds Data, so both matches choose that arm. Empty still needs handling so the expression produces a result for every variant.

Prefer short names such as Data and Empty for construction and matching when the type context determines the enum or the variant name is unambiguous. Result's Ok/Err and Option's Value/None follow the same rule; variable, return, and parameter types can supply generic arguments. Use a necessary qualifier such as Packet::Data when names collide and context is insufficient.

The first match binds a value copy of the payload as text, changes it to copy and yields it to label. It does not update packet. Matching again therefore reads original, producing `copy original`. Each text binding belongs to its own arm scope; the second is not the modified variable from the first match.


When the target enum and its arguments are known, a uniquely matching single-payload variant may also be omitted. For example, a String payload for Option<String> may appear directly in initialization, reassignment, arguments or returns and is wrapped as Value. Multiple-payload variants still use explicit construction; equally matching variants require an explicit name. Chapter 14 applies the same rule to error objects. Move and borrow checks remain in force.

## 10.2 Multiple payloads and wildcards

Write multiple payloads in declaration order. Bind ordinary payloads directly, or use _ when their values are unnecessary. Interface payload refinement uses Type value inside an explicit outer variant; Chapter 17 explains the concrete-type test.

<!-- verify-book -->
```chali
enum Event
{
    Idle,
    Message(String sender, String text)
};
void main()
{
    let event = Message("Ada", "ready");
    match (event)
    {
        Idle => { println("idle"); }
        Message(sender, text) => { println("{}: {}", sender, text); }
    }
    match (event)
    {
        Idle => { println("idle"); }
        Message(_, text) => { println("{}", text); }
    }
}
```

```text
Ada: ready
ready
```

Message declares two ordered payload fields. Construction supplies Ada for sender and ready for text. The first match binds both and prints `Ada: ready`; Idle is not selected on this run.

The second match uses `_` to ignore the first payload while binding text, so it prints only ready. Ignoring a binding does not remove the sender from event. Read matching in two stages: the variant name selects the arm, then the positions in parentheses select the values to bind.


## 10.3 Copying and derived operations

Payloads retain their own value, borrow and resource rules. Copying a String-bearing enum does not make later value mutations affect the original. Reference payloads still express borrowing. An enum wrapper does not bypass ownership checks.

<!-- verify-book -->
```chali
import std::macro;

enum BookPacket
{
    Empty,
    Text(String text);
    $std::macro::Equal<BookPacket>(leftEqual);
    $std::macro::Hash<BookPacket>(hash);
    $std::macro::Clone<BookPacket>(clone);
}

void main()
{
    BookPacket first = Text("value");
    BookPacket second = first.clone();
    assert(!(first == Empty));
    println("{} {}", first == second, first.hash(first) == second.hash(second));
}
```

```text
1 1
```

Derivation supplies comparison, hashing and cloning for BookPacket. First holds Text("value"), and second retains its variant and payload after cloning. The assertion checks that Text and Empty differ: their variant identities already differ before considering a string payload.

Println then verifies equality between first and second and consistent hashes, giving two true results. Derived behavior must consider both variant and payload. The actual hash value is not printed here and should not be treated as a stable external identifier.


## 10.4 Computing with match

Match can produce a value when its branches satisfy a common result type. Keep payload-dependent work in its matching branch instead of asserting a guessed variant later. Option and Result apply this structure to absence and failure.

### Extract an enum payload

A matching payload variant binds its data inside the branch.

<!-- verify-book -->
```chali
enum Signal {
    Idle, Data(int64 value)
};

void main() {
    Signal item = Data(42);
    match (item) {
        Data(value) => {
            println("{}", value);
        }
        _ => {
            println("idle");
        }
    }
}
```

```text
42
```

### Summary

Enums define possible states, match maps states to paths and payload bindings expose typed data within the corresponding arm. For result-producing matches, check coverage and result compatibility instead of guessing states afterward.

Option and Result apply this structure to absence and failure in Chapter 14. Chapter 13 organizes typed values in containers.

Next: [Chapter 11](11-modules-projects.md).
