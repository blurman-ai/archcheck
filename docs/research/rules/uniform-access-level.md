# uniform-access-level

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG C.134 — Ensure all non-`const` data members have the same access level](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c134-ensure-all-non-const-data-members-have-the-same-access-level)

## Rule

> "Prevention of logical confusion leading to errors. If the non-`const` data members don't have the same access level, the type is confused about what it's trying to do."

## Why for archcheck

A marker of a "god-class" / mixed concerns: some fields public (struct-like), some private (class-like). Such classes usually grow out of a POD onto which an invariant was bolted but never finished — that is, technical debt right at the module's responsibility boundary.

## Detection

AST: for each `CXXRecordDecl`, collect the access levels of all non-`const` non-static `FieldDecl`s. If the set of levels > 1 — flag.

## Fixtures

- `pass_struct/` — all fields `public` (POD).
- `pass_class/` — all non-const fields `private`.
- `pass_mixed_const/` — `public: const int kVersion = 1; private: int _state;` (const allowed publicly).
- `fail_mixed/` — `public: int x; private: int _y;` — both non-const, different levels.
