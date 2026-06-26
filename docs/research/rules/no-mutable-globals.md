# no-mutable-globals

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG I.2 — Avoid non-`const` global variables](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i2-avoid-non-const-global-variables)
- **Equivalent:** clang-tidy `cppcoreguidelines-avoid-non-const-global-variables`

## Rule

> "Non-`const` global variables hide dependencies and make them subject to unpredictable changes."

## Why for archcheck

Global mutable state breaks modularity: any module can read/write it, and the dependency graph lies — formally there is no dependency, but in fact it exists through the global. A direct architectural defect, not style.

## Detection

AST pass: a `VarDecl` in namespace or file scope (not `function-local`, not a `class member`), type not `const`-qualified, not `constexpr`/`constinit`. Ignore `inline constexpr` constants — they are allowed.

## Fixtures

- `pass/` — only `const` / `constexpr` / `constinit` globals.
- `fail_mutable/` — `int counter = 0;` in namespace scope.
- `fail_static_storage/` — `static int counter;` in a `.cpp` (whether to flag this too — decide at implementation time).
