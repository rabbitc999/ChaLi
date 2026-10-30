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
    List<String> lines = std::text::Text::split(text, "\n");
    String needle = query;
    if (ignoreCase)
    {
        needle = std::text::Text::toLower(needle);
    }
    for (int64 index in range(0, lines.len()))
    {
        String line = lines[index];
        if (index == lines.len() - 1 && line.isEmpty())
        {
            break;
        }
        if (std::text::Text::endsWith(line, "\r"))
        {
            line = line.substring(0, line.length() - 1);
        }
        String candidate = line;
        if (ignoreCase)
        {
            candidate = std::text::Text::toLower(candidate);
        }
        if (std::text::Text::contains(candidate, needle))
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
    let hits = log_search::search("INFO ready\nERROR disk\n", "error", true);
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
    std::io::FileStream stream = std::io::FileStream::open(path, "r")?;
    return stream.readAllString(4194304);
}
```

The first statement opens the file and propagates failure with ?. The next reads the remaining text within a four-MiB budget and returns that Result. The local stream's resource is cleaned up when its scope ends; the caller receives text, not ownership of the stream. This size is the tool's policy, not String's language limit.

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
    let result = log_search::run(arguments);
    if (result.isErr())
    {
        panic("log-search: " + result.error().toString());
    }
}
```

The first call delegates the work. The failure branch reports an unsuccessful invocation; success ends normally. No matches is a successful empty result, with zero in count mode. This teaching entry does not implement every grep exit-status convention.

## 16.7 Extend one boundary at a time

Project checks cover spaces in paths, CRLF, empty files, a final line without newline, Unicode, zero bytes, and oversized input. A display limit can be added as configuration and applied at output. After learning iterators, revisit the intermediate lists.

## 16.8 Exercises

1. Compare case modes while preserving output text.
2. Add a --limit option.
3. Distinguish missing arguments from a missing file, preserving path context.
4. Why can block-based input not simply search each block independently?

### Answers and checks

1. Fold comparison copies, not Hit.text.
2. Validate a nonnegative limit and apply it while displaying original line numbers.
3. Argument errors precede file access; read errors include the actual path.
4. Lines and queries can cross block boundaries, requiring retained partial input and accumulated positions.
