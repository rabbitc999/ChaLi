# Chapter 18 Projects and modules

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../18-模块与工程.md)
<!-- END LANGUAGE NAVIGATION -->

A project organizes entry points, modules, resources and build configuration. Modules establish declaration ownership and visibility; configuration controls source lookup and application generation.

## 18.1 Choose the entry point and directories

Place the entry in src/main.chl, separate business modules by responsibility, and keep runtime assets such as web pages in a separate directory. Relative file access depends on the working directory; import search paths do not locate runtime assets.

## 18.2 Module declarations and qualified names

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

## 18.3 Project configuration and commands

chali.toml records the entry and project options. Inspect configuration and chali help when working with an existing project; an old interpret setting does not establish an interpreter execution mode. Current run uses checked AOT and aot exports native artifacts. Chapter 24 covers target and linker configuration.

### Why the standard library can travel with the compiler

Distinguish source text, precompiled frontend representation and final machine code. Source supports reading, diagnostics and export. Frontend representation preserves recognized declarations and syntax. Machine code depends on the target and instantiated types. Embedding one layer does not complete all subsequent work.

In 1.0.11, the parsed std::lang core is embedded as a binary asset and restored inside the compiler process. A distributed compiler does not need its original build directory to find that asset, and default standard-library compilation does not read or write a disk standard-library cache. This reduces repeated parsing of core source and removes a developer cache directory as a deployment prerequisite.

This stage covers core parsing results only. Optional modules and uninstantiated templates still require source frontend work. Concrete generic instantiation, macro expansion, user-program checking and target code generation remain necessary. A container template used with two element types still requires the corresponding checks and generated code. Embedded templates do not establish that all library calls have prebuilt machine code, and historical acceptance results do not measure the current program's compilation time.

Embedded library assets, user-project incremental results and application resources serve different purposes. Copying a compiler does not copy web pages, configuration or databases. Deleting project caches does not repair a damaged embedded asset. Identify whether a failure concerns asset loading, user-module parsing or application resource access before choosing a remedy.

The [core-stage acceptance record](../../../tools/history/20261030/chali-1.0.11-precompiled-core/README.md) identifies the compiler, platform and checks performed at that time. It does not establish completion of the full standard-library precompilation plan.

## 18.4 Reproducible project boundaries

Document the working directory, external libraries, data files and target platform. Separate checking, example execution and packaging. Maintain generated files through their sources so later regeneration preserves changes.

## 18.5 Exercises

1. Design src and web directories for a student system.
2. Are source lookup and resource lookup the same?
3. How do you choose between two Error declarations?
4. Document project startup requirements.

### Answers and checks

1. Separate entry, HTTP, business, storage and web assets.
2. No; import configuration and runtime paths govern them separately.
3. Use a qualified name.
4. Record the compiler, target, external libraries, configuration and working directory.

Next: [Chapter 19](19-macros.md).
