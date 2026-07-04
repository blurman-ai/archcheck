# [DOCS] Launch FAQ / design-rationale answers to four recurring critiques

**Created:** 2026-07-04
**Status:** completed 2026-07-04
**Module:** DOCS
**Priority:** minor
**Complexity:** S
**Related:** #042 (semantic backend — answers the preprocessor limits), #052 (cross-TU dup), #083 (dup precision gate-grade), #163 (community/launch files)

## Goal

Write an authored "Design rationale / FAQ" section (README or a short
`docs/faq.md`) that pre-answers four critiques that will predictably arrive on
launch. Each has a correct, code-backed answer today; better to state it in our
own words than to argue it reactively in issues.

Three of the four are **framing corrections** (the tool already does the right
thing, the critique assumes it doesn't); one is an **honest gap** (modules). Do
not oversell — concede where the concession is real.

## The four critiques and the verified answers

### 1. "The fast preprocessor scan chokes on complex macros and `#ifdef`"

Half-outdated — the scanner already sees both cases; the question is what it does
with them:

- **Conditional compilation** — [src/scan/include_scanner.cpp:157](../../src/scan/include_scanner.cpp)
  tracks `#if/#ifdef/#ifndef … #endif` and does **not** pick a branch: it collects
  `#include`s from all branches and tags the edge `conditional`. The graph is a
  deliberate conservative **superset** of possible edges — the correct side of the
  error for cycle / god-header rules (a cycle that exists under *any* config is
  worth showing).
- **Computed includes** (`#include MY_HEADER_MACRO`) —
  [src/scan/include_scanner.cpp:258](../../src/scan/include_scanner.cpp) recognizes
  them and emits `DiagnosticKind::MacroInclude` — it does **not** silently
  misparse. The genuine blind spot: the macro is not expanded, so the edge target
  is unknown — but it is *flagged*, not dropped. Semantics belong to the libclang
  backend (v0.2, #042), a recorded ADR decision, not debt.

Answer to write: the parser is less naive than "wrote our own and it trips." The
one real limitation (macro-name expansion) is intentional, visible, and deferred
to the semantic backend.

### 2. "No C++20 modules (`import`) support — will age out"

The critique is correct; the critic's own caveat ("not a problem for 95% of
projects yet") is the answer. Module-graph analysis is roadmap, not a defect.
YAGNI: build it when a modules-based project asks. Answer to write: honest
"not supported, deliberate, deferred" — no spin.

### 3. "Not a linter? — it does clone detection, complexity, TODO/FIXME, test co-evolution"

The features **do** exist (`satd_scan`, `local_complexity_drift`,
`scan/duplication/`, `test_co_evolution`). The rebuttal is the **gate split**:
none of them sit on the hard gate. The gate is narrow and deterministic
(SF.9 cycles, `CASE_MISMATCH_INCLUDE`, `DRIFT.1/2/4.CYCLE`, new/grown cycles and
new god-headers in `--diff`); everything fuzzy — clones, SATD, complexity, bool
accretion, even SF.7/8 — is **advisory**. And the meaning differs from a linter:
a linter judges a file; these judge the **delta of the dependency graph over
time**. Word-overlap ("complexity", "copy-paste"), not purpose-overlap. Answer to
write: reproduce the gate-vs-advisory table from the README and the
"file vs graph-delta" distinction.

### 4. "76–91% clone precision / bool-accretion FPs — 2% FP on a hard gate demotivates"

The sharpest point, and it targets the exact failure mode the design avoids. The
2%-on-a-hard-gate argument is *correct* — which is why every fuzzy heuristic is
kept **off** the gate. The gate carries only deterministic graph facts (a cycle
is present or not; a case mismatch is a real Linux build break) whose FP rate is
~0 by construction, not 76%. Advisory-threshold tuning is real work (concede it),
but the cost of an error there is an ignored report line, not a red CI. Answer to
write: the critic's mental model ("fuzzy heuristics on a hard gate") is the error
the architecture is built to prevent.

## Acceptance criteria

- A `docs/faq.md` (or README "Design rationale" section) with the four Q&A entries,
  each linking the code/CHANGELOG that backs it.
- Prose passes the AI-tell self-check (see memory `feedback_prose_ai_tells`): no
  "X not Y" antithesis pile-up, no self-praise, minimal em-dashes.
- Concessions on #2 (modules) and #4 (advisory tuning) are explicit, not spun.
- Cross-linked from the launch checklist (#163 lineage).

## Notes

- Source: a launch-critique analysis produced by Qwen, discussed 2026-07-04; the one factual error in
  the critique was attributing the fuzzy heuristics to the hard gate — they are
  advisory. Keep that correction central.
- Keep it short. This is an FAQ, not a whitepaper — the spec already carries the
  long-form rationale.

---

## Completed (2026-07-04)

**Deliverable:** [docs/faq.md](../../docs/faq.md) — four Q&A entries, each backed
by code or a recorded decision:

1. Preprocessor limits → `opens_conditional`/`closes_conditional`
   (include_scanner.cpp:157-162, verified) collect includes from all `#if`
   branches; computed includes emit `DiagnosticKind::MacroInclude`
   (include_scanner.cpp:266, verified); expansion deferred by ADR-003.
2. C++20 modules → conceded as deliberate deferral, no spin.
3. "Not a linter?" → gate/advisory split quoted from `--help`; file-vs-graph-delta
   distinction.
4. Heuristic precision → fuzzy heuristics never gate; measured precisions
   describe advisories; threshold tuning conceded as ongoing work.

**Cross-links:** README ("Embedding in CI?" paragraph) and the #163 launch
checklist ("Done since 2026-07-04"). The launch post draft (companion repo,
`launch_drafts/launch_post_draft.md`) carries the same three pre-emptions
inline (macros/`#ifdef` sentence, linter-split sentence, gate-precision
paragraph); modules stay FAQ-only by choice — an announcement should not
enumerate gaps nobody asked about.

**Prose check:** AI-tell self-check applied (2 "X, not Y" contrasts total,
no self-praise, no outdated banners).
