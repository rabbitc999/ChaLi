# 06 Native deployment

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../06-deployment.md)
<!-- END LANGUAGE NAVIGATION -->

ChaLi lowers the checked AST to LLVM IR and uses LLVM build to produce native machine code for the target platform. Deploy the build output together with the system libraries it needs.

## Run from source

```sh
chali run app.chl
```

`run` uses checked native compilation: compile with `default<O3>` and execute the resulting native program in a child process, with runtime checks (bounds, null, type) still enabled. It is for development and acceptance, not a delivery form.

## Ship an executable

```sh
chali build app.chl --out build/app
```

`build` is unchecked native compilation: same `default<O3>`, runtime checks removed, and the output is a self-contained executable (`.exe` on Windows). Copy it to the target machine together with any dynamic libraries and data files the program uses. `--debug` switches to `default<O0>,globaldce` and keeps debug information for troubleshooting. `--debug-optimized` retains DWARF while keeping O3; see [native debugging](43-native-debugging.md).

Startup no longer needs a host to register glue, reserve the data area or set an instruction budget — those were runtime-era interfaces. To call an external C function, declare it with `#abi("C", "library path", "symbol")`; the path resolves relative to the declaring source file, and the library must sit next to the artifact.

## Embed in an existing host

To be called from C/C++, export a library plus header:

```sh
chali build lib.chl --shared --export add --out build/lib
```

The host calls `chali_init()`, then `f_add(...)`, then `chali_finish()`; exceptions surface as `panic` crossing the C frame. See [host integration and the C ABI](08-host-integration.md).

## Intermediates only

```sh
chali build app.chl --emit-llvm --out build/app
```

Writes `.quad`, `.data.bin` and `.layout` without linking or executing — useful for inspecting quadruples or assembling them yourself. `.quad` is not machine code.

## Resources and boundaries

The compiler embeds its standard library; operating-system interfaces, dynamic libraries, networks, databases and peripherals remain provided by the target environment. Default run/build builds publish final files beside the source and clean their hidden `.chali/` workspace on success or failure. Explicit `--keep-artifacts`, `--incremental` or `--out` retains the corresponding intermediate content; selecting LLVM IR, object or assembly output alone does not retain the workspace. See [build artifacts and intermediates](../15-artifacts.md).

Embedded Linux uses the same back end, but artifacts must be rebuilt for the target architecture: LLVM triple, C++17 runtime and system services all need checking. "Runs on embedded Linux" is not "build once, deliver anywhere".

Upgrades replace the executable as a whole; there is no bytecode version compatibility layer, and running stacks, closures and globals do not migrate across versions.

See [host integration](08-host-integration.md), [toolchain](09-toolchain.md) and [artifact layout](../15-artifacts.md).
