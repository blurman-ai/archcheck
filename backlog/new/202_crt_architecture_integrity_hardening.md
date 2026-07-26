# [CORE][GRAPH][CLI][CI] Architecture integrity hardening after the 2026-07-25 audit

**Created:** 2026-07-25
**Start date:** —
**Status:** new
**Module:** CORE / GRAPH / SCAN / RULES / REPORT / CLI / BUILD / CI
**Priority:** critical
**Difficulty:** XL (umbrella; mandatory child tasks and separate PRs)
**Blocks:** #163 (public_launch_plan)
**Blocked by:** —
**Related:** #002 (github_actions_ci), #015 (graph_diff_primitives), #030 (baseline_file_flag),
#075 (mvp_v1_trusted_diff_workflow), #086 (drift2_cycle_default_gate),
#117/#118 (diff_max_added_lines), #124 (corpus_validate_new_clone_gate),
#129 (unify_source_scan_one_pass),
#133 (first_run_noise_floor), #141 (centralize_gate_policy),
#142 (prebuilt_archcheck_binary_for_ci), #154 (file_classification_config_consolidation),
#168 (unresolved_local_include_gate), #169 (openwiki_agent_context_layer),
#170 (history_commit_order_contract), #171 (gate_decision_single_source),
#199 (window_free_clone_detection), #201 (window_free_dedup_recall_and_perf)

## Goal

Restore one trustworthy, fail-closed architecture-analysis pipeline in which every
public command validates the same project inputs, the core graph gate cannot be
skipped by an advisory path, graph drift recognizes architectural identity
correctly, and CI/release fitness checks exercise the architecture that is
actually shipped.

## Context

The 2026-07-25 architecture audit found several independent false-green paths:
valid findings can disappear after graph identity changes, bulk mode can bypass
the core graph gate, and some CLI modes can report success after invalid or
partial input processing. It also found boundary erosion between scanning,
classification, graph construction, rule evaluation, gate policy, reporting,
and release verification.

The debt must be fixed as a coordinated integrity programme because local
patches at individual report or command sites would preserve conflicting models
of the same analysis run. The detailed findings, proposed remediation, required
tests, sequencing, and completion criteria are captured below.

## Why this is one umbrella task

The audit findings are individually fixable, but the critical failures share one
cause: the product has several partially independent ways to construct an
analysis run.

- `check`, `--drift-baseline`, `--diff`, and preview commands do not share one
  validated command/context boundary.
- graph-gating data is owned by an advisory result in `--diff`;
- scan scope, include resolution, graph membership, rule findings, gate policy,
  and reporting use overlapping but different models;
- CMake's single `archcheck_core` target cannot enforce the intended dependency
  direction.

The one outcome owned by #202 is therefore **architecture integrity of the
end-to-end CI gate**, not a miscellaneous cleanup sweep.

This file is **not authorization for one mega-refactor or one mega-PR**. Before
implementation, each mandatory slice below must become a related child task when
it cannot fit the repository limits from
[docs/code_quality.md](../../docs/code_quality.md) or cannot be reviewed and
reverted independently. The umbrella closes only when all mandatory slices (or
their child tasks) satisfy the shared acceptance matrix.

Already-owned independent tails stay in their existing tasks:

- history ordering remains #170;
- window-free clone architecture/performance remains #199/#201.

They are recorded here for audit completeness, but their implementation must not
be duplicated in #202.

## Sources of truth and audit snapshot

Authority order for decisions in this task:

1. [CHANGELOG.md](../../CHANGELOG.md) for shipped behavior;
2. [docs/architecture-spec.md](../../docs/architecture-spec.md) for the product
   and rule contracts;
3. [docs/MVP.md](../../docs/MVP.md);
4. [README.md](../../README.md);
5. code and executable tests for the current implementation.

Audit snapshot:

- date: 2026-07-25;
- audited HEAD: `4374618`;
- full Debug suite: 666/666 tests passed;
- lizard thresholds: clean;
- actual dogfood graph:
  - `src`: 58 nodes, 8 edges, 184 unresolved;
  - `include`: 66 nodes, 82 edges, 0 unresolved;
  - `tests`: 2 nodes, 1 edge, 0 unresolved.

Green tests are not evidence against the findings below: the critical
counterexamples are absent from the current fixture matrix, and several tests
pin the problematic behavior itself.

## Architectural invariants to restore

These are acceptance invariants, not implementation suggestions:

1. **No false green on an unexecuted gate.** `gate: ok` means the documented core
   gate ran successfully.
2. **Fail closed on invalid analysis input.** A missing root, malformed config,
   invalid ref, invalid option, or unreadable baseline is exit `2`, never an empty
   clean project.
3. **Advisory work cannot control gate execution.** Skipping clone/complexity
   analysis cannot skip cycles or new god-headers.
4. **One project has one explicit analysis context.** Results do not depend on
   process-global state, command order, or whether disk/memory adapters were
   constructed first.
5. **Existence, ownership, and applicability are different facts.** A file
   excluded from one signal still exists for include resolution.
6. **Graph facts remain auditable.** A gating cycle retains the exact
   `file:line` evidence of the `#include` edges that created it.
7. **Architectural identity survives a rename when rename evidence is
   available.** In Git diff, a pure move is not a new cycle or god-header;
   replacing a member of a cycle is not an unchanged cycle. A standalone graph
   baseline has no rename evidence and must not pretend otherwise.
8. **The dependency direction is enforceable by the build.** `scan`, `graph`,
   `rules`, `report`, and Git adapters do not form a physical SCC hidden inside
   one target.
9. **One rule/finding identity drives policy and output.** A typo or unknown
   gating ID cannot silently demote a finding to advisory.
10. **Artifacts are verified before publication.** A failed smoke/test job cannot
    occur only after a GitHub Release or `latest` image is public.

## Confirmed debt register

| ID | Severity | Confirmed defect | Observable consequence | Ownership |
|----|----------|------------------|------------------------|-----------|
| GATE-1 | critical | SCC matching uses maximum path overlap and compares only cardinality | baseline cycle `{a,b}` → current cycle `{a,c}` is not reported by DRIFT.2; drift can exit 0 | mandatory |
| GATE-2 | critical | `DiffAdvisories` owns snapshots required by the graph; `diff_max_added_lines` returns before they exist | a bulk diff that creates a cycle deliberately exits 0 | mandatory, decision checkpoint |
| CLI-1 | critical | root/config/argument validation is distributed across command branches | nonexistent root can produce `gate: ok`; graph baseline can be saved with 0 nodes | mandatory |
| CLI-2 | high | docs-only fast path runs before config discovery and bypasses the Markdown reporter | malformed `.archcheck.yml` passes; `--format=md` emits text | mandatory |
| GRAPH-1 | high | `IncludeDirective::line` is discarded when an edge enters `DependencyGraph` | SF.9/DRIFT.2 emit line 0 and cannot provide the promised include witness | mandatory |
| GRAPH-2 | high | god-header identity is path-only; rename suppression covers cycles only | pure rename of an unchanged god-header can gate as new | mandatory with GATE-1 |
| SCOPE-1 | high | `bool authored` conflates existence, ownership, and signal applicability; filtering happens before resolution | an existing authored→excluded include becomes `UNRESOLVED_LOCAL_INCLUDE` | mandatory |
| SCOPE-2 | high | test-name heuristics apply to all signals | production `test_co_evolution.{h,cpp}` disappears from the graph | mandatory |
| CONTEXT-1 | high | classification extras and self-project identity are mutable process globals | result depends on command order/backend; parallel analysis is unsafe | mandatory |
| BOUNDARY-1 | high | `scan ↔ git`, `scan ↔ rules`, and `rules → graph → scan → rules` cycles are hidden by `archcheck_core` | intended pipeline cannot be enforced or independently compiled | mandatory |
| RULE-1 | high | findings have parallel extension paths; `IRule::id()` is not authoritative; gate policy compares free strings | adding a rule requires edits in factory/CLI/gate/report and a typo fails open to advisory | mandatory |
| DRIFT-1 | high | implementation flags any new old-node edge, while the spec requires a pre-existing path of length ≥2 | an ordinary new dependency can produce a false-red gating DRIFT.1 | mandatory |
| FITNESS-1 | high | dogfood scans `src`, `include`, and `tests` as disconnected/filtered graphs | cross-root cycles and almost all test architecture are invisible | mandatory |
| RELEASE-1 | high | release/image publication precedes tests on published artifacts | a workflow can fail after broken assets or `latest` are public | mandatory |
| BASELINE-1 | medium | violation-baseline loader parses the writer's whitespace layout, not JSON | valid minified/reordered JSON silently loads as an empty baseline | mandatory |
| AREA-1 | medium | drift rules and `--diff` use different zero-config area classifiers | the same path can belong to different modules in different modes | mandatory decision; implementation may be a child |
| PERF-1 | medium | each drift rule owns a full copy of the baseline graph | memory/startup cost grows with `rules × (V+E)` | opportunistic inside boundary work |
| TEST-1 | medium | several shipped rule fixture trees are not executed by tests | changing a fixture cannot fail CI; DTO-only tests miss seams | mandatory fitness work |
| HISTORY-1 | medium | history producer returns newest-first while the detector reads it as oldest-first | “recent consecutive growth” starts at the oldest commit | owned by #170 |
| DUP-1 | medium | window-free and classic clone strategies share a God-options/result DTO with mode-invalid fields | experimental result semantics and layering are coupled | owned by #199/#201 |

## Evidence and current reproductions

### GATE-1 — equal-cardinality SCC replacement is missed

Current implementation:

- [`best_baseline_match`](../../src/graph/diff.cpp#L70) accepts any non-zero
  member overlap;
- [`grownSccs`](../../src/graph/diff.cpp#L144) reports only when
  `baselineSize < currentSize`;
- [`DriftNoShortcutEdge`](../../src/rules/drift_no_shortcut_edge.cpp#L18)
  ignores a new edge if either endpoint is new;
- [`gate_policy.cpp`](../../src/rules/gate_policy.cpp#L18) makes intrinsic SF.9
  advisory in drift mode.

Minimal counterexample:

```text
baseline: a.h -> b.h -> a.h
current:  a.h -> c.h -> a.h
```

The current SCC overlaps the baseline SCC by `a.h`, both have size 2, and neither
new edge qualifies for DRIFT.1. No gating finding remains.

The formal product contract says DRIFT.2 catches a **new SCC**, or an existing SCC
that grew in nodes/edges:
[architecture-spec.md](../../docs/architecture-spec.md#L392).

### GATE-2 — advisory bulk skip disables the gate

Current flow:

```text
collect advisories
  -> compute total added lines
  -> bulk? return without SourceSnapshot
  -> graph builder has no snapshots
  -> skip graph and return gate ok
```

Evidence:

- [`DiffAdvisories`](../../src/cli/diff_command.cpp#L193) owns both snapshots;
- [`collectDiffAdvisories`](../../src/cli/diff_command.cpp#L237) returns early;
- [`runDiffFullPath`](../../src/cli/diff_command.cpp#L459) leaves the graph empty;
- the E2E test
  [`bulk import skips graph gating`](../../tests/integration/diff/diff_workflow_e2e_test.cpp#L248)
  pins “cycle + 10,100 lines → exit 0”.

There is a decision conflict that must not be erased:

- `backlog/DEBT.md` records an earlier explicit #117/#124 decision that bulk
  imports do **not** graph-gate;
- current public config documentation says
  [`diff_max_added_lines`](../../docs/config_format.md#thresholds) skips only the
  local-complexity advisory;
- README/CI docs describe cycles/new god-headers as the core gate without a bulk
  exception.

Task #202 reopens that decision because the old implementation also inverted ownership:
an advisory result controls whether the core mechanism exists.

### CLI-1/CLI-2 — parallel command paths fail open

Reproduced with the real binary:

```text
archcheck --format json /tmp/does-not-exist
=> {"gate":"ok", ...}, exit 0

archcheck --save-graph-baseline out.yml /tmp/does-not-exist
=> saves a 0-node baseline, exit 0

# Commit changes only malformed .archcheck.yml:
archcheck --diff --format=md HEAD~1..HEAD .
=> plain text fast-path report, config not loaded, exit 0
```

Root validation currently exists only in selected branches of
[`main.cpp`](../../src/main.cpp#L210); the use-case boundary in
[`check_command.cpp`](../../src/cli/check_command.cpp#L255) accepts an empty file
listing as a clean project. The fast path in
[`diff_command.cpp`](../../src/cli/diff_command.cpp#L53) runs before full-path
config/report setup.

### SCOPE-1/SCOPE-2 — excluded is treated as nonexistent

`SourceSnapshot` correctly retains all files plus `authored`, but
[`buildGraphForSnapshot`](../../src/graph/graph_builder.cpp#L49) and
[`scanLocalIncludeIssues`](../../src/scan/local_include_scan.cpp#L120) build their
resolver indexes only from authored files.

Current self-reproducer:

```text
archcheck .
src/cli/diff_command.cpp:28: [UNRESOLVED_LOCAL_INCLUDE]
quoted local include "archcheck/scan/test_co_evolution.h" does not resolve
```

The target exists at
`include/archcheck/scan/test_co_evolution.h`; its `test_*` stem makes the shared
classifier remove it from every analysis view.

### GRAPH-1 — source evidence is discarded

[`IncludeDirective`](../../include/archcheck/scan/include_directive.h#L15) has a
line, and `ResolvedInclude` preserves the directive. The graph builder reduces it
to `addEdge(from, to, conditional)` and
[`DependencyGraph`](../../include/archcheck/graph/dependency_graph.h#L16) stores
only adjacency plus a conditional bit. SF.9 consequently emits line 0:
[`sf9_no_cycles.cpp`](../../src/rules/sf9_no_cycles.cpp#L139).

This contradicts the public `file:line` contract and the FAQ claim that a gating
cycle comes with an exact include chain.

## Proposed target data flow

The names below are working names, not a mandate to add abstractions. Prefer
extending an existing entity when it already owns the responsibility.

```text
argv
  -> parse one Command request
  -> validate command + root + refs + output + config
  -> build explicit AnalysisContext
  -> read one full SourceSnapshot (resolution universe)
  -> select explicit analysis view(s)
  -> scan/resolve -> graph with edge evidence
  -> intrinsic rules + optional advisories (independent siblings)
  -> central gate decision
  -> selected reporter
  -> exit code
```

Required ownership properties:

- config is loaded/validated before any fast-path success;
- the full project universe exists independently of which files a rule analyzes;
- graph construction is a core stage, not a field of an advisory DTO;
- gate status distinguishes `pass`, `fail`, and any deliberately `skipped` state;
- reporters consume one completed result instead of reconstructing policy;
- disk and Git sources provide bytes/listing but do not mutate process policy;
- Git rename knowledge is applied at the graph-diff identity boundary.

## Execution plan — mandatory slices

### Slice 0 — freeze every critical counterexample before refactoring

Add failing tests first. Each test must fail on the unmodified implementation for
the reason stated in this task.

#### Graph/drift fixtures

- [ ] Baseline cycle `{a,b}` → current `{a,c}` of equal cardinality is a new
      gating cycle in both `--drift-baseline` and `--diff`.
- [ ] A current SCC that is an exact match is unchanged.
- [ ] A current SCC that is a strict subset of one baseline SCC is a shrink, not
      a regression.
- [ ] Two pre-existing SCCs merged by a new edge are a gating growth.
- [ ] A newly introduced self-loop either gates as required by the current spec,
      or its exclusion is an explicit product-contract decision.
- [ ] A pure rename of every cycle member stays non-gating.
- [ ] A rename mixed with a real new cyclic member still gates.
- [ ] A pure rename of an existing god-header stays non-gating.
- [ ] A genuine threshold crossing still reports a new god-header.
- [ ] If edge-count growth remains in the DRIFT.2 contract, same-membership SCC
      with new internal edges has an explicit pinned result.

#### CLI/gate fixtures

- [ ] Bulk diff plus a newly closed cycle still exercises the selected core-gate
      policy; it must never be reported as ordinary `gate: ok` without stating
      that the gate was skipped.
- [ ] Nonexistent root returns exit `2` in plain check, `--format`,
      `--config`, baseline save/load, graph-baseline save, and preview modes.
- [ ] A failed graph-baseline save creates no output file and does not truncate
      an existing destination.
- [ ] Unknown options, missing option values, invalid/negative integers, extra
      positional arguments, and unsupported option combinations return exit `2`.
- [ ] Config-only malformed diff returns exit `2`, including the docs-only fast
      path.
- [ ] Docs-only `--format=md` uses the Markdown reporter contract.

#### Scope/provenance fixtures

- [ ] An authored file including an existing `third_party/` target is not
      unresolved, while excluded vendor internals remain outside authored-rule
      analysis.
- [ ] Production files named `test_co_evolution.*` remain production inputs.
- [ ] Test files can be intentionally included in a dogfood analysis view
      without changing the zero-config external default.
- [ ] Disk and memory sources classify a repository named `fmt` identically.
- [ ] Constructing/using two project sources in either order produces identical
      per-project results.
- [ ] A cycle finding contains a non-zero source line for every witness edge.
- [ ] Duplicate includes of the same target have deterministic evidence.

### Slice 1 — correct graph-diff and gate semantics

#### 1A. Replace overlap/cardinality SCC identity

Recommended semantic model:

1. Canonicalize current paths through an explicit Git rename-pair map where a
   diff has rename evidence (`newPath -> oldPath` for comparison).
2. Decompose baseline and current graphs into SCCs.
3. A current non-trivial SCC is pre-existing only when all of its canonical
   members belonged to one baseline SCC.
4. An exact set is unchanged.
5. A strict subset is shrink/improvement.
6. Any current SCC whose canonical members were not all mutually strongly
   connected in one baseline SCC is new/grown and gates.
7. If the authoritative DRIFT.2 contract retains edge growth, classify added
   internal SCC edges separately rather than inferring it from node count.
8. Treat a one-node SCC with a self-edge according to the final explicit
   self-loop contract; do not let the generic `size < 2` filter decide it
   accidentally.

This directly handles:

- `{a,b} → {a,c}` as new;
- `{a,b,c} → {a,b}` as shrink;
- `{a,b}` renamed to `{x,y}` as unchanged after canonicalization;
- SCC merges as growth.

The diff result should carry a reason such as `New`, `MembershipGrowth`,
`EdgeGrowth`, or `Unchanged/Shrunk` (exact representation is an implementation
choice). Do not keep overloading `baselineSize/currentSize` as the entire
identity model.

#### 1B. Make all gating graph metrics rename-aware

Use one old→new identity map for:

- SCC comparison;
- god-header threshold crossings;
- any path-keyed future gate.

The existing `collectRenamedPaths` returns only new-side names and is sufficient
only for all-or-nothing cycle suppression. A stable identity comparison needs
the old/new pair, parsed from a NUL-delimited Git result so valid path characters
cannot corrupt it. A rename must not suppress a real structural change made in
the renamed file.

Once identity is normalized before comparison, the post-hoc
`dropRenameArtifactCycles` path must be removed or reduced to a compatibility
reporting layer. It must not remain a second, competing identity algorithm.

Compatibility boundary: Git-backed `--diff` has rename evidence.
`--drift-baseline` currently stores paths/topology only, so a pure rename remains
a path change unless a separately versioned identity contract is introduced.
Do not silently enlarge the graph-baseline schema in this slice.

#### 1C. Align DRIFT.1 with its authority

Recommended choice: implement the architecture-spec definition.

- Consider only a new edge `u -> v` whose endpoints existed in baseline.
- Search the baseline graph for a path `u => v` of length at least 2.
- Emit DRIFT.1 only if that path existed.
- Pin deterministic witness/path output.

For the expected PR-sized set of added edges, a bounded BFS/DFS per candidate is
acceptable until measurement proves otherwise. Do not add a generic transitive
closure engine without evidence.

Alternative: deliberately redefine DRIFT.1 as “any dependency between old
nodes”, rename the rule, update the spec/CHANGELOG, and revalidate its gate-grade
precision. This is not the recommended option because it discards the
“shortcut” attribution and currently false-reds ordinary dependencies.

#### 1D. Resolve the bulk-gate decision explicitly

Recommended choice for a CI-first invariant tool:

- `diff_max_added_lines` skips only advisory analysis named by its contract;
- preprocessor graph construction and cycle/new-god-header gate always run;
- a large clean diff can be `gate: ok`;
- a large diff with a new cycle is `gate: fail`.
- core snapshots are owned by diff orchestration, not by an advisory result;
- each advisory has its own explicit budget and skip reason;
- skipping complexity does not suppress later clone/flag/bool output through an
  early return in `printDiffAdvisories`.

If the previous #117/#124 product decision is intentionally retained instead,
the architecture must still change:

- bulk skipping belongs to an explicit gate policy, not `DiffAdvisories`;
- output must say `gate: skipped`, not `gate: ok`;
- text, JSON, and Markdown schemas must expose the reason;
- README, CI docs, config docs, CHANGELOG, and exit semantics must state the
  exception;
- an advisory threshold must not accidentally own snapshots/core execution.

This checkpoint must be decided and recorded before implementation. Silent
preservation of the current contradictory behavior is not acceptable.

### Slice 2 — make CLI composition fail closed

Use one parsed command model, or an equivalently centralized validation boundary,
without adding a CLI dependency unless the standard-library implementation proves
unmaintainable.

Required behavior:

- [ ] Parse the command once; do not infer the whole mode only from scattered
      `argv[1]` branches.
- [ ] Validate arity and reject unknown/extra arguments for every command.
- [ ] Parse integers with checked conversion (`from_chars` or equivalent), not
      `atoi`.
- [ ] Validate root existence/type before config discovery or scanning.
- [ ] Validate refs and input baseline files before creating output.
- [ ] On any validation failure, leave an existing output file untouched.
- [ ] Keep use-case functions defensive: an empty listing caused by an invalid
      root must not be indistinguishable from a valid empty project.
- [ ] Load/validate config before the docs-only/no-C++ fast path.
- [ ] Route empty fast-path results through the same Text/JSON/Markdown
      reporters as full results.
- [ ] Represent option compatibility centrally so `--config`, `--format`,
      baseline/drift flags, and unresolved-local policy do not acquire
      order-dependent grammars.
- [ ] Preserve the exit-code contract: 0 clean, 1 gated finding, 2 user/config
      error, 3 internal error.
- [ ] Error paths write a complete diagnostic to stderr and never emit a partial
      success JSON document.

Do not turn this slice into a feature-rich CLI framework migration. The objective
is one grammar and one validation boundary.

### Slice 3 — separate file universe, classification, and analysis views

#### 3A. Resolve before analysis filtering

Keep one full `SourceSnapshot` of candidate project files. Build the include
resolver index from that full universe, then apply signal-specific selection.

At minimum, the model must distinguish:

- file exists in the project/repository;
- file is authored vs vendored/generated;
- file is test/support code;
- file participates in a particular analysis view.

Expected semantics:

- authored→vendor include resolves as existing;
- excluded vendor internals do not generate authored architectural debt;
- an excluded target is not counted as an unresolved local include;
- test exclusion for clone/noise control does not erase production files whose
  names merely start with `test_`;
- dogfood can select tests explicitly.

A single replacement enum/bitset on `SnapshotFile`, or narrowly named selection
helpers over existing classification, is preferable to several new hierarchy
objects. Do not build a generic policy engine.

#### 3B. Remove ambient classification state

Replace:

- `classificationExtrasStorage()`;
- `selfProjectDirStorage()`;
- constructor side effects in `DiskFileSource`/`GitObjectFileSource`;

with an explicit immutable classification input owned by the analysis run.

The CLI config-to-scan translation may remain at the composition boundary, but
preview, check, diff-memory, and diff-disk must receive the same value. A custom
`FileSource` must be sufficient to reproduce a result without relying on which
other source object was constructed first.

#### 3C. Preserve include-edge evidence

Extend the existing graph rather than creating a second evidence graph.

The graph must retain deterministic evidence for each resolved project edge:

- source file;
- source line;
- conditional/unconditional status;
- enough information to print an exact cycle witness.

For multiple directives creating the same `(from,to)` edge:

- any unconditional directive makes the architectural edge unconditional;
- choose a deterministic representative line (recommended: lowest
  unconditional line, otherwise lowest line), or retain all locations if the
  measured memory cost is acceptable;
- never let unordered insertion order select the reported location.

Graph baselines can remain a raw path/edge topology if drift does not require
historical source lines. Do not bump the baseline format merely to persist data
that only the current witness needs. If the schema does change, version it and
keep backward-compatible reading.

### Slice 4 — enforce subsystem boundaries and one finding identity

#### 4A. Break the physical dependency SCC

Target dependency direction:

```text
config/value models
        |
scan core / source ports  <- Git/disk adapters
        |
      graph
        |
      rules
        |
      report

CLI/application composition depends on all of the above.
```

Concrete boundary work to evaluate:

- move the neutral finding DTO out of `rules` so scan detectors do not import
  `rules::ViolationList`;
- do not include `rules/rule_set.h` merely to name a finding list;
- move Git change/history DTOs to a neutral boundary, or move detectors that
  intrinsically combine Git history with scans out of the low-level scan core;
- keep `FileSource` as the port and Git/disk implementations as adapters;
- keep the deliberate `include_scanner` / future `clang_scanner` split;
- do not add a plugin system or generic rule engine.

Create CMake targets that make the intended DAG observable. The exact number of
targets is secondary; acceptance is that a forbidden reverse include/link
dependency is no longer possible merely because everything is in
`archcheck_core`. A temporary `archcheck_core` interface/facade may preserve test
link compatibility while the physical targets become independent. Dependencies
such as `ryml` stay private to the targets that actually parse config/baselines.

#### 4B. Make rule/finding identity authoritative

Choose one source for each stable rule ID.

Acceptable minimal directions:

- remove unused `IRule::id()` and make emitted finding descriptors authoritative;
  or
- make the rule descriptor authoritative and explicitly model graded child IDs
  such as `DRIFT.4.CYCLE/.SDP/.NEW`.

In either case:

- the gate policy consumes registered identities, not unrelated string literals;
- an unknown identity is diagnosable and cannot silently demote a supposed
  gating rule;
- one neutral finding type is shared by rules and scanner-based checks;
- one command-level collector assembles findings before gate/report;
- adding a normal default rule still follows one class/one file plus one
  registration point.

Do not force `RegressionReport` and every advisory into `IRule` merely for visual
uniformity. The accepted typed check/drift versus diff gate split from #171 may
remain, provided both paths share validated inputs and cannot bypass the central
gate contract.

#### 4C. Decide zero-config area identity once

`rules::areaOf` and `diff::AreaClassifier` currently implement different
concepts. Choose one of:

1. one shared zero-config area policy used by both modes; or
2. explicitly different, differently named concepts with separate public
   contracts.

Do not leave two functions both described as “area/module” while classifying the
same layout differently. Pin common `src/<module>` plus
`include/<project>/<module>` layouts and root-level files. The same slice must
remove duplicate defaults such as a separately initialized
`godHeaderFanIn = 50`; callers pass the value from the loaded config contract.

#### 4D. Stop copying the baseline graph per drift rule

As part of the rule-boundary work, share one immutable baseline graph between
drift checks, or pass it through an immutable check context. Do not add caching
or ownership machinery beyond what is required to eliminate the current full
copies.

### Slice 5 — repair persistence and executable fitness functions

#### 5A. Parse the violation baseline as its declared format

Use a standards-compliant parser already available to the project where
practical; do not add a heavy dependency for this small schema.

Required schema behavior:

- require and validate `version`;
- require `violations` with the expected type;
- accept insignificant whitespace, minified input, reordered object keys, and
  valid JSON escaping;
- reject malformed entries with a precise `BaselineError`;
- preserve compatibility with baselines written by all shipped v0.1 versions;
- reject unknown incompatible versions rather than treating them as empty;
- round-trip writer output through the independent reader.

Tests must include minified, pretty, reordered, empty, escaped-control,
malformed, missing-field, and unknown-version cases.

#### 5B. Make rule fixtures executable contracts

Wire shipped SF.7/SF.8/SF.9/Lakos and applicable advisory fixture trees into
tests that execute the real seam, not only inline strings or manually assembled
DTOs.

Every shipped rule must have:

- at least one pass fixture;
- at least one failing fixture;
- a test that proves those directories are actually enumerated;
- command/gate assertions where the rule is public CLI behavior.

#### 5C. Replace vacuous dogfood with an architectural fitness test

The fitness test must analyze the logical production project, not disconnected
subtrees:

- assert at least one known `src → include` edge is resolved;
- assert representative test files are present when the dogfood test view asks
  for tests;
- assert no forbidden subsystem cycle exists after Slice 4;
- retain ordinary zero-config behavior for external projects.

The implementation may use a purpose-built integration `FileSource`/snapshot
view if the public CLI cannot yet express the union without broadening its
contract. Do not keep advertising `archcheck tests` as dogfood while it analyzes
only two nodes.

#### 5D. Verify release artifacts before publication

Required workflow order:

```text
build artifacts
  -> test + dogfood + unpack/checksum/smoke each artifact
  -> smoke Docker image locally
  -> publish GitHub Release
  -> push version image
  -> move/update latest only after all verification succeeds
```

The release workflow must not rely solely on the assumption that a tag points to
a previously green branch commit. Publication jobs explicitly depend on all
required verification jobs. The existing old-glibc/static-binary verification
remains pre-publication, and the bytes/checksum verified are the bytes/checksum
later published—there is no rebuild between verification and promotion.

### Slice 6 — synchronize contracts and close the audit

- [ ] Record every user-visible correction in `[Unreleased]`.
- [ ] Update `docs/architecture-spec.md` for final DRIFT.1/DRIFT.2 semantics.
- [ ] Reconcile the bulk-gate decision across config docs, README, CI docs,
      CHANGELOG, help, and schemas.
- [ ] Update affected openwiki pages in the same PR as their code.
- [ ] Remove/supersede corresponding open `backlog/DEBT.md` entries; preserve the
      historical #117/#124 decision rather than rewriting history.
- [ ] Link every child task/commit back to #202.
- [ ] On completion, document how the final pipeline works, what controls it,
      related components, and diagnostics.

## Shared acceptance matrix

| Scenario | Expected result |
|----------|-----------------|
| Equal-size cycle membership replacement `{a,b} → {a,c}` | gating finding, exit 1 |
| Unchanged or strictly shrinking pre-existing SCC | no new-cycle gate |
| SCC merge or real new cyclic member | gating finding, exit 1 |
| New self-loop | pinned gate result matching the final documented DRIFT.2 contract |
| Pure cycle rename | no gate; suppression/identity is auditable |
| Pure god-header rename with unchanged fan-in | no gate |
| Genuine god-header threshold crossing | gating finding, exit 1 |
| New old→old edge with no prior path | no DRIFT.1 shortcut |
| New old→old edge replacing a baseline path of length ≥2 | DRIFT.1 gate |
| Bulk diff with a new cycle, recommended policy | graph runs, exit 1 |
| Advisory skipped for volume, no structural regression | gate reflects graph result; advisory skip is explicit |
| Missing/non-directory root in every mode | diagnostic + exit 2; no output baseline |
| Invalid save command with an existing destination | destination is unchanged |
| Malformed config in docs-only diff | diagnostic + exit 2 |
| Docs-only Markdown diff | valid Markdown report, not text fallback |
| Existing include target excluded from authored analysis | resolves as existing; no unresolved-local finding |
| Production file whose stem begins `test_` | classified according to explicit project role, not erased globally |
| Same project via memory and disk adapters | identical file universe and graph report |
| Two analyses in either construction order | identical isolated results |
| SF.9/DRIFT.2 witness | concrete non-zero `file:line` edge evidence |
| Valid minified/reordered violation baseline | loads and suppresses the same entries as writer output |
| Unknown/malformed violation baseline version/schema | diagnostic + exit 2 |
| Self-dogfood production view | contains known cross-root edges; tests view is non-vacuous |
| Release verification failure | no GitHub Release, version image, or `latest` publication |

Global gates before any child/umbrella completion:

- [ ] Debug build succeeds.
- [ ] Full `ctest --output-on-failure` succeeds.
- [ ] Mandatory regression fixtures above execute in CI.
- [ ] `lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/ tests/`
      has no new warning.
- [ ] Text/JSON/Markdown schemas are deterministic.
- [ ] Disk/memory diff parity tests pass.
- [ ] No source dependency cycle remains between the CMake subsystem targets.
- [ ] No unrelated WIP/refactor is folded into the same change.

## Recommended child-task / PR order

1. **Evidence-only PR:** Slice 0 failing/characterization tests, split by seam if
   necessary.
2. **Graph identity PR:** GATE-1, GRAPH-2, DRIFT-1.
3. **Gate ownership PR:** GATE-2 and the recorded bulk-policy decision.
4. **CLI boundary PR:** CLI-1/CLI-2.
5. **Scope/context PRs:** SCOPE-1/SCOPE-2 and CONTEXT-1.
6. **Graph evidence PR:** GRAPH-1.
7. **Physical-boundary PRs:** BOUNDARY-1, RULE-1, PERF-1.
8. **Persistence/fitness PRs:** BASELINE-1, TEST-1, meaningful dogfood.
9. **Release workflow PR:** RELEASE-1.
10. **Contract closeout PR:** docs/openwiki/CHANGELOG/DEBT reconciliation.

The first four items are release-critical. Do not begin the package-boundary
refactor while the known false-green gate paths are still open.

## Risks and rejected shortcuts

### Risk: a correctness fix becomes an unreviewable architecture rewrite

Mitigation: tests first, child tasks, narrow commits, and no package-boundary work
inside a gate-semantic fix.

### Risk: rename normalization hides a real change

Mitigation: canonicalize identity only; still compare edges, SCC membership,
fan-in, and thresholds after the rename.

### Risk: resolving against excluded files starts reporting vendor debt

Mitigation: resolution universe and analysis selection are separate. Existence
does not imply that vendor nodes produce findings.

### Risk: provenance makes graph/baseline memory or schema unnecessarily heavy

Mitigation: measure representative evidence versus all locations; keep runtime
evidence out of persistent graph baselines unless a demonstrated consumer needs
it.

### Rejected shortcuts

- Do not “fix” a false green by demoting or hiding the affected gate.
- Do not keep the bulk exception hidden behind an advisory field.
- Do not report `gate: ok` when the core gate did not run.
- Do not solve globals with another singleton/thread-local registry.
- Do not scan vendor/test/generated code for every rule merely to make includes
  resolve.
- Do not add a generic rule engine, plugin system, AST work, dashboard, or IDE
  integration.
- Do not collapse the deliberate include-scanner/clang-scanner boundary.
- Do not add a heavy JSON/graph/CLI dependency without measured necessity.
- Do not silently change a baseline schema; version all breaking persistence
  changes.
- Do not fold #170 or #199/#201 implementation into this umbrella.

## Diagnostics during implementation

Primary commands:

```bash
cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/ tests/
```

Architecture-specific checks:

```bash
./build/debug/src/archcheck --graph src
./build/debug/src/archcheck --graph include
./build/debug/src/archcheck --graph tests
./build/debug/src/archcheck .
```

During each slice, record:

- exact command and exit code;
- whether the gate ran, failed, or was explicitly skipped;
- node/edge/unresolved counts for both diff backends;
- concrete `file:line` witnesses for cycles;
- child task and commit that owns the change.

## Key decisions

| Decision | Status / reason |
|----------|-----------------|
| #202 is an umbrella with one outcome, not one mega-PR | fixed; required by backlog/code-quality constraints |
| Critical false-green fixtures land before refactoring | fixed |
| SCC identity is structural membership after rename canonicalization, not maximum overlap + size | recommended target |
| DRIFT.1 follows the documented pre-existing-path definition | recommended target |
| Core graph gate runs independently of advisory thresholds | recommended; explicitly reopens #117/#124 |
| Invalid roots/config/options fail with exit 2 at one command boundary | fixed by the public exit-code contract |
| Full resolution universe is separate from signal applicability | fixed architectural invariant |
| Classification context is immutable and explicit | fixed architectural invariant |
| Edge provenance is retained in the current graph; baseline schema changes only if proven necessary | recommended target |
| Rule IDs have one authoritative owner | fixed invariant; exact representation decided in child task |
| Existing two scanner backends remain separate | fixed scope boundary |
| History and window-free clone tails stay in #170 and #199/#201 | fixed ownership |
| Release is verify-before-publish | fixed invariant |

## Changed files

Planned ownership only; exact files belong in child tasks.

| Area | Expected change |
|------|-----------------|
| `src/graph/`, `include/archcheck/graph/` | SCC identity, rename-aware comparison, edge evidence |
| `src/cli/` | one validated command flow; independent gate/advisory orchestration |
| `src/scan/`, `include/archcheck/scan/` | explicit classification context and resolution universe |
| `src/rules/`, `include/archcheck/rules/` | authoritative rule identity, DRIFT.1 semantics, shared baseline ownership |
| `src/report/`, `include/archcheck/report/` | real baseline parsing and completed-result reporting |
| `src/CMakeLists.txt`, `tests/CMakeLists.txt` | enforceable subsystem DAG and executable fixtures |
| `tests/unit/`, `tests/integration/`, `fixtures/` | audit counterexamples and end-to-end contracts |
| `.github/workflows/` | meaningful dogfood and verify-before-publish release graph |
| `README.md`, `CHANGELOG.md`, `docs/`, `backlog/DEBT.md` | final public contract and diagnostics |

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Confirm the bulk-gate policy checkpoint, explicitly acknowledging the prior
   #117/#124 decision.
2. Create child tasks for Slice 0, graph identity, gate ownership, and CLI
   fail-closed behavior.
3. Land the failing regression matrix before changing production code.
