# Appendix D ChaLi Language Limitations

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../D-语言限制.md)
<!-- END LANGUAGE NAVIGATION -->

This appendix summarizes current implementation boundaries and practical alternatives. Language rules, conservative analysis, and host requirements are distinct. Passing fixed stress tests does not promise compilation for arbitrary nesting or resource consumption. See [compiler safety and limits](../../en/34-compiler-safety.md) for diagnostics and reproductions.

## D.1 Syntax and compilation scale

Use for-in, C-style `for(init; condition; update)`, while, or loop with break. The initializer may declare loop-scoped variables. Conditions require bool; an omitted condition is true, and continue executes the update before testing again. Standalone loop statements may omit their semicolon and retain compatibility with the old spelling; loop expressions follow their enclosing statement terminator. Both for forms accept else with access to initialized variables or the last iterated value. Empty input uses ordinary value defaults; references without a valid origin cannot be read. Borrowed iterator elements require a copyable snapshot for else; handle noncopyable borrowed elements in the body. Do while is not supported. Tuple access requires a compile-time constant index.

AST fragment traversal has a depth budget of 256 and a budget of 1000000 visited nodes. These measure tree structure, not source length, parentheses, or one universal nesting limit. Fixed regressions cover 192 redundant parentheses, 48 nested blocks, 16 nested generic containers, and complex macro type punctuation; a 600-term left-associated addition chain is rejected by the budget diagnostic. Split generated expressions into local variables or functions.

Incomplete generic delimiters must produce a syntax diagnostic. Successful parsing still requires type, borrow, and control-flow checks. Formatting does not make an invalid program valid.

## D.2 Names, generics, and enum inference

Imported types, functions, constants, and variants can use short names when unambiguous. Conflicts require qualification or enough context to select one candidate. Import order does not arbitrarily choose a winner.

Generic arguments can be inferred from actual arguments and available expected types. Insufficient information still requires annotations or explicit arguments. Generic bodies are checked for concrete instances; one successful instance does not prove every unused type combination. Compilation also depends on instantiation, memory, and host time budgets.

When the target enum and its arguments are known, returns, initialization, assignment, and arguments may wrap a payload in a uniquely matching single-payload variant. Equal-priority candidates require an explicit variant. In `Result<T, Error>`, T may itself be an error type, so a generic error branch may still need `Err(error)` to preserve its meaning. Multiple-payload variants require explicit construction. Wrapping preserves move, borrow, and cleanup rules.

## D.3 Borrow contracts across functions

`#borrow(a,b)` means target a borrows from source b. Use #ret for the return target and a source list for multiple sources. Contracts support generic and cross-module direct calls; concrete instances and call sites are checked. Contracts describe origin relationships and do not extend object lifetime or promise dynamic liveness.

Functions with #borrow or #static contracts cannot become ordinary function values, because function-value types do not retain those contracts. Keep a direct call or an explicit entry point with a suitable contract. Synchronous callbacks and stored asynchronous callbacks have different lifetime requirements; a local borrow cannot satisfy a static-source registration contract.

Storage checks cover assignment, rebinding, known receiver aliases, container reference-element writes, Any.set, and calls with explicit output borrow contracts. Arbitrary container writes inside callees without declared contracts are not fully proved across functions. Raw pointers, host interfaces, and erased types retain their trust boundaries. See [resources and lifetimes](../../en/22-resource-lifetime.md).

## D.4 Known conservative analysis limits

Control flow does not prove arbitrary relationships between separate conditions. Selecting an origin in one if and returning under the negation in another may be conservatively rejected. Keep origin selection and return in the same branch structure; see structural in the [cross-module helper sample](../../../example/samples/modules/syntax_helpers.chl).

Returning arrays with borrowed elements through `return { ...; yield array; };` may retain an old temporary identity and be rejected even for global origins. Use a named array and an ordinary return, as in the [verified alternative](../../../example/samples/syntax_borrowed_array_named.chl). Returning references to local objects remains invalid.

Loops containing destruction distinguish the first iteration within a budget based on the current function's source size, then merge conservatively when the budget is exhausted. Analysis does not prove arbitrary dynamic heap graphs acyclic or enumerate every alias through unknown pointer fields. Saving the next node before freeing the current node is covered by regression tests; reading the freed node or an old alias must still be rejected.

## D.5 Resources, errors, and unsafe operations

Moving a resource invalidates its source, including moves through mutable references. Reinitialization must follow ownership and move rules. A defer body executes on exit; its origins must remain valid on actual exit paths, not merely when registered.

Propagation with `?` requires a compatible enclosing return type. Nested Result values may propagate successively, but incompatible error types still fail. unwrap may terminate on failure; ordinary application code should prefer match, ?, or an explicit default.

unsafe permits specified low-level operations while retaining other type and ownership rules. Callers remain responsible for address size, alignment, lifetime, thread access, and ABI representation. The memory pool guarantees only 16-byte alignment. Checked native execution adds dynamic bounds and liveness checks without proving arbitrary external addresses or host code correct.

## D.6 Builds and platforms

run uses checked native builds; build defaults to unchecked release builds. Unchecked builds do not promise the same dynamic checks. LLVM performs optimization. A compiler with statically embedded LLVM rejects --llvm and build.llvm project settings; external LLVM variants accept the relevant tool-path configuration.

The shared runtime requires C++17 and operating-system services; bare-metal support is not promised. Building the compiler for an architecture does not establish support for every native export from that architecture. Dynamic libraries, graphics, audio, and debugging depend on the deployment platform. Language function values are not machine function pointers; host callbacks use the public C ABI invocation boundary. Consult the [LLVM build reference](../../en/38-build.md) and actual platform validation.

Fixed stress programs and rejection cases live in the sample directories and run in the full gate. For an uncovered crash, invalid access accepted by checking, or clear false positive, retain the minimal program, command, compiler version, and diagnostic. Do not hide the problem by deleting checks or changing expected results.
