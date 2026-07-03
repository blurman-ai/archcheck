---
title: "SF.7 — no 'using namespace' at global scope in a header"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/sf7_using_namespace.cpp"
  - "include/archcheck/rules/sf7_using_namespace.h"
  - "src/rules/rule_set.cpp"
  - "tests/unit/rules/sf7_using_namespace_test.cpp"
  - "fixtures/sf7_using_namespace/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
external_sources: []
---

# SF.7 — no `using namespace` at global scope in a header

## Purpose

Flags `using namespace` written at global scope inside a header (Core Guidelines
SF.7: don't `using namespace` at global scope in a header file).

## Verified facts

- **Rule id / message**: emits
  `{"SF.7", <path>, <line>, "'using namespace' at global scope in header (SF.7)"}`
  (`sf7_using_namespace.cpp:67`). `line` is the real 1-based source line.
- **Scope**: header files only (`scan::isHeaderFile`, `:91`); triggers only at
  brace depth 0 — i.e. global scope, not inside a namespace/class block
  (`braceDepth == 0`, `:66`).
- **Comment handling**: strips `//` line comments and tracks `/* */` block comments
  before matching (`stripLineComment` `:15`, `updateBlockCommentState` `:32`) — a
  `using namespace` inside a comment does not trigger.
- **Registration**: `rule_set.cpp:20`. No threshold.
- **Gate status**: **structural advisory** — reported, exit `0` (README gate table,
  `README.md:53`).
- **Attribution**: C++ Core Guidelines SF.7.

## For an agent

- Change → `src/rules/sf7_using_namespace.cpp` (+ header). Line-scanning helpers are
  file-local (anonymous namespace) — the matcher is approximate (ignores string
  literals; see the `stripLineComment` comment).
- After changing: rebuild Debug, run `unit/rules/sf7_using_namespace_test.cpp`,
  exercise `fixtures/sf7_using_namespace/{pass,fail_*}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/sf7_using_namespace.cpp:66-67` — the depth-0 check + emitted violation.
- `src/rules/sf7_using_namespace.cpp:91` — header-only guard.
- `README.md:53` — structural-advisory classification.

## Related pages

- [[../index.md]] · [[../cli-contract.md]]
