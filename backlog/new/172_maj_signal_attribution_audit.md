# [RULES] Audit attribution gaps in advisory signals (authority over opinion)

**Created:** 2026-07-03
**Start date:** —
**Status:** new
**Module:** RULES
**Priority:** maj
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #169 (openwiki — see docs/openwiki/backlog/unverified.md)

## Goal

Reconcile the advisory scan signals with the project's "authority over opinion"
principle: every one either gets a Core Guidelines / Lakos / Martin citation, or is
explicitly reclassified as a non-authority-backed (Level 4 / advisory-only) check.

## Context

Surfaced while building the openwiki (#169). Writing one page per signal forced the
question "what authority backs this?", and several signals turned out to carry **no
formal Core Guidelines / Lakos / Martin citation** — only an issue anchor. Per
CLAUDE.md: *"Every default rule must cite Core Guidelines / Lakos / Martin. If a
proposed default rule has no citation, it goes under Level 4 ('indisputable practices')
or doesn't ship as a default."* So these are not necessarily wrong to have — they just
must not masquerade as authority-backed defaults.

Signals flagged `unverified` for attribution (from the fan-out; see
`docs/openwiki/backlog/unverified.md` and the feature pages):

- **SATD** (`satd_scan`, #096) — no named authority.
- **test co-evolution** (`test_co_evolution`, #097) — no named authority.
- **flag-argument `ARG.2.boolean_literal_call`** — `ARG.1` is attributed (Core
  Guidelines F.21 + Martin), but `ARG.2` specifically has no separate citation.
- **new-clone drift** (`DRIFT.NEW_CLONE`, #123/#149) — issue anchors only.
- **local-complexity drift** — the *metric* cites Sonar / clang-tidy cognitive
  complexity, but the *drift* signal has no Core Guidelines / Lakos / Martin cite.
- **history analytics** (`SIZE.1.god_file_growth`, `HIST.1.defect_attractor`) — no
  formal citation.
- **local-include gate** (`CASE_MISMATCH_INCLUDE`, #168) — framed as "a real build
  break on case-sensitive Linux" (`README.md:43`), not an authority citation.

## Execution plan

- [ ] For each signal: search for a legitimate authority (Core Guidelines / Lakos /
      Martin / a citable empirical source) and add it, OR
- [ ] reclassify it explicitly as Level 4 / advisory-only / not-a-default, and make
      that classification visible where the signal is described (CHANGELOG / spec /
      the signal's openwiki page).
- [ ] Confirm none of these is silently presented to users as an authority-backed
      default (check README gate table + `--help` framing).
- [ ] Update the corresponding `docs/openwiki/` pages + `backlog/unverified.md`; bump
      `last_checked_commit`.

## Done
- (empty)

## In progress
- (empty)

## Next steps
1. Triage the list above into "has an authority we can cite" vs "advisory-only by
   nature", one row at a time.

## Key decisions

| Decision | Reason |
|---------|---------|
| Audit + reclassify, not delete | Signals can be legitimately useful without a Core Guidelines/Lakos/Martin citation — they just belong at Level 4 / advisory, not as authority-backed defaults (CLAUDE.md "authority over opinion"). |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |
