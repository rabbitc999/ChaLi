# Frozen precompiled-library LTO comparison

**English** · [简体中文](README.md)

This directory compares library implementation and LTO settings of one ChaLi static LLVM compiler. It does not rerun or relabel other-language or older-version baselines. Current results, identities and limits are in [library packages](../../doc/en/44-library-packages.md).

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
