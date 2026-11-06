# Appendix B Standard library lookup

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../B-标准库速查.md)
<!-- END LANGUAGE NAVIGATION -->

Start library lookup from the data and intended outcome: numerical operations, text conversion, iteration and external resources use different interfaces. Modules locate capabilities, signatures specify inputs/results and failure contracts describe premises and recovery.

A combined example demonstrates lookup and invocation before the task-oriented reference entries. Short samples cannot cover every API boundary; inspect concrete types, source mutation, ownership and platform conditions for actual use.

## B.1 Combine explicit imports

<!-- verify-book -->
```chali
import std::math;
import std::text;
import std::iter;

void main()
{
    println("sqrt={}", std::math::sqrt(144.0));
    println("upper={}", std::text::Text::toUpper("chali"));
    int64 sum = 0;
    for (int64 v in std::iter::range(1, 5))
    {
        sum += v;
    }
    println("sum={}", sum);
}
```

```text
sqrt=12
upper=CHALI
sum=10
```

### Code walkthrough

The imports support three tasks: math supplies square root, text supplies case conversion and iter supplies range traversal. Qualified names identify each operation's module. Sqrt(144.0) yields twelve, and toUpper converts chali to CHALI for their respective outputs.

Range(1,5) yields one through four. Sum lives outside the loop and accumulates ten. Math and text results are printed directly, whereas the range is reduced by iteration. Importing makes a module available rather than executing its business operations automatically. Consult input types, results and failure behavior when choosing APIs.


range(1, 5) includes 1 through 4. Check endpoint and error conventions separately for other APIs.

## B.2 Core facilities

The core includes String, List, Map, Set, Pair, Option, Result, Error, MessageError, Any, formatting, assertions and numeric input. Value operations, borrowing and resources retain their type-specific rules. Explicit absence uses None; do not infer absence from default construction.

The core functions `inputBigInteger()` and `inputBigDecimal()` return types defined in [`std::big_number`](../../library/std-big_number.md). Import that module explicitly when declaring or constructing these types, as in the example below.

## B.3 Modules by task

| Module | Task |
| --- | --- |
| [`std::big_number`](../../library/std-big_number.md) | Arbitrary-precision `BigInteger` and high-precision decimal `BigDecimal` |
| [`std::math`](../../library/std-math.md) | Scalar mathematics and the `float64` module constants `PI` and `E` |
| [`std::numerics`](../../library/std-numerics.md) | Numerical methods |
| [`std::number_theory`](../../library/std-number_theory.md) | Integer number theory |
| [`std::complex`](../../library/std-complex.md) | Complex arithmetic |
| [`std::linear_algebra`](../../library/std-linear_algebra.md) | Dense vectors and matrices |
| [`std::random`](../../library/std-random.md) | Deterministic LCG and entropy seeding; not a session-token generator |
| [`std::signal`](../../library/std-signal.md) | DFT and FFT |
| [`std::cas`](../../library/std-cas.md) | Symbolic expressions and exact mathematics |
| [`std::text`](../../library/std-text.md) | Text operations |
| [`std::utf8`](../../library/std-utf8.md) | UTF-8 validation, code points and slicing |
| [`std::encoding`](../../library/std-encoding.md) | Hex, Base64 and URL encoding |
| [`std::csv`](../../library/std-csv.md) | CSV reading and writing |
| [`std::json`](../../library/std-json.md) | JSON parsing and reflected conversion |
| [`std::toml`](../../library/std-toml.md) | TOML values and reflected conversion |
| [`std::regex`](../../library/std-regex.md) | Compiled regular expressions |
| [`std::parse`](../../library/std-parse.md) | Text-to-number parsing |
| [`std::binary`](../../library/std-binary.md) | Fixed-width binary decoding |
| [`std::path`](../../library/std-path.md) | Path operations |
| [`std::compress`](../../library/std-compress.md) | Binary compression as List<uint8> |
| [`std::collections`](../../library/std-collections.md) | Deque, Queue and local value queues |
| [`std::linked_list`](../../library/std-linked_list.md) | Doubly linked lists |
| [`std::tree`](../../library/std-tree.md) | Binary search trees, AVL trees, red-black trees, and heaps. AVL trees support ordered bounds and minimum or maximum removal. |
| [`std::graph`](../../library/std-graph.md) | Graph algorithms and disjoint sets |
| [`std::algorithms`](../../library/std-algorithms.md) | Sorting, searching and aggregate operations |
| [`std::image`](../../library/std-image.md) | Raster data and image operations |
| [`std::reflect`](../../library/std-reflect.md) | Type, field, method and annotation metadata |
| [`std::serialization`](../../library/std-serialization.md) | Typed binary serialization |
| [`std::thread`](../../library/std-thread.md) | Isolated threads and task groups |
| [`std::message`](../../library/std-message.md) | Cross-thread messages |
| [`std::concurrency`](../../library/std-concurrency.md) | Bounded shared channels and explicit shared state |
| [`std::event`](../../library/std-event.md) | Events and timing sources |
| [`std::io`](../../library/std-io.md) | Files and IoError |
| [`std::net`](../../library/std-net.md) | TCP and HTTP client; no TLS client support |
| [`std::sql`](../../library/std-sql.md) | Database connections and backend-specific querying |
| [`std::process`](../../library/std-process.md) | Host processes and pipelines |
| [`std::clock`](../../library/std-clock.md) | Monotonic timing for differences |
| [`std::date_time`](../../library/std-date_time.md) | UTC calendar dates and Unix seconds |
| [`std::cli`](../../library/std-cli.md) | Options, positionals and help |
| [`std::memory`](../../library/std-memory.md) | Borrowed Span views and memory statistics |
| [`std::iter`](../../library/std-iter.md) | Iterator composition and terminal operations |


## B.4 Boundaries to check

Resource cleanup and repeated-close behavior belong to each type; do not assume every object has an interchangeable close method. Binary compressed output is a byte list. UTF-8 byte lengths and code-point operations are different. Local collections queues and explicit cross-thread channels have different sharing rules.

Database parameter binding varies by backend; the current MySQL example uses Database.quote for text. Reflection metadata does not validate business permissions. Optional modules and foreign libraries remain subject to the actual target and link configuration.

## B.5 API and source lookup

Use the [full symbol index](../../library/symbols.md) for signatures and source locations. Module bodies live in [source/std](../../../source/std/) and host services in [source/abi](../../../source/abi/); filenames need not match module names exactly. Current execution uses checked AOT or native exports.

## B.6 Verify a combination

Check input types, Result/Option returns, cleanup and platform support. Run a small successful example and a failure path, then validate borrowing and ownership across module boundaries. [Chapter 29](29-student-management.md) combines database services; [Chapter 14](14-error-handling.md) matches concrete interface errors.

## Standard macros and exact large numbers

Import `std::macro` for `$MIN`, `$MAX` and `$CLAMP`. Each supplied argument is evaluated once. The first call below invokes `next` twice in total, and the last clamps 9 to the closed interval from 0 to 6.

<!-- verify-book -->
```chali
import std::macro;

int64 calls = 0;
int64 next(int64 value) {
    calls++;
    return value;
}
void main() {
    println("{} {} {}", $std::macro::MIN(next(7), next(3)), $MAX(7, 3), $CLAMP(9, 0, 6));
    println("{}", calls);
}
```

```text
3 7 6
2
```

The public large-number types live in `std::big_number`. This program checks the qualified types at compile time and verifies that JSON serialization and deserialization preserve both values.

<!-- verify-book -->
```chali
import std::big_number;
import std::json;
import std::reflect;

@Reflect
struct Numbers {
    std::big_number::BigInteger integer;
    std::big_number::BigDecimal decimal;
}

$macro CHECK_IDENTITIES<T>() -> stmt
$if ($fields<T>())[0].type != std::big_number::BigInteger
$compile_error("unexpected BigInteger identity");
$end_if
$if ($fields<T>())[1].type != std::big_number::BigDecimal
$compile_error("unexpected BigDecimal identity");
$end_if
$end_macro

void main() {
    $CHECK_IDENTITIES<Numbers>();
    Numbers values;
    values.integer = std::big_number::BigInteger("9000000000000000000") + BigInteger(7);
    values.decimal = std::big_number::BigDecimal("1.25") + BigDecimal("2.50");
    Option<BigInteger> boxed = Option<BigInteger>::Value(values.integer);
    uint64 integerId = std::reflect::getTypeId<std::big_number::BigInteger>();
    assert(integerId != 0 && integerId == std::reflect::getTypeId<BigInteger>());
    String encoded = std::json::serialize<Numbers>(&values).unwrap();
    Numbers decoded = std::json::deserialize<Numbers>(encoded).unwrap();
    assert(decoded.integer == boxed.unwrap() && decoded.decimal == values.decimal);
    println("{} {}", decoded.integer, decoded.decimal);
}
```

```text
9000000000000000007 3.75
```

## B.7 Practical library operations

- Import `std::math` for the module constants `std::math::PI` and `std::math::E`. Use `std::math::pi(digits)` for a high-precision result.
- `std::date_time::parseOffset` reads a fixed UTC offset. `utc()` converts it to the same instant in UTC.
- `std::iter::groupBy` consumes a finite cursor and groups values by key. `chunk` lazily retains the final short block. `zip` stops at the shorter input.
- `std::log::formatRecord` creates a structured JSON line. `writeRecord` sends it to standard error.
- `std::io::copyTo` copies by blocks. `walkDirectory` lists descendants. Pair `createTempFile` with `replaceFile` for same-filesystem replacement.

Build the desktop, audio, XML, cryptography and streaming HTTP extensions on demand. See the [optional host libraries](../../en/45-optional-libraries.md) for build commands, complete programs and outputs.
