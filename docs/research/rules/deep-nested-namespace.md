# deep-nested-namespace

- **Category:** P (preprocessor scan) or R (text pattern)
- **Authority:** medium — Google C++ Style Guide + common practice
- **Source:** [Google C++ Style Guide — Namespaces](https://google.github.io/styleguide/cppguide.html#Namespaces); Lakos *LSCSD* §2.4 (physical packaging).

## Rule

> "Namespace nesting depth > 4 levels — flag."

## Why for archcheck

A simple sprawl detector. `archcheck::rules::core_guidelines::sf::detail::` — five levels — is a signal that the package hierarchy has gotten out of hand, or that instead of physical separation (via directories / components) people are trying to do logical separation via nested namespaces. No deep architectural meaning, but cheap and catches a real anti-pattern.

## Detection

Preprocessor scan: for each file, track the maximum nesting depth of `namespace X { namespace Y { ... } }`. Count the C++17 compact form (`namespace X::Y::Z {`) as depth 3. If > threshold (default 4) — flag. Threshold configurable.

Ignore anonymous namespaces in the count (they are structural, not semantic).

## Fixtures

- `pass_shallow/` — `namespace archcheck::rules { ... }`.
- `pass_compact_form/` — `namespace archcheck::rules::lakos { ... }` (3 levels).
- `fail_deep/` — `namespace a::b::c::d::e::f { ... }` (6 levels).
- `fail_traditional/` — five nested `namespace X {` blocks.
