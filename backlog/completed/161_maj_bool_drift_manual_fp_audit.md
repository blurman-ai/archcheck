# [SCAN][DRIFT] Bool-field drift manual FP audit and example catalog

**Created:** 2026-07-01
**Started:** 2026-07-02
**Status:** done 2026-07-02 — 53 findings audited; post-fix re-run FP 7→1 (2%), 98% factual precision; doc: docs/research/bool_field_drift_manual_audit.md
**Module:** SCAN / DIFF / BOOL_FIELD_DRIFT
**Priority:** major
**Complexity:** M
**Blocks:** #160 final scanner evidence base
**Blocked by:** —
**Related:** #089 (boolean state drift research), #090 (metric), #134 (full corpus run),
#135 (per-commit column), #160 (scanner evidence base)

## Goal

Manually verify what `DRIFT.BOOL_FIELD_ACCRETION` catches in real commits and build a documented
TP/FP example set. The question is not "does the parser run"; it is whether the signal is useful
and explainable to users.

## What to verify

For each sampled finding, inspect the actual before/after code and classify:

- **TP:** a type accumulates boolean state / mode flags / feature toggles in a way that suggests
  state-space growth or illegal-state risk.
- **USEFUL_ADVISORY:** config/options structs or UI state where the bools are real but less
  architectural.
- **FP:** parser/matching/classification bug, generated/vendor/test leak, rename/move artifact,
  or a field that is not actually newly accumulated state.
- **MIXED:** a commit with both legitimate boolean-state growth and noisy side findings.

## Data sources

Use:

- #134 outputs and `docs/research/boolean_state_perstruct_drift_fullcorpus.md`;
- native `archcheck --diff` runs for commits with `DRIFT.BOOL_FIELD_ACCRETION`;
- `~/oss` local clones.

If existing artifacts are stale, rerun a targeted sample rather than trusting the old report.

## Required output

Create or update:

```text
docs/research/bool_field_drift_manual_audit.md
```

Include:

- commands used;
- sample-selection method;
- at least 30 manually reviewed bool-drift findings;
- at least 12 full before/after code examples;
- FP taxonomy with concrete examples;
- final recommendation:
  - keep as-is;
  - tune reporting/wording;
  - add suppression/fix tasks;
  - or downgrade visibility.

## Checks to perform

- [ ] Verify generated/vendor/test exclusion on sampled findings.
- [ ] Check rename/move commits separately from real field accumulation.
- [ ] Separate config-bag bools from domain-state bools.
- [ ] Check whether repeated additions across several commits are more convincing than one-shot
      additions.
- [ ] Confirm the public message does not overstate the finding: it should say "added boolean
      state", not claim a bug.

## Acceptance

- [x] 30+ findings manually classified.
- [x] 12+ real code examples recorded.
- [x] Precision estimate and FP taxonomy written down.
- [x] Any concrete product bug found has a follow-up task or a fix with tests (#164).
- [x] Results are linked from #160.

## Do not do

- Do not count config/options bags as parser FP; classify them separately.
- Do not infer intent from names alone; inspect the surrounding type and commit diff.
- Do not change thresholds without a measured before/after and examples.

## Checkpoint — 2026-07-02 autonomous run

Artifact created: `docs/research/bool_field_drift_manual_audit.md`.
Run log submitted: `docs/research/scanner_advisory_runlog_2026_07_02.md`.

Commands / verification:

- `cmake --build build/debug` → `ninja: no work to do`;
- `./build/debug/src/archcheck --version` → `archcheck 0.1.5`;
- `cd build/debug && ctest --output-on-failure` → 616/616 passed;
- parsed `experiments/per_commit/results_full.boolrule.jsonl`;
- live `--diff --format=json` on `EricLeeFriedman_CopilotChess`
  `d19b8246d1322e2cf0c89f6d62217371dc098f75`.

Corpus summary from the native per-commit artifact:

- 517,975 OK diff rows;
- 10,735 rows with `DRIFT.BOOL_FIELD_ACCRETION`;
- 787 repos with bool-field findings;
- 17,510 added bool fields reported;
- 13,315 struct events reported.

Live current-binary reproduction:

- `src/moves.h:15`: `struct 'Move' accreted 1 bool field(s): is_castling`;
- the same commit also reports 3 LCX findings and 2 new-clone findings in
  `src/moves.cpp`, making it a good cross-signal example.

Evidence carried forward from #134:

- 499 structs accreted bools across at least 4 commits;
- split: 335 content-drift, 163 config-bag, 1 churn;
- top-15 eye-check: about 11 clean TP, 2 demo/example, 2 borderline-config.

Status: not complete against this task's stricter acceptance target. The next
step is to add 30 explicit per-finding verdict rows and 12 before/after examples
to the research doc, reusing the live corpus rows rather than only the older
aggregate.

## Completed — 2026-07-02 fresh-sample audit (second autonomous pass)

The acceptance gap above is closed. `docs/research/bool_field_drift_manual_audit.md`
now contains the full audit:

- **53 findings** from 45 distinct repos (seed=159 stratified sample from
  `results_full.boolrule.jsonl`), each re-produced with the CURRENT binary
  (`--diff --diff-mode=memory`) and classified by reading the before/after struct.
- Verdicts: **TP 26 (49%) / USEFUL_ADVISORY 20 (38%) / FP 7 (13%)** — the scanner's
  factual claim held in 87%.
- **12 full before/after examples** in the doc (xiaozhi Ota, ShapeWorks flag pair,
  MaaFramework 7-bool state machine, btop const-bool FP, mvfst brace-init FP, ...).
- FP taxonomy with root causes verified against scanner source; three parser gaps
  confirmed (`const bool`, `bool x{false};`, and the NanaBox case — root-caused later
  the same night: the method-decl paren guard rejected lines whose TRAILING COMMENT
  contains parentheses), plus two scope gaps (`fmt` vendor, `testlib` test dir).
- **Same-night follow-up:** all of the above FIXED (#164 A.1-A.5/B.1-B.3) with unit
  tests; identical sample re-run: FP 7→1 (2%), 98% factually correct, plus recovered
  real findings (stochtree brace-init fields, CopilotChess GameState castling bools).
  See the audit doc's "Post-fix re-run" section.
- Two-reviewer control: 2 bool findings classified twice blind, verdicts matched.
- Concrete product bugs filed as **#164** (fix pack with reproducers).
- Recommendation: ship as advisory with neutral wording; results linked from #160.

All acceptance boxes are satisfied: 30+ findings (53), 12+ examples (12), precision
estimate + FP taxonomy written, product bugs have a follow-up task (#164), linked
from #160. **Ready to move to completed/ on user confirmation.**

## How it works

<!-- TODO -->

## What controls it

<!-- TODO -->

## What it relates to

<!-- TODO -->

## Diagnostics

<!-- TODO -->
