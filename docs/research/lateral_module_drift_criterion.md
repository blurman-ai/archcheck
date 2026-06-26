# Lateral cross-module drift criterion (lateral drift)

_2026-06-12. Criterion design based on: (a) a corpus signal/noise check on 21,736
per-commit edges (481 repos), (b) a web literature review (2 agentic recon passes, key
sources VERIFIED). Status: design; corpus validation is the next step._

---

## 1. The problem: `added_edges` measures activity, not drift

Corpus numbers (all `*_graph_drift.jsonl`, window since 2025-05-01):

| | records | share |
|---|---|---|
| Total commits with `added_edges > 0` | 21,736 | 100% |
| Just edges (0 cycles, 0 cross-area) | 20,549 | **95%** |
| Formed a cycle | 200 | 1% |
| — of those, in small commits (≤30 edges, not a vendor dump) | 94 | **0.4%** |
| Cross-area (`new_area_deps`) | 1,049 | 5% |

A typical record is `foo.cpp → Log.hpp / utils/x.h`: a file started using a shared layer.
That's healthy development. The existing `new_area_deps` catches "the first edge between
top-level areas", but doesn't distinguish a peer module from a shared layer — it's noisy on
"referenced util for the first time".

**We need a criterion that singles out the third type of edge:**

1. an edge into a shared layer (`util/`, `common/`, `log`) — NOT drift;
2. an edge inside one's own module — NOT drift;
3. **a lateral edge between parallel peer modules** (`EngineImpl → EngineKernels`,
   `llui → llxml`) — drift: the first link between two previously independent siblings,
   over time turning a set of modules into a tangle.

The key requirement: a "shared layer" is defined by **structural role, not by name** —
a regex over `log|util|common` won't do (in a given project the shared layer might be
`EngineKernels`).

## 2. What the literature gives us (digest of two recon passes)

| Block | Source | What we take |
|---|---|---|
| "De-facto shared layer" | Baldwin/MacCormack/Rusnak (HBS WP 10-059 VERIFIED; Hidden Structure 2014) | **Shared** = high fan-in at low fan-out; threshold is *relative* — 50% of the system maximum (or relative to the largest SCC). **Core** = membership in the largest cyclic group — Shared is NOT core |
| — refinement | Wen/Tzerpos 2005, omnipresent modules (clustering) | The decisive sign is not raw fan-in but **fan-in diversity**: the number of *distinct* consumer modules (cluster degree), with uniformity of usage |
| — edge direction | Martin 1994 (VERIFIED): I = Ce/(Ca+Ce); SDP | An edge A→B is legitimate if I(B) < I(A); a shared layer has I→0. Arcan Unstable Dependency: a smell when I(x) > I(y)+δ, DoUD > 30% |
| "Lateral" = same level | Lakos levelization; Structure101 LSM (VERIFIED docs): "items in the same row do not depend on each other"; Mo/Cai WICSA 2015 (VERIFIED): Implicit Cross-module Dependency defined for "independent modules **within the same layer**" | The vertical coordinate is derived without config: Lakos level (longest path in the condensed DAG). Siblings = identical/close level |
| "A new edge = an event" | **ArchLint** (Maffort et al., EMSE 2016, dissertation VERIFIED) — the only config-free precedent | An edge is suspicious if: (i) atypical for module A (DepScaRate low), (ii) such edges have historically been removed (DepDelRate — a signal of intent), (iii) B has a dominant consumer ≠ A (HeavyUser) |
| Persistence | Li/Liang/Avgeriou mapping study 2022 (VERIFIED) | Erosion ≠ "temporary violations in one or two versions", it's a sustained trend → alert only on an edge that survives k commits |
| Threshold auto-calibration | Arcan docs (VERIFIED) | Threshold = Max(system, benchmark) — frequency analysis instead of constants |
| Worst case | Mo/Cai Cross-Module Cycle (purely structural); Lakos | A lateral edge that closes a module cycle — the highest severity |

**The niche is open:** the 2022 mapping study confirms — edge-level operationalization of
erosion exists only as reflexion divergence (requires a given intended model). Lattix/
Sonargraph/NDepend distinguish utility from lateral only via a manual can-use/public config;
Structure101 automatically catches only cycles. Nobody has assembled the config-free
combination.

A caution from the recon: MacCormack's visibility is transitive — on a C++ include graph the
transitive closure would give "contamination" through header chains. We use **direct**
fan-in diversity; transitivity only for the level (longest path).

## 3. The criterion

### Preparation (per repo, no config)

- **Modules**: directories at depth k; auto-selecting k — the shallowest depth where ≥3
  siblings contain sources (skipping wrapper directories: `src/` with a single child).
  Vendor/third_party/test paths are excluded (rules as in `file_classification.h`).
- **State graph** G(t) — the include graph at the moment *before* commit c; the module graph
  M(t) — aggregation of file edges into module pairs.

### Definitions (everything at the pre-commit state)

- **FID(B)** — fan-in diversity: the number of distinct modules ≠ B that have ≥1 direct edge
  into B.
- **FOD(B)** — the number of distinct modules that B depends on.
- **B — a de-facto shared layer**, if:
  `FID(B) ≥ 0.5 · max_M FID(M)` **and** FOD(B) ≤ the median FOD across modules
  (MacCormack Shared: high-in/low-out, a relative threshold; high-in/high-out is a
  Hub/Core, NOT considered a legitimate target).
- **Level(M)** — the Lakos level in the condensed (SCC → node) module DAG.
- **I(M)** = Ce/(Ca+Ce) — Martin's instability at the module level.

### LATERAL event: commit c adds a file edge from A into B, where

1. A ≠ B, neither is nested in the other (siblings);
2. **B is not a de-facto shared layer** at the moment before c;
3. **the pair A→B did not exist** in any form before c (the first link of the modules —
   otherwise we're again measuring activity);
4. the commit is not a mass-touch (≤150 added edges) and not vendor;
5. **persistence**: the edge is alive through k commits / at the end of the window
   (Li/Liang: erosion is a trend, not a one-day event). Assessed retrospectively;
   in the product's CI mode condition 5 isn't needed — there the gate stands *before* merge.

### Severity (in descending order of authority)

| Grade | Condition | Authority |
|---|---|---|
| **LATERAL.CYCLE** | B→A already existed: the edge closes a module cycle | Lakos; Mo/Cai Cross-Module Cycle (purely structural) |
| **LATERAL.SDP** | I(B) > I(A) + δ: a dependency on something less stable | Martin SDP; Arcan UD (δ and DoUD>30% — their calibration) |
| **LATERAL.NEW** | Just the first lateral link of same-level siblings | Structure101 LSM; Mo/Cai same-layer |

### Confidence amplifiers (optional, per ArchLint)

- HeavyUser(B) ≠ A — the main traffic into B comes from another module;
- A→B edges (or similar) have already been removed in history — a direct signal of intent to
  keep the modules separate (DepDelRate).

## 4. The monotonic-FID problem — a solution from the literature

FID grows over time: when the third or fourth module reaches into B — is it drift, or is B
legitimately *becoming* a shared layer? The answer assembled from three pieces:

1. **The threshold is relative**, not absolute (MacCormack 50% max): B is recognized as
   shared when it catches up with the project's *most shared* module, not with a magic
   number of consumers.
2. **Two-dimensionality**: shared = high-in **and low-out**. If, as its fan-in grows, B
   itself starts depending on neighbors — that's a Hub/tangle, not a layer, and edges into it
   remain suspicious.
3. **Persistence and trend** (Li/Liang): a series of new lateral pairs in B over a short
   window — either the birth of a new layer (FOD stable, consumers diverse, no back edges),
   or the formation of a tangle (back edges appear). They are separated automatically by the
   LATERAL.CYCLE condition.

## 5. Known weaknesses / FP classes

- **File moves** (`git mv` between modules) create false "new pairs".
  Verified (2026-06-12): `--no-renames` sits inside `archcheck --diff` itself
  (`src/git/git_state.cpp`) and was put there deliberately — a rename must yield both
  sides A+D, otherwise the LCX move pool loses functions (#109). The fix has to be not in C++
  but on the analysis side: in `lateral_drift_scan.py` a signature heuristic works
  (basename(from), to) — a "new" edge whose basename+target has already been seen from
  another module is treated as a move and does not generate an event.
- **Small repos**: N < 4 modules — the criterion degenerates, don't apply it.
- **Choice of depth k**: a single level per repo can be coarse for mixed layouts
  (`src/` + `libs/<lib>/src/`); k may need to be chosen per subtree.
- **Bridge headers**: A includes B's header, which forwards into C — the direct edge A→B
  masks the actual link A→C. The include graph doesn't see this; the clang backend (#042)
  will.
- **Newborn modules**: a new module's first edges in every direction — that's its
  formation, not drift; a grace period for a module's first m commits.

## 6. Corpus validation plan

1. Prototype `lateral_drift_scan.py`: module aggregation + FID/level/I at the "before
   commit" state. Implemented (2026-06-12): state = a baseline snapshot at the window start
   (`make_window_baselines.py`: `git ls-tree`+`cat-file` → `archcheck
   --save-graph-baseline`, one run per repo) + a direct replay of the full `added` lists
   from the jsonl. The `removed` lists weren't saved in the jsonl (a loss of ≤5.6% of edges —
   phantoms in the replay graph); consequence: the persistence condition (5) is satisfied
   trivially and isn't really checked. A baseline is possible only for repos remaining on
   disk (~184 of 481; the rest were deleted after regeneration).
2. Run over the 481 jsonl repos: how many LATERAL.{CYCLE,SDP,NEW} remain out of 21,736 —
   expectation: hundreds, not thousands.
3. Eyeball top-30: TP share ≥ 70% (the bar as in #103).
4. Slice agentic vs human within mixed repos (repo fixed effects — design as for
   boolean-drift).
5. Regenerate section A in `EXAMPLES_50.md` per the new criterion.

## 7. Path into the product

Candidate **DRIFT.4 (Lateral)** for `--diff` mode: everything is computed on the include
graph, zero-config (modules and layers are derived), attribution is ready: Lakos
(cycles/levelization), Martin (SDP/instability), MacCormack/Baldwin (shared classification),
Mo/Cai (cross-module patterns). LATERAL.CYCLE — a gate candidate; SDP/NEW — advisory
(the cross-cutting rule: probabilistic = advisory). In CI mode the persistence condition
drops out naturally: the gate fires before merge, there are no "temporary" edges.
</content>
