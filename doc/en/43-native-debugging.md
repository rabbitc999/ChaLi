# Native AOT debugging

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../43-native-debugging.md)
<!-- END LANGUAGE NAVIGATION -->

On Windows, per-thread debug observation buffers use FLS-managed heap state. Thread exit retires the directory entry and releases its snapshot buffers; module unload releases the FLS slot and remaining states. This avoids accessing emulated TLS storage after older MinGW runtimes have freed it. Non-Windows builds retain their native TLS path.

`run` compiles and executes checked AOT code. `aot` defaults to unchecked native export. Native debugging uses machine code, DWARF source locations, and actual frame slots.

## VS Code

Install the ChaLi extension and a GDB matching the target platform. Open a `.chl` file and press F5. The extension builds a checked native debug program, then uses GDB/MI for source breakpoints, continue, step in/over/out, stack frames, parameter/local-variable inspection and expression evaluation.

On Linux editor hosts using Node.js 20.8 or later, session-specific abstract sockets avoid requiring socket files on the project mount. Older hosts retain filesystem endpoints, so the temporary directory filesystem must support Unix domain sockets. Windows retains separate named pipes. Both standard-stream channels listen before launch; restart and shutdown retire the previous session connections.

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

Debug builds default to `-O0` and retain native symbols and source locations. Use the O3 mode below to inspect an optimized program. Release builds retain highest optimization, LTO and dead-code elimination.

## Development and O3 debugging

`--debug` selects O0 development debugging. `--debug-optimized` retains DWARF/native symbols while using LLVM O3 and the selected LTO mode. Between the command-line options `--debug`, `--debug-optimized` and `--no-debug`, the last option wins. The project's `debug = true` retains its existing enable-debug behavior without downgrading an explicitly selected O3 mode to O0.

```sh
chali aot main.chl --debug --out build/development
chali aot main.chl --debug-optimized --out build/optimized
```

For VSCode source builds, use `"debugMode": "development"` (the default) or `"debugMode": "optimized"`. Launching or attaching to an existing executable uses its own debug catalog; configuration does not re-optimize an existing binary. Loading an O3 catalog reports its mode, and variable scopes are labelled O3.

O3 debugging does not keep functions alive through fixed basic-block addresses. LLVM can inline, propagate constants and remove dead code normally. The stack preserves the inline frames reported by GDB/DWARF: several source frames can correspond to the same machine location, and the adapter does not merge them by address. Deleted statements may have no executable breakpoint location, and stepping order can differ from source order.

Variables reported by GDB as `<optimized out>` are explicitly marked as having no readable value at the current location. The adapter neither invents a value nor invokes target functions to reconstruct it. Scalars with readable locations retain their actual values and read-only expressions. Optimized mode uses static layouts only for uniquely identified source names. When bindings with the same name cannot be distinguished, confirmed basic GDB values remain visible, but expressions or aggregate expansion requiring that binding are rejected. Unknown aggregate layouts report unavailable data rather than displaying slot or handle integers as aggregate contents. Optimized mode does not reconstruct development mode's PC-based lexical and private macro scopes; use O0 for those details.

All O3 debug variables are read-only. `setVariable` reports an explicit error because the original frame slot may have disappeared or been reused. Inspection retains stop-generation and read budgets and calls no user functions. Debug safety points and snapshot publication remain present, so timings from either O0 or O3 debugging are not measurements of a release program without debugging.

O0/O3 execution has been checked on Windows x64 with static LLVM and a matching external LLVM, using malloc and pool. Checks cover actual inline stacks and disassembly, unrecoverable optimized-out values, expressions over live variables and identical program results. Linux O3 execution has not been validated in this stage.

## Linked symbols and source paths

Debug information in existing native objects is available through DWARF when it survives the final link. Use a debug build and configure `sourceFileMap` for paths recorded on another machine. With no local source, the debugger can still show available symbols, instruction addresses and disassembly.

When an older executable publishes a ChaLi library catalog, the adapter checks that the symbol and implementation identities match before using it for typed inspection. If the identity check fails, DWARF and disassembly remain available.

## Projects, existing artifacts and input

The project-entry configuration reads `main` from the selected workspace's `chali.toml`, retaining compiler-resolved dependencies and native link settings. Current-file configurations use `program`. Relative paths belong to the configuration's workspace; the default working directory is the project root or entry directory. Relevant documents are saved before building. Invalid, non-host and non-executable project configurations are rejected before launch.

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

DWARF distinguishes signed and unsigned 8/16/32/64-bit integers, bool, char, float32 and float64 using their actual storage. It retains declaration lines, source columns and user lexical blocks. Inner bindings shadow outer names within their valid range, and loop bindings disappear after the loop. Generated cleanup blocks do not create user name scopes. Numeric frames currently store rounded float32 values in double slots: debugging reads the actual 64-bit representation while retaining the float32 name. Native debugger sizeof therefore does not replace the language std::core::rawSize operation.

Pattern bindings retain their declaration locations, types and match-arm scopes; they disappear from locals after the arm exits. Debug artifacts also retain macro definitions, calls and nested expansion origins. During an expansion, VSCode provides separate `$MacroName` variable scopes. Private copies do not impersonate same-named caller variables and disappear after expansion. Caller declarations generated through ordinary name parameters retain their caller scope.

Plain records, arrays and references to these aggregates in source frames with verified memory context identities can expose memory references valid for the current stop, for use with VSCode memory tools. Standard DAP `readMemory` requests resolve language logical addresses in static data or live heap allocations. Negative offsets can read an array length header within the same allocation. Out-of-range, released and expired stop references are rejected. Interface and host-object identifiers are not automatically exposed as addresses. Reads are also rejected if the native execution context has changed since publication. Version five also compares the heap, data and object-table identities captured on entry to the selected source frame. Frames from another native context or with unreadable identity slots cannot expose typed heap views. Older artifacts without frame identity descriptors can use the active directory only from the top source frame.

Real C synchronous callbacks are verified with malloc and pool on the main thread and an isolated worker thread: callback records, arrays and caller heap values are read in each thread, with the main thread's null active context distinguished from the worker's independent context. Variable references expire after execution resumes.

One memory inspection shares a 4 MiB read budget across its directory and payload, with at most 65,536 live allocation ranges. Inspection calls no target functions or user methods, allocates no target objects and never resumes the target; raw memory remains available through memory references. Heap inspection reports an unavailable mapping if execution stops during publication, or after allocation/release before the next source checkpoint; continue to a source checkpoint and retry. Changes to another thread's heap can also conservatively invalidate an older directory.

Plain records, arrays, strings and references in the top source frame are shown through versioned logical layouts: records list fields in declaration order (nested records expand by source fields using their actual storage offsets), arrays show their length and paged elements, strings decode to quoted text, and references display the target value. Payload enums, Option and Result show the variant name and payload field names selected by the tag instead of internal `__tag`/`__vN_` slots, and report an unknown variant for invalid tags. Layout text uses the same format as runtime value_text and is deduplicated by the compiler. The adapter reads target memory according to the layout without calling target methods, running user code or allocating language objects. Children default to 100 entries per page and at most 1000, sharing the 4 MiB read budget; unavailable snapshots report the reason and do not expose unverified expansion references.

The adapter supports the standard DAP `disassemble` request: it returns machine instructions and symbol names around the current frame by machine address, supports forward and backward windows as well as byte offsets, caps the count at 4096, and never resumes the target.

The compiler publishes a directory only after a block has executed source statements, so the stop at a function's first statement reuses the most recent publication from before the call. That directory remains readable when nothing was allocated or released since; otherwise inspection reports an unavailable mapping until the next statement. Publishing this way keeps the inserted call from becoming the first source stop after the function entry, so stepping into a function still stops at its first statement.

Owning aggregates and containers still appear as ChaLi logical handles, which must not be dereferenced as C machine pointers. Command-line GDB `print` uses native syntax; VSCode inspection uses the read-only ChaLi subset below.

## Read-only expressions

VSCode hovers, watches and debug-console inspection require a valid source frame at the current stop. Supported expressions include in-scope ChaLi names, ordinary record fields such as `line.start.first`, fields through aggregate references, nonnegative integer array/List indices, tuple indices, and integer, floating-point and bool literals. Operators are parentheses, unary `+ - !`, `+ - * / %`, `< <= > >= == !=`, and short-circuit `&& ||`. Examples include `row.first + values[1700]` and `large > 9007199254740993`. Name shadowing follows the frame's actual lexical scope. Continuing expires old frames and expansion references.

Parsing and arithmetic run in the extension host. Field and index reads reuse the stopped process's read-only layouts and memory snapshots. User input is never assembled into GDB evaluation commands; inspection calls no target functions or user methods and changes no target values. Aggregate results remain expandable. Nested records follow the source layout. Flattened layouts in older artifacts remain child views retaining their original offsets, without allocating target records. Primitive references participate in arithmetic and comparisons through their referenced value. Invisible names, out-of-range indices, unreadable snapshots and unsupported types produce explicit errors.

Integer arithmetic is exact, division truncates toward zero, and results range from the signed 64-bit minimum to the unsigned 64-bit maximum. Overflow, division by zero and non-finite floating-point results are rejected. Mixed integer/floating-point operations require integers with absolute values at most `9007199254740991`, avoiding implicit precision loss. Arithmetic does not simulate overflow of each narrow source integer type. Strings and char values can be inspected by name or field but do not participate in this arithmetic/comparison subset. Function/method calls, assignment, increment/decrement, casts, address/pointer operations and allocation expressions are unsupported. Syntax errors are rejected before target-value reads.

Each expression allows 4096 characters, 512 tokens and 64 levels of parser recursion. Field lookup reads at most 1000 children per operation; array and List indexing reads one requested element and still shares the memory budget. Basic variable modification is described below. Breakpoint conditions and log interpolation retain the native GDB syntax described above.

Source debugging requires source files to remain at their recorded locations. The adapter uses the existing compiler and toolchain to generate machine code; it needs no VM installation and does not cast runtime function values into machine function pointers.

## Basic variable modification

VSCode's local-variable and parameter view supports Set Value for mutable basic bindings in the current scope: `int8/16/32/64`, `uint8/16/32/64`, `float32/64`, `bool` and single-byte `char`. Integers are range-checked against the destination type, bool rejects numeric substitutes, and float32 rounds to the source type before writing its actual double frame slot. Use a single-quoted character or JSON string for char, such as `'Z'` or `'\n'`. Other values accept literals and basic operations on literals, without names, function calls or native GDB expressions.

The compiler separately records the physical frame-slot type in the debug catalog. A write checks the current lexical scope, mutability, the GDB variable type and address availability. Narrow integers update the entire eight-byte slot with the appropriate sign extension, preventing a changed display value from hiding stale high bits used by execution. Modification neither resumes execution nor calls target functions. Late reads and old aggregate/memory references expire after modification; source frames and local scopes remain valid for fresh reads. Clients supporting standard DAP `invalidated` events receive a variable refresh notification. Cancelling a submitted write does not guarantee rollback.

After a basic binding is address-taken, its original frame slot is only a display mirror. New compiler artifacts record a private DWARF storage address and actual width. In the top development source frame, inspection reads the real binding. Modification checks the current thread, stop state, native context, live region and mapping revision before issuing one write to that storage. Inspection executes no user methods and does not refresh or overwrite the stale mirror slot. Integers/floats use their complete eight-byte storage; uint8/char writes use one byte. Fresh inspection in the same stop and resumed execution both observe the real updated value.

Mirrors in non-top frames remain read-only, avoiding use of a caller's logical address in another native context. Immutable bindings, reference variables, aggregates, containers and aggregate children remain read-only. Owning-value assignment is never simulated by overwriting a handle or bypassing ownership and cleanup. Older artifacts lacking physical-slot or mirror-storage metadata remain inspectable but reject the corresponding modifications. Restricted modification of ordinary local owning strings is described below.

## Restricted modification of owning strings

The top development source frame supports editing an ordinary mutable local `String` only when the compiler explicitly records its independent ownership. Enter a JSON string literal, for example replacing `"hello"` with `"world"`. The old and new contents must have the **same UTF-8 byte count**, up to 4096 bytes. For example, `"中文"` and `"世界"` each occupy six bytes. An empty string accepts only an empty string. Input evaluates no names or calls and rejects unpaired Unicode surrogates.

Before writing, the adapter checks source scope, ownership, mutability, the real binding, frame context, move state and complete live allocations, then validates the descriptor, capacity, inline or separate buffer, and terminator. One write updates only the content payload. It preserves the owning handle, descriptor, length, capacity and terminator, calls no target functions, allocates or releases no language objects, and does not resume execution. Existing borrows retain their buffer address, while independently copied strings and container values retain their own contents. Old aggregate and memory references expire after editing and must be read again. Cancelling a submitted write does not guarantee rollback.

Optimized frames, non-top frames, String parameters, pattern bindings, borrows and older artifacts without explicit ownership metadata do not support this operation. Ambiguous bindings, static data, empty or moved handles, stale directories, corrupt descriptors, unsupported buffer layouts, excessive budgets and different byte counts produce explicit errors. Editing other owning types, changing string length, modifying aggregate children and replacing owning handles remain unsupported.

## All-stop threads

The thread view receives standard DAP thread-started/exited events and lists live threads. Step in, over and out use the thread selected by the client, rather than GDB's previous selection. Debugging uses all-stop mode: every thread stops when a stop is reached. Other threads may run while the selected thread steps. Exclusive single-thread execution is unavailable and requests with `singleThread: true` are rejected.

Thread exit invalidates existing frames, aggregate/memory references and pending old reads, and notifies the client to refresh stacks and variables. A late thread-list response cannot resurrect an exited thread. Inspection and stepping reject exited IDs. If the original stopped thread exits, the adapter rechecks and selects a live thread. Exception inspection uses the actual stop generation: another thread's exit does not swallow an exception on a live thread, while resumed or closed sessions receive no stale stop. Limits are 4096 live threads, 65536 exited-thread history entries and 4096 characters per name.

Internal native frames are filtered back to source, including Windows GDB finish notifications lacking a stop reason, subject to the existing filter limit. Explicit pause cancels automatic stepping filters; execution resumes only on a subsequent user command. Windows native pause uses system Windows PowerShell to request a system debug break. It verifies the target PID, an ancestor chain of at most eight processes, creation times and the actual image, retaining process handles through the operation. Helper processes inherit the same temporary directory and are cleaned up at session end. This covers the local `gdb.exe` launcher spawning `gdborig.exe`. The Linux path uses MI interruption. When a Linux system library has no function bounds, stepping falls back to the caller only after verifying that the selected thread has no source. Thread-specific execution checks have run on Windows x64 and Linux x64. Local attach and Linux gdbserver connections are described below.

## Attach, detach and source mapping

Use `attach`, an explicit `processId`, and a symbol-bearing `executable` whose bytes match the running image. Attach does not build sources or change target arguments, environment or standard streams. `env` configures the debugger only. The target remains stopped after breakpoint configuration by default; `stopOnAttach: false` resumes it.

```json
{
  "type": "chali",
  "request": "attach",
  "name": "ChaLi attach",
  "processId": 12345,
  "executable": "${workspaceFolder}/build/app-debug.exe",
  "sourceFileMap": {
    "C:/builder/project": "${workspaceFolder}"
  }
}
```

Local attach verifies process creation identity and the actual image SHA-256 before and after GDB takes control. Windows retains a process handle during identity queries; Linux retains the `/proc/PID` directory descriptor. PID reuse, process exit and image changes report errors. Each image is limited to 512 MiB for verification. Stack, variable and memory inspection remain unavailable until verification completes. Windows interruption of an attached target uses the verified creation time and actual image, without requiring a debugger ancestor relationship.

Attached sessions detach and resume the target by default. VSCode Stop Debugging also preserves the external target. DAP `disconnect` with `terminateDebuggee: true`, or an explicit `terminate` request, terminates it. Detach confirms an all-stop state, releases the GDB relationship, then exits the debugger. Preserving the target never forcibly kills the debugger process tree. A failed detach retains the session for retry rather than reporting success. Attach does not support restart or `suspendDebuggee: true`. Temporary source builds and integrated-terminal launches cannot preserve the target on disconnect. Ordinary launches of existing executables can explicitly request `terminateDebuggee: false`.

`sourceFileMap` maps absolute target source prefixes to local directories; relative local directories resolve against `cwd`. Matching uses the longest directory prefix with directory boundaries, and breakpoints use the reverse mapping. Windows target paths are case-insensitive; Linux target paths are case-sensitive. Spaces and Unicode paths work. At most 128 mappings are accepted. Duplicate local roots and reverse paths overridden by a more specific forward rule report errors to avoid setting breakpoints in the wrong source. The debug catalog retains original source identities; stack frames, macro origins and error locations display local paths.

For remote debugging add `"remote": { "host": "127.0.0.1", "port": 2345 }` to the attach configuration. `processId` must match the actual process controlled by that Linux gdbserver, and `executable` is a local copy of the same ELF. Only TCP endpoints are accepted. The adapter connects to an existing server; it does not run remote commands or deploy programs. Targets must be little-endian 64-bit Linux x64 or ARM64, and GDB must include the corresponding Linux target support. An ordinary Windows MinGW GDB may lack the Linux OS ABI. After connecting, the adapter checks the PID reported by GDB and fetches `/proc/PID/exe` through gdbserver file transfer to verify the complete SHA-256. Missing transfer support or mismatched identity reports an error and detaches. Remote pause uses MI interruption, and panic messages use the ELF target ABI rather than the Windows host argument register.

Run the remote gate with `python tools/run_module_gates.py debug_remote --chali /path/to/linux/compiler --jobs 1`. It requires a real Linux host, matching GDB, gdbserver and an LLVM toolchain. `CHALI_GDB` and `CHALI_GDBSERVER` can select tools. Missing prerequisites fail the gate; skipped execution or a simulated protocol does not count as passing. Accepting an architecture's image format does not establish execution validation for that architecture.

Remote execution has been exercised on Ubuntu 22.04 x64 with GDB/gdbserver 12.1, using both malloc and pool. Local attach, source mapping and thread-specific tests have also run in that Linux environment. ARM64 target execution remains unverified. A WSL workspace mounted from NTFS cannot create the Unix sockets used by the integrated terminal, so the complete debug gate fails at that terminal scenario in this environment. Passing the remote gate does not establish a passing complete Linux debug module or Linux graphical extension-host suite.

## Real VSCode extension tests

Run `npm run test:extension` from the repository's `vscode/` directory. This requires an installed VSCode, GDB, an executable ChaLi compiler and a graphical session. `CHALI_BIN` selects the stage compiler, defaulting to the bundled compiler. `CHALI_VSCODE_EXECUTABLE` selects the VSCode executable; Windows defaults to the Program Files installation, while other hosts resolve `code` on PATH. `CHALI_GDB` overrides GDB. Missing prerequisites cause a failure.

The entry isolates user configuration, installed extensions and the workspace beneath the repository's root `tmp/`. The real extension host checks activation, language identification, source builds and breakpoints, stack frames, scalar and array values, basic variable changes, cancellation, panic messages and failure exit, plus attachment to an external target and Stop Debugging while it runs. Each scenario records its adapter and complete debugger descendant tree and verifies their exit. The attached target is checked separately for survival, then cleaned up by the test caller. Logs and result JSON remain in the temporary directory for archival and cleanup according to repository rules. VSCode 1.132.0 has been exercised on Windows x64. Process-tree readers exist for other hosts; this does not establish execution evidence for those platforms.

`npm run test:extension -- --debug` pauses before extension tests for a JavaScript debugger on port 9333. The overall timeout is 240 seconds. Automated gates omit this option. Run the independent module gate from the repository root with `python tools/run_module_gates.py editor_host --chali COMPILER_PATH`. Release gates and VSIX packaging also run the real host tests.

## Concrete interface types and fields

Debug builds register layouts for dynamic types used by the current program. Version three of the read-only memory directory copies interface/Any box type numbers and stored values on the normal execution path before a source stop. It invokes no dynamic type queries, user formatters, copy methods or destructors. The adapter resolves verified type numbers to concrete record layouts and displays their actual types, nested fields and text. Owned interfaces, read-only/mutable interface borrows and interfaces inside containers share this read path. Fields support paging; continuing invalidates old expansion references.

Object numbers are not exported as raw memory addresses. Concrete record addresses are used only after checking their type and current live memory mapping. Empty boxes, missing dynamic types and unsupported nested boxes have explicit states rather than guessed types. Version three retains the object, value-node, recursion and text budgets of version two, and older container directories remain readable. Closure captures are described below; cycles and complete moved-value states are validated separately.

## Closure captures

Debug builds register source names, ordered capture layouts and capture modes for language function tags. Version four of the read-only directory copies existing closure tags and capture descriptors before source stops. Copy/move captures use their existing stable cells; borrowed captures retain their source addresses. Inspection invokes no closure, copy helper or user formatter, creates no language objects and materializes no cells.

Closures expand into named captures. Scalars display directly; records, strings, containers and references use the existing typed memory reader. Default read-only borrows carry a read-only hint, while mutable borrows and copy/move cells retain their own values. Cloned closures expose their independent state. Children support paging, and continuing or restarting invalidates old references. Functions without captures display their name. Empty values, unknown tags and capture/layout mismatches have explicit states. Tags identify language data and are never executed as machine function addresses. The per-thread limits remain 4096 objects, 32768 value nodes, 32 value levels and 2 MiB of snapshots, with older directory versions readable. Version five adds reference and storage states as described below.

Memory publication skips only an empty initial function entry. Empty blocks created by variable-mirror refreshes or control-flow joins still republish allocations performed earlier, preventing stale heap directories after closure construction.

## References, cycles and storage states

Version five retains runtime move/destruction markers for currently live storage. A separate observation revision covers objects and storage. Moves, reinitialization and object changes invalidate the previous publication; typed reads check this revision before and after decoding without changing language borrow-view validity. Typed inspection first checks the complete live allocation range. Marked values report their state and runtime type identifier; invalid addresses report an invalid, released or non-live reference without inferring a precise history that cannot be verified. References cleared by the runtime display `null`. Primitive references display the actual referenced value. Expansion tracks the current ancestor path, marks cycles and stops without treating every shared child as a cycle. Recursive targets use only unambiguous named record layouts in the directory, and expansion stops at 32 levels or the observation budget. Raw memory views do not interpret move markers but still require a currently live allocation. Incomplete object snapshots are distinguished from missing handles in complete snapshots; unsupported live host objects retain an opaque state.

Primitive values, strings and other owning handles track source storage in a separate debug move table. Zeros written during a move do not become readable `0`, empty strings or empty containers. Fields and indices are checked against their actual storage ranges. Genuine source writes clear covered debug markers; mirror refreshes do not revive moved values. Moving a reference binding into a closure uses a private availability slot and is distinguished from moving the referenced target. Reading a real binding requires a unique compiler-described private address slot in the selected frame, together with frame identity, live-range and observation-revision checks. Missing, duplicate or optimized observation slots report an unreadable value instead of falling back to another same-name binding. Primitive assignment uses the same availability checks. Observation events run only on the normal execution path of debug artifacts and are ignored by quadruple export.

Entry identity capture uses an internal synthetic location. VSCode maps entry frames back to their source declarations. When one source breakpoint contains both an internal entry and actual source locations, the adapter uses function-relative conditions that follow image relocation, pausing and counting hits only at actual source locations. Private counters belong to GDB state and do not write target data. With GDB directly, prefer statement lines inside the function body; when a declaration and its entire body share one line, check the multiple locations and files reported by GDB.
