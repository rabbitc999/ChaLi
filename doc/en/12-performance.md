# LLVM AOT performance across languages

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-performance.md)
<!-- END LANGUAGE NAVIGATION -->

The current program version is **ChaLi 1.0.8**. This page answers one question: once ChaLi hands its quads to LLVM AOT, where do the same algorithms stand against C, C++, Rust, Go, Java, Node, Lua and Python. The 1.0.8 compiler no longer embeds a VM interpreter, so there is no "VM versus AOT" section.

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

On the ChaLi side the compute function is exported and the timing stays in C:

```powershell
chali aot work.chl --shared --out tmp/int_chain-kernel
clang -O2 -DNDEBUG driver.c int_chain-kernel.dll -lm -o int_chain-aot.exe
```

`driver.c` is the C case itself with the measured function replaced by the `#export`ed `f_work`; the timing function and the `sink` merge are identical on both sides.

## Measured for 1.0.8

Windows x64, external LLVM variant (clang 23.1.4, AOT at `-O3`); 5 processes × 5 rounds per configuration, milliseconds:

| Configuration | int_chain | float_chain | call | array | list |
| --- | ---: | ---: | ---: | ---: | ---: |
| ChaLi 1.0.8 (AOT, unchecked export) | 91.306 | 23.047 | 22.746 | 174.833 | 398.753 |
| ChaLi 1.0.8 (AOT, checked `run`) | 93.570 | 23.095 | 23.384 | 175.028 | 406.467 |
| C (gcc -O2) | 4.326 | 1.301 | 0.864 | 6.929 | 1.015 |
| C++17 (g++ -O2) | 4.337 | 1.302 | 0.866 | 6.938 | 0.963 |
| C++17 (g++ -O3) | 4.319 | 1.302 | 0.867 | 6.930 | 0.945 |
| Rust (rustc -O) | 0.864 | 1.300 | 1.089 | 6.263 | 0.945 |
| Go (default optimization) | 4.677 | 1.559 | 1.048 | 7.000 | 1.065 |
| Java 22 `-Xint` (interpreter only) | 75.170 | 15.583 | 30.593 | 76.576 | 14.116 |
| Java 22 `-Xbatch` (JIT) | 5.050 | 1.301 | 1.085 | 6.209 | 1.480 |
| Node 26 (V8 JIT) | 71.781 | 1.297 | 15.943 | 59.336 | 8.586 |
| Lua 5.5.1 (official VM, default GC) | 31.094 | 6.472 | 16.940 | 41.805 | 12.402 |
| CPython 3.12 (interpreter only) | 320.579 | 30.111 | 117.605 | 205.620 | 55.950 |

Multiples of C (same column divided by the C median):

| Configuration | int_chain | float_chain | call | array | list |
| --- | ---: | ---: | ---: | ---: | ---: |
| ChaLi (unchecked export) | 21.1 | 17.7 | 26.3 | 25.2 | 392.7 |
| ChaLi (checked `run`) | 21.6 | 17.8 | 27.1 | 25.3 | 400.3 |
| Rust | 0.2 | 1.0 | 1.3 | 0.9 | 0.9 |
| Go | 1.1 | 1.2 | 1.2 | 1.0 | 1.0 |
| Java `-Xint` | 17.4 | 12.0 | 35.4 | 11.1 | 13.9 |
| Java `-Xbatch` | 1.2 | 1.0 | 1.3 | 0.9 | 1.5 |
| Node 26 | 16.6 | 1.0 | 18.5 | 8.6 | 8.5 |
| Lua 5.5.1 | 7.2 | 5.0 | 19.6 | 6.0 | 12.2 |
| CPython 3.12 | 74.1 | 23.2 | 136.1 | 29.7 | 55.1 |

## Reading the table

**The four scalar loads land between 17× and 26×.** They sit close together, so the bottleneck is not one operator but the path every quad takes: read the operands out of their slots, compute, write the result back. The equivalent C loop is "a register and one multiply-add"; the ChaLi loop also carries loads, stores and type-slot bookkeeping, and LLVM's O3 can optimize within that shape but cannot replace it.

**`list` is an order of magnitude more expensive.** Each of the 400,000 `push` calls goes through a runtime allocation and registers a value slot, then the traversal reads every element back, which lands at 400 ms — slower than CPython's built-in `list` (55.950). This is not "LLVM optimizing too little" but call density: `int_chain` does one multiply-add per round, `list` walks "allocate → store → growth callback" per element.

**Checked and unchecked differ by only 0.3–2.2%.** Bounds and liveness checks are a small share of the total, because slot access and runtime calls dominate. Turning checks off will not rescue these five loads; only a different lowering shape would.

**Rust beats C by 5× on `int_chain`** (0.864 against 4.326) because LLVM rewrites that recurrence into a stronger form; the same LLVM on the same machine could do it for ChaLi, provided the IR looks like a loop instead of a series of global variable accesses.

**Java `-Xint` beats ChaLi on `int_chain`** (75.170 against 91.306), and Node is close behind. That is the most important line of this round: the interpreter is fast because its values stay in registers and frames, while the current AOT output hands LLVM memory slots.

## Relation to 1.0.7

The order of magnitude did not change: 1.0.7 reported 21.9, 17.4, 26.0, 24.7 and 492.5×, this round measures 21.1, 17.7, 26.3, 25.2 and 392.7×. `list` moved from 492 to 393; the other four are within noise.

What actually changed in 1.0.8 is **compile time**: the internal quad optimizer and the aggregate optimizer were removed entirely, cutting about 0.2 s per compile (2.79 s against 3.01 s for the same source on the same machine), while runtime stayed within ±3.5%. The table above explains why: runtime cost is decided by the lowering shape, quad-level simplifications are fully covered by LLVM O3, and keeping them only spent compile time. The 1.0.8 compiler therefore no longer has `CHALI_NO_OPT`-style internal switches; only the five AOT fast-path switches remain (see [AOT lowering switches and counters](40-backend-ownership.md)).

## Reproducing

```powershell
# Every language, 5 processes × 5 rounds
python benchmarks/perf/run.py --runs 5 --out benchmarks/results/aot-1.0.8

# ChaLi against native references only
python benchmarks/perf/run.py --only chali-run,chali-aot,c,cpp-o3,rust,go
```

Lua needs its host built first (the first run downloads the official sources): `python benchmarks/perf/build_lua.py`. Raw samples, checksums, the compiler SHA256 and every tool version are in the [1.0.8 benchmark report](../../benchmarks/results/aot-1.0.8/README.md) and [results.json](../../benchmarks/results/aot-1.0.8/results.json).

## Boundaries

- One machine, one architecture: Windows 10 x64 on a 12-thread Ryzen. There is no Linux or ARM data and none of this transfers to those platforms.
- Sample size: 25 execution samples per cell, median reported; process start-up, loading and JIT warm-up stay outside the timed region, which flatters Node and Java compared with a cold start.
- The loads are small: five micro-loops of a few million iterations, describing **call and access density**, not the mixed load of a real program.
- The "×" values divide one column by the C median and cannot be compared across columns; results such as Rust beating C hold only for this source.

## Historical reports

[1.0.7 general AOT comparison](../../benchmarks/results/aot-1.0.7/README.md), [1.0.6 general AOT comparison](../../benchmarks/results/aot-1.0.6/README.md), [1.0.6 default containers](../../benchmarks/results/containers-1.0.6/README.md) and [1.0.7 containers and strings](../../benchmarks/results/raw-containers-1.0.7/README.md) keep the real settings and raw samples of their own versions; older data includes a VM and an internal optimization budget and is not part of this comparison.
