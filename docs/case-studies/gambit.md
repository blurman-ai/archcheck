# Case study: Gambit

## Project

[Gambit](https://github.com/abhi-saurav-saroya/Gambit) is a public C++17/SFML chess
project by Abhi Saurav Saroya — board and game logic, pieces, rendering, and input,
with legal moves, castling, en passant, promotion, checkmate, and stalemate.

The author introduced it on LinkedIn as a project built after semester exams and
explicitly invited feedback and code review. This case study is that review loop, not
a public critique: the author reviewed, tested, and merged the resulting PRs, and
approved featuring Gambit here with credit and a repository link.

## What archcheck reported

`archcheck` was run against the repository. The architecture gate was quiet:

```text
gate: ok
cycles: 0
```

The graph pass did not invent violations. Two practical signals came from elsewhere:

- **7 unresolved local includes.** In `archcheck 0.1.5` these surfaced only as graph
  metadata while the main gate still reported `ok`. In `0.1.6` the same signal is a
  first-class, gating finding:

  ```text
  CASE_MISMATCH_INCLUDE: 7
  cycles: 0
  gate: failed   (exit 1)
  ```

- **Duplicated sliding-piece move generation.** The duplication pass flagged repeated
  ray-walking logic shared by bishop, rook, and queen.

## Finding 1: Linux case-sensitivity build issue

Several local includes and CMake references used a different filename case than the
files on disk:

```cpp
#include "core/Board.hpp"        // file is include/core/board.hpp
#include "rendering/Renderer.hpp" // file is include/rendering/renderer.hpp
#include "input/InputHandler.hpp" // file is include/input/inputHandler.hpp
```

This resolves on a case-insensitive filesystem (a typical Windows setup) but breaks on
a case-sensitive Linux build. It is a portability issue, not a naming preference.

`archcheck 0.1.6` reports each mismatch as `CASE_MISMATCH_INCLUDE` with `file:line`.
(The same detector independently found real case mismatches in Windows-authored code
under OpenCV, so the class is not Gambit-specific.)

**Fix:** a PR normalized the include and CMake path case to match the filesystem.

## Finding 2: duplicated sliding-piece move generation

Bishop, rook, and queen each carried the same ray-walking loop; only the direction set
differed. A stale comment made the risk concrete — a queen loop labelled "try all 4
directions" iterated over eight:

```cpp
// try all 4 directions
for (int dir = 0; dir < 8; dir++) {
```

The concern is not the repetition itself but what happens next: once one copy changes,
the copies disagree, and a later AI-assisted edit sees several conflicting local
examples with no canonical version. A deterministic pass names the shared structure
before a model has to infer it.

**Fix:** a PR extracted the shared ray-walking helper and reused it for all three
pieces, preserving behavior.

## Verification

The move-generation refactor was checked with equivalence testing over random boards —
old implementation against new — so the maintainer did not have to trust it by
inspection alone. From the author's review:

> I especially appreciated the detailed explanations and the equivalence testing — it
> made reviewing the changes much easier.

## What was merged

Two PRs were submitted; the author reviewed and tested both locally and merged them:

- the case-sensitivity fix;
- the move-generation refactor (equivalence-tested);
- a CI workflow to catch the Linux build issue automatically.

## Why it matters

Without a deterministic pass, the prompt is vague: *"review this C++ chess project."*
With one, the agent starts from bounded evidence — exact files, exact lines, a clear
definition of done:

```text
7 case-mismatched local includes (break a case-sensitive Linux build).
No include cycles.
Duplicated bishop/rook/queen move generation.
```

The workflow the case demonstrates:

```text
deterministic structural finding
→ scoped AI-assisted patch
→ equivalence test
→ PR review
→ merge
```

`archcheck` acted as the deterministic front-end that reduced the search space before
any code was edited.

---

*Credit: Abhi Saurav Saroya, author of Gambit, who reviewed and merged the PRs and
approved this case study. Repository: https://github.com/abhi-saurav-saroya/Gambit*
