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

Conditional/log breakpoints, language-container views and runtime-error filters are not yet provided. The debugger advertises only implemented capabilities. Run the protocol, real GDB and lifecycle checks with:

```sh
python tools/run_module_gates.py debug --chali PATH --random-recheck 1
```

## Command line

```text
chali aot app.chl --checked --debug --out build/app-debug
gdb build/app-debug.exe
```

Linux executables have no `.exe` suffix. Use `break app.chl:line`, `run`, `next`, `step`, `finish`, `backtrace`, `info locals` and `print expression`. `--debug` retains debug information; `--checked` independently controls array and pointer checks.

DWARF distinguishes signed and unsigned 8/16/32/64-bit integers, bool, char, float32 and float64 using their actual storage. It retains declaration lines, source columns and user lexical blocks. Inner bindings shadow outer names within their valid range, and loop bindings disappear after the loop. Generated cleanup blocks do not create user name scopes. Numeric frames currently store rounded float32 values in double slots: debugging reads the actual 64-bit representation while retaining the float32 name. Native debugger sizeof therefore does not replace the language std::lang::rawSize operation.

Owning aggregates and containers still appear as ChaLi logical handles, which must not be dereferenced as C machine pointers. Expressions use syntax accepted by GDB.

Source debugging requires source files to remain at their recorded locations. The adapter uses the existing compiler and toolchain to generate machine code; it needs no VM installation and does not cast runtime function values into machine function pointers.
