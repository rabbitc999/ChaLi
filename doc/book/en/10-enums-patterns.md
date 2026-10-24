# Chapter 10 Enums and pattern matching

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../10-枚举与模式匹配.md)
<!-- END LANGUAGE NAVIGATION -->

An enum describes a choice among named alternatives. Pattern matching handles alternatives in a structured way, with branch bindings local to the selected branch. This is useful for optional values, operation results and domain states that cannot be represented clearly by an unrelated integer flag.

Use `match` to make the relationship between a value's shape and its processing visible. Check exhaustiveness and the ordering of overlapping patterns. A wildcard is convenient when every remaining case really has the same treatment; otherwise it can hide a state that deserves its own behavior.

Match expressions can produce values. The selected branch must provide a compatible result, and any reference result must remain valid after the match finishes. Pattern syntax does not grant ownership of an object that was only borrowed.

The compiler reports overlapping integer patterns with warning `CHL1601`; a warning is still useful information even when compilation proceeds. Review whether the later branch can ever receive the intended input.

Exercise: model a small workflow with pending, accepted and rejected states, then write a match that displays each state. Add another state and review every consumer. Consult the [language reference](../../en/04-language-reference.md) for the exact enum and pattern forms. Next: [containers](11-containers.md).
