# Design: `boolean_state_accumulation` — drift metric (variant 2)

**Date:** 2026-06-07 · **Task:** #089 → candidate for #090/v0.3
**Status:** design, NOT implemented in `src/`. Prototype — `experiments/boolean_state/perstruct_drift.py`.

> This is not a static linter (that failed: names are useless, the counter is 78% noise). This is a **drift metric over time** — in the same family as cycle-growth (#086) and bidirectional-coupling drift (#087): it measures the CHANGE in a structure between revisions, not a single snapshot.

## What the metric asserts

"Structure S accumulated bool fields across ≥K distinct commits over ≥D days, and its fields are interdependent → a growing implicit state machine / constraint decay."

Empirical basis: per-struct + git-blame yielded 0% gross FP and 57% confirmed real drift on verification (see [perstruct](boolean_state_perstruct_drift.md), [eyecheck](boolean_state_drift_eyecheck.md)).

## Algorithm (3 gates, derived from research)

```
for each header in diff/history:
  1. PARSE: find struct/class → bool fields ONLY as direct members
     (depth-0; exclude bool in method signatures and locals in bodies).        [FP gate]
  2. ATTRIBUTION: bind each field to the SPECIFIC structure (not the file);
     git blame the field → commit + date of appearance.                       [single-struct gate]
  3. HISTORY: group by structure. drift_commits = number of DISTINCT
     commits that added its bool fields.
     Require FULL history (not shallow); otherwise mark "lower-bound".         [shallow gate]
  4. INTERDEPENDENCE (opt., reinforces): on the CURRENT snapshot of the structure, check
     whether combinations are constrained — via a cheap regex proxy over group assignment
     (`a=true; b=false; c=false` together) in .cpp. If fields are orthogonal → it's
     bloat, NOT an illegal-state risk (lower severity).                       [config-vs-state gate]

flag if: nfields >= 5  AND  drift_commits >= 4  AND  interdependence != orthogonal
```

## ⚠️ #042 is NOT needed for the diff

Important (correction): the metric runs over **history** (diffs between commits), whereas #042 (libclang) is AST on a SINGLE snapshot. Running AST on every historical commit is infeasible (×1350 slower than regex + old commits don't build). Therefore:

- **Gates 1-3 (the diff itself)** = `git blame` + depth-0 regex. **No AST needed, #042 not involved.**
- **Gate 4 (interdependence)** is checked ONLY on the current snapshot of an already-flagged structure (not over history!) and is covered by a **cheap regex proxy over group assignment** — the same one that, without AST, found ZuluSCSI audio / ncnn winograd (`experiments/boolean_state/transition_scan.py`).
- **#042 is merely an optional precision boost for gate 4** (semantic mutual-exclusion instead of the regex proxy), NOT a blocker. The metric is built entirely on the **fast backend**.

## Thresholds (from corpus data)

| Parameter | Value | Rationale |
|---|---|---|
| min_bool_fields | 5 | below this — almost never a machine |
| min_drift_commits | 4 | "accumulation a bit at a time", not a single feature dump |
| shallow → | severity↓ / "lower-bound" | 54% of shallow candidates are unreliable |
| orthogonal fields → | severity↓ (bloat, not illegal-state) | Channel/MethodState: ~all 2^N are legal |

## Complexity and where it lives

- Gates 1-3 = diff parsing + `git blame` (as already in the prototype). It rides on git history — i.e. on **archcheck's diff mode** (`--diff`, #015/#018), not on a single-shot scan. **Fast backend, no #042.**
- Gate 4 = regex proxy over group assignment on the current snapshot (present in `transient_scan.py`). Also fast backend. #042 is an optional boost, not a dependency.
- Implementation — NOT in `src/rules/` as an ordinary rule, but as a history metric alongside the drift family (#086/#087). **Not blocked by #042** — buildable on the fast backend right now.

## Known limitations (empirical, not theoretical)

1. **Shallow clones** → 54% of the drift is invisible. On the user's real repos (full history) this is not a problem; on the corpus — yes.
2. **config-bag vs state** are inseparable by name without gate 4 (43% residual confusion). Field semantics are needed.
3. **Orthogonal bags** — high bool count without a rise in illegal-state risk; gate 4 demotes them.

## Conclusion on feasibility

The metric is **technically feasible on the fast backend right now** (git blame + regex; #042 is NOT needed for the diff, only an optional boost for gate 4), and on the verified sample it is accurate (0% gross FP). By archcheck's positioning it is a borderline candidate: closer to the drift family (#086/#087) than to a linter. The main blocker is **not technical but demand-driven**: no user has asked for it (YAGNI). The decision to implement — defer until explicitly requested, but NOT behind #042.
