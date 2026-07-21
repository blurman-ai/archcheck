# [RULES][SF] 189: SF.11 — a header must not include a precompiled header

**Created:** 2026-07-18
**Start date:** —
**Status:** new
**Module:** RULES/SF
**Priority:** major
**Difficulty:** small
**Blocks:** —
**Blocked by:** —
**Related:** #031 (godheader_pch_exclusion — reuses `isKnownPchBasename`), #126 (sf9_collapse_impl_into_component), #032 (conditional_include_cycles)

## Goal

Add a default rule that flags a **header** (`.h`/`.hpp`/`.hxx`/`.hh`) which `#include`s a
**precompiled header** (`pch.h`, `stdafx.h`, `precompiled.h`, …). This is a direct, graph-visible
violation of header self-containment, and it is the *root* of the pch-through-header cycles the
SF.9 rule currently reports as a mere symptom.

## Context

### Where this came from

During the outreach corpus sweep (2026-07-17/18) archcheck reported an SF.9 cycle in
`microsoft_MIDI`: `MidiMessage128.h ⇄ pch.h`. Investigating it surfaced a deeper problem than a
cycle:

- `pch.h` includes 95 local headers, 78 of them the project's own `Midi*.h`.
- Four `Midi*.h` include `pch.h` back → SF.9 fires.

The author's point: the real defect is not the cycle. It is that **a header includes the
precompiled header at all**. A `.h` is not a translation unit; a PCH exists to speed up TU
compilation and belongs at the top of a `.cpp`, nowhere else. A header that pulls the PCH:

1. Is not self-contained — it cannot be compiled or understood on its own; it silently relies on
   whatever the PCH dragged in.
2. Hides its real dependencies — "give me the whole box" instead of naming what it uses.
3. Abuses a build-speed mechanism as an implicit dependency store.

This violation exists **with or without a cycle**. Break the four back-edges in MIDI and SF.9 goes
quiet, but every `.h → pch` edge remains, now invisible. SF.9 catches the symptom; nothing catches
the cause.

### Authority (not opinion — clears the archcheck attribution bar)

- **C++ Core Guidelines SF.11** — "Header files should be self-contained." A header including a PCH
  is not self-contained. <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>
- **C++ Core Guidelines SF.10** — "Avoid dependencies on implicitly `#included` names."
- **Compiler docs, hard rule**: "you cannot include a precompiled header from inside another
  header." <https://learn.microsoft.com/en-us/cpp/build/creating-precompiled-header-files>
- Also in the Google C++ Style Guide (self-contained headers) and Sutter/Alexandrescu
  *C++ Coding Standards*, item 23 ("Make header files self-sufficient").

archcheck already ships SF.7/8/9 from this same family; SF.10 and SF.11 are unimplemented
neighbours. This lands SF.11's first concrete, unambiguous case.

### Frequency (measured, not assumed)

Corpus grep, `.h`/`.hpp`/`.hxx` that `#include` a known PCH basename (excluding the PCH files
themselves), across all 2042 repos:

- **39 repos, 259 header files** violate this.
- Not one odd project: `microsoft_Teams-AdaptiveCards-Mobile` (84), `justnullname_QuickView` (27),
  `OpenKneeboard` (25), `microsoft_MIDI` (13), `microsoft_terminal` (11), `opencv` (7),
  `LibreOffice_core` (5), `inet-framework_inet` (5)…

Most of these are **not** cycles, so SF.9 never sees them. They are exactly what this rule adds.

### Competitive gap (checked)

- **clang-tidy** has `misc-header-include-cycle` — it detects the *cycle* (A↔B), same as SF.9. It
  does **not** flag `.h → pch` on its own. So the ~255 non-cyclic cases here are invisible to it.
  <https://clang.llvm.org/extra/clang-tidy/checks/misc/header-include-cycle.html>
- **SonarQube (sonar-cxx)** has no such rule; in practice it *stumbles* over PCH (tries to simulate
  it) rather than checking it. <https://community.sonarsource.com/>
- No mainstream linter flags `header includes PCH` as its own rule → a genuine differentiator, and
  it fits archcheck's include-graph-first, zero-`compile_commands.json` model perfectly.

### Why this is cheap

- Pure include-graph check — no symbols, no libclang. Fits the fast backend.
- Deterministic and threshold-free: a header either includes a PCH or it does not.
- The PCH detector already exists: `scan::isKnownPchBasename()` in
  `include/archcheck/scan/file_classification.h:312` (added in #031 for god-header). Reuse it.
- The "is this node a header" side is already known to the graph.

## Open design questions (decide before coding)

1. **Rule id / name.** `SF.11` (self-contained headers) with message specific to PCH? Or a
   narrower id like `SF.11.PCH`? Leaning `SF.11` with a PCH-specific message, leaving room for
   other self-containment cases later.
2. **Gating or advisory?** SF.9 gates (exit 1). This is a real defect but a *style/physical-design*
   one, and gating on 259-across-39-repos could mean "5000 violations on first run" for adopters
   → probably **advisory** by default (like SF.7/8), with `--baseline` covering legacy. Confirm.
3. **Interaction with SF.9.** When the PCH edge also closes a cycle (MIDI), both SF.9 and SF.11
   would fire on overlapping files. Acceptable (different lens), or should SF.11 suppress the SF.9
   line in that specific pch case? Leaning: keep both, they say different things.
4. **Custom PCH names.** `isKnownPchBasename` covers 4 hardcoded names + `extraExcludes`. Enough
   for v0.1; config-driven names when the YAML config lands.

## Execution plan

- [ ] Decide the four design questions above (rule id, gating, SF.9 overlap, names).
- [ ] New rule class `Sf11SelfContainedHeader` — one file pair under `src/rules/`, registered in
      `makeDefaultRuleSet` (`src/rules/rule_set.cpp`). One rule = one class = one file.
- [ ] Check: for each graph node that is a header (not `isKnownPchBasename`), if any of its direct
      includes `isKnownPchBasename`, emit `SF.11` at the includer with `file:line`.
- [ ] Wire severity into `gate_policy.h` (advisory unless decided otherwise).
- [ ] Fixtures `fixtures/sf11_self_contained/pass/` and `fail_*/`.
- [ ] Dogfood: archcheck's own `src/ include/ tests/` must stay clean (it has no PCH → trivially 0).
- [ ] CHANGELOG entry; openwiki page if a rule page pattern exists.

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Confirm gating decision (advisory vs gate) with the author — drives adopter first-run experience.
2. Pick the rule id.

## Key decisions

| Decision | Reason |
|---------|---------|
| Reuse `isKnownPchBasename` | Detector already exists (#031); no second PCH list (dedup-before-copy) |
| Attribute to Core Guidelines SF.11/SF.10 | Authority over opinion; not a bespoke "author's blister" rule |
| Include-graph only, no symbols | Fits the fast backend; keeps it in v0.1 scope, not v0.2 libclang |

## Changed files

| File | Change |
|------|-----------|
| `src/rules/sf11_self_contained_header.{h,cpp}` | new rule class |
| `src/rules/rule_set.cpp` | register in `makeDefaultRuleSet` (one line) |
| `include/archcheck/rules/gate_policy.h` | severity for SF.11 |
| `fixtures/sf11_self_contained/` | pass + fail_* |
| `CHANGELOG.md` | rule entry |

## Fixtures (rule)

- [ ] `fixtures/sf11_self_contained/pass/` — a `.h` that includes only what it uses, no PCH
- [ ] `fixtures/sf11_self_contained/fail_header_includes_pch/` — a `.h` that `#include "pch.h"`
- [ ] `fixtures/sf11_self_contained/fail_hpp_includes_stdafx/` — `.hpp` → `stdafx.h`
