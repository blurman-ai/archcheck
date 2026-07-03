# When does an agent wiki pay for itself? A/B measurements on archcheck

2026-07-03. Companion write-up for task #169. All numbers measured on this
repository; method and caveats below.

## What we wanted

Give coding agents a repo context layer (the OpenWiki / Karpathy "LLM wiki"
pattern): derived pages that answer "which files, tests, and fixtures implement X"
so an agent doesn't rediscover the repo on every task. Before adopting it as
process, measure whether it actually saves tokens and time.

## Hypothesis

An agent that starts from wiki pages answers repo questions cheaper (fewer tokens,
fewer tool calls) than an agent that greps the code.

## What we built first

28 pages under `docs/openwiki/`: an entry map, a source map, a CLI page, one page
per rule and per scan signal, plus schema/log/backlog scaffolding.

Built from two layers of sources. The *method* came from the OpenWiki / LLM-wiki
material (LangChain's OpenWiki announcement and repo, Karpathy's LLM-wiki gist;
a video summary used only for workflow hints): derived pages, an index and log,
lint, freshness. The *facts* came exclusively from the repository itself:
`CHANGELOG.md` and the README gate table for what shipped and what gates;
`src/rules/*.cpp` and `rule_set.cpp` for rule ids, verbatim message literals, and
registration; `gate_policy.cpp` for gating per mode; `config.h` for threshold
defaults; the scanners under `src/scan/`; `tests/CMakeLists.txt` for the real test
topology; the `fixtures/` tree; and the existing docs corpus (GLOSSARY, spec,
ADRs, duplication architecture), which was linked, never restated. Authored by an
Opus pilot (template plus the first 8 pages) and four parallel Sonnet agents.

Evidence discipline throughout: every claim cites `path:line`, message strings
quoted verbatim from the emitting code, unverifiable items parked as `unverified`. A
deterministic lint (`scripts/check_openwiki.py`) checks front matter, that every
cited path exists, and staleness: each page records `last_checked_commit`, and the
lint flags pages whose cited sources changed since.

## The experiment

Four lookup questions with hand-verified ground truth (exact violation message;
gate vs advisory status; threshold default and its source of truth; where to
register a new rule). Each cell = a fresh Sonnet agent. Isolation via git
worktrees: the no-wiki arm ran on a checkout of the pre-wiki commit, so it
physically could not peek. Transcript audits confirmed no arm touched files it
shouldn't have.

## Trials, errors, and what they taught

**Trial 1 — wiki with "verify against code" discipline: no effect.** Same tool
calls (10 vs 10), same tokens. Transcripts showed why: on half the questions the
agent read the wiki page *and then opened the source anyway*, because every page
says "derived — never trust over code". A cache you must re-verify on every read
saves nothing. Either the wiki is trusted at query time (with freshness maintained
separately), or it should not exist.

**Trial 2 — blind trust: still no effect.** Wiki-only agents, code reads
forbidden (audited): 10 tool calls, tokens slightly worse than grepping.

**Measurement error found and fixed.** Raw per-agent token counts looked flat
across arms. Two null agents (one doing nothing, one making two trivial calls)
showed why: a do-nothing agent already costs 24.3k tokens (system prompt, tools,
project context), and each tool call adds ~0.7k. Raw totals were ~80% constant.
All numbers below are marginal (total − 24.3k).

**Root cause of the null.** Marginal costs exposed it: reading
`src/rules/rule_set.cpp` cost 2.7k, while reading the wiki's entry map plus its
source-map page cost 7.5k. In a codebase where one rule = one ~100-line file, the
per-entity wiki pages were *larger than the sources they described*. Compression
ratio ≤ 1 means the cache is more expensive than the original. The 28-page wiki
also decayed immediately: the staleness lint flagged 8 pages the same day, after
unrelated commits touched files the pages cited.

**What the sources actually promise.** Re-reading them confirmed the boundary
rather than contradicting it. Karpathy's wiki compresses heterogeneous corpora
(papers, transcripts, chat threads) and is contrasted with RAG re-derivation, not
with grepping code; its stated value is compounding synthesis. LangChain's
OpenWiki promises documentation freshness for large repos and explicitly warns
that embedding the wiki into the instruction file is wasteful. Anthropic's
context-engineering guidance recommends just-in-time retrieval and agentic search
for well-organized repos. Practitioners in the gist comments report the pattern
paying off at hundreds to thousands of concepts, with drift as the dominant
failure mode. None of them promise token savings on a small, greppable codebase.

## What we arrived at

Deleted 27 of 28 pages. Kept a single dense map (`docs/openwiki/index.md`,
~130 lines) containing only facts that no single repo file states: a routing
table to authoritative docs, exit codes, the gate policy across all three modes
(including the deliberate check-vs-`--diff` divergence), a full rule registry
(id, exact message, threshold, impl, test, fixture — 18 rows, literals
grep-verified), and the subsystem matrix. Entry is zero-hop: `AGENTS.md` /
`CLAUDE.md` point straight at it. The lint and `last_checked_commit` stay as the
freshness loop, so the map can be trusted at query time.

## Results

Lookup questions (4, marginal tokens, sums; correctness 4/4 in every arm):

| arm | tool calls | tokens | wall-clock |
|---|---|---|---|
| grep the code | 10 | 27.3k | 93.7s |
| 28-page wiki, verify | 10 | 27.6k | 92.3s |
| 28-page wiki, trust | 10 | 29.8k | 60.2s |
| **one-page map, trust** | **4** | **13.8k** | **31.4s** |

Harder probe, modelled on the grep-heavy recon of task #168 (5-part pre-change
question on `CASE_MISMATCH_INCLUDE`; both arms 5/5 correct):

| arm | tool calls | tokens | wall-clock |
|---|---|---|---|
| grep the code | 12 | 16.5k | 52.9s |
| **one-page map, trust** | **1** | **7.2k** | **13.3s** |

The same content lost as 28 mirror pages (+9% tokens) and won as one compression
page (−49% on lookups, −56% on recon, −92% tool calls). The grep arm of the probe
also went deeper than the map and exposed a real gap in it (the rule-id string is
assigned in `cli/check_command.cpp::toViolation`, not in the scanner); the map row
was corrected the same day. That is the intended division of labor: the map wins
reconnaissance, actual modification still reads the code, and what it finds flows
back into the map.

## Conclusions

1. **Token economy has a boundary condition:** it appears only when a page
   aggregates facts scattered across many files (compression ratio > 1) and the
   entry is zero-hop. Per-entity pages that mirror small source files sit below
   the boundary and are pure maintenance debt.
2. **Trust is not optional.** A wiki the agent must re-verify saves nothing by
   construction. The workable design is trust-at-query-time plus a mechanical
   freshness loop (staleness lint keyed to `last_checked_commit`), which is also
   what the original sources describe.
3. **Size matters twice.** Small greppable repo → mirror pages lose to grep;
   the pattern's natural habitat (per its authors and production users) is large
   heterogeneous corpora or repos with hundreds of concepts. And measurement-wise,
   sub-agent token counts are ~80% fixed overhead — compare marginal costs or the
   comparison measures a constant.
4. **The durable formula:** savings = (compression > 1) × (zero-hop entry) ×
   (trust + freshness lint). On this repo that meant one page, not a wiki.
5. Side yield of writing source-backed pages: three real findings promoted to
   tasks (#170 commit-order bug, #171 dual gate mechanisms, #172 attribution
   audit). The verification process, not the wiki itself, produced these.

## Caveats

n = 1 agent per cell; one model (Sonnet); wall-clock is noisy. The four lookup
questions predate the map's design, but the map was shaped by the same experiment
that then re-measured it — the #168-style recon probe was added as a check against
that bias. Absolute savings per question are small (a few thousand tokens); the
relative numbers are what scale.
