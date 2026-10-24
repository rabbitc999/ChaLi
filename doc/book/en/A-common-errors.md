# Appendix A Common errors

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../A-常见错误速查.md)
<!-- END LANGUAGE NAVIGATION -->

Read the first diagnostic at its source location, then distinguish syntax, type, borrowing, host-service and runtime-budget failures. Later errors may be consequences of the first.

| Symptom | What to inspect |
| --- | --- |
| An identifier is unknown | Module declaration, imports, spelling and qualification |
| No overload matches | Actual argument count and types, defaults and expected callback shape |
| More than one callback overload remains | Supply the intended type information |
| CHL1204 lifetime error | Whether a reference or closure outlives the source it borrows |
| A read-only value cannot change | Parameter/reference/capture mutability |
| A resource cannot be copied | Its noncopyable ownership protocol; whether a move is intended |
| A result cannot be unwrapped | Check success or propagate the concrete error |
| A string slice displays broken text | Byte offsets may split a UTF-8 character |
| An array access fails | Index, length and the lifetime of the underlying storage |
| A native library cannot load | Target architecture, dependency paths and declared C ABI |
| A compiler memory-budget diagnostic occurs | Frontend compilation scale, not only the program's runtime heap |
| A runtime allocation fails | Configured host heap budget and requested size |
| AOT export cannot locate tools | The selected LLVM variant and compatible host toolchain |

Use `chali --check` for static diagnostics. Use the original runtime message for failed operations rather than replacing it with a generic string. If reporting a defect, preserve a minimal source, compiler version, exact command, host architecture, output and exit code.

For macro problems, inspect the expansion and its source mapping. For C callbacks, record ownership and callback lifetime. For performance problems, use timed workloads with verified results rather than inferring speed from a capability list. Continue with [the standard-library reference](B-standard-library.md).
