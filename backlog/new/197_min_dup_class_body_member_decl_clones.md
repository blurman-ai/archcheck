# [DUP] 197: clones of member-function runs in a class body are not detected

**Created:** 2026-07-21
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION
**Priority:** minor
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #190 (brace-descent), #191 (statement-run), #195 (boundary-run), #196 (trailing-qualifier bodies)

## Goal

Detect copy-paste that is a run of **member-function declarations/definitions inside a
class body** — i.e. several small sibling methods copied between two classes — which the
current fragmenter misses because it operates on *function bodies*, not on runs of
members inside a `class { … }` scope.

## Evidence (hand-read, 2026-07-21)

Surfaced classifying the 6 Duplo-uncovered blocks on duckdb `src/execution` (#191
acceptance). One residual real miss:

`operator/join/physical_nested_loop_join.cpp:223-236` ↔
`operator/join/physical_piecewise_merge_join.cpp:129-142`

A local-state class copied between the two join operators: `ResetState(...)` +
`SupportsReuse()` + the head of `Reset(...)`, byte-identical except the `Cast<…>` type
(`NestedLoopJoinGlobalState` vs `MergeJoinGlobalState`). It is a real, refactorable clone.

Why it is missed: these are member declarations/definitions sitting directly in a
`class { … }` body. #190 descends into function *bodies*; #191 statement-runs fire inside a
body; #195 boundary-runs anchor to a body's edges. None emits a fragment for a run of
*members* at class scope.

## Not in scope / cautions

- This is a **minor** recall edge, not a correctness bug. Do not build it if it inflates
  candidate pairs or FPs — class bodies are full of near-identical tiny getters/overrides
  (`SupportsReuse() { return true; }` appears everywhere), exactly the idiom-flood risk.
- Measure precision on the corpus (baseline diff, `lost=0`) before keeping, same gate as
  #191. A cheap first cut: only emit a class-scope run when it spans ≥ `min_tokens` and
  contains at least one non-trivial method body, not just declarations.

## Acceptance criteria

- [ ] The witness above is reported.
- [ ] duckdb `src/execution`: `lost = 0`; Duplo coverage does not regress.
- [ ] Hand-triage new pairs; report TP/FP with sample size (watch the getter/override idiom flood).
- [ ] Fixtures `fixtures/duplication/class_body_member_clone/{pass,fail_*}/`.
- [ ] `docs/duplication_architecture.md` updated.

---

## Addendum (2026-08-25) — the DATA-member variant, and why its FP profile differs

Independently confirmed on three repos while cross-validating against Duplo (#194). Same root
cause as above — no fragment is emitted for a run of members at class scope — but the members
are **data fields**, not methods.

`u236/homed-service-zigbee` `ezsp.h:171-179` ↔ `ezsp.h:186-193`:

```cpp
struct ezspSendUnicastStruct        struct ezspSendMulticastStruct
{                                   {
    quint8  type;                       quint16 profileId;
    quint16 networkAddress;             quint16 clusterId;
    quint16 profileId;                  quint8  srcEndpointId;
    quint16 clusterId;                  quint8  dstEndpointId;
    quint8  srcEndpointId;              quint16 options;
    quint8  dstEndpointId;              quint16 groupId;
    quint16 options;                    quint8  sequence;
    quint16 groupId;                    quint8  hops;
    quint8  sequence;               ...
```

Also seen as a run of method declarations, which IS the shape this task already covers:
`fmidev/smartmet-plugin-wms` `ArrowLayer.h:32-38` ↔ `RasterLayer.h:39-45` — an identical
`init` / `generate` / `getFeatureInfo` override run.

**The FP profile is not the same as for copied methods, and the fix must not assume it is.** A
run of copied sibling *methods* is refactorable — extract a base, share the implementation. A run
of identical *fields* in two structs is very often a wire-format mirror: `ezspSendUnicastStruct`
and `ezspSendMulticastStruct` follow the EZSP protocol frame layout, and collapsing them into a
common base would be wrong. By Kapser & Godfrey that is an API-protocol (incidental) clone —
which the project's own doctrine says to tag or down-rank, never suppress
([[project_accidental_clones_no_shape_signal]], `docs/duplication_architecture.md` §5.y).

So detecting the data-member run without a way to separate "copied struct" from "protocol
mirror" would add a noisy class to the output. Whether it should fire at all is a design
question this addendum does not settle — but it must be answered before implementing, otherwise
the layer ships an FP class the project has spent five tasks removing elsewhere.

Method runs (the original scope) carry no such caveat and can proceed on their own.

## Evidence

`experiments/clone_compare/show_clone_deltas.py duplo_only` prints each finding with both code
fragments; report at `archcheck-journal/reports/duplo_vs_archcheck.md`.
