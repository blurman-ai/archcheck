# Review of the local_complexity_drift scorer: where it measures something other than complexity

_2026-06-11. Subject: `experiments/local_complexity_drift/scan_commit.py` (prototype #102,
candidate for #101). Each defect is reproduced by a live `--old-file/--new-file` run;
commands at the bottom. Verdict: the direction is right (per-function, diff-based, author
filters), but the current formula measures "lines with control words × indentation", not
the complexity of understanding — and systematically penalizes the most readable C++
constructs._

## The formula right now

```
score = branch_score + deep_lines + floor(indent_sum / 20)
branch_score(line) = |if,for,while,switch,catch,do,case,default| × (1 + active_depth)
                   + |break,continue,goto,co_await| + |&&| + |‖| + |?|
```

The reference I compare against: Sonar Cognitive Complexity (Campbell 2018) — a public
specification, implementable on tokens without an AST; it's exactly what the CMU study
(arXiv 2511.04427) uses, which the task's motivation rests on.

## Confirmed defects (in descending order of harm)

### D1. A flat switch explodes: 0 → 19 where cognitive complexity gives 1

An 8-case dispatcher (`case N: return fN();`) — **scorer: 19 and a `zero_to_complex`
finding; Sonar: +1** (a switch counts as one structure, cases are free; that's the whole
idea of cognitive complexity: a switch is more readable than the equivalent if-else
ladder). Here, instead, every `case` is a control-hit × (1 + depth), and inside a switch
depth ≥ 1, i.e. each case costs ≥2. Consequence for the corpus: the top findings
(`process_connection` 2184→2486, `decodeOSC`, `translateToPlainText`, `afp_over_dsi`) —
are protocol/parser switch machines; they dominate not because they became less
understandable, but because they have many case lines. **The scorer measures the size of
the dispatch, not complexity.**

A mirror-image skew: `else` isn't counted at all (Sonar: +1), so an if/else-if ladder is
cheaper than a switch — the metric encourages rewriting a switch as else-if, which is
contrary to common sense and to this-very-metric.

### D2. The `&&` of rvalue references is counted as branching

`Item&& item`, `auto&& slot` — +1 to branch_score for each (repro: 0→1 on a function
without a single branch). On modern-C++ code (perfect forwarding, `if constexpr` traits,
move overloads) this is a systematic positive shift, correlated with style, not with
complexity. Sonar, moreover, counts a **series** of identical operators as +1, not each
operator (`a && b && c && d` = +1, ours = +3).

### D3. Aligning line continuations = fake depth

`indent_level = leading_spaces // 4`, and `max(indent_level, active_depth)` goes into
deep_lines/indent_sum. Two flat calls with arguments aligned at the bracket
(clang-format `AlignAfterOpenBracket`) give 0→4 on code **without a single branch or
nesting** (repro below): the continuations of `registry.bind(...)` at 16 spaces — that's
indent_level 4 → "deep lines". The flip side: **tabs aren't stripped** (`lstrip(" ")`),
so for tab-indented projects the indent component is always 0. Result: the contribution of
indentation depends on the repository's formatting style → cross-repo comparisons (which is
the goal of the corpus run) are incomparable. The hard `// 4` is additionally wrong for
2-space projects (including archcheck itself).

### D4. do-while costs 2–3 points for one loop

`do` and its `while` are matched by CONTROL_RE independently (repro: 0→3 on a trivial
do-while). Sonar: +1.

### D5. Absolute thresholds on a metric proportional to size

`finding_reason`: `delta >= 5` — absolute. On a function with score 2184 any cosmetic
patch yields a finding; 477 of 524 corpus findings are exactly `complexity_delta`. The
scale itself is a sum of three different units (weighted control-hits + a line counter +
indentation area/20), so no single threshold makes sense simultaneously for score-10 and
score-2000 functions. Normalization is needed: relative growth with a minimal absolute (as
already done in `relative_complexity_delta`, but it almost never fires now — 12/524), or
score/meaningful_loc, or a logarithm (CMU uses a log-transform + project aggregation; Sonar
— a per-function threshold of 15).

### D6. Test code leaks through, and TEST_F matching compares unrelated bodies

The filter cuts only the `{test,tests,unittest,unittests}` directories and the
`*_test/_tests/_spec` stems: `source/EngineTests/ConstructorTests.cpp` passes (the
normalized segment `enginetests` doesn't equal `tests`) — in the top-20 of the corpus
report there are two `TEST_F` findings. Worse: for all `TEST_F(Suite, Name)` the symbol is
`TEST_F`, and overload matching takes the "nearest by line" → the bodies of **different**
tests get compared (part of the 43 low-confidence matches). It's cheaply fixable: a
suffix-match of segments (`*tests`), a blacklist of symbols
`TEST/TEST_F/TEST_P/TYPED_TEST/BENCHMARK`, and including the parameter arity in the symbol
key.

### D7. Overloads are matched "by the nearest line"

For C++, overloads of one name are the norm; nearest-start-line after a big edit pairs up
the wrong pairs and gives birth to phantom deltas (low-confidence, but findings aren't
excluded from them). The minimal fix — account for the number of top-level parameters in
the key.

## What's good in the scorer (verified and worth keeping)

- `strip_comments_and_literals` is correct, including raw strings; **a pure reformat
  (wrapping a long condition over 5 lines) doesn't change the score** (4→4) — that's a strong
  property.
- A data table (a multi-line aggregate-init) does **not** generate false complexity (0→0).
- Author filters (vendored dirs/stems/license headers) — a reasonable first line.
- The design itself — "function × diff of two revisions × git show" — is right and cheap.

## Recommendation for #101/#102

Don't invent a scale — take the **Sonar Cognitive Complexity specification** (it's
token-implementable, there's a public white paper and it's also what CMU uses, i.e. results
will be comparable with the literature):

1. +1 per structure: `if`, `else`/`else if`, `?:`, `switch` (not per-case!), `for`, `while`,
   `do-while` (once), `catch`, `goto`; keep `co_await` as our +1 — debatable, but cheap.
2. Nesting increment (+depth) — only from **control structures and lambdas**, counted as a
   stack of those structures, not bracket/indent depth. Remove indentation from the formula
   entirely (it remains in #099 as a separate file-level fallback — then the tasks don't
   overlap).
3. A series of identical `&&`/`||` = +1; `&&` immediately after an identifier/`>` in a
   declaration context — filter out with a heuristic (or honestly accept a small error, but
   then at least the series, not each operator).
4. Finding: `delta_percent >= X` with `delta >= Y` and `new_score >= Z` (relative growth
   with a minimal absolute and a floor), plus a separate `zero_to_complex` with a threshold
   on the cognitive scale (25 — the default for Sonar's C-family and clang-tidy). The full
   signal design — [cognitive_complexity_delta_design.md](cognitive_complexity_delta_design.md).
5. D6/D7 fixes for the test filter and the symbol key.
6. Re-validate on the same 1614 commits: expectation — the top stops consisting of
   switch parsers and TEST_F, and the 6/6 TP of the manual labeling (examples doc) are
   preserved — they're all about real growth of branching/nesting, which the cognitive scale
   also sees.

## Repro

The pair files are saved in the repo: `experiments/local_complexity_drift/review_repros/`
(`/tmp/lcd_test/` — volatile, the commands below work with the in-repo copies too).

```bash
P=experiments/local_complexity_drift/scan_commit.py
# D1: flat 8-case switch: 0 -> 19, FINDING zero_to_complex (Sonar: 1)
# D2: rvalue &&: 0 -> 1 without branching
# D3: aligned arguments: 0 -> 4 on flat code
# D4: do-while: 0 -> 3 for one loop
python3 $P --old-file /tmp/lcd_test/a_old.cpp --new-file /tmp/lcd_test/a_new.cpp
python3 $P --old-file /tmp/lcd_test/b_old.cpp --new-file /tmp/lcd_test/b_new.cpp
python3 $P --old-file /tmp/lcd_test/d_old.cpp --new-file /tmp/lcd_test/d_new.cpp
python3 $P --old-file /tmp/lcd_test/e_old.cpp --new-file /tmp/lcd_test/e_new.cpp
# control (preserve behavior): reformat 4 -> 4; data-table 0 -> 0
python3 $P --old-file /tmp/lcd_test/f_old.cpp --new-file /tmp/lcd_test/f_new.cpp
python3 $P --old-file /tmp/lcd_test/c_old.cpp --new-file /tmp/lcd_test/c_new.cpp
```

Related: the prototype's corpus report (`local_complexity_drift_corpus_report.md`) already
recommends `revise` — this review makes concrete **what exactly** to revise; the manual
labeling examples (`local_complexity_drift_examples.md`, 6/6 TP) will remain TP on the
cognitive scale too.

## Resolution (2026-06-11, v2 scorer)

All seven defects are closed by rewriting `scan_commit.py` to a Sonar Cognitive Complexity
token scanner (design — [cognitive_complexity_delta_design.md](cognitive_complexity_delta_design.md) §4/§6).
Verified on the repro pairs `review_repros/`:

- **D1** (flat switch): `a` is now 0→1, no finding (was 0→19).
- **D2** (rvalue `&&`): `b` delta 0 — a `&&` glued to a token (`Item&&`, `auto&&`) isn't logical.
- **D3** (aligned continuations): `d` delta 0 — indentation removed from score entirely.
- **D4** (do-while): `e` 0→1 (was 3) — the trailing `while` isn't counted.
- **D5** (absolute threshold on a size metric): the score is now cognitive — linear code = 0 by
  construction; thresholds are the hierarchy LCX.1 `crossed_25` / LCX.2 `grew_when_already_above` / LCX.3 `Δ>=5`.
- **D6** (TEST_F): a blacklist of symbols `TEST*/BENCHMARK` + a suffix filter for `*tests` directories →
  0 TEST findings in the corpus (was 2 in the top-20).
- **D7** (overloads): top-level arity in the matching key → low-confidence dropped 43→21.

Control: `f` (condition reformat) 2→2, delta 0 — a multi-line series of `&&` is now counted
once (a lastOp stack by bracket depth). Synthetic suite 13/13, the 6/6 manual TP are
preserved after the corpus re-run (`local_complexity_drift_corpus_report.md`).
</content>
