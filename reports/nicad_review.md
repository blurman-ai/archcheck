# NiCad clone-quality review

_Manual review of the clone groups NiCad 6.2 produced on the corpus. Companion to
[nicad_installation.md](nicad_installation.md), [nicad_metrics.csv](nicad_metrics.csv),
[nicad_vs_archcheck.md](nicad_vs_archcheck.md). All clone groups come from
`nicad6 {functions,blocks} c <farm>` (default config: 30 % diff, blind rename),
see installation log for why `c` and the farm workaround. Links are GitHub
permalinks at the scanned commit._

## How NiCad reports, and what was reviewed

NiCad outputs **clone classes** (groups of ≥2 near-miss fragments), each with
`nclones` (group size), `nlines`, and `similarity`. Reviewed per repo: the **20
largest classes** (by `nlines × size`) and all **cross-module** classes (members
in different top-level dirs). Most repos have < 20 classes total, so the review is
effectively exhaustive except AetherSDR (537) and BambuStudio (194), where the
largest + a representative sample were read.

### Classification scheme (applied to each group)

| Class | Meaning (for an arch-gate consumer like archcheck) |
|---|---|
| **useful duplication** | Authored copy-paste a developer could plausibly refactor (real signal) |
| **generated / template noise** | Machine-generated or template-expanded code (glad.c, liquid-dsp `*.proto.c`, expat tables) |
| **vendored** | Third-party library code bundled in-tree (jpeglib, libpng, qhull, hidapi, …) |
| **expected pattern** | Authored but structurally-similar-by-design (Qt widget headers, constants headers, command-pattern headers) — low refactor value |

"Vendored" and "generated" and "expected pattern" are all **noise** for an
architecture gate (you don't refactor your dependencies, and you don't extract a
base class out of every Qt widget header). "useful duplication" is the only
**signal**. The extractability test from the project FP rules was applied:
a group is *useful* only if a single shared function/header could replace it
without contortion.

## Corpus-wide result

NiCad produced **802 clone classes** across the 8 repos (functions + blocks). By
category:

| category | classes | share |
|---|---:|---:|
| vendored (third-party in-tree) | ~755 | ~94 % |
| generated / template noise | (subset of vendored dirs: glad.c, liquid-dsp, expat) | — |
| expected pattern (authored, low value) | ~16 | ~2 % |
| **useful duplication (authored signal)** | **~6–8** | **< 1 %** |

**Signal-to-noise ≈ 1 : 100.** Of 802 clone groups, on the order of **6–8** are
authored duplication a developer might actually refactor — and they are clustered
almost entirely in one repo (LibreSprite). Everything else is vendored library
code, generated loaders/tables, or by-design-similar boilerplate.

The reason is structural, not tuning: NiCad's C grammar parses **only the C-language
subset** of each repo (37–94 % of files fail to parse — see metrics), and in this
corpus that subset *is* the vendored C dependencies. The projects' own code is C++
and never reaches the clone detector. So NiCad ends up reporting the duplication
*inside its dependencies*.

## Per-repo

### Catch2 — 0 classes
83 % parse failure, 0 functions/blocks of interest, **0 clone groups**. Nothing to
review. Pure modern C++/templates; NiCad is blind to it.

### GWToolboxpp — 7 classes · signal 0 %
All groups are vendored dependencies: `Dependencies/nativefiledialog/nfd_gtk.c`,
`nfd_zenity.c`, and giant constant-table headers `Dependencies/GWCA/.../AgentIDs.h`
↔ `ItemIDs.h`. The latter are generated game-ID tables.
→ all **vendored / generated**, 0 useful.

### Kartend — 1 class · signal 0 % (1 expected-pattern)
Only group:
[`src/ui/uiconstants/detailspaneconstants.h`](https://github.com/EtherAura/Kartend/blob/54bfbdfc29c0387b466ac43fcff28e960a35dbd0/src/ui/uiconstants/detailspaneconstants.h#L4-L53)
↔ `selection.h` ↔ `keyboard.h` … (23 constant-definition headers).
These parse because they are trivial `constexpr` namespaces. Classifying as
**expected pattern**: a namespace of UI constants is supposed to look like other
namespaces of UI constants; extracting a base offers nothing. Kartend's *real*
duplication is in `.cpp` (see archcheck) and NiCad missed all of it (94 % parse
failure).

### IrredenEngine — 5 classes · signal 0 %
All five are inside [`engine/render/src/opengl/glad.c`](https://github.com/jakildev/IrredenEngine/blob/3a825a50d84a2ce5dda505531a72429d812a795c/engine/render/src/opengl/glad.c#L974-L1027)
— the **generated** GLAD OpenGL loader. → all **generated noise**, 0 useful.

### LibreSprite — 12 classes · signal ~50 % (the corpus high-water mark)
The one repo where NiCad found genuine authored duplication, because LibreSprite
has little in-tree C and several of its own files happened to parse:

- **useful**: [`src/doc/algo.cpp:461-534`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L461-L534)
  ↔ `:536-612` ↔ `:614-690` — three near-identical line/ellipse algorithm bodies;
  also [`:20-101 ↔ :105-190`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/doc/algo.cpp#L20-L101)
  and [`src/app/util/autocrop.cpp:21-64 ↔ :66-109`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/util/autocrop.cpp#L21-L64).
  Real copy-paste, plausibly refactorable. **~6 groups.**
- **expected pattern**: command-class headers
  [`src/app/cmd/remove_palette.h ↔ remove_cel.h ↔ remove_frame_tag.h`](https://github.com/LibreSprite/LibreSprite/blob/276fdbdb27b537a074c3e170af6afc88c244a539/src/app/cmd/remove_palette.h#L12-L26),
  `trim_cel.h ↔ move_layer.h`, and tiny RAII headers `mutex.h/chrono.h`. **~4 groups.**
- **vendored**: `src/fixmath/fixmath.h` (bundled fixed-point lib). **2 groups.**

### irrlicht-1.8.3 — 46 classes (functions); blocks **crashed** · signal 0 %
**100 % vendored.** Every group is in `source/Irrlicht/jpeglib/`, `libpng/contrib/gregbook/`,
or `bzip2/` — e.g. the libpng demo programs
[`rpng2-win.c ↔ rpng2-x.c`](https://github.com/hyyh619/irrlicht-1.8.3/blob/4280a27f7611b7ce4cda877e91d7d831b0b8edfe/source/Irrlicht/libpng/contrib/gregbook/rpng2-win.c#L831-L1062)
(Win/X11 ports of the same demo — textbook copy-paste, but in a vendored lib).
Irrlicht's own C++ engine produced **0** groups. The **blocks** run **crashed**
(`Renaming failed, code 1` → exit 99) after extracting 3561 blocks — a NiCad
robustness failure on a mid-size input.

### AetherSDR — 537 classes · signal ~0 % (≤11 expected-pattern)
**526 of 537 vendored**, all in `third_party/`: `liquid-dsp` (the size-37 group
spans `firpfbch2/firpfbchr/firpfbch.proto.c` — template-expanded DSP, i.e.
**generated noise**), `r8brain`, `mosquitto`, `libspecbleach`. The 11 non-vendored
are Qt widget *header* declarations that parsed as C-ish, e.g.
[`ClientEqOutputFader.h ↔ ClientCompThresholdFader.h`](https://github.com/aethersdr/AetherSDR/blob/44ce91c3cf42e4a6aa7c5f552dc662c7e7c753aa/src/gui/ClientEqOutputFader.h#L7-L73)
and the applet-header cluster `ClientRxDspApplet.h ↔ MeterApplet.h ↔ ClientReverbApplet.h`.
A couple (`TgxlConnection.h ↔ PgxlConnection.h`) look like genuine copy-paste of a
connection class; most are **expected-pattern** widget boilerplate. Generously
≤ 2–3 useful, the rest expected pattern. AetherSDR's authored `.cpp` duplication
(found by archcheck) is entirely absent from NiCad's output.

### BambuStudio — 194 classes · signal 0 %
**All vendored / generated.** Largest group (size 43/44) is `deps/EXPAT/expat/xmlrole.c`
(generated parser tables). The rest: `src/qhull/{libqhull,libqhull_r}` (the same
library shipped twice — reentrant vs non-reentrant API, so it's ~maximally
self-similar), `src/glu-libtess`, `src/hidapi/{linux,mac,win}`, `src/libigl`,
`src/agg`, `src/mcut`. Zero authored slic3r duplication — that is C++ and didn't
parse. archcheck found 187 authored slic3r clones here; NiCad found none of them.

## Signal/noise summary

| repo | NiCad classes | useful (signal) | noise (vendored/generated/expected) | signal % |
|---|---:|---:|---:|---:|
| Catch2 | 0 | 0 | 0 | — |
| GWToolboxpp | 7 | 0 | 7 | 0 % |
| Kartend | 1 | 0 | 1 | 0 % |
| IrredenEngine | 5 | 0 | 5 | 0 % |
| LibreSprite | 12 | ~6 | ~6 | ~50 % |
| irrlicht-1.8.3 | 46 | 0 | 46 | 0 % |
| AetherSDR | 537 | ≤3 | ≥534 | ~0 % |
| BambuStudio | 194 | 0 | 194 | 0 % |
| **total** | **802** | **~6–9** | **~793** | **~1 %** |

**Caveat / fairness to NiCad:** as a *clone detector*, NiCad's precision is high —
the groups it reports are genuinely similar code (its 89–96 % Type-3 reputation
holds on the C it can parse). The ~1 % signal figure is **not** a NiCad precision
failure; it measures *usefulness for an architecture gate on a C++ corpus*, where
NiCad's blindness to C++ means the only code it sees is the vendored C it shouldn't
be flagging. NiCad has no default vendored-code exclusion (`exclude=""`), so 100 %
of that noise is reported. The single bright spot (LibreSprite `algo.cpp`) shows it
*can* surface real authored clones when the authored code is C-parseable — but that
is the exception in this corpus, not the rule.
