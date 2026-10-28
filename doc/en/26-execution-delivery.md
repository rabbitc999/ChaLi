# 26 Execution and deployment boundaries

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../26-execution-delivery.md)
<!-- END LANGUAGE NAVIGATION -->

Normal run and AOT compilation generate LLVM code directly from the semantically checked AST, bypassing quadruple generation and assembly. The compiler carries no interpreter core and `.svmb` bytecode export has been removed.

| Task | Command or API | Result |
| --- | --- | --- |
| Run source | `chali run app.chl` | Checked AOT compile-and-run, returns the program's exit status |
| Check | `chali check app.chl` | Syntax, type, borrow and semantic diagnostics; nothing runs, nothing is written |
| Native export | `chali aot app.chl` | Native executable next to `app.chl` (`.exe` on Windows) |
| LLVM IR | `chali aot app.chl --emit-llvm --out build/app` | `build/app.ll`, generated directly from the AST |
| Host embedding | `#export` / `--shared` | Shared library plus a C header the host calls directly |

`run` keeps bounds and liveness checks, `aot` defaults to unchecked; `--checked` / `--unchecked` switch either way. Reflection is a fixed part of the language and cannot be trimmed.

## Where intermediate files go

Compiler intermediates (`.ll`, the ABI header, LLVM compilation logs) all land in a hidden `.chali/` directory in the current directory, marked hidden on Windows:

- After a run or export only the standard-library precompiled cache `.chali/chali-stdlib-cache/` remains; everything else is deleted;
- `--keep-artifacts` keeps every intermediate for comparison and debugging;
- An explicit `--out PREFIX` keeps everything at the path you gave and skips cleanup; `--incremental` and `--emit-llvm` also keep artifacts;
- `.chali/` is already excluded by the `.gitignore` that `chali new` writes.

The `aot` executable defaults to the source directory and has no suffix on Linux and similar platforms. Preserve LLVM IR with an explicit `--out` prefix. The `emit` mode has been removed.

```powershell
chali run app.chl                     # only the stdlib cache is left in .chali/
chali aot app.chl                     # writes ./app.exe
chali aot app.chl --out build/app     # writes build/app.exe and build/app.ll, all kept
chali aot app.chl --emit-llvm --out build/app                    # generates build/app.ll directly from the AST
```

The compiler embeds its standard library; external drivers, shared libraries and devices remain deployment dependencies. See [toolchain](09-toolchain.md), [deployment](06-deployment.md), [host integration](08-host-integration.md) and [capabilities](17-capabilities.md).
