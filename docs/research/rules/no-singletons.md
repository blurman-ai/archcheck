# no-singletons

- **Category:** S (AST / semantic)
- **Authority:** high — C++ Core Guidelines
- **Source:** [CCG I.3 — Avoid singletons](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i3-avoid-singletons)

## Rule

> "Singletons are basically complicated global objects in disguise."

## Why for archcheck

A singleton is a global disguised as an object. It creates hidden inter-module dependencies: `Foo::Instance()` is called by anyone from anywhere, and that is invisible in the graph. An architectural anti-pattern of exactly the kind archcheck should catch.

## Detection

AST-pattern:
- a class with a private/deleted constructor,
- a static method `Instance()` / `GetInstance()` / `instance()` (names configurable),
- inside the method — `static T instance;` or `static T* instance = new T;`.

Confidence level — heuristic, not 100%. Behind the flag `--enable=I.3`, not a default.

## Fixtures

- `pass/` — ordinary classes without a static `Instance()`.
- `fail_classic/` — canonical Meyer's singleton.
- `fail_pointer/` — singleton via `new` in a static-local.
- `pass_false_positive_candidate/` — a class with a public constructor and a convenient static `Default()` function — not a singleton, do not flag.
