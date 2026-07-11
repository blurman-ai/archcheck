# 181 maj — `--diff` clone-scan performance on large C++ repos

Status: wip
Created: 2026-07-11
Start date: 2026-07-11
Related: #149 (byte-cap), #180 (surfaced the cost wall), DEBT.md 2026-07-11

## Goal

`archcheck --diff --diff-mode=memory` on a large C++ repo spends almost all its time in the
new-clone/duplication scan: **~84 s per commit on CUBRID** (2,354 files, tree < 40 MB, so the
#149 byte-cap does not trigger). A real user checking a PR on a big repo waits over a minute,
and at corpus scale (#180) per-commit replay of large repos becomes infeasible (~20 h for 57 k
commits). Make `--diff` new-clone detection fast without changing which clones it reports.

## How it works (this increment)

The new-clone detector ran **three whole-tree duplication scans** per commit — parent (for the
pre-existing-pair guard), parent again (for the deleted-fragment guard), and the new tree — each
an all-pairs `scanForDuplication` over every authored file. Two changes, both result-preserving:

1. **Focus set (`ScannerOptions::focusFiles`).** A NEW clone can only involve a file the diff
   touched, and `emitNewClones` already discards every pair not incident to an added line. So
   `detectNewClones` now passes the commit's changed files (added ∪ deleted) as a focus set, and
   `phase3ScoreCandidates` skips the expensive similarity math for any candidate pair with neither
   fragment in a focus file (`pairInFocus`). Empty `focusFiles` (whole-repo `--duplication`) keeps
   every pair — that path is byte-for-byte unchanged.
2. **One parent scan, not two.** `parentPairKeys` and `deletedFragmentKeys` scanned the *same*
   parent tree separately; they now share a single `ScanResult`.

Net: 3 whole-tree scans → 2, and each scores only touched-incident candidates.

## Key decisions

- **Result equivalence is the correctness bar, verified on a real repo.** Baseline vs optimized
  new-clone witnesses on a CUBRID commit are the **identical set** (20/20); only the emission
  order of equal-score clones differs (`std::sort` is not stable — pre-existing, benign, no test
  or baseline depends on it). Fixtures (`fixtures/diff_new_clone`) and all 644 tests stay green.
- **Opt-in, zero blast radius.** `--duplication` and every existing test are unaffected because
  `focusFiles` defaults empty.
- Kept `--diff-mode=memory` and the #149 byte-cap as-is.

## Result

CUBRID commit: **84 s → ~42 s (≈2×)**, identical clones. Coverage 91.9/96.0/57.3 (PASS),
lizard/cppcheck/clang-format clean, dogfood 0 violations.

## Follow-up (not in this increment)

- **Index-level focus (bigger win):** phase1 fragmenting + `buildIndex` (sharedRare generation)
  still run over all files each scan; ~half the remaining ~42 s. Restricting `sharedRare` pair
  generation to focus-incident fragments would cut it further. Higher risk (touches `clone_index`)
  — separate careful increment with the same equivalence gate.
- **`--no-duplication` / rule-select for `--diff`:** an escape hatch for corpus runs / callers
  who only want gate + complexity + graph + flag (skip clone entirely → instant). Cheap, but a
  new surface — decide against the "not a rule engine" line first.
- Consider `std::stable_sort` in `phase4Sort` if deterministic clone emission order across
  versions ever matters for baseline diffs.

## Changed files

- `include/archcheck/scan/duplication/duplication_scanner.h` — `focusFiles` field.
- `src/scan/duplication/duplication_scanner.cpp` — `pairInFocus` helper + phase3 gate.
- `src/scan/new_clone_drift.cpp` — thread focus into the three scans; merge the two parent scans.
- `tests/duplication_fp_guards_test.cpp` — focusFiles unit test (both branches).
