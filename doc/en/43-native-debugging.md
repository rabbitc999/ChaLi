# Native AOT debugging

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../43-native-debugging.md)
<!-- END LANGUAGE NAVIGATION -->

ChaLi 1.0.8 executes `run` through checked AOT compilation, while `aot` defaults to an unchecked native export. The compiler no longer embeds a VM execution engine; `.svmb` export has been removed; use native AOT artifacts. Debugging uses machine code, DWARF source locations and actual frame slots, rather than VM breakpoints or interpreter state.

## VS Code

Install the ChaLi extension and a GDB matching the target platform. Open a `.chl` file and press F5. The extension builds a checked native debug program, then uses GDB/MI for source breakpoints, continue, step in/over/out, stack frames, parameter/local-variable inspection and expression evaluation.

```json
{
  "type": "chali",
  "request": "launch",
  "name": "ChaLi native debugging",
  "program": "${file}",
  "cwd": "${fileDirname}",
  "debuggerPath": "gdb",
  "stopOnEntry": false,
  "args": []
}
```

`debuggerPath` selects GDB; otherwise the adapter uses `CHALI_GDB`, then `gdb` on PATH. Its architecture must match the generated program. Compiler, adapter and GDB subprocesses inherit the same project `tmp/` directory, where native debug artifacts remain available for inspection. Failed breakpoints are reported as unverified with diagnostics, with no VM fallback.

The Check Debug Environment command probes the selected compiler and GDB. `chali.debuggerPath` sets the default GDB; the launch configuration overrides it. Windows GDB may encode launch arguments through a system code page. Current ChaLi debug artifacts also read length-prefixed UTF-8 arguments, preserving empty strings, Unicode and non-BMP characters. Release programs continue to obtain arguments from the native process entry.

Debug builds use `-O0` and retain native symbols and source locations, allowing variables and steps to remain observable. Release builds retain highest optimization, LTO and dead-code elimination. Debug timings are not release-performance measurements.

## Projects, existing artifacts and input

The project-entry configuration reads `main` from the selected workspace's `chali.toml`, retaining compiler-resolved dependencies, library locks and native link settings. Current-file configurations use `program`. Relative paths belong to the configuration's workspace; the default working directory is the project root or entry directory. Relevant documents are saved before building. Invalid, non-host and non-executable project configurations are rejected before launch.

Set `executable` to an existing symbol-bearing program and omit `program` to skip compilation, including on restart. `args` supplies arguments. `env` overrides environment variables; a `null` value removes an inherited entry. Build diagnostics go directly to Problems without a second compilation.

```json
{
  "type": "chali",
  "request": "launch",
  "name": "ChaLi project with input",
  "entry": "project",
  "console": "integratedTerminal",
  "env": { "APP_MODE": "development", "OLD_SETTING": null },
  "args": ["sample file.txt"]
}
```

The integrated VSCode pseudoterminal provides line input: Enter submits, Backspace edits, Ctrl+C requests pause, and Ctrl+D or Ctrl+Z ends input. A session-specific standard-stream channel keeps input separate from GDB/MI: a named pipe on Windows or local socket on Linux. Standard output and error share the terminal. The default `internalConsole` suits programs without interactive input.

Integrated input requires an artifact generated with the current compiler's `--debug`; arbitrary existing programs can use `internalConsole`. Closing the active terminal ends the session. Restart creates a fresh connection. Debug stream setup precedes user initialization and is absent from release entry points. This provides line input rather than a complete system TTY or full-screen terminal emulation.

## Session control

Build, configuration, running, stopped and ended are distinct states. Control requests execute in order. Stack frames, variables and expressions belong to the current stop; continuing invalidates their references, and late inspection responses report expiration instead of displaying stale values.

Debug builds and inspections can be cancelled. Clients supporting progress reporting can also cancel the build progress. Disconnect and terminate clean up the session compiler and GDB process trees. Source-mode restart rebuilds the program and reinstalls source breakpoints, including when the previous build is still active. Normal exit and repeated cleanup produce one termination event.

DAP headers are limited to 8 KiB, message bodies and individual MI records to 16 MiB. DAP and MI each allow at most 256 pending requests. Malformed lengths, truncated envelopes and invalid JSON stop dispatch. MI requests default to a 20-second timeout; late replies cannot satisfy newer requests. Cancelling submitted execution does not undo program actions; an uncertain execution state ends the session.

Source breakpoints support conditions, hit counts and log text. Conditions are evaluated as native GDB expressions. Hit counts accept a positive integer and stop from the N-th hit onwards. Log breakpoints interpolate `{expression}` fragments, write the text as output and continue automatically without reporting a stop. Function breakpoints are registered by source name and translated to the actual linkage name through the debug catalog. Pending breakpoints update through DAP `breakpoint` events once GDB resolves them.

Stepping into internal standard modules automatically finishes the current internal frame and continues until user source is reached; user functions still stop normally, and repeated filtering is bounded with a notice. Panics, assertions and checked memory errors stop at the failing call in debug builds, and the adapter reports an exception stop with the original message and the nearest user source location. `Result::Err` is an ordinary value and does not produce an exception stop. Native signals are reported as exception stops with the nearest call frame that has a source line. Without debug information, the signal description remains available.

Exception breakpoints provide a `runtime` filter, enabled by default, for panics, assertions and checked memory errors. Disabling it lets the program follow its original failure exit path; enabling it restores the stop, and restart preserves the selection. Native signals are independent of this filter. Filter conditions and exception-path options are unsupported and rejected.

List/Map/Set display their length and paginated elements using static layouts; each Map entry expands into key/value fields. Scalars, strings, ordinary records and nested containers are supported. Read-only snapshots are copied during normal execution before source stops. Inspection calls no user methods, creates no language objects and does not resume execution. Container handles and snapshot indices are never exposed as memory addresses.

Each thread's container snapshot allows at most 4096 collections, 32768 value nodes, 32 aggregate levels and 2 MiB of tables and text, within the overall memory-read budget. Released values, unsynchronized element borrows, publication races and exhausted budgets report unavailable data. Continuing invalidates old references. Debug builds route numeric List access through host revision checks; release builds retain the fast path. Interface dynamic types and closure captures are described below; complete reference states remain pending. Run the protocol, real GDB and lifecycle checks with:

```sh
python tools/run_module_gates.py debug --chali PATH --random-recheck 1
```

## Source maps carried by the artifact

Debug artifacts contain a versioned read-only catalog and a basic-block address table with function instance names, parameter declarations, lexical ranges and source positions. The adapter selects positions using the actual machine PC. Existing artifacts need no temporary source-side map files. Generic names retain complete arguments, such as `identity<int64>` and `Holder<int64>`, while link symbols retain their existing convention.

Loading uses internal symbol and memory queries without calling target functions. The catalog budget is 4 MiB with at most 65,536 positions; each memory request reads at most 64 KiB. Version, pointer width, byte order, contiguous addresses and references are validated. Continuing cancels unfinished reads. Queries share loading, while cancellation of one waiter does not cancel other waiters. Missing or damaged catalogs report the reason and fall back to DWARF; fallback columns are not precise positions.

Address-taken locals and value parameters may use separate logical storage. Debug builds refresh their frame display values through existing memory primitives before source stops, preserving the view after assignment, mutable macro borrowing and pool growth. Refreshing creates no language objects and calls no user methods. Release builds contain no refresh instructions. Owning values, containers and arbitrary native interrupt locations still require their actual logical layout; frame handles are not machine pointers.

## Command line

```text
chali aot app.chl --checked --debug --out build/app-debug
gdb build/app-debug.exe
```

Linux executables have no `.exe` suffix. Use `break app.chl:line`, `run`, `next`, `step`, `finish`, `backtrace`, `info locals` and `print expression`. `--debug` retains debug information; `--checked` independently controls array and pointer checks.

DWARF distinguishes signed and unsigned 8/16/32/64-bit integers, bool, char, float32 and float64 using their actual storage. It retains declaration lines, source columns and user lexical blocks. Inner bindings shadow outer names within their valid range, and loop bindings disappear after the loop. Generated cleanup blocks do not create user name scopes. Numeric frames currently store rounded float32 values in double slots: debugging reads the actual 64-bit representation while retaining the float32 name. Native debugger sizeof therefore does not replace the language std::lang::rawSize operation.

Pattern bindings retain their declaration locations, types and match-arm scopes; they disappear from locals after the arm exits. Debug artifacts also retain macro definitions, calls and nested expansion origins. During an expansion, VSCode provides separate `$MacroName` variable scopes. Private copies do not impersonate same-named caller variables and disappear after expansion. Caller declarations generated through ordinary name parameters retain their caller scope.

Plain records, arrays and references to these aggregates in any call frame can expose memory references valid for the current stop, for use with VSCode memory tools. Standard DAP `readMemory` requests resolve language logical addresses in static data or live heap allocations. Negative offsets can read an array length header within the same allocation. Out-of-range, released and expired stop references are rejected. Interface and host-object identifiers are not automatically exposed as addresses. Reads are also rejected if the native execution context has changed since publication.

Real C synchronous callbacks are verified with malloc and pool on the main thread and an isolated worker thread: callback records, arrays and caller heap values are read in each thread, with the main thread's null active context distinguished from the worker's independent context. Variable references expire after execution resumes.

One memory inspection shares a 4 MiB read budget across its directory and payload, with at most 65,536 live allocation ranges. Inspection calls no target functions or user methods, allocates no target objects and never resumes the target; raw memory remains available through memory references. Heap inspection reports an unavailable mapping if execution stops during publication, or after allocation/release before the next source checkpoint; continue to a source checkpoint and retry. Changes to another thread's heap can also conservatively invalidate an older directory.

Plain records, arrays, strings and references in the top source frame are shown through versioned logical layouts: records list fields in declaration order (nested records appear as flattened dotted slots, matching language storage), arrays show their length and paged elements, strings decode to quoted text, and references display the target value. Payload enums, Option and Result show the variant name and payload field names selected by the tag instead of internal `__tag`/`__vN_` slots, and report an unknown variant for invalid tags. Layout text uses the same format as runtime value_text and is deduplicated by the compiler. The adapter reads target memory according to the layout without calling target methods, running user code or allocating language objects. Children default to 100 entries per page and at most 1000, sharing the 4 MiB read budget; when a snapshot is unavailable the raw slot value is kept and the memory reference remains available.

The adapter supports the standard DAP `disassemble` request: it returns machine instructions and symbol names around the current frame by machine address, supports forward and backward windows as well as byte offsets, caps the count at 4096, and never resumes the target.

The compiler publishes a directory only after a block has executed source statements, so the stop at a function's first statement reuses the most recent publication from before the call. That directory remains readable when nothing was allocated or released since; otherwise inspection reports an unavailable mapping until the next statement. Publishing this way keeps the inserted call from becoming the first source stop after the function entry, so stepping into a function still stops at its first statement.

Owning aggregates and containers still appear as ChaLi logical handles, which must not be dereferenced as C machine pointers. Command-line GDB `print` uses native syntax; VSCode inspection uses the read-only ChaLi subset below.

## Read-only expressions

VSCode hovers, watches and debug-console inspection require a valid source frame at the current stop. Supported expressions include in-scope ChaLi names, ordinary record fields such as `line.start.first`, fields through aggregate references, nonnegative integer array/List indices, tuple indices, and integer, floating-point and bool literals. Operators are parentheses, unary `+ - !`, `+ - * / %`, `< <= > >= == !=`, and short-circuit `&& ||`. Examples include `row.first + values[1700]` and `large > 9007199254740993`. Name shadowing follows the frame's actual lexical scope. Continuing expires old frames and expansion references.

Parsing and arithmetic run in the extension host. Field and index reads reuse the stopped process's read-only layouts and memory snapshots. User input is never assembled into GDB evaluation commands; inspection calls no target functions or user methods and changes no target values. Aggregate results remain expandable. Flattened nested record layouts become child views retaining their original offsets, without allocating target records. Invisible names, out-of-range indices, unreadable snapshots and unsupported types produce explicit errors.

Integer arithmetic is exact, division truncates toward zero, and results range from the signed 64-bit minimum to the unsigned 64-bit maximum. Overflow, division by zero and non-finite floating-point results are rejected. Mixed integer/floating-point operations require integers with absolute values at most `9007199254740991`, avoiding implicit precision loss. Arithmetic does not simulate overflow of each narrow source integer type. Strings and char values can be inspected by name or field but do not participate in this arithmetic/comparison subset. Function/method calls, assignment, increment/decrement, casts, address/pointer operations and allocation expressions are unsupported. Syntax errors are rejected before target-value reads.

Each expression allows 4096 characters, 512 tokens and 64 levels of parser recursion. Field lookup reads at most 1000 children per operation; array and List indexing reads one requested element and still shares the memory budget. Variable modification is a separate pending feature. Breakpoint conditions and log interpolation retain the native GDB syntax described above.

Source debugging requires source files to remain at their recorded locations. The adapter uses the existing compiler and toolchain to generate machine code; it needs no VM installation and does not cast runtime function values into machine function pointers.

## Real VSCode extension tests

Run `npm run test:extension` from the repository's `vscode/` directory. This requires an installed VSCode, GDB, an executable ChaLi compiler and a graphical session. `CHALI_BIN` selects the stage compiler, defaulting to the bundled compiler. `CHALI_VSCODE_EXECUTABLE` selects the VSCode executable; Windows defaults to the Program Files installation, while other hosts resolve `code` on PATH. `CHALI_GDB` overrides GDB. Missing prerequisites cause a failure.

The entry isolates user configuration, installed extensions and the workspace beneath the repository's root `tmp/`. The real extension host checks activation, language identification, source builds and breakpoints, stack frames, scalar and array values, cancellation, panic messages and failure exit. Each scenario records its adapter, GDB and target processes and verifies that all exit. Logs and result JSON remain in the temporary directory for archival and cleanup according to repository rules. VSCode 1.132.0 has been exercised on Windows x64. Process-tree readers exist for other hosts; this does not establish execution evidence for those platforms.

`npm run test:extension -- --debug` pauses before extension tests for a JavaScript debugger on port 9333. The overall timeout is 240 seconds. Automated gates omit this option. Run the independent module gate from the repository root with `python tools/run_module_gates.py editor_host --chali COMPILER_PATH`. Release gates and VSIX packaging also run the real host tests.

## Concrete interface types and fields

Debug builds register layouts for dynamic types used by the current program. Version three of the read-only memory directory copies interface/Any box type numbers and stored values on the normal execution path before a source stop. It invokes no dynamic type queries, user formatters, copy methods or destructors. The adapter resolves verified type numbers to concrete record layouts and displays their actual types, nested fields and text. Owned interfaces, read-only/mutable interface borrows and interfaces inside containers share this read path. Fields support paging; continuing invalidates old expansion references.

Object numbers are not exported as raw memory addresses. Concrete record addresses are used only after checking their type and current live memory mapping. Empty boxes, missing dynamic types and unsupported nested boxes have explicit states rather than guessed types. Version three retains the object, value-node, recursion and text budgets of version two, and older container directories remain readable. Closure captures are described below; cycles and complete moved-value states are validated separately.

## Closure captures

Debug builds register source names, ordered capture layouts and capture modes for language function tags. Version four of the read-only directory copies existing closure tags and capture descriptors before source stops. Copy/move captures use their existing stable cells; borrowed captures retain their source addresses. Inspection invokes no closure, copy helper or user formatter, creates no language objects and materializes no cells.

Closures expand into named captures. Scalars display directly; records, strings, containers and references use the existing typed memory reader. Default read-only borrows carry a read-only hint, while mutable borrows and copy/move cells retain their own values. Cloned closures expose their independent state. Children support paging, and continuing or restarting invalidates old references. Functions without captures display their name. Empty values, unknown tags and capture/layout mismatches have explicit states. Tags identify language data and are never executed as machine function addresses. The per-thread limits remain 4096 objects, 32768 value nodes, 32 value levels and 2 MiB of snapshots, with older directory versions readable. Complete cycle and moved/released-value states are validated separately.

Memory publication skips only an empty initial function entry. Empty blocks created by variable-mirror refreshes or control-flow joins still republish allocations performed earlier, preventing stale heap directories after closure construction.
