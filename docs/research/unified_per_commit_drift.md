# Unified per-commit drift dataset: correlations (#119)

**Status:** first full run, 2026-06-25.
**Data:** `experiments/per_commit/results_full.boolrule.jsonl` — a wide per-commit
table (1 row = 1 commit), each drift signal already a separate column. **517,975**
ok-commits, **1188** repos. Authorship joined in from `experiments/per_commit/authorship.tsv`
(coverage 93.4%: human 408,222, agent 72,778, automation 3,006, no label 33,969).
**Analysis code:** `experiments/ai_repo_run/drift_correlation.py`.

## Question

Is there an observable "signature of a drift-prone commit" — do per-commit
drift signals correlate with each other, and is drift distinguishable for agentic
authors — **net of** commit size (the universal driver) and repo composition
(Simpson, lesson #115).

## Method (discipline is mandatory)

- **Spearman** (rank-based; the distributions are heavy-tailed, almost all signals are sparse counters).
- **Control for** commit size (`added_total`): first-order partial Spearman.
- **Repo fixed effects**: ranks are demeaned within-repo, then partial — this kills cross-repo
  artifacts (Simpson).
- **Case-by-case verification** of every notable link on real commits ([[feedback_verify_each_case_over_aggregate]]).
- **Agentic slice** — within-repo, size-banded, sign test across repos (not pooling — #115).

## Signal prevalence (share of non-zero, ok-commits)

| signal | % | | signal | % |
|---|---|---|---|---|
| n_test | 22.6 | | n_satd | 4.5 |
| n_complexity | 17.2 | | n_bool_field | 2.07 |
| n_added_edges | 14.9 | | n_flag_arg | 1.9 |
| n_newclone | 9.9 | | n_new_cross_area | 0.49 |
| nccd_delta | 9.2 | | n_new_god_headers | 0.21 |
| n_removed_edges | 3.0 | | n_grown_cycles | 0.11 |

The very rare ones (cycles/god-headers/cross-area) — almost all zeros; Spearman there is
degenerate and does not go into conclusions.

## Finding A — there is no strong "drift signature"; size is the universal driver

In the raw Spearman matrix everything is weakly positively correlated, and most of all — with
commit size: `n_test×size=.54`, `added_edges×size=.39`, `complexity×size=.38`, `newclone×size=.31`.
That is, "everything grows together" — mostly because a big commit touches everything at once.
After controlling for size, most of the pairs collapse (e.g. `added_edges×complexity` .16→.01).

## Finding B — a handful of weak but robust structural couplings

Pairs that survive **both** size control **and** repo FE (within-repo partial Spearman, ×100):

| pair | base | →size | →within-repo | meaning |
|---|---|---|---|---|
| added_edges × removed_edges | 28 | 26 | **24** | restructuring (edges back and forth) |
| added_edges × new_cross_area | 18 | 16 | **16** | new edges across area boundaries |
| added_edges × test | 33 | 15 | **15** | graph edits together with tests |
| newclone × test | 27 | 14 | **12** | copy-paste together with tests |
| **bool_field × complexity** | 17 | 13 | **12** | **bool accretion ⟷ complexity growth** |
| newclone × complexity | 23 | 13 | **11** | duplication + complexity |
| added_edges × newclone | 22 | 11 | **11** | new dependencies + clones |

All links are **weak** (within partial ≤ .24). Conclusion: there is no single "drift-prone commit";
when controlled for size+repo, the signals are almost independent, except for a few local mechanisms.

## Finding C — `bool_field × complexity`: a robust coupling, confirmed case by case

This is the main substantive result (and the payoff for #089/#090/#135). The link:
- survives size control (.17→.13) and repo FE (.13→.12);
- is **not** driven by config-bags/outliers: the size-partial holds at .12–.13 even after
  dropping all commits with `n_bool_field ≥ 15/10/5/3` — it is produced by the mass +1/+2 band,
  not by giant config structures;
- 59.2% of commits with bool accretion also grow complexity.

**Case-by-case verification (6 commits, independent method — diff on sha^/sha):** in 5 of 6 the bool
and the complexity growth are **in the same module**, and the consuming function branches exactly on
the added bool:

| commit | bool (where) | complexity (where) | coupling |
|---|---|---|---|
| anakryiko/wprof `ce28295` | `env.record` (env.h) | `parse_arg` (env.c) +4 | ✓ flag→arg parsing |
| anakryiko/wprof `0067e48` | `env.emit_req_split/embed` | `emit_req_event` +16, `emit_req_task_event` +7 | ✓ flag→branching |
| jameshball/osci-render `a4deab1` | `ShapeVoice.glideActive/hadPreviousNote` | `ShapeVoice::startNote` +13 | ✓ glide flags→branching |
| teriflix/scrite `880d129` | `Watcher.m_hasCursor` | `syncDocument` | ✓ same module |
| openeuler/umdk `6223f14` | `seg.is_reused` | `bondp_import_seg` +10 | ✓ same module |
| DnCraptor/pico-spec `4f12d19` | `mem.is_rom` | 6 functions in different modules | ~ big commit, weaker |

**Mechanism:** a state flag was added to a struct → the function consuming the flag branches on it →
local complexity grows. Exactly the "boolean blindness → complexity" from research #089. This is the
interpretation that a raw bool count could not give (thesis of #135: meaning comes from the correlation,
not the counter).

## Finding D — there is NO agentic drift signature

Within-repo, size-banded (10 quantile bins of `added_total`), sign test of the share of non-zero
signals across 632 repos that have both agent and human commits:

| signal | repos | agent>human | human>agent | p | verdict |
|---|---|---|---|---|---|
| n_bool_field | 376 | 154 | 222 | 0.001 | HUMAN↑ |
| n_complexity | 565 | 289 | 276 | 0.614 | — |
| n_newclone | 552 | 227 | 325 | <0.001 | HUMAN↑ |
| n_added_edges | 563 | 227 | 336 | <0.001 | HUMAN↑ |
| n_satd | 480 | 143 | 337 | <0.001 | HUMAN↑ |
| n_test | 560 | 218 | 342 | <0.001 | HUMAN↑ |
| n_flag_arg | 395 | 125 | 270 | <0.001 | HUMAN↑ |

In the full sample — **not a single `agent↑`**, human↑ on 6 of 7 signals, but the magnitude is tiny
(median within-repo difference in share of non-zero: −0.5…−2.8 pp).

**Labeling caveat (mandatory):** "agent" = an explicit AI marker in git metadata (author/committer
identity + Co-authored-by) — this is **declared** AI, often PR-gated and more cleaned up. "human" =
"no AI marker" = a **floor** on AI (IDE autocomplete and stripped trailers are invisible). Contamination
of the human bucket with undeclared AI **weakens** the real agent effect and can produce a spurious human↑.

**Sensitivity — AI-saturated repos** (agent share ≥ 30%, 356 repos, where the human bucket is cleaner): human↑
**disappears across all signals**. The only significant shift is `n_complexity` AGENT↑ (p=0.032,
median +0.021), but it **does not survive a multiplicity correction** (Bonferroni 0.05/7 = 0.007;
0.032×7 ≈ 0.22 — n.s.), so it is not pursued.

| signal | full: verdict (median) | AI-native (agent≥30%): verdict (p) |
|---|---|---|
| n_bool_field | HUMAN↑ (−0.005) | — (0.065) |
| n_complexity | — (+0.002) | agent↑ weak, not robust (0.032) |
| n_newclone | HUMAN↑ (−0.021) | — (0.700) |
| n_added_edges | HUMAN↑ (−0.020) | — (0.494) |
| n_satd | HUMAN↑ (−0.017) | — (0.081) |
| n_test | HUMAN↑ (−0.028) | — (0.059) |
| n_flag_arg | HUMAN↑ (−0.012) | — (0.598) |

**Bottom line D:** **there is no robust agentic signature of per-commit drift.** The full-sample human↑ is
a labeling artifact (it fades in clean AI repos); the only hint (complexity↑ for agents in AI repos) is weak and
does not survive correction. The direction of the effect depends on how authorship is classified —
itself a strong argument that there is no observable agentic signal here. Consistent with #115
(the agentic effect dies under repo FE) and extends the conclusion to all signals, including bool.

## Verdict

1. **There is no "drift-prone commit" as a strong phenomenon.** The main common driver is commit size;
   when controlled for it, the signals are almost independent.
2. **There are several weak but robust local mechanisms** (B): edge restructuring,
   copy-paste+tests, duplication+complexity, and — substantively valuable — **bool accretion ⟷ complexity
   growth** (C), confirmed case by case as "flag→branching in the same module".
3. **There is no robust agentic signature of per-commit drift** (D): the full-sample human↑ is a labeling
   artifact (it fades in clean AI repos), the only hint (complexity↑ for agents) does not survive correction;
   the direction depends on the authorship classification.

(`n_other` — the catch-all for un-bucketed advisory violations — is **identically 0** across all 517,975
rows: every archcheck signal lands in a named column, there are no hidden signals in the output;
nothing to unfold.)

## Boundaries

- Observational, descriptive. **Not causal** — the "bool↔complexity" correlation is compatible both with
  "a flag spawns a branch" and "a complex feature is written with a flag" — the direction is not provable from this data.
- All effects are weak (within partial ≤ .24) — this is about tendencies in the mass, not about predicting a commit.
- `author_kind` is a floor on AI (see caveat D); the corpus window/composition is `worklist_light` (1188 repos),
  not a representative sample of the population (criteria-gated, see [[project_corpus_criteria_gate]]).
- The very rare graph signals (cycles 0.11%, god-headers 0.21%, cross-area 0.49%) are almost all
  zeros: they do not go into the correlation matrix (B) as degenerate, but in the agentic sign test (D)
  they are present and confirm the same pattern.
- **Coverage boundary (no silent truncation):** the signals that archcheck emits in `--diff`
  (graph/clone/complexity/satd/test/flag-arg/bool) are unified. The **lateral grade flags** (CYCLE/SDP/NEW
  from `lateral_drift_scan.py`) are a separate family with their own run and were **not** joined here (they
  are not in the binary's advisory output — see `n_other`=0); for lateral, the conclusion about the agentic effect was
  obtained separately and is the same (it dies under repo FE) in #115. Joining lateral in is an optional matrix upgrade.

## Reproduce

```bash
python3 experiments/ai_repo_run/drift_correlation.py   # A/B/C-survivors matrices
# agentic slice D and config-bag sensitivity — inline snippets in the #119 history
```
