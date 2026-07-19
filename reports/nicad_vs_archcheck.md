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

> **Granularity caveat (added 2026-07-18, #190).** The run used NiCad's `functions`
> granularity — the same granularity archcheck itself works at. Every number and verdict
> below, including "0 recall bugs", is scoped to function-granularity clones and says
> nothing about copy-paste nested *inside* a function. archcheck cannot detect that class
> at all: `fragmenter.cpp` emits a function body whole and never descends into it.
> The `blocks` rerun has since been done — see [§Sub-function granularity](#sub-function-granularity-blocks-rerun-190).

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

## Sub-function granularity: `blocks` rerun (#190)

Date: 2026-07-18. Same source tree, same archcheck build:

```bash
./nicad3 blocks c examples/monit-4.2      # minsize=10, threshold=0.3, blind rename
```

NiCad extracted 1506 blocks and reported 35 clone pairs / 20 classes, against 437
functions and 27 pairs at `functions` granularity.

To isolate the sub-function class, each `blocks` class was tested against the
`functions` classes; a `blocks` class with no function-granularity counterpart is a
genuinely sub-function finding.

| Metric | Value |
|---|---:|
| NiCad `blocks` classes | 35 |
| of those, also visible at `functions` granularity | 27 |
| **genuinely sub-function classes** | **8** |
| archcheck finds — before #190 | 1 |
| **archcheck finds — after #190** | **6** |

The 7 pre-#190 misses, all copy-paste nested inside function bodies. The last two are
still missed after the fix (both are low-similarity cross-file cases):

| Lines | Similarity | Location | After #190 |
|---:|---:|---|---|
| 23 | 86 | `http/cervlet.c:521-545` ↔ `555-579` | found |
| 19 | 94 | `http/cervlet.c:1550-1580` ↔ `1711-1741` | found |
| 15 | 73 | `alert.c:100-127` ↔ `148-168` | found |
| 13 | 76 | `util.c:653-671` ↔ `http/cervlet.c:1447-1465` | still missed |
| 11 | 100 | `http/cervlet.c:1325-1341` ↔ `1412-1428` | found |
| 10 | 100 | `http/cervlet.c:2020-2029` ↔ `2034-2043` | found |
| 10 | 70 | `util.c:1084-1095` ↔ `1166-1177` | still missed |

Two were verified by hand:

- `cervlet.c:1325-1341` ↔ `1412-1428` — a byte-identical 17-line
  `{ struct mytimestamp *t; for (...) ... }` block, copied between two *different*
  functions (`do_service_directory` and `do_service_file`). Now reported as
  `1328-1339 ↔ 1415-1426` (EXACT).
- `cervlet.c:2020-2029` ↔ `2034-2043` — the same permission-check ladder twice inside
  `get_service_status`, differing only in `S_ISREG` vs `S_ISDIR`. Now reported as
  `2021-2028 ↔ 2035-2042` (EXACT).

**Even at 6 of 8 this is a floor, not the ceiling.** NiCad ran at `minsize=10`
pretty-printed lines, so blocks shorter than that are invisible to this oracle
regardless of what archcheck does — including the 5-line duckdb
`pipeline_executor.cpp` case that motivated #190 (which archcheck now reports).

### Why this was not extended to C++

NiCad has no C++ grammar; the standard workaround is to present C++ files to the C
parser. On eight C++ repositories that workaround fails outright — NiCad could not
parse 38–94% of the files (Kartend 94%, IrredenEngine 90%, BambuStudio 87%,
LibreSprite 86%, Catch2 83%), and the `blocks` run on irrlicht aborted with `rc=99`.
A recall comparison over the ~10% of files that parsed carries no signal, so no C++
`blocks` number is reported here. The C run above is the usable measurement.

## What this does not prove

This does **not** prove that archcheck is a better research clone detector than
NiCad in general.

This does **not** prove a C++ head-to-head against NiCad. The run was
`functions c` on `monit-4.2`, and the C++ workaround does not parse (see above).

The sub-function numbers are a **lower bound** measured on one small C codebase at
`minsize=10`. They do not estimate how common the class is in C++ generally.

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
