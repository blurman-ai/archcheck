# Graph-baseline format

Minimal specification of the `graph-baseline` format — a snapshot of the project's
dependency graph that archcheck stores in the repo and compares against the current
graph for DRIFT rules (see §"Drift-regression rules" in `docs/architecture-spec.md`).

## Purpose

A baseline is an **immutable snapshot of resolved project edges and nodes** of the
graph. It is the contract against which drift violations are computed. The file is
stored in the repository, reviewed in PRs, and read by humans.

## Schema (YAML)

```yaml
format_version: "1"
nodes:
   - "include/foo.h"
   - "include/bar.h"
   - "src/foo.cpp"
edges:
   - [2, 0]
   - [2, 1]
```

Fields:

- `format_version` — a string, current value `"1"`. A string, not a number, because
  YAML numbers have their own surprises (`1.0` ≠ `1`, leading zeros, etc.).
- `nodes` — an array of strings, normalized paths of the graph nodes.
- `edges` — an array of pairs `[from_idx, to_idx]`, indices into the `nodes` array.

## Determinism

The same logical graph **must** serialize byte-for-byte identically, regardless of
the insertion order of nodes and edges into `DependencyGraph`. This is guaranteed by
two sorts:

- `nodes` are sorted lexicographically (by `std::string`).
- `edges` are sorted by the pair `(from_idx, to_idx)`, where indices already use the
  post-sort numbering of `nodes`.

## What is NOT in the baseline

- `external` / `unresolved` / `ambiguous` includes — they are unstable across
  machines and builds.
- Scanner diagnostics.
- Derived metrics (`SCC`, `CCD`, `ACD`, `NCCD`, blast radius, hubness) — recomputed
  on load.

The baseline stores only the subgraph of project files with resolved edges.

## Versioning

Any schema change (new fields, renames, semantic changes) → a new `format_version`
value. On an unknown version the loader returns
`BaselineLoadError{Kind::UnknownVersion}` and **does not attempt to read further**.

## Error handling

`load_baseline` does not throw exceptions outward. All errors are returned as a
structured `BaselineLoadError`:

| `Kind`             | When                                                       |
|--------------------|-------------------------------------------------------------|
| `ParseError`       | invalid YAML, ryml could not build the tree                |
| `UnknownVersion`   | `format_version` differs from `"1"`                        |
| `MissingField`     | `format_version`, `nodes`, or `edges` is missing           |
| `MalformedSchema`  | fields are present, but types/indices don't match up (edge out of `nodes`, non-numeric index, edge not a pair) |

`message` — human-readable text, safe to print to stderr. `line` — the line number
in the file if ryml reported it, otherwise `0`.
