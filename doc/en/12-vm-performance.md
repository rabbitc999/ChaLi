# VM and LLVM AOT performance measurements

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-vm-performance.md)
<!-- END LANGUAGE NAVIGATION -->

The current program version is **ChaLi 1.0.5**. The separate [1.0.5 bootstrap container report](../../benchmarks/results/bootstrap-containers-1.0.5/README.md) compares List, Map and Set written in ChaLi against the default containers using the same compiler, with VM, LLVM AOT and raw samples.

The [slot Map/Set and segmented List AOT comparison](../../benchmarks/results/slot-segmented-1.0.5/README.md) tests a `{ Value, HasValue, HashKey }` Map, an independent Set slot array, and a List using a segmented deque layout. This campaign samples AOT separately and preserves the actual compiler version and raw measurements.

The historical default-container call-path baseline below is **ChaLi 0.31.0**. Measurements retain the actual binary versions and are not relabeled as 1.0.5. Default List, Map and Set workloads were measured before and after the call-path changes on the same host. C++17, CPython and the official Lua VM were measured in the same campaign.

[Container raw samples](../../benchmarks/results/containers-0310/results.json)

## Method and configuration

Each configuration has three processes, one warm-up and five timed repetitions per process: 15 execution samples per workload. Configuration order changes between rounds. Work includes setup, allocation and destruction; compiler startup and compilation are outside the internal timer. Process wall times and build times remain in the raw report.

| Item | Configuration |
| --- | --- |
| Host | Windows-10-10.0.19045-SP0 |
| CPU | AMD64 Family 25 Model 33 Stepping 2, AuthenticAMD, 12 logical processors |
| C++ | g++ (x86_64-posix-seh-rev3, Built by MinGW-W64 project) 11.2.0，`-std=c++17 -O2 -DNDEBUG` |
| Python | 3.12.3 (tags/v3.12.3:f6650f9, Apr  9 2024, 14:05:25) [MSC v.1938 64 bit (AMD64)] |
| Lua | Lua 5.5.1 (official VM, monotonic clock host) |
| LLVM | clang version 23.1.4 (https://github.com/llvm/llvm-project.git 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)，`-O2` |
| VM | Highest defaults; reflection enabled; full quadruple optimizer with maxRounds = 4 |
| AOT | LLVM machine code with default bounds and live-memory checks; shared standard-library runtime |

Both container binaries report 0.31.0. Before SHA256: `a7138cfa20ba4558c14622b4cfda206cb4cbe68489661b7aaf04e5bc6f945897`; after SHA256: `f7b7067bb5d130a3324a267d450e5acc2afee3717bdb7afc61f81ab48832e8a9`.

## Default-container workloads

All container elements, keys and values are int64. Each work call performs 50,000 primary loop iterations; read/update workloads prefill 1,024 entries. Build workloads insert distinct entries and read or query them; removal workloads build then empty the container. Set read queries alternate between the populated and missing halves of a 2,048-key range; Set write repeatedly inserts 1,024 distinct keys. Independent closed-form checksum formulas validate all configurations.

C++ uses std::vector, std::unordered_map and std::unordered_set with default growth and no reserve. Python uses list/dict/set. Lua uses tables for sequences and maps and key-to-true entries with an explicit count for sets; dense integer keys may use its array part. These tables compare the specified programs and their default allocation strategies. C++ timing enters work through a volatile function pointer to prevent moving the computation outside the timer.

[Sources and generator](../../benchmarks/perf/container_cases.py)；[Driver](../../benchmarks/perf/containers.py)

## Before/after container execution times (ms)

| Workload | VM before | VM after | VM change | AOT before | AOT after | AOT change |
| --- | --- | --- | --- | --- | --- | --- |
| list_build | 99.536 | 96.331 | -3.22% | 89.041 | 79.688 | -10.50% |
| list_read | 53.255 | 49.111 | -7.78% | 44.888 | 39.617 | -11.74% |
| list_write | 108.079 | 104.797 | -3.04% | 102.610 | 84.162 | -17.98% |
| list_remove | 98.663 | 100.161 | +1.52% | 87.753 | 78.786 | -10.22% |
| map_build | 170.456 | 167.010 | -2.02% | 136.872 | 111.804 | -18.31% |
| map_read | 72.379 | 69.118 | -4.51% | 54.050 | 46.670 | -13.65% |
| map_write | 148.790 | 142.564 | -4.18% | 114.718 | 94.702 | -17.45% |
| map_remove | 216.351 | 208.842 | -3.47% | 186.062 | 159.732 | -14.15% |
| set_build | 142.692 | 132.748 | -6.97% | 127.577 | 113.788 | -10.81% |
| set_read | 56.687 | 55.038 | -2.91% | 48.998 | 43.586 | -11.04% |
| set_write | 56.251 | 51.911 | -7.72% | 49.137 | 43.168 | -12.15% |
| set_remove | 204.048 | 191.572 | -6.11% | 187.339 | 162.720 | -13.14% |

| Container | VM time reduction | AOT time reduction |
| --- | --- | --- |
| List | 3.19% | 12.67% |
| Map | 3.55% | 15.92% |
| Set | 5.94% | 11.79% |

The primary VM List removal result is approximately 1.5% slower. An independent seven-process review gives -1.33% instead; both datasets are retained. This small VM change is not established as a consistent speedup. Aggregate reductions above use the geometric mean of speed ratios from the original four workloads per container, without replacing samples. [Review samples](../../benchmarks/results/containers-0310/list-remove-review.json)

The shared container glue borrows native arguments and reads slots on demand, removing an extra temporary vector and copy on each call. Internal combined operations use stack arrays. The AOT bridge reserves argument capacity once. Argument counts, value semantics, lifetimes, bounds checks and key policies remain covered by the VM/AOT regression suite.

## Same-campaign language comparison (ms)

| Workload | VM | LLVM AOT | C++ | Python | Lua |
| --- | --- | --- | --- | --- | --- |
| list_build | 96.331 | 79.688 | 0.287 | 3.431 | 1.469 |
| list_read | 49.111 | 39.617 | 0.023 | 2.362 | 0.445 |
| list_write | 104.797 | 84.162 | 0.024 | 4.297 | 0.880 |
| list_remove | 100.161 | 78.786 | 0.289 | 3.639 | 2.966 |
| map_build | 167.010 | 111.804 | 3.096 | 6.105 | 1.114 |
| map_read | 69.118 | 46.670 | 0.119 | 2.827 | 0.418 |
| map_write | 142.564 | 94.702 | 0.206 | 5.183 | 0.779 |
| map_remove | 208.842 | 159.732 | 3.115 | 7.674 | 1.455 |
| set_build | 132.748 | 113.788 | 2.921 | 4.284 | 1.000 |
| set_read | 55.038 | 43.586 | 0.154 | 2.475 | 0.556 |
| set_write | 51.911 | 43.168 | 0.193 | 2.322 | 0.416 |
| set_remove | 191.572 | 162.720 | 2.999 | 5.702 | 1.311 |

## AOT versus VM execution performance and VM language comparisons

All ratios below divide the optimized VM execution time by the corresponding implementation time for the same workload. An AOT speedup of 1.20x means AOT completes the workload in 1 / 1.20 of the VM time. A VM / Python ratio of 20x means VM takes twenty times as long as Python. These are execution-performance comparisons, excluding compilation and startup.

| Workload | AOT speedup over VM | VM / C++ time | VM / Python time | VM / Lua time |
| --- | --- | --- | --- | --- |
| list_build | 1.21x | 335.88x | 28.08x | 65.57x |
| list_read | 1.24x | 2098.76x | 20.79x | 110.39x |
| list_write | 1.25x | 4348.44x | 24.39x | 119.13x |
| list_remove | 1.27x | 346.94x | 27.52x | 33.76x |
| map_build | 1.49x | 53.94x | 27.36x | 149.87x |
| map_read | 1.48x | 581.31x | 24.45x | 165.47x |
| map_write | 1.51x | 692.39x | 27.50x | 182.92x |
| map_remove | 1.31x | 67.05x | 27.22x | 143.57x |
| set_build | 1.17x | 45.45x | 30.99x | 132.76x |
| set_read | 1.26x | 358.09x | 22.23x | 98.95x |
| set_write | 1.20x | 269.39x | 22.36x | 124.79x |
| set_remove | 1.18x | 63.88x | 33.60x | 146.09x |

The current VM and AOT container paths still have substantial overhead relative to these other implementations; the measured reductions do not remove that gap.

## Small calls and fixed-array reads (ms)

| Configuration | Small calls | Fixed-array reads | List growth/traversal |
| --- | --- | --- | --- |
| ChaLi 0.31.0 (VM, highest defaults) | 50.445 | 318.169 | 832.125 |
| ChaLi 0.31.0 (LLVM AOT, default checks) | 0.215 | 80.726 | 645.179 |
| C (GCC -O2) | 0.868 | 6.917 | 0.868 |
| C++17 (GCC -O2) | 0.864 | 6.928 | 0.915 |
| Lua 5.5.1 (official VM) | 17.087 | 43.836 | 12.124 |
| CPython 3.12 | 122.150 | 201.434 | 55.147 |

[Hotspot samples](../../benchmarks/results/hotspots-0310/results.json)

The hotspot List case has 400,000 elements and a preallocated C/C++ comparison. It is a separate workload from the default-container campaign above.

## Fixed-array read/write (ms)

| Configuration | Fixed-array read/write |
| --- | --- |
| ChaLi 0.31.0 (VM, highest defaults) | 625.920 |
| ChaLi 0.31.0 (LLVM AOT, default checks) | 156.419 |
| C (GCC -O2) | 8.656 |
| Lua 5.5.1 (official VM) | 71.044 |

[Array read/write samples](../../benchmarks/results/array-rw-0310/results.json)

## AOT versus VM hotspot execution performance

| Workload | AOT speedup over VM |
| --- | --- |
| Small calls | 234.63x |
| Fixed-array reads | 3.94x |
| List growth/traversal | 1.29x |
| Fixed-array read/write | 4.00x |

The small-call workload permits inlining in every language. Its ratio measures the execution time of the complete program after each implementation optimizes it; it is not an isolated measurement of one non-inlined function call. Ratios are specific to these workloads.

## Historical VM hotspot baseline (ms)

The preserved older compiler reports 0.30.0. It was rerun in alternating order with 0.31.0, retaining its actual version and binary identity. This is separate from the 0.31.0 container before/after baseline.

| Workload | Before 0.30.0 | After 0.31.0 | Speed ratio |
| --- | --- | --- | --- |
| call | 339.113 | 48.729 | 6.96x |
| array | 351.954 | 315.262 | 1.12x |
| array_rw | 1464.481 | 600.532 | 2.44x |
| list | 888.714 | 824.027 | 1.08x |

[Historical interleaved samples](../../benchmarks/results/hotspots-0310/vm-historical-before-after.json)

[Same-version VM comparison](../../benchmarks/results/hotspots-0310/vm-before-after.json)
