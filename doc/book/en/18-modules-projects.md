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
