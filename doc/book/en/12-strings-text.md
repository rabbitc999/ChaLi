# Chapter 12 Strings and formatting

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-字符串与文本.md)
<!-- END LANGUAGE NAVIGATION -->

A string is text and also data with a length and capacity. Distinguish character meaning, byte boundaries and formatting before generating pages, parsing files or displaying errors.

## 12.1 Text and boundaries

String uses UTF-8. Byte offsets are not necessarily character boundaries. State whether an input limit counts bytes or user-visible characters, and do not infer non-ASCII behavior from ASCII examples.

## 12.2 Interpolation and escaping

Formatting inserts values and evaluates supported embedded expressions. Escape literal braces according to the formatting syntax. Formatting is not a replacement for HTML escaping, SQL quoting or JSON serialization.

<!-- verify-book -->
```chali
void main()
{
    int64 count = 3;
    String text = f"count={count + 1}, {f"inner={count}"}, \{literal\}";
    println("{}", text);
}
```

```text
count=4, inner=3, {literal}
```

## 12.3 Structural and custom display

Default structural display helps diagnostics but is not a stable storage format. Provide toString for domain display. Use a serialization protocol with explicit failure handling for persistence or transport.

<!-- verify-book -->
```chali
struct Pair
{
    String left;
    String right;
};

void main()
{
    Pair pair;
    pair.left = "one";
    pair.right = "two";
    List<Any> list = [1, "two", true];
    Set<int64> set = {3, 1, 2};
    Map<String, Any> map = {"name": "ChaLi", "count": 3};
    let tuple = (1, "two", list);

    println("{}", pair.toString());
    println("{}", list.toString());
    println("{}", set.toString());
    println("{}", map.toString());
    println("{}", tuple.toString());
}
```

```text
{"one", "two"}
[1, "two", true]
{3, 1, 2}
{"name": "ChaLi", "count": 3}
(1, "two", [1, "two", true])
```

<!-- verify-book -->
```chali
struct Named
{
    String name;
    String toString() { return "name=" + self.name; }
};

void main()
{
    Named value;
    value.name = "ChaLi";
    String text = value;
    println("{}", text);
    println("{}", cast<String>(value));
}
```

```text
name=ChaLi
name=ChaLi
```

## 12.4 Appending, buffers and safe output

Repeated appends depend on copies, temporaries and capacity growth. String can reuse allocated buffers; shortening does not imply freeing capacity. Optimizations must preserve observable copy and borrow behavior. Chapter 24 distinguishes compiler settings, and Chapter 25 uses textContent to display user data without interpreting names as HTML.

## 12.5 Exercises

1. Print an error containing non-ASCII text.
2. Does formatting safely construct SQL?
3. When should toString be used?
4. Must shortening immediately free the buffer?

### Answers and checks

1. Inspect encoding and output instead of treating bytes as characters.
2. No; use database quoting or parameter binding.
3. For display; use explicit formats for stable storage.
4. No; retained capacity supports later growth and drop releases the final buffer.

Next: [Chapter 13](13-interfaces.md).
