# Chapter 12 Strings and formatting

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-字符串与文本.md)
<!-- END LANGUAGE NAVIGATION -->

Strings serve human display and external file, network and database protocols. Displaying a value differs from encoding it correctly for a particular format, just as UTF-8 byte length differs from visible character count.

Begin with interpolation and escaping, then compare structural display with custom toString behavior. Understanding how text is formed supports appropriate choices of persistence formats, escaping and buffering.

## 12.1 From concatenation to slicing

<!-- verify-book -->
```chali
void main()
{
    String first = "ChaLi";
    String second = " 2026";
    String joined = first + second;
    println("{}", joined);
    println("{}", joined.length());
    println("{}", joined.substring(0, 5));
    println("{}", joined.substring(5, joined.length() - 5));
}
```

```text
ChaLi 2026
10
ChaLi
 2026
```

Concatenation creates joined from two strings. length counts bytes; ASCII uses one byte per character but Chinese does not. substring takes a start and a length, so the two calls select the first five bytes and the remaining suffix.

## 12.2 Text and boundaries

String uses UTF-8. Byte offsets are not necessarily character boundaries. State whether an input limit counts bytes or user-visible characters, and do not infer non-ASCII behavior from ASCII examples.

## 12.3 Interpolation and escaping

To include a count in a message, interpolation places its expression beside the surrounding text. The relationship remains visible: evaluate the expression, then display its result.

Braces may also be literal characters, so escaping distinguishes them from expression boundaries. The example combines arithmetic, nested interpolation and literal braces. Producing display text still does not perform the separate encoding required for HTML, SQL or JSON.

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

The f prefix enables interpolation. `{count + 1}` evaluates to four before insertion. The second interpolation contains another f string, which reads the unchanged count and produces inner=3 for insertion into the outer text.

Escaped braces around literal preserve those characters instead of requesting a variable lookup. Once text is constructed, println merely prints that completed string through its placeholder. Calculating count+1 does not assign to count, explaining why the result contains both four and three.


## 12.4 Structural and custom display

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

The five calls show structural display for a structure, list, set, map and tuple. Pair displays its field contents, while List<Any> formats integers, strings and booleans according to their contents. Quotes around two identify a string in the display; they are not extra characters added to the stored value.

The tuple's third item is the whole list, so its output nests list notation inside parentheses. The set and map have the shown acceptance output, but that observation does not promise sorting after every operation or across implementations. Structural text supports reading and diagnostics. Use a serialization API for exchange data even when a display resembles JSON.


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

Named defines toString to join a fixed prefix with self.name. Setting name to ChaLi therefore produces name=ChaLi instead of the default structural field display.

`String text = value` demonstrates conversion in a String destination context. `cast<String>(value)` requests it explicitly. Both use the model's display behavior and print the same text. Producing a string does not permanently change value's type or establish that conversion to other destination types exists.


## 12.5 Appending, buffers and safe output

Repeated appends depend on copies, temporaries and capacity growth. String can reuse allocated buffers; shortening does not imply freeing capacity. Optimizations must preserve observable copy and borrow behavior. Chapter 24 distinguishes compiler settings, and Chapter 25 uses textContent to display user data without interpreting names as HTML.

### Summary

Decide whether text is for display or exchange before choosing interpolation, toString or serialization. Interpolation evaluates expressions, display conversion explains a human view and protocol encoding defines how another endpoint parses data.

External text may still be malformed or violate business rules. Error handling and the final project place parsing, field validation, SQL quoting and page display at their respective boundaries.

## 12.6 Exercises

1. Print an error containing non-ASCII text.
2. Does formatting safely construct SQL?
3. When should toString be used?
4. Must shortening immediately free the buffer?

### Answers and checks

1. Inspect encoding and output instead of treating bytes as characters.
2. No; use database quoting or parameter binding.
3. For display; use explicit formats for stable storage.
4. No; retained capacity supports later growth and drop releases the final buffer.

Next: [Chapter 17](17-interfaces.md).
