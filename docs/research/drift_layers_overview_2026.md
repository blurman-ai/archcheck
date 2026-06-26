# The four layers of Constraint Decay: architecture vs state (2026)

_An overview of complementary drift signals on a corpus of 185 agentic C++ projects._

---

## Introduction: why four?

Classical global duplication metrics are **silent** on AI drift (CMU difference-in-differences, MSR 2026, arXiv 2511.04427: duplicated lines density +7.03% ±4.79 — insignificant, while cognitive complexity +41.6% — robust). Constraint decay (Dente et al., EURECOM, 2026, arXiv 2605.06445 — this is a different work, not CMU) acts via a **dual mechanism**:

1. **Architectural drift** — new edges in the include graph (cross-module connectivity grows)
2. **State drift** — bool fields in structs accumulate (local complexity grows)

Both work **independently**. You can pour architecture with clean state (or vice versa). Hence four:
- Graph-drift per-commit (architecture)
- Copypaste (a maintenance metric)
- Boolean-drift per-commit (state, a temporal slice)
- Boolean-drift per-struct (state, the accumulated effect)

---

## Layer 1: Per-Commit Graph-Drift (architecture)

### What is it?

Each commit → diff `include/` → parse edges → `archcheck --diff` → record **new dependencies** (include edges that weren't there before).

### Format

**`*_graph_drift.jsonl`** — a line per commit:
```json
{
  "sha": "3a4a2a525f", "date": "2025-05-04", "subject": "Fix normals",
  "added_edges": 0, "removed_edges": 0, "grown_cycles": 0, "new_area_deps": 0,
  "added": ["+ indra/llui/llkeywords.cpp -> indra/llxml/llcontrol.h"],
  "new_cross_area_dependencies": []
}
```

### Signals

- **added_edges > 0** — an ordinary dependency (file X now includes file Y)
- **grown_cycles > 0** — 🚨 a cycle grew (was a DAG before, now there's a loop)
- **new_area_deps > 0** — ⚠️ a new cross-area (a module boundary was violated)

### Examples from the toplist (EXAMPLES_50.md)

| Repo | Commit | Signal | Edge |
|---|---|---|---|
| QuartermindGames/ape | `6a602455` | ⟲cycle+1 →13 | gui.c → editor.h |
| Collabora/online | `7a64768c` | ⟲cycle+1 →2 | TestStubs.cpp → Log.hpp |
| netdata/netdata | `cc0502ab` | ⟲cycle+1 ⊗area+2 →82 | api_v2_contexts_agents.c → rrd-retention.h |

### Trend (185 new repos, window May 2025)

- **Top by graph-errors** (cumulative): facebook/react-native (278), AlchemyViewer (256), LegalizeAdulthood/iterated-dynamics (242)
- **Repos with no graph-drift**: ~70 of 185 (clean architecture, copypaste only)
- **Mode**: shallow-since=2025-05-01 (a ~1-year window)

---

## Layer 2: Copypaste Clones (maintenance)

### What is it?

A HEAD snapshot → scan all C/C++ files → look for pairs (fragment A, fragment B) matching 3+ lines → classify: EXACT / RENAMED / LITERAL.

### Why is this drift?

Per Juergens et al. (ICSE 2009): 52% of clones are inconsistent (one changed, the other didn't). Each new clone = maintenance tech debt. Agents are prone to copy-paste (the supporting demographics: courses, bootstraps), so it's a problem.

### Examples (EXAMPLES_50.md, 25 pairs)

| Repo | Type | Length | Fragment A | Fragment B |
|---|---|---|---|---|
| libvirt | EXACT | 53 lines | bhyve_driver.c:2085 | qemu_driver.c:16657 |
| postgresql | EXACT | 150 lines | rangetypes_selfuncs.c:858 | multirangetypes_selfuncs.c:969 |
| apache/arrow | EXACT | 39 lines | c_glib/.../read-stream.c:27 | receive-network.c:57 |
| sqlite | EXACT | 48 lines | tool/showdb.c:1019 | tool/showtmlog.c:17 |
| dolphin | EXACT | 90 lines | RangeSet.h:143 | RangeSizeSet.h:242 |

### Metrics

- **A per-commit layer for copypaste does NOT exist yet** — there's only the HEAD snapshot; closed
  by task #103 (5514 per-commit records — that's bool_history, layer 3, not clones)
- **Average dup/repo**: 6–8 pairs, max 1288 (AlchemyViewer)
- **Typical clone size**: 20–60 lines (exact), 10–40 (renamed)

---

## Layer 3: Boolean-Drift Per-Commit (state, temporal)

### What is it?

Each commit → diff of all `*.h` headers → look for `+ bool field;` in struct declarations (not in locals, not in function parameters) → record **additions of bool fields to existing structs**.

### Format

**`bool_history_new185.csv`** — a line per commit:
```
repo,sha,date,author,subject,exist_bools,new_bools,files
ape,6a602455,2025-05-04,author@,Overhauled profiler,13,3,[file1.h:line,...]
```

### Signals

- **new_bools > 0** → a bool field **added for the first time** to an existing header
- **exist_bools > 0** → a bool field **added to already existing ones** (a structure-growing pattern)

### Trend (185 new repos, May 2025)

- **Total records**: 5514 commits with a bool addition
- **Top repos**: 
  - OloEngineBase (207 commits)
  - FastLED (129)
  - llama.cpp (113)
  - Serial-Studio (102)
  - AlchemyViewer (86)

### Interpretation

High numbers attest to a **step-by-step accumulation of state**. On the May 2025 – May 2026 (year) window — a natural indicator of activity + design.

---

## Layer 4: Boolean-Drift Per-Struct (state, the accumulated effect)

### What is it?

Parse all structs from the headers → count bool fields → for each struct with ≥4 bool fields run `git blame` on each field → count **how many distinct commits** introduced a bool into this struct. Filter: ≥ MIN_COMMITS (default 4) = **constraint decay**.

### Format

**`perstruct_drift_new185.csv`** — a line per struct:
```
repo,struct,nfields,ncommits,span_days,first,last,is_config,file
circt,LoweringOptions,22,5,735,2024-06-01,2026-05-26,1,include/.../Options.h
```

### Signals

- **nfields** — how many bool fields in the struct (>= MIN_FIELDS=4)
- **ncommits** — how many DISTINCT commits added them (>= MIN_COMMITS=4)
- **span_days** — the time range (first bool – last bool)
- **is_config=1** — the struct looks like a config/bag of parameters (the name contains config/opts/params)

### Top Decay examples (735+ days = 2+ years)

| Repo | Struct | Fields | Commits | Span, days | Period |
|---|---|---|---|---|---|
| llvm/circt | LoweringOptions | 22 | 5 | 735 | 2024-06-01 → 2026-05-26 |
| Phobos-dev | ExtData | 22 | 15 | 734 | 2024-06-02 → 2026-06-06 |
| qt/qtmultimedia | QWasmVideoOutput | 11 | 6 | 733 | ? → ? |
| sailfishos/qt | QWasmVideoOutput | 11 | 6 | 733 | ? → ? |
| intel/compute | MockDebugSession | 18 | 4 | 732 | 2024-06-04 → 2026-04-20 |

### What does it mean?

**Constraint decay** (the term is Dente et al.'s — for them it's the degradation of LLM agents under the accumulation of structural constraints; applying it to struct contracts is our extrapolation): a struct is well-defined (a contract), then **gradually blurs** (each developer adds their own bool flag, because:
- they don't want to refactor the whole struct
- the new flag is "local" and doesn't break old code
- after 2 years you get a monster with 20+ flags, half of them dead

### Statistics (455 structs with decay)

- **Average span**: 400–600 days (1.5+ years)
- **Average nfields**: 8–15 bool fields
- **Average ncommits**: 5–8 distinct commits

---

## Integration: from theory to practice

### How are they connected?

```
A commit adds:
├─ Graph-drift (an include edge) ──→ architectural degradation
├─ Bool-field (into a struct) ─────→ local complexity
├─ Copypaste (a code clone) ───────→ maintenance tech debt
└─ Span widens ────────────────────→ constraint decay accumulates
```

### Simultaneous impact

Example: **facebook/react-native**
- Graph-errors: **278** (the architecture is silting up)
- AI%: **0.3** (old base code, almost no AI in the May25–May26 window)
- Bool-commit: **not in the top** (state is clean)
- Copypaste: **104 pairs** (clones exist, but they're non-agentic)

→ Conclusion: this is **pure architectural drift**, not state/AI drift, just a large project with legacy.

---

## Methodology: why exactly these four?

1. **Independence** — each layer catches a different signal, underivable from the others
2. **Objectivity** — we parse from git log and headers, no semantic analysis
3. **Scalability** — they work even on 185 agentic repos without OOM (after the streaming fix)
4. **Window**: all at shallow-since=2025-05-01 (consistent in time)
5. **Validity**: the growth of local complexity under AI development is confirmed by CMU MSR 2026 (arXiv 2511.04427, +41.6% cognitive complexity); constraint decay — Dente et al., EURECOM (arXiv 2605.06445); bool-drift — archcheck's own result (1.6–2.3×, `boolean_state_agentic_vs_not.md`), no direct analogs found in the literature; copypaste — a classic (Juergens, ICSE 2009)

---

## Limitations and interpretation

### Shallow History Bias
Repo clones with `--shallow-since=2025-05-01` see only commits since May 2025 → bools added earlier aren't visible as separate commits: per-struct git blame collapses them into one boundary commit (conservatively — it understates ncommits, the absolute numbers = a lower bound).

### In the window ≠ caused by AI
The window includes both agentic (with a commit) and ordinary developers. The tendency: **if bool-drift is high AND ai%>20% OR copypaste is above baseline** → a suspicion of AI influence.

### Control
Each signal needs a baseline (old agentic vs old non-agentic, before May 2025). The CSVs contain an `agentic=1` flag, so grouping by effort is simple.

---

## Files and reproduction

- **Scripts**: `experiments/ai_repo_run/generate_per_commit_graph_drift.py`, `experiments/boolean_state/bool_history_scan.py`, `perstruct_drift_all.py`, `experiments/ai_repo_run/make_examples.py`
- **Data**: `experiments/ai_repo_run/EXAMPLES_50.md`, `experiments/ai_repo_run/corpus_summary.tsv`, `experiments/boolean_state/bool_history_new185.csv`, `perstruct_drift_new185.csv`
- **Reproduction**: `bash experiments/RESUME_pending.sh` (idempotent for what's not yet counted)

---

_Finalized 2026-06-11 (source attribution corrected 2026-06-11: CMU ≠ Dente et al.). This structure (4 layers) is used as the basis for constraint-decay analysis v1 in archcheck research._
</content>
