# Gate moderator: does a weak human gate amplify agent-authored drift?

**Build date:** 2026-07-06 · **Task:** #174 · **Genre:** evidentiary line for the article.
Crosses two earlier axes — #119/#115 (author axis: no per-commit agentic main effect net of
composition) and #146 (gate axis: 0/38 AI-reviewed drift PRs blocked). Every number is verbatim
from the primary run under `experiments/gate_moderator/`. Skeptic rule: a conclusion rests on the
right denominator, not on "the run produced numbers".

> **Short answer.** No. The #173 interaction hypothesis — that agent-authored commits drift more
> *when* they pass through a weak or absent gate — is not supported. The raw funnel looks like it
> supports it (agent drift is 85% non-blocking-gated vs 71% for human drift), but that gap is
> **composition, not interaction**: agent commits route through weak/non-blocking gates ~90% of the
> time *whether or not they drift* (Copilot→PR→AI-review, or direct push). Measured against the
> agent's own baseline gate-mix, agent drift is if anything slightly *more* gated, not less
> (82.8% vs 90.6% non-blocking, −7.8pp). The gate is a property of how agents commit, not something
> drift slips through. Consistent with the #115/#119 result that the agentic effect dies under
> composition controls.

---

## 1. The question and why neither prior axis could see it

#119/#115 asked "do agent commits drift more?" — population answer: no, composition explains it.
#146 asked "do AI reviewers block drift?" — answer: never (0/38). The cross of the two —
P(drift | agent, weak gate) vs P(drift | agent, strong gate), within repos, against the human
equivalent — lives in the author × gate × drift three-way table that neither run built. #174 builds it.

## 2. Data and method

All local corpus, phases A–B zero-API, phase C a budgeted read-only GitHub sweep.

- **Drift set:** the 648 gate-fail commits already vetted by #146
  (`experiments/trending_run/inverted_corpus.jsonl`), each carrying its gate `bucket`.
  (A separate 49-raw / 17-trustworthy agentic stratum has zero overlap with the per-commit scan
  and is set aside; see §7.)
- **Author axis (Phase A):** each of the 648 classified fresh from git with the leak-audited
  `authorship_join.py` classifier (identity fields + `Co-authored-by` trailer only; the commit
  body is never matched). 0 git failures, 0 mismatches against the stored `authorship.tsv`.
- **Denominators (Phase B):** every `results_full.jsonl` ok commit in the 230 contributing repos,
  classified over full history (no `--since` window, so numerator and denominator share one
  population). 0 unclassified.
- **Control gate-mix (Phase C):** for the 19 repos with both agent ≥ 30 and human ≥ 30 non-drift
  volume, a deterministic sample of 40 agent + 40 human non-drift commits per repo, each commit's
  merge-gate classified with the #146 machinery (`gh api commits/{sha}/pulls` → reviews).
  1,530 commits classified (734 agent / 796 human; 83 api_err excluded).
- **Analysis (Phase D):** within-repo contrasts only; drift gate-mix vs baseline gate-mix, per author.

**Eye-check / correction.** The first pass put 49 commits in the agent class. Hand-checking every
one found 2 committer-only false-agents — a human author whose commit was *committed* by monorepo
automation (`facebook/wangle`, committer `meta-codesync[bot]`; `ROCm/clr`, committer
`systems-assistant[bot]`). The `[bot]` identity rule caught the committer, not the author. Both
moved to human. Corrected agent drift n = **47** (26 AI-authored + 21 AI-`Co-authored-by`). The
Phase-C classifier was hardened to require the AI/bot signal in the *author* field.

## 3. The three-way table (drift-conditioned author × gate), corrected

```
gate tier                agent   human   TOTAL
----------------------------------------------
zero (direct push)          12     292     304
weak (PR, no review)        11      91     102
AI-review only (nonblock)   14      29      43
generic-bot only (nonblk)    2       9      11
human review                 7     173     180
unknown (api_err)            1       7       8
----------------------------------------------
TOTAL                       47     601     648

non-blocking-gate share of drift:  agent 39/46 = 84.8%   human 421/594 = 70.9%
```

Read alone, this is the #173 hypothesis: agent drift is more non-blocking-gated than human drift.
The trap is that it has no denominator — it does not know how agents gate their *non*-drift commits.

**Repo concentration (the #146 caveat).** The cell that most separates the two authors —
agent × AI-review-only (14) — is 11 of 14 from three repos: FlashCpp (5), Xorion (4), TinyMUSH (2).
Agent drift overall: 47 commits across 32 repos.

## 4. Denominators (Phase B): agent does not drift more

Pooled drift rate by author class, across the 230 contributing repos:

```
class        n_commits   n_drift   rate/1k
agent            22289        49     2.198
human           139712       599     4.287
```

Agent drift rate is *below* human, pooled. Within-repo paired (repos with ≥ 20 of each class,
66 repos): agent-rate > human in 19, agent-rate < human in 47. No agent main effect — the #119
result reproduces on this slice.

## 5. The denominator that settles it (Phase C + D): composition, not interaction

Baseline = the author's non-drift gate-mix, sampled per repo. Restricted to the 19 repos that have
a baseline (apples-to-apples: drift rows counted only in those repos):

```
             baseline non-drift    drift            drift − baseline
agent        weak 626/691 = 90.6%  weak 24/29 = 82.8%   −7.8 pp
human        weak 715/756 = 94.6%  weak 10/12 = 83.3%   −11.2 pp
```

Agent commits go through a weak/non-blocking gate **90.6% of the time regardless of drift**. Agent
drift, at 82.8%, is not routed through a *weaker* gate than the agent's own baseline — it is
marginally more gated. There is no "drift slips past the gate" signal.

**Within-repo, drift vs the author's own baseline (agent):** across the 14 repos with an estimable
baseline, agent drift weak-share was higher than baseline in 3, lower in 3, equal in 11 (mostly
repos that are 100% direct-push for everyone, where there is no gate to slip). The one repo where
agent drift did skew weaker than baseline is FlashCpp (77% baseline → 5 AI-reviewed drift commits) —
the same repo the concentration flag names.

**The composition channel is real but repo-specific, not directional.** Agent-vs-human baseline
gate-mix within repo ranges from −45pp (agents *more* gated, e.g. `starseeker/obol`, `tkadauke/raytracer`,
`ImagingTools/Acf`) to +60pp (agents less gated, `antgroup/vsag`: agents route 100% through
AI-review, humans 40% weak / 60% human-review). Agents were weaker-gated than humans in 2 repos and
more-gated in 6. There is no general "agents get weaker gates" law; the gate-by-author relationship
is entirely a per-repo policy artifact.

## 6. Why the raw funnel looked like an interaction

The Phase-3 gap (agent drift 84.8% weak vs human drift 70.9%) is not agents slipping drift past
gates. Two composition facts produce it:

1. Agent commits are ~90% non-blocking-gated at baseline — the AI workflow is PR-with-AI-review or
   direct push, with human review rare.
2. The full human-drift population (594 commits over 230 repos) includes repos with a human-review
   culture, where human drift attracts human review (in the unrestricted pool, human drift is 70.9%
   weak against a 94.6% human baseline — humans self-gate on architecturally significant changes).
   Restricted to the same 19 repos as the agent baseline, that human self-gating shrinks to −11.2pp
   on n=12, i.e. it is carried by repos outside the agent-drift set.

## 7. Caveats (stated, not corrected away)

- **Small n on the drift side.** 47 agent drift commits, 29 of them in the 19 baseline repos. Every
  agent cell in §3 is hand-verified; the within-repo drift-vs-baseline test is directionally clean
  but underpowered per repo.
- **Concentration.** The AI-review-only agent cell is 3-repo-dominated (FlashCpp, Xorion, TinyMUSH);
  FlashCpp is also the one repo supporting the interaction direction. Any headline that leaned on
  that cell would be a Simpson artifact.
- **Known contamination (`central_finding.md:121`).** Squash / stripped-trailer commits with hidden
  AI authorship land in the human class, biasing the author contrast toward null. Not corrected for.
- **api_err.** 8/648 drift and 83/1530 baseline commits returned no PR data (deleted PRs, API
  errors); excluded from "known" denominators, not imputed.
- **Window.** Baseline and denominators use the same population; the drift set includes a handful of
  pre-2024-06 commits that the stored `authorship.tsv` window omits — Phase A reclassified all 648
  fresh, so the drift author labels do not depend on the window.

## 8. Verdict

The interaction the #173 hypothesis predicted is absent. Agent-authored drift does not route through
weaker gates than agents' normal commits; the apparent skew in the raw funnel is the composition
signature of the agent workflow (weak/no gate by default), the same signal that #119/#115 already
isolated on the author axis and #146 on the gate axis. A weak human gate does not *amplify* agent
drift in this corpus — it is simply the gate agents already use for everything. Null result, written
up with the same care as a positive one.

**Primary artifacts:** `experiments/gate_moderator/` — `phase_a_joined_corrected.jsonl` (author×gate),
`phase_b_perrepo.tsv` (denominators), `phase_c.jsonl` (baseline gate-mix), `phase_d_summary.txt`
(this analysis), and the `phase_*.py` scripts.
