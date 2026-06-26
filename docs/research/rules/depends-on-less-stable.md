# depends-on-less-stable (SDP)

- **Category:** G (graph metrics)
- **Authority:** high — Robert C. Martin
- **Source:** Martin. *OO Design Quality Metrics* (1994); Stable Dependencies Principle. [PDF](https://linux.ime.usp.br/~joaomm/mac499/arquivos/referencias/oodmetrics.pdf)

## Rule

> "Depend in the direction of stability. A package should only depend upon packages that are more stable than it is."

## Why for archcheck

A direct graph check on top of the already-computed *Instability* `I = Ce / (Ca + Ce)` (low I = stable, high I = unstable). If module X with `I=0.2` depends on module Y with `I=0.8`, X drags along an unstable (frequently changing) Y — any change in Y will cascade and churn X. This is a basic layered-architecture mistake.

Requires no new data: `I` is already computed for Martin metrics.

## Detection

For each direct dependency `X → Y`:
- Compute `I_X` and `I_Y`.
- If `I_X < I_Y` — flag "SDP violation: stable X depends on less stable Y".

An optional tolerance threshold (`I_Y - I_X > 0.2`) to avoid noise on borderline cases.

## Fixtures

- `pass_layered/` — domain (`I≈0`) ← application (`I≈0.5`) ← ui (`I≈1`). Each depends downward — toward stability.
- `fail_inverted/` — domain depends on ui (classic inverted layering).
- `pass_tolerance/` — both ≈0.5, the difference is below the threshold, not flagged.
