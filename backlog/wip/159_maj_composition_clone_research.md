# [SCAN][DUPLICATION] Composition/API-choreography clone research

**Created:** 2026-07-01
**Started:** 2026-07-01
**Status:** wip
**Module:** SCAN / DUPLICATION
**Priority:** major
**Complexity:** M
**Blocks:** possible future precision guard for #158 residual idiom/composition noise
**Blocked by:** —
**Related:** #158 (clone-detector FP reduction), #131 (fresh corpus remeasure),
#103 (copy-paste precision baseline), #070 (FP guards), #056/#072 (duplication detector)

> **RESUME HERE.** This is a research task, not a pre-approved product fix. Work autonomously:
> write temporary probes, run them, inspect real code examples, adjust, rerun, and leave a
> reproducible analysis. Do not ship a generic suppression guard unless the corpus evidence is
> clean enough and fixtures prove it.

## Scope clarification 2026-07-01

User clarified the first pass: do not run per-commit verification yet. The OSS corpus is available
at `~/oss`; for now check snapshot presence of function-composition patterns in
projects: functions made mostly from the same top-level call choreography with different domain
arguments.

## Why this exists

During #158 residual-FP review, the user raised a specific hypothesis:

```cpp
void f1()
{
  helper1(1, 2, 3);
  helper2(4, 5);
  helper3(7, 8);
}

void f2()
{
  helper1(5, 6, 7);
  helper2(11, 12);
  helper3(5, 4);
}
```

The detector may report this as copy-paste because the call choreography is the same after
normalization, but it may be legitimate composition/API usage: same sequence of operations over
different domain values, not a duplicated algorithm body that should be merged.

This is distinct from ordinary copy-paste with renamed variables. The task is to find whether a
real, suppressible class exists in the corpus, and whether it can be separated from true copy-paste
without losing important findings.

## Current evidence from #158

Temporary probes already tried on `experiments/corpus_remeasure_131/group3_findings_INLINECASE.jsonl`:

- generic call/callee LCS + argument overlap:
  - strict-ish configs removed FP only with equal TP collateral (`FP 1 / TP 1`, `FP 2 / TP 2`,
    `FP 4 / TP 5`, etc.).
- top-level call-statement choreography (`callee(...);` / `obj.callee(...);`):
  - `strict`: `FP 0 / TP 0`
  - `base`: `FP 2 / TP 0`
  - `loose`: `FP 3 / TP 3`
  - `many`: `FP 2 / TP 0`

But the actual base/many hits were not clean suppressible composition:

- `viperx1/Usagi-dono`: manager boilerplate around `QSqlDatabase`, `QSqlQuery`, `LOG`, `q.exec`.
  The user classified it as useful copy-paste/advisory.
- `xsscx/research`: heuristic-library diagnostic/check blocks. The user classified it as useful
  copy-paste/advisory.
- `FULL-FIRMWARE menu_led_control`: TFT slider drawing chain. It is composition-like, but the user
  also classified it as copy-paste/repeated UI drawing and the loose guard had TP collateral.

Conclusion so far: the signal is real, but the first generic thresholds are not safe.

## Research question

Can we identify a narrow, defensible class of composition/API choreography clones where:

- the normalized structure matches mainly because arguments inside calls were ignored or generalized;
- the fragments are dominated by top-level calls into an API/builder/DSL rather than local algorithmic
  control/data transformation;
- the call sequence is intentionally repeated with different domain arguments;
- suppressing or down-ranking these findings improves precision without unacceptable TP loss.

If the answer is "no", document that clearly with examples and leave no product change.

## Required datasets

Use at least:

- Group-3 labelled corpus current state:
  `experiments/corpus_remeasure_131/group3_findings_INLINECASE.jsonl`
- The TSV labels:
  `experiments/verification/fp_corpus_r2.tsv`
- Current matcher:
  `experiments/corpus_remeasure_131/match.py`

Then broaden if needed:

- `experiments/ai_repo_run/*_dup.txt` for raw detector output examples.
- Current `--duplication` runs on selected local repos under `~/oss`.
- Real snippets from repositories, not summaries. When reporting examples in chat or task notes,
  show full fragments up to 10 lines; if longer, state total line count and show the most relevant
  call sequence.

## Suggested temporary probe

Create a temporary script under `experiments/corpus_remeasure_131/`, for example
`composition_argstrip_probe.py`. It may be gitignored or committed only if it becomes useful
documentation.

The probe should:

1. Pull witness findings via `match.py`.
2. Extract both code spans from local git repos.
3. Build at least two views:
   - **arg-stripped view:** replace call arguments with placeholders, e.g.
     `helper1(ARG); helper2(ARG); helper3(ARG);`
   - **argument view:** preserve the argument token sets/sequences for each matched call.
4. Compare:
   - top-level call sequence LCS;
   - proportion of statements that are calls;
   - proportion of matched calls whose callee is the same but argument tokens differ;
   - argument overlap, both raw and after dropping local variable names;
   - shared non-call substantive lines;
   - control-flow density (`if/for/while/switch/return` vs plain calls).
5. Print ranked candidates with:
   - repo/sha/files/ranges;
   - corpus label and class;
   - current detector score if available;
   - metrics;
   - full call-sequence snippet.

Important: do not only count. Every promising bucket must be backed by real code examples.

## Hypotheses to test

### H1 — Argument-stripped clones reveal composition candidates

Run a mode that finds pairs whose arg-stripped call sequence is highly similar while argument
payloads differ substantially. Check whether these are real composition or just copy-paste with
renamed constants.

Expected risk: many UI/SQL/diagnostic blocks are still useful copy-paste findings.

### H2 — API/DSL family restriction may separate better than generic calls

Composition may concentrate in fluent/builder/drawing/plotting/menu APIs:

- Qt/QString/style builders;
- ImGui/ImPlot;
- TFT/rendering/menu drawing;
- SQL/schema setup;
- command/menu registration tables;
- diagnostic/reporting pipelines.

Measure these as named families, but do not hard-code a library-specific product rule without
evidence. A future product rule must be phrased as a general structural signal, not "skip ImGui".

### H3 — Non-call substance is the discriminator

A suppressible composition candidate should have little shared non-call logic. If both fragments
also share assignments, loops, branches, data transformations, or magic thresholds, it is more likely
copy-paste.

Test thresholds like:

- high top-level call density;
- low shared non-call line count;
- low control-flow density;
- low local computation token overlap outside call arguments.

### H4 — Down-rank may be safer than drop

If composition candidates are useful but lower priority, evaluate down-ranking / tagging instead of
suppression. The public product may tolerate "composition-like" advisory findings if they are rare
and explainable.

## Mandatory output

Produce an analysis section in this task file with:

- commands run;
- scripts/probes used;
- corpus numbers for each tested config:
  - FP removed;
  - TP collateral;
  - recall;
  - precision;
  - suppression;
- at least 10 real candidate examples, grouped as:
  - clean suppressible composition;
  - copy-paste despite same call chain;
  - ambiguous / user-review-needed;
  - TP collateral controls.
- final decision:
  - **ship guard** with exact rule and fixtures; or
  - **ship tag/down-rank only**; or
  - **do not ship**, document as future research.

## Acceptance if a product change is proposed

- [ ] The rule is narrow and structural, not library-name-specific.
- [ ] It has unit tests and duplication fixtures:
      `fixtures/duplication/composition/pass_*` and `fail_*`.
- [ ] It is measured on Group-3 with the corrected matcher.
- [ ] It does not regress #103 precision beyond an explicitly accepted threshold.
- [ ] It does not hide user-classified useful copy-paste examples from #158:
      `Usagi-dono`, `xsscx/research`, `FULL-FIRMWARE menu_led_control`, `NexusMiner`.
- [ ] Dogfood stays green.
- [ ] `lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/`
      has no new warnings.

## Do not do

- Do not add a broad "same callees, different args => suppress" guard.
- Do not tune thresholds only against labels; inspect the witness code.
- Do not trust cropped snippets. Show enough code to classify the example.
- Do not split this into another task until the first autonomous research pass has real results.
- Do not commit experimental product code unless it has fixtures and measured tradeoff.
