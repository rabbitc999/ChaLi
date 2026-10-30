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

### Code walkthrough

Packet has a Data variant carrying a String and an Empty variant carrying none. Packet currently holds Data, so both matches choose that arm. Empty still needs handling so the expression produces a result for every variant.

The first match binds a value copy of the payload as text, changes it to copy and yields it to label. It does not update packet. Matching again therefore reads original, producing `copy original`. Each text binding belongs to its own arm scope; the second is not the modified variable from the first match.


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

### Code walkthrough

Message declares two ordered payload fields. Construction supplies Ada for sender and ready for text. The first match binds both and prints `Ada: ready`; Idle is not selected on this run.

The second match uses `_` to ignore the first payload while binding text, so it prints only ready. Ignoring a binding does not remove the sender from event. Read matching in two stages: the variant name selects the arm, then the positions in parentheses select the values to bind.


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

### Code walkthrough

Derivation supplies comparison, hashing and cloning for BookPacket. First holds Text("value"), and second retains its variant and payload after cloning. The assertion checks that Text and Empty differ: their variant identities already differ before considering a string payload.

Println then verifies equality between first and second and consistent hashes, giving two true results. Derived behavior must consider both variant and payload. The actual hash value is not printed here and should not be treated as a stable external identifier.


## 10.4 Computing with match

Match can produce a value when its branches satisfy a common result type. Keep payload-dependent work in its matching branch instead of asserting a guessed variant later. Option and Result apply this structure to absence and failure.

### Summary

Enums define possible states, match maps states to paths and payload bindings expose typed data within the corresponding arm. For result-producing matches, check coverage and result compatibility instead of guessing states afterward.

Option and Result apply this structure to absence and failure in Chapter 16. First, Chapter 11 organizes typed values in containers.

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
