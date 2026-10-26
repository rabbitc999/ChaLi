# Chapter 12 Strings and text

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-字符串与文本.md)
<!-- END LANGUAGE NAVIGATION -->

`String` owns a sequence of bytes. `len` counts bytes, and indexing or slicing uses byte positions. Ordinary indexing requires a valid UTF-8 character boundary. Slices do not require those boundaries; a slice can therefore contain only part of an encoded character. This is useful for byte-oriented protocols but must be handled deliberately in human text.

String search, comparison, slicing, transformations, splitting, joining, UTF-8 decoding and value copying are implemented in ChaLi. Heap strings retain their existing length-header and byte layout, and ordinary assignment still creates an independent copy. Host boundaries provide allocation, storage-layout access, I/O and numeric display formatting.

```chali
void main()
{
    String text = "ChaLi";
    String prefix = text[0:3];
    println("{} {}", prefix, len(text));
}
```

Use the UTF-8 library when the operation requires Unicode character boundaries or validation. Use text helpers for documented parsing, searching, splitting and joining. A byte search and a character-level transformation are different operations even when both accept strings.

Repeated concatenation may allocate and copy. For substantial incremental output, inspect `StringBuilder` and its ownership behavior. For binary input, avoid assuming that every byte sequence is valid display text.

Error messages from text and encoding operations implement the shared `Error` interface through their concrete error types. Read or propagate the error before accessing a result. Exercise: slice an ASCII string, then inspect the bytes of a multibyte UTF-8 string and explain why a one-byte slice may not display as a character. Next: [interfaces](13-interfaces.md).
