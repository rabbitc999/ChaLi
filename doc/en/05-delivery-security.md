# 05 Delivery security

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../05-delivery-security.md)
<!-- END LANGUAGE NAVIGATION -->

Native executables are never encrypted or obfuscated: `--encrypt` and `[chali] encrypt` are unsupported, so deliverables always come from source through AOT.

## Delivery forms

| Artifact | Recipient needs | Typical use |
| --- | --- | --- |
| `.chl` source | ChaLi compiler | Source sharing and joint development |
| `aot` executable | Target platform; any dynamic library the program uses must sit next to it | Closed-source delivery and offline runs |
| `--shared --export` library + header | A C/C++ host project and linker | Embedding in an existing host |
| Quadruple triple (`emit`) | A toolchain able to assemble it | Inspecting the intermediate form, custom assembly |

## Export and distribute

Executable:

```sh
chali aot src/main.chl --out build/app
```

Library plus header:

```sh
chali aot src/lib.chl --shared --export add --out build/lib
```

The second command produces `build/lib.dll` (`.so` on Linux) and `build/lib.h`; the host calls `chali_init()`, then `f_add(...)`, then `chali_finish()`. On Windows the loader searches the executable's directory and `PATH`, so libraries referenced by `#abi` must be placed next to the artifact.

To inspect the intermediate form only, run `chali emit src/main.chl --out build/app`. It writes `.quad`, `.data.bin` and `.layout` without linking or executing.

## Boundaries

Executables carry no publisher authentication. Release provenance and access control belong to the deployment system, code signing or the package manager.

Default builds drop symbols and debug information (`--strip-all`). Pass `--debug` to keep function names and source locations; that mode compiles at `O0` and retains debug sections. String literals, type names and the [reflection table](../19-reflection.md) still appear in the artifact — if that matters, do not ship the source.

## Artifact management

| File | Kept by | Purpose |
| --- | --- | --- |
| `.chl` | Developer or collaborators | Source and rebuilds |
| Executable / library + header | Runtime recipient | Running directly or embedding |
| `.chali/` intermediates | Build machine | Removed after a default build; for troubleshooting only |
| Debug builds and build records | Developer | Reproduction and troubleshooting |

Rebuild from source when upgrading, and recheck target architecture, dynamic library dependencies and system services. Legacy project keys such as `chali.encrypt` now fail loudly instead of being ignored.

## Next

- [Toolchain](09-toolchain.md): commands and builds.
- [Build artifacts and intermediates](../15-artifacts.md): layout, the `.chali/` directory and cleanup rules.
- [Host integration and the C ABI](08-host-integration.md): calling conventions in both directions.
- [Panic traces](../18-panic-trace.md): diagnostics and location mapping.
