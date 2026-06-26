# no-complex-global-init

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG I.22 — Avoid complex initialization of global objects](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i22-avoid-complex-initialization-of-global-objects)

## Rule

> "Complex initialization can lead to undefined order of execution."

## Why for archcheck

A direct path to the static initialization order fiasco. This is not a logic bug — it's an architectural flaw: a global depends on another global from a neighboring module, and the initialization order is unpredictable across TUs. Often manifests as "sometimes it works, sometimes it crashes at startup" and almost always falls neatly into the category of inter-module bugs.

## Detection

AST pass: a global with an initializer that:
- calls a non-`constexpr` function, or
- reads another global, or
- allocates memory (`new`, container operations).

Allowed: literal init, a `constexpr` constructor, `{}`-aggregate initialization of a POD.

## Fixtures

- `pass/` — `inline constexpr int kMax = 42;`, `inline const std::string_view kName = "x";`.
- `fail_function_call/` — `inline std::vector<int> g_data = ComputeData();`.
- `fail_cross_global/` — `inline int g_b = g_a + 1;` where `g_a` is declared in another TU.
