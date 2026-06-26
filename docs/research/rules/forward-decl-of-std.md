# forward-decl-of-std

- **Category:** S (AST) or R (text pattern)
- **Authority:** high — C++ standard (`[namespace.std]`)
- **Source:** C++ standard §16.4.5.7 [namespace.std]; mentioned in the archcheck spec as an "indisputable practice" without an explicit rule ID.

## Rule

> "It is undefined behavior to add declarations or definitions to namespace `std` or to a namespace within namespace `std`, with limited exceptions (template specializations of certain library templates)."

## Why for archcheck

Forward-declaring types from `std::` is formal UB per the standard. People often write `namespace std { class string; }` to avoid `#include <string>` in a header, but this violates `[namespace.std]`: the implementation may declare `string` as a typedef, a template, or something inside an inline namespace, and linking silently breaks or behavior drifts. archcheck must catch this as a security-critical pattern, because the compiler usually stays silent.

## Detection

Two options:
1. **AST (reliable):** find a `NamespaceDecl` with `getName() == "std"` in a project file (not in an STL header) that contains any declarations. Flag everything inside it. Exception: template specializations via `template<> ...` — allowed by the standard.
2. **Text pattern (fast):** regex `\bnamespace\s+std\s*\{` in project files. Cheap, may produce false negatives on `namespace ::std`.

In the fast backend — R (regex); in the clang backend — S (AST, more precise).

## Fixtures

- `pass_specialization/` — `template<> struct std::hash<MyKey> { ... };` (allowed).
- `pass_no_std/` — no declarations in `std`.
- `fail_forward_class/` — `namespace std { class string; }` in `foo.h`.
- `fail_forward_struct/` — `namespace std { struct vector; }`.
