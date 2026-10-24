# Chapter 19 Preprocessing and hygienic macros

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../19-预处理与宏.md)
<!-- END LANGUAGE NAVIGATION -->

Macros transform program structure during compilation. They are useful when repeated syntax expresses one stable operation, but ordinary functions remain the clearer choice for most runtime calculations.

ChaLi's hygienic macros preserve binding relationships so that an internal temporary does not accidentally capture a caller's variable. Macro arguments have documented fragment categories; expression, type, identifier, statement and token-group arguments should not be treated as one unstructured string.

Macro declarations and invocations use the lowercase `$macro` system. Parameter groups and repetition describe how source fragments are matched. Regex branches use the documented ECMAScript behavior. Consult the [macro reference](../../en/24-literals-operators-macros.md) for the exact declaration forms rather than copying another language's macro syntax.

Generated code still undergoes semantic checking. Expansion does not bypass types, references, resource ownership or the compiler's bounded fragment budgets. When debugging, inspect the expansion and its source mapping. The VS Code expansion view groups related results into tabs, with searchable macro names and a tree of expansion levels.

Exercise: identify a repeated source pattern, first implement it as a function, then explain which additional requirement would justify a macro. Review variable names at the call site to understand why hygiene matters. Next: [reflection](20-reflection.md).
