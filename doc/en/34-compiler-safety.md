# Compiler safety boundaries and focused regression tests

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../34-compiler-safety.md)
<!-- END LANGUAGE NAVIGATION -->

> The current compiler uses checked and unchecked LLVM native paths. This page separates current regression entry points, design boundaries and historical measurements; delivery reports describe only validation actually performed.

This page records protection for confirmed defects. It does not claim that the compiler has no other defects.

## Representative inputs and measured boundaries

The [case catalog](../../tests/compiler_limits_cases.py) selects 25 cases each for everyday application components, advanced compositions and bounded stress inputs.
The [driver](../../tests/chali_compiler_limits.py) checks compilation, execution and an independent expected result for checked and unchecked native paths, retaining exit codes, diagnostics, timeouts and output bytes.
Discovery finishes with a frozen inventory before implementation fixes begin. Use `--verify` for regression checks that fail on unexpected outcomes.

The [0.31.0 discovery snapshot](../../tests/fixtures/compiler_limits/discovery_0_31_0.json) records sources, host, LLVM, build options and compiler SHA256.
The initial closure example also involved borrowing local parameters. The [legal copy-capture follow-up](../../tests/fixtures/compiler_limits/return_type_followup_0_31_0.json) isolates a failure with inferred generic arguments while explicit arguments work.
New closures produced by automatic instantiation must have their functions and types registered before semantic replay.
The [focused regression](../../tests/chali_generic_function_returns.py) covers closure copies, floating values, container storage and nested calls, including the diagnostics for invalid return values and escaping local borrows.

For overloaded callbacks, generic inference first collects bindings from ordinary arguments and then filters candidates by the full function signature.
The selected signature remains attached to the expression through type checking and lowering. If multiple candidates remain, the compiler requires explicit generic arguments or a function variable with an explicit signature.

The following table records **historical 0.31.0 measurements** with a separate 64 MiB execution budget; it is not a current backend capability table:

| Input | 0.31.0 historical interpreter | 0.31.0 LLVM build |
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

Platform fixes avoid duplicate long/int64 constructors on LP64, delay JSON/TOML member template definitions until value types are complete, and avoid a glibc htons macro collision.

Run `python tests/chali_compiler_safety.py`. On Linux, `--cxx clang++ --sanitize-native` enables ASan, UBSan and float-to-integer checks for native boundaries; `--native-only` runs only that portion. Reflection remains enabled in both frontend and runtime; checked and unchecked paths retain their respective default optimization levels.

Penetration testing: `python tests/chali_penetration.py` (`--only SUBSTRING` narrows it down). It attacks numeric conversion, literals, container boundaries, ownership propagation and size budgets, then classifies each case as UB / CRASH / TIMEOUT / FAIL / OUTPUT — it collects facts only and leaves the verdict to the reader.

The tests cover repeated success/error/container propagation and overwrite disk data while optimization output is blocked. See [the native boundary test](../../tests/chali_native_safety.cpp). Large-number operations remain subject to host memory and time budgets. Delivery reports distinguish completed checks from deferred platform and long-running validation.

Compilation and program execution use separate memory budgets. The default frontend pool is 512 MiB; --memory-size changes the execution heap, not the frontend pool. Frontend allocation failures report capacity, live bytes at failure and the requested allocation, then return a nonzero status instead of propagating std::bad_alloc. Parsed replay snapshots are released before quadruple generation once monomorphization finishes. The combined standard-error fixture still reaches a large contiguous-allocation boundary in this pool. Live bytes do not describe the largest free contiguous block, and this observation is not a universal limit on library combinations. Run python tests/chali_compiler_budget.py for Chinese and English native-build budget diagnostics.

## Complex syntax, semantic composition and formatting round trips

`python tests/chali_syntax_semantic_stress.py` reads complete positive/error examples, builds original and formatted sources in checked and unchecked modes, compares independent expected output and checks formatter idempotence. Timeouts terminate the process tree; crashes, silent failures and unexpected acceptance fail the test. Agreement between two modes alone is not an oracle.

Fixed inputs cover 192 nested parentheses, 48 blocks, 16 generic-container levels, a 64-term expression, macro arguments containing Map/List/tuple/function types and adjacent closing angle brackets, higher-order callbacks, nested enum wrapping, cross-module generic borrows, and 512 nested Result/defer paths. Runnable inputs: [depth](../../example/samples/syntax_pressure_depth.chl), [macro type punctuation](../../example/samples/syntax_macro_type_tokens.chl), [nested callback arguments](../../example/samples/syntax_nested_function_argument.chl), and [semantic composition](../../example/samples/syntax_semantic_composition.chl). These are test points rather than an unlimited-depth guarantee.

## Analysis order and corrections

Module/text expansion precedes grammar parsing; raw macro arguments are then structured by category. Symbolic generic signatures guide concrete instantiation requests, registration and semantic replay. Reflection-generated declarations also trigger replay. Parsed snapshots, lifetime directives and symbolic type indexes must survive replay without temporary signature-probe nodes contaminating real contracts.

Semantic lowering records bound storage, origins, control exits and cleanup obligations. Contracts are checked on concrete control flow; reference-move summaries reach a fixed point across concrete functions before final caller invalid-access checks. Quadruples and LLVM native output follow. The stage order is retained; fixes address facts and identity crossing those stages:

- Moves through macro/function references invalidate known owners and aliases.
- Deferred captures are checked on actual exit edges.
- Storage-release loops separate initial and later iterations within a budget based on this function. One analysis clone cannot justify removing the shared source cleanup operation.
- Owned arrays with no borrowed elements gain an independent result owner when yielded; arrays containing borrows retain origin checks.
- Missing nested generic reference wrappers are reconstructed from canonical instance arguments, not truncated mangled names.

[Generic heap traversal](../../example/samples/borrow_loop_heap_traversal.chl) covers while, for-in, conditional-break loop and 384 closes; negative examples still reject freed-node/alias access and moves repeated across a loop backedge.

## Known limitations and diagnostic boundaries

| Boundary | Current behavior/workaround |
| --- | --- |
| C-style for and do-while | `for(init; condition; update)` supports loop-local declarations, optional parts, comma-ordered updates, and continue through the update. Do-while remains outside the surface grammar; use while or standalone `loop { ... }`. Positive sample: syntax_for_loops; rejection sample: syntax_limit_do_while |
| AST fragment traversal | Maximum depth 256 and 1000000 visited nodes; this is not a character or parenthesis count. A 600-term left-associated sum is diagnosed, while the fixed 192-parenthesis input passes. Split large generated expressions into statements/functions |
| Incomplete complex type punctuation | Missing generic closers must produce diagnostics without crashes or silent acceptance; syntax_limit_missing_generic_closer records this boundary |
| Correlated conditions | Arbitrary predicate/negation relationships are not proved; syntax_limit_path_correlation is a known conservative precision limit. Keep selection and return in one branch structure, as syntax_helpers::structural does |
| Loop peeling | Bounded precision improvement; exhausted budget retains conservative checks. It does not prove arbitrary heap graphs acyclic or enumerate unknown field-pointer aliases |
| Array value-expression returns containing borrows | Known conservative precision limit: even global references may be rejected through `return { ...; yield array; };`. Use an ordinary return of a named array; see [the verified workaround](../../example/samples/syntax_borrowed_array_named.chl). syntax_limit_borrowed_array_value_return records the limitation; local-reference escape cases remain rejected |
| Generics and contracts | Bodies are checked for concrete instances rather than universally for unused instances. Function values do not carry #borrow/#static contracts; see [lifetimes](22-resource-lifetime.md) |
| for-in fallback variables | Empty input uses ordinary value defaults. Borrowed elements need a copyable snapshot before the next iterator call. References or borrowed aggregates without a proven valid origin are conservatively rejected in the fallback; handle them inside the body. Samples: syntax_for_else and syntax_for_else_stress |
| Enum wrapping | The target enum and type arguments must be known. Equal best payload matches need an explicit variant; move and escape checks remain |

Distinguish safe rejection, known precision limits and actual false positives/negatives. Compiler memory, instantiation rounds and host resources remain bounded; one passing case does not imply arbitrary complex programs are parseable.
