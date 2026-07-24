# [SCAN] Window-free clone detection: remove dependence on fragment/token-window size

**Created:** 2026-07-24
**Start date:** 2026-07-24
**Status:** wip
**Module:** SCAN
**Priority:** major
**Difficulty:** large (design spike first, then core rewrite)
**Blocks:** #200 (min-size param becomes honest/monotonic only on a window-free core)
**Blocked by:** —
**Related:** #052/#056/#059/#083/#158/#190/#191/#195/#196 (all built on the fragment model), #198 (Duplo comparison = the motivating evidence), duplication_architecture.md (SSOT — must be updated)

## Problem

The token-pass clone detector fragments each function body into fixed token windows
(`minTokens`/`2×`/`4×` = 30/60/120, [fragmenter.h:38](../../include/archcheck/scan/duplication/fragmenter.h#L38))
and scores fragment-vs-fragment with weighted Jaccard. **The window size decides what
aligns**, so results depend on an internal implementation detail:

- Changing `minTokens` 30→50 is **non-monotonic**: some real clones vanish (a ~40-token
  clone falls below the floor), while *new* pairs appear (a ~55-token clone that straddled
  30-windows now fits one 50-window and aligns). Verified on our own `src/` (35→18 pairs,
  #35 `diff_query` 19-line clone lost, 2 new surfaced).
- The team already documented this boundary effect at [fragmenter.h `maxTokens` (#091)](../../include/archcheck/scan/duplication/fragmenter.h#L39):
  *"twin functions that straddle the cap fragment differently — one stays whole, the other
  subdivides — so the clone never aligns."*
- Consequence: clones look "tiny" and unstable; a user-facing "minimum clone size" knob
  can't be honest while the core re-fragments under it. This is a **root design flaw**, not
  a tuning problem — no threshold value fixes it.

The #198 comparison against Duplo (stable, line-based, monotonic in `-ml`) made this
concrete: low set-overlap and threshold-fragile output are symptoms of the window model.

## Goal

A clone-detection core whose results **do not depend on any internal window size**, so that
"minimum clone size" (#200) is a true, monotonic, understandable parameter.

## Direction (to be pinned in the spike)

Well-established window-free approaches (not our invention):

- **Maximal matching token runs** — suffix array / suffix automaton over the *normalized*
  token stream. Find the longest common token substrings directly, no windows. Identifier
  normalization → Type-2 (renamed) for free; Type-3 (gapped) by stitching adjacent exact
  runs. This is essentially how CCFinder / PMD-CPD / Duplo work; it makes min-clone-size a
  genuine post-filter with monotonic behavior.
- **AST subtree fingerprinting** (Deckard-style) — naturally window-free, but needs the
  parser → aligns with the planned v0.2 libclang backend. Consider as the semantic tier.

## The hard trade-off (the crux of the spike)

Our current differentiator — **typed STRUCTURAL / weighted-Jaccard "fuzzy similarity"** — is
exactly the part that lives on windows. A pure maximal-match core gives strong Type-1/2 but
not the fuzzy structural score in its current form. The spike MUST decide: drop STRUCTURAL,
keep a hybrid (maximal-match core + optional structural tier), or re-derive structural
similarity on a window-free basis. This changes the nature of the output and the marketing
framing — decide deliberately.

## Execution plan

- [ ] **Design spike / ADR** — problem statement (this), proposed window-free core, what we
      gain (window-independence, honest monotonic min-size, stability) vs lose (STRUCTURAL
      typing), and a migration plan. Do NOT start the rewrite before the ADR is accepted.
- [ ] Prototype the maximal-normalized-token-run detector on a few fixtures; confirm
      monotonicity in min-size and that the #198 witnesses (RENAMED, member-decl, cross-fn)
      behave sanely.
- [ ] Migration map: how #190/#191/#195/#196 (nested / statement-run / boundary / const-method
      classes) and #059/#083/#158 (coincidental filter / precision / FP reduction) translate
      — or become unnecessary — under the new core.
- [ ] Re-baseline the duplication fixtures consciously; update `duplication_architecture.md`
      (SSOT) in the same PR.

## Fixtures (if a rule)

- All existing `fixtures/dup*` / duplication fixtures must pass or be consciously re-baselined
  with the reason recorded. Add a monotonicity fixture: raising min-size yields a strict
  subset (the property the window model violates).

## Notes

- Keep the two-backend split (fast token pass vs libclang) — this rewrites the token pass core,
  not the backend architecture.
- #198's comparison numbers are a snapshot of the *current* (window) algorithm; they are the
  baseline the rewrite is measured against, not a claim about the future tool.
