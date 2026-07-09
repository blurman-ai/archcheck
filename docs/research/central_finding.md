# Central finding: does AI accelerate architectural drift in C++?

**Build date:** 2026-06-23 · **Genre:** evidentiary line for the article (its spine). Every number is
verbatim from the primary source; links to docs at the end of each block. Skeptic rule: a conclusion rests
on cross-validation, not on "the run produced numbers".

> **Short answer.** The project started out to prove "AI erodes C++ architecture" (constraint
> decay). The empirics on 1,188 repositories / 484,500 commits gave the **opposite**: controlling for
> repository and commit size, AI code is no worse than human code in **any** category of architectural
> drift, and on copy-paste and tech debt it is **measurably cleaner**. The only stable difference is that agentic
> commits are **~1.3× larger**: that is throughput, not a different *kind* of drift. With
> one live caveat (boolean-state) and one methodological ceiling (an observational design cannot
> distinguish an effect < 3.5×). This is a **counter-narrative to "AI slop"**.

---

## 1. The bet: why the question is not naive

The constraint decay hypothesis (Dente et al., EURECOM, arXiv 2605.06445 — about the degradation of LLM agents under
accumulating constraints; the transfer to C++ architecture is our extrapolation) predicts: agents
add flag after flag, edge after edge, without refactoring, and the structure "silts up". The whole
tool is built around this: `archcheck --diff` measures architectural regression **per commit**. The corpus is
1,188 C++ OSS repos, 484,500 analyzed commits since 2024-06, 15.0% of commits AI-attributed
(72,778 agent / 408,716 human / 3,006 non-AI automation), attribution by bot author / `Co-authored-by` /
AI committer — **never** by the commit text.

## 2. The rigor ladder: how the "seductive headline" died

"Agents add more complexity" — the truth depends **entirely** on what you control for:

| Control level | complexity result | Reading |
|---|---|---|
| 1. Pooled (no control) | agents **1.76×** (27.0% vs 15.4%) | "seductive headline" |
| 2. Within-repo, raw per commit | Δ = **+1.55 pp**, 135 vs 102 repos, **p = 0.037** | agents ratchet a bit more **per commit** |
| 3. Within-repo + size-matched | Δ = **+0.06 pp**, 172 vs 157, **p = 0.44** | **null** — same for equal change |

The level-2 effect is real, but **entirely explained by commit size**: agentic commits are larger
(within-repo median ratio **1.26×**, p = 0.005; corpus 50 vs 21 LOC), and complexity mechanically
grows with size. Hold size fixed (level 3) and the difference vanishes.

_Source: docs/research/agent_drift_within_repo.md §3 (read in the primary source)._

## 3. Seven categories, within-repo + size-matched (344 repos)

| Category | median Δ (pp) | repos A>H / A<H | ties | sign-test p | verdict |
|---|---|---|---|---|---|
| complexity | +0.06 | 172 / 157 | 15 | 0.44 | **null** |
| newclone (copy-paste) | −0.23 | 130 / 178 | 36 | 0.0073 | **agents cleaner** |
| satd (TODO/FIXME/HACK) | −0.52 | 85 / 191 | 68 | 1.6·10⁻¹⁰ | **agents cleaner** |
| test_coevo | −1.63 | 121 / 206 | 17 | 3·10⁻⁶ | agents cleaner (noisy) |
| cross_area | +0.00 | 60 / 104 | 180 | 0.0007 | no deficit |
| grown_cycles | +0.00 | 8 / 30 | 306 | 0.0005 | no deficit |
| god_headers | +0.00 | 28 / 53 | 263 | 0.0073 | no deficit |

**Not a single category where agentic commits drift worse than human commits of equal size in the same repo.**
Two honest caveats from the doc's author:
- *Three structural rows are nearly degenerate* — the median repo = zero (180–306 of 344 are ties); the sign-test
  reflects a minority of repos. Read as "no agent deficit", **not** "agents are population-wide cleaner".
- *Multiple testing.* The four "agents better / no deficit" survive a 7-way Bonferroni
  (α = 0.0071); the single signal *against* the thesis (raw complexity, p = 0.037) does **not**
  survive. That is, the only result against the hypothesis is the most fragile. This favors the conclusion, but it is flagged,
  not hidden.

The two robust high-precision "agents cleaner" signals are **SATD** (p = 1.6·10⁻¹⁰) and **copy-paste**
(p = 0.0073). _Source: agent_drift_within_repo.md §3._

## 4. Three independent confirmations (not a single metric)

- **Null model "who births cycles".** Commit-level attribution within the repo: grown-cycle births
  AI=17 against an expectation of 25.9 → **0.66×**; cross-area events AI=194 against an expectation of 427.9 → **0.45×**.
  AI commits are **under**represented in drift — a direct refutation of the causal thesis.
  _experiments/CONSEQUENCES_RANKING.md._
- **Before/after (n = 61 repos, agent-adoption window).** Structural debt **did not grow** relative to
  the human control, even though commit volume/size grew. Cycles grew in 3% (2/60) agentic
  vs 6% (2/31) control; copy-paste 38% vs 32%, Δmedian = 0 in both. _experiments/CPP_AI_DRIFT_REPORT.md._
- **Re-run on the fixed scanner (#129).** 128,393 commits / 226 repos. The verdict is unchanged in 6 of
  7 categories; the only shift is copy-paste **in favor of agents**. The banner recall fix exposed
  **more human** copy-paste (1.41% human vs 0.84% agent) — the opposite of inflating an agent
  deficit. _agent_drift_within_repo.md §5._

## 5. Global metrics stay silent — both for us and worldwide

- **Authored copy-paste, agentic vs control:** Mann-Whitney p = **0.144** (non-significant), the median for
  agentic is even **lower** (0.78 vs 1.00 per 100 files). Selection worked against the null (agentic was
  pre-selected for violations) — and still no signal. _docs/research/cpp_copypaste_report.md._
- **Corpus slice AI% ↔ debt:** ρ ≈ **±0.06** (graph_errors −0.065, dup_pairs +0.061). The worst repo by
  graph drift = 43% AI; the second = **0% AI**. The AI share does not predict architectural debt.
  _experiments/CORPUS_SUMMARY_REPORT.md._
- **External validation (CMU MSR 2026, arXiv 2511.04427):** duplicated lines density **+7.03% ±4.79 —
  non-significant**, while cognitive complexity is **+41.6% — robust**. Global duplication stays silent on
  AI drift for them too; what grows is **local complexity**. _docs/research/drift_layers_overview_2026.md._

## 6. Velocity and outcome-linkage — the same deaths under the size confound

- **Velocity surge (#115).** Pooled 294 agent / 201 human = **2.15×** → under repo fixed effects
  (50 mixed repos): 23 repos agent>human, 27 human>agent, **p ≈ 0.67**. 60% of all agent events (176/294)
  come from **one** repo (ThemisDB, 13,024 copilot-agent commits out of 18,818). Simpson. The top ramp repos by
  speed have **AI% = 0** — velocity and agency diverge. _lateral_module_drift_corpus_run.md §8.4._
- **Velocity paradox vs the external productivity literature.** External studies report large
  *within-developer* speedups from AI adoption: the Copilot RCT (Cui, Demirer et al., *Management Science*,
  4,867 devs) **+26% PRs/wk**; NBER WP 35275 *Writing Code vs. Shipping Code* (Demirer, Musolff, Yang,
  ~100k GitHub devs, within-developer before/after) autonomous agents **+180% commits → only +30%
  releases**; Faros AI telemetry **+98% PRs / +91% review time**; DX (135k devs) daily-AI-users **+60% PRs
  merged**. Our headline ("cross-year commit density flat, cpp median 0.95×") is not a contradiction but a
  different **cut** of the same distribution. Re-sliced the MIT way — within-repo, before vs after the
  agent-adoption date (`unified_metrics.tsv`, 66/68 usable agentic repos) — our own corpus surges, against
  a flat matched control:

  | after/before | agentic (n≈67) | control (n≈31) |
  |---|---|---|
  | commits | median **×1.67**, p90 ×24.9, 58% ≥1.5× | **×1.00**, p90 ×2.1 |
  | lines/commit | median **×1.40**, p90 ×10.3 | ×0.93 |
  | files/commit | ×0.96 | ×0.94 |

  The control (C++ OSS, no adoption, split at a fixed 2025-09 date) is flat, so the surge is
  adoption-linked, not a repo-lifecycle artifact. The "flat" headline is the **population median** (adopters
  are ~15% of commits, a minority); the external studies report the **within-adopter** slice — our tail.
  Both true. The gap between our +67% median and MIT's +180% is the §8 machinery: a contaminated baseline
  (silent autocomplete in "before"), measurement **downstream of the review funnel** (squash/merge; +300%
  generation → +30% releases collapses before it reaches git `main`), and the hardest population (mature
  C++). Cross-check: lines/commit **×1.40** here matches the independent within-repo size-matched **~1.3×**
  (§3, §9) from a different method. Caveat: agentic repos are partly selected for recent activity, inflating
  the raw commit-count ratio; the timing-insensitive lines/commit gap (1.40 vs 0.93) holds. _Reproduce:
  median of `commits_ratio` / `lpc_ratio` over `unified_metrics.tsv`, group agentic vs control, rows with
  before > 0._
- **Outcome-linkage: do flags predict breakage?** Raw re-fix lift **1.43×** → within-repo
  size×file matched **p = 0.86** (clean null). Mantel-Haenszel residual OR=1.19 (p=0.003) → leave-one-out
  removes one repo (FlashCpp) → **OR=1.07, p=0.35**. Flagged commits are **~10× larger** (200 vs 19
  lines). Reverts are useless (48 out of 19.6k, flagged commits revert *less*). _docs/research/drift_outcome_linkage.md._

## 7. What survived AGAINST agents (honestly)

**Boolean-state per-struct drift** — the only "agents worse" signal that survived the checks.
Per-struct (git blame, ≥4 commits into the struct): agentic **9.8%** of structs drift vs non-agentic
**6.0%** (≈1.6×); per repo 36% vs 17% (≈2.1×). The activity confound is removed (commit medians 1013 vs 940 =
**1.08×**). Eye-check of both groups (FP/config): after correcting for verified real drift,
**5.6% vs 2.4% ≈ 2.3×** (non-agentic has more FP dilution, and the correction *strengthens* the signal).

Honest caveats: this is a **cumulative across-commit** effect (not per-commit size-matched, like §3);
n=73 agentic is modest; both corpora are shallow → a lower bound; it **does not prove causality**. An early
*file-level* version of this signal was a metric artifact (45% FP, ~23% precision) — only the
**per-struct** attribution survived. _docs/research/boolean_state_agentic_vs_not.md, boolean_state_drift_eyecheck.md._

## 8. Methodological ceiling: "not confirmed" ≠ "proven equal"

This is critical for honesty — the observational design is **underpowered**:
- **Contaminated control (bias-to-null).** "Human" = "no AI marker", but IDE autocomplete and stripped
  trailers are invisible in git → the human class is contaminated with hidden AI. At 20% false agentic + 30% hidden AI
  in the control, a true ratio of 2.0 is observed as **1.38** — below the threshold of detectability. The design does not
  distinguish an effect **< ~3.5×** on threshold metrics at a horizon < 12 months. _full_audit §3.2._
- **Survivorship.** The selection gate (>300 commits since May 2025) takes survivors; born-agentic repos
  are discarded.
- **Global metrics are low-powered.** The MDE for duplication ≈ 29 pp; for cycles the ratio ≈ 3.5× (n=61 vs 31).

Conclusion: the fact that boolean-state drift (2.3×) **survived on a contaminated control** is an argument rather FOR
the hypothesis than against. The honest framing: "not distinguished on the available design", not "no effect".

## 9. A single cause for all the "false alarms": size, not kind

Every naive "horror" (complexity 1.76×, outcome-linkage 1.43×, velocity 2.15×) died under one
correction — **commit size**. The only stable agentic difference: commits **~1.3× larger**
(50 vs 21 LOC) and more of them. This is **throughput, not sloppiness**. That is why a per-PR CI gate (measures
regression *per change*, author-agnostic) is the right control: it does not care who — or what —
wrote the code.

## 10. What this means for archcheck: differentiation, not prediction

- The tool's value is **differentiation**: a unique per-commit copy-paste finding (0 FP against
  external oracles like NiCad), a category **no mainstream CI models** (new include cycles,
  god-headers, cross-file copy-paste). _experiments/showcase/._
- What the tool does **NOT** do: it does not predict "bad" commits (§6), it does not catch "AI worse than humans" (§3).
- Precision from the manual audit (§5): copy-paste is actionable in ~⅓ of fires (and those cases are genuinely
  valuable: GpgFrontend inserted 30 lines in a "reduce code duplication" commit); a complexity fire reliably
  means "the function really is complex", but rarely "you have eliminable debt" (0 TP / 9 JUSTIFIED / 3 FP out of 12);
  the structural categories are rare and carry export-header / reorg / template FP classes.
- Gate on the precise signals (copy-paste post-filter + genuinely new cycles); the rest is
  advisory. `--baseline` from day one, so legacy debt does not block and only the PR delta is caught.

## 11. External literature (framing)

- **CMU MSR 2026** (arXiv 2511.04427): duplication +7.03% n.s. / cognitive complexity +41.6% robust.
- **Dente et al., EURECOM** (arXiv 2605.06445): constraint decay in LLM agents.
- **Juergens et al., ICSE 2009:** 52% of clones are inconsistent (every new clone = maintenance tech debt).
- **Xu et al., FSE 2015:** Hadoop 17→173 config knobs over 7.5 years — this is the **norm**, not drift (why
  a static boolean counter drowns in config-bag FP, 78%).

## 12. Measurement-reliability caveats (data hygiene)

Three bugs that produced plausible nulls, caught by dogfooding the run itself:
- **empty-blob desync** in the memory backend corrupted all graph categories — fixed in `f3ab6ac`, the run
  restarted from scratch (the low final cycle/god rates confirm it).
- **parentless / shallow-boundary commits** fake "the whole tree was added" (on the polluted ledger
  grown cycles = 62% of all fires) — excluded at the source.
- **token-scanner CCN** overestimates on `#if` / `?:` / `[[attr]]` — a metric limitation, not a bug
  (deterministic: memory == disk). _agent_drift_within_repo.md §1, §5, §8._

## 13. The honest bar to break through the ceiling

To turn "not distinguished" into "proven": within-repo, size+file matched odds ratio on the **full**
corpus (not 30 repos), remaining **> 1 at p < 0.001 after leave-one-out**; a born-agentic design
(a repo agentic from a known moment, before/after); a better outcome proxy (bug-fix commits that later
edit the same span — needs message classification) or linkage to issue trackers. Reverts will not
carry it (too rare). Until then — **tested and not found**, not unexplored.

---

_Primary sources read directly: agent_drift_within_repo.md, drift_outcome_linkage.md,
boolean_state_agentic_vs_not.md, drift_layers_overview_2026.md. The remaining numbers come from research docs
and experiments reports via structured extraction (6-cluster run, 96 probes,
28 survived / 30 died). Linked to [JOURNEY.md](../JOURNEY.md) — there the same conclusion appears as a path and its reversals._
