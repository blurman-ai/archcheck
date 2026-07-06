# [RESEARCH][DRIFT] Gate moderator: does a weak human gate amplify agent-authored drift?

**Creation date:** 2026-07-06
**Start date:** 2026-07-06
**Status:** wip
**Module:** RESEARCH][DRIFT
**Priority:** major
**Difficulty:** large (research judgment required — NOT a Haiku/Sonnet task)
**Executor:** Opus-class or above. Phases A–C are executable as specified; Phase D is where
judgment concentrates. **Mandatory checkpoint: after Phases A–B, show the raw three-way table
to the user BEFORE any interpretation, write-up, or Phase C sampling design.** Do not proceed
past the checkpoint without an explicit go-ahead.
**Blocks:** —
**Blocked by:** —
**Related:** #119 (population-level: no per-commit agentic main effect net of composition),
#115 (agentic effect dies under repo fixed effects — within-repo design is mandatory),
#146 (gate axis on drift commits: 38 AI-reviewed drift PRs, 0 blocked, all merged),
#173/#173b (GitHub-wide weak-review corpus — the breadth complement to this task)

## Goal

Test the **interaction**, not the main effect. #119/#115 asked "do agent commits drift more?"
— population answer: no, composition explains it. #146 asked "do AI reviewers block drift?" —
answer: never (0/38). Nobody has yet crossed the two axes:

> P(drift | agent-authored, weak gate) vs P(drift | agent-authored, strong gate),
> compared within the same repositories against the human-authored equivalents.

The #173 hypothesis is exactly this interaction. If it exists, it lives in the
author × gate × drift three-way table, which neither #119 (no gate axis) nor #146
(no author axis) could see.

## Why the meat is here and not in GitHub search

The GitHub-discovery path (#173/#173b) mostly surfaces small personal repos where archcheck
has little architecture to measure. Meanwhile the local corpus already contains both
ingredients, verified 2026-07-06:

- **Agent authorship at scale in real repos:** trailer sweep over `/home/localadm/oss`
  (2,042 repos, window since 2024-06-01): **107,229 commits with AI co-author trailers in
  1,220 repos; 220 repos have ≥ 100 each** — including mruby (989, committed by Matz himself),
  tenstorrent/tt-metal (1,623), GaijinEntertainment/daScript (1,478), qore (1,162).
  Sweep file: `experiments/trending_run/trailer_sweep_local_corpus_20260706.txt`
  (format: `count repo_dir/`). Eye-checked on mruby and daScript — trailers are real.
- **Drift already computed:** per-commit scan `experiments/per_commit/results_full.jsonl`
  (520,177 commits; fields incl. `gate`, `n_grown_cycles`, `n_new_cross_area`; **no author
  column** — derive author locally from git). Gate-drift event lists:
  `experiments/trending_run/corpus_gatefail.jsonl` (1,615 raw / 648 trustworthy after #146
  vetting) and `ai_stratum_gatefail.jsonl` (49 raw / 17 trustworthy).
- **Gate classification already computed for drift commits:**
  `experiments/trending_run/inverted_corpus.jsonl` (648 rows, field `bucket`:
  no_pr / human / ai-reviewed / …) — the #146 funnel output, reusable as-is.
  Classifier script: `experiments/trending_run/inverted_bot_drift.py`.

## Method

### Phase A — author axis on the existing funnel (local, zero API)

For each of the 648 (+17 stratum) gate-drift commits: `git -C <repo> show -s` → classify
`author_kind` (agent trailer / bot author / human) with the same regex as the trailer sweep.
Join with `bucket` from `inverted_corpus.jsonl` → the drift-conditioned author × gate table.
Key cells: agent × no_pr (direct push, zero gate) and agent × ai-review-only.

### Phase B — denominators (local, zero API)

For every repo contributing to Phase A cells: count total commits by `author_kind` in the
same window (git log over the local clone). Combined with `results_full.jsonl` this gives
per-repo drift rates by author class.

### Phase C — gate for control (non-drift) commits, sampled

Gate class is unknown for non-drift commits. Sample per repo (e.g. N agent + N human commits,
repos from the top of the trailer sweep) and classify their merge path with the #146
machinery (`gh api commits/{sha}/pulls` → reviews). This makes the full
author × gate × drift table estimable on a sample. Budget API calls; incremental + resume.

### Phase D — analysis discipline

- **Within-repo contrasts only** (#115 lesson: cross-repo comparisons die under fixed effects).
- Report raw cell counts and repo concentration (the #146 caveat: 38 PRs = 17 repos,
  SPECFEMPP alone 11 — check for the same concentration here; Simpson memory applies).
- Eye-check a sample from every populated cell: trailer real, drift real, bucket correct.
- Known contamination (docs/research/central_finding.md:121): squash/hidden-AI commits sit in
  the "human" class → biases the contrast toward null. State it; do not correct for it silently.
- No causal claims. The deliverable is the table + verified examples.

## Deliverables

- The three-way table with per-cell counts, per-repo breakdown, and drift-rate ratios.
- Eye-checked example set (agent-authored + zero-gate + drift = showcase candidates, cf. #123).
- Write-up in `docs/research/` (new page or a §of an existing one — decide by content).
- JOURNEY entry (this is research substance — passes the 2026-07-06 bar).

## Acceptance criteria

- Every populated cell of the table has ≥ 3 hand-verified examples or an explicit
  "cell too small" note.
- Denominators enumerated per repo, not extrapolated (per-case-over-aggregate memory).
- Repo-concentration stated for every headline number.
- Null result is a valid outcome and gets written up with the same care.

## Non-goals

- No corpus growth, no new cloning (that's #173 §storage / #173b).
- No GitHub-wide PR search (that's #173b).
- No changes to archcheck itself.

## Progress

**2026-07-06 — ALL PHASES DONE. Verdict: null (composition, not interaction).**
Deliverable: [docs/research/gate_moderator_interaction.md](../../docs/research/gate_moderator_interaction.md).
JOURNEY appended+pushed. The #173 interaction hypothesis is NOT supported: agent commits
route through weak/non-blocking gates ~90% of the time regardless of drift; agent drift
(82.8% weak) is not weaker-gated than the agent baseline (90.6%). The raw-funnel gap
(agent 84.8% vs human 70.9%) is the composition signature of the agent workflow, not
drift slipping the gate. Same outcome as #115/#119. Acceptance criteria met: every
populated agent cell hand-verified, denominators enumerated per repo (0 unclassified),
repo-concentration stated (AI-review-only cell = 11/14 from 3 repos), null written up.
Phase C = 1530 baseline commits classified via gh api (734 agent / 796 human).
**Status: analysis complete; awaiting user command to move to completed/.**

---

**2026-07-06 (earlier) — Phases A+B done; mandatory checkpoint (got go-ahead for C).**

- Phase A (`phase_a.py`): classified all 648 drift commits fresh from git with the
  leak-audited `authorship_join.py` classifier (identity + Co-authored-by trailer only).
  0 git failures, 0 mismatches vs the stored `authorship.tsv` (strong provenance).
- Phase B (`phase_b.py`): full-history sweep of the 230 contributing repos (no `--since`
  window, so numerator and denominator share one population). 0 unclassified.
- Eye-check caught 2 committer-only false-agents (human author + automation committer:
  facebook/wangle meta-codesync[bot], ROCm/clr systems-assistant[bot]) → moved to human.
  Corrected agent drift n=47 (26 AI-authored + 21 AI-trailer). FlashCpp/Xorion cells
  spot-verified: trailer real, drift real, gate bucket correct.
- **Concentration flag:** agent × AI-review-only (the cell that most separates agent from
  human) is 11/14 from 3 repos (FlashCpp 5, Xorion 4, TinyMUSH 2) — the #146 trap.
- Outputs in `experiments/gate_moderator/`: phase_a_joined[_corrected].jsonl,
  phase_b_perrepo.tsv, phase_b_summary.txt, checkpoint scripts.

Next (post go-ahead): Phase C sampling design for control-commit gate classification.

## Changed files

| File | Change |
|------|--------|
| `experiments/trending_run/trailer_sweep_local_corpus_20260706.txt` | sweep snapshot (done) |
| `experiments/gate_moderator/phase_a.py` | Phase A author-axis join (done) |
| `experiments/gate_moderator/phase_b.py` | Phase B denominator sweep (done) |
| `experiments/gate_moderator/*.jsonl,*.tsv,*.txt` | phase A/B outputs |
| `backlog/wip/174_maj_gate_moderator_agent_drift.md` | this task |
