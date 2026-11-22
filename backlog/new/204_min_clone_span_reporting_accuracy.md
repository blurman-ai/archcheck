# 204 — Clone findings report spans that do not match what was compared

**Type:** bug (precision of the message, not of the detection)
**Status:** new
**Found:** 2026-08-22, during the clone-core A/B on the characteristic sample (#203 groundwork)

## What is wrong

A `DRIFT.NEW_CLONE` message names two line ranges. On hand-checked findings those ranges do not
describe what actually matched, in two distinct ways.

**1. Window-free over-reports the span.** `kernelslacker/trinity` `f36f4d0a00f5`:

```
copy-paste introduced (MIXED): lines 926-947 - clone of childops/io_uring/recipes-net.c:142-162
```

22 lines claimed on each side. Reading both: `supervisor.c:926-947` is a `WIFEXITED` /
seccomp-strict block, `recipes-net.c:142-162` is an `ENOSYS` block. What they share is three
lines:

```c
*unsupported = true;
__atomic_add_fetch(&shm->stats....., 1, __ATOMIC_RELAXED);
return false;
```

The surrounding code is unrelated. A reviewer who trusts the range sees a 22-line copy; a
reviewer who opens the files sees an idiom. The first reading is the one that gets acted on.

**2. Spans overlap each other.** `ignaciosgithub/GameEngineDarkest` `707f8275f9ac`:

```
lines 82-92 - clone of src/Rendering/Meshes/Mesh.cpp:92-102
```

Same file, and line 92 is in both sides. The underlying finding is real (two `if/else` branches
with an identical error-check + log block), but a pair that shares a line reads as a detector
artifact and undermines a true positive.

## Why it matters

The message is the whole product surface for this rule — nobody sees fragments, indices or
scores, only `file:line - clone of file:line`. An inflated span turns a JUSTIFIED idiom into
what looks like a serious copy, which is exactly the FP class the project already spends effort
suppressing. It also makes hand-auditing slower: every finding has to be opened to learn what
part of the range is load-bearing.

## Not in scope

Whether these findings *should* fire at all — that is the core-selection question and belongs to
the clone-core work. This task is only about the reported range describing the matched region.

## Acceptance criteria

- [ ] A finding's reported range covers the matched region and not more — or, if a widened range
      is deliberate (anchor context), the message says which part matched
- [ ] Two sides of a pair never overlap when they are in the same file
- [ ] Fixtures: one scenario per defect, asserting the reported range against the known match
- [ ] The two commits above re-checked by hand after the fix

## Evidence

`experiments/rerun_2026-08/sample_results.jsonl` + `show_disagreements.py`, which prints each
finding together with both code fragments:

```
python3 show_disagreements.py only_windowfree 4 1
python3 show_disagreements.py agree 5 1
```
