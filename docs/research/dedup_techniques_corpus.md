# Deduplication techniques in C++ open-source projects

Empirical grounding for defining **TP** (what archcheck must count as genuine
copy-paste): for projects whose share of duplicated code (`dup_ratio` from
`line_duplication`) decreased over time, the git history was used to establish **which
engineering techniques** developers used to remove copy-paste. So this is not clone theory
(that's in [code_clones.md](code_clones.md)) and not an FP catalog (that's
in [../duplication_fp_analysis.md](../duplication_fp_analysis.md)), but ground truth from
the other side: real commits where the duplication was recognized and factored out.

In the spirit of "authority over opinion": every technique is confirmed via a `git show` of a real
commit — and that is precisely the authority of Lakos's practice over our opinion.

> **Provenance.** The data was obtained in a corpus run (#079/#080) whose artifacts
> live locally outside git (`experiments/` — not tracked). The source of dates/windows at the time:
> `runs_history/all/<repo>.tsv` + the summary `drift_summary_v2.tsv`; commits were verified
> in the clones `~/oss/_aidev_run/<repo>`. Preserved here is the result — the tables and
> conclusions that should not be lost together with the local dumps.

The theme is physical design in the spirit of John Lakos: lifting common code up the levels,
eliminating duplicate implementations across platforms/types/backends.

---

## What was counted as real deduplication

| Project | Date window | dup% | Commit SHA | Technique | What exactly was extracted / removed | Commit text |
|---|---|---|---|---|---|---|
| **alpaka** | 2022-02 (idx10→11) | ~33→32* | `f060ddf7` | extract-to-common-header | Common CUDA/HIP code extracted into `CudaHipCommon.hpp`; `Cuda.hpp` −376, `Hip.hpp` −356 (net −729/+362). One source of the `DimType`/builtin-type traits instead of two copies | "refactor common CUDA/HIP code into common header" |
| **alpaka** | 2022-03 (idx15→16) | 23.6→22.5 | `7559f91c` | template parameterization | Two classes `QueueUniformCudaHipRtBlocking` and `…NonBlocking` (~160 lines of traits each) → one `QueueUniformCudaHipRt<bool TBlocking>` + `using` aliases (−481/+313) | "Refactor CUDA/HIP queues to reduce code duplication" |
| **alpaka** | 2022-11 (idx16→17) | 22.5→20.3 | `19bed293` | merge template specialization | The partial specialization `ConcurrentExecPool<…,false>` merged with the primary template (−223/+88, "pure refactoring, should not change behavior") | "Merge ConcurrentExecPool primary template with specialization" |
| **vgmtrans** | 2026-01 (idx≈18→19) | 15.6→14.3 | `1d92b84` | base-class API (NVI) | ~8 `*Samp` classes (MP2k/NDS/PSX/SNES/Konami/Dialogic…) each had their own `convertToStdWave(buf)` with the same boilerplate (allocate buffer → decode). A single non-virtual `convertToStdWave()` was introduced in the base + a protected `virtual decodeToNativePcm()` overridden by the subclasses | "VGMSamp refactor (part 1) (#722)" |
| **vgmtrans** | 2024-04 (idx≈17→18) | 16.3→15.6 | `be35fce` | extract common widget | A common `TableView` (widget) for `VGMFileListView` and `RawFileListView`; appearance/header logic lifted into the base, `VGMFileListView.cpp` −52 | "add TableView widget to consolidate appearance and header logic …" |
| **Effekseer** | 2022→2024 (idx21→22) | 36.2→29.0 | `d1fece84` | extract-to-common-module | The vertex-buffer logic of 5 render backends (DX9/DX11/GL/Metal/Vulkan) consolidated into `EffekseerRendererCommon` (40 files, −1633/+528) | "Refactor vertex buffer (#935)" |
| **Effekseer** | 2022→2024 (idx21→22) | 36.2→29.0 | `5564dccb` | extract-to-common-module | A common `VertexLayout` instead of per-backend copies (30 files, −924/+718) | "Refactor VertexLayout (#932)" |
| **acts** | 2024-03→06 (idx14→15) | 10.4→8.5 | `e4a3ec90a` | extract-to-common-header + helper class | 3 nearly identical benchmark files (`Atlas/Eigen/StraightLineStepperBenchmark.cpp`, ~126 lines of copy-paste each) → ~10 lines each via a common `StepperBenchmarkCommons.hpp` (class `BenchmarkStepper`: `parseOptions/makeField/run`) (−343/+209) | "refactor: Share code between stepper benchmarks (#3162)" |
| **acts** | 2024-03→06 (idx14→15) | 10.4→8.5 | `357480730` | extract-function | Duplicated track storage in the Core CKF extracted into a common function | "refactor: Common function to store tracks in Core `CKF` (#3164)" |
| **acts** | 2024-03→06 (idx14→15) | 10.4→8.5 | `eb8677409` | extract-function (move impl) | Boundary-check implementations moved into a common `VerticesHelper` | "refactor: Move boundary check implementations to `VerticesHelper` (#3205)" |
| **oneDAL** | 2021-06→08 (idx10→11) | 37.2→33.6 | `80d98e7c2` | delete-dead / legacy-cleanup | Entire deprecated algorithms removed (adaboost/brownboost/logitboost of the old API) that duplicated the new interfaces: −40049/+186, 259 `.cpp` + 190 `.h`. The main cause of the sustained dup drop and the node count falling 3479→3295 | "Clean old interfaces (#1611)" |
| **oneDAL** | 2021-06→08 (idx10→11) | 37.2→33.6 | `09354a9ac` | parametrize / unify | Unification of `label`→`response` for svm/kmeans/knn/df (94 files) — a single parameter instead of algorithm-specific copies | "replace of label with response for svm, kmeans, knn, df (#1755)" |
| **FastLED** | 2025-04 (idx9→11) | 25.4→20.3 | `e25c69209` | extract + template | Numerous path classes (Line/Circle/Heart/Rose/Phyllotaxis/Gielis…) that inherited `XYPathGenerator` were reorganized and generalized via `function<T>`, extracted into `xypaths.{h,cpp}` (−473/+513) | "Refactor XYPath classes for common path generation" |
| **ESPAsyncWebServer** | 2025-10 (idx≈19→20) | 10.0→7.7 | `cf4f12232` | encapsulate / extract class | The URI-matching logic (repeated for regex and non-regex paths) encapsulated into an `AsyncURIMatcher` class; `WebHandlers.cpp` −38. The codebase is tiny (~26 nodes) — the technique is real, the contribution to the metric small | "Introduce AsyncURIMatcher class … with and without regex support" |

\* alpaka: the main dup decline is spread across 2021–2023 (33.7%→17.4%); the commits with the clearest dedup effect are listed. The idx10→11 boundary is the point of the primary/specialization merge and the common header.

---

## Flagged separately — NOT counted as manual deduplication (artifacts / noise)

| Project | Why it wasn't counted |
|---|---|
| **azure-sdk-for-cpp** | The claimed "41.5%→31.4%" is misleading: idx1 (41.5%) is a single outlier on an early, small codebase (71 nodes), and the real range is stable ~30–33% throughout the history. **Code generation** dominates: `077d32ff` "New protocol layer for blob/datalake/queue generated by CodeGen (#3261)" (+17795/−17769) — duplication between REST clients is now generated from a specification, not removed manually. There is no clear manual dedup window. |
| **vgmtrans `8743874` "Remove common.h"** | 482 files, +10911/−9841 — a mass replacement of type shorthands (`u8`→`uint8_t`), stylistic normalization, NOT logic deduplication. Not counted (but other vgmtrans commits were counted). |
| **PcapPlusPlus** | dup flat at ~9–11% throughout the history (delta −1.8) — measurement noise, no pronounced deduplication. |
| **cpprestsdk** | dup flat at ~18–20% throughout the history (delta −1.6) — noise, no dedup window. |
| **ESPAsyncWebServer (the metric)** | Codebase of 8–27 nodes; `dup_ratio` is noisy (a single 13% outlier at idx0, a rise to 11% when fork files were added). The technique (AsyncURIMatcher) is real, but its contribution to the percentage is unstable — medium confidence. |
| Skipped by topic (security / reverse-engineering / game-mod), as requested | SDRPlusPlusBrown, Nidhogg, HexRaysCodeXplorer, skyrim-community-shaders, Universal-Dear-ImGui-Hook, zed-open-capture. |

---

## Summary statistics of the techniques (counted commits)

Total projects with real deduplication analyzed: **7**
(alpaka, vgmtrans, Effekseer, acts, oneDAL, FastLED, ESPAsyncWebServer).
Plus **1 flagged as an artifact** (azure-sdk-for-cpp).

Technique frequency across the counted commits (13 commits):

| Technique | Count | Commits |
|---|---|---|
| **extract-to-common-header / -module** (lift common code up) | **5** | alpaka `f060ddf7`, Effekseer `d1fece84` `5564dccb`, acts `e4a3ec90a`, FastLED `e25c69209` |
| **template parameterization / merge specialization** (copies per type/platform → one template) | **2** | alpaka `7559f91c` `19bed293` |
| **base-class / common-widget / encapsulate** (lift the common part into a base/class) | **3** | vgmtrans `1d92b84` (NVI) `be35fce`, ESPAsyncWebServer `cf4f12232` |
| **extract-function** (repetition → one function/helper) | **2** | acts `357480730` `eb8677409` |
| **delete-dead / legacy-cleanup** (removal of duplicating obsolete code) | **1** | oneDAL `80d98e7c2` |
| **parametrize / unify a parameter** (N cases → one parameterized) | **1** | oneDAL `09354a9ac` |
| X-macro / manual codegen | 0 | (azure codegen — an artifact, not a manual technique) |

**The dominant technique is extract-to-common-header/module** (lifting common code into a common
header/module up the physical hierarchy). This is the core of the Lakos methodology: common code
should live at a lower level that the rest depend on.

The second-strongest motive is **generalizing copies across "axes of variability"**: platforms
(CUDA/HIP in alpaka), graphics backends (DX/GL/Metal/Vulkan in Effekseer), algorithm types
(oneDAL), geometric curves (FastLED). In every case, N nearly identical implementations are reduced to
one via a template, a base class, or a common module.

---

## Standout examples

1. **acts `e4a3ec90a` — extract-to-common-header.** Three stepper benchmarks
   (`AtlasStepperBenchmark.cpp`, `EigenStepperBenchmark.cpp`,
   `StraightLineStepperBenchmark.cpp`) were ~126 lines of copy-paste each. A `BenchmarkStepper`
   class (`parseOptions/makeField/run`) was extracted into `StepperBenchmarkCommons.hpp` — each
   file shrank to ~10 lines. −343/+209.

2. **alpaka `7559f91c` — template parameterization of platform classes.**
   `QueueUniformCudaHipRtBlocking` and `…NonBlocking` (each with their own DevType/EventType
   traits) replaced by `QueueUniformCudaHipRt<bool TBlocking>` + `using` aliases.
   Additionally, `f060ddf7` extracted all the common CUDA/HIP code into `CudaHipCommon.hpp` (−729 lines).

3. **Effekseer `d1fece84` / `5564dccb` — a common module for N graphics backends.**
   Vertex buffers and VertexLayout, duplicated across the DX9/DX11/GL/Metal/Vulkan renderers,
   lifted into `EffekseerRendererCommon`. In total −2557/+1246 across 70 files.

4. **oneDAL `80d98e7c2` — delete-dead.** "Clean old interfaces": entire
   obsolete algorithms removed (adaboost/brownboost/logitboost) that duplicated the new API.
   −40049/+186 — the single commit explaining the sustained break in dup_ratio 37.2%→33.6%,
   which holds for years.

---

## What this gives archcheck

Each counted technique is a **TP that someone actually factored out**: if archcheck had
scanned the previous snapshot, it would have been obligated to show this pair. The hallmark of genuine
copy-paste in this sample is *extractability*: the duplication is removed by one of the six techniques
above (see also the extractability-test rule in mem:`fp_classification_rules`). What cannot be
removed (idioms, generated code, stylistic normalization like `u8`→`uint8_t`) is in the
right-hand "not counted" table and overlaps with the FP classes from
[../duplication_fp_analysis.md](../duplication_fp_analysis.md).
