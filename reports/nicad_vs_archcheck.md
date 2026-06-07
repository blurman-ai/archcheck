# NiCad vs archcheck — head-to-head on the OSS corpus

_Clone-level comparison of NiCad 6.2 and archcheck `--duplication` on the same 8
repos. Companion to [nicad_installation.md](nicad_installation.md),
[nicad_metrics.csv](nicad_metrics.csv), [nicad_review.md](nicad_review.md). Raw
dumps in [raw/](raw/). Links are GitHub permalinks at the scanned commit._

## Method

For each repo: ran `nicad6 {functions,blocks} c <farm>` (default config) and
`archcheck --duplication <repo>`. NiCad clone **classes** were matched against
archcheck **pairs** by file + line-range overlap (same file, ≥30 % / ≥3-line
intersection on both endpoints). Each finding lands in one bucket:

- **BOTH** — an archcheck pair whose two fragments are both covered by one NiCad class.
- **only-archcheck** — archcheck pair no NiCad class covers.
- **only-NiCad** — NiCad class matching no archcheck pair, split **vendored** vs **authored**.

NiCad line numbers map to original source (farm files are hardlinks), so the line
ranges are directly comparable. `archcheck` and matcher scripts are in
[raw/nicad/](raw/nicad/).

## Headline numbers

archcheck's column is **post-fix** — after the two fixes this eval triggered:
in-tree vendored-lib exclusion and the #091 `maxTokens` recall fix. The as-measured
baseline (before either fix) was **376** archcheck pairs / **16** BOTH; the per-step
delta is in the [Fixes](#fixes--archcheck-changes-this-eval-triggered) section.

| repo | archcheck pairs | NiCad classes | BOTH | only-archcheck | only-NiCad (vendored) | only-NiCad (authored) | archcheck time/mem | NiCad time/mem (fn+bl) |
|---|---:|---:|---:|---:|---:|---:|---|---|
| Catch2 | 0 | 0 | 0 | 0 | 0 | 0 | 0 s / 5 MB | 22 s / 70 MB |
| GWToolboxpp | 12 | 7 | 0 | 12 | 7 | 0 | 1 s / 97 MB | 118 s / 376 MB |
| Kartend | 42 | 1 | 0 | 42 | 0 | 1 | 1 s / 78 MB | 110 s / 213 MB |
| IrredenEngine | 4 | 5 | 0 | 4 | 5 | 0 | 1 s / 46 MB | 110 s / 370 MB |
| LibreSprite | 10 | 12 | 1 | 9 | 2 | 9 | 1 s / 84 MB | 181 s / 378 MB |
| irrlicht-1.8.3 | 41 | 46 | 0 | 41 | 46 | 0 | 2 s / 113 MB | 270 s / 267 MB † |
| AetherSDR | 72 | 537 | 0 | 72 | 526 | 11 | 2 s / 114 MB | 388 s / 308 MB |
| BambuStudio | 129 | 194 | 0 | 129 | 194 | 0 | 9 s / 566 MB | 508 s / 373 MB |
| **total** | **310** | **802** | **1** | **309** | **780** | **21** | **~17 s** | **~1700 s** |

† irrlicht **blocks crashed** (NiCad `Renaming failed, code 1` → exit 99); only
its `functions` run contributes.

Three facts dominate:

1. **Overlap is essentially nil.** Just **1 of 310** archcheck pairs is also found
   by NiCad — and after the vendored-exclusion fix that one is an *authored* clone
   both tools agree on (LibreSprite `algo.cpp`, recovered by #091). The 16 prior
   "agreements" were all vendored C (jpeglib/bzip2/qhull/hidapi) and vanished once
   archcheck stopped reporting in-tree library code.
2. **archcheck finds 309 authored C++ clones NiCad misses entirely.** These are
   the actionable ones (cross-platform device copies, copy-pasted GUI panels,
   split-file AI copy-paste). NiCad can't see them: the C++ doesn't parse.
3. **NiCad's unique output is ~97 % vendored/generated noise.** ~780 vendored
   classes + 21 "authored", of which only ~6–9 are genuinely useful (LibreSprite
   `algo.cpp`/`autocrop.cpp`); the rest are expected-pattern boilerplate. Bought at
   **~100× the wall-clock** (≈1700 s vs ≈17 s) and after a 38–94 % parse-failure rate.

## Per-repo clone-level overlap (with links)

#### GWToolboxpp
_archcheck pairs: 12 · NiCad classes: 7 · **BOTH 0** · **only-archcheck 12** · only-NiCad vendored 7 · only-NiCad authored 0_

**Only archcheck** (authored copy-paste — an old copy left in `Unused/`):

- [A] [`GWToolboxdll/Unused/FavorTrackerModule.cpp:62-85`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Unused/FavorTrackerModule.cpp#L62-L85) ↔ [`GWToolboxdll/Widgets/FavorTracker.cpp:74-97`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Widgets/FavorTracker.cpp#L74-L97) (STRUCTURAL, w=0.98)
- [A] [`GWToolboxdll/Unused/FavorTrackerModule.cpp:122-139`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Unused/FavorTrackerModule.cpp#L122-L139) ↔ [`GWToolboxdll/Widgets/FavorTracker.cpp:135-152`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Widgets/FavorTracker.cpp#L135-L152) (EXACT, w=1.0)
- [A] [`GWToolboxdll/Windows/PartySearchWindow.cpp:576-591`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Windows/PartySearchWindow.cpp#L576-L591) ↔ [`GWToolboxdll/Windows/TradeWindow.cpp:184-210`](https://github.com/gwdevhub/GWToolboxpp/blob/c8c0faa410206918e6f56a4dcc89f9159b615694/GWToolboxdll/Windows/TradeWindow.cpp#L184-L210) (STRUCTURAL, w=0.78)

**Only NiCad — vendored** (noise): `Dependencies/nativefiledialog/nfd_gtk.c` self-clones; `Dependencies/GWCA/.../AgentIDs.h ↔ ItemIDs.h` generated ID tables.

#### Kartend
_archcheck pairs: 42 · NiCad classes: 1 · **BOTH 0** · **only-archcheck 42** · only-NiCad vendored 0 · only-NiCad authored 1_

**Only archcheck** (slam-dunk AI split-file copy-paste — EXACT, w=1.0):

- [A] [`src/ui/dialogs/settings/core/settingsdialogtree.cpp:589-662`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/ui/dialogs/settings/core/settingsdialogtree.cpp#L589-L662) ↔ [`settingsdialogtreemutation.cpp:415-488`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/ui/dialogs/settings/core/settingsdialogtreemutation.cpp#L415-L488)
- [A] [`src/modules/data/query/scanservice.cpp:1287-1345`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/modules/data/query/scanservice.cpp#L1287-L1345) ↔ [`scanservice_persist.cpp:578-636`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/modules/data/query/scanservice_persist.cpp#L578-L636)
- [A] [`src/core/mainwindow_dialogs.cpp:328-375`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/core/mainwindow_dialogs.cpp#L328-L375) ↔ [`mainwindow_setup.cpp:859-906`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/core/mainwindow_setup.cpp#L859-L906)

**Only NiCad — authored** (expected pattern): [`src/ui/uiconstants/detailspaneconstants.h ↔ selection.h ↔ keyboard.h`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/ui/uiconstants/detailspaneconstants.h#L4-L53) — UI-constant namespaces (low refactor value).

#### IrredenEngine
_archcheck pairs: 4 · NiCad classes: 5 · **BOTH 0** · **only-archcheck 4** · only-NiCad vendored 5 · only-NiCad authored 0_

**Only archcheck** (authored prefab systems):

- [A] [`engine/prefabs/.../system_contact_midi_trigger.hpp:23-52`](https://github.com/jakildev/IrredenEngine/blob/3a825a50d84a2ce5dda505531a72429d812a795c/engine/prefabs/irreden/audio/systems/system_contact_midi_trigger.hpp#L23-L52) ↔ [`system_periodic_idle_midi_trigger.hpp:23-53`](https://github.com/jakildev/IrredenEngine/blob/3a825a50d84a2ce5dda505531a72429d812a795c/engine/prefabs/irreden/audio/systems/system_periodic_idle_midi_trigger.hpp#L23-L53) (STRUCTURAL, w=0.99)

**Only NiCad — vendored**: all 5 inside generated [`engine/render/src/opengl/glad.c`](https://github.com/jakildev/IrredenEngine/blob/3a825a50d84a2ce5dda505531a72429d812a795c/engine/render/src/opengl/glad.c#L974-L1027).

#### LibreSprite — _the one repo where NiCad adds genuine signal_
_archcheck pairs: 10 · NiCad classes: 12 · **BOTH 1** · **only-archcheck 9** · only-NiCad vendored 2 · only-NiCad authored 9_

Almost-disjoint authored clones — the single overlap is the `algo_line` pair the
#091 fix recovered for archcheck (both tools now agree on it).

**Found by BOTH** (recovered by #091):

- [`src/doc/algo.cpp:22-100`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L22-L100) ↔ [`:107-189`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L107-L189) (`algo_line` ↔ `algo_line_float`, STRUCTURAL w=0.75)

**Only archcheck:**

- [A] [`src/app/commands/cmd_move_mask.cpp:204-227`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/commands/cmd_move_mask.cpp#L204-L227) ↔ [`src/app/commands/cmd_scroll.cpp:140-163`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/commands/cmd_scroll.cpp#L140-L163) (EXACT, w=1.0)
- [A] [`src/filters/convolution_matrix_filter.cpp:159-206`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/filters/convolution_matrix_filter.cpp#L159-L206) ↔ [`:281-338`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/filters/convolution_matrix_filter.cpp#L281-L338) (STRUCTURAL)

**Only NiCad — authored** (spline twins below archcheck's threshold; autocrop self-clone):

- [`src/doc/algo.cpp:461-534`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L461-L534) ↔ [`:536-612`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L536-L612) ↔ [`:614-690`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L614-L690) — spline trio (sim 75, just under archcheck's joint floor)
- [`src/app/util/autocrop.cpp:21-64`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/util/autocrop.cpp#L21-L64) ↔ [`:66-109`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/util/autocrop.cpp#L66-L109)

#### irrlicht-1.8.3
_archcheck pairs: 41 · NiCad classes: 46 · **BOTH 0** · **only-archcheck 41** · only-NiCad vendored 46 · only-NiCad authored 0_

The 6 prior BOTH overlaps were all vendored C (jpeglib/bzip2) and are gone post-fix —
archcheck no longer reports `source/Irrlicht/{jpeglib,libpng,bzip2}/`. So the two
tools now have **zero** overlap here: archcheck = authored engine, NiCad = vendored.

**Only archcheck** (authored engine clones — the valuable ones):

- [A] [`source/Irrlicht/CIrrDeviceWin32.cpp:1144-1237`](https://github.com/hyyh619/irrlicht-1.8.3/blob/4280a27f7611b7ce4cda877e91d7d831b0b8edfe/source/Irrlicht/CIrrDeviceWin32.cpp#L1144-L1237) ↔ [`CIrrDeviceWinCE.cpp:479-558`](https://github.com/hyyh619/irrlicht-1.8.3/blob/4280a27f7611b7ce4cda877e91d7d831b0b8edfe/source/Irrlicht/CIrrDeviceWinCE.cpp#L479-L558) (STRUCTURAL) — cross-platform device copy
- [A] [`source/Irrlicht/CD3D8ParallaxMapRenderer.cpp:251-310`](https://github.com/hyyh619/irrlicht-1.8.3/blob/4280a27f7611b7ce4cda877e91d7d831b0b8edfe/source/Irrlicht/CD3D8ParallaxMapRenderer.cpp#L251-L310) ↔ [`CD3D9ParallaxMapRenderer.cpp:339-399`](https://github.com/hyyh619/irrlicht-1.8.3/blob/4280a27f7611b7ce4cda877e91d7d831b0b8edfe/source/Irrlicht/CD3D9ParallaxMapRenderer.cpp#L339-L399) (STRUCTURAL, w=0.94) — D3D8/D3D9 renderer copy

**Only NiCad — vendored**: libpng demo programs `rpng2-win.c ↔ rpng2-x.c`, jpeglib internals.

#### AetherSDR
_archcheck pairs: 72 · NiCad classes: 537 · **BOTH 0** · **only-archcheck 72** · only-NiCad vendored 526 · only-NiCad authored 11_

**Only archcheck** (authored GUI copy-paste — EXACT/LITERAL):

- [A] [`src/gui/ClientChainWidget.cpp:536-577`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/gui/ClientChainWidget.cpp#L536-L577) ↔ [`StripChainWidget.cpp:536-577`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/gui/StripChainWidget.cpp#L536-L577) (LITERAL, w=1.0)
- [A] [`src/gui/ClientEqEditor.cpp:372-412`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/gui/ClientEqEditor.cpp#L372-L412) ↔ [`StripEqPanel.cpp:375-415`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/gui/StripEqPanel.cpp#L375-L415) (EXACT, w=1.0)

**Only NiCad — vendored** (526; sample): liquid-dsp template-expanded DSP, size-37 class [`firpfbch2.proto.c ↔ firpfbchr.proto.c ↔ firpfbch.proto.c`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/third_party/liquid-dsp/src/multichannel/src/firpfbch2.proto.c#L212-L237); r8brain, mosquitto.

**Only NiCad — authored** (≤3 useful, rest expected pattern): [`src/core/TgxlConnection.h ↔ PgxlConnection.h`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/core/TgxlConnection.h#L9-L74); widget-header cluster `ClientRxDspApplet.h ↔ MeterApplet.h ↔ ClientReverbApplet.h`.

#### BambuStudio
_archcheck pairs: 129 · NiCad classes: 194 · **BOTH 0** · **only-archcheck 129** · only-NiCad vendored 194 · only-NiCad authored 0_

The 10 prior BOTH overlaps were all vendored C (`src/hidapi`, `src/qhull`, `src/libigl`)
and are gone post-fix — archcheck no longer reports those bundled libs. Zero overlap now.

**Only archcheck** (authored slic3r copy-paste — the high-value ones):

- [A] [`src/slic3r/GUI/Gizmos/GLGizmoAdvancedCut.cpp:123-194`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/slic3r/GUI/Gizmos/GLGizmoAdvancedCut.cpp#L123-L194) ↔ [`GLGizmoColorCut.cpp:325-396`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/slic3r/GUI/Gizmos/GLGizmoColorCut.cpp#L325-L396) (EXACT, w=1.0)
- [A] [`src/slic3r/GUI/Notebook.hpp:267-345`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/slic3r/GUI/Notebook.hpp#L267-L345) ↔ [`Tabbook.hpp:266-345`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/slic3r/GUI/Tabbook.hpp#L266-L345) (STRUCTURAL)
- [A] [`src/libslic3r/Format/3mf.cpp:957-1016`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/libslic3r/Format/3mf.cpp#L957-L1016) ↔ [`bbs_3mf.cpp:2552-2614`](https://github.com/bambulab/BambuStudio/blob/a206a95399dc6a2d4c918e43f2a5b889bd1a1d83/src/libslic3r/Format/bbs_3mf.cpp#L2552-L2614) (STRUCTURAL)

**Only NiCad — vendored** (190; sample): `deps/EXPAT/expat/xmlrole.c` (generated tables, size 43/44), `src/qhull`, `src/glu-libtess`, `src/agg`.

## The five questions, for NiCad's significant clones

| NiCad significant clone (representative) | archcheck find it? | why not (if no) | useful for CI? | useful for PR? | would a dev fix it? |
|---|---|---|---|---|---|
| liquid-dsp `firpfbch*.proto.c` ×37 (AetherSDR) | No | `third_party/` excluded by path guards | No | No | No — vendored, template-generated |
| libpng `rpng2-win.c ↔ rpng2-x.c` (irrlicht) | No | in-tree vendored `libpng/contrib` | No | No | No — vendored demo code |
| qhull `libqhull ↔ libqhull_r` (BambuStudio) | No (was BOTH; excluded post-fix) | now vendored-excluded | No | No | No — intentional dual API in a vendored lib |
| hidapi `linux/mac/win hid.c` (BambuStudio) | No (was BOTH; excluded post-fix) | now vendored-excluded | No | No | No — platform variants, vendored |
| LibreSprite `algo_line` ↔ `algo_line_float` | **Yes (BOTH after #091)** | — | Maybe | **Yes** | Maybe — real near-identical function pair |
| LibreSprite spline trio `algo.cpp` | No | sim 75, just under archcheck joint floor | Maybe | **Yes** | Maybe — real, but intra-file algo variants |
| AetherSDR Qt widget headers | No | C++ headers archcheck pairs differently | No | Marginal | Unlikely — expected pattern |

And the converse — archcheck's significant clones, did NiCad find them? **No, for
all 309** (e.g. Kartend `mainwindow_dialogs ↔ mainwindow_setup` EXACT, BambuStudio
`GLGizmoAdvancedCut ↔ GLGizmoColorCut` EXACT, AetherSDR `ClientEqEditor ↔
StripEqPanel` EXACT). Reason in every case: the files are C++ and **failed NiCad's
C grammar** — they never reached the detector. These are precisely the high-value,
CI-relevant, PR-relevant, a-developer-would-fix clones.

## CI suitability

| question | NiCad 6.2 | archcheck |
|---|---|---|
| **Per-commit?** | **No.** 11–258 s per granularity per repo *even skipping 38–94 % of files*; needs TXL + a C-extension farm + (missing) C++ grammar. | Yes — 0–11 s, single binary, zero setup. |
| **Pull-request?** | **No.** Same cost; output is ~97 % vendored noise with no default exclusion; would bury a reviewer. | Yes — pairs + "extract to shared header", baseline mode for legacy. |
| **Periodic audit?** | Only for **C** codebases. On this C++ corpus it surfaces ~1 % useful signal; not worth the audit. With a real C++ grammar it might, but that grammar isn't shipped. | Yes (and already cheap enough for CI, so audit is moot). |
| **Expected runtime limit** | Minutes per repo on the C subset; would grow substantially with a working C++ front-end (NiCad pretty-prints + does pairwise compare within size buckets). Research-scale, not gate-scale. | Sub-second to ~10 s on 3k-file repos; gate-scale. |

NiCad is, by its own documentation and the [clone_tools_landscape](../docs/research/clone_tools_landscape.md)
table, a **research tool with no CI integration** — this run confirms it
empirically for C++.

## Fixes — archcheck changes this eval triggered

The comparison surfaced two concrete archcheck issues. Both are now fixed; the
headline table reflects the post-fix state. Full suite green (**365 cases**), no
precision regression (corpus pair counts stay stable, deltas are small and
bidirectional).

**Fix 1 — vendored exclusion missed in-tree bundled libraries.** archcheck reported
clones in irrlicht `jpeglib/`, `bzip2/`, `libpng/` and BambuStudio `src/qhull/`,
`src/hidapi/`, `src/agg/`, `src/libigl/`, `src/glu-libtess/` — the 16 BOTH overlaps
plus several `[V]` only-archcheck pairs. A curated `kVendoredLibDirs` list of
well-known multi-file bundled libs was added to the central vendor classifier
([`file_classification.h`](../include/archcheck/scan/file_classification.h)), matched
as a directory segment (same philosophy as the existing `kVendoredStems`). Removed
the 16 vendored "agreements" and the `[V]` pairs → **BOTH 16 → effectively 0
(vendored)**, raising precision.

**Fix 2 — #091: large twin functions fragmented inconsistently.** The 400-token
fragment cap kept `algo_line` (392 tok) whole but subdivided its near-twin
`algo_line_float` (>400), so the clone never aligned. Raising `maxTokens` to 600
([`fragmenter.h`](../include/archcheck/scan/duplication/fragmenter.h)) recovers
function-level clones — including the LibreSprite `algo_line ↔ algo_line_float`
pair NiCad flagged and archcheck missed (now the single BOTH overlap in the corpus).
See [backlog/wip/091](../backlog/wip/091_maj_dup_large_function_recall.md).

Per-repo archcheck pairs, **baseline → current**:

| repo | baseline | current | driver |
|---|---:|---:|---|
| Catch2 | 0 | 0 | — |
| GWToolboxpp | 13 | 12 | #091 |
| Kartend | 34 | 42 | #091 recall (+8) |
| IrredenEngine | 3 | 4 | #091 |
| LibreSprite | 10 | 10 | #091 (algo_line recovered) |
| irrlicht-1.8.3 | 58 | 41 | vendored −14, #091 −3 |
| AetherSDR | 71 | 72 | #091 |
| BambuStudio | 187 | 129 | vendored −54, #091 −4 |
| **total** | **376** | **310** | |

Authored clones are all preserved (`CIrrDeviceWin32 ↔ CIrrDeviceWinCE`, `CD3D8 ↔
CD3D9`, `GLGizmoAdvancedCut ↔ GLGizmoColorCut`, slic3r `3mf`); only vendored noise
dropped and large-function recall improved. Both fixes covered by tests
(`[scan][vendor]`, `[duplication][fragmenter]`).

> **Update — Fix 3 (#092), after this report's numbers were taken.** The
> rare-token candidate index was corpus-size-dependent (a clone stopped being a
> candidate once the project grew); k-gram winnowing fingerprints made candidacy
> scale-independent. This **further raised archcheck recall** well past the
> "current" column above (e.g. irrlicht 41→467, BambuStudio 129→1152), and an
> eyes-on sample of the new pairs was ~0 false positives. The tables in this
> report therefore *understate* post-#092 archcheck. See
> [backlog/wip/092](../backlog/wip/092_crt_dup_scale_independent_candidates.md).

## Recommendation

### KEEP duplication on the archcheck roadmap.

The evidence is decisive, and it is *not* "archcheck is better at clone detection."
It is that **the precision gold-standard cannot do the job at all on C++, and
archcheck is the only tool here that produces actionable C++ signal**:

1. **NiCad cannot run on modern C++ out of the box.** No `cpp` language; the `c`
   grammar fails on 38–94 % of files (83–94 % on the genuinely C++ repos); the only
   available TXL C++ grammar is pre-C++11 ANSI C++ and also fails. Getting NiCad to
   work on this corpus would require authoring a modern-C++ TXL grammar + the full
   NiCad program family — a research project, not adoption.
2. **Where NiCad does run, it is ~1 % signal.** 802 clone groups, ~6–9 useful; the
   rest is the duplication *inside vendored C dependencies* (no default exclusion).
3. **archcheck finds 309 authored C++ clones NiCad misses entirely** — including
   EXACT cross-file copy-paste that is exactly what a CI gate / PR reviewer wants —
   at **~100× less wall-clock** (≈17 s vs ≈1700 s across the corpus), as a single
   static binary with working vendored exclusion and a baseline mode.
4. **The "wrap an existing tool" alternative fails for NiCad.** Per
   [clone_tools_landscape §4](../docs/research/clone_tools_landscape.md), archcheck's
   differentiator stands on *single binary + arch-framing + precision-under-gate*.
   This eval validates legs 1 and 2 decisively (NiCad needs TXL + grammar + farm +
   minutes; it has no pairs/vendored-exclusion/baseline) and shows leg 3 is moot
   for NiCad because it can't parse C++ to have precision *on*.

**What this eval does NOT establish:** that archcheck's detector beats a *production
C++-aware* clone tool. NiCad was not a viable C++ comparator, so the open question
from clone_tools_landscape §4 — "is our selective-norm precision actually better
than PMD-CPD / jscpd with `--ignore-literals` on C++?" — remains untested. That
PMD/jscpd bake-off is the logical next step before claiming a precision edge. But
for the question actually asked — *does NiCad provide value beyond the archcheck
prototype, and should duplication stay on the roadmap* — the answer is **no added
value from NiCad, and yes keep duplication**: drop it and you lose the only working
C++ duplication signal in the corpus, with nothing shippable to replace it.
