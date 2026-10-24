# Chapter 18 Modules and projects

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../18-模块与工程.md)
<!-- END LANGUAGE NAVIGATION -->

A module provides a name for related declarations. Declare it explicitly with `module name;` and import other modules with `import`. A user source file without a module declaration belongs to lowercase `program`.

```chali
module application;

import std::text;

void main()
{
    println("Application started");
}
```

Keep module responsibilities visible: a model describes data, a store handles persistence and an entry point coordinates them. A module boundary does not by itself create an operating-system dynamic library. Native dynamic-library export is a separate AOT operation.

Project configuration and dependency resolution belong in `chali.toml` and the documented dependency tools. Prefer reproducible dependency versions and preserve the lock information. Do not assume that starting the compiler from another directory changes which module the source declares.

Imported macros are visible through the macro completion path. Typing `$` asks for macros rather than ordinary symbols; module qualification narrows that search. Function overloads and macro alternatives have separate matching rules.

Exercise: move a pure helper into another module, import it and call it with a qualified name. Compare an explicitly named module with the implicit `program` identity. Next: [macros](19-macros.md).
