# Library containers and asset inspection

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../44-library-packages.md)
<!-- END LANGUAGE NAVIGATION -->

A `.chlib` is a single-file container for a language interface, target implementations and dependency identities. `chali lib template` produces a canonical manifest, `pack` bundles existing files, and `inspect` validates the format and reports JSON. Ordinary `import` can load a `c-v1` package interface and its native object/archive. Format commands do not compile implementations or execute initialization; consumer compilation checks interface language semantics.

## Creating and inspecting a container

Prepare `public.chl` and an existing native archive. An interface may declare a C symbol supplied by the final link:

```chali
module example::math;
#abi("C", "@link", "add")
int64 add(int64 a, int64 b);
```

For Windows x64, with an existing `build` directory:

```sh
chali ar build/add.obj --out build/libmath.a
chali lib template --module example::math --package-version 1.2.3 --target windows-x64 --license Apache-2.0 --out build/math.manifest.json
chali lib pack --manifest build/math.manifest.json --entry "interface:interface/main.chl=public.chl" --entry "archive:native/x86_64-w64-windows-gnu/library.a=build/libmath.a" --out build/math.chlib
chali lib inspect build/math.chlib --module example::math --package-version 1.2.3 --target windows-x64
```

Without `--out`, `template` prints canonical single-line JSON. The manifest uses the canonical target triple; its implementation name matches the archive entry above. `pack` reads that canonical manifest. Field order, set ordering and JSON encoding are part of the format; trailing CR/LF is allowed in the manifest input file. Each asset argument is `KIND:NAME=FILE`, keeping its container identity separate from its actual input path. Repeated assets are serialized in name order.

`inspect` reports the whole-package SHA-256, manifest and each entry's name, kind, length and payload SHA-256. It does not print source or binary payloads. `--module` and `--package-version` check expected identity; `--target`/`--cpu` check native selection. Inspection without a target can examine cross-target packages. Successful inspection does not establish execution on that target.

## Building native libraries from language functions

`lib build` compiles checked functions directly through LLVM into an independent object and bundles an automatically generated public interface. A supported scalar example is:

```chali
module example::math;
inline int64 adjust(int64 value) {
    return value + 1;
}
int64 add(int64 a, int64 b) {
    return adjust(a) + b - 1;
}
```

The `inline` declaration keeps `adjust` module-local. The public interface contains only `add`'s signature; consumers do not recompile its body. Arrange package directories by qualified module name:

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --out modules/example/math.chlib
chali lib inspect modules/example/math.chlib
```

The output directory must exist and the entry's actual module must match `--module`. The package contains the public interface and target object, without implementation source or an independent runtime copy. The final application provides the shared runtime. Public symbols derive from module/signature/ABI identity; private functions remain hidden. `--target`/`--cpu` select the implementation target; cross-target generation still needs actual target validation.

The producer supports ordinary functions with void/bool/char, fixed-width signed/unsigned integer and float32/float64 signatures, plus the nonowning record and enum parameters/results described below. Bodies support scalar operations, locals, control flow, direct calls within the unit, recursion and direct C ABI calls to imported scalar libraries. LLVM emits C ABI width/floating conversions directly. Unsupported owning aggregates, generic definitions, owning values, globals, methods, variadic parameters, unsafe exports, host/address operations and independent debugging/build-time step budgets are explicitly rejected. They are not silently omitted or treated as scalars. Templates, initialization and the broader internal language ABI remain subsequent work.

Scalar default parameters preserve definition scope and call-time evaluation. For example:

```chali
inline int64 defaultValue() { return 7; }
int64 addDefault(int64 x, int64 y=defaultValue()) { return x+y; }
```

After import, `example::math::addDefault(3)` evaluates the default only for this omitted `y`; `addDefault(3,9)` does not evaluate it. The producer compiles the checked expression into a scalar entry. The public interface stores declarations and calls, without private bodies, and importing or building does not execute the expression. Multiple omitted parameters retain their evaluation order; private and imported calls resolve in definition scope. Scalar-unit restrictions still apply: defaults do not enable globals, owning values or other unsupported operations. Generated default entries are implementation declarations, not APIs intended for manual use.

Public interfaces also preserve plain record declarations. Fields may be basic scalars or nested plain records from the same module or verified precompiled dependencies, retaining names, order and complete module identity. For example, a provider may declare:

```chali
struct Point { int64 x; float64 y; };
struct Bounds { Point low; Point high; char marker; };
```

A consumer can declare `example::math::Bounds bounds;`, access `bounds.low.x` and make independent value copies. Interfaces retain original field structure; consumers reconstruct layout under the same language version instead of parsing flattened field names as source syntax. Implicit language methods are rebuilt by consumers. User methods, constructors, field initializers, owning fields, custom layouts and interface-inheritance contracts remain rejected. A package still needs at least one supported public function. Parameters may use scalars, plain record values or borrows. Results may also use the plain record relocation contract below; owning aggregate calls remain unavailable. Same-named records in different modules remain different types.

Public enums support empty, single-payload and multiple-payload variants. Payloads may use basic scalars or supported records/enums. For example:

```chali
enum Event {
    Empty,
    Count(int64 value),
    Pair(int64 left, float64 right),
    At(Point position)
};
```

Declarations preserve variant order, original payload names and complete type names. Consumers can construct values and use `match`: `Event::Empty` has no payload, while `Event::Pair(3,4.5)` has two. Records may contain enums from precompiled dependencies, whose declarations remain in the original module. Nonowning enum function entries are described below. Owning payloads, user methods and complete interface contracts remain unsupported.

Nonowning enums support value parameters/results and read-only/mutable borrows, including empty and multiple payloads, nested records and nested enums. Qualified construction and matching resolve the actual declaring module, so consumer types with the same name do not shadow fully qualified names:

```chali
Event echo(Event value) { return value; }
void replace(Event mut& value) { value = Event::Count(77); }
```

Value calls retain `chali-value-v1`/`ChaLiValue`; borrows retain `chali-borrow-v1`/`ChaLiBorrow`. These packages additionally require `plain-enum-value-v1`, so older compilers reject the unknown capability during package inspection. Calls pass ChaLi aggregate logical addresses with the language's tag/payload layout. Mutating a value copy leaves the caller unchanged; mutable borrows update the original. Results, recursion and cross-library forwarding share the final program's allocator and lifecycle type identities. Full module identity, field layout and variant order enter symbol hashes, preventing old consumer objects from linking new layouts or variant numbering under the previous symbols. Owning payloads, enum default arguments, reference results and physical C ABI `#export` entries are outside this contract.

Plain nonowning records can also enter compiled language units as read-only or mutable borrows:

```chali
int64 read(Point const& value) { return value.x; }
void bump(Point mut& value, int64 delta) { value.x = value.x + delta; }
```

These entries pass the caller's ChaLi logical address directly and access fields through the final program's shared memory/checking context. They do not convert to host C structs or generate C++ argument marshaling. Plain record results use the relocation contract below; bodies retain the supported scalar control flow, field access and direct calls. Owning values, aggregate-reference defaults, reference results and library globals are not enabled. Language checks preserve read-only/mutable rules, and scalar parameters may accompany record borrows. A borrowed-record function cannot also use a physical-C-layout `#export`.

Packages use native ABI `chali-borrow-v1` and required feature `borrowed-record-v1`. The generated `ChaLiBorrow` marker is allowed only in verified library sources with matching ABI; copying it into ordinary source is rejected. Symbols include full record and nested-layout hashes, so an old independent consumer object cannot link to a package with a changed layout under the old symbol. Ordinary C ABI packages retain `c-v1`. Borrowed calls and the complete owning-value, closure and dynamic-function contracts remain separately verified capabilities.

Abstract interface declarations support scalar parameters/results (including void results), static protocol methods and inheritance. Bases must come from the same module or verified precompiled dependencies. Generated interfaces preserve original inheritance, adding imports for foreign bases so a consumer's same-named declaration cannot change resolution. For example:

```chali
interface Error { int64 code(); };
interface DetailedError : Error { bool retry(); };
```

Consumers may implement these interfaces, call through a base interface and use interface type matches such as `IoError(error)`. Declarations preserve original parameter names and static markers; inherited methods retain their original declaring interface. Default method bodies, associated types, generic definitions and nonscalar method signatures remain rejected by the producer. Creating interface values inside library code or passing them through native entries still requires the shared-runtime work.

Nested records from a verified `.chlib` dependency retain their original module names. A consuming library records exact dependency pins and references the original declaration instead of copying it into its own module. Transitive consumers load the declaring package even when they import only the outer library. Cross-module record fields originating only from implementation source are rejected; precompile that dependency first.

Imported packages are recorded with exact versions and content hashes; final consumers restore the complete dependency closure. Source `#export("native_name")` preserves an additional native entry, while ordinary language imports retain their module-and-signature-derived entries. Private `inline` functions remain absent from the public language interface. An object entry does not automatically become a final shared-library export; configure final linker exports separately.

`lib build` also accepts delivery metadata directly:

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --out modules/example/math.chlib --license Apache-2.0 --license-asset legal/LICENSE.txt=LICENSE --license-asset legal/NOTICE.txt=NOTICE --runtime-library example-plugin.dll
```

Repeat `--license-asset` with a package name on the left of `=` and an input file on the right. License and attribution bytes are preserved. Names, duplicates, size budgets and regular files are checked before code compilation; output cannot overwrite these inputs. Packaging uses the same read snapshots and failures preserve the previous package. `--system-library NAME` declares a final link input added by consumers; use names actually supplied by the target SDK, such as `kernel32` on Windows or `m` on applicable Linux targets. `--runtime-library` records an application dynamic dependency; it neither copies nor loads that file. The example plugin name is illustrative and must reflect actual application dependencies.

`extensions.producer` records AST→LLVM, O3, checked mode, the actual static/external LLVM backend and the consumer-provided runtime contract, without absolute build-host paths. Exact language dependencies remain in `dependencies`. The producer's LLVM toolchain is separate from application dynamic dependencies. Runtime dependency names are publisher declarations, not a claim that every dynamic-loading path has been scanned.

These packages require `scalar-unit-v1`, so compilers without that capability reject them before loading. Ordinary imports accept both these language-generated objects and the manually declared C ABI archives above; neither interface contains runtime function bodies.

## Ordinary imports

Place the package at `modules/example/math.chlib`. A consumer uses:

```chali
import example::math;
void main() {
    int64 answer = example::math::add(20, 22);
    assert(answer == 42);
}
```

`chali --check consumer.chl` checks declarations; `chali aot consumer.chl --out build/consumer` links the selected implementation. Provider implementation source is not needed. Search proceeds through the current unit directory, its `modules/` directory and `-I` directories, preferring `.chlib` over `.chl` at each location. A found but invalid/incompatible package fails without source fallback. Embedded standard-library loading remains available.

The current native interface accepts type/function declarations with explicit C ABI or verified language entry points. Runtime function bodies, globals and static fields are rejected. Normal type, visibility and borrowing checks apply. Template instantiation, library initialization and the owning-value internal language ABI are not enabled by carrying optional source/template assets.

Manifest dependencies are resolved even when absent from public imports, with exact versions and whole-package SHA-256. A located package contributes its qualified module root for transitive lookup; `-I modules` may also supply it explicitly. Pinned dependencies cannot fall back to source. Repeated module identities must have the same digest; cycles fail. A compilation loads at most 128 packages with at most 256 MiB of total payload. Checking and linking use the same verified owned snapshot; importing does not initialize a library. Target system libraries join the link; deployment still supplies application dynamic libraries.

Non-standard imports in a package interface must appear in its exact manifest dependencies; unpinned source modules cannot complete that interface.

## Project content locks and standalone linking

Generate a separate lock for every selected package, including all transitive dependencies:

```sh
chali lib lock modules/example/math.chlib --out chali.libraries.lock
chali check consumer.chl --library-lock chali.libraries.lock
chali aot consumer.chl --library-lock chali.libraries.lock --out build/consumer
```

Pass multiple package paths to `lib lock`. Every manifest dependency must be supplied with the matching version and digest. The lock records its format, compiler version and sorted module/version/whole-package SHA-256 records without absolute host paths. The same set produces identical bytes. Failed generation preserves an existing lock and cannot overwrite an input package. Checking never creates or modifies a lock.

`--library-lock` checks every loaded package. Unlisted packages and changed versions/digests fail. A module named in the lock cannot fall back to `.chl`; other ordinary project source modules retain normal importing. The format-one canonical JSON is limited to 128 modules and 1 MiB. Duplicate keys, unknown fields and incompatible versions fail. This separate file does not alter the existing source dependency `chali.lock` format.

Objects/assembly do not embed library archives. Supply original packages to a standalone link:

```sh
chali aot consumer.chl --emit-obj --library-lock chali.libraries.lock --out build/consumer
chali link build/consumer.o --package modules/example/math.chlib --library-lock chali.libraries.lock --out build/consumer.exe
```

`--package` is repeatable; `-L` also supplies additional package roots. Standalone linking reads the manifests and exact transitive dependencies without parsing/executing interface source or running initialization. Native implementations come from verified snapshots and target system libraries join the link. Output cannot overwrite a package or lock. Link receipts are not generated automatically; callers explicitly retain the packages and lock used.

## Independent compatibility fields

| Information | Check |
| --- | --- |
| Container format | Currently 1; other formats are rejected |
| Interface format, language ABI, runtime ABI | Recorded independently, currently each 1 |
| Compiler version | Must exactly match `VERSION`; older assets are not silently loaded |
| Module and package version | Full module identity and exact package version; dependencies also pin the whole-package SHA-256 |
| Target triple | Canonical Windows x64, Linux x64 or Linux ARM64 triple; at most one implementation per target |
| Object format, width, endian, layout | Matching COFF/ELF, currently 64-bit little endian, and the registered target runtime's data layout |
| CPU/features | Default-CPU implementations may serve the same architecture; specialized CPUs must match, and unmet required features are rejected |
| Native convention | Current format tools accept `c-v1`, separately from interface and runtime protocols |
| Bitcode | An optional independent asset records its exact LLVM version; native selection does not treat it as a machine object |
| Ignorable extensions | Only under `extensions`; unknown semantic fields, required capabilities and reserved flags are rejected |

Repeated `--dependency "MODULE=VERSION#SHA256"` pins dependencies. `--system-library NAME` records link libraries; `--runtime-library NAME` records application dynamic libraries. Format checks validate these declarations; the consuming build resolves dependency files and prepares final deployment.

`--license` records the license name. `--license-asset legal/LICENSE.txt` names an in-package license, supplied during packing as `--entry "license:legal/LICENSE.txt=LICENSE"`. Multiple licenses and attribution files are supported. Compiler tooling, static link inputs and application dynamic libraries are separate concerns; embedding LLVM in the producer does not make LLVM DLLs an application dependency.

## Binary encoding

Integers use explicit little-endian encoding; C++ structures, pointers and `size_t` are never written directly. Digests are 32 raw bytes. SHA-256 follows [NIST FIPS 180-4](https://csrc.nist.gov/pubs/fips/180-4/final). Digests check content changes; origin authentication requires separate signatures or a trusted distribution channel.

| Header offset | Width | Meaning |
| --- | --- | --- |
| 0 | 8 | ASCII `CHLIB1\r\n` |
| 8 | 4 | Container version 1 |
| 12 | 4 | Entry count |
| 16 | 8 | Canonical manifest byte length |
| 24 | 32 | Manifest SHA-256 |
| 56 | Manifest length | Canonical UTF-8 JSON without BOM |

Entries follow sequentially: `u16 name length`, `u16 kind`, `u32 flags`, `u64 payload length`, `32-byte record digest`, `name`, `payload`. The record digest covers the 16-byte metadata, name and complete payload. Changes to names, kinds and lengths are therefore checked as well. There is no padding or implicit host alignment. Names must be strictly increasing; duplicate names, parent/empty path segments and unregistered trailing data are rejected.

| Kind | Payload |
| --- | --- |
| 1 | Language interface |
| 2 | Uninstantiated templates |
| 3/4 | Native object/self-contained archive |
| 5/6 | Bitcode/independent bytecode |
| 7/8 | Explicit source/license and attribution |

Flag bit 0 permits an unknown kind to be ignored; other bits are reserved. Referenced interfaces, implementations and licenses must exist with the right kinds. Bitcode/bytecode tags identify container payloads: successful inspection does not establish verification by their execution backends. `source` merely records an explicit source asset; native target selection does not automatically fall back to it.

Budgets are 256 MiB per package, 1 MiB per manifest, 4096 entries and 240 bytes per entry name. Text entries are limited to 32 MiB and reject NUL. There are at most 64 targets and 1024 dependencies/items in each string collection. Declared lengths are checked against actual remaining file bytes before allocation. These bound encoded data and payloads, not total compiler process memory.

Native headers and archive-member bounds are inspected on the same owned bytes whose digests were checked, without reopening a package to decide implementation identity. The applicable toolchain and interoperability contract still validate symbol indexes, sections, relocations and actual native signatures. Thin archives, nested archives and wrong architectures are rejected.

## Failure handling and reproduction

Packages are written in an isolated workspace under root `tmp/` and published only after validation. Failures preserve existing output. CLI packing cannot overwrite its manifest or input assets. Failed reading does not publish partial state. Digest, version, target and required-capability errors never silently select another implementation.

```sh
python tools/run_module_gates.py libraries defaults docs --chali PATH --random-recheck 1
python tests/chali_digest.py
python tests/chali_library_import.py --chali PATH
python tests/chali_library_package.py --chali PATH
python tests/chali_library_package.py --chali STATIC_PATH --other-chali EXTERNAL_PATH
```

Tests independently decode the public format and compare digests with `hashlib`. They cover chunk/padding boundaries, canonical encoding, corruption, budgets, optional entries, three-target selection and transactional publication. A restored archive is linked and executed with the standalone linker. Cross-target generation/format checks and host execution are recorded separately.

Explicit lifetime contracts are not yet stored in library interfaces. Functions with #static or #borrow are rejected during export so that consumer constraints cannot be weakened by omitting a contract.

## Plain record parameters by value

Nonowning plain records can be passed by value using native ABI chali-value-v1 and required feature plain-record-value-v1. The private ChaLiValue declaration is restricted to verified package interfaces. The callee allocates an independent copy with the shared allocator and frees it on exit. Mutating a copied parameter does not change the caller. Nested record identity and layout hashes are included in entry symbols.

For example, an int64 shifted(Point value, int64 delta) function may update value.x and return it. Calling shifted(point, 2) with point.x equal to 40 returns 42 and leaves point.x equal to 40. Plain record results use the relocation contract below. Owning fields, explicit lifetime contracts and the complete shared type registry remain unavailable; these entries cannot declare a physical C ABI #export.

## Unit data and plain record results

A nonowning plain record can be returned by value. For example, Point make(int64 x, float64 y) may create a local Point, assign its fields and return it. The package stores the target data image and per-symbol layout. The final program appends each unit at a 16-byte-aligned base and defines its content-identified base symbol. The caller receives a normal record copy; repeated calls do not retain additional heap blocks. Nested plain records and returned record parameters are covered, while same-named types retain module identity.

Targets carrying data add unitData, unitLayout and unitSymbol together and require unit-data-v1. Image and layout entries use kind 9 (unit-data). The base symbol hashes module, target triple, complete layout and image. Missing assets, mismatched identities, duplicate layout names and out-of-bounds ranges are rejected. Packages without unit data retain the existing encoding.

Record result declarations require a verified interface carrying this capability. Plain record moves and fresh transfers use the shared lifetime identity contract below; other unsupported bodies remain rejected. Owning fields, interface/closure results and initialization rollback remain separate acceptance work.

## Shared lifetime type identities

Packages using native record lifetime operations require shared-type-id-v1. Generated LLVM code references type symbols derived from the full module identity and record layout. The final program supplies its current type numbers. Adding same-named or unrelated consumer types cannot redirect move markers to a different type. Only explicit lifetime type positions are relocated; ordinary integer values are not guessed or rewritten.

Lifetime queries, checks and end markers call shared C primitives and use the same object table. Fresh plain record forwarding, recursion and explicit moves are supported; repeated moves remain rejected. Interface boxing, closures, dynamic type queries and owning resources still require further shared registration work.

Independent units also support AST ownership counters when compiled with CHALI_BACKEND_PROFILE=1. Site strings remain private to the unit and calls reach the final program shared counter. CHALI_BACKEND_PROFILE_OUTPUT selects a local JSON file. Instrumented results are not release timing measurements.
