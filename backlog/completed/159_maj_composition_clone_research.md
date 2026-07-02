# [SCAN][DUPLICATION] Composition/API-choreography clone research

**Created:** 2026-07-01
**Started:** 2026-07-01
**Status:** done 2026-07-02 — research complete: no product change; class named (accidental/API-protocol clones); 4 shape signals null; v0.2 tag candidate documented in docs/duplication_architecture.md §5.y
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

## Analysis 2026-07-01 — snapshot presence pass, no per-commit verification

User clarification: use the local OSS corpus if needed, but do not run per-commit verification yet.
This pass checks only whether function-composition patterns exist in current project snapshots.

Commands:

```bash
python3 experiments/corpus_remeasure_131/composition_probe.py group3_findings_INLINECASE.jsonl
python3 experiments/corpus_remeasure_131/composition_snapshot_probe.py \
  --max-files-per-repo 500 --max-pairs-per-repo 0 --top 100000 \
  --out-md experiments/corpus_remeasure_131/composition_snapshot_full.md \
  --out-jsonl experiments/corpus_remeasure_131/composition_snapshot_full.jsonl
```

Probes:

- `composition_probe.py` — existing ignored Group-3 witness probe; re-run as a control.
- `composition_snapshot_probe.py` — temporary ignored snapshot probe. It scans `~/oss`
  current trees, skips build/vendor/vendored/test directories and test-like filenames, extracts
  C/C++ functions heuristically, and keeps pairs where:
  - both functions have at least 3 top-level call statements;
  - call density is at least 0.75;
  - the normalized top-level callee sequence is identical;
  - argument-token Jaccard is at most 0.45.
- Full-run navigation artifacts:
  - `experiments/corpus_remeasure_131/composition_snapshot_full.jsonl` — all retained pair records.
  - `experiments/corpus_remeasure_131/composition_snapshot_full.md` — all retained pairs with snippets.
  - `experiments/corpus_remeasure_131/composition_snapshot_full_index.md` — compact index.
  - `experiments/corpus_remeasure_131/composition_snapshot_repo_summary.tsv` — repo-level summary.
  - `experiments/corpus_remeasure_131/composition_snapshot_sequence_summary.tsv` — sequence-level summary.

Control on `group3_findings_INLINECASE.jsonl` reproduced the earlier warning: generic call-sequence
thresholds still trade FP removals for TP collateral.

```text
minCalls=4 calleeLcs>=0.80 argOverlap<=0.35 family>=0.60: FP 1 / TP 1
minCalls=5 calleeLcs>=0.85 argOverlap<=0.65 family>=0.60: FP 2 / TP 2
minCalls=5 calleeLcs>=0.85 argOverlap<=0.75 family>=0.60: FP 4 / TP 5
minCalls=6 calleeLcs>=0.85 argOverlap<=0.75 family>=0.70: FP 3 / TP 3
```

Full snapshot numbers over the local OSS corpus:

```text
repos_scanned=2042
repos_with_pairs=991
pairs_retained=90020
unique_call_sequences=2975
unique_repo_sequence_combinations=4019
files_scanned=504412
functions_extracted=4939360
```

The run used `--max-files-per-repo 500` as a runtime cap, but disabled the pair cap
(`--max-pairs-per-repo 0`). Therefore this is "all retained examples found under the scan cap",
not a mathematical exhaust of every file in very large repos.

Heuristic category sketch from the full index:

| Category | Pairs |
|---|---:|
| critical-section / generated-like | 35765 |
| other | 28433 |
| assert / test-like | 9863 |
| status/error-chain | 5727 |
| registration/table | 4879 |
| hardware/io | 2156 |
| print/log | 2149 |
| ui-binding | 797 |
| serialization/walk | 209 |
| sql/binding | 42 |

Representative composition-like examples found in real projects:

| Class | Example |
|---|---|
| registration table | `BearWare_TeamTalk5` `SoundEventsModel` / `StatusBarEventsModel`: 31 `push_back(...)` calls, args overlap 0.0 |
| MFC/UI data binding | `CUBRID_cubrid` `VAS.CPP` / `WAS.CPP` `DoDataExchange`: `CDialog::DoDataExchange` + 15 `DDX_Control(...)`, args 0.016 |
| AST visitor field walk | `CUBRID_cubrid` `pt_apply_delete` / `pt_apply_spec`: 15 `PT_APPLY_WALK(...)`, args 0.143 |
| hardware register setup | `78_xiaozhi-esp32` board `Pmic` constructors: 13 `WriteReg(...)`, args 0.0 |
| UI toggle composition | `AlchemyViewer_Alchemy` `selectAllTypes` / `selectNoTypes`: 13 `setValue(...)`, args 0.0 |
| Qt translation/UI text setup | `Cockatrice_Cockatrice` `retranslateUi` / `retranslateUi`: `setTitle` + 11 `setText`, args 0.2 |
| telemetry/fact registration | `CubePilot_qgroundcontrol-herelink` fact groups: `_addFact(...)` + `setRawValue(...)`, args 0.0 |
| binding/init mirrors | `203-Systems_MatrixOS` PikaObj populate/init: `obj_setInt/obj_setStr` field sequence, args 0.1 |
| JSON serialization | `AB-lab113_hidering` `toJsonValue` overloads: `StartObject` + `INSERT_INTO_JSON_OBJECT`, args 0.045 |
| explicit serialization chains | `Arsia-Mons_Silencer` `Serialize` overloads: 10 `Serialize(...)`, args 0.045 |
| SQL statement binding | `0xV4h3_cpp-atlas` `completeSession` / `writeJournalEntry`: `prepare` + `bindValue`, args 0.0 |
| status/error propagation idiom | `AOMediaCodec_iamf-tools` `ReadStreamInfo` / `ReadAndValidate`: 8 `RETURN_IF_NOT_OK(...)`, args 0.097 |

Interpretation:

- The class exists. Repeated function-composition/API-choreography bodies are common enough to show
  up in 991/2042 snapshot repos under the current scan cap.
- The class is mixed. The same signature covers clean composition, data/registration tables,
  serializer/visitor boilerplate, status-macro chains, and likely real copy-paste with edited
  payloads.
- The full count is dominated by generated/SDK-like macro ceremony (`*_CRITICAL_SECTION_ENTER`,
  GLEW info dumps, generated layout printers) and by assert/error macro chains. That reinforces
  the need for manual class labels before any product rule.
- A generic "same callees, different args" suppression would be unsafe. Group-3 still shows TP
  collateral; the snapshot examples also include extractable/review-worthy cases such as settings
  save chains and SQL binding chains.

Current decision: **do not ship a guard**. This pass proves presence, not suppressibility. If this
continues, the safer product direction is a `composition-like` tag/down-rank candidate, then only
after a labelled sample separates UI/registration/binding composition from real copy-paste.

## Analysis 2026-07-01 — composition-percent gate recalculation

User corrected the classifier target: the interesting class is not ordinary copied blocks with a
few changed literals, but blocks made mostly of repeated API calls where all or almost all calls
have different arguments. Manual classifications from the discussion:

- `FLOX-Foundation_flox` `o.Set(...)` object population: **not copy-paste**, clean composition.
- `MUME_MMapper` `connect(...)` setup: **not copy-paste**, composition, even if accidental.
- `CUBRID_cubrid` `DDX_Control(...)`: copy-paste-ish, but structurally hard to fix; do not treat as
  a clear "bad duplicate" control.

Recalculation command:

```bash
python3 experiments/corpus_remeasure_131/composition_gate_recalc.py \
  --min-call-density 0.20 --min-diff-arg-ratio 0.50 --max-control-line-ratio 0.30 \
  --thresholds 0.10 0.15 0.20 0.25 0.30 0.35 \
  --out-md experiments/corpus_remeasure_131/composition_gate_recalc_full.md \
  --out-labelled-jsonl experiments/corpus_remeasure_131/composition_gate_labelled_full.jsonl \
  --out-snapshot-jsonl experiments/corpus_remeasure_131/composition_gate_snapshot_full.jsonl
```

Metric used:

```text
composition_percent =
  aligned same-callee calls with different argument tokens
  / average statement count of the two matched blocks
```

The sweep gate also required `matched_calls >= 3`, `callee_lcs >= 0.8`,
`call_density >= 0.2`, `diff_arg_ratio >= 0.5`, and `control_line_ratio <= 0.3`.

Labelled Group-3 baseline:

```text
labels=311 (143 TP, 168 FP)
fired_labelled_rows=76 (58 TP, 18 FP)
measurable_fired_witness_pairs=236
snapshot_pairs_with_recomputed_metrics=89382
```

Gate sweep on labelled Group-3 rows:

| Composition threshold | Suppressed rows | FP removed | TP collateral | Remaining precision | Remaining recall |
|---:|---:|---:|---:|---:|---:|
| 0.10 | 6 | 3 | 3 | 0.786 | 0.385 |
| 0.15 | 6 | 3 | 3 | 0.786 | 0.385 |
| 0.20 | 3 | 1 | 2 | 0.767 | 0.392 |
| 0.25 | 2 | 1 | 1 | 0.770 | 0.399 |
| 0.30 | 2 | 1 | 1 | 0.770 | 0.399 |
| 0.35 | 0 | 0 | 0 | 0.763 | 0.406 |

The top labelled examples explain why the percentage alone cannot be a suppress guard:

- `FULL-FIRMWARE-Coche-Marcos` TFT slider drawing is the highest FP
  (`composition_percent=0.333..0.345`), but it is exactly the kind of repeated UI drawing that a
  reviewer may still want to see as copy-paste/advisory.
- `esrrhs/fakelua` benchmark wrappers are TP at the same strength
  (`composition_percent=0.333`): repeated benchmark bodies with only backend/test names changed.
- `aethersdr/AetherSDR` JSON/theme serialization is TP collateral at
  `composition_percent=0.230`.
- `Usagi-dono` SQL-manager boilerplate and `NexusMiner` recovery/logging blocks appear in the low
  threshold bucket; the latter is simultaneously represented as a labelled TP control, so suppressing
  it would hide a user-classified real duplicate.

Snapshot top-10 by composition percent is dominated by GLEW info dumps and generated/table-like
registration/check functions (`glewInfoFunc`, `Add`, `CHECK_MEMBER`, `LIB_FUNCTION`) with
`composition_percent ~= 0.96..0.99`. These are good examples of "composition exists", but poor
evidence for a product suppressor because many are generated, SDK-like, or table declarations.

Additional sensitivity checks:

```text
strict: callDensity>=0.50, diffArgRatio>=0.75, control<=0.20
  -> 0 FP removed / 0 TP collateral at all tested thresholds

medium: callDensity>=0.50, diffArgRatio>=0.50, control<=0.30
  -> 1 FP removed / 1 TP collateral until threshold 0.30, then 0/0 at 0.35

diff-heavy: callDensity>=0.20, diffArgRatio>=0.75, control<=0.30
  -> 1 FP removed / 1 TP collateral at thresholds 0.10..0.15,
     then 0 FP / 1 TP at 0.20, then 0/0
```

Updated decision: **do not ship suppression**. A composition percentage is useful as a diagnostic
feature and maybe as a future `composition-like` tag/down-rank input, but by itself it does not
separate clean composition from true copy-paste. The first removals already include TP collateral,
and the highest-score snapshot examples are dominated by generated/table-like code.

## Analysis 2026-07-02 — probe-defect audit, v2 re-measure, ubiquity signal (all verified against fresh corpus run)

User hypothesis for this pass: maybe the composition *detector* (the probes) was written
wrong, and maybe the *definition* itself was wrong. Both were tested.

### Fresh corpus verification first

`group3_precision.py` + `rescore.py` re-run from scratch on the current binary: recall
58/140 = 41.4%, suppression 148/166 = 89.2%, precision 58/76 = 76.3% — exactly the #158
closing numbers. The baseline is sound.

### Probe-defect audit — the v1 probes WERE defective

Confirmed parsing defects in `composition_gate_recalc.py` / `composition_snapshot_probe.py`:

1. **Fluent/member chains missed entirely.** The call regex allowed at most one `.m`/`->m`
   segment and no chaining, so `obj.setX(1).setY(2);` and `ui->label->setText(...);`
   parsed as nothing. This is exactly the Qt-builder class the task flagged as the future
   signal.
2. **Assignment forms missed.** `auto rc = f(x);` was invisible to the snapshot probe
   (no assignment prefix at all) and only `x = f();` worked in the recalc probe.
3. **Template callees missed** (`f<T>(x);`).
4. **Argument tokens did not separate domain payload from shared locals.** Shared local
   names (`theme`, `c`, `q`) inflated argument overlap for genuine composition — the
   documented Lightpad failure.
5. **Snapshot probe grouped only EXACT callee sequences** — near-composition with one
   inserted call was invisible.
6. **Snapshot corpus polluted by vendor/generated** (`lib/glew`, Atmel `hri/`) that the
   real product's AuthoredScope would exclude — the 90 020-pair count and the category
   table overstate what archcheck itself would flag.

### v2 probe — defects fixed, verdict UNCHANGED

`composition_v2_probe.py` (same dir): chain-aware parser (10/10 sanity cases), payload
divergence computed over literals/`UPPER_CASE`/`kCamel` constants only, plus a
corpus-wide callee ubiquity signal. Materially different measurement: 69 witnesses newly
measurable, 111/219 common witnesses matched MORE calls, 67 changed composition% by >0.02.

Gate sweep on fresh `group3_findings.jsonl` — numerically identical outcome to v1:

```text
thr 0.10: suppressed 6 rows -> FP 3 / TP 3
thr 0.20: suppressed 3 rows -> FP 1 / TP 2
thr 0.25: suppressed 2 rows -> FP 1 / TP 1
thr 0.35+: 0 / 0
```

Same rows as v1 top the metric (NexusMiner FP+TP, AetherSDR TP, fakelua TP ×4,
FULL-FIRMWARE FP, Usagi-dono FP). **The "do not ship suppression" decision was NOT an
artifact of the buggy parser — it is now confirmed by two independent probe
implementations.** TP and FP interleave at every threshold; the best FP (TFT sliders) sits
at the same composition strength as real TP (fakelua benchmark bodies).

### Callee-ubiquity signal — REFUTED (wrong sign)

Hypothesis (literature-inspired): calls that appear in many unrelated repos = API
protocol/idiom → composition FP. Built a real document frequency for all 277 witness
callees over the whole local OSS corpus (`build_callee_df.sh`, ripgrep pass, 270 callees
resolved). Result:

```text
ubiq_df>=20:  FP median ubiquitous-call ratio 0.250 (mean 0.374)
              TP median ubiquitous-call ratio 0.600 (mean 0.542)
ubiq_df>=50:  FP 0.000/0.329  TP 0.600/0.531
ubiq_df>=150: FP 0.000/0.297  TP 0.500/0.489
```

TP witnesses are MORE ubiquitous than FP — the opposite of the hypothesis. Real
copy-paste in this corpus is built from common calls (`memcpy`, `push_back`, `prepare`),
while the residual idiom FPs use niche APIs (TFT drawing, `hri_*` register ceremony).
Cross-corpus callee frequency cannot down-rank composition. (Consistent with the #158
lesson: rarity — token, line, or callee — does not separate.)

### Micro-finding — Lightpad is NOT call choreography

The Lightpad Qt-stylesheet witnesses have call density 0.03–0.10: the bodies are
`return QString("QLineEdit { ... }").arg(...).arg(...);` — a *returned builder
expression* whose duplication mass is the CSS string literal. The user's top-level
call-statement definition does not cover this class at all; it is closer to a
template/data-table family. Any future composition tag must treat "call-statement
choreography" and "builder-expression templates" as distinct classes.

### Artifacts

- `composition_v2_probe.py` (sanity mode: `--sanity`), `build_callee_df.sh`,
  `callee_df_corpus.tsv`, `witness_callees.txt`, `composition_v2_labelled.jsonl` —
  all in `experiments/corpus_remeasure_131/` (gitignored).

## Literature review 2026-07-02 — the class has a name, a definition, and a verdict

Full web literature pass (primary sources fetched and quoted; details in the session log).
Key findings:

### Correct name and definition

The user's "composition/API choreography" class is, in the literature:

- **"Accidental clones" / "clones by accident"** — *"code fragments that are similar due
  to the precise protocols they must use when interacting with a given API or set of
  libraries"* (Al-Ekram, Kapser, Holt, Godfrey, ISESE 2005). In their study, 69% of
  cross-system clone pairs were this class.
- An instance of the **"Templating → API/Library Protocols"** cloning pattern (Kapser &
  Godfrey, *"Cloning considered harmful" considered harmful*, WCRE 2006 / EMSE 2008):
  *"the use of particular APIs require ordered series of procedure calls... Developers
  will often copy-and-paste these sequences and then parameterize them."*
- Rated **"incidental"** in their harm taxonomy: *"clones that cannot be abstracted
  further... referencing a single function or multiple calls to the same function, and
  the parameters to that function are changed in a non-systematic way."*
- Syntactically these are ordinary Type-2/Type-3 clones (Roy & Cordy taxonomy) — the
  taxonomy encodes similarity, not origin/intent, which is why token-level thresholds
  keep failing to separate them.
- Roy & Cordy's TR 2007-541 §7.5.2 explicitly catalogs "consecutive method invocations"
  as a *frequent false positive* class (citing Higo et al. 2006).

### What the literature says about detection

- **No shipped detector has an API-choreography filter.** State of practice: CCFinderX's
  RNR (non-repetition ratio) + TKS (token-kind diversity floor) thresholds; PMD CPD's
  `--ignore-sequences`; manual suppression. Note: archcheck's existing `diversity`
  metric is TKS-like, and the statement floor / switch-skeleton levers are RNR-adjacent —
  the shipped pipeline already implements the practical state of the art.
- **Kapser & Godfrey's own data explains our 1:1 trade:** parameterized-code clones
  (same shape, different identifiers) were *harmful 71–76% of the time* in Gnumeric —
  "same calls, different args" alone selects true positives. Direct literature
  confirmation that no single such feature separates the classes.
- Suppression is NOT supported; **tagging/down-ranking is**: 71% of clones are
  beneficial (K&G), harm concentrates in *unintended inconsistent evolution*
  (Juergens ICSE 2009), and even API-protocol clones have a harmful tail with an
  "obvious abstraction" (K&G) — the class stays actionable as an
  abstraction-opportunity signal, under a different label than copy-paste.

### Two remaining literature-backed signals — TESTED, both null on our residual set

1. **Argument-mapping systematicity** (Baker's p-match vs K&G's "non-systematic"
   incidental definition). `composition_systematicity_probe.py` on labelled witnesses
   (substitutions ≥ 3): FP n=41 median 1.000 / mean 0.727 / 27% below 0.5;
   TP n=33 median 1.000 / mean 0.717 / 30% below 0.5. **No separation.** Counterexample
   verified by eye: QtMeshEditor CLI-subcommand copy (real TP) has systematicity 0.06 —
   non-systematic payload edits in genuine copy-paste.
2. **Cross-repo callee-sequence 3-gram frequency** (HAGGIS: "clones seldom recur across
   projects", idioms do). `sequence_ngram_df.py`, full-corpus pass (capped 300
   files/repo, vendor/test dirs skipped): FP pairs median-of-medians df = 1,
   TP = 0; pairs with median df ≥ 3: FP 6%, TP 3%. **Both classes are repo-local; no
   separation.** Caveat: the capped scan undercounts df, but the FP/TP direction is flat.

### Why all four signals fail HERE — the structural conclusion

The labelled residual "FP" set is **not** the literature's incidental-clone class.
The user's own #158 review reclassified most residual FPs as useful copy-paste; the
clean incidental examples found in the wild (FLOX `o.Set` population, MMapper `connect`
chains, registration tables — present in 991/2042 snapshot repos) **do not reach the
current detector's output**: the existing TKS/RNR-like gates (diversity, statement
floor, switch-skeleton, joint floor) already filter them. What remains fired is a
mixture where choreography-shaped TPs and FPs are structurally identical — per the
literature, separable only by origin/evolution (copy event, inconsistent divergence),
not by shape.

## FINAL VERDICT 2026-07-02 — research complete

- **No v0.1 product change.** Suppression rejected by two independent probe
  implementations and four null separation signals; literature explicitly warns
  "different args" selects true positives.
- **v0.2 candidate documented:** an advisory `incidental / api-protocol` TAG
  (down-rank, never suppress) for pairs with high call-density + high recomposed-ratio,
  named per the literature. Its value is presentation (letting users filter), not
  precision. Requires its own fixtures + a labelled sample of *tag* accuracy.
- **Origin-based signals are the real future direction** (per Juergens: harm =
  unintended inconsistent evolution): `--diff`-time detection of copy events and
  divergence tracking, not snapshot shape analysis. Out of scope for this task.
- Probes and artifacts: `composition_v2_probe.py`, `composition_systematicity_probe.py`,
  `sequence_ngram_df.py`, `build_callee_df.sh` + outputs in
  `experiments/corpus_remeasure_131/` (gitignored).
- **Recommendation: close #159** (research goal answered: the class exists, has a
  literature name, and is not separable/suppressible at token level; tag idea filed
  for v0.2). Leaving in wip/ for the user's confirmation to close.

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
