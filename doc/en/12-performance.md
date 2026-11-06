# ChaLi 1.0.0 performance measurements

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../12-performance.md)
<!-- END LANGUAGE NAVIGATION -->

All values below were collected in this run on **2026-11-06**, from 18 workloads and eight languages: **144 combinations, 3600 timed observations**. No earlier timings are reused. Each cell is the median of 25 intervals, in **milliseconds**.

## Environment and identity

Host: **AMD Ryzen 5 5600X 6-Core Processor**, 12 logical CPUs; `Windows-10-10.0.19045-SP0`. Sampling started at `2026-11-06T19:04:46.758142+08:00` and finished at `2026-11-06T19:08:17.585521+08:00` (UTC+08:00).

Measured ChaLi compiler source: `7063074b3c74becce3a3cb412fffc2508508e6a0`; executable SHA-256: `3b83256bb3bbc537ebb4d1436f0351f5c93f3a2ec2aa7a90b67a212c8fe3c577`. This identifies the measured executable; subsequent documentation builds may have different bytes. Workload sources, generated wrappers, tool versions and artifact hashes are retained with the raw records.

### Toolchain

| Language | Actual tool version |
| --- | --- |
| ChaLi | ChaLi 1.0.0 / static LLVM 23.1.4 |
| C | gcc.exe (Rev6, Built by MSYS2 project) 16.2.0 |
| C++ | g++.exe (Rev6, Built by MSYS2 project) 16.2.0 |
| Rust | rustc 1.98.1 (48a229cea 2026-09-01) |
| Java | java version "22.0.1" 2024-04-16 |
| Lua | Lua 5.5.1 (official VM, monotonic clock host) |
| Python | Python 3.12.3 |
| Go | go version go1.27.1 windows/amd64 |

## Method

Build and linking finish before sampling. Every combination runs in five independent processes. Each process records five intervals using seeds 0–4; native and interpreted implementations warm once beforehand. Java uses default tiered compilation and GC with `-Xbatch`, warming for at least 32 calls and at least one second per process. Warmup counts and durations remain in the raw output. This does not prove that every method has reached a final compilation tier.

ChaLi uses `build --shared --unchecked --allocator malloc`, with default LLVM O3/LTO. C/C++ use GCC O3/LTO; Rust uses optimization level 3, fat LTO and one codegen unit. Go uses its default optimizer. Lua and CPython use their normal interpreters and memory management. No optimization or implementation is changed after inspecting timings.

All workloads and languages are rerun. Five-seed integer results must match exactly; floating-point results are checked with relative and absolute tolerances of 1e-12. Compilation, process startup, global setup, warmup and checksum aggregation are outside each interval. Local allocation, growth, copying and cleanup performed by the work function are inside it. GC outside an interval is not charged back to that interval.

Sampling is serial; workload and language order are shuffled reproducibly with seed 20261106 on each round. All valid observations are retained, including slow samples. CPU frequency and affinity are not fixed and the operating system and other applications remain active. The ranges below show variation in this environment. No overall language score is calculated.

## Numeric and container workloads

| Workload | ChaLi | C | C++ | Rust | Java | Lua | Python | Go |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `int_chain` | 0.8663 | 4.3295 | 4.3355 | 0.8665 | 4.9786 | 29.6695 | 329.9217 | 4.4444 |
| `float_chain` | 1.3029 | 1.2975 | 1.2995 | 1.2999 | 1.2998 | 5.6132 | 31.2620 | 1.3070 |
| `call` | 0.2155 | 0.8756 | 0.8678 | 1.1289 | 1.0804 | 18.2484 | 127.1658 | 1.0941 |
| `array` | 9.9733 | 6.9366 | 6.9103 | 6.2278 | 6.1709 | 42.0423 | 212.4555 | 7.0522 |
| `array_rw` | 23.9131 | 8.6499 | 8.6565 | 6.9895 | 8.6508 | 73.0905 | 539.3807 | 8.8837 |
| `list_growth` | 7.7601 | 1.8587 | 1.9049 | 2.0619 | 3.1080 | 11.7896 | 59.0366 | 2.6038 |
| `map_growth` | 153.2406 | 1.9329 | 1.8066 | 2.0377 | 3.8943 | 4.3602 | 20.9199 | 2.5158 |
| `set_growth` | 105.1344 | 1.7682 | 1.8334 | 1.5678 | 3.3567 | 4.1564 | 17.7812 | 2.3294 |

Sizes: integer chain and fixed-array tasks 4,000,000 iterations; float chain and small-function task 1,000,000; List 400,000 appends; Map/Set 20,000 dispersed keys with insert/overwrite or duplicate insert, hit/miss queries, deletion and reinsertion. All growth workloads start empty without reserving capacity. The previous mixed preallocation `list` case is retired in favor of `list_growth`.

## Payloads, method dispatch, structures and multidimensional arrays

| Workload | ChaLi | C | C++ | Rust | Java | Lua | Python | Go |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `enum_payload` | 0.8646 | 0.8576 | 0.8141 | 1.1996 | 0.8554 | 9.3200 | 55.7007 | 0.6557 |
| `interface_mono` | 15.9554 | 0.2839 | 0.2825 | 0.0565 | 0.1705 | 6.2647 | 32.9437 | 0.3413 |
| `interface_poly` | 280.2986 | 0.4139 | 0.2825 | 0.3896 | 2.0571 | 9.6731 | 42.4961 | 0.5272 |
| `struct_nested` | 1.8441 | 1.1329 | 1.1362 | 1.0665 | 2.6545 | 28.1842 | 90.0458 | 1.2458 |
| `array_2d` | 0.6876 | 0.5710 | 0.5718 | 0.5502 | 0.8449 | 5.9203 | 38.5898 | 0.6241 |
| `array_3d` | 0.7869 | 0.6870 | 0.6808 | 0.6141 | 1.1920 | 7.9681 | 46.3695 | 0.6470 |

Each work function executes 262,144 iterations. Enum and nested-structure inputs contain 256 entries; arrays are 64×64 and 16×16×16. Inputs are built before timing. The method tasks use one implementation or a 2:1:1 mix of three implementations. Normal inlining and devirtualization remain allowed.

## UTF-8 byte tasks

| Workload | ChaLi | C | C++ | Rust | Java | Lua | Python | Go |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `string_read` | 2.5093 | 0.5387 | 0.5385 | 0.5388 | 0.5391 | 5.8600 | 25.8657 | 0.5718 |
| `string_concat` | 10.1643 | 5.9478 | 0.1053 | 0.0676 | 6.1735 | 22.6339 | 6.5798 | 6.0376 |
| `string_slice` | 5.7923 | 0.1637 | 0.1981 | 0.1098 | 0.0147 | 0.3080 | 0.4321 | 0.1018 |
| `string_find` | 10.1182 | 0.2065 | 0.1115 | 0.0363 | 0.3833 | 0.1704 | 0.1053 | 0.0719 |

The input contains 8448 UTF-8 bytes. Workloads perform 131,072 dependent byte reads, 2048 successive concatenations followed by complete-byte reduction, 1024 pairs of owning slices and comparisons, or 256 searches with present and absent needles. Positions and results are byte-based.

## Implementation differences

Container, object representation, library algorithm and runtime costs are part of these implementations. C uses an explicit open-addressed hash reference; C++ and Rust use standard containers. Go uses slices and maps; Java uses ArrayList/HashMap/HashSet with Long boxing; Python uses list/dict/set; Lua uses tables. C models payloads and dispatch with tags and function pointers, Rust with enum/dyn trait, and dynamic languages with objects and method lookup. These are task comparisons, not identical ABIs or storage layouts.

String rows compare UTF-8 byte tasks, not all languages’ Unicode String APIs. C++ and Rust append to growable strings and can reuse capacity; the C, ChaLi, Lua, Python and Go/Java byte implementations create successive concatenation results. Python uses bytes; Go and Java use byte sequences with explicit owned slice copies. Java byte search is an explicit loop; other implementations may use optimized library searches. Allocation strategies and library algorithms differ; these rows do not isolate language-compiler costs.

Single-thread kernels, library operations and warmed method loops do not represent application startup, concurrent servers, I/O, build time or peak memory. No language-wide conclusion follows from one row. Read each result together with its source, representation and full range.

## Location, range and quantiles

Every row uses all 25 observations. Time values are in milliseconds. Four quantile points are P25, P50, P75 and P100; P50 is the median and P100 is the maximum. R7 linearly interpolates the sorted sample at zero-based position (n−1)p. With n=25 these are the 7th, 13th, 19th and 25th ordered values. See the [R quantile definition](https://stat.ethz.ch/R-manual/R-devel/library/stats/html/quantile.html).

| Workload | Language | Minimum | Mean | P25 | Median / P50 | P75 | Maximum / P100 |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `int_chain` | ChaLi | 0.8618 | 0.8904 | 0.8633 | 0.8663 | 0.8694 | 1.0773 |
| `int_chain` | C | 4.3145 | 4.3585 | 4.3181 | 4.3295 | 4.3519 | 4.5601 |
| `int_chain` | C++ | 4.3137 | 4.3796 | 4.3198 | 4.3355 | 4.4265 | 4.5797 |
| `int_chain` | Rust | 0.8618 | 0.8792 | 0.8637 | 0.8665 | 0.8725 | 1.0049 |
| `int_chain` | Java | 4.9628 | 4.9975 | 4.9654 | 4.9786 | 4.9995 | 5.2337 |
| `int_chain` | Lua | 28.5605 | 33.5498 | 28.7523 | 29.6695 | 38.4202 | 45.5575 |
| `int_chain` | Python | 312.4036 | 335.2571 | 325.6007 | 329.9217 | 344.1926 | 389.4222 |
| `int_chain` | Go | 4.3397 | 4.4482 | 4.3869 | 4.4444 | 4.5168 | 4.5687 |
| `float_chain` | ChaLi | 1.2940 | 1.3372 | 1.2956 | 1.3029 | 1.3360 | 1.5582 |
| `float_chain` | C | 1.2941 | 1.3180 | 1.2954 | 1.2975 | 1.3097 | 1.4962 |
| `float_chain` | C++ | 1.2939 | 1.3285 | 1.2956 | 1.2995 | 1.3499 | 1.4899 |
| `float_chain` | Rust | 1.2939 | 1.3356 | 1.2951 | 1.2999 | 1.3271 | 1.5063 |
| `float_chain` | Java | 1.2942 | 1.3254 | 1.2946 | 1.2998 | 1.3205 | 1.5105 |
| `float_chain` | Lua | 5.2418 | 5.6784 | 5.4903 | 5.6132 | 5.6437 | 8.2696 |
| `float_chain` | Python | 29.6106 | 34.7068 | 30.1940 | 31.2620 | 38.4936 | 46.7742 |
| `float_chain` | Go | 1.2976 | 1.3291 | 1.3006 | 1.3070 | 1.3198 | 1.4859 |
| `call` | ChaLi | 0.2155 | 0.2204 | 0.2155 | 0.2155 | 0.2185 | 0.2693 |
| `call` | C | 0.8619 | 0.8978 | 0.8635 | 0.8756 | 0.9117 | 1.0179 |
| `call` | C++ | 0.8619 | 0.8781 | 0.8639 | 0.8678 | 0.8741 | 1.0207 |
| `call` | Rust | 1.0787 | 1.2187 | 1.0843 | 1.1289 | 1.3518 | 1.6103 |
| `call` | Java | 1.0787 | 1.1142 | 1.0790 | 1.0804 | 1.1213 | 1.2952 |
| `call` | Lua | 16.6976 | 19.7866 | 17.7727 | 18.2484 | 21.2756 | 28.3226 |
| `call` | Python | 112.5493 | 132.6711 | 120.2290 | 127.1658 | 142.8101 | 181.0112 |
| `call` | Go | 1.0822 | 1.1305 | 1.0848 | 1.0941 | 1.1757 | 1.2831 |
| `array` | ChaLi | 9.7386 | 10.1978 | 9.8548 | 9.9733 | 10.1572 | 14.6380 |
| `array` | C | 6.9024 | 6.9650 | 6.9195 | 6.9366 | 6.9589 | 7.1973 |
| `array` | C++ | 6.9026 | 6.9193 | 6.9075 | 6.9103 | 6.9263 | 6.9576 |
| `array` | Rust | 6.2122 | 6.2461 | 6.2171 | 6.2278 | 6.2442 | 6.4524 |
| `array` | Java | 6.1476 | 6.2325 | 6.1588 | 6.1709 | 6.2832 | 6.4193 |
| `array` | Lua | 34.2971 | 40.5514 | 36.8165 | 42.0423 | 42.5688 | 50.1789 |
| `array` | Python | 195.2578 | 221.1901 | 202.2598 | 212.4555 | 221.0955 | 301.9105 |
| `array` | Go | 6.9134 | 7.1023 | 6.9597 | 7.0522 | 7.1952 | 7.5823 |
| `array_rw` | ChaLi | 23.6296 | 24.3182 | 23.7845 | 23.9131 | 24.0758 | 29.6265 |
| `array_rw` | C | 8.6296 | 8.7171 | 8.6370 | 8.6499 | 8.7493 | 9.0979 |
| `array_rw` | C++ | 8.6285 | 8.6904 | 8.6364 | 8.6565 | 8.6888 | 9.0627 |
| `array_rw` | Rust | 6.9515 | 6.9972 | 6.9723 | 6.9895 | 7.0127 | 7.1342 |
| `array_rw` | Java | 8.6311 | 8.7364 | 8.6436 | 8.6508 | 8.7498 | 9.0772 |
| `array_rw` | Lua | 62.9245 | 73.2038 | 65.6572 | 73.0905 | 78.2157 | 104.6645 |
| `array_rw` | Python | 495.6788 | 543.4288 | 507.0529 | 539.3807 | 567.7683 | 635.7739 |
| `array_rw` | Go | 8.6696 | 8.8912 | 8.7394 | 8.8837 | 9.0216 | 9.1838 |
| `list_growth` | ChaLi | 5.9259 | 7.6934 | 6.0590 | 7.7601 | 8.5368 | 10.7056 |
| `list_growth` | C | 1.3790 | 1.9434 | 1.7108 | 1.8587 | 2.1508 | 2.6874 |
| `list_growth` | C++ | 1.6206 | 2.0094 | 1.7831 | 1.9049 | 2.1311 | 2.6480 |
| `list_growth` | Rust | 1.4825 | 2.0116 | 1.8148 | 2.0619 | 2.2648 | 2.4393 |
| `list_growth` | Java | 2.5823 | 3.2799 | 2.8946 | 3.1080 | 3.4866 | 4.5021 |
| `list_growth` | Lua | 11.1500 | 13.3658 | 11.4151 | 11.7896 | 15.0594 | 19.6176 |
| `list_growth` | Python | 54.0500 | 61.9268 | 56.0354 | 59.0366 | 66.7684 | 74.9419 |
| `list_growth` | Go | 1.9604 | 2.8563 | 2.3556 | 2.6038 | 3.2475 | 4.4597 |
| `map_growth` | ChaLi | 133.3780 | 151.8469 | 142.0623 | 153.2406 | 157.1721 | 190.0638 |
| `map_growth` | C | 1.8212 | 2.0768 | 1.8628 | 1.9329 | 2.1862 | 2.5772 |
| `map_growth` | C++ | 1.6822 | 1.8309 | 1.7734 | 1.8066 | 1.8433 | 2.2431 |
| `map_growth` | Rust | 1.9210 | 2.0769 | 1.9701 | 2.0377 | 2.1045 | 2.6444 |
| `map_growth` | Java | 3.3802 | 3.9633 | 3.5743 | 3.8943 | 3.9695 | 6.7427 |
| `map_growth` | Lua | 4.0931 | 4.6102 | 4.2790 | 4.3602 | 4.4500 | 7.8686 |
| `map_growth` | Python | 19.9915 | 23.6471 | 20.2084 | 20.9199 | 27.0620 | 36.8561 |
| `map_growth` | Go | 2.2398 | 2.6460 | 2.4427 | 2.5158 | 2.9698 | 3.4854 |
| `set_growth` | ChaLi | 95.1728 | 106.2130 | 97.7833 | 105.1344 | 109.2335 | 129.7745 |
| `set_growth` | C | 1.7264 | 1.8881 | 1.7386 | 1.7682 | 1.9381 | 2.6198 |
| `set_growth` | C++ | 1.6638 | 1.9700 | 1.7620 | 1.8334 | 2.0852 | 3.2851 |
| `set_growth` | Rust | 1.4957 | 1.6760 | 1.5385 | 1.5678 | 1.7357 | 2.2905 |
| `set_growth` | Java | 3.0888 | 3.4008 | 3.2736 | 3.3567 | 3.5708 | 3.7580 |
| `set_growth` | Lua | 3.9352 | 4.8043 | 4.0966 | 4.1564 | 4.2829 | 8.4022 |
| `set_growth` | Python | 17.0212 | 21.8899 | 17.4565 | 17.7812 | 22.6053 | 52.5155 |
| `set_growth` | Go | 1.9574 | 2.4322 | 2.1182 | 2.3294 | 2.5744 | 3.7309 |
| `enum_payload` | ChaLi | 0.8506 | 1.1115 | 0.8559 | 0.8646 | 0.9026 | 2.3075 |
| `enum_payload` | C | 0.8535 | 0.8682 | 0.8557 | 0.8576 | 0.8627 | 0.9833 |
| `enum_payload` | C++ | 0.8072 | 0.8277 | 0.8105 | 0.8141 | 0.8225 | 0.9512 |
| `enum_payload` | Rust | 1.1887 | 1.2090 | 1.1928 | 1.1996 | 1.2086 | 1.3033 |
| `enum_payload` | Java | 0.8444 | 0.8750 | 0.8468 | 0.8554 | 0.8741 | 0.9800 |
| `enum_payload` | Lua | 8.9651 | 11.0944 | 9.2310 | 9.3200 | 12.3839 | 21.4589 |
| `enum_payload` | Python | 53.0366 | 59.3735 | 53.2946 | 55.7007 | 64.8491 | 75.1636 |
| `enum_payload` | Go | 0.6511 | 0.6596 | 0.6529 | 0.6557 | 0.6646 | 0.6971 |
| `interface_mono` | ChaLi | 15.2261 | 17.8310 | 15.8868 | 15.9554 | 16.9662 | 30.3039 |
| `interface_mono` | C | 0.2824 | 0.2973 | 0.2824 | 0.2839 | 0.2950 | 0.3668 |
| `interface_mono` | C++ | 0.2824 | 0.2864 | 0.2824 | 0.2825 | 0.2841 | 0.3259 |
| `interface_mono` | Rust | 0.0564 | 0.0599 | 0.0565 | 0.0565 | 0.0590 | 0.0707 |
| `interface_mono` | Java | 0.1695 | 0.1719 | 0.1696 | 0.1705 | 0.1723 | 0.1800 |
| `interface_mono` | Lua | 6.1564 | 6.4768 | 6.2364 | 6.2647 | 6.3646 | 10.9914 |
| `interface_mono` | Python | 31.8402 | 33.9312 | 32.3761 | 32.9437 | 34.3515 | 41.6336 |
| `interface_mono` | Go | 0.3389 | 0.3412 | 0.3391 | 0.3413 | 0.3425 | 0.3466 |
| `interface_poly` | ChaLi | 259.0079 | 300.6498 | 272.0042 | 280.2986 | 308.3975 | 431.1328 |
| `interface_poly` | C | 0.4095 | 0.4174 | 0.4113 | 0.4139 | 0.4208 | 0.4510 |
| `interface_poly` | C++ | 0.2824 | 0.2886 | 0.2824 | 0.2825 | 0.2852 | 0.3374 |
| `interface_poly` | Rust | 0.3671 | 0.3969 | 0.3672 | 0.3896 | 0.4112 | 0.4780 |
| `interface_poly` | Java | 1.9375 | 2.0715 | 2.0190 | 2.0571 | 2.1067 | 2.2835 |
| `interface_poly` | Lua | 9.1735 | 10.6231 | 9.4959 | 9.6731 | 12.2963 | 13.8873 |
| `interface_poly` | Python | 41.5396 | 44.8762 | 42.0353 | 42.4961 | 44.4806 | 60.7762 |
| `interface_poly` | Go | 0.5114 | 0.5386 | 0.5263 | 0.5272 | 0.5315 | 0.6811 |
| `struct_nested` | ChaLi | 1.7596 | 1.8749 | 1.8184 | 1.8441 | 1.8759 | 2.2189 |
| `struct_nested` | C | 0.9923 | 1.1490 | 1.0099 | 1.1329 | 1.2040 | 1.3900 |
| `struct_nested` | C++ | 0.9915 | 1.1428 | 1.0042 | 1.1362 | 1.1989 | 1.4213 |
| `struct_nested` | Rust | 0.9668 | 1.0679 | 0.9941 | 1.0665 | 1.0779 | 1.4729 |
| `struct_nested` | Java | 1.7801 | 2.4344 | 2.0860 | 2.6545 | 2.6909 | 2.9870 |
| `struct_nested` | Lua | 21.9492 | 28.1513 | 22.8262 | 28.1842 | 30.1734 | 41.7105 |
| `struct_nested` | Python | 73.9187 | 89.8439 | 78.2739 | 90.0458 | 95.9715 | 117.2026 |
| `struct_nested` | Go | 1.0629 | 1.2054 | 1.0970 | 1.2458 | 1.2523 | 1.5081 |
| `array_2d` | ChaLi | 0.6500 | 0.7169 | 0.6561 | 0.6876 | 0.7740 | 0.9067 |
| `array_2d` | C | 0.5650 | 0.5852 | 0.5686 | 0.5710 | 0.5795 | 0.6993 |
| `array_2d` | C++ | 0.5651 | 0.5881 | 0.5694 | 0.5718 | 0.5782 | 0.6838 |
| `array_2d` | Rust | 0.5367 | 0.5665 | 0.5392 | 0.5502 | 0.5784 | 0.6534 |
| `array_2d` | Java | 0.8381 | 0.8677 | 0.8421 | 0.8449 | 0.8537 | 1.0348 |
| `array_2d` | Lua | 5.8376 | 6.0408 | 5.8979 | 5.9203 | 6.1685 | 6.6028 |
| `array_2d` | Python | 37.3216 | 41.0840 | 38.1063 | 38.5898 | 39.3465 | 55.6074 |
| `array_2d` | Go | 0.5900 | 0.6179 | 0.6075 | 0.6241 | 0.6283 | 0.6363 |
| `array_3d` | ChaLi | 0.7731 | 0.8287 | 0.7811 | 0.7869 | 0.8163 | 1.1347 |
| `array_3d` | C | 0.6796 | 0.7036 | 0.6829 | 0.6870 | 0.7056 | 0.8277 |
| `array_3d` | C++ | 0.6780 | 0.7367 | 0.6794 | 0.6808 | 0.6880 | 1.1816 |
| `array_3d` | Rust | 0.5940 | 0.6264 | 0.6024 | 0.6141 | 0.6211 | 0.7433 |
| `array_3d` | Java | 1.1825 | 1.2215 | 1.1840 | 1.1920 | 1.2464 | 1.4170 |
| `array_3d` | Lua | 7.2603 | 9.9014 | 7.8070 | 7.9681 | 10.5276 | 18.0755 |
| `array_3d` | Python | 43.9089 | 49.8848 | 44.5468 | 46.3695 | 52.8184 | 69.9714 |
| `array_3d` | Go | 0.6396 | 0.6570 | 0.6410 | 0.6470 | 0.6665 | 0.7333 |
| `string_read` | ChaLi | 2.4449 | 2.6312 | 2.4655 | 2.5093 | 2.6635 | 3.3916 |
| `string_read` | C | 0.5366 | 0.5664 | 0.5368 | 0.5387 | 0.5713 | 0.6779 |
| `string_read` | C++ | 0.5365 | 0.5426 | 0.5378 | 0.5385 | 0.5402 | 0.6108 |
| `string_read` | Rust | 0.5366 | 0.5392 | 0.5370 | 0.5388 | 0.5393 | 0.5491 |
| `string_read` | Java | 0.5367 | 0.5466 | 0.5371 | 0.5391 | 0.5407 | 0.6761 |
| `string_read` | Lua | 5.6887 | 6.6900 | 5.8038 | 5.8600 | 7.1575 | 9.8141 |
| `string_read` | Python | 24.4063 | 28.4451 | 25.1148 | 25.8657 | 29.6141 | 45.3400 |
| `string_read` | Go | 0.5650 | 0.5765 | 0.5686 | 0.5718 | 0.5825 | 0.6127 |
| `string_concat` | ChaLi | 8.2402 | 13.1193 | 8.7807 | 10.1643 | 14.8248 | 25.0734 |
| `string_concat` | C | 2.6549 | 7.7168 | 3.6893 | 5.9478 | 10.0751 | 19.9755 |
| `string_concat` | C++ | 0.0931 | 0.1102 | 0.1031 | 0.1053 | 0.1135 | 0.1453 |
| `string_concat` | Rust | 0.0503 | 0.0663 | 0.0625 | 0.0676 | 0.0699 | 0.0954 |
| `string_concat` | Java | 5.8222 | 6.4824 | 5.9998 | 6.1735 | 6.5754 | 8.8131 |
| `string_concat` | Lua | 10.8440 | 20.9761 | 16.3399 | 22.6339 | 26.4626 | 29.2308 |
| `string_concat` | Python | 6.4438 | 6.8999 | 6.5081 | 6.5798 | 6.6946 | 11.0059 |
| `string_concat` | Go | 5.0001 | 8.3942 | 5.4332 | 6.0376 | 11.0106 | 28.4931 |
| `string_slice` | ChaLi | 5.3964 | 6.4330 | 5.6793 | 5.7923 | 6.0082 | 10.9938 |
| `string_slice` | C | 0.0711 | 0.1340 | 0.0744 | 0.1637 | 0.1704 | 0.2066 |
| `string_slice` | C++ | 0.0766 | 0.1911 | 0.0968 | 0.1981 | 0.2146 | 0.4286 |
| `string_slice` | Rust | 0.0741 | 0.1208 | 0.0790 | 0.1098 | 0.1396 | 0.2245 |
| `string_slice` | Java | 0.0138 | 0.0152 | 0.0145 | 0.0147 | 0.0152 | 0.0204 |
| `string_slice` | Lua | 0.2977 | 0.3125 | 0.3047 | 0.3080 | 0.3130 | 0.3773 |
| `string_slice` | Python | 0.4162 | 0.5714 | 0.4239 | 0.4321 | 0.8784 | 0.8934 |
| `string_slice` | Go | 0.0939 | 0.1246 | 0.0977 | 0.1018 | 0.1240 | 0.2314 |
| `string_find` | ChaLi | 7.1060 | 10.1631 | 7.6227 | 10.1182 | 11.7246 | 15.8449 |
| `string_find` | C | 0.1875 | 0.2195 | 0.1937 | 0.2065 | 0.2141 | 0.4011 |
| `string_find` | C++ | 0.1010 | 0.1119 | 0.1024 | 0.1115 | 0.1127 | 0.1461 |
| `string_find` | Rust | 0.0331 | 0.0413 | 0.0357 | 0.0363 | 0.0473 | 0.0589 |
| `string_find` | Java | 0.3476 | 0.3836 | 0.3592 | 0.3833 | 0.3925 | 0.4742 |
| `string_find` | Lua | 0.1199 | 0.1840 | 0.1341 | 0.1704 | 0.2146 | 0.2842 |
| `string_find` | Python | 0.0969 | 0.1181 | 0.0993 | 0.1053 | 0.1287 | 0.2025 |
| `string_find` | Go | 0.0593 | 0.0801 | 0.0651 | 0.0719 | 0.0934 | 0.1350 |

## Dispersion and distribution shape

Sample variance is s² = sum((x−mean)²)/(n−1), in ms²; sample SD is its square root, in ms. CV is 100s/mean, expressed as a percentage ([NIST definition](https://www.itl.nist.gov/div898/software/dataplot/refman2/auxillar/coefvari.htm)). Adjusted Fisher–Pearson sample skewness is G1 = n/((n−1)(n−2)) × sum(((x−mean)/s)³), a dimensionless value ([NIST skewness definitions](https://www.itl.nist.gov/div898/handbook/eda/section3/eda35b.htm)). When s=0, skewness is undefined rather than zero.

These are descriptive statistics of the 25 intervals, including differences between seeds and execution environments. Five intervals share each process; they are not 25 independent processes and are not used here for significance claims. Positive skewness describes a longer right tail, not a speed-based judgment. No observations are removed.

| Workload | Language | Sample variance (ms²) | SD (ms) | CV (%) | Skewness G1 |
| --- | --- | ---: | ---: | ---: | ---: |
| `int_chain` | ChaLi | 0.00324685 | 0.0569811 | 6.39953 | 2.525 |
| `int_chain` | C | 0.00481312 | 0.0693767 | 1.59174 | 1.96396 |
| `int_chain` | C++ | 0.00717922 | 0.0847303 | 1.93466 | 1.33989 |
| `int_chain` | Rust | 0.00125344 | 0.0354039 | 4.02694 | 3.0057 |
| `int_chain` | Java | 0.00407058 | 0.0638011 | 1.27667 | 3.05744 |
| `int_chain` | Lua | 33.7391 | 5.80853 | 17.3132 | 0.831392 |
| `int_chain` | Python | 300.54 | 17.3361 | 5.17098 | 1.30161 |
| `int_chain` | Go | 0.00534183 | 0.0730878 | 1.64307 | 0.14665 |
| `float_chain` | ChaLi | 0.00523608 | 0.0723608 | 5.41128 | 1.94124 |
| `float_chain` | C | 0.00231806 | 0.0481463 | 3.65303 | 2.81431 |
| `float_chain` | C++ | 0.00301222 | 0.0548837 | 4.13113 | 1.75065 |
| `float_chain` | Rust | 0.00485944 | 0.0697097 | 5.21929 | 1.7111 |
| `float_chain` | Java | 0.00312244 | 0.0558788 | 4.21596 | 2.37517 |
| `float_chain` | Lua | 0.314156 | 0.560496 | 9.87062 | 4.41453 |
| `float_chain` | Python | 31.477 | 5.61043 | 16.1652 | 0.918297 |
| `float_chain` | Go | 0.0027135 | 0.0520913 | 3.91937 | 2.20493 |
| `call` | ChaLi | 0.000182288 | 0.0135014 | 6.12454 | 3.22011 |
| `call` | C | 0.00229396 | 0.0478953 | 5.33451 | 1.51222 |
| `call` | C++ | 0.00114736 | 0.0338727 | 3.85737 | 3.55875 |
| `call` | Rust | 0.025969 | 0.161149 | 13.2225 | 0.987831 |
| `call` | Java | 0.00415428 | 0.0644537 | 5.78479 | 1.90291 |
| `call` | Lua | 11.8185 | 3.43781 | 17.3744 | 1.63234 |
| `call` | Python | 312.842 | 17.6873 | 13.3317 | 1.33004 |
| `call` | Go | 0.00455453 | 0.0674872 | 5.96951 | 1.18135 |
| `array` | ChaLi | 0.893519 | 0.945261 | 9.2693 | 4.66615 |
| `array` | C | 0.00662473 | 0.0813925 | 1.16859 | 2.1607 |
| `array` | C++ | 0.0002741 | 0.016556 | 0.239273 | 1.15313 |
| `array` | Rust | 0.00321488 | 0.0566999 | 0.907764 | 2.96939 |
| `array` | Java | 0.00988727 | 0.0994347 | 1.59541 | 0.916922 |
| `array` | Lua | 17.8666 | 4.22689 | 10.4235 | 0.439074 |
| `array` | Python | 779.573 | 27.9208 | 12.623 | 1.67768 |
| `array` | Go | 0.0307216 | 0.175276 | 2.46786 | 1.1941 |
| `array_rw` | ChaLi | 1.96913 | 1.40326 | 5.7704 | 3.31877 |
| `array_rw` | C | 0.0176616 | 0.132897 | 1.52456 | 1.86844 |
| `array_rw` | C++ | 0.0119826 | 0.109465 | 1.25961 | 2.88157 |
| `array_rw` | Rust | 0.00149823 | 0.0387069 | 0.553175 | 1.89752 |
| `array_rw` | Java | 0.0229946 | 0.15164 | 1.73573 | 1.40107 |
| `array_rw` | Lua | 91.0091 | 9.53987 | 13.0319 | 1.55278 |
| `array_rw` | Python | 1762.79 | 41.9856 | 7.72605 | 0.77514 |
| `array_rw` | Go | 0.0274805 | 0.165772 | 1.86445 | 0.242535 |
| `list_growth` | ChaLi | 2.31255 | 1.52071 | 19.7664 | 0.488938 |
| `list_growth` | C | 0.106508 | 0.326355 | 16.793 | 0.674361 |
| `list_growth` | C++ | 0.10037 | 0.316813 | 15.7669 | 0.895962 |
| `list_growth` | Rust | 0.0800547 | 0.282939 | 14.0651 | -0.354251 |
| `list_growth` | Java | 0.292653 | 0.540974 | 16.4937 | 1.0821 |
| `list_growth` | Lua | 7.62831 | 2.76194 | 20.6642 | 1.24289 |
| `list_growth` | Python | 45.9459 | 6.77834 | 10.9457 | 0.547747 |
| `list_growth` | Go | 0.427133 | 0.653554 | 22.8814 | 0.775323 |
| `map_growth` | ChaLi | 201.462 | 14.1937 | 9.34741 | 1.12551 |
| `map_growth` | C | 0.0809239 | 0.284471 | 13.6975 | 0.976351 |
| `map_growth` | C++ | 0.015565 | 0.12476 | 6.81406 | 2.17264 |
| `map_growth` | Rust | 0.0323301 | 0.179806 | 8.65739 | 2.19168 |
| `map_growth` | Java | 0.478501 | 0.691738 | 17.4537 | 2.9648 |
| `map_growth` | Lua | 0.843011 | 0.918156 | 19.9159 | 3.19806 |
| `map_growth` | Python | 21.1278 | 4.5965 | 19.4379 | 1.33308 |
| `map_growth` | Go | 0.111744 | 0.334281 | 12.6334 | 0.962814 |
| `set_growth` | ChaLi | 91.5442 | 9.56787 | 9.0082 | 1.05269 |
| `set_growth` | C | 0.0635157 | 0.252023 | 13.3476 | 2.11633 |
| `set_growth` | C++ | 0.1398 | 0.373899 | 18.9797 | 2.50659 |
| `set_growth` | Rust | 0.0448707 | 0.211827 | 12.6386 | 1.71 |
| `set_growth` | Java | 0.0376009 | 0.193909 | 5.70192 | 0.475085 |
| `set_growth` | Lua | 2.01997 | 1.42126 | 29.5829 | 1.73612 |
| `set_growth` | Python | 69.8493 | 8.35759 | 38.1801 | 2.69554 |
| `set_growth` | Go | 0.168397 | 0.410361 | 16.8721 | 1.44456 |
| `enum_payload` | ChaLi | 0.229436 | 0.478995 | 43.0937 | 1.70828 |
| `enum_payload` | C | 0.000892968 | 0.0298826 | 3.44172 | 3.10238 |
| `enum_payload` | C++ | 0.00132362 | 0.0363816 | 4.39538 | 2.58157 |
| `enum_payload` | Rust | 0.000765835 | 0.0276737 | 2.28892 | 2.4885 |
| `enum_payload` | Java | 0.0017768 | 0.0421521 | 4.81755 | 1.45637 |
| `enum_payload` | Lua | 10.4337 | 3.23012 | 29.115 | 1.89955 |
| `enum_payload` | Python | 56.0537 | 7.4869 | 12.6098 | 0.886762 |
| `enum_payload` | Go | 0.000102 | 0.0100995 | 1.53124 | 2.34474 |
| `interface_mono` | ChaLi | 16.0168 | 4.0021 | 22.4447 | 1.93936 |
| `interface_mono` | C | 0.000690854 | 0.0262841 | 8.84189 | 1.7728 |
| `interface_mono` | C++ | 0.000124857 | 0.0111739 | 3.90097 | 3.16641 |
| `interface_mono` | Rust | 3.38967e-05 | 0.00582209 | 9.72683 | 1.33469 |
| `interface_mono` | Java | 1.28683e-05 | 0.00358725 | 2.08634 | 1.58404 |
| `interface_mono` | Lua | 0.892684 | 0.944819 | 14.5878 | 4.92734 |
| `interface_mono` | Python | 5.84162 | 2.41694 | 7.12307 | 1.9127 |
| `interface_mono` | Go | 3.9626e-06 | 0.00199063 | 0.583502 | 0.795882 |
| `interface_poly` | ChaLi | 1893.44 | 43.5137 | 14.4732 | 1.58508 |
| `interface_poly` | C | 0.000113155 | 0.0106374 | 2.54843 | 1.94372 |
| `interface_poly` | C++ | 0.000199477 | 0.0141236 | 4.89405 | 2.62162 |
| `interface_poly` | Rust | 0.00114869 | 0.0338923 | 8.54011 | 1.01329 |
| `interface_poly` | Java | 0.0076842 | 0.0876596 | 4.23165 | 0.857635 |
| `interface_poly` | Lua | 2.45209 | 1.56591 | 14.7406 | 0.939086 |
| `interface_poly` | Python | 22.4612 | 4.73932 | 10.5609 | 2.08001 |
| `interface_poly` | Go | 0.00130891 | 0.0361788 | 6.71725 | 3.39154 |
| `struct_nested` | ChaLi | 0.0109106 | 0.104454 | 5.57112 | 1.87202 |
| `struct_nested` | C | 0.0150343 | 0.122615 | 10.6712 | 0.466631 |
| `struct_nested` | C++ | 0.0157588 | 0.125534 | 10.9847 | 0.580867 |
| `struct_nested` | Rust | 0.010938 | 0.104585 | 9.79309 | 2.61532 |
| `struct_nested` | Java | 0.141971 | 0.37679 | 15.478 | -0.671027 |
| `struct_nested` | Lua | 38.8493 | 6.23292 | 22.1408 | 0.910234 |
| `struct_nested` | Python | 154.03 | 12.4109 | 13.8138 | 0.560393 |
| `struct_nested` | Go | 0.012319 | 0.110991 | 9.20784 | 0.593514 |
| `array_2d` | ChaLi | 0.00593597 | 0.0770452 | 10.7474 | 1.14736 |
| `array_2d` | C | 0.00129235 | 0.0359492 | 6.14269 | 2.64788 |
| `array_2d` | C++ | 0.00129294 | 0.0359574 | 6.11446 | 1.96443 |
| `array_2d` | Rust | 0.00140837 | 0.0375282 | 6.62505 | 1.27201 |
| `array_2d` | Java | 0.00279402 | 0.0528584 | 6.0921 | 2.19381 |
| `array_2d` | Lua | 0.0468731 | 0.216502 | 3.58399 | 1.37437 |
| `array_2d` | Python | 29.4081 | 5.42292 | 13.1996 | 1.73328 |
| `array_2d` | Go | 0.000181833 | 0.0134846 | 2.18239 | -0.775469 |
| `array_3d` | ChaLi | 0.00817845 | 0.0904348 | 10.9133 | 2.24618 |
| `array_3d` | C | 0.00148808 | 0.0385757 | 5.48284 | 2.27334 |
| `array_3d` | C++ | 0.0158185 | 0.125772 | 17.072 | 2.58648 |
| `array_3d` | Rust | 0.00153808 | 0.0392183 | 6.26098 | 1.76938 |
| `array_3d` | Java | 0.00382737 | 0.0618657 | 5.06487 | 2.10462 |
| `array_3d` | Lua | 14.2977 | 3.78124 | 38.1888 | 1.47593 |
| `array_3d` | Python | 58.6417 | 7.65779 | 15.351 | 1.52736 |
| `array_3d` | Go | 0.000593114 | 0.0243539 | 3.70711 | 1.99005 |
| `string_read` | ChaLi | 0.0663023 | 0.257492 | 9.78616 | 1.74387 |
| `string_read` | C | 0.00224667 | 0.0473991 | 8.36872 | 1.47693 |
| `string_read` | C++ | 0.000225106 | 0.0150035 | 2.76526 | 4.31549 |
| `string_read` | Rust | 9.59727e-06 | 0.00309795 | 0.574579 | 2.34416 |
| `string_read` | Java | 0.000802778 | 0.0283333 | 5.18318 | 4.35008 |
| `string_read` | Lua | 1.84961 | 1.36 | 20.3289 | 1.34441 |
| `string_read` | Python | 30.6454 | 5.53583 | 19.4615 | 1.93715 |
| `string_read` | Go | 0.000150513 | 0.0122684 | 2.12821 | 1.7581 |
| `string_concat` | ChaLi | 30.944 | 5.56273 | 42.401 | 1.13643 |
| `string_concat` | C | 26.022 | 5.10118 | 66.1049 | 0.954997 |
| `string_concat` | C++ | 0.00019324 | 0.0139011 | 12.6199 | 1.27269 |
| `string_concat` | Rust | 0.000100401 | 0.01002 | 15.1204 | 0.60809 |
| `string_concat` | Java | 0.545632 | 0.738669 | 11.395 | 1.91222 |
| `string_concat` | Lua | 39.6818 | 6.29935 | 30.031 | -0.477903 |
| `string_concat` | Python | 1.09655 | 1.04716 | 15.1764 | 3.4323 |
| `string_concat` | Go | 25.6088 | 5.06051 | 60.2855 | 2.84052 |
| `string_slice` | ChaLi | 2.55248 | 1.59765 | 24.835 | 2.21843 |
| `string_slice` | C | 0.00244771 | 0.0494744 | 36.9344 | -0.296 |
| `string_slice` | C++ | 0.0103654 | 0.101811 | 53.2672 | 0.991226 |
| `string_slice` | Rust | 0.00196293 | 0.044305 | 36.6836 | 1.03318 |
| `string_slice` | Java | 2.39667e-06 | 0.00154812 | 10.1582 | 2.31434 |
| `string_slice` | Lua | 0.000299721 | 0.0173125 | 5.5402 | 2.78317 |
| `string_slice` | Python | 0.0434719 | 0.208499 | 36.4876 | 0.877484 |
| `string_slice` | Go | 0.00188973 | 0.043471 | 34.8862 | 1.50062 |
| `string_find` | ChaLi | 7.34032 | 2.7093 | 26.6583 | 0.663089 |
| `string_find` | C | 0.00246575 | 0.0496564 | 22.6229 | 2.94231 |
| `string_find` | C++ | 0.000138862 | 0.011784 | 10.5285 | 1.94029 |
| `string_find` | Rust | 7.18786e-05 | 0.00847812 | 20.5341 | 0.812226 |
| `string_find` | Java | 0.000915105 | 0.0302507 | 7.88682 | 1.12359 |
| `string_find` | Lua | 0.0029401 | 0.0542227 | 29.4644 | 0.599931 |
| `string_find` | Python | 0.000772817 | 0.0277996 | 23.5462 | 1.67498 |
| `string_find` | Go | 0.000385268 | 0.0196283 | 24.5108 | 1.00018 |

## Reproduction and raw records

See [the driver](../../benchmarks/current/run.py), [implementations and method](../../benchmarks/current/README.md), [identity audit](../../benchmarks/current/audit.py), and [report generator](../../benchmarks/current/report.py). Raw JSON contains commands, output, seed results, all samples, summaries, order and SHA-256 identities. The exported data archive also retains measured artifacts and source snapshots.

```powershell
python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --build-only
python benchmarks/current/audit.py build/validation/performance-current/run/results.json
python benchmarks/current/run.py --chali PATH --compiler-source-commit COMMIT --out build/validation/performance-current/run --measure-only
python benchmarks/current/audit.py build/validation/performance-current/run/results.json --after
python benchmarks/current/report.py build/validation/performance-current/run/results.json --write-docs
```
