# Frozen precompiled-library LTO comparison

**English** · [简体中文](README.md)

These are frozen records from one ChaLi static LLVM compiler before library execution support was retired. Reproduction requires the frozen compiler and its actual build source identity. See the [library package guide](../../doc/en/44-library-packages.md) for the current format tools.

- [base.chl](base.chl) provides a single-step integer dependency chain and a function executing 256 steps per call; [bridge.chl](bridge.chl) forms the second precompiled library layer.
- [call.chl](call.chl) makes 1,048,576 cross-library calls. [bulk.chl](bulk.chl) makes 4,096 calls of 256 steps each. Both execute the same operations in the same order, checked for five seeds against an independent Python model.
- [timing.c](timing.c) reuses the existing monotonic timing shell without modifying historical timing sources. A C host calls ordinary native scalar exports. Initialization, warmup, result aggregation, output and cleanup are outside kernel timing.

The [driver](../../tools/benchmark_library_lto.py) requires a self-contained static LLVM compiler:

```sh
python tools/benchmark_library_lto.py --chali PATH_TO_STATIC_COMPILER --compiler-source-commit ACTUAL_COMPILER_SOURCE_COMMIT --out build/lto-comparison-new
```

The output directory must not exist, preventing mixed or overwritten history. Add `--verify-only` with another output directory for functional checking alone. `--compiler-source-commit` must identify the compiler's actual build source; the driver does not infer current HEAD.

The driver copies and checks the compiler, workloads, timing shell and scripts, builds two dual-implementation libraries, and removes provider sources from the work directory. All ten configurations must pass per-seed checks for AOT products, separately linked products and frozen runtime copies before `baseline.json` is saved and formal timing starts. Local `results.json` retains commands, output, digests, CPU checks and samples. Runtime binaries and final build products remain in the delivery directory and are not committed to Git.

The five configurations are native/off, native/full, bitcode/off, bitcode/full and bitcode/thin, with unchecked O3, malloc and a 512 MiB heap budget. The full functional matrix warms compiler and standard-library caches before measurement; caches are not cleared between formal samples. Execution is serial and configuration order rotates using a fixed random seed. CPU is checked outside every timed command. Usage of 40% or more, or unavailable readings, causes a wait; exceeding the wait budget saves `deferred` without claiming completion. Frequency and affinity are not pinned, and CPU records describe startup checks only.

Build timing defaults to three repetitions each for native consumer-object compilation, independent linking of that object with selected libraries, and complete AOT. The independent consumer is already machine code, while complete AOT can optimize the main unit; the first two timings do not add up to complete AOT. Runtime defaults to five processes per configuration, each warming up once before measuring five seeds, giving 25 kernel samples and their median. `--build-repetitions` and `--runtime-processes` can increase counts, with a minimum of three rounds.

Compilers and VSIX packages rebuilt after documentation receive separate functional acceptance; frozen measurements are not relabeled as results for those new binaries. These workloads establish no universal benefit across programs, platforms or LLVM variants and provide no cross-language ranking.

## Frozen comparison with a pre-release static LLVM build

The measured development build predates the 1.0.0 release and retains its original identifier, `1.0.12`.

This comparison ran on 2026-11-02 under Windows 10 x64 on an AMD Ryzen 5 5600X (6 cores/12 logical processors), using embedded LLVM 23.1.4 and GCC 11.2.0 for the C11 timing shell. Compiler source was `66afd9adc30650649493c4d618638b84d1edbdae`, with actual binary SHA-256 `2bc93306c8b4825bcf1242a3e123b353a0bc76c310ab68fda396d03f20126d68`. These measurements belong only to those frozen bytes. Compilers and VSIX packages rebuilt after documentation receive separate functional checks without relabeling this data.

Formal records comprise 66 build samples, 50 runtime processes and 250 kernel samples. Each workload/configuration median uses five processes with five kernel samples each. Initialization, warmup, aggregation and cleanup are excluded. CPU is checked outside each of 116 timed commands; startup readings ranged from 1% to 29%, all below 40%. Frequency and affinity were not pinned, and the machine is not claimed to have remained fully idle. The complete functional matrix warmed caches, which were not cleared between samples; configuration order rotated with a fixed seed.

| Library/LTO | call kernel (ms) | bulk kernel (ms) | AOT DLL (bytes) | Separately linked DLL (bytes) |
| --- | ---: | ---: | ---: | ---: |
| native/off | 10.2953 | 6.3899 | 4,886,528 | 4,886,528 |
| native/full | 11.0066 | 6.0879 | 4,877,824 | 4,886,528 |
| bitcode/off | 8.9765 | 6.5387 | 4,886,528 | 4,886,528 |
| bitcode/full | 1.1566 | 1.1423 | 4,876,288 | 4,886,016 |
| bitcode/thin | 1.1462 | 1.1523 | 4,876,288 | 4,886,016 |

DLL sizes happened to match across these two workloads. AOT sizes refer to frozen runtime copies; independent-link sizes refer to final actual build products. Bitcode Full/Thin LTO reduced AOT DLL size by 10,240 bytes relative to native/off, without establishing a universal size reduction.

Median native consumer-object compilation took 2,140.1 ms for call and 2,065.1 ms for bulk, each from three repetitions. Other build timings are three-repetition medians:

| Library/LTO | call independent link (ms) | call complete AOT (ms) | bulk independent link (ms) | bulk complete AOT (ms) |
| --- | ---: | ---: | ---: | ---: |
| native/off | 590.2 | 2,425.6 | 610.5 | 2,428.4 |
| native/full | 617.8 | 2,585.4 | 585.6 | 2,378.8 |
| bitcode/off | 643.0 | 2,396.9 | 633.5 | 2,494.9 |
| bitcode/full | 611.8 | 2,397.1 | 604.2 | 2,428.6 |
| bitcode/thin | 631.0 | 2,381.5 | 636.7 | 2,520.6 |

Compile/link wall times include process startup; independent linking also includes selected-library bitcode re-encoding or materialization. Independent linking uses an already generated native consumer object, whereas complete AOT can optimize the main unit. Their compile/link timings therefore do not add up to complete AOT. Enabling main-unit LTO alone with native libraries did not consistently improve results, and neither ThinLTO nor Full LTO had a universal timing advantage. Only these two arithmetic kernels showed the recorded library-bitcode LTO benefit; no conclusion is extended to other programs, platforms, allocators or LLVM variants.

Raw commands, per-command CPU checks, seed results, every sample, digests and runtime binaries remain local under `build/independent-validation/` and are not committed to Git. Actual `results.json` SHA-256 is `8c572e9c391001ccf5216a46ca6a7a9e2ea02019e073eaca926622177665f770`. Independent checks confirmed all 116 timed commands succeeded, 250 samples matched their checksums, and frozen bytes were unchanged. Historical 1.0.10/1.0.11 and other-language identities remain unchanged and were not remeasured.
