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

The current native interface accepts type/function declarations with explicit C ABI entry points. Runtime function bodies, globals and static fields are rejected. Normal type, visibility and borrowing checks apply. Template instantiation, library initialization and the owning-value internal language ABI are not enabled by carrying optional source/template assets.

Manifest dependencies are resolved even when absent from public imports, with exact versions and whole-package SHA-256. A located package contributes its qualified module root for transitive lookup; `-I modules` may also supply it explicitly. Pinned dependencies cannot fall back to source. Repeated module identities must have the same digest; cycles fail. A compilation loads at most 128 packages with at most 256 MiB of total payload. Checking and linking use the same verified owned snapshot; importing does not initialize a library. Target system libraries join the link; deployment still supplies application dynamic libraries.

Object/assembly output covers the current compilation unit. A subsequent standalone link must still supply library implementations; consumer `.o`/`.s` files do not embed them. Standalone link receipts and project-level library lock files are not produced yet.

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
python tests/chali_digest.py
python tests/chali_library_import.py --chali PATH
python tests/chali_library_package.py --chali PATH
python tests/chali_library_package.py --chali STATIC_PATH --other-chali EXTERNAL_PATH
```

Tests independently decode the public format and compare digests with `hashlib`. They cover chunk/padding boundaries, canonical encoding, corruption, budgets, optional entries, three-target selection and transactional publication. A restored archive is linked and executed with the standalone linker. Cross-target generation/format checks and host execution are recorded separately.
