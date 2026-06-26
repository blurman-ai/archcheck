# no-protected-data

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG C.133 — Avoid `protected` data](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c133-avoid-protected-data)
- **Equivalent:** clang-tidy `cppcoreguidelines-non-private-member-variables-in-classes` (covers both public and protected)

## Rule

> "`protected` data is a source of complexity and errors. `protected` data creates a restricted public interface."

## Why for archcheck

A `protected` field is a public API for the entire inheritance hierarchy that is invisible to the outside world and therefore has no test coverage. At module boundaries this is especially dangerous: module B inherits a class from module A → A cannot change `protected` without coordination. This is a hidden binary dependency that is not present in the include graph.

## Detection

AST: for each `CXXRecordDecl`, walk the members, flag `FieldDecl`s with `protected` access. Ignore `static constexpr` fields.

## Fixtures

- `pass/` — all data `private`, accessed via protected/public getters.
- `fail_protected_int/` — `protected: int _count;`.
- `pass_protected_method/` — `protected: void DoStep();` (not data — allowed).
