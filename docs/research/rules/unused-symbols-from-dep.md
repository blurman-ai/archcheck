# unused-symbols-from-dep (CRP)

- **Category:** S (AST / semantic)
- **Authority:** high — Robert C. Martin
- **Source:** Martin. *OO Design Quality Metrics* (1994); Common Reuse Principle. [principles-wiki](http://principles-wiki.net/principles:common_reuse_principle)

## Rule

> "The classes in a package are reused together. If you reuse one of them, you reuse them all. Don't depend on packages whose symbols you don't use."

## Why for archcheck

If module X includes a header from module Y but actually uses <20% of the symbols declared there, X depends on Y at too coarse a granularity. Any change in the unused part of Y still triggers a rebuild of X. This lowers the real transitive cohesion and diverges from what the user agreed to.

Borders on IWYU but does not duplicate it: IWYU minimizes includes at the TU level, CRP looks at module-level granularity.

## Detection

For each include `X → Y`:
1. Collect the public symbols of Y (from the public `.h`).
2. Collect the symbols from Y used in X (via clang `MatchFinder` or USR cross-reference).
3. If `used / declared < threshold` (default 0.2) — flag.

An expensive check (AST + symbol resolution). Optional, not an MVP default.

## Fixtures

- `pass/` — X uses ≥ half of Y's public symbols.
- `fail_overbroad/` — Y declares 20 classes, X uses 1.
- `pass_typed_facade/` — Y is a facade with a single function, X uses it. Nothing extra; the low used/declared is not flagged because declared is small.
