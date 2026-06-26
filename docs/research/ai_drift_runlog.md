# DRIFT runs on AI PRs — working log

This file is the operational journal of the DRIFT research for task
[backlog/future/033](../../backlog/future/033_maj_ai_drift_dataset.md).
Final findings move to [ai_drift_cases.md](ai_drift_cases.md).
Milestone summaries are in [milestones.md](../milestones.md).

**Clones:** `~/oss/<repo>/` (permanent storage, not a sandbox).
**archcheck version:** working tree after the #047 fix (BOM strip).
**Methodology:** clean checkout via `git clean -fdx` + `git checkout -f`
(see [#048](../../backlog/new/048_maj_drift_clean_checkout_methodology.md)
for the reasons). Helper: `/tmp/drift_one.sh`.

## Summary table of all runs (clean checkout)

All rows obtained via the single helper `drift_one.sh`, which:
1. `git clean -fdx <sub>` (including ignored)
2. `git checkout -f <parent>^1 -- <sub>` → archcheck `--save-graph-baseline`
3. `git clean -fdx <sub>` again
4. `git checkout -f <merge> -- <sub>` → archcheck `--drift-baseline`

`parent` is `merge^1`, i.e. the parent of the merge commit on the first side,
so the range contains only the commits of the PR itself.

| Date | Repo | PR | Files | DRIFT.1 | DRIFT.2 | Notes |
|------|------|----|-------|---------|---------|-------|
| 2026-05-29 | LibreSprite/LibreSprite | #581 toolbar/menu/macOS | 1253 | **1** | 0 | shortcut ui→pref |
| 2026-05-29 | proximafusion/vmecpp | #360 asymmetric infra | 232 | 0 | 0 | clean |
| 2026-05-29 | proximafusion/vmecpp | #340 consolidate constants | 232 | 0 | 0 | clean |
| 2026-05-29 | bambulab/BambuStudio | #10794 color cutting/dovetail | 3019 | **2** | 0 | shortcut ui→Widgets ×2 |
| 2026-05-29 | sxs-collaboration/spectre | #7238 Filters::Filter base | 3531 | 0 | 0 | clean (+1352 LOC) |
| 2026-05-29 | sxs-collaboration/spectre | #7244 ZernikeB1 filter | 3531 | 0 | 0 | clean |
| 2026-05-29 | sxs-collaboration/spectre | #7237 GridToInertialJacobian | 3531 | 0 | 0 | clean |
| 2026-05-29 | sxs-collaboration/spectre | #7253 GenerateTetrahedral fix | 3531 | 0 | 0 | clean |
| 2026-05-29 | sxs-collaboration/spectre | #7251 GH Cartoon bases | 3531 | 0 | 0 | clean |
| 2026-05-29 | gwdevhub/GWToolboxpp | #2108 Armory grouping | 375 | 0 | 0 | clean (+150 LOC) |
| 2026-05-29 | gwdevhub/GWToolboxpp | #2156 sort INI sections | 375 | 0 | 0 | clean |
| 2026-05-29 | gwdevhub/GWToolboxpp | #2158 Sort by Map buttons | 375 | 0 | 0 | clean |
| 2026-05-29 | jakildev/IrredenEngine | #798 editor layer system | 665 | 0 | 0 | clean (+427 LOC) |
| 2026-05-29 | jakildev/IrredenEngine | #1200 chunk persistence | 665 | 0 | 0 | clean |
| 2026-05-29 | jakildev/IrredenEngine | #1207 rename ChunkDiskPersistence | 665 | 0 | 0 | clean (pure rename) |
| 2026-05-29 | jakildev/IrredenEngine | #727 render LOD Phase 1 | 666 | **2** | 0 | shortcut system→component_lod, system→lod_utils |
| 2026-05-29 | openmoq/moqx | #327 CrossExecFilter | 67 | 0 | 0 | clean (+1183 LOC, new module) |
| 2026-05-29 | openmoq/moqx | #328 CrossExecFilter relay | 67 | 0 | 0 | clean |
| 2026-05-29 | openmoq/moqx | #198 idle eviction fix | 67 | 0 | 0 | clean |
| 2026-05-29 | aethersdr/AetherSDR | #3065 TCI per-mode overflow | 550 | 0 | 0 | clean |
| 2026-05-29 | aethersdr/AetherSDR | #2390 connect to last radio | 552 | 0 | 0 | clean |
| 2026-05-29 | aethersdr/AetherSDR | #2685 wasVisible-guard | 552 | 0 | 0 | clean |
| 2026-05-29 | OreStudio/OreStudio | #618 ORE instrument pipeline | 4968 | 0 | 0 | clean (+1770 LOC) |
| 2026-05-29 | OreStudio/OreStudio | #558 s2s authentication | 4968 | 0 | 0 | clean |
| 2026-05-29 | OreStudio/OreStudio | #588 composite instrument | 4968 | 0 | 0 | clean (+9762 LOC!) |
| 2026-05-29 | OreStudio/OreStudio | #547 E2E lifecycle refactor | 4968 | 0 | 0 | clean |
| 2026-05-29 | EtherAura/Kartend | #26 errorutils refactor | 693 | 0 | 0 | clean |
| 2026-05-29 | EtherAura/Kartend | #34 covers leaf-struct | 693 | 0 | 0 | clean |
| 2026-05-29 | EtherAura/Kartend | #27 promote uiconstants | 693 | **5** | 0 | shortcut data→ui/uiconstants ×4 + data→utils/view |
| 2026-05-29 | community-shaders/skyrim | #2326 fix singleton ptr | 229 | **1** | 0 | shortcut State.cpp → Features/InteriorSun |
| 2026-05-29 | community-shaders/skyrim | #2207 refactor common Util | 230 | **1** | 0 | shortcut VRStereoOpt → Utils/UI |
| 2026-05-29 | community-shaders/skyrim | #2205 perf VR DLSS | 230 | 0 | 0 | clean |
| 2026-05-29 | community-shaders/skyrim | #2077 build version proposals | 230 | 0 | 0 | clean |

## Corpus summary

- **33 PRs** checked across **10 repositories**.
- **7 PRs with DRIFT.1 hits** (12 findings total): LibreSprite #581,
  BambuStudio #10794, IrredenEngine #727, Kartend #27, Skyrim #2326, #2207
  → confirmed by grep against file-content.
- **26 clean PRs** — 0 DRIFT.1, 0 DRIFT.2.
- **DRIFT.2 (cycles) never fired.** Not a single AI commit in the corpus
  introduced a new include cycle. Possible interpretations:
  - cycles are rare even in uncontrolled C++ code
  - our threshold largest_scc>=2 is strict — a small inversion does not count as a cycle
  - the corpus is small (33 PRs) for statistics

## Archetypes of DRIFT.1 findings in the corpus

1. **UI → low-level widgets** (BambuStudio #10794, Kartend #27 partially,
   LibreSprite #581) — a dialog/widget drags in the include of a concrete lower-level
   component instead of an abstraction.
2. **Generic → Features** (Skyrim #2326, #2207) — generic code pulls the include
   of a concrete feature module that was previously isolated.
3. **System → Component** (IrredenEngine #727) — an ECS system starts
   importing a component instead of requesting through a query.
4. **UI-config → core data** (LibreSprite #581) — the reverse direction,
   a widget reads the preferences layer directly.

All 4 archetypes are exactly the classes of findings that DRIFT.1
was designed for.

## Clean PRs — what 0 DRIFT tells us

26 of 33 PRs passed without drift. This is a useful negative signal:
**DRIFT.1 is not a false-positive noise generator**.

The large PRs are especially telling:
- spectre #7238: **+1352 LOC** — a new Filters::Filter base class — clean
- OreStudio #588: **+9762 LOC** — composite/scripted instrument domain model — clean
- moqx #327: **+1183 LOC** — a new CrossExecFilter module — clean
- IrredenEngine #798: **+427 LOC** — editor layer system — clean

Clean large PRs prove that DRIFT.1 distinguishes "new code within
its own boundaries" (OK) from "existing code crossing new boundaries" (drift).

## Bugs found in archcheck during the run

- **#047 (closed)** — UTF-8 BOM not stripped, breaks DRIFT.1 on
  wxWidgets/MSVC projects. Discovered on BambuStudio. Fix applied.
- **#048 (open)** — methodological trap: a dirty working tree gives
  mass false positives in DRIFT. Requires a helper script +
  possibly an `archcheck --diff` mode with git worktree isolation.

## Candidates in the queue (deferred)

- apache/thrift (13 AI PRs, but almost all non-C++)
- dancinlab/hexa-lang (24 PRs, but a Python project)
- raspberrypi/pico-sdk (3 PRs, all +1 LOC)
- Tier 2 from task #033: PX4 (forbids AI attribution),
  nodos-dev/sys-device (1 file)

## Methodology: clean checkout is mandatory (2026-06-11)

### Cause

`git checkout <sha> -- <path>` updates only files that exist in `<sha>`. Files that are present in the working tree (from a previous loop iteration or from master HEAD) but absent in `<sha>` remain in the working tree. `git clean -fd` removes only untracked files; tracked files from other revisions that are recorded in the index are not removed. Result: during the scan `archcheck --save-graph-baseline` sees files that should not be in the `parent` revision, and saves their edges. Then on the `after` checkout these files may disappear or remain. If the file list or their edges change — DRIFT falsely fires.

### Empirical data

| Repo | PR | Dirty | Clean | Delta |
|------|----|-------|-------|-------|
| Kartend | #26 errorutils refactor | 26 DRIFT.1 | 0 | -26 (all FP) |
| Kartend | #27 promote uiconstants | 5 DRIFT.1 | 5 DRIFT.1 | 0 (real) |
| Kartend | #34 covers leaf-struct | 0 | 0 | 0 |
| OreStudio | #547 service-to-service auth | 1 DRIFT.1 | 0 | -1 (FP) |
| OreStudio | #558 sql isolation | 0 | 0 | 0 |
| OreStudio | #588 composite instrument | 2 DRIFT.1 | 0 | -2 (FP) |
| OreStudio | #618 ORE instrument | 4 DRIFT.1 | 0 | -4 (FP) |
| IrredenEngine | #727 render LOD | 2 DRIFT.1 | 2 DRIFT.1 | 0 (real) |
| vmecpp | #360, #340 | 0 | 0 | 0 |

### Rule

Any DRIFT run on a foreign repo — only via `scripts/drift_run.sh`:

```bash
scripts/drift_run.sh <repo-path> <subdir> <before-sha> <after-sha> <label>
```

The script guarantees a clean state before each revision via `git clean -fdx` + `git checkout -f`.

## Reproduction commands

```bash
# One PR:
bash scripts/drift_run.sh <repo-path> <subdir> <before-sha> <after-sha> <label>

# Example:
bash scripts/drift_run.sh ~/oss/LibreSprite src \
    5f0fcd28ab2d5e74e0fa5b0e7bda79d8fafcc61a 276fdbdb27b537a074c3e170af6afc88c244a539 libresprite_581
```
