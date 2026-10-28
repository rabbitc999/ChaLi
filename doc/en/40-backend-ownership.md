# AOT lowering switches and counters

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../40-backend-ownership.md)
<!-- END LANGUAGE NAVIGATION -->

After the ChaLi front end completes type, borrow and deterministic-cleanup lowering, quadruples go straight to LLVM AOT; LLVM performs all optimization. Independent value semantics for ordinary assignment and the exclusion between `Copy<T>` and `#NonCopyable` remain in force.

LLVM optimization for AOT is always fully enabled: static LLVM uses `default<O3>` and aggressive code generation; external LLVM uses `-O3`. Normal floating-point semantics remain enabled without fast-math. Safety checks and fallbacks still apply, and instrumentation is off by default.

## Retained AOT fast paths

The lowering layer keeps only ChaLi-specific fast paths that LLVM cannot replace:

- Aggregate word copies use a bulk path only when both full ranges are live and disjoint. Overlap or a failed range proof falls back to the original forward, per-word read-before-write order, including partial writes and traps. Zero length performs no access.
- Copies of plain 64-bit integer and 64-bit floating arrays can use the word-copy helper. Other layouts and very large counts retain the element loop. Existing checked allocation and initialization remain; no uninitialized-storage facility is introduced.
- A validated address is reused only on a straight-line path with an unchanged address symbol and exact access width. Calls, allocation, release, control-flow boundaries and stores invalidate the relevant facts.
- Eligible tiny internal AOT functions receive inline hints. A function value proven to name an environment-free function on the current straight-line path can use a direct call. Unknown targets, captured environments and facts invalidated by reentry retain the existing protocol.

Explicit step limits retain logical operation checks; default execution permits the normal fast paths. Existing division, overflow and shift semantics remain enabled.

## Comparison switches

Set a variable to a nonempty value during compilation to retain the corresponding original path:

| Variable | Disabled fast path |
| --- | --- |
| `CHALI_NO_ARRAY_COPY_BULK` | Word-copy path for plain 64-bit arrays |
| `CHALI_NO_AOT_BULK_COPY` | Native aggregate bulk-copy helper |
| `CHALI_NO_AOT_ADDRESS_CACHE` | Reuse of validated AOT addresses |
| `CHALI_NO_AOT_INLINE_HINT` | Tiny AOT function inline hints |
| `CHALI_NO_AOT_DIRECT_CALLS` | Direct calls of proven environment-free function values |

Case entry points: [language sample](../../example/samples/backend_ownership.chl), [generation-evidence and copy-boundary test](../../tests/chali_backend_ownership.py).

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

Records contain the operation, function, source location, quad index, execution count and known requested bytes. These count remaining lowered operations, not source-language semantic copies. Eliminated operations do not appear. Allocation bytes cover quad `alloc` requests, not every allocation inside host objects, heap residency or peak memory. Recursive cleanup and low-level release are different layers and must not be summed as user-destructor calls. Raw word transfers are distinct from deep copies of owned fields.
