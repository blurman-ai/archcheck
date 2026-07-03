---
title: "SF.8 — header must have an include guard or #pragma once"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/sf8_include_guard.cpp"
  - "include/archcheck/rules/sf8_include_guard.h"
  - "src/rules/rule_set.cpp"
  - "tests/unit/rules/sf8_include_guard_test.cpp"
  - "fixtures/sf8_include_guard/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
external_sources: []
---

# SF.8 — header must have an include guard or `#pragma once`

## Purpose

Flags a header that has neither `#pragma once` nor a real `#ifndef/#define` include
guard (Core Guidelines SF.8).

## Verified facts

- **Rule id / message**: emits
  `{"SF.8", <path>, 1, "header missing #pragma once or include guard (SF.8)"}`
  (`sf8_include_guard.cpp:167`). `line = 1`.
- **Scope**: header files only (`scan::isHeaderFile`, `:159`).
- **What counts as a guard**: `#pragma once` (`hasPragmaOnce`, `:23`) **or** a real
  pair `#ifndef NAME` + later `#define NAME` of the *same* macro (`closesGuardPair`
  `:126`, `hasIncludeGuard` `:141`). A lone `#ifndef` (e.g. an `NDEBUG` tweak) does
  **not** satisfy SF.8.
- **Scan budget**: only the first `kScanLines = 60` comment-stripped, non-blank code
  lines are scanned (`:21`, `:84`) — a long license banner never consumes the budget (#128).
- **Deliberate exclusions**: `.inc` fragments (`isIncFile`, `:121`) and Objective-C
  headers (`#import`/`@interface`, `isObjcFile`, `:108`) are skipped.
- **Registration**: `rule_set.cpp:21`. No threshold.
- **Gate status**: **structural advisory** — reported, exit `0` (`README.md:53`).
- **Attribution**: C++ Core Guidelines SF.8.

## For an agent

- Change → `src/rules/sf8_include_guard.cpp` (+ header). Guard detection is
  comment-aware but not string-aware (guard regions hold no string literals — see the
  `stripComment` comment).
- After changing: rebuild Debug, run `unit/rules/sf8_include_guard_test.cpp`, exercise
  `fixtures/sf8_include_guard/{pass,fail_*}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/sf8_include_guard.cpp:141-146` — guard acceptance (pragma or ifndef/define pair).
- `src/rules/sf8_include_guard.cpp:161-165` — `.inc` / ObjC exclusions.
- `src/rules/sf8_include_guard.cpp:167` — the emitted `SF.8` violation.

## Related pages

- [[../index.md]] · [[../cli-contract.md]]
