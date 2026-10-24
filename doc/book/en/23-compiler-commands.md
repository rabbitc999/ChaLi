# Chapter 23 Compiler commands

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../23-编译器指令.md)
<!-- END LANGUAGE NAVIGATION -->

The compiler provides source execution, validation, artifact export and development tools. Begin with `chali --help`; use `chali man help` for the embedded manual's own command structure.

```sh
chali hello.chl
chali --check hello.chl
chali svmb hello.chl out build/hello
chali aot hello.chl --out build/hello
chali man api std::text
chali man book 15 --lang en
```

`fmt` formats source, `test` runs the documented project tests and `lsp` starts the language server. Dependency commands manage the project's declared dependencies. A filename with an unsupported format is rejected rather than guessed to be a different artifact kind.

Set the tool display language with the documented language option or configuration. `man book --lang en` selects English book content; this is independent of ChaLi source syntax. `--json` gives machine-readable output for supported manual queries.

Diagnostics contain a location and useful context. Preserve their original text when propagating errors through wrappers. Compilation and program execution are distinct phases, so report their times separately in performance work.

Exercise: inspect the same book chapter as terminal text and JSON. Export the book in an empty directory and open its English README. Next: [AOT export](24-aot-export.md).
