# transitive-fan-in

- **Category:** G (graph metrics)
- **Authority:** high — Lakos
- **Source:** Lakos. *Large-Scale C++ Software Design* §3.10; Component Dependency (CD).

## Rule

> "A component with transitive fan-in > 50% of the system size — flag."

## Why for archcheck

Complements god-headers (direct fan-in) and the CD metric (own CD). If a component is transitively included by > half of all components in the project, any edit to it triggers a rebuild of half the system — and any bug in it touches half the system. This is an early warning about rebuild-storm and about over-coupled common code. Computed directly from the already-built graph (transitive closure → in-degree).

## Detection

After building the component graph:
1. Compute the transitive closure (reverse reachability) for each node.
2. For each component `C`, count `transitive_fan_in(C) = |{X : X transitively depends on C}|`.
3. If `transitive_fan_in(C) / N > 0.5` (where N is the total number of components) — flag. Threshold configurable.

It's worth reporting the top-5 by transitive fan-in even without crossing the threshold — it's a useful metric for understanding.

## Fixtures

- `pass_balanced/` — 20 components, maximum transitive_fan_in = 6 (30%).
- `fail_god_component/` — 20 components, `core.h` is transitively pulled in by 15 (75%).
- `pass_small_project/` — 5 components: 50% is 2-3 components, which is normal for a small project (the rule is disabled when `N < min_N`, default 10).
