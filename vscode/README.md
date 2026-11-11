# ChaLi for VS Code

**English** · [简体中文](README.cn.md)

Default `run` and `build` publish final outputs beside the `.chl` source, including executables produced by run. Intermediate files use a unique workspace under the source-adjacent `.chali` directory, hidden on Windows and removed when empty after success or failure. Existing user files are preserved. Explicit output/retention settings remain available. VS Code defaults to source-adjacent output; static VSIX packages omit `Chali › Build: Llvm Path`, while external packages require it and pass `--llvm` for both build and run.

The extension runs the compiler's native language server (`chali lsp`). The VSIX includes a compiler; Python is needed for repository build and verification tools, not for editor language services. The compiler and extension versions are 1.0.0.

## Editing

- Diagnostics come from the actual compiler. Warning levels support project configuration, CLI overrides and line suppression. Quick fixes insert suppression without removing the declaration or its initializer.
- Hover, completion, signature help and optional type inlays show declarations and available inferred types. Semantic tokens and macro-specific TextMate rules complement each other.
- Definition navigation opens project declarations or embedded standard-library source. Standard source documents are read-only. Intrinsics without a language declaration do not get a fabricated body.
- References and rename distinguish shadowed bindings across the current module graph, including unsaved open files. Ambiguous bindings, conflicts, macro-template uncertainty and standard-source edits are rejected. Rename checks document versions and source fingerprints before returning edits; it does not lock external file writers.
- Code actions can fill directly listed non-generic interface methods or add an incomplete match's fallback. The proposed bodies contain `TODO panic` placeholders. Each proposal passes an isolated compiler check; unsupported or still-invalid programs do not receive a proposal.
- Macro preview shows the actual compiler AST, hygienic bindings, source/definition/metadata/control-target locations and direct effects. Origins are navigable. Tabs, search, folding, zoom and standalone HTML export are available. Preview does not establish transitive purity.

## Build and debug

Use **ChaLi: Select Project Target**, **ChaLi: Select Build Output**, or **ChaLi: Open Project Configuration**. Changes remain undoable edits in the TOML buffer and preserve comments and multiline strings. Build saves the source and dirty manifest before reading them. The status bar shows the actual compiler variant, saved configuration and pending configuration edits.

Registered targets are Windows x64, Linux x64 and Linux ARM64. Output can be an executable, object file or LLVM IR. A cross-target object does not establish that its target runtime or executable deployment has been tested. See the [toolchain reference](../doc/en/09-toolchain.md).

Run executes checked native compilation in the integrated terminal. Native debugging uses DWARF and GDB/MI through the compiler's DAP adapter; a suitable GDB is required. Source builds use O0 by default; `debugMode: "optimized"` selects O3/LTO with DWARF and read-only optimized variables. See [native debugging](../doc/en/43-native-debugging.md).

Static and external LLVM VSIX files are separate. The static variant embeds LLVM/LLD and required link assets; the external variant needs a suitable LLVM toolchain for code generation. Extra application C ABI libraries follow their own link/deployment requirements. Neither variant needs Python for language services.

## Compiler selection and dependencies

Compiler selection checks `chali.path`, the embedded `bin/` compiler, supported workspace locations and PATH. The output channel reports the actual executable. Server restart is serialized and waits for process closure. Unsaved documents are replayed to the replacement server.

Dependency commands download and lock modules, verify them offline, or open the dependency cache. They run in trusted workspaces and select a project when needed. Downloaded modules and language-service caches are not part of the VSIX.

## Repository verification and packaging

Run from this directory:

```sh
npm ci
npm test
node tools/package.js --llvm-variant static --compiler ABSOLUTE_COMPILER_PATH --out dist/RELEASE
```

Use `external` for the external LLVM package. Packaging checks the manifest, real compiler version/variant, native LSP behavior, extension interactions, macro rendering and highlighting, cancellation and build outputs. Test dependencies and tool history are excluded from the package. Do not treat package checks as the repository's full regression suite.

The publisher is `rabbitc999`; the author is 小兔糖糖_OvO <rabbitc999@outlook.com>. The extension uses Apache-2.0; retain the root LICENSE, NOTICE and dependency notices.

Run `npm run test:extension` for real extension-host tests, or `npm run test:extension -- --debug` to debug the tests (port 9333, overall timeout 240 seconds). An installed VSCode, GDB and graphical session are required. Select tools with `CHALI_VSCODE_EXECUTABLE`, `CHALI_BIN` and `CHALI_GDB`. Tests cover breakpoint values, cancellation, panic and native process cleanup; see [native debugging](../doc/en/43-native-debugging.md).

Code block folding uses the native LSP, including nested braces and unfinished blocks. Macro directive folding remains available.
