167 — Add unresolved local include / case mismatch gate before public launch
Context
archcheck 0.1.5 already exposes unresolved includes in the graph output, but the main gate can still report ok.

A small public C++/SFML project exposed the gap:

gate: ok
cycles: 0
unresolved: 7
Manual inspection showed that the unresolved entries were not harmless. They were local include path case mismatches:

#include "core/Board.hpp"
#include "rendering/Renderer.hpp"
#include "input/InputHandler.hpp"
while the actual files were:

include/core/board.hpp
include/rendering/renderer.hpp
include/input/inputHandler.hpp
On a case-sensitive Linux filesystem this is a real build portability bug.

This task adds a deterministic gate for that class of problem before the public launch.

Goal
Make archcheck report local include resolution problems as first-class findings, not only as graph metadata.

The tool should distinguish:

exact-case local include resolution failure;
local include path that resolves only with case-insensitive matching;
external/system includes that should not be treated as project violations without full compiler context.
Non-goals
Do not implement CMake parsing in this task.

Do not try to fully reproduce compiler include search semantics.

Do not fail on all unresolved includes by default if the include may be generated or external.

Do not introduce LLM-specific behavior. The output should remain deterministic and machine-readable.

Required behavior
1. Case mismatch must be a violation
If a quoted local include cannot be resolved with exact filesystem case, but a case-insensitive match exists under configured project/include roots, report:

CASE_MISMATCH_INCLUDE
Example:

#include "core/Board.hpp"
Actual file:

include/core/board.hpp
Expected diagnostic should include:

file: src/main.cpp
line: 2
include: core/Board.hpp
resolved_case_insensitive: include/core/board.hpp
This should fail the gate by default.

2. Unresolved local include must be visible and optionally failing
If a quoted local include cannot be resolved exactly and no case-insensitive match exists, report:

UNRESOLVED_LOCAL_INCLUDE
Default severity can be warning, because the header may be generated or depend on custom include roots.

Add a CLI/config option to fail on unresolved local includes, for example:

archcheck check . --fail-on-unresolved-local
or equivalent config:

unresolved:
  local_includes: fail
  case_mismatch: fail
  external_includes: ignore
Use the naming that best fits the existing CLI/config style.

3. External includes must not be treated as local violations
Do not report these as project violations in this task:

#include <vector>
#include <SFML/Graphics.hpp>
They may be counted in graph/debug metadata if the current implementation already does that, but they must not fail the gate.

Expected JSON shape
The exact schema may follow existing archcheck conventions, but it must be easy for an AI agent or CI system to consume.

Example shape:

{
  "version": 1,
  "gate": "failed",
  "violations": [
    {
      "rule": "CASE_MISMATCH_INCLUDE",
      "file": "src/main.cpp",
      "line": 2,
      "include": "core/Board.hpp",
      "resolved_case_insensitive": "include/core/board.hpp",
      "message": "Local include resolves only with different filesystem case"
    }
  ],
  "summary": {
    "total": 1,
    "by_rule": {
      "CASE_MISMATCH_INCLUDE": 1
    }
  }
}
Test fixtures
Add fixtures covering at least these cases.

Fixture A — exact match
include/core/board.hpp
src/main.cpp
#include "core/board.hpp"
Expected:

gate: ok
CASE_MISMATCH_INCLUDE: 0
UNRESOLVED_LOCAL_INCLUDE: 0
Fixture B — case mismatch
include/core/board.hpp
src/main.cpp
#include "core/Board.hpp"
Expected:

gate: failed
CASE_MISMATCH_INCLUDE: 1
Fixture C — unresolved generated-style local include
#include "generated/config.hpp"
Expected by default:

gate: ok or warning-only
UNRESOLVED_LOCAL_INCLUDE: 1
Expected with fail flag/config:

gate: failed
UNRESOLVED_LOCAL_INCLUDE: 1
Fixture D — external include
#include <SFML/Graphics.hpp>
#include <vector>
Expected:

no local include violation
Manual validation target
After implementation, re-run on the Gambit test repository.

Current expected useful result after this task:

cycles: 0
CASE_MISMATCH_INCLUDE: 7
gate: failed
The important part is that the gate must no longer say ok when local headers resolve only through case-insensitive matching.

Acceptance criteria
archcheck check reports CASE_MISMATCH_INCLUDE as a first-class violation.
Case mismatch fails the gate by default.
Unresolved quoted local includes are reported.
There is a documented way to make unresolved local includes fail the gate.
Angle-bracket includes do not fail this gate.
JSON output includes enough location/path data for an AI agent to propose a focused fix.
README or CHANGELOG contains a short note about the new behavior.
Release can be tagged as 0.1.6.
Why this matters
This improves the archcheck + AI workflow.

The deterministic tool gives a small external verdict first:

this include is unresolved
this path only works on case-insensitive filesystems
this dependency crosses a boundary
this block was copied
Only after that should an AI agent analyze the specific evidence and propose a patch.

That is better than asking the model to review the whole repository from scratch.