# [SCAN][DRIFT] Scope/parser fix pack from #161/#162 manual audits

**Created:** 2026-07-02
**Status:** new — **items A.1-A.5, B.1-B.3 FIXED in-tree 2026-07-02** (same session, user
asked for clean results; see "Fixed 2026-07-02" below). Remaining open: B.4, C.1-C.3.
**Module:** SCAN / BOOL_FIELD_DRIFT / LOCAL_COMPLEXITY / classification
**Priority:** major
**Complexity:** M (several independent S-size fixes; each needs fixtures)
**Blocks:** —
**Blocked by:** —
**Related:** #161 (bool audit), #162 (LCX audit), #160 (evidence base), #158 (test-name
gaps precedent), #154 (constants belong in file_classification config)

Every item below has a pinned real-world reproducer from the 2026-07-02 audits
(`docs/research/bool_field_drift_manual_audit.md`,
`docs/research/local_complexity_manual_audit.md`). One fix = one commit with fixtures;
do not batch unrelated items into one diff.

## A. Bool-field drift parser (`src/scan/bool_field_drift.cpp`)

- [ ] **A.1 `kBoolRe` misses `const bool`.** Reproducer: btop `21ca9093`
      `include/fmt/chrono.h` — `const bool is_classic_;` → `bool is_classic_;` reads as
      a new field. Add optional `const`/`constexpr`... carefully: `static constexpr bool`
      members are compile-time constants, not state — decide whether to count them
      (recommend: match `const bool` but skip `static`).
- [ ] **A.2 `kBoolRe` misses brace-init `bool x{false};`.** Reproducer: mvfst `16abd44c`
      `OutstandingPacket.h` — `bool x{false};` → `bool x : 1;` produced 3 phantom fields.
      Add `(?:\s*\{[^;{}]*\})?` alternative to the `= init` group.
- [ ] **A.3 Unexplained before-image extraction failure.** Reproducer: NanaBox
      `6b3d1e04^`/`6b3d1e04` `NanaBox.Configuration.Specification.h` —
      `RemoveHyperVDevices` flagged as new though the before-line is identical and
      kBoolRe matches it (verified during the audit; the "comment content" hypothesis is
      REFUTED). Suspect struct-body span extraction. Turn the file pair into a unit test
      and debug; do not guess the mechanism.
- [ ] **A.4 Message arithmetic.** `bool_field_drift.cpp:205-207`: reported N = net delta,
      but the list = all added names → "accreted 1 bool field(s): A, B" (cpp-atlas
      `02f6dea8`). Report gross adds (and optionally "(net +N)").
- [ ] **A.5 (minor) Nested-class attribution.** `class Type::Function : ...` reported as
      struct 'Type' (nico `65b1a2e7`). Capture the last `::` segment.

## B. Classification scope (`include/archcheck/scan/file_classification.h`)

- [ ] **B.1 `fmt` missing from `kVendoredLibDirs`.** Reproducer: btop bundled
      `include/fmt/` bump produced 2 findings. Per #154, this is the central config —
      also consider `cal3d` (see B.3). Beware false vendor matches on projects genuinely
      named fmt-something; dir-segment exact match only.
- [ ] **B.2 Bare test stems.** `isTestBasename` misses stems `test` / `tests`
      (`tests.c` is a common C idiom). Reproducer: secp256k1-zkp `f5e815f4`
      `src/tests.c` LCX finding. Negative cases stay mandatory (#158 A.1 list:
      `latest.cpp`, `contest.cpp`, ...) — an EXACT-stem match on `test`/`tests` does not
      collide with them.
- [ ] **B.3 `testlib` dir not in `kTestDirNames`.** Reproducer: zera-classes `c2d05170`
      `modulemanager/testlib/`. Check corpus for legit non-test `testlib` dirs first.
- [ ] **B.4 Vendored source snapshot dirs like `cal3d-src/`.** Reproducer: Cal3DWrapper
      `236f27be` — in-tree upstream library copy not recognized; also produced duplicate
      findings from 2 identical file copies. Consider a `-src` suffix heuristic ONLY with
      corpus measurement (risky), or leave to #153 vendor-exclusion work; at minimum
      document.

## C. Local-complexity drift (`src/scan/local_complexity_drift.cpp`)

- [ ] **C.1 Grew-path ignores the moved-function pool.** Reproducer: Windows-CalcEngine
      `f839c9ef` `CBSDFLayer::jscPrime` 1→7 — a deleted override's verbatim body absorbed
      by an existing stub fires "grew" while the movedPool only guards "new" reports.
      Suppress (or down-rank) a grew-report whose after-body matches a disappeared
      function.
- [ ] **C.2 Same-body cross-path dedup.** Cal3D reproducer: identical function reported
      at 2 paths in one commit. Fold identical-body findings.
- [ ] **C.3 (document only) Extraction-as-new.** edk2-nvidia `95662bbc`
      `TH500UpdatePcieNode`, Creation-Engine `f69ef254` `connectRooms`: partial
      extraction can't match the whole-function move pool; flags complexity-reducing
      refactors. No cheap fix known — record as a known limitation in the rule docs
      unless a partial-body match proves cheap and safe.

## Fixed 2026-07-02 (in-tree, uncommitted)

Shipped the same night on the user's "clean results, no caveats" instruction:

- **A.1 + A.2** — `kBoolRe` now accepts `(?:(?:mutable|const)\s+)?` and brace-init
  `{...}`; `static`/`constexpr` members stay excluded (new unit test).
- **A.3 ROOT CAUSE FOUND** — not the regex and not comment *content*: the paren guard in
  `appendStructBools` (meant to skip method declarations) rejected any line whose
  TRAILING COMMENT contains parentheses (`// TODO(Phase4): ...`). The guard now checks
  only the pre-`//` code part. This both kills the NanaBox phantom class and RECOVERS
  genuinely-new fields that carry parenthesized comments (CopilotChess `GameState`
  +4 castling bools was a missed TP before the fix — now reported).
- **A.4** — the message now counts the gained names it lists (gate stays net).
- **A.5** — `kStructRe` captures qualified names (`Type::Function`).
- **B.1** — `fmt` added to `kVendoredLibDirs`; **B.2** — bare `test`/`tests` stems in
  `isTestBasename` (negative cases kept); **B.3** — `testlib`/`testlibs` in
  `kTestDirNames` (corpus-checked: zera-classes, papi, GEM5 — all test support).

Verification: 622/622 tests (6 new), dogfood 0, lizard clean on touched files; all five
real-corpus reproducers silent (btop, mvfst, NanaBox, secp256k1 `tests.c`, zera
`testlib/`); CopilotChess control unchanged plus the recovered `GameState` TP; full
100-commit audit-sample re-run and Group-3 clone-gate re-run recorded in the audit docs.

Still open here: **B.4** (`cal3d-src`-style vendored snapshots), **C.1** (grew-path
movedPool), **C.2** (same-body dedup), **C.3** (extraction-as-new — likely documented
limitation).

## Acceptance

- [ ] Each shipped item has a unit test / fixture derived from its reproducer.
- [ ] Bool + LCX corpora spot-reruns show the reproducer findings gone and no new
      regressions on the #161/#162 sampled sets.
- [ ] Dogfood 0, full suite green, lizard clean.
- [ ] `docs/research/*_manual_audit.md` updated with "fixed in #164" notes.

## Do not do

- Do not add broad substring heuristics to test/vendor lists — #158's negative-case
  discipline applies (guard 1.5).
- Do not change the cognitive-complexity scorer itself — #162 verified its arithmetic;
  the issues are scope/move, not the metric.
