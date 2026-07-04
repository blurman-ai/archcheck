# FAQ — design rationale

Short answers to four questions that predictably come up. The long-form rationale
lives in [architecture-spec.md](architecture-spec.md).

## "A homegrown preprocessor scan will trip over macros and `#ifdef`"

The scanner sees both cases; the question is what it does with them.

**Conditional compilation.** The scanner tracks `#if`/`#ifdef`/`#ifndef` … `#endif`
([src/scan/include_scanner.cpp](../src/scan/include_scanner.cpp),
`opens_conditional`) and does not pick a branch: it collects `#include`s from every
branch and tags the edge `conditional`. The graph is a deliberate conservative
superset of the possible edges, which is the correct side of the error for cycle
and god-header rules: a cycle that exists under any configuration is worth
reporting.

**Computed includes** (`#include MY_HEADER_MACRO`). Recognized and reported as a
`DiagnosticKind::MacroInclude` diagnostic (same file). The real limitation: the
macro is not expanded, so the edge target stays unknown, but the site is flagged
rather than silently misparsed. Macro expansion needs semantics and is deferred to
the libclang backend (v0.2) by a recorded decision
([ADR-003](decisions/003-fast-backend-default.md)).

## "No C++20 modules (`import`) support — it will age out"

Correct: archcheck reads `#include` edges and knows nothing about `import` yet.
This is a deliberate deferral. Module-graph analysis is roadmap work and gets
built when a modules-based project asks for it (YAGNI). For the large majority of
C++ projects today, the include graph still is the physical architecture.

## "'Not a linter'? It detects clones, complexity, TODO/FIXME, silent tests"

Those checks exist, and none of them sit on the hard gate. The gate is narrow and
deterministic: include cycles (SF.9), include case mismatches, and in
drift/diff modes new or grown cycles and new god-headers (see `archcheck --help`
for the exact split). Everything fuzzy is advisory and never fails the run: clone
detection, complexity drift, SATD markers, bool-field accretion, even SF.7/SF.8
hygiene.

The purpose also differs from a linter's. A linter judges the contents of a file;
these signals judge what a change did to the dependency graph and the codebase
over time. The overlap with clang-tidy or SonarQube is in vocabulary, not in the
job.

## "76–91% clone precision on a hard gate will demotivate the team"

The premise of that argument is right, and it is the failure mode the
gate/advisory split exists to prevent. Fuzzy heuristics never gate. The gate
carries only deterministic, auditable graph facts (a cycle is present or it is
not; a case mismatch really does break a Linux build), and a gated finding comes
with the exact include chain that created it, quoted as `file:line`. One caveat is
inherited from the scanner: the graph is a conservative superset over `#if`
branches (see the first question), so a reported cycle may live in a configuration
you don't build — the quoted chain is what makes that auditable. The measured
precisions — ~92% cycle drift, 76–91% clones, 98%
bool-accretion claims ([CHANGELOG](../CHANGELOG.md),
[docs/research/](research/)) — describe advisories, where the cost of an error is
an ignored report line, not a red CI.

What remains real work is threshold tuning for the advisories. That is ongoing;
the numbers above are measured on corpus runs and manual triage, and they move
with each fix.
