# Bundled ChaLi compiler

`npm run package` copies the real ChaLi compiler here as `chali.exe` (or
`chali` on POSIX) and writes `compiler.json` with its version, size, source,
and SHA-256. The installed plugin prefers this binary over a repository or
`PATH` lookup, so it stays useful outside this checkout.

The binary is deliberately not tracked by Git: it is several hundred MiB and
is produced from the ChaLi release pipeline. Run the package script in every
checkout that should install a self-contained plugin.
