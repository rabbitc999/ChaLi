# Chapter 30 — A First Look at the Standard Library

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../30-标准库初探.md)
<!-- END LANGUAGE NAVIGATION -->

When a small program needs collections, text, I/O or time handling, start by identifying the relevant module.

## 30.1 Start with the auto-loaded core module

Strings and lists are available directly. This program imports math for the module constant PI, computes the circumference at radius 2, and explicitly converts the result to an integer. The conversion discards the fractional part. For more decimal precision, use pi(digits).

<!-- verify-book -->
```chali
import std::math;

void main() {
    List<String> names;
    names.push("Ada");
    names.push("Lin");

    float64 perimeter = 2.0 * std::math::PI * 2.0;
    println("{} / {}", names.len(), (int64)perimeter);
}
```

```text
2 / 12
```


## 30.2 Keep keys in order

An AVL tree is useful when keys arrive out of order but later need ordered access. Its in-order snapshot is sorted by key. Use a Map when each key needs a value, or use the iterator adapters to chunk or pair an existing sequence.

<!-- verify-book -->
```chali
import std::tree;

void main() {
    AvlTree keys = AvlTree::create();
    keys.insert(30);
    keys.insert(10);
    keys.insert(20);
    List<int64> ordered = keys.inorder();
    println("{} {} {}", ordered[0], ordered[1], ordered[2]);
}
```

```text
10 20 30
```


## 30.3 Handle file failures first

Run this example in an empty directory. A failed read returns a Result, so the caller decides whether to report, retry, or use default content. For large files, use FileStream and copy_to. A temporary file in the destination directory lets the caller replace the target after writing finishes.

<!-- verify-book -->
```chali
import std::io;

void main() {
    let result = std::io::readAllText("missing-standard-library-example.txt");
    if (result.isErr()) {
        println("input unavailable");
        return;
    }
    println("{}", result.take());
}
```

```text
input unavailable
```


## 30.4 Parse an explicit UTC offset

The input clock time is 05:30 at offset +05:30. Converting it to UTC preserves the instant. A fixed offset is enough for a timestamp in a data file. It does not carry a region’s daylight-saving rules.

<!-- verify-book -->
```chali
import std::date_time;

void main() {
    let input = std::date_time::parse_offset("1970-01-01T05:30:00+05:30");
    let value = input.unwrap();
    let utc = value.utc().unwrap();
    println("{}", std::date_time::format_utc(utc).unwrap());
}
```

```text
1970-01-01T00:00:00Z
```


## 30.5 Produce logs that tools can read

Keep the operation name in its own field so another tool can query it. format_record returns one JSON record for inspection. write_record emits the same record on standard error.

<!-- verify-book -->
```chali
import std::log;

void main() {
    Map<String,String> fields;
    fields.put("operation", "read");
    String record = std::log::format_record(std::log::LogLevel::Info, "ready", fields);
    println("{}", record);
}
```

```text
{"level":"info","message":"ready","fields":{"operation":"read"}}
```


## 30.6 When to install optional modules

The compiler includes the modules used in the preceding examples. Install the [optional host libraries](../../en/45-optional-libraries.md) when the program needs windows, audio, XML, cryptography or streaming HTTP. Use the [standard-library appendix](B-standard-library.md) and [API reference](../../library/README.md) to check inputs, results and limits.
