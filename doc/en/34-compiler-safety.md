# Compiler safety boundaries and focused regression tests

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../34-compiler-safety.md)
<!-- END LANGUAGE NAVIGATION -->

> Status: the changes below are in source, and parser regeneration plus compiler rebuilding are done; both the focused regression and the penetration suite pass locally. No penetration case is classified UB / CRASH / TIMEOUT any more — only diagnosable failures and normal output remain.

This page records protection for confirmed defects. It does not claim that the compiler has no other defects.

## Representative inputs and measured boundaries

The [case catalog](../../tests/compiler_limits_cases.py) selects 25 cases each for everyday application components, advanced compositions and bounded stress inputs.
The [driver](../../tests/chali_compiler_limits.py) checks compilation, execution and an independent expected result for VM and LLVM AOT, retaining exit codes, diagnostics, timeouts and output bytes.
Discovery finishes with a frozen inventory before implementation fixes begin. Use `--verify` for regression checks that fail on unexpected outcomes.

The [0.31.0 discovery snapshot](../../tests/fixtures/compiler_limits/discovery_0_31_0.json) records sources, host, LLVM, build options and compiler SHA256.
The initial closure example also involved borrowing local parameters. The [legal copy-capture follow-up](../../tests/fixtures/compiler_limits/return_type_followup_0_31_0.json) isolates a failure with inferred generic arguments while explicit arguments work.
New closures produced by automatic instantiation must have their functions and types registered before semantic replay.
The [focused regression](../../tests/chali_generic_function_returns.py) covers closure copies, floating values, container storage and nested calls, including the diagnostics for invalid return values and escaping local borrows.

For overloaded callbacks, generic inference first collects bindings from ordinary arguments and then filters candidates by the full function signature.
The selected signature remains attached to the expression through type checking and lowering. If multiple candidates remain, the compiler requires explicit generic arguments or a function variable with an explicit signature.

Stress runs use a separate 64 MiB execution budget and the default compiler pool. Recorded points include:

| Input | VM | LLVM AOT |
| --- | --- | --- |
| A plain addition expression with 512 terms | Fragment depth or size budget diagnostic | Same diagnostic |
| A dynamic array requesting approximately 128 MiB | Heap capacity diagnostic | Heap capacity diagnostic |
| Read/write access to an 8192-element fixed array | Execution preparation exceeds the 64 MiB budget and reports insufficient capacity | Passes |

These observations apply to the recorded host, implementation and budgets. Re-measure boundaries when capacity, toolchains or input structure change.

| Boundary | Behaviour |
| --- | --- |
| Integer data export beyond double precision | Parsed as 64-bit directly, preserving the signed minimum and unsigned maximum |
| Large floating values exported to the data area | Integer conversion range is checked first |
| BigFloat power and root internals | Exponent range checked before multiplication; precision growth avoids signed overflow |
| Owning `?` result as an enum constructor argument | The constructor takes the temporary and clears the original owning slot |
| Concurrent writers on one output prefix | Direct source execution loads its generated data from an in-memory snapshot |
| Float-to-integer conversion out of range | Runtime reports "float to integer out of range"; constant folding refuses out-of-range values |
| Decimal integer literal beyond int64 | Compile-time diagnostic (previously saturated silently to `INT64_MAX`); `-9223372036854775808` is the exception, where the unary minus types the 2^63 magnitude as the minimum |
| Floating literal overflowing to inf / NaN | Compile-time diagnostic (previously produced "undefined variable: inf.0") |
| JSON / TOML nesting depth | Beyond 256 levels is a parse diagnostic (previously overflowed the host stack) |
| Local fixed-size arrays | Compile-time "fixed array exceeds the current memory budget" (previously `std::bad_alloc` escaped as `terminate`), matching struct fields |
| Fixed-array dimension product | Accumulated in 64 bits and clamped at the int bound (previously `int` overflow produced wrong loop bounds and slot offsets) |
| Negative string slice endpoints | Runtime diagnostic (previously `"abcdef"[0:-2]` silently returned `abcdef`); `end == -1` remains the built-in ABI's "to the end" convention |
| Re-entrant container callbacks | `vecFold` / `vecMap` / `vecFilter` and predicate callbacks lease the iterator; `push` / `clear` inside them is rejected |
| Mutation during iteration | `push` / `set` share the "cannot move or remove list elements while a for-in reference is active" guard with `remove` |
| `INT64_MIN / -1` | Runtime overflow diagnostic (previously returned `-9223372036854775808`) |
| Shift amount beyond width | Runtime diagnostic; the valid range is 0..63 (previously masked with `& 63`, so `1 << 64` yielded `1`) |

Platform fixes avoid duplicate long/int64 constructors on LP64, delay JSON/TOML member template definitions until value types are complete, and avoid a glibc htons macro collision. The standalone VM CMake target declares its CAS, SQL generation, SQLite, compression, include and host dependencies.

Run `python tests/chali_compiler_safety.py`. On Linux, `--cxx clang++ --sanitize-native` enables ASan, UBSan and float-to-integer checks for native boundaries; `--native-only` runs only that portion. Reflection remains enabled in both frontend and runtime, with default maximum VM and quadruple optimization.

Penetration testing: `python tests/chali_penetration.py` (`--only SUBSTRING` narrows it down). It attacks numeric conversion, literals, container boundaries, ownership propagation and size budgets, then classifies each case as UB / CRASH / TIMEOUT / FAIL / OUTPUT — it collects facts only and leaves the verdict to the reader.

The tests cover repeated success/error/container propagation and overwrite disk data while optimization output is blocked. See [the native boundary test](../../tests/chali_native_safety.cpp). Large-number operations remain subject to host memory and time budgets. Delivery reports distinguish completed checks from deferred platform and long-running validation.

Compilation and program execution use separate memory budgets. The default frontend pool is 512 MiB; --memory-size changes the execution heap, not the frontend pool. Frontend allocation failures report capacity, live bytes at failure and the requested allocation, then return a nonzero status instead of propagating std::bad_alloc. Parsed replay snapshots are released before quadruple generation once monomorphization finishes. The combined standard-error fixture still reaches a large contiguous-allocation boundary in this pool. Live bytes do not describe the largest free contiguous block, and this observation is not a universal limit on library combinations. Run python tests/chali_compiler_budget.py for Chinese VM and English AOT budget diagnostics.
