# Resource lifetimes and borrow control flow

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../22-resource-lifetime.md)
<!-- END LANGUAGE NAVIGATION -->

## Allocation identities without tombstones

`source/ircode/allocation_index.hpp` stores active logical intervals and each physical root's latest generation and displacement width. Release removes the active interval immediately. No retired identities or sweeping boundaries are stored. Repeated release fails; physical reuse never revives an old logical reference. Generation exhaustion refuses reuse instead of wrapping, and repeated reuse of one physical root needs only one identity slot.

## Resource construction and cleanup modules

Explicit release uses `value.drop()` for heap references, arrays and owned resources. Neither `delete` nor a standalone `drop` statement exists. An ordinary type's unrelated method named `drop` remains an ordinary call.

`source/semantics/resource_lifetime.cpp` centralizes construction registration, Closeable/Drop calls, aggregate destruction, owner-slot cleanup and scope exits. Explicit termination, automatic cleanup and moves share the ownership contract. Executed release clears the owner slot without removing another branch's cleanup. Host close shuts down the operating-system resource; drop later destroys its still-owned record rather than retaining a released record as a tombstone.

A struct that implements `Closeable` is automatically non-copyable (`MStructDef::nonCopyable`, sharing `TypeNonCopyable` with `#NonCopyable opcode`): a copy would mean two owners and two `close()` calls. Copy initialization, assignment, value arguments and return copies are rejected with a diagnostic naming both origins; transfer with `move x`, share through a reference, and return local owned values directly. The rule follows fields and elements: an aggregate with a non-copyable field is non-copyable, and arrays or containers that copy such elements are rejected.

The independently compiled `source/chali_resource_cleanup.cpp` consumes borrow control-flow facts. It merges drop for the same precise storage identity: remove a later operation when all incoming paths have terminated the resource, or guard it when only some paths have. Reinitialization, rebinding and allocation update the proof. Dynamic indices, ambiguous origins and opaque cross-call effects do not prove identity. Runtime release remains non-idempotent and reports CHL1205 for unmerged repeated releases; ordinary reads and writes still reject use after release.

Explicit close on a named aggregate uses a compiler-local closed flag, updated on the executed path and reset at construction. Automatic close and explicit drop share that flag without adding a runtime tombstone API.

A `move` through a mutable reference invalidates the owned value it points to. Macro `mut &` caches follow the same origin-binding rules as ordinary reference declarations. Known concrete functions and forwarding calls propagate potentially moved reference-parameter paths to callers. Later reads or moves through the original owner or an old alias report CHL1205, including after branch joins. A reference slot may be rebound to another live object; redeclaring an owning binding requires the old binding to have been fully moved on every incoming path.

`defer` checks captures at registration and checks their borrowed origins again on the normal, return, break, and continue paths that actually execute cleanup. A callback cannot read an origin consumed after registration. An unexecuted registration or completed inner cleanup does not extend the borrow into an outer scope. Native language functions restore the caller's source location on normal return, so a nested `close()` cannot claim a later runtime failure after that cleanup function has returned.

`defer` delays the following statement and borrows its variables by default. `defer f();` calls `f` at exit; `defer f;` only delays evaluating that expression and does not immediately copy and register a function value. Moving or dropping the borrowed `f` before exit must be rejected. For an independent function snapshot, first create `let saved = void fn copy[f](){f();};` and then use `defer saved();`. External references held by that snapshot must still remain valid.

## Full-expression temporary ownership

Fresh string arguments borrowed by printing, formatting and file writes are first evaluated into internal owning temporaries. Calls run before reverse-order cleanup at the full-expression boundary. Transferring a result to a named owner, return value or another temporary clears the source slot. Loop iterations cannot release a previous iteration's object again. Literals and empty slots need no extra owner; concatenation chains retain the single-join allocation path.

## runtime and restart

The runtime heap in `source/ircode/runtime_heap.cpp` uses direct payload allocations with system malloc or split and merged free intervals with pool mode. Ordinary scopes manage values created with `new`, and release immediately invalidates their allocation identities. There is no implicit garbage collection.

## fn borrowing research

The capture-analysis organization was checked against Rust's
[upvar.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_typeck/src/upvar.rs)
and lifetime-constraint conversion against
[constraint_conversion.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/type_check/constraint_conversion.rs).
ChaLi retains its own scope-source facts and control-flow checker, with mutable borrowing
as its requested default capture rule.

Capture discovery lives in `CheckLambdaCaptures` in `source/semantics/generic_inference.cpp`;
`source/semantics/artifacts.cpp` prepares stable storage and `source/semantics/boxing.cpp` builds/restores
borrowed environments. `source/semantics/borrow_facts.cpp` propagates source lifetimes through
assignments, returns and aggregate fields. Assignment inference must discover captures
before recording a storage constraint. Copying a closure does not extend its sources.

Asynchronous event entry signatures require static callback sources, including public
stored callback fields. Capturing local variables cannot be hidden behind an ordinary
function-pointer conversion of an event registration entry. Synchronous algorithm callbacks
can borrow the caller's live variables.


## Checking later closure uses

Capture discovery and later uses are modeled separately. `FlowState::captures` records external borrowed places held by function values and aggregate fields. Copies, assignments, tuples and enum payloads propagate these sets; reference fields inside owned captures still depend on their external referents. Reading a callable checks whether a captured place was freed, moved or dropped on a reachable path. The callable itself and its captured sources are checked again after argument evaluation, preventing `callback(move source)` from invalidating its environment before invocation. Callable results conservatively inherit known external borrows from arguments and receivers. Reassignment replaces the previous capture set, and destruction after the closure's final use remains valid.

The design follows the explicit loan registration in Rust's [borrow_set.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/borrow_set.rs), capture-mode distinctions in [upvar.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_typeck/src/upvar.rs), and projection conflicts in [places_conflict.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/places_conflict.rs). It uses ChaLi's own place and control-flow analysis without copying Rust implementation code.

Checks cover visible invalidation and propagation, with concrete function summaries propagating resolvable moves through references. Opaque host code, raw memory writes and erased interprocedural side effects still depend on interface lifetime contracts. This analysis does not establish the absence of all undefined behavior.

## Explicit lifetime contracts `#borrow` / `#static`

The borrow checker is conservative by default: a call site folds the origins of **all** reference arguments into its result, so the result of `first(&x, &local)` is treated as also borrowing `&local`, and returning it is rejected by CHL1204. A contract written before a function declaration states which input sources the result borrows, under the rules below:

| ChaLi | Rust |
| --- | --- |
| `int64 mut& first(int64 mut& a, int64 mut& b)` with `#borrow(#ret, [a])` | `fn first<'a, 'b>(a: &'a mut i64, b: &'b mut i64) -> &'a mut i64` |
| `int64 mut& install(int64 mut& slot)` with `#static(slot)` | `fn install(slot: &'static mut i64) -> &'static mut i64` |

Three forms, all written before the function declaration and freely combinable:

- `#borrow(a,b)` is equivalent to `#borrow(a,[b])`: references saved in target `a` may come only from `b`. Use `#borrow(a,[b,c])` for multiple possible sources.
- `#static(a)` — `a` is treated as `'static`, so it can be returned across functions and stored for the long term; call sites must pass a genuinely `'static` source (a global).
- `#borrow(#ret,a)` permits only `a` as a returned borrow source. `#borrow(#ret,[a,b])` permits either source; callers retain their union.

Names must be parameters or locals of the same function, and sources of `#ret` must be parameters. A contract does two things: call-site propagation follows only the declared arguments instead of taking the maximum over all reference arguments, so `first(&x, &local)` can be returned; and the contract becomes an obligation, so a function with `#borrow(#ret, [a])` may not return a reference from anywhere else, and `out` in `#borrow(out, [a])` may not store one.

### Contract scope and limits

The first name in `#borrow(a,b)` is the destination, not an implicit list of return sources; write `#ret` explicitly for returned references. Directives precede ordinary or generic function declarations and may be repeated. Single-source shorthand and bracket notation are equivalent. Initialization, rebinding, fields, aliases, and branch/loop joins are checked against bound storage origins, so renaming an alias or rebinding a parameter cannot disguise its actual input source.

A source list is an upper bound on possible origins, not a requirement that every path use all sources. Even an unused listed argument conservatively constrains a caller. Declare only possible returned sources when a narrower lifetime is needed. Globals and independent allocations retain their existing rules; a contract cannot extend a value parameter's slot, a local slot, or a moved object's lifetime.

Saving a reference differs from copying a scalar: `a=b` through scalar references writes a value; `a=&b` rebinds the reference. Calls with `#borrow(out,b)` also check that a shorter-lived input cannot be saved into a longer-lived output aggregate containing references. The directive supplies no exclusive mutable-borrow lock, synchronization, or additional runtime ownership.

Control-flow analysis conservatively unions reachable origins and does not prove arbitrary numerical conditions complementary. Prefer an explicit `if/else` when every branch switches to the same origin; a separate loop and later condition may retain the initial origin.

Generic functions also support contracts, including direct calls with inferred type arguments. For example, put `#borrow(#ret,a)` before `T mut& pick<T>(T mut& a,T mut& b){return a;}` and call `pick(&x,&y)` without an explicit `<int64>`. Origins in the concrete body are checked at instantiation; returning the undeclared `b` must be rejected. An uninstantiated generic declaration does not establish that every possible type combination has passed checking.

Contracts follow declarations across module imports. Ordinary calls and generic instances check the declaration actually selected. Qualified calls to equally named functions in different modules retain their distinct source contracts instead of merging them; type parameters, interface constraints, and directive parameter names resolve in the declaring module. See the [cross-module contract sample](../../example/samples/borrow_contract_modules.chl) and the rejected `borrow_contract_module_*` samples.

Function-value types currently carry no lifetime contracts. Functions marked `#borrow` or `#static` must therefore be called directly rather than converted to a function value. A callback may call the function directly inside a closure, subject to normal capture and argument checks. External host contracts are trusted declarations whose implementations cannot be verified here; `unsafe` raw-address conversions do not gain a complete static alias proof. Checked and unchecked native paths retain their existing liveness, move, and resource-protocol checks; contracts do not certify arbitrary host code as memory-safe.

Reproducible coverage lives in [borrow contract stress tests](../../tests/chali_borrow_contract_stress.py), [move/defer comparisons](../../tests/chali_borrow_move_defer.py), and [nested Result stress tests](../../tests/chali_nested_result_stress.py). Cases include shorthand, allowed and forbidden origins, generic instances, long alias chains, large parameter lists, repeated directives, cleanup, and multilayer error propagation. Validation reports distinguish actual platforms and execution scope.

| Diagnostic | Trigger |
| --- | --- |
| CHL1210 | An unknown contract name, a non-parameter return source, or conversion of a contract-bearing function to a function value that cannot carry its contract |
| CHL1211 | A function with `#borrow(#ret, ...)` returns an undeclared source |
| CHL1212 | `out` of `#borrow(out, ...)` stored a reference from an undeclared source |
| CHL1213 | `#static(...)` was applied to a local variable |
| CHL1214 | A call site passed a non-`'static` argument to a `#static` parameter |

Two Rust entry points informed this work, adapted to ChaLi's existing semantics:

| Rust source | ChaLi counterpart |
| --- | --- |
| [wfcheck.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_hir_analysis/src/check/wfcheck.rs): well-formedness of explicit lifetime bounds in signatures (`region_known_to_outlive` / `ty_known_to_outlive`) | Every contract name is checked for its scope home; `#static` parameters are checked at call sites for an argument source depth of 0 |
| [region_infer/mod.rs](https://github.com/rust-lang/rust/blob/main/compiler/rustc_borrowck/src/region_infer/mod.rs): solving region constraints | A call to a function with `#borrow(#ret, ...)` propagates only the declared arguments, replacing the conservative maximum |

The implementation reuses ChaLi's own borrow source depths and borrow flow graph. It does not port Rust source, nor does it implement Rust's full region inference or lifetime variance analysis.

Automatic enum wrapping preserves payload copy restrictions: omitting a constructor never turns a copy of an existing non-copyable resource into an implicit move. Enums and structs owning such fields are non-copyable as well. Copy-based APIs and propagation protocols on standard `Option<T>` and `Result<T,E>` still require copyable payloads at instantiation. Non-copyable resources can use reference payloads or custom enums with explicit ownership transfer. Replacement evaluates the new value before cleaning up the old active payload; resource-count regressions cover variables, references and fields.

A traversal may save the next node before freeing the current node; reading the freed current node remains invalid. Analysis separates the first iteration from later iterations for loops with storage release, with extra graph nodes bounded by the current function source size. It does not prove arbitrary dynamic heap graphs acyclic or enumerate every alias of an unknown reference field; checked liveness checks remain a runtime boundary for those structures. An owned array returned from a value expression gains a result owner under the existing transfer/copy rules, while borrowed elements remain subject to origin and escape checks.

Store contracts cover assignment, borrowed List/Set/Map element stores, Any.set and calls with explicit #borrow output contracts on their actual evaluation paths. Receiver aliases resolve to original storage; copying an ordinary Map key is not a borrow store. Arbitrary internal container effects of callees lacking contracts are not fully proved across functions; host and erased values remain trust boundaries. Array value-expression returns containing borrows retain a conservative precision limit; see the [known limitation and named-array return workaround](34-compiler-safety.md).
