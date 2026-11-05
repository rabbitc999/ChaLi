# AOT executable size and sample validation

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../46-aot-executable-size.md)
<!-- END LANGUAGE NAVIGATION -->

## Measurement configuration

Measurements were performed on Windows x64 on 2026-11-05 with ChaLi 1.0.0 and statically embedded LLVM. The original release corresponds to commit `76c0c5b`. The new compiler was rebuilt from commit `3d9fb60`, which contains the size reduction, using the repository-pinned LLVM 23.1.4 and GCC 16.2.0. All sizes are actual EXE file sizes in bytes.

Compilation, optimization and linking used the new compiler's normal `chali aot` entry point. Executables ran locally on Windows and use the system C runtime.

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

## Other programs across LTO modes

The table covers 9 programs and 27 AOT compilations. The other 8 programs use existing repository samples. Every executable's standard output was compared with `example/expected/<sample>.txt`, with a zero exit code and empty standard error required.

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

## Runtime and failure-path samples

The new compiler also ran checked `run` and AOT comparisons for the following 12 samples. All compiled successfully, exited with zero, produced matching standard output, and left AOT standard error empty.

- Closures and capture modes cover `lambda_callbacks` and `lambda_capture_modes`.
- Errors, structured data, reflection serialization, binary data and UTF-8 cover `std_errors`, `std_structured_data`, `reflect_serialization`, `std_binary` and `std_utf8`.
- Collection algorithms cover `std_collections_algorithms`.
- Thread messages, channels, thread pools and timed events cover `thread_messages`, `thread_channel`, `thread_pool` and `event_timer`.

The runtime regression also checked integer division by zero, integer division overflow, out-of-range shifts and floating-point division by zero. Both checked `run` and AOT failed at the expected point, retained output produced before the failure, and reported the expected diagnostic. An infinite loop with a 1,000-step limit also stopped as expected, with matching exit codes, output and diagnostics.

```powershell
python tests/chali_aot_runtime.py --chali path/to/chali.exe --samples lambda_callbacks lambda_capture_modes std_errors std_structured_data reflect_serialization std_binary std_utf8 std_collections_algorithms thread_messages thread_channel thread_pool event_timer
```

These results cover the Windows x64 samples and failure paths listed on this page.

## Conditions for trimming

For an ordinary Windows EXE built with static LLVM, the compiler copies the LLVM module, attempts to omit host lifecycle calls, and lets LLVM remove unreachable definitions. It uses the lightweight module and omits the general host archive only when every remaining external reference is an allowed C standard-output interface or LLVM intrinsic.

Modules requiring objects, dynamic storage, native glue or thread services retain host references and use the full runtime. Explicit exports, external objects or libraries, shared libraries, debug builds, and object-only or assembly-only requests follow their respective full build paths. The lightweight path currently applies to the Windows compiler with statically embedded LLVM.

## Reproduction

Run from the repository root, with `chali.exe` pointing to the rebuilt static LLVM compiler.

```powershell
chali.exe aot tt.chl --lto-mode off --out tmp/tt-off
chali.exe aot tt.chl --lto-mode full --out tmp/tt-full
chali.exe aot tt.chl --lto-mode thin --out tmp/tt-thin
Get-Item tmp/tt-off.exe,tmp/tt-full.exe,tmp/tt-thin.exe |
    Select-Object Name,Length
chali.exe aot example/samples/dynamic_array.chl --lto-mode full --out tmp/dynamic-array
./tmp/dynamic-array.exe
```

`tests/chali_aot_static.py` checks the basic output program's behavior and size limit in all three LTO modes, alongside container calculations and a C ABI shared library. `tests/chali_aot_runtime.py` compares runtime samples under checked `run` and AOT.
