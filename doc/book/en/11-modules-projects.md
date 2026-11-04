# Chapter 11 Projects and modules

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../11-模块与工程.md)
<!-- END LANGUAGE NAVIGATION -->

Larger programs need explicit locations for entries, modules, configuration and runtime assets. Modules govern ownership and lookup, project configuration controls compilation, and working directories affect application file access. These responsibilities interact without being identical.

Begin with directories and qualified names, then reproducible boundaries and the relationship between embedded core-library assets and user projects. A project should remain explainable outside its original development directory.

## 11.1 Choose the entry point and directories

Choose an explicit entry, then organize cooperating code by responsibility. Start from src/main.chl and separate HTTP, business and storage work rather than placing every responsibility in every file.

Pages and configuration have runtime paths. Successful import establishes source availability, while relative application file access also depends on startup location. Record both during directory design to make deployment predictable.

Mutable module globals are private to their declaring module. To publish a read-only value, declare a module constant such as `const int64 DEFAULT_PORT = 8080;`. Importers can write `config::DEFAULT_PORT`. Completion, hover and definition links use the declaration and its documentation comment.

## 11.2 Module declarations and qualified names

Module declarations give names an owner. Import required modules and qualify names to choose declarations explicitly, especially with identically named types. Do not depend on import order to resolve ambiguity.

<!-- verify-book -->
```chali
inline int64 square(int64 value)
{
    return value * value;
}

void main()
{
    println("{}", square(3));
}
```

```text
9
```

Square takes and returns int64, computing value*value. Passing three produces nine. Inline is a compilation-related annotation rather than a change to argument or return semantics; the marker alone does not prove actual machine-code inlining.

This complete runnable example contains no module or import. It illustrates a function that can belong to a module rather than demonstrating multi-file lookup or visibility. Splitting a real project still requires checking ownership, qualification and source paths. Arithmetic behavior and module organization are separate concerns.


## 11.3 Project configuration and commands

chali.toml records the entry and project options. When opening an existing project, compare its settings with the options listed by chali help. The run command uses checked AOT, and aot exports native artifacts. Chapter 27 covers targets and linker configuration.

### Why the standard library can travel with the compiler

Distinguish source text, precompiled frontend representation and final machine code. Source supports reading, diagnostics and export. Frontend representation preserves recognized declarations and syntax. Machine code depends on the target and instantiated types. Embedding one layer does not complete all subsequent work.

Version 1.0.0 embeds the std::core core, optional standard modules and template catalogs as binary assets. Imports restore parsing nodes and template definitions in process, checking the format, source identity and content digest. Distributed compilers do not need the original build directory for these assets, and default standard-library compilation does not read or write a disk standard-library cache.

Concrete generic instantiation, macro expansion, user-program checking and target code generation still take place. For example, `List<int64>` and `List<String>` share an embedded template definition, while the current compilation checks their concrete types and generates target code. The embedded representation is relocatable frontend data, so it does not copy one target's container machine code to another. Source remains embedded for reading, diagnostics, export and parsing fallback when needed.

Embedded library assets, user-project incremental results and application resources serve different purposes. Copying a compiler does not copy web pages, configuration or databases. Deleting project caches does not repair a damaged embedded asset. Identify whether a failure concerns asset loading, user-module parsing or application resource access before choosing a remedy.

See the [toolchain reference](../../en/09-toolchain.md) for asset formats, source fallback and project incremental-check options.

## 11.4 Reproducible project boundaries

Document the working directory, external libraries, data files and target platform. Separate checking, example execution and packaging. Maintain generated files through their sources so later regeneration preserves changes.

### Declare a module in source

A single file can declare a module and qualify a function call with it.

<!-- verify-book -->
```chali
module book_demo;

int64 answer() {
    return 42;
}
void main() {
    println("{}", book_demo::answer());
}
```

```text
42
```

### Summary

Record source inputs, compiler-owned assets and runtime application files separately. Successful import does not validate a web path, and restored core library data does not mean optional modules and user generic instances are already machine code.

Document the entry, configuration location, working directory, target and external libraries. Macro and reflection mechanisms retain these name and type ownership boundaries even when they generate code.

Next: [Chapter 22](22-macros.md).
