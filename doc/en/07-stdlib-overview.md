# 07 Standard library overview

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../07-stdlib-overview.md)
<!-- END LANGUAGE NAVIGATION -->

ChaLi's built-in modules, grouped by capability. Everything needs an `import` except the core module
`std::core`.

Standard-library failures use `Result<T, Error>` with an owned interface payload that retains the concrete error. IoError, ParseError, CsvError and other structured errors keep their variants, positions and messages. Text errors use MessageError::from(text) before boxing as Error. Shared handlers accept an Error value, Error const& or an E: Error generic constraint. Custom Result<T, E> values may retain a concrete E; when E implements Error, ? can propagate into a function returning Result<U, Error>.

```chali
import std::math;
import std::json;
void main() {
    println("sin0={} sqrt16={}", sin(0.0), sqrt(16.0));
    match (parse("{\"a\": 1}")) {
        Result<JsonValue, Error>::Ok(doc) => {
            println("isObject={} len={}", doc.isObject(), doc.len());
        }
        Result<JsonValue, Error>::Err(e) => { println("err={}", e); }
    }
}
```

```text
sin0=0 sqrt16=4
isObject=1 len=1
```

---

## 1. Core module `std::core` (auto-loaded)

Use `String` and `List<T>` directly, or spell them `std::core::String` and `std::core::List<T>`. An explicit `import std::core;` is also valid.

No `import` needed.

| Capability | Examples |
| --- | --- |
| Output and formatting | `println(fmt, …)`, `format(fmt, …)` |
| Assertions | `assert(cond)` |
| Length | `len(x)` (strings / containers) |
| Strings | `std::core::String`; index to read a character |
| Containers | `List<T>`, `Map<K,V>`, `Set<T>` |
| Type-erased box | `std::core::Any` (for runtime polymorphism) |
| Big numbers | `BigInteger`, `BigDecimal` |

The public opcode types are `std::big_number::BigInteger` and `std::big_number::BigDecimal`. Use `import std::big_number;` to state the dependency explicitly. The language prelude also makes their short names and literals available.

---

## 2. By capability

### Math and numerics

| Module | Contents |
| --- | --- |
| `std::math` | `sin` `cos` `sqrt` and friends; `abs` `min` `max` are generic entry points (`min`/`max` take any number of arguments) while `clamp` keeps `int64`/`float64` overloads; constants `std::math::PI` and `std::math::E` |
| `std::numerics` | scalar numerical methods: derivative, Simpson integration, bisection, RK4, Newton |
| `std::number_theory` | integer number theory: module-level generic `gcd` `lcm` (any number of arguments), `addMod` `mulMod` `powMod` `modInverse`, deterministic Miller-Rabin `isPrime` |
| `std::complex` | complex numbers `Complex<T>` with arithmetic traits |
| `std::linear_algebra` | dense vectors and matrices, `luDecompose` `symmetricEigen` `svd` `jordan2` `diagonalize2` |
| `std::random` | deterministic LCG and entropy seeding; handle-based state, implements `Drop` |
| `std::signal` | DFT / FFT / inverse; radix-2 FFT requires a power-of-two length |

### Standard macros

Import `std::macro` to use these expression macros. Each argument is evaluated once, including arguments that do not become the result.

| Macro | Behavior |
| --- | --- |
| `$MIN(left, right)` | Selects the smaller value. |
| `$MAX(left, right)` | Selects the larger value. |
| `$CLAMP(value, low, high)` | Limits a value to the inclusive interval. Reversed bounds trigger an assertion. |
| `$IN_RANGE(value, low, high)` | Tests whether the value is in the inclusive interval. Reversed bounds return false. |
| `$BOX(value)` | Creates a Box and infers its payload type from the expression. |

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

### Text and data formats

| Module | Contents |
| --- | --- |
| `std::text` | `Text::find` `split` `join` `replace` `trim` `toUpper` `padStart` … |
| `std::json` | `parse` → `Result<JsonValue, Error>`; `isInt` `isObject` `len` `asInt` `asString` … |
| `std::toml` | TOML document access, isomorphic to the JSON API |
| `std::regex` | patterns are cached host-side by string; ChaLi holds only an integer handle |
| `std::parse` | string → integer parsing with `ParseError` |
| `std::binary` | fixed-width decoding, byte order and bounds checking |
| `std::path` | path joining, file name, extension, normalisation |
| `std::compress` | text compress / decompress; the result is **binary** `List<uint8>`, never print it as a string |

### Growable arrays

| Module | Contents |
| --- | --- |
| `std::array` | ChaLi DynamicArray<T>: growable contiguous storage, reserve, resize, capacity and checked access |

### Concurrency

| Module | Contents |
| --- | --- |
| `std::thread` | isolated threads (`Thread`) |
| `std::message` | messages across threads |
| `std::concurrency` | shared slots `Shared<T>`, cross-thread strong references `Arc<T>` and weak references `ArcWeak<T>`. Ordinary globals stay isolated between threads |
| `std::event` | explicit events and timers; receive consumes notifications and coalesces missed periods |

> std::thread executes tasks, std::concurrency provides channels, and std::event provides explicit events.

`AtomicInt64.compareExchange(expected, desired)` checks and updates the current value in one atomic operation. It returns `true` after writing the new value. A mismatch returns `false` and leaves the current value in place.

### System and environment

| Module | Contents |
| --- | --- |
| `std::io` | file I/O with error type `IoError` (`OpenFailed` `ReadFailed` `WriteFailed` `SeekFailed` `Closed`) |
| `std::net` | TCP and HTTP clients; typed socket and listener interfaces use host network services |
| `std::sql` | MySQL / SQLite / PostgreSQL / SQL Server; SQL is executed by the engine, ChaLi sends and fetches |
| `std::process` | processes and system calls; command pipes are held by a host handle pool |
| `std::clock` | monotonic instants, nonnegative durations, and millisecond or nanosecond readings |
| `std::memory` | `Span<T>` (pointer + length view); its iterator returns **element pointers**, so `for (T mut& p in span)` writes directly |

Use `Instant::now().plus(Duration::fromMillis(250))` to create a deadline. Before each timed operation, call `deadline.remaining().toMillisCeil()` to share one timeout budget across several operations. The readings come from the monotonic clock.

`FileStream.readInto(&buffer)` reads into an existing `uint8[]` and returns the number of bytes read. `writeFrom(&buffer, count)` writes the specified prefix. `copyTo` reuses one buffer throughout the copy and leaves both streams open.

### Iteration

| Module | Contents |
| --- | --- |
| `std::iter` | `range`, lazy `map` / `filter` / `collect` chains, the `IterNext<T>` interface |

`std::probe` is a test stub, not for applications.

---

## 3. Usage conventions

**Follow each resource type's lifetime protocol.** Files, sockets and database connections support their documented explicit close or scope cleanup. JSON/TOML values own independent data; do not treat every value as a raw handle needing manual closure, or assume identical repeated-close results across types.

**Compressed output is binary.** `std::compress` output contains NUL bytes and has type
`std::core::List<uint8>`; only the decompressed result is a `std::core::String`.

Module availability follows the target host bindings, system libraries and build configuration. Embedded Linux uses ordinary Linux hosts without a separate module-blocking deployment mode. See [deployment](06-deployment.md).

**Modules other than core `std::core` require an explicit `import`.**

---

## 4. Where to find complete APIs

The standard library source is itself the best reference, and each module header documents its
capabilities:

- [`source/std/`](../../source/std) — all module sources, `source/std/<name>.chl`
- [`source/abi/`](../../source/abi) — host C ABI services and necessary C++ adapters; `abi_*.c` / `abi_*.cpp` implement services,
  and `aot_modules.inc` organizes demand-driven native bindings
- [`example/samples/`](../../example/samples/) — runnable samples organised by feature
- Per-domain API reference: [`doc/library/`](../library/README.md) has one page per module
  (`std-json.md`, `std-toml.md`, `std-regex.md`, `std-linear_algebra.md`, `std-complex.md`, `std-net.md`,
  `std-sql.md`, …)

---

## 5. Next

| You want | Where |
| --- | --- |
| Call your own C/C++ from ChaLi | [Host integration and C ABI](08-host-integration.md) |
| Capabilities beyond the standard library | [Language reference](04-language-reference.md) |
| Runnable samples for a feature | [`example/samples/`](../../example/samples/) |

0.14.0 adds `std::encoding`, `std::csv`, `std::date_time`, `std::cli`, `std::utf8` and extended file operations. See the [Any, explicit generics and data utilities](23-any-generics-data.md).
