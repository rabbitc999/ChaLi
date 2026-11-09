# Chapter 16 A Log Search Tool

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../16-日志检索工具.md)
<!-- END LANGUAGE NAVIGATION -->

A log often contains only a few lines relevant to the current problem. We will build a command accepting a query and file, returning matching line numbers and original text. Case-sensitive matching comes first, followed by ASCII case folding and a count option.

The [complete project](../../../example/log-search/README.en.md) separates the entry in src/main.chl from search and input handling in src/log_search.chl. Follow the chapter alongside those files.

## 16.1 Use the tool

From the project directory:

```sh
chali run src/main.chl -- ERROR sample.log
```

```text
2:ERROR cannot open report
```

The compiler-level `--` forwards application arguments. Add --ignore-case to match the lowercase fourth line too; --count then prints 2.

## 16.2 Describe a hit

```chali
struct Hit
{
    int64 lineNumber;
    String text;
}
```

lineNumber uses one-based human numbering. text preserves the original line. Keeping them together prevents later printing or sorting from separating related data.

```chali
List<Hit> search(String text, String query, bool ignoreCase)
{
    List<Hit> hits;
    List<String> lines = Text::split(text, "\n");
    String needle = query;
    if (ignoreCase)
    {
        needle = Text::toLower(needle);
    }
    for (int64 index in range(0, lines.len()))
    {
        String line = lines[index];
        if (index == lines.len() - 1 && line.isEmpty())
        {
            break;
        }
        if (Text::endsWith(line, "\r"))
        {
            line = line.substring(0, line.length() - 1);
        }
        String candidate = line;
        if (ignoreCase)
        {
            candidate = Text::toLower(candidate);
        }
        if (Text::contains(candidate, needle))
        {
            Hit hit;
            hit.lineNumber = index + 1;
            hit.text = line;
            hits.push(hit);
        }
    }
    return hits;
}
```

The preparation section creates a result list, splits the input into lines, and prepares the comparison needle. Case folding is ASCII-only; Unicode text otherwise matches its original bytes.

The loop first handles line endings. A final empty split fragment is not an extra log line; an actual empty interior line still contributes to numbering. A trailing CR from CRLF is removed.

The comparison section folds a separate candidate when requested, preserving the output text. A match produces a Hit with index plus one and the original line. The function returns the completed list.

## 16.3 Check a small input

Save this complete example alongside the project module in src:

<!-- verify-book -->
```chali
import log_search;

void main()
{
    let hits = search("INFO ready\nERROR disk\n", "error", true);
    assert(hits.len() == 1);
    assert(hits[0].lineNumber == 2);
    println("{}:{}", hits[0].lineNumber, hits[0].text);
}
```

```text
2:ERROR disk
```

The call supplies two lines, a query, and a case option. The assertions separately check count and position. Printing shows that the original uppercase ERROR was retained.

## 16.4 Connect file input

```chali
Result<String, Error> readText(String path)
{
    FileStream stream = FileStream::open(path, "r")?;
    return stream.readAllString(4194304);
}
```

The first statement opens the file and propagates failure with ?. The next reads the remaining text within a four-MiB budget and returns that Result. The local stream's resource is cleaned up when its scope ends; the caller receives text, not ownership of the stream. This size is the tool's policy, not String's language limit.

### Reuse a byte buffer for chunked I/O

For input containing zero bytes or for large files you want to process in chunks, open a binary stream and reuse a runtime `uint8[]`. `readInto(&buffer)` reads up to `len(buffer)` bytes from the current position and returns the number read. Only the first `n` entries belong to that read. After a short read, the tail still holds its previous contents. `Ok(0)` with a nonempty buffer means end of file. A zero-length buffer can also produce zero, so use a nonempty buffer when testing for EOF.

`writeFrom(&buffer, count)` writes the first `count` bytes. The count must be between zero and the buffer length. An out-of-range count produces an error. When copying a binary stream, use the actual number of bytes read for each write. This keeps the tail of the previous chunk out of a short final write. `copyTo(&source, &destination)` performs this loop from the streams' current positions and returns the total byte count on success. The caller closes both streams. An I/O failure can occur after a position has advanced or some bytes have been written, so account for partial progress when handling errors.

Run this example in a temporary directory. It creates or overwrites two files there. The program writes five bytes and reads them back using a three-byte buffer. After the second read, the third slot still contains `255`.

<!-- verify-book -->
```chali
import std::io;

void main() {
    uint8 bytes[] = new[3];
    bytes[0] = (uint8)0;
    bytes[1] = (uint8)10;
    bytes[2] = (uint8)255;

    FileStream output = FileStream::open("book-buffer.bin", "wb").take();
    assert(output.writeFrom(&bytes, 4).isErr());
    assert(output.writeFrom(&bytes, 3).unwrap() == 3);
    assert(output.writeFrom(&bytes, 2).unwrap() == 2);
    assert(output.close() == 0);

    FileStream input = FileStream::open("book-buffer.bin", "rb").take();
    assert(input.readInto(&bytes).unwrap() == 3);
    assert(bytes[0] == (uint8)0 && bytes[2] == (uint8)255);
    assert(input.readInto(&bytes).unwrap() == 2);
    assert(bytes[0] == (uint8)0 && bytes[1] == (uint8)10);
    assert(bytes[2] == (uint8)255);
    assert(input.readInto(&bytes).unwrap() == 0);

    input.seek(0, SeekOrigin::Begin).unwrap();
    FileStream copied = FileStream::open("book-buffer-copy.bin", "wb").take();
    int64 count = copyTo(&input, &copied).unwrap();
    assert(count == 5);
    assert(copied.close() == 0);
    assert(input.close() == 0);
    println("copied={} bytes", count);
}
```

```text
copied=5 bytes
```

## 16.5 Parse a query configuration

Options contains query, path, ignoreCase, countOnly, and help. parseOptions visits arguments in order, collecting positional values and updating switches, then requires a query and file.

```chali
if (flags && argument == "--")
{
    flags = false;
}
else if (flags && argument == "--ignore-case")
{
    options.ignoreCase = true;
}
```

The first branch ends application option parsing. The second changes a bool without reading files. The complete function also handles count, help, unknown options, and argument count. The compiler's separator and the application's separator have different jobs; try a literal query beginning with --.

## 16.6 Connect work and output

run parses arguments, handles help before file access, reads text, searches, and selects line or count output. Read errors gain the supplied file path before returning to the entry:

```chali
void main(List<String> arguments)
{
    let result = run(arguments);
    if (result.isErr())
    {
        panic("log-search: " + result.error().toString());
    }
}
```

The command in Section 16.1 writes one line to stdout when run in the complete project.

```text
2:ERROR cannot open report
```

The first call delegates the work. The failure branch reports an unsuccessful invocation; success ends normally. No matches is a successful empty result, with zero in count mode. This teaching entry does not implement every grep exit-status convention.

## 16.7 Extend one boundary at a time

Project checks cover spaces in paths, CRLF, empty files, a final line without newline, Unicode, zero bytes, and oversized input. A display limit can be added as configuration and applied at output. After learning iterators, revisit the intermediate lists.

### Test the log filter independently

Verify the filter on two fixed log lines before reading a file.

<!-- verify-book -->
```chali
bool wanted(String line) {
    return line.contains("ERROR");
}
void main() {
    println("{} {}", wanted("INFO ready"), wanted("ERROR timeout"));
}
```

```text
0 1
```
