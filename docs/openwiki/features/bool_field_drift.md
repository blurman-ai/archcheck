---
title: "Bool-field drift — boolean-state accretion advisory"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/bool_field_drift.cpp"
  - "include/archcheck/scan/bool_field_drift.h"
  - "src/cli/diff_command.cpp"
  - "tests/unit/scan/bool_field_drift_test.cpp"
  - "tests/integration/scan/bool_field_drift_fixtures_test.cpp"
  - "fixtures/bool_field_drift/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Bool-field drift — boolean-state accretion advisory

## Purpose

Detects a struct/class that existed in the baseline and **gained** net depth-0
`bool` fields between two versions of a diff — incremental boolean-state
accretion. This is a **NEUTRAL** signal, not a verdict: the header comment states
it directly — "Advisory only. A struct that EXISTED in the baseline and gained net
bool fields is incremental boolean-state drift (#135 metric; constraint decay —
Martin 'Make Illegal States Unrepresentable')" (`bool_field_drift.h:15-17`). The
scanner reports *that* the count grew, not that the growth is wrong; whether N
booleans are legitimately orthogonal state or boolean-blindness (mutually
exclusive states that should be an enum) is a judgment call left to the reader,
not asserted by the tool.

## Verified facts

- **Rule id**: `kRuleId = "DRIFT.BOOL_FIELD_ACCRETION"` (`bool_field_drift.cpp:19`).
- **Two entry points** (`bool_field_drift.h:27-36`):
  - `compareBoolFields(oldSource, newSource, file)` — compares one file's two
    versions directly (impl `bool_field_drift.cpp:220-241`).
  - `detectBoolFieldDrift(oldSnapshot, newSnapshot, changedFiles)` — runs the
    comparison over a diff's changed files, using pre-read+classified
    `SourceSnapshot`s (impl `:243-262`); a file is skipped unless `authored`
    (vendored/test/generated dropped, `:253-255`).
- **Struct/field extraction** is regex + brace-matching, not a real parser:
  - `kStructRe` (`:25`) matches `struct`/`class` names, including qualified nested
    names like `Type::Function` (confirmed `bool_field_drift_test.cpp:120-127`).
  - `kBoolRe` (`:26-27`) matches a depth-0 `bool` field declaration, optionally
    `mutable`/`const`-qualified, with an optional bitfield (`: N`), default
    initializer (`= …` or `{…}`), and trailing `//` comment.
  - `static` bool members are excluded (class constants, not instance state) —
    confirmed `bool_field_drift_test.cpp:129-134`; `constexpr` never matches
    `const\s` in `kBoolRe` per the code comment (`:24`).
  - `neutralizeBraces` (`:83-101`) blanks `{`/`}` found inside string/char
    literals or comments before brace-depth matching, so a literal like
    `"x={ "` cannot corrupt struct-body detection (`#136`, confirmed
    `bool_field_drift_test.cpp:82-90`).
  - A trailing `//` comment's parens are stripped before the "is this a method
    declaration, not a field" paren guard (`appendStructBools:157`, `#164 A.3`,
    confirmed `bool_field_drift_test.cpp:100-107`).
- **Accretion test** (`pushIfAccreted`, `:194-216`): fires only when
  `newNames.size() > oldNames.size()` (net count) — a rename (old name replaced by
  new, same count) does **not** fire (confirmed
  `bool_field_drift_test.cpp:61-66`), nor does a reduction (`:68-73`).
  A struct absent from the baseline entirely (new file / new struct) is
  "greenfield, not drift" and skipped (`compareBoolFields:235`, confirmed
  `bool_field_drift_test.cpp:46-50`).
- **Message format** (`bool_field_drift.cpp:214-215`, literal):
  `"struct '" + name + "' accreted " + std::to_string(gainedCount) + " bool
  field(s): " + gained` — e.g. `"struct 'S' accreted 2 bool field(s): b, c"`
  (confirmed `bool_field_drift_test.cpp:109-118`). `gainedCount` counts **gross**
  adds among the newly-seen names (matching the listed names), while the fire
  condition itself is net (`:211`, `#164 A.4`).
- **Wiring**: called once per `--diff` run —
  `result.boolFields = collectBoolFieldDrift(...)` →
  `archcheck::scan::detectBoolFieldDrift(baseSnap, curSnap, *changed)`
  (`src/cli/diff_command.cpp:144`, wrapper `collectBoolFieldDrift` at `:136-145`).
- **Report header**: printed under the literal header `"bool-field accretion
  (advisory)"` via the shared `printViolationList` helper (`diff_command.cpp:288`),
  one line per finding as `"  " << file << ":" << line << ": " << ruleId << " — "
  << message` (`:264`).
- **Gate status**: **advisory only** — never gates the exit code (README gate
  table: "PR hygiene advisories | … | reported, exit `0`", `README.md:54`; also
  explicit in the header comment, `bool_field_drift.h:15`).
- **Attribution**: Martin, "Make Illegal States Unrepresentable" (cited directly in
  `bool_field_drift.h:17` and in CHANGELOG.md's `DRIFT.BOOL_FIELD_ACCRETION` entry).
  CHANGELOG also notes it replaces a research sidecar, "metric validated
  field-for-field against it (#090, #135)".

## For an agent

- Change detection → `src/scan/bool_field_drift.cpp` (+ header). The
  struct/bool-field regexes (`kStructRe`, `kBoolRe`, `:25-27`) and the brace
  neutralizer (`neutralizeBraces`, `:83-101`) are the fragile core — several `#164`
  fixes list specific FP reproducers (const-drop, brace-init rewrite, trailing
  comment parens) as regression tests, so any regex change should re-run the full
  `bool_field_drift_test.cpp` suite, not just new cases.
- Called only from the `--diff` path (`src/cli/diff_command.cpp:144`); there is no
  check-mode invocation.
- After changing: rebuild Debug, run `unit/scan/bool_field_drift_test.cpp` and
  `integration/scan/bool_field_drift_fixtures_test.cpp`, exercise
  `fixtures/bool_field_drift/{pass,fail_accretion}/`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/bool_field_drift.cpp:19` — `kRuleId = "DRIFT.BOOL_FIELD_ACCRETION"`.
- `src/scan/bool_field_drift.cpp:25-27` — struct/bool-field regexes.
- `src/scan/bool_field_drift.cpp:194-216` — accretion test + exact message format.
- `include/archcheck/scan/bool_field_drift.h:15-17` — "Advisory only" + Martin
  attribution, stated directly in the header comment.
- `src/cli/diff_command.cpp:136-145,288` — wiring + report header string.
- `README.md:54` — advisory gate classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
