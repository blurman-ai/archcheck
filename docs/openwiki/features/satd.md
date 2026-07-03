---
title: "SATD — self-admitted technical debt markers in `--diff`"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/satd_scan.cpp"
  - "include/archcheck/scan/satd_scan.h"
  - "include/archcheck/scan/authored_scope.h"
  - "src/cli/diff_command.cpp"
  - "tests/unit/scan/satd_scan_test.cpp"
  - "fixtures/satd_delta/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# SATD — self-admitted technical debt markers in `--diff`

## Purpose

Scans the **added lines** of a `--diff` for comment markers the author left behind
admitting debt — TODO/FIXME/HACK/XXX/TEMP and lowercase equivalents
(`satd_scan.h:11-22`). SATD = Self-Admitted Technical Debt.

## Verified facts

- **Entry point**: `detectSatdMarkers(const std::vector<git::AddedLine> &addedLines)`
  (`satd_scan.h:23`, impl `satd_scan.cpp:130-160`). Only fed **added** lines from a
  unified diff — not a whole-tree scan.
- **Two rule ids, evaluated per added line** (`satd_scan.cpp:78-102`, `:146-156`):
  - `SATD.1` — any marker: uppercase `\b(TODO|FIXME|HACK|XXX|TEMP)\b`
    (`kUppercasePattern`, `:66`) or case-insensitive
    `\b(temporary|workaround|dirty)\b|quick.?fix` (`kLowercasePattern`, `:72`).
  - `SATD.2` — more specific, checked first (`:146`): `FIXME` or `HACK`
    (`kFixmeOrHack`, `:90`) **without** an issue id nearby, where an issue id matches
    `([A-Z][A-Z0-9]+-\d+)|(#\d+)|(\b(gh|issue)[-/]\d+)` (`kIssueId`, `:96`). If an
    issue id is present, `SATD.2` is suppressed and only `SATD.1` fires (confirmed by
    `satd_scan_test.cpp:131-162`, e.g. `HACK PROJ-123` → `SATD.1` only).
  - One violation per line (not per marker) — `satd_scan.cpp:150,156`.
- **Comment-only**: the marker must be inside a `//` line comment or `/* … */` block
  comment; `extractCommentPart` (`:32-62`) tracks quote state so a marker inside a
  string literal (e.g. `"TODO"`) is ignored (confirmed by
  `satd_scan_test.cpp:184-192`, "marker in code (no comment) is ignored").
- **Exclusion**: vendored / test / generated files are skipped via the shared gate
  `AuthoredScope::pathExcluded(added.file)` (`satd_scan.cpp:139`, gate defined
  `include/archcheck/scan/authored_scope.h:33-38`) — "one shared gate every rule
  uses (#129), not an open-coded subset" (`satd_scan.cpp:136-138`).
- **Message truncation**: `truncateMessage(msg, maxLen = 120)` (`satd_scan.cpp:105`)
  breaks at the last space/tab before 120 chars and strips trailing whitespace
  (`:105-126`); confirmed by `satd_scan_test.cpp:221-234`.
- **Violation shape**: `{ruleId, added.file, added.lineNumber, truncateMessage(added.text)}`
  (`satd_scan.cpp:150,156`) — `line` is the real added-line number (not 0; this is a
  line-level signal, unlike the graph-level rules).
- **Wiring**: called once per `--diff` run —
  `result.satd = archcheck::scan::detectSatdMarkers(addedLines);` (`src/cli/diff_command.cpp:239`).
- **Report header**: printed under the literal header
  `"self-admitted technical debt (advisory)"` via the shared
  `printViolationList` helper (`diff_command.cpp:258-269`), one line per finding as
  `"  " << file << ":" << line << ": " << ruleId << " — " << message` (`:264`).
- **Gate status**: **advisory only** — never gates the exit code (README gate table:
  "PR hygiene advisories | SATD, test co-evolution, local complexity, flag arguments,
  new clones | reported, exit `0`", `README.md:54`). CHANGELOG: "Reported after the
  structural diff, never gates." (CHANGELOG.md, SATD delta advisory entry, #096).
- **Attribution**: no Core Guidelines/Lakos/Martin citation found in code or
  CHANGELOG for this specific signal — `unverified` whether it carries a formal
  attribution the way the default rule set does; CHANGELOG only cites issue #096.

## For an agent

- Change detection → `src/scan/satd_scan.cpp` (+ header). The shared vendor/test/
  generated exclusion is `include/archcheck/scan/authored_scope.h` — don't
  reimplement a local exclusion list (CLAUDE.md "Constants — into config first" /
  #129 pattern).
- Called only from the `--diff` path (`src/cli/diff_command.cpp:239`); there is no
  check-mode invocation.
- After changing: rebuild Debug, run `unit/scan/satd_scan_test.cpp`, exercise
  `fixtures/satd_delta/{pass,fail}/*.diff`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/satd_scan.cpp:64-102` — `SATD.1` / `SATD.2` regex definitions and checks.
- `src/scan/satd_scan.cpp:130-160` — `detectSatdMarkers`, exclusion gate, violation push.
- `src/scan/satd_scan.cpp:105-126` — 120-char message truncation.
- `src/cli/diff_command.cpp:239,269` — wiring + report header string.
- `README.md:54` — advisory gate classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
