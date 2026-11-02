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

Typed display for List/Map/Set containers, interface dynamic types and closure captures is not yet available, and neither are runtime-error filters or internal-code step filtering. The debugger advertises only implemented capabilities. Run the protocol, real GDB and lifecycle checks with:

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

Plain records, arrays and references to these aggregates in the top source frame can expose memory references valid for the current stop, for use with VSCode memory tools. Standard DAP `readMemory` requests resolve language logical addresses in static data or live heap allocations. Negative offsets can read an array length header within the same allocation. Out-of-range, released and expired stop references are rejected. Interface and host-object identifiers are not automatically exposed as addresses. Reads are also rejected if the native execution context has changed since publication.

One memory inspection shares a 4 MiB read budget across its directory and payload, with at most 65,536 live allocation ranges. Inspection calls no target functions or user methods and allocates no target objects. Heap inspection reports an unavailable mapping if execution stops during publication, or after allocation/release before the next source checkpoint; continue to a source checkpoint and retry. Changes to another thread's heap can also conservatively invalidate an older directory.

Plain records, arrays, strings and references in the top source frame are shown through versioned logical layouts: records list fields in declaration order (nested records appear as flattened dotted slots, matching language storage), arrays show their length and paged elements, strings decode to quoted text, and references display the target value. Payload enums, Option and Result show the variant name and payload field names selected by the tag instead of internal `__tag`/`__vN_` slots, and report an unknown variant for invalid tags. Layout text uses the same format as runtime value_text and is deduplicated by the compiler. The adapter reads target memory according to the layout without calling target methods, running user code or allocating language objects. Children default to 100 entries per page and at most 1000, sharing the 4 MiB read budget; when a snapshot is unavailable the raw slot value is kept and the memory reference remains available.

The compiler publishes a directory only after a block has executed source statements, so the stop at a function's first statement reuses the most recent publication from before the call. That directory remains readable when nothing was allocated or released since; otherwise inspection reports an unavailable mapping until the next statement. Publishing this way keeps the inserted call from becoming the first source stop after the function entry, so stepping into a function still stops at its first statement.

Owning aggregates and containers still appear as ChaLi logical handles, which must not be dereferenced as C machine pointers. Expressions use syntax accepted by GDB.

Source debugging requires source files to remain at their recorded locations. The adapter uses the existing compiler and toolchain to generate machine code; it needs no VM installation and does not cast runtime function values into machine function pointers.
