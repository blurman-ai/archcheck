# Adoption event study — confirmatory results (task #180)

**Status:** confirmatory analysis run on the expanded panel (n=361 adopters / 265
pseudo-adoption placebo controls). Pre-registration: `adoption_event_study_prereg.md`
(5 endpoints P1–P5 + S1 frozen before mining). Pilot: same doc §Phase-0.
**Bottom line up front:** the defensible, multiply-anchored finding is the **volume channel** —
after agent adoption a repository accumulates architectural-drift events per week ~1.3–1.7×
faster, while per unit of code quality is flat-to-contested. The per-unit "agents are worse"
claims (P3 flag programming; P2 complexity per KLOC) **do not hold up**. Two registered
endpoints (S1 pure-agent stratum, P5 as-submitted PR drift) are **not yet run**.

## Design (as pre-registered)

Repository-level event study. Event = earliest of 4 adoption markers (CLAUDE.md /
copilot-swe-agent PR / codex/ branch / Co-Authored-By: Claude), ≥50 total pre-commits and
≥12 weeks each side. Post/pre rate ratios on three exposure channels: **weeks** (volume /
"more often"), **commits** and **KLOC-added** (per-unit). Owner-clustered bootstrap CIs,
pseudo-adoption placebo (never-adopters run through the identical machinery), leave-one-owner-out,
donut [−4,+4], pre-trend gate on event-weeks [−12,−1]. Estimator note: pooled ratios sum events
and exposure across repos; per framing rule #2 the **per-week rate is the finding**, per-commit
and per-KLOC are mechanism attribution, never a mitigation of the per-week result.

## Confirmatory results (n=361 adopters, 265 placebo)

Weeks-channel RR with owner-clustered 95% CI; placebo RR; per-unit ratios; pre-trend verdict.

| Endpoint | /week [95% CI] | /commit | /KLOC | placebo /wk | LOO /wk | pre-trend |
|---|---|---|---|---|---|---|
| **P1 union drift** | **1.32 [1.08, 1.61]** | 1.09 | 1.07 | 1.09 | [1.27, 1.37] | flat ✓ |
| **P2 complexity** | **1.71 [1.34, 2.16]** | 1.42 | 1.39 | 1.16 | [1.60, 1.80] | flat (marginal) |
| **P3 flag (bool+flag)** | 1.26 [0.89, 1.81] | 1.04 | 1.02 | 1.03 | [1.18, 1.47] | flat ✓ |
| **P4 clone introductions** | **1.37 [1.11, 1.70]** | 1.13 | 1.11 | 1.06 | [1.32, 1.41] | flat ✓ |

P1 half-split stability 1.35 / 1.28. Union = complexity + bool_field + flag_arg + newclone +
satd + added_edges.

**vs the pilot (n=212):** P1 1.34→1.32 (stable); P2 1.76→1.71 (stable, and its pre-trend went
from NOT-flat to marginally flat with more data — slope +0.0050, se 0.0025); P4i 1.32→1.37
(stable); **P3 1.50 [1.05, 2.09] → 1.26 [0.89, 1.81] — weakened, CI now crosses 1.** The
pilot's P3 was small-n optimism.

## What each endpoint means

### P1 volume — CONFIRMED, and it is the headline
×1.32/week [1.08, 1.61], clears placebo (1.09), LOO-stable, flat pre-trend, per-commit ≈1.09.
Consistent across pilot (1.34), expansion (1.32), the Agarwal replication (commits/week median
1.56, `agarwal_replication.md`), NBER WP 35275 (+36–109% commits) and Faros 2026 (review time
×5). Reading: more drift events per week because more (and larger) commits, not worse code per
commit. **This is the archcheck pitch — volume outpaces review — stated as a rate effect.**

### P2 complexity — per-week up, per-unit CONTESTED
×1.71/week, clears placebo (1.16), LOO-stable. But the per-unit numbers **contradict between
measurements**: the panel (whole-history per-commit `results_full`) gives per-commit 1.42 /
per-KLOC 1.39 (up), while the Agarwal replication (windowed `--diff-mode=memory` replay on his
own 21 C++ repos) gives per-commit 0.91 / per-KLOC 0.71 (down). This is the per-KLOC puzzle,
still unresolved and measurement-dependent. **Do not cite a per-unit complexity number in
either direction.** Per framing rule #2 the per-week rise is the finding; the per-unit channel
is reported as contested mechanism. Note Agarwal's own headline "+35% cognitive complexity" is
an un-normalized repo-month total on +77% lines — i.e. the same volume channel without the
decomposition.

### P3 flag programming (the novel claim) — NOT CONFIRMED in the adoption design
The weekly accretion-count proxy is weak: ×1.26 with the CI crossing 1, per-commit ≈1.0, and it
**does not replicate** on Agarwal's repos (median bool-field/week 0.72). Two independent lines
point null. **Important scope note:** the prior strong signal (#119, per-struct boolean-state
×2.3 surviving size+repo FE) came from a *different method* — git-blame per-struct, ≥4 commits
into a struct — which was **not applied to the adoption cohorts** here. So P3 in the adoption
framework is *unconfirmed*, not refuted: the cheap weekly proxy shows no robust effect, and the
method that produced the ×2.3 has not been re-run against adoption cohorts. This is the endpoint
the author specifically asked to foreground ("сложность и программирование на флагах"); the
honest status is that the accretion-rate proxy does not carry it, and the per-struct re-run is
the outstanding test.

### P4 clone stock — introductions up per week, STOCK null
Introductions ×1.37/week but per-commit ≈1.13. The registered endpoint is the **stock**
trajectory, measured in the Agarwal replication via `--duplication` snapshots at window edges:
**median stock change ≈0%** (n=19, nearly every real repo within ±12%; the two positives are
tiny young repos). Consistent with our per-commit clone null and with Mao (arXiv:2603.27130).
The leadline 19-clones observation is a high-dose (~100% AI, no review gate) limiting case, not
the general-adopter pattern — which is exactly why P4/P5 were framed around *stock* and *dose*.

## What is NOT yet run (registered but outstanding)

- **S1 — pure-agent descriptive stratum.** Full drift profile per commit & per KLOC on ≥90%
  coverage, redoing the #149-crippled ai-377 run. Not run.
- **P5 — as-submitted PR drift.** Agent PRs as submitted (`git fetch origin pull/N/head`) vs
  matched human PRs in the same repos. Assets exist (`experiments/agentic_weak_review/`) but
  carry `pr_url`/author, not base/head SHAs → needs network head-ref fetches (Phase-3 run).
  Not run.
- **P3 per-struct re-run** on adoption cohorts (see above) — the real test of the flag claim.

## Honest headline

Of the four endpoints computable from the panel, the **volume channel (P1)** is confirmed and
externally corroborated; **P2 per-week** rises but its per-unit reading is contested; **P3 flag**
and **P4 clone stock** do **not** show a per-unit "agents worse" effect. This is the
volume-channel story the project has consistently measured — agents don't write worse code per
unit, they generate more of it faster than review scales. The registered-but-unrun S1/P5 and the
P3 per-struct re-run are where any remaining per-unit signal would have to come from; until then
the claim stops at "more drift per week, per-unit flat," which is the defensible pitch and is
consistent with every prior null in this project.

## Assets

- Panel/cohorts/estimator: `experiments/adoption_event/` (gitignored) — `pilot_eventstudy.py`
  on `panel_weekly.tsv` + `cohorts.tsv` (expanded: 209 corpus + 62 local-agentic + 105 mined +
  external #182 C++ adopters).
- Independent replication: `docs/research/agarwal_replication.md` (#182 Phase E).
- Pre-registration: `docs/research/adoption_event_study_prereg.md`.
- Literature anchors: `docs/research/agentic_drift_literature.md`.
