# archcheck v0.1 — MVP status & remaining gap

_Snapshot 2026-06-20. Reconciles `docs/MVP.md` acceptance criteria (last dated
2026-06-11) against what has shipped since (CHANGELOG `[Unreleased]`), the corpus
re-measure of 2026-06-19 (#131), and the external-oracle clone validation (#132).
Authoritative scope source stays `docs/MVP.md`; this is the current-state delta._

## TL;DR

**9.5 of 10 acceptance criteria are met.** The only open criterion is **#10
(copy-paste not hidden)**, and even there the *code* shipped — `--diff` now surfaces
copy-paste a commit introduces (#123). What remains is **validation**, not feature
work: prove the new-clone gate's precision before publishing.

The single remaining MVP work-item is the **clone-gate validation trilogy**:

| task | what | status | role |
|---|---|---|---|
| **#123** | 10-commit control set (5+/5−), local + GitHub test repo | **Stage 1 ✅ (10/10, 2026-06-20)**; Stage 2 (GitHub) remains | acceptance proof for the `--diff` entry point |
| **#103** | per-commit copy-paste precision measured on the corpus | wip (phases 1–5 done; presentation numbers move to product run) | "is the gold real" — precision baseline |
| **#124** | corpus-wide `archcheck --diff` fire-rate on real clone-commits (1.05M commits / 1685 repos) | wip (resumable run in progress; 2 product bugs found+fixed) | recall/precision at scale for the gate decision |

Plus one go-public polish item: **#133** (`--diff` mass-rename guard — ~19% false
alarm on large refactors; check-mode advisory-first already shipped and corpus-confirmed).

Everything else on the MVP list is shipped and verified.

## Acceptance criteria — current state

| # | criterion | MVP.md (06-11) | now (06-20) | evidence |
|---|---|---|---|---|
| 1 | cold start: dozens not thousands, seconds, zero-config | ✅ | ✅ (caveat) | #131 re-measure: 1689 repos, median ≪1 s, DCMTK 2196 files = 0.46 s. Authored repos report dozens. **Caveat:** repos that are themselves vendored mirrors / bundled kernel headers still over-report SF.8 (see Gaps) |
| 2 | no known coarse-FP classes; each has fix+fixture | ✅ | ✅ | #131: no authored-detect regression after #127/#128/#129 |
| 3 | legacy freeze (`--save-baseline`/`--baseline`) | ✅ | ✅ | regression-tested |
| 4 | drift gate regression-only | ✅ | ✅ | live on LibreSprite PR #581 |
| 5 | deterministic output | ✅ | ✅ | CI requirement |
| 6 | honest exit-code contracts | ✅ | ✅ | exit 0/1/2/3 reachable; malformed YAML → 2 |
| 7 | fixtures per shipped rule | ✅ | ✅ | 8/8 rules; duplication advisory + unit-tested |
| 8 | safe on untrusted repos (S3–S6) | ◑ #105 | ✅ | **#105 closed** (commit cb6e09d): symlink-escape, 64 MiB cap, RFC 8259 JSON, git hardening |
| 9 | dogfood green in CI | ✅ | ✅ | own src/include/tests gated |
| 10 | copy-paste not hidden in `--diff` | ○ open | ◐ **code shipped; Stage-1 validated** | #123 `--diff` surfacing shipped; **Stage-1 control set 10/10 (06-20)**. Open: #123 Stage-2 (GitHub live), #103 precision, #124 fire-rate |

Also shipped since 06-11 (not new criteria, but de-risk the first-run experience):
**#133** check-mode advisory-first — a bare `archcheck` exits 1 only on SF.9 cycles;
ChainLength/GodHeader/SF.7-8 are advisory. Confirmed live by the #131 run: of 1689
repos, exactly the 505 with include cycles gate (exit 1); a header-heavy lib no longer
fails with a wall.

## What this session contributed to the MVP picture

### #132 — external-oracle validation of the *detector* (snapshot precision)
Strengthens the evidence base under criterion 10's premise ("a *validated* detector").
The gate (#103/#124) measures per-commit fire-rate; #132 independently validates the
underlying detector quality:
- **NiCad/monit (pure C, NiCad's home turf):** agree on every real cross-file clone
  above ~80 %; 0 genuine recall bugs. NiCad ≠ ground truth (it over-enumerates benign
  twins we suppress by design).
- **POJ-104 (Type-4 FP boundary):** 1950 programs / 39 582 candidates → 15 pairs, all
  same-problem textual clones, **0 cross-problem false positives**. We correctly do
  not flag Type-4.
- **Corpus FP sweep (18 repos, 8094 pairs):** danger-zone (w 0.51–0.55) all genuine
  duplicates, **0 false positives** beyond LibreSprite.
- Full write-up: `reports/clone_detection_comparison.md`.

This does **not** replace #103/#124 (those measure the per-commit *gate*), but it
removes doubt about the detector the gate sits on.

### #131 — corpus re-measure of the *graph/SF rules* (criteria 1, 2, 9, and #133)
- No authored-detect regression after #127/#128/#129 classification changes.
- #133 advisory-first confirmed at scale (505/1689 gate on real cycles only).
- **Two gaps surfaced** (quality, not MVP blockers — see below).
- Full write-up: `experiments/corpus_remeasure_131/FINDINGS.md`.

## Gaps surfaced this session (quality backlog, not MVP blockers)

1. **#127 vendored-exclusion misses two cases** — (a) repos that are *themselves*
   vendored mirrors (`openharmony_third_party_musl` SF.8=1166, gnulib mirrors, Android
   kernel) — the `third_party/`-segment detector doesn't fire when the whole repo is
   vendored; (b) bundled kernel/stdlib headers inside a real repo (bpftrace
   `src/stdlib/include/linux/`). These inflate SF.8 on a minority of repos. Touches
   criterion 1 (first-run noise) for those repos. Actionable: whole-repo-vendored
   heuristic + bundled-kernel-header path rule.
2. **Corpus-selection noise (#122 criteria gate)** — pure-vendored mirrors and
   ports/data trees (`FreeBSD-Electron` SF.8=9704 in `devel/electron*/files/`) skew any
   aggregate; they should be filtered at corpus intake, not by the scanner.
3. **@generated bindings** — auto-generated binding boilerplate (`tulpa
   RcppExports.cpp`) is real duplication but a candidate for the generated-exclusion
   layer if it carries a banner (relates to #127/#131).

None of these block v0.1: they affect a minority of vendored-natured repos, not the
authored-code signal the MVP is graded on.

## The road to v0.1 (ordered)

1. **#123 control set** — **Stage 1 done (10/10 local, 2026-06-20)**: all 5 positives
   fire, all 5 negatives (incl. parent-guard, move, below-threshold) stay silent.
   Remaining: Stage 2 (GitHub test repo, push-triggered workflow — outward-facing) and,
   optionally, port the control set to a committed Catch2 E2E.
2. **#103 precision numbers** — finish the per-commit copy-paste measurement on the
   product `archcheck --diff` run (phases 1–5 done; needs presentation-grade numbers).
3. **#124 fire-rate** — let the resumable corpus run accumulate coverage; it yields the
   precision/recall the gate decision rests on. *Long pole (run in progress).*
4. **#133 polish** — `--diff` mass-rename guard (~19% false alarm on large refactors)
   before the public announcement.
5. *(post-MVP, quality)* — #127 vendored gaps (1) and corpus-intake filter (2).

When 1–4 land, all ten MVP acceptance criteria hold and v0.1 is publishable. Item 5 is
a quality follow-up, explicitly out of MVP scope.
