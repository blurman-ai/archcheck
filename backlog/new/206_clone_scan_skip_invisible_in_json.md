# 206 — A skipped clone scan is indistinguishable from a clean one in JSON

**Type:** bug (silent censoring in the machine-readable output)
**Status:** new
**Found:** 2026-08-23, while normalising the adopter-sample run (#203 groundwork)

## What is wrong

`--diff` has two guards that switch the clone advisory off wholesale:

| guard | threshold | reported in text | reported in JSON |
|---|---|---|---|
| bulk import (#117) | `diff_max_added_lines` = 10,000 | yes | yes — `complexity_skipped_added_lines` |
| large authored tree (#149) | `diff_max_clone_scan_bytes` = 40 MB | yes | **no** |

Text output says so plainly (src/cli/diff_command.cpp:290):

```
new-clone drift (advisory): skipped - authored tree exceeds
thresholds.diff_max_clone_scan_bytes (#149)
```

The JSON advisory block carries `added_edges`, `removed_edges`,
`new_cross_area_dependencies`, `chain_length_grown`, `nccd_delta`,
`complexity_skipped_added_lines`, `rename_suppressed_cycles`, `violations` — and nothing for
`newClones.skippedLargeTree`. A consumer reading JSON sees zero clone violations and cannot tell
whether the scan ran.

## Why it matters

This is precisely the confusion `complexity_skipped_added_lines` was added to prevent. Its own
header says so (include/archcheck/diff/diff_json_report.h:20):

> lets JSON consumers tell a skipped zero from a genuinely clean zero. 0 = not skipped.

The same reasoning applies one level down, and JSON is the CI-facing format — the one where a
silent zero actually costs something. A repo above 40 MB of authored C++ reports "no new clones"
on every commit forever, and nothing in the output says the rule is off.

It also biases measurement built on the JSON: a corpus study cannot exclude the censored commits
because it cannot see them. That is how this was found — the adopter-sample run collected clone
counts per commit from `--format=json` and has no way to separate the two zeros.

## Fix

Add a boolean (or the byte count, matching the sibling field's shape) to the JSON advisory block
and thread `NewCloneDriftResult::skippedLargeTree` into `DiffJsonContext`. The text path already
has the information.

While there: check whether any other advisory has a skip path visible in text but not in JSON.
The bulk-import gate returns before `collectSnapshotAdvisories`, which also zeroes the
flag-argument and bool-field advisories and (via `bulk`) the whole graph section — those are
inferable from `complexity_skipped_added_lines > 0`, but only if the consumer knows the gate is
that wide. Worth a line in the field's documentation.

## Acceptance criteria

- [ ] JSON exposes the #149 clone-scan skip; 0/false when the scan ran
- [ ] Fixture: a diff over an authored tree above the threshold, asserting the field is set and
      `violations` is empty for the clone rule
- [ ] Fixture: a normal diff, asserting the field is unset
- [ ] `complexity_skipped_added_lines` documentation states that it also implies skipped
      graph, flag-argument and bool-field results
