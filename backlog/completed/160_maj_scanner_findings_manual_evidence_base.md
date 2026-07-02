# [SCAN][RESEARCH] Manual evidence base for scanner findings before public push

**Created:** 2026-07-01
**Started:** 2026-07-02
**Status:** done 2026-07-02 — 145-finding fresh evidence base consolidated; per-signal verdicts in docs/research/scanner_findings_manual_evidence.md; fixable FP classes fixed same day (#164 A/B)
**Module:** SCAN / DIFF / RESEARCH
**Priority:** major
**Complexity:** M
**Blocks:** public confidence in advisory scanner output
**Blocked by:** —
**Related:** #158 (clone FP reduction), #159 (composition research), #161 (bool drift audit),
#162 (local complexity audit), #103 (copy-paste precision), #119 (unified per-commit drift)

## Goal

Build a documented, eyes-on evidence base for what `archcheck` actually catches today:

- representative true positives;
- representative false positives;
- ambiguous but useful advisory findings;
- known noisy classes that should stay open as follow-up tasks.

This is not a new rule implementation task. It is the final manual audit layer before a public
push: the project needs real code examples and defensible FP/TP judgement, not only aggregate
numbers.

## Scope

Cover the scanner/advisory findings that can appear in `--diff` or scanner-driven checks:

- `DRIFT.NEW_CLONE`;
- `DRIFT.BOOL_FIELD_ACCRETION`;
- `DRIFT.LOCAL_COMPLEXITY`;
- SATD/test co-evolution/flag-argument if they appear in the same sampled commits;
- duplication `--duplication` snapshot findings where they are relevant to #158/#159.

Deep dives for bool drift and local complexity are split into #161 and #162. This task is the
cross-signal index and final narrative.

## Required output

Create or update a documentation artifact, for example:

```text
docs/research/scanner_findings_manual_evidence.md
```

The document must contain:

- methodology: repos/commits selected, commands used, binary/version/commit hash;
- a table of at least 50 findings reviewed by eye;
- per-finding verdict: `TP`, `FP`, `MIXED`, `USEFUL_ADVISORY`, `UNKNOWN`;
- root-cause class for every FP;
- at least 20 full code examples, with both before/after or both clone sides when applicable;
- a short public-facing conclusion: which signals are reliable enough to show, which are
  advisory-only, and which still need follow-up.

## Sampling plan

Use a mix of:

- recent corpus runs already in `experiments/`;
- targeted local runs on `~/oss` repos;
- examples from completed validation tasks (#103, #134, #145, #158);
- fresh runs for any signal whose examples are stale or too synthetic.

Do not rely only on labels. For every example, open the actual code.

## Acceptance

- [x] A research doc exists with commands, sample selection, and verdict table.
- [x] At least 50 findings are manually classified (145 fresh: 53 bool + 92 LCX).
- [x] At least 20 real code examples are included or linked with enough context.
- [x] Bool drift and local complexity rows link to #161/#162 results.
- [x] All material FP classes either have an open task (#164/#159/#153) or are explicitly accepted as advisory noise.
- [x] No product code is changed unless a concrete bug is found and covered by tests (none changed; bugs go through #164).

## Do not do

- Do not massage metrics without examples.
- Do not show cropped fragments that hide why a verdict was made.
- Do not mark a signal "public-ready" if the FP class is only guessed, not inspected.

## Checkpoint — 2026-07-02 autonomous run

Artifact created: `docs/research/scanner_findings_manual_evidence.md`.
Run log submitted: `docs/research/scanner_advisory_runlog_2026_07_02.md`.

Commands / verification:

- `cmake --build build/debug` → `ninja: no work to do`;
- `./build/debug/src/archcheck --version` → `archcheck 0.1.5`;
- `git rev-parse HEAD` → `4a38bde2af2c3fa9f1c1a023bd45a33374fb7674`;
- `cd build/debug && ctest --output-on-failure` → 616/616 passed;
- live `--diff --format=json` on `EricLeeFriedman_CopilotChess`
  `d19b8246d1322e2cf0c89f6d62217371dc098f75`;
- live `--diff --format=json` on `AlchemyViewer_Alchemy`
  `fc56e0801fabc19d514d8c877050d0feb00b7de5` and
  `9a6ace99ded620a0f30a549c2f68153cee89daf2`.

Current evidence index:

- `DRIFT.NEW_CLONE`: #103 manual triage, 70 findings, precision 86-91%;
- `DRIFT.BOOL_FIELD_ACCRETION`: #161 checkpoint, 10,735 per-commit findings in
  787 repos from `experiments/per_commit/results_full.boolrule.jsonl`;
- `DRIFT.LOCAL_COMPLEXITY`: #162 checkpoint, fresh top-100 artifact has 931
  finding commits / 2,623 violations; old manual `triage.tsv` has 167 TP,
  15 MIXED, 7 historical FP;
- live cross-signal example: CopilotChess reports bool + LCX + new-clone in one
  commit;
- live LCX TP: AlchemyViewer AVIF commit reports `LLLocalBitmap::decodeBitmap`
  23 -> 26;
- live historical LCX FP recheck: AlchemyViewer `9a6ace99de` no longer emits LCX
  with the current binary.

Status: this is a useful evidence-base checkpoint, not task completion. The
remaining work is to consolidate at least 50 manually classified rows and at
least 20 full code examples into the research doc.

## Completed — 2026-07-02 fresh-sample consolidation (second autonomous pass)

`docs/research/scanner_findings_manual_evidence.md` is now the consolidated index:

- **145 findings manually classified in this pass** (53 bool + 92 LCX, fresh
  current-binary samples across 100 distinct repos) — full verdict tables live in the
  #161/#162 audit docs, which the index links; duplication evidence carried from
  #158/#103 (306-row labelled corpus + 82-fire eyeball triage + user's residual review).
- **24 full code examples** across the two audit docs + live cross-signal examples
  (CopilotChess bool+LCX+clone; AlchemyViewer AVIF TP; AlchemyViewer fixed-FP recheck).
- Per-signal public-readiness table: new-clone / bool accretion / local complexity all
  **ship as advisory** with ~87-90% factually-correct claims; SATD/test/flag stay
  advisory but out of launch materials until sampled the same way.
- Every material FP class has an open task (#164 scope/parser fix pack, #159 incidental
  clones tag candidate, #153 vendor exclusion) or is explicitly accepted (mega-import
  floods, vendor-mirror repos → corpus/showcase gate).
- No product code changed in this task (bugs go through #164 with fixtures).

All acceptance boxes satisfied: doc with commands/sampling/verdicts, 50+ findings
(145), 20+ examples (24 + live), #161/#162 linked, FP classes tasked or accepted.
**Ready to move to completed/ on user confirmation.**

## How it works

<!-- TODO -->

## What controls it

<!-- TODO -->

## What it relates to

<!-- TODO -->

## Diagnostics

<!-- TODO -->
