# ChaLi 1.0.11 performance: previous version and language baselines

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-performance.md)
<!-- END LANGUAGE NAVIGATION -->

This report compares the original **ChaLi 1.0.10 release with the measured 1.0.11 implementation**, alongside frozen C++, Rust, Lua and Java JIT references, with older verifiable C, Go, Node and Python supplements. List and String reading beat Lua in this run. Map, Set, concatenation, slicing and search remain slower. Interfaces meet the 40/400 ms limits with the pool; the default malloc configuration misses the polymorphic limit in this measurement.

Optimization stops after the current LLVM List work. Unmet targets remain visible; historical language references retain their original identities and dates.

## Identity and environment

The new measurements were collected on **2026-10-29**, Windows 10 x64, AMD Ryzen 5 5600X, 6 cores and 12 logical processors. Both external-LLVM compilers use the same clang **23.1.4**, LLVM O3/LTO kernels and an `-O3 -DNDEBUG` C timing shell. Both use their default 512 MiB heap budget.

| Measured compiler | Source identity | SHA-256 |
| --- | --- | --- |
| Original 1.0.10 release, unchanged bytes | `001cdd6` | `35e6d2602054e757452ce570bf7fd6051a2b46d196e1e8ab2da3fa69b974ce6b` |
| Measured 1.0.11 implementation | `a522cf0` | `7f781f0ad808c2bfc5414302907b12fbb9e493d437b30a6695a1ceb87a6f736c` |

The benchmark driver snapshot is `0bb28ec`. [Final data](../../benchmarks/results/1.0.11-final/results.json) records commands, input hashes, compiler and kernel hashes, verification results and every raw process sample. Final documentation rebuilds embedded assets and the compiler; these measurements keep the identities above, while functional gates for the final documentation build are recorded separately.

The three complete configurations are **1.0.10 pool, 1.0.11 pool and 1.0.11 malloc**. Version 1.0.10 has no allocator switch. The first two compare versions with the same allocator; the malloc column preserves the system-allocator baseline. Results do not combine per-case winners from different configurations.

The isolated 1.0.10 ABI build needs its own AST-header include path and an explicit clang path. The driver extracts headers from the matching Git snapshot; it changes no release compiler, standard library, runtime or production LLVM generator. Both versions already generate LLVM from the semantically checked AST.

## Timing and verification

Each case/configuration uses **five independent processes, five samples each**. The median is taken over 25 nanosecond observations: 51 combinations and **1275 formal observations**. Each process warms once with seed 0, then measures seeds 0 through 4. Configuration order rotates between rounds.

Compilation, linking, module initialization and global setup are outside the interval. Local construction, growth, copying and cleanup inside `work` are timed. The basic fixed array is initialized inside work; feature globals and String input are initialized beforehand. Interface views and dispatch, concatenated results and slices remain inside work.

Every case has a CPU startup check; values at or above 40% defer timing. Recorded checks range from **1% to 27%**. These checks do not establish continuously idle hardware. Frequency and affinity were not fixed, and frozen references were collected on earlier dates.

Feature and String results are verified against independent models for five seeds. Basic cases verify frozen sinks; Map/Set references also include independent arithmetic models. The floating chain preserves the original weighted sink, `sink = sink * 0.5 + work(seed)`; integer shells sum the warmup and measured results. Failed processes or unexpected stderr are rejected.

Workload size, seeds, operation sequences and outputs match. Standard-library algorithms and optimizer decisions can differ. Small functions may inline, and equivalent redundant computation may disappear. Lua concatenation additionally uses the faster `table.concat` implementation of the same overall task.

## Code and workload sizes

| Workload and complete source | Scale and operations |
| --- | --- |
| [int_chain](../../benchmarks/perf/int_chain.chl) | 4,000,000 dependent integer updates |
| [float_chain](../../benchmarks/perf/float_chain.chl) | 1,000,000 dependent double multiply/add updates |
| [call](../../benchmarks/perf/call.chl) | 1,000,000 updates through a small function; inlining permitted |
| [array](../../benchmarks/perf/array.chl) | 4,000,000 reads from 16 elements; initialization inside work |
| [list_growth](../../benchmarks/perf/list_growth.chl) | 400,000 int64 pushes from empty, then indexed reduction |
| [map_growth](../../benchmarks/perf/map_growth.chl) | 20,000 sparse integer keys; insert, overwrite, hit/miss, delete/reinsert every third key |
| [set_growth](../../benchmarks/perf/set_growth.chl) | 20,000 sparse integer keys, duplicates, hit/miss, delete/reinsert |
| [enum_payload](../../benchmarks/features/enum_payload.chl) | 256 values, four payload variants, 262,144 dependent matches |
| [interface_mono](../../benchmarks/features/interface_mono.chl) | 262,144 calls through a derived interface, one implementation type |
| [interface_poly](../../benchmarks/features/interface_poly.chl) | 262,144 calls, four selector patterns across three implementation types |
| [struct_nested](../../benchmarks/features/struct_nested.chl) | 256 nested records, 262,144 field and fixed-array accesses |
| [array_2d](../../benchmarks/features/array_2d.chl) | 64×64 array, 262,144 dependent accesses |
| [array_3d](../../benchmarks/features/array_3d.chl) | 16×16×16 array, 262,144 accesses |
| [string_read](../../benchmarks/strings/cases.chl) | 8,448 UTF-8 bytes, 131,072 byteAt/unwrap reads and reduction |
| [string_concat](../../benchmarks/strings/cases.chl) | 2,048 chunks of 33 bytes, 67,584 output bytes, full output checksum |
| [string_slice](../../benchmarks/strings/cases.chl) | 1,024 slice pairs, widths 33/66/99/132 bytes, including comparison and cleanup |
| [string_find](../../benchmarks/strings/cases.chl) | 256 searches with start offsets: Chinese, emoji, ASCII and absent patterns |

Complete C++/Rust/Lua implementations are in [benchmarks/perf](../../benchmarks/perf), feature implementations and the Java shell in [benchmarks/features](../../benchmarks/features), and String implementations in [benchmarks/strings](../../benchmarks/strings). The [version driver](../../benchmarks/versions/run.py) builds and schedules programs; production LLVM IR comes from C++.

The complete exported List compute kernel is:

```chali
#export("f_work")
int64 work(int64 seed) {
    List<int64> xs;
    int64 i = 0;
    while (i < 400000) {
        xs.push(i * 3 + seed);
        i = i + 1;
    }
    int64 total = seed;
    i = 0;
    while (i < 400000) {
        total = (total * 31 + xs[i]) & 2147483647;
        i = i + 1;
    }
    return total;
}
```

It starts empty and includes growth, indexed reduction and scope cleanup. No implementation receives a workload-specific reserve. The shared [timing.c](../../benchmarks/features/timing.c) and [basic wrapper](../../benchmarks/perf/build_aot.py) document the timing boundary:

```c
const int64_t start = now_ns();
const int64_t value = kernel(seed);
const int64_t elapsed = now_ns() - start;
sink += value;
printf("rep %lld\n", (long long)elapsed);
```

## Frozen language references

No other language was remeasured in this run.

| Reference | Configuration | Retained evidence |
| --- | --- | --- |
| C++ | MinGW g++ 11.2.0, C++17, `-O2`; supplementary reference while ChaLi uses O3 | [C++ O2](../../benchmarks/results/1.0.11-final/references/cpp-o2.json), [String](../../benchmarks/results/1.0.11-final/references/strings.json) |
| Rust | rustc 1.98.1, `-O` | [Basic](../../benchmarks/results/1.0.11-final/references/basic.json), [features](../../benchmarks/results/1.0.11-final/references/features.json), [String](../../benchmarks/results/1.0.11-final/references/strings.json) |
| Lua | Official Lua 5.5.1 VM, default GC; faster table.concat reference for concatenation | [Basic](../../benchmarks/results/1.0.11-final/references/basic.json), [String](../../benchmarks/results/1.0.11-final/references/strings.json) |
| Java | HotSpot 22.0.1, `-Xbatch`, warmed steady-state interfaces | [Feature and warmup records](../../benchmarks/results/1.0.11-final/references/features.json) |

Each Java JVM performs at least 256 warmup calls and at least one second of warmup. Evidence verifies C2 level 4 compilation of work before sampling. Five JVMs per interface case retain separate [JIT logs](../../benchmarks/results/1.0.11-final/references/java-jit). Startup and interpreted execution are outside this comparison.

Most references were collected on 2026-10-28. The first four C++ O2 cases reuse 2026-10-27 observations without contemporaneous source hashes. Their original report checks initial repository source, tool major version, host and sink; this evidence limitation remains. Other C++ O2 cases were recorded on 2026-10-28. Lua table.concat is **4.1247 ms** versus **25.6472 ms** for repeated concatenation; the faster builder is the target reference.

## Basic computation and containers

All tables report medians in **ms; lower is faster**. A dash means no frozen corresponding reference.

| Workload | 1.0.10 pool | 1.0.11 pool | 1.0.11 malloc | C++ O2 | Rust -O | Lua 5.5.1 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `int_chain` | 0.8655 | 0.8634 | 0.8633 | 4.33 | 0.864 | 27.762 |
| `float_chain` | 1.2956 | 1.2955 | 1.3017 | 1.303 | 1.295 | 6.106 |
| `call` | 0.2156 | 0.2156 | 0.2156 | 0.903 | 1.083 | 16.955 |
| `array` | 76.4546 | 9.8599 | 9.878 | 6.998 | 6.22 | 41.553 |
| `list_growth` | 374.1419 | 3.4057 | 6.4306 | 2.137 | 1.624 | 11.722 |
| `map_growth` | 345.0664 | 179.256 | 169.6778 | 1.836 | 2.021 | 4.676 |
| `set_growth` | 291.6311 | 123.9572 | 148.9858 | 1.8816 | 1.554 | 4.563 |

Integer, floating-point and small-function results are effectively unchanged from 1.0.10; tiny last-digit differences are not treated as established improvements. The fixed array drops from 76.4546 to 9.8599 ms with the pool. List drops from 374.1419 to 3.4057 ms under the same allocator, an observed **109.86×** ratio. The malloc result, 6.4306 ms, also beats Lua at 11.722 ms.

Map and Set improve over 1.0.10 but still miss Lua: pool Map is 179.256 versus 4.676 ms, and Set is 123.9572 versus 4.563 ms. List success does not satisfy those separate conditions.

### Supplementary frozen C, Go, Node and Python baselines

These four basic cases retain 2026-10-27 observations: GCC 11.2.0 C at `-O2`, Go 1.27.1 with default optimization, Node 26.5.0/V8 and CPython 3.12.3. Their source matches archive snapshot `06478a5`, and their sinks match this run. None was remeasured.

| Workload | 1.0.11 pool | 1.0.11 malloc | C O2 | Go | Node / V8 | CPython |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `int_chain` | 0.8634 | 0.8633 | 4.322 | 4.657 | 72.138 | 341.159 |
| `float_chain` | 1.2955 | 1.3017 | 1.302 | 1.546 | 1.301 | 31.328 |
| `call` | 0.2156 | 0.2156 | 0.869 | 1.047 | 16.097 | 126.102 |
| `array` | 9.8599 | 9.878 | 7.145 | 7.252 | 60.173 | 212.077 |

The older report lacks contemporaneous source hashes. Retained hashes are explicitly archive-source hashes, not sampling-time hashes. Node has the original single warmup rather than the Java interface C2 evidence. These supplements do not participate in the Lua/interface conditions. [Raw supplementary references](../../benchmarks/results/1.0.11-final/references/supplementary-basic.json) retain tools, samples and limitations; missing container or feature cases are not estimated.

## Payload enums, interfaces, nested records and arrays

| Workload | 1.0.10 pool | 1.0.11 pool | 1.0.11 malloc | C++ O2 | Rust -O | Java JIT |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `enum_payload` | 338.9185 | 0.8353 | 0.8329 | 1.6884 | 1.214 | — |
| `interface_mono` | 1205.4122 | 33.1437 | 33.9196 | 0.2825 | — | 0.2845 |
| `interface_poly` | 2350.4833 | 314.9402 | 437.8846 | 0.5095 | — | 1.0682 |
| `struct_nested` | 2164.7936 | 1.7447 | 1.7991 | 1.1711 | 1.0543 | — |
| `array_2d` | 3.0841 | 0.6521 | 0.6445 | 0.5668 | 0.5433 | — |
| `array_3d` | 3.1952 | 0.7649 | 0.7802 | 0.6806 | 0.6277 | — |

Enum and nested-record gains include several changes across versions: reduced materialization, refined scalar storage, address views and data-area snapshots for proven pure accesses. These observations do not isolate each mechanism. Borrowed matching, nested field access and native multidimensional arrays retain the original verification.

Both monomorphic results meet 40 ms. Polymorphic pool at 314.9402 ms meets 400 ms; malloc at 437.8846 ms misses it. Both remain much slower than warmed Java JIT. There is no Lua interface reference.

## String

| Workload | 1.0.10 pool | 1.0.11 pool | 1.0.11 malloc | C++ O2 | Rust -O | Lua 5.5.1 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `string_read` | 3610.6336 | 2.5745 | 2.5599 | 0.5427 | 0.5437 | 6.0014 |
| `string_concat` | 1865.9993 | 6.662 | 14.2666 | 0.1012 | 0.0699 | 4.1247 |
| `string_slice` | 2068.8784 | 9.6465 | 9.7462 | 0.1329 | 0.1323 | 0.3515 |
| `string_find` | 12212.2159 | 14.1769 | 11.7448 | 0.1123 | 0.0451 | 0.1703 |

Inputs include Chinese and emoji with UTF-8 byte semantics. Reading drops from 3610.6336 ms in 1.0.10 pool to 2.5745 ms in 1.0.11 pool; malloc is 2.5599 ms. Both beat frozen Lua at 6.0014 ms. This is the combination of existing changes, including scalar immediate byteAt/unwrap and String/address fast paths.

Concatenation, slicing and search still miss their Lua references. Owned results, copying, temporaries, storage access and search implementations remain relevant costs; no single cause is established by this run.

## Allocators and conditions

Pool has lower medians in 10 of 17 cases. The equal-weight geometric time ratio is **pool/malloc = 0.9028**, approximately **9.72%** lower. Malloc wins other cases, including Map, String reading and search, so both complete columns remain. The default malloc policy has not changed. This aggregate allocator comparison never overrides an individual condition.

| Uniform configuration | Cases with conditions | Passed | Missed | Unassessed |
| --- | ---: | ---: | ---: | ---: |
| 1.0.11 pool | 13 | 8 | 5 | 4 |
| 1.0.11 malloc | 13 | 7 | 6 | 4 |

Conditions are strictly faster than Lua where a reference exists, monomorphic ≤40 ms and polymorphic ≤400 ms. C++ O2 is informational. Payload enum, nested record, 2D and 3D arrays have no independent limits. Saved-data evaluations are [pool](../../benchmarks/results/1.0.11-final/gate-pool/results.json) and [malloc](../../benchmarks/results/1.0.11-final/gate-malloc/results.json); neither configuration passes every condition.

## List optimization progression

![List optimization progression](../images/performance-list-1.0.11.png)

The chart keeps LLVM O3, malloc and the same List workload. Relative speed is the starting-stage time divided by each stage time. Stages cover fused append, in-place construction, packed numeric slots and direct LLVM access; the final point is 6.4306 ms from this formal comparison. Earlier points come from different times, not an isolated causal experiment.

The host numeric slot shrinks from 104 to 16 bytes on this platform; total memory also includes headers, capacity and allocator metadata. LLVM hits read slots or append within existing capacity directly, retaining complete-handle, object-store revision, bounds and capacity checks. Growth, element borrowing, owned values and unsupported layouts use compatible paths. Functional coverage includes bool, char, all integer widths and float32/64; the timed containers specifically use int64.

The current branch retains the static image, final measurements and final references. Intermediate observations, editable plot data and temporary scripts are removed. Earlier observations remain traceable in the preserved Git history.

## Reproduction and limits

Run from the Windows x64 repository root with the verified compilers:

```powershell
python benchmarks/versions/run.py --build-only --new build/perf-baselines/a522cf0/chali-1.0.11-llvm-external-windows-x64.exe --new-source a522cf0 --out tmp/reproduce-performance
python benchmarks/versions/run.py --measure-only --out tmp/reproduce-performance
python benchmarks/evaluate_saved.py --reports benchmarks/results/1.0.11-final/results.json --configuration 1.0.11-pool --out tmp/recheck-pool
```

Build and verify before timing. Temporary artifact paths in the final report record the actual commands used; rebuild for reproduction after cleanup. The saved-data evaluator runs no benchmark and does not turn missed conditions into passes.

These are 17 fixed microbenchmarks. Startup, compilation speed, RSS, full applications, dedicated iterator workloads and dedicated closure workloads are outside the measurements. Windows x64 numbers do not establish performance on other platforms. The measured source passed targeted layout, 16 view/fallback combinations, complete AOT, callback and capacity checks; see the [source-stage evidence](../../tools/history/20261029/chali-1.0.11-numeric-list-llvm/README.md). This performance/documentation snapshot regenerates assets and the compiler without running the full gate. Later revisions of the same version must not relabel these measurements. Subsequent functionality and validation remain in TASK.md.
