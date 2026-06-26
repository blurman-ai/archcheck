# stable-but-concrete (SAP)

- **Category:** G (graph metrics)
- **Authority:** high — Robert C. Martin
- **Source:** Martin. *OO Design Quality Metrics* (1994); Stable Abstractions Principle. [PDF](https://linux.ime.usp.br/~joaomm/mac499/arquivos/referencias/oodmetrics.pdf)

## Rule

> "A package should be as abstract as it is stable. Stable packages should be abstract (so they don't lock in concrete implementation choices)."

## Why for archcheck

The "zone of pain": a module with low `I` (stable) + low `A` (concrete). A stable module is already hard to change, and if it is also concrete (lots of implementation classes, little pure abstract) — it literally cements the implementation that everyone depends on. This is the most expensive mistake of all the Martin metrics.

Requires no new data: `I` and `A` are already computed.

## Detection

For each module:
- `pain = (1 - I) * (1 - A)`.
- If `pain > 0.6` (configurable) — flag "SAP violation: stable + concrete (zone of pain)".

Additionally: compute `D = |A + I - 1|` (distance from the main sequence) and report the top-N modules with the worst D — this is already a standard Martin metric, but stable-but-concrete gives a more understandable phrasing of the cause in the report.

## Fixtures

- `pass_main_sequence/` — modules with `D < 0.3`.
- `fail_zone_of_pain/` — a module with `I=0.1`, `A=0.1` — stable and concrete.
- `pass_zone_of_uselessness/` — a module with `I=0.9`, `A=0.9` — unstable and abstract. Also not ideal, but a different pain; reported as a separate message, without `fail`.
