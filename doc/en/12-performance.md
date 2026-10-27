# LLVM AOT performance across languages

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-performance.md)
<!-- END LANGUAGE NAVIGATION -->

The current program version is **ChaLi 1.0.8**. This page answers one question: once ChaLi hands its quads to LLVM AOT (`-O3` plus LTO), where do the same algorithms stand against C, C++, Rust, Go, Java, Node, Lua and Python. The 1.0.8 compiler no longer embeds a VM interpreter, so there is no "VM versus AOT" section.

## What is measured

Five loads, each written as "one compute function plus one timing shell":

| Load | Algorithm | What it stresses |
| --- | --- | --- |
| `int_chain` | 4,000,000 rounds of `acc = (acc * 48271 + i) & 2147483647` | Integer dependency chain, every round depends on the previous one |
| `float_chain` | 4,000,000 rounds of fused float multiply-add | FP dependency chain, register allocation |
| `call` | 4,000,000 calls to a tiny function | Calling convention and inlining decisions |
| `array` | 4,000,000 reads of `a[acc & 15]` | Fixed-array indexing and bounds |
| `list` | 400,000 `push` calls, then a traversal | Container growth and heap allocation |

Each program brackets `work()` with a monotonic clock and prints five `rep <nanoseconds>` lines plus one `sink <checksum>` line. The driver only builds, schedules and takes medians; **checksums must agree across languages**, otherwise the run fails — that is the only evidence that every language did the same work.

On the ChaLi side the compute function is exported and the timing stays in C. Both the kernel and the C shell are built by LLVM at `-O3` (the kernel with `-O3 -flto=full`, the shell with `-O3 -DNDEBUG`), so the shell's optimization level cannot skew the comparison:

```powershell
chali aot work.chl --shared --out tmp/int_chain-kernel
clang -O3 -DNDEBUG driver.c int_chain-kernel.dll -lm -o int_chain-aot.exe
```

`driver.c` is the C case itself with the measured function replaced by the `#export`ed `f_work`; the timing function and the `sink` merge are identical on both sides.

## Measured for 1.0.8

Windows 10 x64 on a 12-thread Ryzen, external LLVM variant (clang 23.1.4); 5 processes × 5 rounds per configuration, milliseconds:

| Configuration | int_chain | float_chain | call | array | list |
| --- | ---: | ---: | ---: | ---: | ---: |
| ChaLi 1.0.8 (AOT, unchecked export) | 97.047 | 23.997 | 23.714 | 178.331 | 411.435 |
| ChaLi 1.0.8 (AOT, checked `run`) | 100.273 | 23.973 | 24.447 | 188.494 | 444.205 |
| C (gcc -O2) | 4.322 | 1.302 | 0.869 | 7.145 | 0.904 |
| C++17 (g++ -O2) | 4.330 | 1.303 | 0.903 | 6.998 | 0.990 |
| C++17 (g++ -O3) | 4.348 | 1.303 | 0.917 | 7.082 | 0.856 |
| Rust (rustc -O) | 0.864 | 1.301 | 1.087 | 6.264 | 1.063 |
| Go (default optimization) | 4.657 | 1.546 | 1.047 | 7.252 | 1.553 |
| Java 22 `-Xint` (interpreter only) | 64.815 | 15.827 | 29.804 | 71.175 | 13.884 |
| Java 22 `-Xbatch` (JIT) | 5.049 | 1.313 | 1.088 | 6.322 | 1.575 |
| Node 26 (V8 JIT) | 72.138 | 1.301 | 16.097 | 60.173 | 8.973 |
| Lua 5.5.1 (official VM, default GC) | 26.777 | 6.544 | 17.391 | 39.848 | 12.482 |
| CPython 3.12 (interpreter only) | 341.159 | 31.328 | 126.102 | 212.077 | 57.961 |

Multiples of C (same column divided by the C median):

| Configuration | int_chain | float_chain | call | array | list |
| --- | ---: | ---: | ---: | ---: | ---: |
| ChaLi (unchecked export) | 22.5 | 18.4 | 27.3 | 25.0 | 455.1 |
| ChaLi (checked `run`) | 23.2 | 18.4 | 28.1 | 26.4 | 491.4 |
| C++17 (g++ -O3) | 1.0 | 1.0 | 1.1 | 1.0 | 0.9 |
| Rust | 0.2 | 1.0 | 1.3 | 0.9 | 1.2 |
| Go | 1.1 | 1.2 | 1.2 | 1.0 | 1.7 |
| Java `-Xint` | 15.0 | 12.2 | 34.3 | 10.0 | 15.4 |
| Java `-Xbatch` | 1.2 | 1.0 | 1.3 | 0.9 | 1.7 |
| Node 26 | 16.7 | 1.0 | 18.5 | 8.4 | 9.9 |
| Lua 5.5.1 | 6.2 | 5.0 | 20.0 | 5.6 | 13.8 |
| CPython 3.12 | 78.9 | 24.1 | 145.1 | 29.7 | 64.1 |

## Reading the table

**The four scalar loads land between 18× and 28×.** They sit close together, so the bottleneck is not one operator but the path every quad takes: read the operands out of their slots, compute, write the result back. The equivalent C loop is "a register and one multiply-add"; the ChaLi loop also carries loads, stores and type-slot bookkeeping, and LLVM's `-O3` can optimize within that shape but cannot replace it.

**`list` is an order of magnitude more expensive.** Each of the 400,000 `push` calls goes through a runtime allocation and registers a value slot, then the traversal reads every element back, which lands at 400 ms — seven times slower than CPython's built-in `list` (57.961). This is not "LLVM optimizing too little" but call density: `int_chain` does one multiply-add per round, `list` walks "allocate → store → growth callback" per element.

**Checked and unchecked differ by 0–8%.** Bounds and liveness checks are a small share of the total, because slot access and runtime calls dominate; `float_chain` is identical (23.973 against 23.997) and `list`, which has a check point per element, shows the widest gap (8%) — yet turning checks off only takes 444 ms down to 411 ms. Disabling checks will not rescue these five loads; only a different lowering shape would.

**Rust beats C by 5× on `int_chain`** (0.864 against 4.322) because LLVM rewrites that recurrence into a stronger form; the same LLVM on the same machine could do it for ChaLi, provided the IR looks like a loop instead of a series of global variable accesses.

**Java `-Xint` and Node both beat ChaLi on `int_chain`** (64.815 and 72.138 against 97.047). That is the most important line of this round: the interpreter is fast because its values stay in registers and frames, while the current AOT output hands LLVM memory slots. On `float_chain` Node matches C (1.301) while ChaLi trails by 18×, so the cost of slot access is not specific to integers.

## Where the numbers stop

This machine occasionally takes a scheduling hit, and long tasks are hit more often: in one `int_chain` cell of 25 samples, ChaLi AOT ranges from 91.038 (minimum) through 97.047 (median) to 131.7 (maximum), while C stays between 4.31 and 4.35. Therefore:

- the multiples above use **medians**, which keeps them comparable with the 1.0.7 report;
- to compare across sessions, align on **minimums**: this round's minimum-based multiples for the four scalar loads are 21.1, 17.5, 26.4 and 24.2, matching the previous report's medians of 21.1, 17.7, 26.3 and 25.2 — the scalar conclusion is stable. `list` reads 507.3 this round and 392.7 last round, because the C baseline itself (about 0.9 ms) moves by tens of percent;
- `list` ratios are the least reliable: C's own `list` case is under a millisecond, so a 10% wiggle moves the ratio by tens of times. Prefer absolute times there.

## Relation to 1.0.7

The order of magnitude did not change: 1.0.7 reported 21.9, 17.4, 26.0, 24.7 and 492.5×, this round measures 22.5, 18.4, 27.3, 25.0 and 455.1×. `list` moved from 492 to 455; the other four are within noise.

What actually changed in 1.0.8 is **compile time**: the internal quad optimizer and the aggregate optimizer were removed entirely, cutting about 0.2 s per compile (2.79 s against 3.01 s for the same source on the same machine), while runtime stayed within ±3.5%. The table above explains why: runtime cost is decided by the lowering shape, quad-level simplifications are fully covered by LLVM `-O3`, and keeping them only spent compile time. The 1.0.8 compiler therefore no longer has `CHALI_NO_OPT`-style internal switches; only the five AOT fast-path switches remain (see [AOT lowering switches and counters](40-backend-ownership.md)).

## Reproducing

```powershell
# Every language, 5 processes × 5 rounds
python benchmarks/perf/run.py --runs 5 --out benchmarks/results/aot-1.0.8

# ChaLi against native references only
python benchmarks/perf/run.py --only chali-run,chali-aot,c,cpp-o3,rust,go
```

The external LLVM variant needs clang on `PATH` (AOT asset preparation uses it too). Lua needs its host built first (the first run downloads the official sources): `python benchmarks/perf/build_lua.py`. Raw samples, checksums, the compiler SHA256 and every tool version are in the [1.0.8 benchmark report](../../benchmarks/results/aot-1.0.8/README.md) and [results.json](../../benchmarks/results/aot-1.0.8/results.json).

## Boundaries

- One machine, one architecture: Windows 10 x64 on a 12-thread Ryzen. There is no Linux or ARM data and none of this transfers to those platforms.
- Sample size: 25 execution samples per cell, median reported; process start-up, loading and JIT warm-up stay outside the timed region, which flatters Node and Java compared with a cold start.
- The loads are small: five micro-loops of a few million iterations, describing **call and access density**, not the mixed load of a real program.
- The "×" values divide one column by the C median and cannot be compared across columns; results such as Rust beating C hold only for this source.

## Historical reports

[1.0.7 general AOT comparison](../../benchmarks/results/aot-1.0.7/README.md), [1.0.6 general AOT comparison](../../benchmarks/results/aot-1.0.6/README.md), [1.0.6 default containers](../../benchmarks/results/containers-1.0.6/README.md) and [1.0.7 containers and strings](../../benchmarks/results/raw-containers-1.0.7/README.md) keep the real settings and raw samples of their own versions; older data includes a VM and an internal optimization budget and is not part of this comparison.
