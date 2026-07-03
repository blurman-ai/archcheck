---
title: "Duplication — token-based clone detector"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/duplication/"
  - "include/archcheck/scan/duplication/"
  - "docs/duplication_architecture.md"
  - "src/main.cpp"
  - "src/cli/preview_commands.cpp"
  - "fixtures/duplication/"
  - "tests/duplication_clone_index_test.cpp"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Duplication — token-based clone detector

## Purpose

A **thin pointer**, not documentation. The duplication subsystem already has an
authoritative design doc — [docs/duplication_architecture.md](../../duplication_architecture.md)
(the SSOT: pipeline stages, normalization rationale, precision layers, FP
classes, metrics semantics). This page exists only so an agent lands on the
right doc/dir/tests/fixtures in one hop; it does not restate the design.

## Verified facts

- **Mode**: `archcheck --duplication <paths>` is a **report-only, advisory**
  mode — it never gates CI. Help text: "report duplicate code; advisory, does
  not gate CI" (`src/main.cpp:36`); flag parsed at `src/main.cpp:232`; listed
  among the advisory preview flags at `src/main.cpp:257`. Confirmed in README
  under "Advisory reports (report-only, never gate)" (`README.md:142-143`) and
  the Status section, "advisory duplication ... reports" (`README.md:229`).
- **Detector**: a single retained **token-based clone detector** (#056),
  targeting Type-3 "edited copies" (Kapser & Godfrey clone taxonomy) as the
  sole layer — the line-based (#053) and AST cross-TU (#052) layers were
  removed from the tree (`docs/duplication_architecture.md:6-16`).
- **CLI wiring**: the `--duplication` handler calls
  `archcheck::scan::duplication::scanForDuplication(sources)`
  (`src/cli/preview_commands.cpp:130`).
- **Implementation**: `src/scan/duplication/{duplication_scanner,fragmenter,
  token_normalizer,similarity,clone_index,clone_classifier,fp_corpus_eval}.cpp`
  + matching headers under `include/archcheck/scan/duplication/`.
- **Tests**: 17 flat test files, no subdirectory — `tests/duplication_*_test.cpp`
  (e.g. `tests/duplication_clone_index_test.cpp`, `tests/duplication_fragmenter_test.cpp`).
- **Fixtures**: `fixtures/duplication/{data_table,normalization,test_boilerplate,
  wholefile_extract_fp}/`.
- **Authoritative design doc**: [docs/duplication_architecture.md](../../duplication_architecture.md).
  Read it before touching any layer — do not re-derive design intent from this page.

## For an agent

- Need to change duplication behavior or understand a metric/threshold? Read
  [docs/duplication_architecture.md](../../duplication_architecture.md) first,
  then `src/scan/duplication/`. This page only routes you there.
- After changing: rebuild Debug, run the `duplication_*_test.cpp` tests, then run the standard post-change checks (see [schema.md](../schema.md)).
- `--duplication` is advisory-only by design (see Purpose in the SSOT doc,
  "detection without a code change = zero" framing) — do not add a gating path
  here without a CLAUDE.md / exit-code-contract discussion; see
  [../cli-contract.md](../cli-contract.md) for the exit-code contract.

## Evidence

- `src/main.cpp:36,232,257` — flag help text, parsing, advisory classification.
- `src/cli/preview_commands.cpp:130` — CLI → scanner wiring.
- `README.md:142-143,229` — advisory / exit-0 confirmation.
- `docs/duplication_architecture.md:6-16` — single-detector decision (token-based, #056).

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
