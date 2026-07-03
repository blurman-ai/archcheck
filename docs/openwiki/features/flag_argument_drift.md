---
title: "Flag-argument drift — boolean flag parameters and literal calls"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/flag_argument_scan.cpp"
  - "include/archcheck/scan/flag_argument_scan.h"
  - "include/archcheck/scan/function_body_scan.h"
  - "src/cli/diff_command.cpp"
  - "tests/flag_argument_scan_test.cpp"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Flag-argument drift — boolean flag parameters and literal calls

## Purpose

Token-level heuristic flagging "behaviour leaking into the interface" via boolean
flag parameters — a cheap early-warning, "NOT a strict semantic rule"
(`flag_argument_scan.h:13`). Two sibling checks: flag-shaped function signatures
(`ARG.1`) and opaque boolean-literal call sites (`ARG.2`).

## Verified facts

- **Entry point**: `detectFlagArguments(const std::vector<duplication::Token>
  &tokens, const std::string &file)` (`flag_argument_scan.h:29-30`, impl
  `flag_argument_scan.cpp:187-194`) — runs both checks over one token stream via
  `discoverFunctions` (`include/archcheck/scan/function_body_scan.h:29`) for
  function spans.
- **`ARG.1.flag_argument_signature`** (`detectFlagSignatures`,
  `flag_argument_scan.cpp:119-139`) fires per discovered function when:
  - `boolParams >= 2` (`many`, `:127`) — two or more by-value `bool` parameters, or
  - `boolParams == 1 && flagLikeName(lastName)` (`:128`) — exactly one `bool`
    parameter whose name matches a flag-like prefix.
  - `bool*` / `bool&` parameters are **excluded** — `segmentIsBoolFlag` requires
    `hasBool && !hasPtrRef` (`:66-87`), confirmed
    `flag_argument_scan_test.cpp:37-40` ("out-parameters, not flags").
  - `bool` inside a template argument (e.g. `std::vector<bool>`) is not counted —
    `bracketDelta` treats `<`/`>` as nesting so the inner `bool` sits at `depth >
    0` and is skipped (`:74-77`, confirmed `flag_argument_scan_test.cpp:47-50`).
  - Flag-like prefixes (`flagLikeName`, `:21-39`): the literal array
    `kPrefixes = {"enable", "disable", "use", "force", "skip", "with", "without",
    "allow", "is", "no"}` (`:23-24`), matched as a prefix on a **word boundary**
    (`_` or an uppercase next char, or the name equals the prefix exactly,
    `:32-36`) — so `node`/`normal` (start with "no") do **not** trip, but
    `enableX`/`skip_y` do (confirmed `flag_argument_scan_test.cpp:24-35`).
  - Declarations and definitions dedup by `qualifiedName + "/" + paramArity`
    (`:131-133`).
  - Message (literal, `:134-136`): `many` → `"function '" + qualifiedName + "'
    takes " + N + " boolean flag parameters"`; single → `"function '" +
    qualifiedName + "' takes a boolean flag parameter '" + lastName + "'"`.
  - Violation: `{"ARG.1.flag_argument_signature", file, fn.startLine, msg}`
    (`:137`).
- **`ARG.2.boolean_literal_call`** (`detectBooleanLiteralCalls`,
  `flag_argument_scan.cpp:168-183`) fires on a call site (`id` token followed by
  `(`, that is **not** a function definition's own parameter-list paren,
  `signatureParen`/`:171-176`) passing `>= 2` boolean literals (`true`/`false`) at
  the call's **top level** (`countCallBoolLiterals`, `:142-160`, using
  `callBracketDelta` over `()[]{}` only — deliberately **not** `<>`, "far more
  often comparison operators … than template brackets", `:52-54`).
  - Control headers (`if`/`for`/`while`/`switch`/`catch`) tokenize as keywords,
    never `id`, so they are never candidates — confirmed
    `flag_argument_scan_test.cpp:65-70`.
  - A nested call counts only its own top-level literals — `outer(inner(true),
    false)` fires on neither (confirmed `:72-76`).
  - A function definition's own default-arg literals are not treated as a call
    (`ARG.1`'s territory) — confirmed `:78-84`.
  - Message (literal, `:180-181`): `"call passes " + N + " boolean literals
    positionally"`. Violation: `{"ARG.2.boolean_literal_call", file,
    tokens[k].line, msg}` (`:180`).
- **Wiring**: called from the `--diff` path over the **current** tree's authored
  sources, filtered to lines the diff actually added — `collectFlagArguments`
  (`src/cli/diff_command.cpp:171-188`) lexes each authored file
  (`duplication::lex` + `stripDirectiveTokens`, `:182`), runs
  `detectFlagArguments`, and keeps only findings whose `file:line` matches an
  added line (`:183-184`).
- **Report header**: printed under the literal header `"flag-argument drift
  (advisory)"` via the shared `printViolationList` helper (`diff_command.cpp:287`),
  one line per finding as `"  " << file << ":" << line << ": " << ruleId << " — "
  << message` (`:264`).
- **Gate status**: **advisory only** — never gates the exit code (README gate
  table: "PR hygiene advisories | … flag arguments … | reported, exit `0`",
  `README.md:54`; also stated in the source comment "Token-level, advisory",
  `flag_argument_scan.h:13`).
- **Attribution**: `ARG.1` — "Flag arguments are a known smell (C++ Core
  Guidelines F.21, 'prefer a different design'; Martin)" (CHANGELOG.md, #093).
  `ARG.2` has no separate CHANGELOG entry or explicit authority citation found in
  code — `unverified` whether it carries its own formal attribution beyond the
  shared "known smell" framing of `ARG.1`.

## For an agent

- Change detection → `src/scan/flag_argument_scan.cpp` (+ header). Function-span
  discovery itself (`FunctionSpan`, `discoverFunctions`) lives in
  `include/archcheck/scan/function_body_scan.h` + `src/scan/function_body_scan.cpp`
  — shared with other token-level scans, so a change there has a wider blast
  radius than this file alone.
- Called only from the `--diff` path (`src/cli/diff_command.cpp:171-188`); there is
  no check-mode invocation, and unlike the other three features in this group,
  **no dedicated `fixtures/` directory exists** for this scanner (verified: no
  `fixtures/flag_argument*` path in the tree) — coverage lives entirely in
  `tests/flag_argument_scan_test.cpp`.
- After changing: rebuild Debug, run `flag_argument_scan_test.cpp` (it is a flat
  test under `tests/`, not `tests/unit/scan/`), then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/flag_argument_scan.cpp:119-139` — `ARG.1` signature check + message.
- `src/scan/flag_argument_scan.cpp:168-183` — `ARG.2` call-site check + message.
- `src/scan/flag_argument_scan.cpp:21-39` — `flagLikeName` prefix list and
  word-boundary rule.
- `src/cli/diff_command.cpp:171-188,287` — wiring (added-line filter) + report
  header string.
- `README.md:54` — advisory gate classification.
- CHANGELOG.md, `#093` entry — `ARG.1` attribution (Core Guidelines F.21, Martin).

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
