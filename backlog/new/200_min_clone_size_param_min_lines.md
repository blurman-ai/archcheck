# [SCAN] User-facing minimum clone size: `--min-lines` (and `--min-tokens` as internal/advanced)

**Created:** 2026-07-24
**Start date:** 2026-07-24
**Status:** new
**Module:** SCAN
**Priority:** major
**Difficulty:** small-medium (interim output filter) → clean once #199 lands
**Blocks:** —
**Blocked by:** partially #199 (the parameter becomes *intrinsic + monotonic* only on a
window-free core; can ship as an output post-filter before then)
**Related:** #198 (comparison surfaced the gap), #199 (window-free core), `docs/config_format.md`

## Problem

`archcheck --duplication` has **no CLI parameters at all** — only `<path>`. The one threshold
that exists, `minTokens` (30), is an **internal token-window** size ([fragmenter.h:38](../../include/archcheck/scan/duplication/fragmenter.h#L38)),
the wrong abstraction to expose: nobody outside the project cares about our window size.

What users understand and expect (every peer tool has it — Duplo `-ml`, PMD `--minimum-tokens`,
Simian threshold) is a **minimum clone size**, in **lines**. Today we can't offer even that.

## Goal

Expose a user-facing **minimum clone size in lines**:

- **`--min-lines N`** (CLI) + **`min_lines:` under a `duplication:` section** in `.archcheck.yml`.
- Applied as an **output filter**: drop any reported clone pair whose span is `< N` lines. A
  filter on the *result set*, not a re-fragmentation → **monotonic and understandable** (raise
  N ⇒ strict subset), regardless of internal windowing.
- Keep the internal token window **internal**. If a token knob is ever exposed, it is an
  *advanced/hidden* `--min-tokens` for power users, never the primary parameter. Do NOT surface
  the window as the user-facing control.
- Default: an honest small value (~5–6 lines) that suppresses trivial 3-line matches (e.g. the
  self-clone `path`+two `findFile` we flag on our own `src/`) without hiding real copy-paste.

## Interim vs post-#199

- **Now:** ships as a post-filter over the current (window) algorithm — immediately makes the
  tool configurable and drops trivial noise. It hides, but does not remove, the window
  dependence underneath.
- **After #199 (window-free core):** min-clone-size becomes *intrinsic* to detection (maximal
  matches ≥ N), so the same knob is honest end-to-end and the internal token window disappears
  from the story. Land the CLI/config surface now; let #199 make it principled.

## Execution plan

- [ ] Add `min_lines` to the duplication options struct + wire `--min-lines N` in the CLI.
- [ ] Apply as the final output filter in the reporter/scanner (drop pairs with max-span
      `< N` lines); document that it is a result filter.
- [ ] Add `duplication: { min_lines: N }` to config loader + `docs/config_format.md`.
- [ ] Keep `minTokens` internal; if surfaced at all, hide behind an advanced flag with a
      "not the size you want — see --min-lines" help note.

## Fixtures (if a rule)

- `fixtures/dup_min_lines/pass/` — clones both above and below N; assert only ≥N survive.
- Monotonicity fixture: running at N and N+k yields a strict subset (guards against a future
  regression re-coupling the param to windowing).
