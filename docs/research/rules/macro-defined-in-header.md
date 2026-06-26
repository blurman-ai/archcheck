# macro-defined-in-header

- **Category:** S (AST / semantic) or P (preprocessor scan)
- **Authority:** medium — C++ Core Guidelines + common practice
- **Source:** [CCG ES.30 — Don't use macros for program text manipulation](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es30-dont-use-macros-for-program-text-manipulation), [CCG ES.31 — Don't use macros for constants or "functions"](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es31-dont-use-macros-for-constants-or-functions).

## Rule

> "Macro definitions in headers leak into every translation unit that includes the header. Either guard with `#undef` at end of header, or don't use a macro at all."

## Why for archcheck

A macro in a header is a name leak that obeys neither namespace nor access control. A single `#define MAX 100` in `foo.h` touches every `.cpp` that transitively includes it, and breaks a completely unrelated class's `enum { MAX };`. This is an architectural smell at the boundary of two modules.

It borders on style (present in clang-tidy / cppcheck), so it is **under a flag, not a default** — but it has architectural meaning, unlike other macro rules.

## Detection

Preprocessor scan (preferred — no libclang):
1. For each `.h`, find `#define`.
2. Check for a matching `#undef` in the same file before the closing `#endif` of the include guard.
3. If there is no `#undef` — flag it.

Exceptions:
- include guards themselves (`#define FOO_H` paired with `#ifndef FOO_H`),
- macros whose name starts with a given prefix (e.g. `ARCHCHECK_*`) — these are the project's public API,
- definitions marked with the comment `// archcheck: allow-macro <reason>`.

## Fixtures

- `pass_no_macro/` — header with no `#define` (except the guard).
- `pass_undef_at_end/` — `#define HELPER ...` and a matching `#undef HELPER` before `#endif`.
- `pass_public_api/` — `#define ARCHCHECK_VERSION_MAJOR 1` (our public prefix).
- `fail_leaking/` — `#define MAX 100` without `#undef`.
