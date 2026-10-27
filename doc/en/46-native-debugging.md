# Native AOT debugging

**English** · [简体中文](../46-native-debugging.md)

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

Debug builds use `-O0` and retain native symbols and source locations, allowing variables and steps to remain observable. Release builds retain highest optimization, LTO and dead-code elimination. Debug timings are not release-performance measurements.

## Command line

```text
chali aot app.chl --checked --debug --out build/app-debug
gdb build/app-debug.exe
```

Linux executables have no `.exe` suffix. Use `break app.chl:line`, `run`, `next`, `step`, `finish`, `backtrace`, `info locals` and `print expression`. `--debug` retains debug information; `--checked` independently controls array and pointer checks.

Native variable descriptions currently cover parameter and local scalar slots. Owning aggregates and containers still appear as ChaLi logical handles, which must not be dereferenced as C machine pointers. Expressions use syntax accepted by GDB.

Source debugging requires source files to remain at their recorded locations. The adapter uses the existing compiler and toolchain to generate machine code; it needs no VM installation and does not cast runtime function values into machine function pointers.
