# Drift signal validation — does a drift checker have a right to exist (corpus evidence)

_2026-06-06_

The goal of this document is to answer **with data**, not opinion, the question:
is architectural-drift detection in archcheck a real niche or a useless side feature.
Conclusion: **narrow drift rules (cycles, god-header shortcuts, mutual coupling)
are a real niche; raw cross-area coupling as a gate is noise.** The product's value
lies precisely in staying narrow.

## Data

- **Corpus:** 310 C++ repositories (`~/oss/`), ~13-month window
  (May 2025 → June 2026), **135,092 commits** analyzed per-commit
  (`experiments/ai_repo_run/*_graph_drift.{jsonl,md}`,
  generator `generate_per_commit_graph_drift.py`).
- **Per-commit metrics:** `added_edges`, `removed_edges`, `grown_cycles`,
  `new_cross_area_dependencies`.
- **Live validation of the shipped DRIFT.1 rule** — a separate run of
  `archcheck --drift-baseline` on LibreSprite PR #581 (see below).

### Raw distribution

| Signal | Commits | Volume | Share of commits |
|---|---|---|---|
| `added_edges>0` | 13,036 | 232,262 edges | 9.6% |
| `new_area_deps>0` | 855 | 1903 cross-area events | 0.63% |
| **`grown_cycles>0`** | **72** | **136 cycles** | **0.05%** |

`added_edges` is almost entirely benign (a new `.cpp` includes its own `.h` — normal
growth, not drift). So below we discuss only cross-area and cycles.

## Classification of the 1903 cross-area events

Heuristic: NOISE = build/vendor/test/header/codeql + rename artifacts;
STRUCT = parent→child subdirectory; LEGIT = `→core/engine/common` (normal base);
ACTIONABLE = bidirectional `A↔B` (mutual coupling = cycle/coupling smell);
REVIEW = distinct-area unidirectional.

| Bucket | Count | % | Interpretation |
|---|---|---|---|
| NOISE | 622 | 32% | noise (build/vendor/test/header + renames like `src/elab`→`src/elaboration`) |
| STRUCT | 178 | 9% | `src→src/ui` — normal structure |
| LEGIT | 182 | 9% | `→core/engine` — normal base, not drift |
| REVIEW | 627 | 33% | "a human glances at it"; potential with a better area partition |
| **ACTIONABLE** | **294** | **15%** | **bidirectional — a real layering smell** |

**Key conclusion:** the raw cross-area metric as a gate would yield **~50%+ false alarm**
(NOISE + STRUCT + LEGIT = 50% — that's normal development). You can't gate on it.
This explains why archcheck ships **narrow** rules, while cross-area stays a
research probe, not a shipped gate. This is the right discipline, not a shortcoming.

## Where the value is real

### 1. DRIFT.2 (cycles) — gate-grade

72 of 135,092 commits = **0.05%**. Fires exceptionally rarely → near-zero
false alarm. A new cycle is objectively bad (Lakos physical design). Examples from the corpus:
"Refactor CodeGen to classical header/implementation split" (+1), the Japanese
"ファイル分割" / file split (+1), "Fix build: add ESP8266WebServer.h guard" (+7),
"Restructured all source files" (+1). This is exactly the class of regressions you want
to catch in a PR.

### 2. DRIFT.1 (god-header shortcut) — precise, linter-invisible

Live run, LibreSprite PR #581 (Claude Opus 4.5 commit series):

- before `60eed0f` → `--save-graph-baseline` (graph of 1207 nodes) →
  after `276fdbd` → `--drift-baseline`.
- Result: **DRIFT.1 = 1** — `app/ui/toolbar.cpp -> app/pref/preferences.h`
  (a UI widget reached into the settings god-header, fan-in 74, for a single `bool`
  `showToolShortcuts()`); DRIFT.2 = 0.
- Of 260 violations, 259 are pre-existing legacy (ChainLength/GodHeader/SF.8),
  which drift mode ignores. **The signal is separated from the noise.**
- Skeptic check (with three `git` runs): the edge was (re)introduced by AI commit `0aa57ad`,
  not inherited from upstream Aseprite. Verdict CONFIRMED. Locked down with a
  hermetic fixture `fixtures/drift_real_world/libresprite_pr581/`
  (test `drift_fixtures_test.cpp`). Details: [ai_drift_cases.md](ai_drift_cases.md),
  [../milestones.md](../milestones.md) §"Run 10".

Ordinary linters don't catch this class — the diff is clean, the code compiles.

### 3. Bidirectional coupling — 15% of the signal, mostly NOT covered

294 `A↔B` events are genuine layering violations. Examples from the corpus:
`src/hal ↔ src/ui` (HAL — a low-level leaf — must not mutually depend on UI),
`Source/Game ↔ Source/Renderer` (game and renderer entangled), `core ↔ inspect`,
`editor ↔ engine`, `src/hal ↔ {input,math,ui,display}`.

**Important — this is NOT the same as a cycle (DRIFT.2).** At the file level `A↔B` = a 2-node cycle and
is caught by SF.9/DRIFT.2. But bidirectional here is at the **aggregate (area) level**: different
files in each module, no include cycle. Corpus check: of the **65 commits**
with a bidirectional area pair, only **9** had a real file cycle (`grown_cycles>0` →
DRIFT.2 domain); the other **56 (86%)** are area coupling without a cycle, **which DRIFT.2 does
not see**. This is Lakos "non-levelizable design" (modules can't be built separately). A candidate
for a separate narrow rule DRIFT.3 (task #087), strictly not overlapping DRIFT.2.

## DRIFT.3 — manual review of hits (eyeball, 2026-06-06)

A run of the implemented DRIFT.3 over a sample of 56 corpus commits with a bidirectional
area pair, with manual verification of each hit:

**Noise is filtered correctly (silent):** `subdir↔src` (vibe-requirements split),
`<root>` mush (Decodium "Initial commit"), born-at-empty initial structure
(NeoCalculator). The area function (strip `src/include/..`, ignore root/noise) removes
artifacts on which the raw cross-area probe drowned.

**Catches real smells (TP):**
- `hal ↔ ui/apps/math/display/input` (NeoCalculator) — a leaky abstraction layer:
  HAL should be a leaf, but it mutually depends on UI. The strongest signal.
- `common ↔ duckdb`, `common ↔ sqlite` (gizmosql) — the "base" depends back on the backend.
- `game ↔ render` (Standard-of-Iron), `terrain ↔ world` (bakabakaband),
  `engine ↔ game` (teaching engine), `core ↔ semantic` (mantra-lang) — entanglement
  on **feature commits** (genuine incremental drift).
- `model/persistence ↔ ui` (MaximumTrainer) — layering violations (persistence should
  not depend on ui).

**Problems found by eye → one fixed:**
1. **FP `build_overrides`** (UnleashedRecomp) — the exact-noise-set didn't catch `build_overrides`.
   **Fixed** with a prefix filter (`build_*`/`mock*`/`*override*`) → became silent.
2. **Noisy on large restructure commits** (MaximumTrainer — 8 pairs at once, UE5 "Refactor
   scene" — 3). When a commit reorganizes modules, many mutualities surface at once.
   You can't semantically separate "intentional refactor" from "drift" → **confirms: DRIFT.3
   should be advisory, not a blocking gate.**
3. **Coarse area granularity** (Lightpad `App/ui↔App/core` — silent): "first segment"
   collapses everything under one top-dir (`App`). This is under-reporting (a miss), not an FP;
   refining to 2 levels is a separate iteration (risk of new noise).

Eyeball bottom line: DRIFT.3 catches real coupling with acceptable precision after the area filter;
the main FP (build dirs) is closed; the remainder (restructure noise, coarse granularity) are
deliberate trade-offs that justify the advisory mode.

## The useful / useless boundary

```
NARROW (cycles + god-header shortcuts + bidirectional)
   → rare, objective, linter-invisible               → USEFUL
BROAD (any cross-area dependency as a gate)
   → ~50% false alarm, drowns in normal              → USELESS
```

archcheck is currently on the right side of the boundary. It becomes useless only if it
chases breadth.

## Recommendations → implementation status

1. **DRIFT.2 (cycles) → default blocking gate** — ✅ **done** (#086). `--drift-baseline`
   is now a regression gate: exit is determined only by DRIFT.1/DRIFT.2; legacy intrinsic
   (SF/Lakos) and advisory DRIFT.3 are reported but don't gate. Live verification on LibreSprite:
   real drift (DRIFT.1=1) → exit 1; the same version (259 legacy, 0 drift) → exit 0
   (previously failed on legacy).
2. **DRIFT.3 (bidirectional coupling)** — ✅ **implemented** (#087): a narrow rule that
   catches mutual coupling of modules `A↔B` (through different files, without a file cycle) that
   wasn't in the baseline; doesn't overlap DRIFT.2. Validated on `danielraffel/pulp`
   @ `705f86e` (`core ↔ inspect`) + manual review of 56 corpus commits (see the eyeball section).
   Remains **advisory** (noisy on restructure commits).
3. **Cross-area raw — do NOT gate**, keep advisory/research (like duplication
   after #082). Before any "area" rule — fix area detection (renames,
   build/output dirs, `src↔include` produce half the noise).
4. **The REVIEW category (33%)** — a reserve: with a better area partition, part of it becomes
   ACTIONABLE.

### Bottom line: drift core as a CI gate (2026-06-06)

Three narrow rules with an explicit, corpus-grounded gating policy:

| Rule | Signal | Mode | Rationale |
|---|---|---|---|
| **DRIFT.1** | shortcut to a god-header | **gate** | precise, linter-invisible (LibreSprite) |
| **DRIFT.2** | new/grown cycle | **gate** | rare (0.05%), objective (Lakos) |
| **DRIFT.3** | mutual coupling of modules | advisory | real, but noisy on restructure |

This is the answer to "does a checker have a right to exist": **yes** — it gates exactly what
the data called gate-grade (rare objective regressions), and doesn't drown in legacy debt and
the noise of normal development. It would become useless only if it tried to gate raw
cross-area coupling (~50% FP) — which we deliberately don't do.

## Boundaries of this analysis

- Cross-area/cycle metrics come from the python probe `generate_per_commit_graph_drift.py`,
  not 1:1 the shipped rules. `grown_cycles` ≈ DRIFT.2; DRIFT.1 (god-header shortcut)
  was validated separately (LibreSprite).
- Bucketing of the 1903 events is heuristic (calibrated by manual reading of samples,
  not a full manual labeling of all). The orders of magnitude are reliable; the exact percentages ±a few.
