# 09 Toolchain

AOT accepts an explicit GCC command, executable path or installation directory:

```sh
magel aot app.mgl --gcc-toolchain g++ --out app.exe
magel aot app.mgl --gcc-toolchain "D:/GCC/bin/g++.exe" --out app.exe
magel aot app.mgl --gcc-toolchain "D:/GCC" --out app.exe
```

A directory may contain `bin/g++` and `bin/gcc`, or be the `bin` directory itself.
A `gcc` command selects its corresponding `g++`; `--cxx` and `--cc` can override
individual tools. `[magel] gcc_toolchain = "D:/GCC"` supplies a project default.
The toolchain must support the desktop AOT host. Bare-metal `none-eabi` targets
require the standalone MCU VM. Invalid toolchains fail without falling back.

`magel` is the single entry point: interpretation, bytecode export, AOT, project creation and tests all
live in one program.

```sh
magel file.mgl [program arguments...]
magel scenario [file.mgl] [config words...] [-- program arguments...]
```

> On Windows the binary is `magel.exe`. Under Git Bash / MSYS write `./magel`, which resolves to
> `magel.exe`; a bare `magel` goes through PATH lookup.

## AOT library output

Use `export` to select the public C interface. Library source can omit `main`:

```magel
export int32 add(int32 left, int32 right) { return left + right; }
```

```sh
# A Windows GCC target produces a DLL
magel aot math.mgl --out math.dll
# A Linux GCC target produces an ELF shared library
magel aot math.mgl --out math.so
# A relocatable object in the selected toolchain's target format
magel aot math.mgl --out math.o
```

The extension selects the output type automatically; `--shared` and `--object` select it explicitly.
Without an extension, the GCC target determines `.dll` / `.so` for shared output and `.o` for objects.
Conflicting options fail. `.dll` requires a Windows GCC target and `.so` a Linux GCC target;
renaming an artifact does not convert its platform ABI. Static `.lib` / `.a` output is unsupported.

All three forms emit a matching `.h` and require at least one `export`. The host calls `magel_create`,
invokes `magel_<name>`, then calls `magel_destroy`. A nonzero status is described by `magel_last_error`.
Shared libraries expose these C symbols to the system dynamic loader. An `.o` is a relocatable
object for a host linker, not a shared library loaded directly by the dynamic loader. The same GCC
toolchain can link it into a host program or shared library. Final linking needs the C++ runtime
and any system libraries used; Linux commonly needs `-pthread -ldl`.
When a Windows toolchain uses a dynamic C++ runtime, the loader must also find the referenced
`libstdc++`, `libgcc`, threading or other runtime DLLs. Deploy the actual dependencies or configure
the library search path for that toolchain.

Both `--make` and `--separate-modules` support library output. Module output combines the required
module and runtime objects. No host `main` is defined, and a source-level `main`, if present, is not
called during library initialization. See the [ABI example](../examples/abi_contracts/) and [library regression](../../tests/magel_aot_libraries.py).

---

## 1. Scenarios

| Scenario | Effect |
| --- | --- |
| `run` / `interpret` | interpret internally (default) |
| `check FILE [json]` | check only, optionally emitting JSON diagnostics |
| `emit` | produce `.quad` / `.data.bin` / `.layout` only, no execution |
| `svmb` | export `<prefix>.svmb` only, no execution |
| `aot FILE [out EXE] [opt LEVEL] [checked\|unchecked]` | export AOT (default `unchecked`) |
| `new DIR [name NAME]` | create an application project (the destination must not exist) |
| `gen` | generate a standalone compiler project (forwards to `magel-generator compiler`) |
| `clear out DIR` | clean registered intermediate artifacts (no recompilation) |
| `lang zh\|en\|ja\|auto` | choose the display language |
| `help` | show help |

Development scenarios: `test tests backend all`, `fmt check src`, `deps lock project . offline`,
`lsp`.

**When you pass a file directly, everything after it becomes program arguments for `main`**:

```sh
magel app.mgl Alice "two words"
```

When you need arguments, write the entry as `int64 main(List<String> arguments)`.

---

## 2. Common options

| Option | Effect |
| --- | --- |
| `--out prefix` | artifact filename prefix (default `build/magel_out`) |
| `--check` | check only; no execution, no artifacts |
| `--emit` | produce `.quad` / `.data.bin` / `.layout` only, no execution |
| `--svmb` | additionally write `<prefix>.svmb`, loadable directly by StackVM |
| `--debug` | include a debug section in `.svmb`; print a location trace when interpreting |
| `--dap` | run as a debug adapter (DAP over stdio) |
| `--compress-symbols` | write a nameless varint symbol table and a PC mapping file |
| `--no-compress-symbols` | override config and disable symbol compression |
| `--symbol-map FILE` | mapping path (default `<prefix>.symbol_map.json`) |
| `--reflection` / `--no-reflection` | require / disable annotations and reflection (enabled by default) |
| `--hard-realtime` | forbid reflection for the whole compilation unit |
| `--diagnostics=json` | check and emit stable JSON diagnostics |
| `--separate-modules` | AOT caches per-module native object files and links once |
| `--module-cache DIR` | directory for AOT module object files |
| `--memory-size bytes` | runtime heap capacity (default 512 MiB, minimum 64) |
| `--watchdog` / `--no-watchdog` | enable / disable the soft-restart watchdog (off by default) |
| `--max-restarts N` | how many soft restarts (default 3, implies `--watchdog`) |
| `--watchdog-watermark N` | memory watermark in bytes (implies `--watchdog`) |
| `--step-limit N` | cap on instruction count for this run |
| `--threads` | run on an isolated host thread |
| `--incremental` | reuse module preprocessing and full compile artifacts across runs |
| `--stats` | print runtime and incremental file-check statistics |
| `--raw` | skip preprocessing: feed the source to the parser as-is |
| `--deps` | only list files pulled in by `%include` (for build scripts) |
| `-I` / `--include-dir` / `--include-path dir` | append an include search directory |
| `--import-dir dir` | append an import search directory |
| `-D` / `--define sym[=value]` | define a preprocessor symbol |
| `--input "text"` | inputs for `input(variable);` (whitespace separated); reads stdin if omitted |
| `--` | everything after becomes program command-line arguments |

With no source file, input is read from stdin.

---

## 3. `magel.toml`

Put `magel.toml` in the project root; command-line arguments override the config.

```toml
[package]
name = "my-app"
version = "0.1.0"

[magel]
run = "interpret"
opt_level = 2
main = "src/main.mgl"
import_paths = ["."]
```

| Section | Key | Effect |
| --- | --- | --- |
| `[package]` | `name`, `version` | project metadata |
| `[magel]` | `run` | execution mode (`interpret`, etc.; also settable with `--mode`) |
| | `opt_level` | optimisation level |
| | `main` | project main file |
| | `import_paths` | import search directories |
| `[embedded]` | `enabled`, `restrict_modules`, `max_memory` | embedded mode — see [Cross-platform deployment](06-deployment.md#3-embedded-profile-and-standalone-vm) |

`[magel] main` names the project main file: **when no source file is given** it is the entry point
(i.e. you are building the project); if the command line names a file explicitly, that is a
single-file compile and `main` is ignored. `magel new` writes `main = "src/main.mgl"` by default.

Relative paths in the config resolve against the **config directory**; relative paths on the CLI
resolve against the **launch directory**.

---

## 4. Error reporting

Compile errors give a line and column and draw a squiggle sized to the offending token:

```text
語法錯誤：hello.mgl 第 2 行第 8 列：期望 标识符 / 常量(const) / 可变(mut) / 新建(new) 等 17 个之一，实际是 实现(impl)
   |
 2 | impl Foo { }
   |        ~~~~
```

In an English locale the same error reads:

```text
Syntax error: hello.mgl:2:8: expected one of identifier / constant(const) / mutable(mut) / new(new) (17 candidates), found impl(impl)
   |
 2 | impl Foo { }
   |        ~~~~
```

- **Internal token names are never shown**: `SEMI` → `分号(;)`, `IMPL` → `实现(impl)`,
  `VARIABLE` → `标识符`.
- If the error is inside a file pulled in by `%include`, it points at **the file that is actually
  wrong**, not at the expanded main file.
- JSON diagnostics (`--diagnostics=json` or `check FILE json`) carry the same line and column so
  editors can jump.

### Display language

Diagnostics follow the display language. One error, three renderings:

```text
期望 标识符 / 常量(const) / 可变(mut) / 新建(new) 等 17 个之一，实际是 实现(impl)
expected one of identifier / constant(const) / mutable(mut) / new(new) (17 candidates), found impl(impl)
識別子 / 定数(const) / 可変(mut) / 生成(new) 他 17 件のいずれかを期待しましたが、実際は 実装(impl) です
```

Selection order:

1. `magel lang zh|en|ja|auto`;
2. environment variable `MAGEL_LANG`;
3. system language (`LC_ALL` / `LC_MESSAGES` / `LANG`; on Windows `GetUserDefaultUILanguage()`);
4. none of the above → English.

> **Semantic errors (type mismatch, undefined symbol, …) are still Chinese** — those messages live in
> the compiler core, not in the presentation layer.

---

## 5. Debugging

The compiler itself is the debug adapter:

```sh
magel --dap app.mgl
```

With the VS Code extension from [`vscode/`](../../vscode/) installed, choose **Magel** in Run and
Debug and set breakpoints directly in `.mgl` (the extension launches `magel --dap <file>`). Supported:
breakpoints, continue, step in / over / out and the call stack; program output goes to the debug
console.

For a location trace on the command line, add `--debug`.

---

## 6. Projects, tests and formatting

```sh
magel new hello-magel        # create a project
magel test tests             # run tests
magel fmt check src          # format check
magel fmt write src          # format and write back
magel lsp                    # language service (for editor integration)
magel deps lock project . offline
```

`test` / `fmt` / `lsp` / `deps` are native C++ commands and do not require Python at runtime. Build scripts, documentation generation and repository tests still use Python. HTTP uses system WinHTTP on Windows and curl on other platforms; Git dependencies require Git.

---

## 7. Building the compiler itself

After cloning, generate the parser using ParseEngine and then build:

```sh
sh gen.sh          # needs the ParseEngine grammar generator
make               # build magel and libparser.a
sh check.sh        # run the normal and error samples
```

Run `sh gen.sh` for the initial build and after changing the source files. Generated parser files
are not committed:

```sh
sh gen.sh          # regenerate the front end from magel.gram (needs the ParseEngine generator)
```

| You changed | Is `sh gen.sh` needed? |
| --- | --- |
| `source/magel.gram` / `source/magel_token.gram` | yes |
| `source/magel_epilogue.cpp` / `source/epilogue/*.cpp` | yes (expanded into `magel_parser.cpp`) |
| `source/magel_driver.cpp` | yes (expanded into `magel_main.cpp`) |
| any other `.cpp` / `.hpp` | no, just `make` |

> Commit the grammar and semantic sources, not generated parser files. `magel_main.cpp` is a generated copy of
> `source/magel_driver.cpp`; plain `make` never regenerates it, so you get "changed it but nothing happened".

`gen.sh` needs a [ParseEngine](https://gitee.com/rabbitc999/ParseEngine) grammar generator. Put it in
the project root as `./ParseEngine[.exe]`, or leave it in the neighbouring ParseEngine repository and
`gen.sh` will find `../ParseEngine/build/bin/`.

### Building with annotations and reflection

```sh
make reflection     # same as make MAGEL_WITH_REFLECTION=1 STACKVM_WITH_REFLECTION=1
```

The root build includes front-end and runtime reflection and enables it for source compilation.
`make reflection` is an explicit enable target; use `--no-reflection` for the default MCU VM.
See [current capabilities](17-capabilities.md) for the paired macros and separate project defaults.

---

## 8. Next

| You want | Where |
| --- | --- |
| Harden the artifact | [Delivery security](05-delivery-security.md) |
| Deploy to a platform | [Cross-platform deployment](06-deployment.md) |

## Multiple diagnostics and independent borrow checking

The compiler continues at recoverable statement and function boundaries. Plain output lists each error;
`--diagnostics=json` emits one document containing the full `diagnostics` array, with original file and line mappings.
Identical messages at the same location are merged and the list is capped at 100 entries. Unrecoverable syntax or type-registration failures stop that phase.

The independent `source/magel_borrow_checker.hpp` module checks lifetimes after all functions and generic instances have been lowered.
Semantic analysis only records resolved borrow sources and storage operations; the checker receives facts without accessing symbols or generating code.
Escaping stores, returns of local sources, and by-value Any arguments hiding non-global references report `MGL1204`, alongside other semantic errors.
Any static diagnostic prevents execution and deliverable output. Regression entry points are `tests/magel_analysis_diagnostics.py` and `tests/magel_reference_escape.py`.

`source/magel_borrow_flow.hpp` uses branch joins and loop fixed points to report use after free or move as `MGL1205`.
See [borrow control flow and algorithm validation](11-algorithm-validation.md) for design limits and test commands.

Completion labels retain qualified names to show their origin. Functions, variables and types use short insertion text when unambiguous; names shared by modules or shadowed in the current scope retain qualification. Overloads from the same module share a short name.


### Naming warnings and scoped completion

The LSP reports PascalCase type naming violations as `MGL2101` and camelCase function/method naming violations as `MGL2102`. Both are warnings and do not block compilation. Constructors retain their type name; internal double-underscore protocol names are exempt. Renaming a declaration clears its warning.

After `import `, completion contains modules only. `import std::` omits the implicitly imported `lang` module. In code, `std::` lists direct submodules; `a::iter::` lists only that scope's direct submodules, functions and types. A type scope lists its direct methods and fields. Unrelated scopes are excluded.

Standard type and field hover/completion descriptions share the [symbol metadata](../library/symbol-index.json) and the description columns in the API reference. The pause operation belongs to `std::lang`: use `std::lang::pause()` or the implicitly imported `pause()`.

Typing `.` lists the receiver type's direct methods and fields. Receiver types come from explicit declarations and parameters, or from `auto` initializers that use constructors, variable assignments, fields, and declared function/method return types. Generic receivers substitute their type arguments into field and return types. Hovering `auto` or the variable name displays the inferred concrete type: `auto item = makeLeaf();` shows `Leaf item`. Member filtering preserves module qualification and local variable shadowing, so equally named types in different modules do not share candidates. Unknown initializers or ambiguous return types retain `auto`; the compiler performs full expression checking and overload resolution.
