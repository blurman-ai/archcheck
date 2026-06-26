# god-namespace

- **Category:** G (graph metrics)
- **Authority:** medium — extension of Lakos fan-in + Martin Ce/Ca
- **Source:** Lakos *LSCSD* §3.10 (fan-in/fan-out), Martin OODQM (Ce/Ca). The specific rule is our aggregation.

## Rule

> "A namespace with afferent coupling (Ca) > 50 OR efferent coupling (Ce) > 30 is a candidate for splitting."

## Why for archcheck

god-headers are already in the spec (in-degree > 30 — split). This rule lifts the idea to the namespace level: a "god namespace" is a package that everything depends on (`Ca` off the charts) or that depends on everything (`Ce` off the charts). The former means "too much foreign code is bound to this layer" (a fragile stable module), the latter means "the layer can't work without all the others" (an aggregator module). Both are architectural smells, catchable using metrics that are already computed.

Requires no new data: Ce/Ca are already computed for Martin metrics.

## Detection

After building the namespace-level graph:
- For each namespace, compute `Ce` and `Ca`.
- Flag if `Ca > threshold_ca` (default 50) **or** `Ce > threshold_ce` (default 30).
- Both thresholds are configurable.

## Fixtures

- `pass_balanced/` — a namespace with `Ca=10, Ce=5`.
- `fail_high_ca/` — `utils::` with `Ca=80` (everything depends on it).
- `fail_high_ce/` — `application::` with `Ce=40` (depends on everything).
