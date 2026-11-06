# 26 Execution and deployment boundaries

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../26-execution-delivery.md)
<!-- END LANGUAGE NAVIGATION -->

Normal run and AOT compilation generate LLVM code directly from the semantically checked AST, bypassing quadruple generation and assembly.

| Task | Command or API | Result |
| --- | --- | --- |
| Run source | `chali run app.chl` | Checked AOT compile-and-run, returns the program's exit status |
| Check | `chali check app.chl` | Syntax, type, borrow and semantic diagnostics; nothing runs, nothing is written |
| Native export | `chali build app.chl` | Native executable next to `app.chl` (`.exe` on Windows) |
| LLVM IR | `chali build app.chl --emit-llvm --out build/app` | `build/app.ll`, generated directly from the AST |
| Host embedding | `#export` / `--shared` | Shared library plus a C header the host calls directly |

`run` keeps bounds and liveness checks, `build` defaults to unchecked; `--checked` / `--unchecked` switch either way. Reflection is a fixed part of the language and cannot be trimmed.

## Where intermediate files go

Without `--out`, compiler intermediates (`.ll`, the ABI header, LLVM compilation logs) land in a unique `.chali/build-ID/` directory next to the source file; standard input uses the current directory. Each build has its own subdirectory, and `.chali/` is marked hidden on Windows:

- After a run or export, intermediate files are removed. The standard core binary is embedded in the compiler; no standard-library disk cache is created;
- `--keep-artifacts` keeps every intermediate for comparison and debugging;
- An explicit `--out PREFIX` keeps everything at the path you gave and skips cleanup; `--incremental` also retains the default workspace;
- `--emit-llvm`, `--emit-obj` and `--emit-asm` only select the final output format and do not by themselves prevent workspace cleanup. Final outputs are published next to the source or at the explicit output prefix;
- `.chali/` is already excluded by the `.gitignore` that `chali new` writes.

The `build` executable defaults to the source directory and has no suffix on Linux and similar platforms. Preserve LLVM IR with an explicit `--out` prefix.

```powershell
chali run app.chl                     # cleans intermediates; no standard-library disk cache
chali build app.chl                     # writes ./app.exe
chali build app.chl --out build/app     # writes build/app.exe and build/app.ll, all kept
chali build app.chl --emit-llvm --out build/app                    # generates build/app.ll directly from the AST
```

The compiler embeds its standard library; external drivers, shared libraries and devices remain deployment dependencies. See [toolchain](09-toolchain.md), [deployment](06-deployment.md), [host integration](08-host-integration.md) and [capabilities](17-capabilities.md).
