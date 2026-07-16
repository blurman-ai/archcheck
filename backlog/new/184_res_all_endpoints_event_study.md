# 184 — Full-endpoint adoption event study (one run, no pre-aggregation)

**Type:** research
**Status:** DONE pending review (2026-07-12) — outputs below; reproduces #180 on shared endpoints.
Results: `docs/research/all_endpoints_event_study.md`. Assets: `experiments/adoption_event/`
{`full_endpoint_study.py`, `plot_study184.py`, `study184_endpoints.csv`, `study184_timeseries.csv`,
`study184_chart.png`}. Not moved to `completed/` — that's the author's call.
**Depends on:** #180 (`docs/research/adoption_event_study.md`), `experiments/adoption_event/`

## Goal

One event-study run over **every** endpoint archcheck can compute. Post-selection for the
launch write-up happens later, from the table. Aggregate nothing in advance; do not collapse
union metrics.

## Sample (unchanged — do NOT alter)

Same as `docs/research/adoption_event_study.md`: **361 adopters / 265 pseudo-adoption placebo**
(`experiments/adoption_event/cohorts.tsv`). Event = first agent-adoption marker (`event_ts`).
Same estimator (`pilot_eventstudy.py`: pooled post/pre rate ratio, owner-clustered bootstrap CI,
pre-trend gate on [-12,-1], placebo through identical machinery). Same ±12 window for the
estimate; ±26 only for the time-series output. **If anything changed, say what.**

## Endpoints (from code — `run_worklist.categorize`, not from research docs)

All violation metrics the detector emits per commit via `archcheck --diff` JSON:

- **Gating:** `n_grown_cycles`, `n_new_god_headers`
- **Advisory graph:** `n_added_edges`, `n_removed_edges`, `n_new_cross_area`,
  `chain_length_grown` (nullable → count commits where grown), `nccd_delta` (continuous →
  ratio of means, flagged as non-count)
- **Advisory violations:** `n_newclone`, `n_complexity`, `n_satd`, `n_test`, `n_flag_arg`,
  `n_bool_field`, `n_bool_struct`, `n_other`

### Union decomposition (each component separately AND together)

- `P1_union` = complexity ∪ bool_field ∪ flag_arg ∪ newclone ∪ satd ∪ added_edges
- `P3_flag` = bool_field ∪ flag_arg
- graph drift = grown_cycles ∪ new_cross_area ∪ added_edges

### Volume block (separate)

- commits / week (`n_commits`)
- added LOC / week (`added_total`)
- **changed files / week** — NOT in existing panel/per-commit data; derive from git numstat
  over the ±26w window (same repos, sample untouched)
- median commit size (LOC) — from per-commit `results_*.jsonl` `added_total`, not the weekly panel

## Normalizers

Per endpoint, where applicable: **per week, per commit, per KLOC** — each as post/pre rate ratio
(dimensionless, base 1.0).

## Output 1 — main CSV (one row = endpoint × normalizer)

`endpoint, normalizer, ratio, ci_low, ci_high, p_value, n_adopters, n_events_pre,
n_events_post, placebo_ratio, placebo_ci_low, placebo_ci_high, pretrend_flat (bool),
pretrend_note, low_n (bool), coverage_note`

- `p_value`: two-sided bootstrap p for RR≠1 (consistent with the CI construction).
- Low-n endpoints are NOT dropped — emitted with `low_n=true`. Selection is the author's, not
  the script's.
- Negative results reported on equal footing with positive ones.
- Placebo and pre-trend on **every** row, no exceptions.

## Output 2 — time-series CSV (for the over-time chart)

`week (-26..+26), cohort (adopter|placebo), endpoint, normalizer, median, p25, p75`.
Each repo indexed to its own week-0 value.

## Determinism (must reproduce)

Fixed seed, corpus version, archcheck commit hash recorded in a run header. Same input → same
output.

## Acceptance

- Two CSVs written under `experiments/adoption_event/` (gitignored) + a short results `.md` in
  `docs/research/`.
- Every code-emitted endpoint present; every union decomposed; changed-files + median-commit
  gaps closed or explicitly flagged.
- Self-check: 2–3 rows verified by hand against raw per-commit rows before reporting.
