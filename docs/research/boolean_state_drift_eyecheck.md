# Boolean-State DRIFT — manual verification (eye-check)

**Date:** 2026-06-07 · **Task:** #089
**What was checked:** 22 candidate files from 19 agentic repos (top by number of commits that added bools), selected from 219 "drift files". For each one — reading the code + `git log` of the bool-addition history. The question: **is ONE meaningful structure growing** (real constraint decay), or is this an artifact?

## Honest bottom line

| Verdict | Count out of 22 | Share |
|---|---|---|
| 🟢 **real drift** (one meaningful structure accumulating flags) | **5** | **23%** |
| 🟡 expected config growth (config/options/settings by design) | 7 | 32% |
| 🔴 FP (different structures / generated code / bool in signatures / giant header) | 10 | 45% |

**Conclusion: the "drift at the FILE level" metric is flimsy — precision is ~23% for genuinely harmful drift.** The claim "drift in 55/73 repos (75%)" is heavily inflated: it means "bools were added in the file", not "one structure bloated".

## Why so many misses

1. **Per-file ≠ per-struct.** I counted bools per file. But `datalayer.h` is 6 domain POD structs; `datalayer_extended.h` has a struct per battery model; `LidarOdometry.h` spreads its flags across nested option structs. The file "accumulates", the structures do not. → the main source of FP.
2. **bool in method signatures and locals.** `tree.hpp`/`graph.hpp` (hhds) — `+bool` is mostly `is_valid()`, `operator==`, arguments like `bool follow_subtrees`. `ur_print.hpp`, `python_annotation.h` — locals like `bool first=true;`/`bool found;` in the body. The history scan didn't filter them out (no braces on the line).
3. **Giant multi-class headers.** `nms_objects.h` (6405 lines, 192 commits) — 9 bools smeared across dozens of classes.
4. **Generated code / registries.** `ggml-webgpu-shader-lib.hpp` — a pipeline struct per operation; `ur_print.hpp` — codegen from the API spec.
5. **Config bags (🟡).** They grow into a single structure, but that's by design (PlayerbotAIConfig, couchbase Settings, SentrySettings, carbon CompileOptions, workerd IsolateBase compat-flags). This is not architectural decay.

## 🟢 Confirmed real drift (5)

> One meaningful structure (god-class / widget / domain handle) slowly accretes state/mode flags commit after commit.

- **ToolboxUIElement.h** — [file](file://~/oss_corpus/oss/gwdevhub_GWToolboxpp/GWToolboxdll/ToolboxUIElement.h) — `gwdevhub_GWToolboxpp`. ⭐ Reference case: one UI class, ~24 bool, flags feature by feature over 2 years (titlebar→breakout→mobile→collapse→snap).
- **platform.hpp** — [file](file://~/oss_corpus/oss/oneapi-src_unified-runtime/source/adapters/level_zero/platform.hpp) — `oneapi unified-runtime`. `ur_platform_handle_t_` — every new Level Zero extension adds `bool ...Supported{false}`, 2024→2026, one per commit.
- **engine.hpp** — [file](file://~/oss_corpus/oss/ThomasGhione_chess_engine/engine/engine.hpp) — `ThomasGhione_chess_engine`. The god-class `Engine` accumulates pondering/search coordination flags (2025-10→2026-05).
- **channelrhiview.h** — [file](file://~/oss_corpus/oss/mne-tools_mne-cpp/src/libraries/disp/viewers/helpers/channelrhiview.h) — `mne-cpp`. UI class, ~20 bool, one flag per commit (crosshair/scalebars/butterfly/zscore…).
- **solidity_convert.h** — [file](file://~/oss_corpus/oss/esbmc_esbmc/src/solidity-frontend/solidity_convert.h) — `esbmc`. The god-class converter accumulated mode flags (bound/reentry/pointer/unchecked) on top of state.

## 🟡 Config growth by design (7)

PlayerbotAIConfig.h, couchbase settings.h, SentrySettings.h, godot App.h (automation options), workerd setup.h (V8 compat-flags), carbon compile_subcommand.h (CLI options), donner ImageComparisonTestFixture.h (test params).

## 🔴 FP (10)

datalayer.h, datalayer_extended.h, LidarOdometry.h (flags across different structures); hhds tree.hpp, hhds graph.hpp, esbmc python_annotation.h (bool in signatures/locals); ur_print.hpp, ggml-webgpu-shader-lib.hpp (generated code/registry); nms_objects.h (giant header); MeshLib MRUIStyle.h (widget structs + function parameters).

## What's needed for the metric to stop being flimsy

1. **Attribution by STRUCTURE, not by file** — bind each added bool to a specific struct/class (via hunk context/AST). Removes FP #1 and #3.
2. **Count only fields** — discard bool in method signatures and locals in bodies (as in the static scan — depth-0). Removes FP #2 and part of #4.
3. **Separate config bags (🟡) from content classes (🟢)** — growth is expected for the former. Otherwise the signal drowns in settings.
4. After that — measure "one content structure gained ≥N bool over ≥M commits".

## Relation to the #089 verdict

Bool drift within a single structure **exists and is confirmed by eye** (5 clear cases, ToolboxUIElement/platform.hpp are textbook). But it is **rare and drowns in noise**: raw file-level detection gives ~23% precision, ~45% is junk, ~32% is expected config growth. For this to become a usable signal, it needs per-struct attribution over git history (≈ the direction of #042 + diff parsing), not a file-level counter. The bare "75% of repos" figure is an artifact, which I retract.
