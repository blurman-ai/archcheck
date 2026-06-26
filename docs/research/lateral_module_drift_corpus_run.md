# Lateral drift: corpus run of the criterion

_2026-06-12. Run of the lateral cross-module drift criterion
([lateral_module_drift_criterion.md](lateral_module_drift_criterion.md)) on the available
part of the corpus. Pipeline: `make_window_baselines.py` + `lateral_drift_scan.py`
(`experiments/ai_repo_run/`). Task #111._

## 1. Coverage

Of the 481 jsonl repos in the corpus, 184 remained on disk (the rest were deleted after the regen);
a baseline was built for **183**:

| Baseline type | Repos | How |
|---|---|---|
| Exact (parent of the first window commit) | 114 | `git ls-tree`+`cat-file` → `archcheck --save-graph-baseline` |
| Off-by-one (shallow clones: boundary = first commit) | 68 | tree of the first commit; its own event edges are not produced (under-detection) |
| Empty (repo was born within the window) | 1 | — |

`git archive` is not suitable for extraction — it hangs on asset-heavy repos (Alchemy);
only C++ sources are extracted.

## 2. Result: 108× noise suppression

| | records |
|---|---|
| Raw commits with `added_edges > 0` (these 183 repos) | 10,617 |
| **LATERAL events** | **98** |
| — LATERAL.CYCLE (closes a module cycle) | 43 |
| — LATERAL.SDP (dependency on something less stable) | 11 |
| — LATERAL.NEW (first lateral link) | 44 |

19 repos with events out of 183 — for the vast majority of repos the criterion stays silent over the
whole year-long period, which is exactly what is required of a CI gate.

## 3. Quality: eyeball of top 30 → TP ≈ 87%

Sample: round-robin over repos, CYCLE prioritized; "TP" = the edge is genuinely new and
architecturally suspicious (task bar: ≥70%).

**26 TP / 4 FP.** FP classes:

1. **Move with rename** (scylladb `compress.hh → sstables/compressor.hh`) —
   the basename-based rename heuristic is powerless when a file is renamed during the move.
2. **Test code through the filter** (impala `otel-test.cc`, directory `testutil/`) —
   the hyphenated suffix `-test` and the `testutil` segment are not covered; a trivial filter extension.
3. **Healthy consumption** ×2 (KDE kcm → new library; qmk community module →
   the quantum core) — the dependency direction is correct, but the pair is formally new.
   Both are LATERAL.NEW; the CYCLE/SDP grades in the top 30 are 100% clean (except the scylla move).

Illustrative TPs:

- **MCP wave 2025–26**: domoticz (`hardware↔push`, `main→mcpserver`), netdata
  (`libnetdata→daemon`, `web→daemon` — a *library* reached into the daemon), gearmulator
  (`editorLib→networkLib`) — new MCP servers grow sideways through all layers.
- Bitcoin-ABC `kernel→avalanche` — the core depends on a feature (ABC-specific drift).
- rocksdb `util→options` (cycle from the compression API refactor),
  `tools/ldb → db_stress_tool` (a production utility depends on a stress test).
- KDE `libklookandfeel→kcms` — an unfinished extraction: the new library is pulled
  back into kcms for `colorsapplicator.h`. The NEW events of the same extraction (consumers
  migrating to the library) are healthy; grade separation works.
- Lightpad (an AI-developed editor): `syntax→core`, `core→git` — agent commits
  with instant cycles.

## 4. Agentic vs human (raw counts, no normalization)

| author_kind | CYCLE | SDP | NEW | total |
|---|---|---|---|---|
| human | 36 | 9 | 41 | 86 |
| agent (bot author or AI co-author trailer) | 7 | 2 | 3 | 12 |

The classification only catches explicit bot authors (BOT_HINTS) and Co-Authored-By trailers;
agentic work under a maintainer's name (domoticz MCP — the style is obviously assisted)
is indistinguishable. Normalization by the share of agent commits in these repos is the next step
(repo fixed effects, boolean-drift design); 12 events are too few for a conclusion.

> **Updated #115:** on the full corpus (§8) the normalization was done. The raw agent dominance
> **does not survive repo fixed effects** — it is a compositional effect, not a per-commit signal.

## 5. Technical findings along the way

1. **`generate_per_commit_graph_drift.py` did not save the `removed` lists** — only
   the counter. Replay of additions only: ≤5.6% phantom edges (the removed share by
   counters); the criterion's persistence condition is not checked (it is satisfied trivially).
2. **The Apache banner = "vendor"**: `hasVendorLicenseHeader` treats a full Apache banner
   as a vendor marker → in Apache-licensed projects (VPP before the SPDX migration)
   ~90% of their own files drop out of the graph (VPP: 2621 files → 267 nodes).
   A candidate for a separate task in archcheck: a banner common to most of a project's
   files is the project's license, not vendoring.
3. **Mass-touch commits must register pairs**: edges merged in by a mass-touch branch
   (including SPDX migrations across thousands of files) must absorb the "first pair" — otherwise
   the very next normal commit in that pair gives an FP (vpp `vnet→vlibapi`).
4. **Merging `include/X`+`src/X` must require a header/source asymmetry**,
   otherwise different applications with similar subdirectories get merged (xLights+xSchedule
   produced 134 false events — 40% of the run).
5. **Resolution artifacts**: a "new" edge from a file the commit did not touch is a
   re-resolved include (a same-named header appeared: FastLED
   `OctoWS2811.h`); targets with the basename of system headers (`string.h`, `omp.h`) are
   resolver collisions. Both classes are filtered out in the scan.

## 6. Product conclusions (DRIFT.4)

- Signal/noise is confirmed: 108× suppression, TP ≈ 87% on the top 30, the CYCLE grade is clean.
- **LATERAL.CYCLE is a candidate gate** for `--diff` mode; SDP/NEW are advisory
  (cross-cutting rule: probabilistic = advisory). Confirmed by the KDE case: with a
  healthy library extraction, NEW is noisy, CYCLE catches exactly the incompleteness.
- In CI mode neither baseline reconstruction (the parent's graph is computed from git)
  nor persistence (the gate sits before the merge) is needed — the two main crutches of the run
  will be absent in the product.
- Before implementing DRIFT.4, close in archcheck: the Apache banner (item 5.2),
  the `-test`/`testutil` filter (item 3.2).

## 7. Run limitations

- 183/481 repos (38%); the rest require re-cloning (~30–40 GB) — the decision is deferred.
- Off-by-one baseline in 68 shallow repos — under-detection of the first window commit.
- A grace period of 10 commits is short for hot repos (KDE: the library matured over 59 commits);
  a candidate is time-based (30 days).
- The `agent` classification is conservative (see §4).

## 8. Full corpus coverage (#115)

_2026-06-12, continued. Re-clone of the deleted repos + full run + normalized
agentic cut. Scripts: `reclone_missing.py`, `agentic_normalized.py`._

### 8.1. Coverage restored

Of the 297 deleted repos, **full** clones (not shallow, not partial) were re-cloned:
296 successfully, 1 vanished from GitHub (`studiocollective/songbird`, 404), 0 network failures.
A baseline was built for **479** repos:

| Baseline type | Repos |
|---|---|
| Exact (`ok`, parent of the first window commit) | 143 new + 183 prior = 326 |
| Empty (`ok_empty`, repo born within the window: at `base~1` there is no C++ yet) | 151 |
| Root-commit / other | 2 |

Full clones removed off-by-one #111: the true window parent is now available.
`ok_empty` is **legitimate** (verified: the window covers the repo's C++ history from inception,
the first commit's parent is empty), not a bug and not a source of NEW spam (it yields only 82/495 events).

### 8.2. Full-run result

| | records |
|---|---|
| **LATERAL events (479 repos)** | **495** |
| — LATERAL.CYCLE | 153 |
| — LATERAL.SDP | 58 |
| — LATERAL.NEW | 284 |

The subset of the prior 183 repos (`exists`) reproduced **exactly 98 events** — determinism
confirmed, no regression. The growth in NEW (44→284) comes from young re-cloned repos with
a correct baseline (~1.4 NEW/repo vs 0.24 for the old ones): active modularization
of fresh projects; NEW is advisory, this is expected. 56 repos with events out of 479.

### 8.3. Eyeball of top 20 from new repos → TP 85%

Sample of 20 events from re-cloned repos (round-robin, CYCLE-first), verification
**at the event commit** (not HEAD — modules are renamed later):

| | TP | FP | precision |
|---|---|---|---|
| All grades | 17 | 3 | **85%** |
| CYCLE only (gate) | 11 | 2 | **84.6%** |

The bar of ≥70% holds, matching #111 (87%). 3 FP: 2 — CYCLE misgrade (moduleB —
a leaf utility `engine/assets` / `include/common`, no back-edge at `base~1` →
should be NEW, not CYCLE); 1 — NEW from a file split (types moved out into a new header
and re-included).

> **Closed #117 (back-edge confirmation).** The analysis showed: of the 2 "CYCLE-FP",
> only **Astraeus** (leaf `assets`) is really an FP; **PantheonChain** is a genuine
> *same-commit* cycle (the commit adds both arcs `include↔layer1` at once, the eyeball
> missed the opposing edge `units.h → layer1/asset.h`). The true CYCLE precision of the
> eyeball is **12/13 ≈ 92%**. The CYCLE grader now confirms the back-edge
> B→A from live sources at `<sha>~1` (resolving includes as archcheck does: relative
> `../x` → normalization from the file's directory, root-relative `eng/x.h` → suffix,
> bare `x.h` → basename). Corpus effect: **CYCLE 153 → 146** — exactly
> 7 phantom back-edges were downgraded (Astraeus, 3× Aztec, GBAStation, ThemisDB, UE5;
> independently verified with git-grep), 0 lost events, 0 unexpected upgrades.
> Key lesson: 15 of the 22 initial downgrade candidates turned out to be
> **genuine** cycles through relative `../` includes — a naive suffix resolution
> lost them; carried over into the DRIFT.4 spec.

### 8.4. Agentic cut: the signal does not survive repo fixed effects

Raw material: 294 agent / 201 human events. Normalization by exposure (commits in the window):

| Cut | agent | human | ratio |
|---|---|---|---|
| **Pooling** (events / 1k commits) | 9.19 | 4.27 | **2.15×** |
| **Repo fixed effects** (within-repo, 50 mixed repos) | 23 repos higher | 27 repos higher | **n.s.**, sign-test p≈0.67 |

The pooled 2.15× agent dominance is a **composition artifact**: 60% of all agent events
(176/294) come from **one repo** `makr-code/ThemisDB` (13,024 commits by `copilot-swe-agent[bot]`
out of 18,818 — a genuinely agent-built project, the classification is correct). The top-3 repos = 77%
of agent events. Within repos that have BOTH agent and human commits, there is **no** systematic
agent>human drift (23 vs 27, p≈0.67).

**Conclusion:** at the per-commit level, agentic authorship does not increase lateral drift; the growth
is the agent-saturated TAIL of the repo distribution (young, drift-prone projects like
ThemisDB), not a per-unit shift. Consistent with the earlier velocity-surge finding
(density flat, the growth is an agentic tail, not a population signal).

### 8.5. Why a before/after design (interrupted time series) is impossible on this corpus

Repo fixed effects (§8.4) compares agent and human commits *in the same repo in the same
period* — a confounder remains: within a repo the agent may get special tasks. Cleaner would be an
**interrupted time series**: one repo, drift *before* the switch to agents
versus *after* (holding the codebase, team, and domain constant). The probe
`before_after_probe.py` looked for such repos — with a long human history before agents,
agent dominance after, and events on both sides. Result:

| Repo | pre-human commits | agent share after switch | events before/after |
|---|---|---|---|
| AztecProtocol/aztec-packages | 22,980 | **5%** | 24 / 49 |
| scylladb/scylladb | 3,662 | **4%** | 12 / 12 |
| makr-code/ThemisDB | 37 | **74%** | 2 / 198 |

**Treatment strength and codebase maturity are anti-correlated.** Mature repos (Aztec,
scylladb) provide an ideal pre-baseline, but they introduce agents in tiny amounts (4–5% even
after) — there is no treatment, the contrast is smeared. Strongly agentic repos (ThemisDB, 74%)
are born agentic (37 commits before) — there is no baseline. Nobody switches a 5-year-old
human project to 74% agents all at once, so the needed natural experiment **does not exist** in the
population. Cross-sectional repo fixed effects is the ceiling of what can be squeezed
out of observational OSS data; it said "no per-commit effect". Beyond that — only
a prospective design or a targeted search for repos with a *publicly announced* abrupt transition
(by announcements, not by git signature).

### 8.6. What remained open

- ~~CYCLE grader without back-edge confirmation~~ — **closed #117** (§8.3):
  CYCLE is confirmed from live sources; the result is CYCLE 146.
- ~~File split as NEW-FP~~ — **closed #119** (§8.7): content signature + alias.
- ~~Time-based grace period~~ — **closed #121** (§8.7): hybrid 30 days / 10 commits.
- `studiocollective/songbird` (404) — 480/481 coverage, not critical.
- Removed-edge lists (#120): the generator field was added, replay phantoms ≤5.6% are tolerable.

### 8.7. Re-run of #119 + #121 (2026-06-12)

Two improvements applied simultaneously to 479 repos:

**#121 — Time-based grace:** `GRACE_PERIOD_DAYS=30` + hybrid `GRACE_PERIOD_COMMITS=10`,
CYCLE excluded from grace (a cycle at birth is a smell regardless of maturity).

**#119 — File-split FP filter:** `detect_splits()` — content signature (≥50% of the lines
of the new header come from the changed donor) + `split_origin` alias for the future.
Cost: 2 git calls per candidate, only when an event is present.

| Metric | Before (#117) | After (#119+#121) | Delta |
|---------|-----------|-------------------|--------|
| CYCLE | 146 | **186** | +40 (came out of grace — expected) |
| NEW | 288 | **191** | −97 (grace + split) |
| SDP | 61 | **40** | −21 (grace) |
| **Total** | **495** | **417** | **−78 (−16%)** |

Verified on examples:
- `AztecProtocol/barretenberg/chonk`: first appearance 2025-10-28, event 2025-11-11
  (14 days < 30) → correctly suppressed (a nascent module).
- `AmoghS1234/CipherMesh`: the whole project created over 28 days → all events correctly in grace.
- CYCLE not lost — on the contrary they grew (+40) due to being pulled out from under grace.
