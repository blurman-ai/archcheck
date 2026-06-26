# no-recursion-across-modules

- **Category:** S/G (AST + graph)
- **Authority:** medium — extension of clang-tidy
- **Source:** clang-tidy `misc-no-recursion` (basis for the call-graph detector); archcheck-specific part — mapping onto modules

## Rule

> "No call-graph recursion that crosses module boundaries."

## Why for archcheck

Ordinary recursion within a module is fine. Recursion whose cycle crosses ≥ 2 modules (`A::foo() → B::bar() → A::baz()`) means a hidden bidirectional dependency — even if the includes look like a DAG, the runtime call-graph is closed. Often a sign that a layer was cut carelessly and two parts of the modules are really one component. archcheck already builds the component graph — adding a call-graph on top gives an additional signal.

## Detection

1. AST pass: build the call-graph `FunctionDecl → FunctionDecl` (Clang `CallGraph`).
2. Project the vertices onto modules (via path → module mapping).
3. Find strongly-connected components in the projection; for each SCC of size > 1 — flag as "inter-module recursion: A ↔ B".

Optional, not a default: expensive, requires libclang and the full AST.

## Fixtures

- `pass_intra_module/` — `module_a::factorial()` recursively calls itself.
- `pass_dag/` — `A::foo() → B::bar()`, without a back-call.
- `fail_cross_module/` — `A::foo() → B::bar() → A::baz() → A::foo()`.
