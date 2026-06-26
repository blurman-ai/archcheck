# AI-assisted commits as DRIFT-rule test cases

Run date: 2026-05-29
Binary: `build/debug/src/archcheck` (DRIFT.1 + DRIFT.2)
Method: `--save-graph-baseline` on the before-SHA → `--drift-baseline` on the after-SHA.

Related to the task [backlog/future/033_maj_ai_drift_dataset.md](../../backlog/future/033_maj_ai_drift_dataset.md).
DRIFT.1/DRIFT.2 implementation — [backlog/completed/009_maj_ai_drift_regression_rules.md](../../backlog/completed/009_maj_ai_drift_regression_rules.md).

## Summary

| Repo | PR | Before SHA | After SHA | DRIFT.1 | DRIFT.2 | Verdict |
|------|----|------------|-----------|---------|---------|---------|
| LibreSprite/LibreSprite | #581 macOS / menu search / toolbar badges | `60eed0f` | `276fdbd` | **1** | 0 | hit — shortcut edge across the `ui → pref` boundary |
| proximafusion/vmecpp | #360 asymmetric infra | `df63271` | `5eabd51` | 0 | 0 | clean — a large refactor without drift |
| proximafusion/vmecpp | #340 consolidate constants | `b44fb7f` | `a7797dc` | 0 | 0 | clean — refactoring done correctly |
| bambulab/BambuStudio | #10794 color cutting / dovetail / sculpting | `2263815` | `a206a95` | **2 real + 1 fp** | 0 | hit — two UI→Widgets shortcuts + a bug found in the scanner |

The DRIFT rules fired only where it was warranted. On the vmecpp PRs — clean. On BambuStudio, two real violations were found **and one bug in our scanner** (UTF-8 BOM).

## LibreSprite PR #581 — the found violation

```
app/ui/toolbar.cpp: [DRIFT.1] shortcut edge: app/ui/toolbar.cpp -> app/pref/preferences.h
```

Source: commit `0aa57ad` "Add keyboard shortcut badges to toolbar icons"
(Co-Authored-By: Claude Opus 4.5).

Diff:
```cpp
// src/app/ui/toolbar.cpp
+#include "app/pref/preferences.h"
```

What happened: the agent added visual hotkey badges to the toolbar icons
and introduced a new preference `show_tool_shortcuts`. To read it, it added a direct
include of `app/pref/preferences.h` from `toolbar.cpp` — a module that previously
did not talk to the preferences layer directly.

Why this is exactly the class of finding DRIFT.1 was designed for:
- locally convenient (one include, and the preference is available)
- globally it blurs the architecture (a UI widget begins to depend on the
  preferences layer without an explicit intermediary)
- not caught by ordinary linters — the code is syntactically and semantically valid

This is the textbook shortcut edge in the formulation of `docs/research/constraint_decay.md`:
a plausible AI step across a module boundary that creates no obvious junk but
accumulates structural drift.

### Verification (skeptic pre-empt, 2026-05-29)

LibreSprite is a fork of Aseprite. Upstream `aseprite/aseprite`
`src/app/ui/toolbar.cpp` already includes `app/pref/preferences.h`, so the
natural skeptic question is: "this isn't AI drift but just re-convergence to
upstream, where the edge always was." The argument is weak in substance (DRIFT.1 counts the
graph delta of **this** repo, not upstream), but it could take the punch out of a public
demo. Closed with three `git` commands against a sandbox clone of LibreSprite:

```
$ git show 60eed0f:src/app/ui/toolbar.cpp | grep -n 'app/pref/preferences.h'
ABSENT at before-SHA

$ git show 276fdbd:src/app/ui/toolbar.cpp | grep -n 'app/pref/preferences.h'
15:#include "app/pref/preferences.h"

$ git log --oneline 60eed0f..pr-581 -- src/app/ui/toolbar.cpp
0aa57ad Add keyboard shortcut badges to toolbar icons
```

`git log -p -S 'app/pref/preferences.h' 60eed0f..pr-581 -- src/app/ui/toolbar.cpp`
confirmed the single `+#include "app/pref/preferences.h"` in this commit,
with no reversal within the PR.

**Verdict: CONFIRMED.** The LibreSprite fork at 60eed0f indeed did not carry the edge;
the AI commit 0aa57ad (re)introduced it. The framing for the demo: "the edge is present in
upstream Aseprite; LibreSprite lost it over its own history; the AI commit brought it back
in exactly the situation DRIFT.1 is supposed to catch."

## Update 2026-05-29 (second session)

The corpus was expanded to **33 PRs across 10 repositories**. The full table —
[ai_drift_runlog.md](ai_drift_runlog.md). Additional finds:

| Repo | PR | DRIFT.1 | Archetype |
|------|----|---------|---------|
| jakildev/IrredenEngine | #727 render LOD Phase 1 | 2 | system→component_lod, system→lod_utils |
| EtherAura/Kartend | #27 promote uiconstants | 5 | data/settings → ui/uiconstants ×4 + data→utils/view |
| community-shaders/skyrim | #2326 fix singleton ptr | 1 | State.cpp → Features/InteriorSun |
| community-shaders/skyrim | #2207 refactor common Util | 1 | VRStereoOpt → Utils/UI |

During the run a **methodological bug** was found (#048): a partial git checkout
without `git clean -fdx` leaves files from other revisions behind → mass
false-positives in DRIFT. All the numbers above were recomputed via `scripts/drift_run.sh`
with a full clean checkout.

Total: 12 confirmed DRIFT.1 hits across 7 of the 33 PRs (21%).
Archetypes: UI→widgets, generic→features, system→component, ui-config→core-data.

## BambuStudio PR #10794 — two UI→Widgets shortcuts + one false-positive (bug)

```
slic3r/GUI/FilamentMapDialog.hpp: [DRIFT.1] shortcut edge: ... -> slic3r/GUI/Widgets/CheckBox.hpp
slic3r/GUI/FilamentMapPanel.hpp:  [DRIFT.1] shortcut edge: ... -> slic3r/GUI/Widgets/SwitchButton.hpp
slic3r/GUI/MsgDialog.cpp:         [DRIFT.1] shortcut edge: ... -> slic3r/GUI/MsgDialog.hpp   ← FALSE POSITIVE
```

Source of the changes: commit `126aa51` "Integrate AG changes into root source tree" (author: adamgasoft).
This PR is not explicitly marked Co-Authored-By: Claude, but in task #033 it is included in Tier 2 — the PR
contains an AI workflow instruction to always add Claude attribution. We take it as a case of
a large architectural change with possible AI involvement.

**Real findings (2):**
- `FilamentMapDialog.hpp -> Widgets/CheckBox.hpp` — a dialog-level component pulls an include
  of a low-level widget directly. A classic UI-layer shortcut.
- `FilamentMapPanel.hpp -> Widgets/SwitchButton.hpp` — the same thing for the panel.

**False-positive (1) — a bug in the scanner, task filed [#047](../../backlog/completed/047_crt_scan_utf8_bom.md):**
- `MsgDialog.cpp -> MsgDialog.hpp` — `MsgDialog.cpp` ALWAYS began with `#include "MsgDialog.hpp"`.
  In the before-revision the first line has a UTF-8 BOM (`EF BB BF`), and our `include_scanner`
  doesn't strip the BOM → the first line doesn't match the regex → the edge is absent from the graph baseline →
  in the after-revision (without the BOM) the edge looks new → DRIFT.1.

This false-positive is a genuinely important finding of the run: a blocker for the reliability of DRIFT.1
on any wxWidgets / Windows project. Recorded in #047.

## vmecpp PR #360 and #340 — clean

Both merged PRs with explicit Claude attribution. Thousands of lines of changes (PR #360 — infrastructure
for asymmetric VMEC, PR #340 — consolidation of constants in `vmec_constants.h`). The DRIFT rules
did not fire even once — a useful signal that DRIFT.1/DRIFT.2 do not turn into a
noise generator on normal refactoring.

Lakos.GodHeader flags 3 headers — that is a pre-existing state, not drift.

## Reproduction

```bash
cd sandbox/drift_repos/LibreSprite
git checkout 60eed0fd3e39104d50d67c366dd0f312ac45329c
archcheck --save-graph-baseline /tmp/libresprite_before.graph.json src

git fetch origin pull/581/head:pr-581
git checkout pr-581
archcheck --drift-baseline /tmp/libresprite_before.graph.json src
# → 1 DRIFT.1 violation
```

## Next steps

1. Turn the LibreSprite case into an integration fixture `fixtures/drift_real_world/libresprite_pr581/`
   — a minimal reproducible graph slice (without a full repo clone).
   Task filed [#048](../../backlog/completed/048_maj_fixture_libresprite_pr581.md);
   the skeptic check (including "the whole PR, not just the merge-range") is closed
   by the "Verification" section above — CONFIRMED.
2. Add another 2-3 cases from Tier 2 (nodos-dev/sys-device, ...) to
   expand the corpus.
4. Once the corpus has >= 5 PRs with confirmed hits — update the README with a demonstration
   on real data (see `backlog/future/033` §"Target form of the demonstration").
