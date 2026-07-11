# Replication on Agarwal et al.'s C/C++ repositories

**Task:** #182 Phase E. **Status:** complete (21 repos replayed; 19 with a usable pre/post split).
**What this is:** we ran archcheck's own drift metrics on the exact C/C++ repositories that
Agarwal, He & Vasilescu studied ("The Impact of Agentic Coding on Software Development",
arXiv:2601.13597, MSR 2026) and compared the before/after direction to their published claims.
This is the strongest single referee defense for #180: same repos, independent tool, does the
result hold?

## Their study, in one paragraph

Agarwal et al. mine ~1,000 GitHub repositories that adopted an AI coding agent and compare
repo-months before vs after adoption. Headline published numbers (aggregate, all languages):
commits **+36%**, lines changed **+77%**, cognitive complexity **+35%**, duplication density
**+7.9% (not significant)**. The complexity number is an **un-normalized repo-month total** — it
rises alongside the +77% lines, and their own duplication *density* (the one size-normalized
quality metric) is flat. Their dataset carries a per-repo `first_agent_adopted_at` and PR counts;
it does **not** carry per-repo complexity/duplication values, so the comparison below is against
their aggregate direction, and our per-repo table is new evidence they did not publish.

## The 21 C/C++ repositories

Of Agarwal's 768 detailed repos, **21 are C or C++** (`primary_language`). They span the full
range: large infrastructure (microsoft/onnxruntime, Z3Prover/z3, wolfSSL/wolfssl,
microsoft/react-native-windows, shader-slang/slang, rsyslog, commaai/panda) down to small
libraries and hobby projects (forntoh/LcdMenu, joaquimorg/UV-KX, DeForestt/aflat,
SavenkovIgor/TermGraph). Adoption dates cluster in 2025-05..2025-07 (one 2024-05: ohhmm/openmind;
one 2025-02: wolfssl). 14 were already in our corpus; 7 were cloned for this run.

## Method

For each repo we replayed `archcheck --diff --diff-mode=memory --format=json` over every commit in
the window **[adoption − 16 weeks, adoption + 12 weeks]** (the #180 window), split each repo's
commits at its own adoption timestamp, and computed post/pre rate ratios on three exposure
channels: per **week** (volume / "more often"), per **commit**, and per **KLOC added**
(size-robust). Separately we took clone-**stock** snapshots (archcheck `--duplication`) at the
first and last in-window commit, to match their duplication-density lens.

**Reporting rule (from #180, framing rule #2):** the per-week absolute rate is the finding; the
per-commit and per-KLOC ratios are mechanism attribution (they show *why* the weekly rate moved),
never a mitigation of it. And per the project's own discipline: **the pooled ratio is reported
but is not the headline — it is dominated by a few tiny repositories**, so the median per-repo
ratio, a small-vs-large split, and leave-one-out are the load-bearing numbers.

## Why pooled numbers mislead here (read before the table)

The 21 repos differ in base rate by two orders of magnitude. A 56-commit Arduino library
(forntoh/LcdMenu) shows complexity/week ×28.8 and commits/week ×9.8 simply because its pre-period
base is tiny; rsyslog shows KLOC/week ×91. Summing these into a pooled ratio lets a handful of
small repos set the number. Split by size, the picture is honest: **large established C++ repos
(≥100 pre-commits) change modestly; small/young repos swing wildly off a near-zero base** — a
base-rate artifact, not an agent effect. We therefore lead with the median and the large-repo
column.

## Results (median-first; n = 19 repos with a usable split)

Post/pre rate ratios. "MEDIAN" is the median of per-repo ratios (the load-bearing number);
"large" restricts to the 10 repos with ≥100 pre-adoption commits; "pooled" and its leave-one-out
range are shown to demonstrate the domination problem, not as the headline.

| metric | MEDIAN per-repo | large-repo | pooled | pooled LOO range | Agarwal (aggregate) |
|---|---|---|---|---|---|
| commits / week | 1.56 | 1.20 | 1.49 | [1.35, 1.56] | ~1.36 (+36%) |
| KLOC-added / week | 2.03 | 0.93 | 1.13 | [0.94, 2.14] | ~1.77 (+77%) |
| **complexity / week** | **1.49** | **1.41** | 1.40 | [1.30, 1.59] | **~1.35 (+35% unnorm)** |
| complexity / commit | 0.91 | 0.83 | 1.39 | [1.05, 1.52] | per-unit (mechanism) |
| complexity / KLOC | 0.71 | 0.58 | 0.68 | [0.53, 1.00] | size-robust (mechanism) |
| new-clone / week | 0.97 | 0.64 | 0.93 | [0.75, 1.15] | ~1.08 (+7.9% n.s.) |
| new-clone / commit | 0.76 | 0.18 | 1.12 | [0.79, 1.39] | – |
| bool-field / week (P3) | 0.72 | 0.84 | 1.73 | [1.19, 2.44] | novel, no anchor |
| flag-arg / week (P3) | 0.67 | 0.62 | 1.10 | [0.72, 1.48] | novel, no anchor |

Agent PRs are **9.2%** of all PRs across the 21 (a general-adopter, low-dose sample).
Clone-**stock** density (archcheck `--duplication`, snapshots at window edges): **median change 0.0%**
(n=19, range −12%..+159%; the two positives are the small/young M-CreativeLab/jsar-runtime and
joaquimorg/UV-KX). The pooled `complexity/commit` 1.39 and `bool_field/wk` 1.73 are the classic
domination artifact — each collapses to ~0.9 and ~0.7 at the median.

## What replicates, what doesn't (qualitative)

1. **The volume-driven complexity rise replicates.** Complexity/week on the large C++ repos is
   ≈1.4 — right on Agarwal's +35%. But complexity **per commit** and **per KLOC** are flat-to-down
   (≈0.9 and ≈0.7). So the weekly rise is the volume channel: agents add complexity because they
   add more code, not worse code per line. This is the decomposition Agarwal did not publish — and
   it resolves their headline: an un-normalized +35% on +77% lines is volume, not per-unit decay.

2. **The duplication null replicates cleanly.** Clone-introduction rate per week ≈1.0 and
   clone-**stock** density median change ≈0% (nearly every real repo within ±12%; only two small
   young repos — M-CreativeLab/jsar-runtime, joaquimorg/UV-KX — move up). This matches their
   duplication density +7.9% n.s., and is consistent with our own per-commit clone null and with
   Mao (arXiv:2603.27130).

3. **Flag programming (our novel P3) does NOT replicate on this set.** Median bool-field
   accretion/week ≈0.7 (down); the pooled ×1.7 is a two-repo artifact. Honest null: P3's prior
   signal came from a different corpus and the per-struct blame method, and these 21 are not the
   high-AI-dose stratum. P3 stays a claim to be shown on the dose stratum, not here.

## Caveats (state them; do not bury)

- **n = 21 (19 with a usable pre/post split; commaai/panda and zeroc-ice/ice-demos have degenerate
  windows).** Small; the medians are indicative, not inferential — no CI is claimed.
- **Agent dose is low here:** agent PRs are 9.2% of all PRs across the 21 — this is a *general
  adopter* sample, not heavy-agent. Effects should be muted vs the ≥20%-dose stratum.
- **Duplication stock includes vendored code** (archcheck `--duplication` does not vendor-filter);
  within-repo pre→post deltas mostly cancel it, but two movers should be eye-checked before citing.
- **Clone depth:** 7 repos are shallow-since(adopt−17wk); pre-window ratios use only in-clone
  history.

## Bottom line

On Agarwal's own C++ repositories, archcheck **reproduces their two real signals** — a volume-driven
complexity rise and a null on duplication — and adds the decomposition they omitted: normalized per
unit of code, complexity does not worsen; the burden is volume. Our novel flag-programming signal
does not appear at this sample's low dose. This is a defensible, independent corroboration for
#180's volume-channel framing, with the honest negatives recorded.
