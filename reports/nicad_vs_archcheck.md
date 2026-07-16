# NiCad vs archcheck — copy-paste validation

Date: 2026-07-08.

This document records exactly the claim a local run confirmed. Nothing wider.

## What was run

NiCad was run on its C example `monit-4.2`:

```bash
./nicad3 functions c examples/monit-4.2
```

archcheck was run on the same source tree:

```bash
./build/debug/src/archcheck --duplication \
  experiments/clone_oracle_validation/nicad/repo/examples/monit-4.2
```

This is a **C** validation run. It is not a C++ NiCad run.

Raw detailed report:
[../experiments/clone_oracle_validation/NICAD_QUANT.md](../experiments/clone_oracle_validation/NICAD_QUANT.md).

## Results

| Metric | Value |
|---|---:|
| NiCad pairs | 27 |
| NiCad clone classes | 12 |
| archcheck pairs | 21 |
| NiCad classes covered by at least one archcheck pair | 8 / 12 |
| Real recall bugs found in archcheck | 0 |
| archcheck-only pairs | 9 |
| False positives among archcheck-only pairs | 0 |

## Why naive recall is misleading

NiCad reports clone classes as full cliques. If one clone class has five members,
NiCad can emit ten pair edges.

archcheck reports a compact subset of edges. If the same five files are covered
through a star-shaped set of edges, the clone class is still represented, but the
edge count is lower.

Therefore `matched edges / NiCad edges` makes archcheck look worse than it is.
The class-level view is the useful one for this comparison.

## What NiCad missed and archcheck found

archcheck reported 9 additional real clone pairs that NiCad did not report.
Manual triage found **0 false positives** among those 9.

The dominant reason: NiCad's run used `minsize=10`, so it does not report smaller
same-file copy-paste. archcheck does report several real copy-paste pairs below
that size.

Concrete conclusion:

> On the `monit-4.2` C validation run, archcheck found 9 real copy-paste pairs
> that NiCad missed, with 0 false positives in those archcheck-only pairs.

Shorter product wording:

> archcheck catches small same-file copy-paste that NiCad missed in our monit
> validation run.

## What archcheck deliberately did not report

The uncovered NiCad classes were triaged. They are not archcheck correctness bugs:

| Class | Reason |
|---|---|
| Whole-file twins | deliberately suppressed by archcheck |
| Lower-similarity read/write siblings | below archcheck's stricter floor / benign |
| Comment-heavy difference | archcheck strips comments before token comparison |

Correctness bugs found: **0**.

## What this does not prove

This does **not** prove that archcheck is a better research clone detector than
NiCad in general.

This does **not** prove a C++ head-to-head against NiCad. The run was
`functions c` on `monit-4.2`.

This does prove that archcheck's CI-oriented detector can catch actionable
copy-paste that NiCad misses under the tested settings, especially small same-file
copy-paste below NiCad's `minsize=10`.

## Claim to use

Use this:

> In a NiCad/monit cross-check, archcheck reported 9 additional real copy-paste
> pairs with 0 false positives; these were mostly small same-file clones below
> NiCad's `minsize=10`.

Do not use this:

> archcheck is better than NiCad.

The honest claim is narrower and stronger.

## Sources

- Raw validation:
  [../experiments/clone_oracle_validation/NICAD_QUANT.md](../experiments/clone_oracle_validation/NICAD_QUANT.md)
- Completed task #132:
  [../backlog/completed/132_maj_oracle_quantitative_validation.md](../backlog/completed/132_maj_oracle_quantitative_validation.md)
- Methodology/history task #107:
  [../backlog/completed/107_maj_dup_external_oracle_cross_validation.md](../backlog/completed/107_maj_dup_external_oracle_cross_validation.md)
- Clone detector landscape:
  [../docs/research/clone_tools_landscape.md](../docs/research/clone_tools_landscape.md)
