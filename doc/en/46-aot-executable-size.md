# Native executable size and sample validation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../46-aot-executable-size.md)
<!-- END LANGUAGE NAVIGATION -->

## Measurement configuration

Measurements were performed on Windows x64 on 2026-11-05 with ChaLi 1.0.0 and statically embedded LLVM. The original release corresponds to commit `76c0c5b`. The first size reduction was rebuilt from commit `3d9fb60`; the subsequent C service reconstruction retains version 1.0.0 and uses the repository-pinned LLVM 23.1.4 and GCC 16.2.0. All sizes are actual EXE file sizes in bytes.

Compilation, optimization and linking used the new compiler's normal `chali build` entry point. Executables ran locally on Windows and use the system C runtime.

## Hello World baseline

The reference program follows.

```chali
void main()
{
    println("Hello World");
}
```

Recompiling with the original release produced 4,938,752 bytes. The new compiler produced 10,240 bytes, or 10 KiB, with LTO disabled, Full LTO and ThinLTO. The reduction is approximately 99.79%, calculated as `(4,938,752 - 10,240) / 4,938,752`.

Each of the three new executables was run without arguments, with `--help`, and with two ordinary arguments. All printed `Hello World` followed by a newline, exited with zero and left standard error empty.

## Historical first reduction across LTO modes

The table covers 9 programs and 27 native compilations. The other 8 programs use existing repository samples. Every executable's standard output was compared with `example/expected/<sample>.txt`, with a zero exit code and empty standard error required.

| Program | Coverage | LTO off | Full LTO | ThinLTO |
| --- | --- | ---: | ---: | ---: |
| `tt.chl` | Hello World | 10,240 | 10,240 | 10,240 |
| `example/samples/array2d.chl` | Two-dimensional arrays | 2,148,864 | 2,139,136 | 2,139,136 |
| `example/samples/dynamic_array.chl` | Dynamic arrays | 2,701,312 | 2,690,560 | 2,691,072 |
| `example/samples/strings.chl` | Strings | 2,208,256 | 2,198,016 | 2,198,016 |
| `example/samples/generic_inference.chl` | Generic inference | 2,225,664 | 2,216,448 | 2,216,448 |
| `example/samples/interface.chl` | Interfaces | 2,233,344 | 2,224,128 | 2,224,128 |
| `example/samples/drop_destructor.chl` | Destructors | 2,143,232 | 2,134,016 | 2,134,016 |
| `example/samples/recursion.chl` | Recursion | 2,134,016 | 2,124,800 | 2,124,800 |
| `example/samples/try_custom.chl` | Custom error propagation | 2,229,248 | 2,219,008 | 2,219,008 |

The 10 KiB size and 99.79% reduction apply to the measured Hello World benchmark. The more complex programs in the table require language host services, and their sizes depend on their actual dependencies.

## Subsequent C service measurement (version 1.0.0)

The following follow-up covers 15 programs and 45 native compilations. Output, exit codes and diagnostics were checked for every executable. The earlier measurements above retain their original commit and configuration; these follow-up sizes include the C numeric and collection services.

| Program | Coverage | LTO off | Full LTO | ThinLTO |
| --- | --- | ---: | ---: | ---: |
| `tt.chl` | Hello World | 10,240 | 10,240 | 10,240 |
| `array2d` | 2D arrays | 956,416 | 947,712 | 947,712 |
| `dynamic_array` | Dynamic arrays | 1,509,376 | 1,499,136 | 1,499,648 |
| `strings` | Strings | 1,015,808 | 1,006,080 | 1,006,080 |
| `generic_inference` | Generic inference | 1,033,728 | 1,024,000 | 1,024,000 |
| `interface` | Interfaces | 1,041,408 | 1,032,192 | 1,032,192 |
| `drop_destructor` | Destructors | 951,296 | 942,080 | 942,080 |
| `recursion` | Recursion | 941,568 | 932,352 | 932,352 |
| `try_custom` | Error propagation | 1,036,800 | 1,027,584 | 1,027,584 |
| `value_text` | Dynamic value text | 1,572,864 | 1,561,088 | 1,561,088 |
| `float` | Floating point | 947,712 | 938,496 | 938,496 |
| `thread_channel` | Thread channels | 1,237,504 | 1,228,288 | 1,228,288 |
| `std_math_clock_random` | Math, clock and random | 1,088,000 | 1,078,784 | 1,078,784 |
| `std_file_strings` | Files and strings | 1,821,696 | 1,809,920 | 1,809,920 |
| `std_structured_data` | JSON/TOML and structured data | 2,375,680 | 2,364,416 | 2,364,928 |

## Runtime and failure-path samples

The new compiler also ran checked `run` and native execution comparisons for the following 12 samples. All compiled successfully, exited with zero, produced matching standard output, and left native program standard error empty.

- Closures and capture modes cover `lambda_callbacks` and `lambda_capture_modes`.
- Errors, structured data, reflection serialization, binary data and UTF-8 cover `std_errors`, `std_structured_data`, `reflect_serialization`, `std_binary` and `std_utf8`.
- Collection algorithms cover `std_collections_algorithms`.
- Thread messages, channels, thread pools and timed events cover `thread_messages`, `thread_channel`, `thread_pool` and `event_timer`.

The runtime regression also checked integer division by zero, integer division overflow, out-of-range shifts and floating-point division by zero. Both checked `run` and native execution failed at the expected point, retained output produced before the failure, and reported the expected diagnostic. An infinite loop with a 1,000-step limit also stopped as expected, with matching exit codes, output and diagnostics.

```powershell
python tests/chali_aot_runtime.py --chali path/to/chali.exe --samples lambda_callbacks lambda_capture_modes std_errors std_structured_data reflect_serialization std_binary std_utf8 std_collections_algorithms thread_messages thread_channel thread_pool event_timer
```

These results cover the Windows x64 samples and failure paths listed on this page.

## Conditions for trimming

For an ordinary Windows EXE built with static LLVM, the compiler copies the LLVM module, attempts to omit host lifecycle calls, and lets LLVM remove unreachable definitions. It uses the lightweight module and omits the general host archive only when every remaining external reference is an allowed C standard-output interface or LLVM intrinsic.

Modules requiring objects, dynamic storage, native glue or thread services retain host references and use the full runtime. Explicit exports, external objects or libraries, shared libraries, debug builds, and object-only or assembly-only requests follow their respective full build paths. The lightweight path currently applies to the Windows compiler with statically embedded LLVM.

Importing `std::gui`, `std::audio` or `std::xml` alone adds no native dependency. Reachable calls select the archives, LLVM removes unreachable definitions before emitting code, and LLD extracts members by symbol and discards unused sections. The resulting Windows EXE contains the required C code and needs no extension DLLs or runtime extraction. Run `python tests/chali_optional_namespaces.py --embedded --chali path/to/chali.exe` to reproduce the check. See [build and distribution instructions](45-optional-libraries.md) for compiler and platform requirements.

The Windows x64 tests relocated each EXE by itself to a new directory. Importing all three modules without calling them still produced 10,240 bytes in every LTO mode. The combined GUI, audio and XML sample measured 3,801,600, 3,790,336, 3,790,336 bytes with LTO off, Full LTO and ThinLTO, respectively.

## Reproduction

Run from the repository root, with `chali.exe` pointing to the rebuilt static LLVM compiler.

```powershell
chali.exe build tt.chl --lto-mode off --out tmp/tt-off
chali.exe build tt.chl --lto-mode full --out tmp/tt-full
chali.exe build tt.chl --lto-mode thin --out tmp/tt-thin
Get-Item tmp/tt-off.exe,tmp/tt-full.exe,tmp/tt-thin.exe |
    Select-Object Name,Length
chali.exe build example/samples/dynamic_array.chl --lto-mode full --out tmp/dynamic-array
./tmp/dynamic-array.exe
```

`tests/chali_aot_static.py` checks the basic output program's behavior and size limit in all three LTO modes, alongside container calculations and a C ABI shared library. `tests/chali_aot_runtime.py` compares runtime samples under checked `run` and native execution.

## On-demand C services and standard module names

Built-in modules declare `std::xxx`; embedded asset generation checks that each declaration matches its filename. The GUI, audio and XML names are `std::gui`, `std::audio` and `std::xml` in the examples, book and regression fixtures. An explicitly selected external module directory remains useful for dynamic library compatibility tests.

Numeric Lists retain their stable data-pointer, length and capacity header for direct LLVM access, with backing storage managed by a C buffer. Generic List slots, dense Map/Set entries, aggregate key fields and language hash slots use a C object sequence. The C++ adapter supplies construction, move and destruction callbacks for existing value types. Native hash buckets also use a C open-addressed index. Storage captures its original allocator; growth allocates successfully before moving elements and preserves existing values on allocation failure.

Platform socket entry points, initialization, address resolution, blocking-mode changes, local-port queries and readiness waits live in an independent C service. Windows loads Winsock dynamically and uses stack-based readiness sets. Linux uses libc sockets and `poll`, including descriptors above `FD_SETSIZE`. These services participate in section garbage collection in the host archive; unused services are not retained solely because their source exists.

Language records, nested-value ownership and error boundaries retain C++ adapters. CAS symbol interning, structural equality, hashing and rational ordering also use C services. High-level symbolic rules, the expression API and regular expressions retain C++ implementations, so the complete runtime is not a pure C runtime. Measurements distinguish minimal applications from programs that actually reference complex services.

```powershell
python tests/chali_optional_namespaces.py --chali path/to/chali.exe --desktop build/desktop_audio --xml build/xml
python tests/chali_c_runtime_storage.py
python tests/chali_numeric_list_storage.py --build-root build/llvm-static-gcc16
python tools/run_module_gates.py collections net file --chali path/to/chali.exe --build-root build/llvm-static-gcc16
```

## Layered thresholds for C integer multiplication

The C11 multiplication kernel retains version 1.0.0. Its layered design follows the general approach of OpenJDK BigInteger, with independently implemented schoolbook multiplication, specialized squaring and Toom-Cook 3 alongside Karatsuba and two-prime CRT+NTT. No Java source is copied. The C++ compatibility layer preserves the existing interface, ownership and exception protocol; division retains Knuth / Burnikel-Ziegler.

Lengths below are **32-bit words**, not decimal digits. These limits admit candidates; the dispatcher still compares estimated costs.

| Parameter | Native 128-bit products | Portable products |
| --- | ---: | ---: |
| Direct schoolbook path, shorter operand maximum | 64 | 128 |
| Schoolbook cost comparison, shorter operand candidate maximum | 512 | 4096 |
| Specialized schoolbook square candidate maximum | 256 | 256 |
| Toom-Cook 3 multiplication candidate minimum, longer operand | 160 | 160 |
| Toom-Cook 3 square candidate minimum | 256 | 256 |
| CRT+NTT candidate minimum, sum of operand lengths | 6144 | 6144 |
| NTT shorter-operand protection limit | 96 | 96 |

Single-word multiplication keeps its existing fast path. Toom compares the actual evaluation and endpoint slice costs without a fixed operand-ratio limit. Schoolbook, Karatsuba and Toom use integer cost estimates with a ten percent schoolbook margin and a 6.25 percent Toom margin. NTT compares the actual padded transform size times its logarithm, with a separate square weight. No floating-point power or additional host service is needed.

NTT has no permanent crossover: increasing two equally sized operands from 7680 to 7681 words doubles the padded 15-bit transform from 32768 to 65536 points. Toom can become faster again. Schoolbook eligibility uses the shorter operand so that unbalanced products remain eligible. Since both NTT primes are below 2^31, equivalent unsigned-mask corrections replace data-dependent butterfly branches generated by GCC 11.

Windows x64 and Linux x64, each with native and portable products, passed 3978 complete Python product comparisons and 1402 alias/square checks per configuration: 15912 products and 5608 alias/square checks in total. Cases include zero, signs, carry-heavy inputs, unbalanced lengths, splitting and transform-padding boundaries, and operands up to 32768 words. Test builds can force each top-level algorithm while recursive calls use the production policy; production builds omit these hooks.

Additional failure-injection checks on both platforms cover Toom multiplication, squares and output aliases. The same round also passed 211570 allocation-failure points across 29 C decimal operations on each platform.

Reproduce correctness and timing with:

```powershell
python tests/chali_bigint_multiplication.py
python tests/chali_bigint_multiplication.py --portable
python tests/chali_bignum_c_ownership.py --kind integer
python tests/bigint_multiplication_benchmark.py --build --binary tmp/bigint-bench.exe --phase audit --default-policy --paired --samples 11 --target-us 10000 --output tmp/bigint-audit.json
python tests/bigint_multiplication_benchmark.py --build --portable --binary tmp/bigint-bench-portable.exe --phase audit --default-policy --paired --samples 11 --target-us 10000 --output tmp/bigint-audit-portable.json
```

The same entry points work on Linux; executable names may omit `.exe`. Use `python3 tests/chali_bignum_c_ownership.py --kind integer --sanitize` for address and undefined-behavior checks. Benchmarks record compiler version, arguments, binary/source hashes, medians, p10/p90, allocation counts and peak working memory. Algorithm order is shuffled within each repeated input; every sample checks the complete product and zero remaining live allocations. Only the benchmark thread affinity is changed. Raw CSV and JSON evidence remains local and is not committed.

### Threshold measurement configuration and results

Measurements use one Ryzen 5 5600X with Windows GCC 16.2.0 and Ubuntu 22.04 / WSL GCC 11, each with native and portable products. Portable disables native 128-bit products on x64; it does not validate a 32-bit operating system. Final timing uses `-O2`, matching the production host, and the default C allocator. Separate `-O3` calibration and held-out seeds are retained.

Algorithm order is shuffled within each input and repeated for 11 paired timing ratios; candidate-boundary comparisons use 15 repetitions. Ratios below are the largest median automatic/forced-candidate ratio for the measured candidates; 1.10 means ten percent additional time. Stages overlap, so their counts are not independent sample counts. Raw p10/p90, CSV, compiler arguments and hashes remain local.

| System | Products | Stage | Input shapes | Ratio≤1.10 | Ratio≤1.20 | Maximum ratio |
| --- | --- | --- | ---: | ---: | ---: | ---: |
| windows | native | boundary | 58 | 57 | 58 | 1.103 |
| windows | native | ntt | 40 | 40 | 40 | 1.024 |
| windows | native | skew | 77 | 77 | 77 | 1.078 |
| windows | portable | boundary | 58 | 58 | 58 | 1.099 |
| windows | portable | ntt | 40 | 39 | 40 | 1.145 |
| windows | portable | skew | 77 | 76 | 77 | 1.122 |
| linux | native | boundary | 58 | 51 | 56 | 1.256 |
| linux | native | ntt | 40 | 40 | 40 | 1.046 |
| linux | native | skew | 77 | 77 | 77 | 1.079 |
| linux | portable | boundary | 58 | 52 | 57 | 1.311 |
| linux | portable | ntt | 40 | 39 | 40 | 1.117 |
| linux | portable | skew | 77 | 76 | 77 | 1.136 |

The initial portable 4096×1024-word case was approximately 59%–94% slower than forced Toom. The final policy removes the fixed ratio limit and separately estimates the three evaluations and two endpoints, assigning zero cost to empty slices and using actual high-slice lengths. In the Windows `-O2` paired run, automatic Toom takes approximately 1.242 ms versus 2.003 ms for forced Karatsuba.

These are reasonable defaults for the measured hardware, compilers and allocator, with visible Linux/WSL variation. Extreme operand ratios can still benefit from dedicated blocking; actual outliers are retained. Independent C decimal checks also cover 120000-digit mantissas, and native/portable Toom ownership tests pass ASan/UBSan on Linux.
