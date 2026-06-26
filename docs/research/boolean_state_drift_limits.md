# Boolean-State Drift — limitations of the method and clarifications (variant 3)

**Date:** 2026-06-07 · **Task:** #089
Refining the research after the critique "the results are flimsy" and "why is not every combination possible". Three clarifications, each one removing one of my overstatements.

## 1. All corpus clones are shallow → history is incomplete

**73/73 agent clones are shallow** (`.git/shallow` is present in all of them). Consequence: commits before the clone horizon are absent, and git shows every pre-horizon line as "added" in the first visible commit.

What this does to drift detection:
- **Young files** (created after the horizon) — history is COMPLETE, the chronicle is reliable (example: EditorShell, 5→24 over 6 weeks — visible in full).
- **Old files** (predating the horizon) — history is TRUNCATED: the accumulation of bools over years "collapses" into the horizon commit, `git blame` attributes them to it → the structure looks like "1 commit", and the drift **disappears from view** (example: ATS `HttpTransact::State` — 39 bools dumped into the horizon 2024-06-03, even though upstream they accumulated over years).

**Quantitatively (per-struct candidates, 65 of them):**

| | Count | Share |
|---|---|---|
| YOUNG — full history, drift reliable | 30 | 46% |
| OLD — truncated, drift underestimated/invisible | 35 | 54% |

→ **history-detection on a shallow corpus systematically UNDERESTIMATES drift** (conservative bias). Only ~46% of candidates are reliable; for the rest we see only the tail. The real prevalence is most likely **higher** than the measured ~21%, but we cannot say for sure without full clones.

## 2. "Not every combination is possible" — NOT universal (answer to the question)

I checked the interdependence of bool fields by usage across 6 confirmed drift structures (mutual exclusion / group assignment / capability gates / invariants). The thesis "2^N is representable, only a few are legal" holds **only under interdependence**, and it must be proven, not postulated:

| Structure | Verdict | Reachable ≈ out of 2^N | Why |
|---|---|---|---|
| Engine (chess) | 🔗 interdependent | **~6-10 out of 256** | pondering/search state machine, group set/clear |
| EditorShell (donner) | 🧩 mixed (strong) | ~2^16-18 out of 2^24 | group set/clear of layerPanel, invariants "B only when A" |
| ToolboxUIElement (GWToolbox) | 🧩 mixed | ~2^15-17 out of 2^23 | capability flags `is_*`/`has_*`/`can_*` gate clusters |
| Graph (hhds) | 🧩 mixed | ~17 out of 32 | `deleted_` cuts off half; cache pair is free |
| MethodState (MOLA) | ⚙️ independent | **~448 out of 512** | a bag of orthogonal state flags |
| Channel (FluidNC) | ⚙️ independent | **~512 out of 512** | orthogonal "dirty"/mode flags, no links |

**Conclusion:** "impossible states" are a real risk where there is a state machine or capability gates (Engine, EditorShell, ToolboxUIElement), and **there is none** where the flags are orthogonal (Channel, MethodState — there it is simply bloat, not illegal-state). My earlier "2²³ ≈ 8 million garbage" for ToolboxUIElement is overstated: capability gates cut the space, but not down to "dozens" either — on the order of 2^15-17. For Channel the thesis is directly refuted.

In total: **2/6** of even the real drift structures are orthogonal (the thesis does not apply), **1/6** is a strict state machine (applies rigidly), **3/6** are partial.

## 3. Combined implication for the detector

For the metric to not be flimsy, THREE independent conditions are needed, and we checked each one empirically:

1. **Full history** (not shallow) — otherwise 54% of the drift is invisible.
2. **Per-struct attribution + fields only** (depth-0) — gives 0% gross FP (proven in [eyecheck](boolean_state_drift_eyecheck.md): file-level 45% FP → per-struct 0%).
3. **Interdependence check** by usage — otherwise "drift" includes orthogonal bags (Channel/MethodState), where there is no increase in illegal-state risk.

Conditions 1 and 3 are nontrivial: (1) requires full clones, (3) requires usage/AST analysis (≈ #042). Without them, any history-based counter remains approximate.
