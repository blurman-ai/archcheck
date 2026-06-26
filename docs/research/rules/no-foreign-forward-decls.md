# no-foreign-forward-decls

- **Category:** S (AST / semantic)
- **Authority:** medium — Google C++ Style Guide
- **Source:** [Google C++ Style Guide — Forward Declarations](https://google.github.io/styleguide/cppguide.html#Forward_Declarations)

## Rule

> "Avoid using forward declarations where possible. Instead, include the headers you need. Do not use forward declarations of entities your project does not own."

## Why for archcheck

Forward-declaring a foreign type bypasses the include graph. The module formally does not depend on the foreign `.h`, but actually uses the foreign type. If the foreign project changes the layout / namespace / template parameters — our code silently breaks at link time or becomes UB. This is especially dangerous for types from `std::` (formal UB per `[namespace.std]` — see [forward-decl-of-std](forward-decl-of-std.md)).

## Detection

For each forward declaration (`class Foo;` or `struct Foo;` without a definition in the TU):
1. Determine the project's "own" namespace (from the config: `project_namespace: archcheck::`).
2. If the declared type does not belong to the project namespace — flag.

## Fixtures

- `pass/` — `class archcheck::ComponentGraph;` (own namespace).
- `fail_foreign/` — `class llvm::raw_ostream;` in our `.h`.
- `fail_std/` — `namespace std { class string; }` — flag separately as UB.
