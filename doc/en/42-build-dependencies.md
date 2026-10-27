# Downloaded build dependencies

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../42-build-dependencies.md)
<!-- END LANGUAGE NAVIGATION -->

SQLite and miniz source code is neither checked into Git nor copied into generator exports. Builds download fixed releases from the [lock file](../../tools/dependencies.lock.json), verifying both archive and selected-file SHA256 digests. The existing SQLite SHA3-256 record was also checked against the official archive.

| Library | Pinned version | Archive SHA256 |
| --- | --- | --- |
| SQLite | 3.54.0 | `68e913b0fe8ec6b4e5f391c594a28872884e80d6cd042a1e50c957be46b5aa4d` |
| miniz | 3.1.2 | `e7a0334927a069dc5aeb69616d371542a4376e1569290d7102b88c6f3992d6cb` |

Versions, upstream URLs, archive roots and file hashes come from the lock file. Builds do not follow `latest` or moving branches. Network failures, corrupted archives and incorrect file digests fail the build without bypassing verification.

## Building

Repository Make builds prepare `build/dependencies/sqlite` and `build/dependencies/miniz`. CMake builds use `dependencies` inside their build directory. Exported standalone compilers carry the same downloader and lock file. A first build requires Python 3 and network access.

```text
python tools/fetch_dependencies.py
python tools/fetch_dependencies.py --offline
make -j4
```

Archives are cached in project `tmp/dependency_downloads/`; extracted build inputs go in `build/dependencies/`. Neither is committed to Git. Valid build caches work offline, and damaged files can be restored from verified cached archives. `--offline` forbids network access and fails if a required valid archive is missing. CMake accepts `-DCHALI_DEPENDENCY_DIR=path` to select its build cache.

Writes to the shared cache are locked. Files are installed after archive and member checks complete. Only explicitly listed members are read, rather than extracting arbitrary archive paths. The downloader generates miniz's static export macro header without rewriting upstream source files.

## Runtime and delivery

Downloads happen at build time. Compilers and AOT programs still statically link the required implementations; running SQLite programs requires neither a source download nor `sqlite3.dll`. SQLite enables thread safety and disables dynamic extension loading. Only miniz's required tinfl decompression kernel is compiled.

The miniz MIT license is retained at [licenses/miniz/LICENSE](../../licenses/miniz/LICENSE) and included in release packages alongside [NOTICE](../../NOTICE). SQLite remains public domain, with upstream provenance recorded in the lock file and NOTICE.

Version updates change the pinned version, URL and digests, then repeat cache/download tests, full builds, SQLite functionality, native dependency extraction, generator and packaging checks. Downloaded third-party source must not be copied back into `source/` or `library/`.
