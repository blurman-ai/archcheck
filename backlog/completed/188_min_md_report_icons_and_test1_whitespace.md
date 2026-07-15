# [DIFF][REPORT] Fix md-report icons and the TEST.1 whitespace false positive

**Date created:** 2026-07-15
**Date completed:** 2026-07-15
**Status:** completed
**Module:** DIFF / REPORT / GIT
**Priority:** minor
**Related:** #097 (test_co_evolution), #117 (bulk-import gate), #156 (new-clone demo)

## Goal

Two defects found while auditing the `archcheck-demo` showcase (14 PRs) ahead of the
launch article, which links those PRs as receipts.

## What was wrong

**1. Emoji shortcodes in `--format=md`.** The markdown reporter emitted GitHub emoji
shortcodes. `:large_yellow_circle:` is not a real GitHub alias — the correct name is
`:yellow_circle:` — so the advisory icon reached ten PR comments as literal text.
Confirmed against `gh api emojis` and by rendering the gate line through GitHub's
`/markdown` API. `:white_check_mark:` and `:x:` were valid and did render.

Shortcodes resolve only inside GitHub's renderer, so even the valid ones broke the
report when written to a file or read in a terminal. All three icons are now literal
Unicode (🟡 / ✅ / ❌).

A second reported symptom — a "broken character" in the gate line (`gate □ ok`) — was
**not real**. The only non-ASCII there is `·` and `—`, both of which render correctly;
the box was the ✅ emoji falling back to tofu in a terminal font.

**2. `TEST.1.prod_changed_tests_silent` fires on reformats.** `collectNumstat` did not
pass `-w`, so a whitespace-only change counted as production churn. On the demo's
`n5-formatting-only` PR — a tab→space reformat of `util.c`, and the showcase's headline
*negative* case — the rule reported `prod +105/-105 across 1 file(s), tests +0/-0`:

```
git diff --numstat   c8c83f0..baae605  ->  105  105  util.c
git diff -w --numstat c8c83f0..baae605  ->  (empty)
```

Task #097's stated intent is "production code changes **noticeably** while tests do not
move". A reformat moves no logic, so it cannot owe a test update. Whitespace was never
considered in #097 — this was an unhandled FP class, not a deliberate decision.

## How it works

`collectNumstat` gained a `Whitespace` enum (`Count` / `Ignore`) rather than a bool or a
blanket `-w`: its two callers want opposite things, and the enum names the reason at each
call site.

- **TEST.1** reads `Whitespace::Ignore` — a reformat is not a change.
- **The #117 bulk-import gate** keeps `Whitespace::Count` — a mass reformat is exactly the
  volume that gate exists to suppress. Making it whitespace-blind would let complexity
  advisories fire on functions touched only by re-indentation.

## Tests

- `writeMdReport: no emoji shortcodes in any gate state` — regex-asserts no `:shortcode:`
  survives into md output across clean / advisory / failing gates, with and without
  advisories. Verified to **fail** on the original bug before being trusted.
- `test_co_evolution: whitespace-only reformat does not trigger detection` — pins both
  halves: the reformat *would* fire under `Count` (170 lines of raw churn, over the
  80-line threshold) and stays silent under `Ignore`. Without the first assertion the
  test could pass for the wrong reason.

## Verification

- 653 tests pass; dogfooding clean; lizard clean; clang-format clean.
- All 14 demo PRs re-run against the fixed binary: finding **sets** identical to what was
  posted, proving the icon fix touched no detection logic.
- After the TEST.1 fix, 13 PRs byte-identical and only `n5` flipped to silent.
- Live showcase re-verified per-PR against the README: **9 fire / 5 silent, 0 shortcodes,
  0 mismatches**. All 14 comments regenerated.
- archcheck confirmed deterministic (3 runs, identical md5). The finding-order drift seen
  in six PRs is v0.1.5 → HEAD, unrelated to this change.

## Follow-up

The demo workflow pinned `ARCHCHECK_VERSION: v0.1.5`, which carries both bugs — merged PRs
do not re-run, but any new demo PR would regenerate them. Closed by releasing the fix and
bumping the pin.
