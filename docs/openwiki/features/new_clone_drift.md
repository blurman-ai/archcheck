---
title: "New-clone drift — copy-paste a diff introduces (`DRIFT.NEW_CLONE`)"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/new_clone_drift.cpp"
  - "include/archcheck/scan/new_clone_drift.h"
  - "src/cli/diff_command.cpp"
  - "include/archcheck/config/config.h"
  - "tests/unit/scan/new_clone_drift_test.cpp"
  - "fixtures/diff_new_clone/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# New-clone drift — copy-paste a diff introduces (`DRIFT.NEW_CLONE`)

## Purpose

In `--diff`, scans the whole **new** tree for duplication and reports only the pairs
where the copy-paste was **introduced by this commit** — one side overlaps an added
line, and the pair did not already exist in the parent tree (#123). Advisory only.

## Verified facts

- **Rule id**: `DRIFT.NEW_CLONE` (`kRuleId`, `src/scan/new_clone_drift.cpp:16`).
- **Entry points**: `detectNewClones(newSnapshot, parentSnapshot, added, deleted,
  maxScanBytes)` and a `deleted`-less overload (`include/archcheck/scan/new_clone_drift.h:45-52`);
  impl at `new_clone_drift.cpp:164-186`.
- **Advisory only — never gates the exit code**: struct comment
  `new_clone_drift.h:21` ("Clones a commit *introduces* (#123). Advisory only —
  never gates the exit code.").
- **Algorithm** (`emitNewClones`, `:132-160`):
  - Whole-tree `duplication::scanForDuplication` over the new tree's authored
    sources, with `opts.enableWholeFileGuard = false` (`:142-144`) — comment:
    "a commit that adds a file-copy is exactly the signal we want" (`:139-141`).
  - Keep a pair only if at least one fragment overlaps an added line
    (`touchesAdded`, `:72-81`).
  - **Parent-guard**: drop the pair if the same `(file, normalized-token-hash)`
    pair already existed in the parent snapshot (`parentPairKeys`, `:56-68`,
    checked at `:155-156`) — a reformat of a pre-existing copy stays silent.
  - **Moved-code guard**: drop it if the touched side's content matches a
    fragment that touched a **deleted** parent line (`deletedFragmentKeys`,
    `:94-106`, checked at `:153-154`) — code moved out of a deleted region is
    not "an additional copy."
- **Message** (`makeViolation`, `:108-121`): `"copy-paste introduced (" + p.type +
  "): lines " + startLine + "-" + endLine + " — clone of " + source.file + ":" +
  source.startLine + "-" + source.endLine` (`:117-119`). `v.line =
  introduced.startLine` (`:114`).
- **Config guard**: `diffMaxCloneScanBytes`, default `40 * 1024 * 1024`
  (`include/archcheck/config/config.h:55`), YAML key `diff_max_clone_scan_bytes`
  (`src/config/config_loader.cpp:326`). If the new tree's authored bytes exceed
  it, the whole-tree scan is **skipped** — `result.skippedLargeTree = true`, no
  violations (`new_clone_drift.cpp:174-178`) — comment: "the whole-tree pass
  would not fit the per-commit budget. Advisory-only, so the gate (cycles/god-
  headers) still runs and the commit is not lost" (`:171-173`).
- **Wiring**: `collectNewClones` (`src/cli/diff_command.cpp:151-166`) builds
  `AddedLineMap`/`DeletedLineMap` from `git::AddedLine`/`DeletedLine` and calls
  `detectNewClones`; `maxCloneScanBytes` flows from `DiffConfig::maxCloneScanBytes`
  (`:322`, overridden from config at `:337`) into `collectDiffAdvisories`
  (`:234,252`). If the diff's total added lines exceed `diffMaxAddedLines`
  (a separate guard, see the local-complexity page), the new-clone scan is
  skipped too — `collectSnapshotAdvisories` (which calls `collectNewClones`) is
  only reached past that early return (`diff_command.cpp:244-252`).
- **Reporting**: skip notice — `"\nnew-clone drift (advisory): skipped —
  authored tree exceeds thresholds.diff_max_clone_scan_bytes (#149)\n"`
  (`diff_command.cpp:283-285`); otherwise each violation via the shared
  `printViolationList("new-clone drift (advisory)", a.newClones.violations)`
  (`:286`), format `"  " << file << ":" << line << ": " << ruleId << " — " <<
  message` (`:264`). Also flattened into the JSON diff document
  (`flattenAdvisories`, `:297`).
- **Gate status**: advisory — README: "new/grown cycles and new god-headers
  gate, while added edges, chain/NCCD growth, SATD, test co-evolution, local
  complexity, flag-argument drift, and new clone drift are advisory"
  (`README.md:46`); signal table row "PR hygiene advisories | … new clones |
  reported, exit `0`" (`README.md:54`). The live-demo repo's firing PRs are
  described as making `DRIFT.NEW_CLONE` fire (`README.md:64`).
- **Attribution**: no Core Guidelines/Lakos/Martin citation found in code or
  README for this signal — `unverified` whether a formal attribution exists;
  the anchor is issue `#123` (and byte-cap issue `#149`).

## For an agent

- Change detection → `src/scan/new_clone_drift.cpp` (+ header). It calls into
  `duplication::scanForDuplication` (`src/scan/duplication/duplication_scanner.h`)
  — read `docs/duplication_architecture.md` first if touching scan internals.
- Change the byte-cap default → `Thresholds::diffMaxCloneScanBytes` in
  `include/archcheck/config/config.h` (SSOT), not the rule.
- Called only from `--diff` (`src/cli/diff_command.cpp:225-226`); no check-mode
  invocation exists.
- After changing: rebuild Debug, run `unit/scan/new_clone_drift_test.cpp` (7
  cases covering introduced/pre-existing/third-copy/moved/unrelated/empty/byte-
  cap scenarios), exercise `fixtures/diff_new_clone/{pass,pass_preexisting,
  fail_new_file,fail_existing_file}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/new_clone_drift.cpp:16` — `kRuleId = "DRIFT.NEW_CLONE"`.
- `src/scan/new_clone_drift.cpp:108-121` — violation message construction.
- `src/scan/new_clone_drift.cpp:164-186` — `detectNewClones`, byte-cap skip.
- `include/archcheck/config/config.h:55` — `diffMaxCloneScanBytes` default.
- `src/cli/diff_command.cpp:151-166,283-286,322,337` — wiring, skip message, config flow.
- `README.md:46,54,64` — advisory gate classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
