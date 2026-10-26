# Backend ownership optimization and AOT counters

**English** · [简体中文](../43-backend-ownership.md)

ChaLi 1.0.6 optimizes aggregate transfers after type, borrow and deterministic-cleanup lowering, with LLVM AOT as the primary performance target. Independent value semantics and the exclusion between `Copy<T>` and `#NonCopyable` remain in force.

All applicable AOT optimizations are enabled by default. Static LLVM uses `default<O3>` and aggressive code generation; external LLVM uses `-O3`. Quad optimization retains its full budget. Normal floating-point semantics remain enabled without fast-math. Safety preconditions and fallbacks still apply, and instrumentation is off by default.

## Destination storage

For newly allocated aggregate initialization whose destination has not escaped, an internal function variant can write directly into caller storage. This removes an intermediate return transfer and source clearing. Tuple and enum construction can use this path while preserving tag initialization.

Original entries remain available for exports, function values and indirect calls. Assignment over an existing object, self-referential initialization, unsupported return forms and aggregates with observable user destruction retain the original path. Returning an existing borrowed value still requires an independent copy. The hidden argument is a language-runtime logical address; the public C ABI is unchanged.

Fresh aggregate receivers can also be constructed in their own temporary storage and are cleaned up at the end of the full expression. Field and index reads do not acquire ownership through this optimization.

An enum constructor with one owned aggregate payload can consume a proven-exclusive fresh temporary without observable user destruction, clearing its owned source slots instead of deep-copying and then cleaning up that temporary. Existing variables, borrowed values and unsupported payloads still copy.

## AOT and quad optimizations

- Aggregate word copies use a bulk path only when both full ranges are live and disjoint. Overlap or a failed range proof falls back to the original forward, per-word read-before-write order, including partial writes and traps. Zero length performs no access.
- Copies of plain 64-bit integer and 64-bit floating arrays can use the word-copy helper. Other layouts and very large counts retain the element loop. Existing checked allocation and initialization remain; no uninitialized-storage facility is introduced.
- A validated address is reused only on a straight-line path with an unchanged address symbol and exact access width. Calls, allocation, release, control-flow boundaries and stores invalidate the relevant facts.
- Identical repeated checks are removed only within a basic block without invalidation; the first check stays in place.
- Eligible tiny internal AOT functions receive inline hints. A function value proven to name an environment-free function on the current straight-line path can use a direct call. Unknown targets, captured environments and facts invalidated by reentry retain the existing protocol.

Watchdog paths conservatively disable fast paths without a proof of equivalent logical safety points. VM non-loop calls retain their existing entry, breakpoint and call-stack identity. Existing division, overflow and shift semantics remain enabled.

## Optional counters

Compile with `CHALI_BACKEND_PROFILE=1` to insert counters at ownership-related quad sites. At execution, set `CHALI_BACKEND_PROFILE_OUTPUT` to a JSON path. Normal process exit writes accumulated counts from all threads.

```powershell
$env:CHALI_BACKEND_PROFILE='1'
chali aot example/samples/backend_ownership.chl --out tmp/ownership-profile
Remove-Item Env:CHALI_BACKEND_PROFILE
$env:CHALI_BACKEND_PROFILE_OUTPUT="$PWD/tmp/ownership-profile.json"
& ./tmp/ownership-profile.exe
python tools/backend_profile.py tmp/ownership-profile.json --top 20
```

Default builds contain no counter calls. Use instrumented builds to investigate costs and separately built, uninstrumented programs for timing. Setting the output variable does not instrument an ordinary binary.

Records contain the operation, function, source location, optimized quad index, execution count and known requested bytes. These count remaining lowered operations, not source-language semantic copies. Eliminated operations do not appear. Allocation bytes cover quad `alloc` requests, not every allocation inside host objects, heap residency or peak memory. Recursive cleanup and low-level release are different layers and must not be summed as user-destructor calls. Raw word transfers are distinct from deep copies of owned fields.

## Comparison switches

Set a variable to a nonempty value during compilation to retain the corresponding original path:

| Variable | Disabled optimization |
| --- | --- |
| `CHALI_NO_AGGREGATE_OPT` | Aggregate destination variants and fresh temporary destination merging |
| `CHALI_NO_ARRAY_COPY_BULK` | Word-copy path for plain 64-bit arrays |
| `CHALI_NO_AOT_BULK_COPY` | Native aggregate bulk-copy helper |
| `CHALI_NO_AOT_ADDRESS_CACHE` | Reuse of validated AOT addresses |
| `CHALI_NO_AOT_INLINE_HINT` | Tiny AOT function inline hints |
| `CHALI_NO_AOT_DIRECT_CALLS` | Direct calls of proven environment-free function values |
| `CHALI_NO_OPT_CHECKS` | Repeated-check elimination within a basic block |

Existing quad and machine optimization switches remain available. Unsupported cases fall back automatically and do not require unsafe user code.

Focused checks: [language sample](../../example/samples/backend_ownership.chl), [generation and copy-boundary tests](../../tests/chali_backend_ownership.py).
