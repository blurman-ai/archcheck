# archcheck — glossary

A single reference of project terms: thesis, drift signals, rules, tool
architecture, metrics, corpus methodology. Purpose — a shared language for the docs, the article,
and the tasks. Authority on specifics: [CHANGELOG.md](../CHANGELOG.md) (what shipped),
[docs/architecture-spec.md](architecture-spec.md) (design), `docs/research/` (research).

---

## A. Thesis and basic concepts

- **Constraint decay** — the gradual erosion of architectural constraints as the
  codebase grows: a contract is set, but each subsequent commit weakens it a little, instead of
  refactoring. Primary source — Dente et al. (EURECOM 2026): LLM agents lose ~30 pp
  of quality as structural constraints grow harder (L0→L3). The backbone of archcheck's motivation.
  See [docs/research/constraint_decay.md](research/constraint_decay.md).
- **Drift** — the observable trace of constraint decay in git history: added
  shortcut edges, grown cycles, copy-paste, accretion of boolean flags, lateral links
  between modules. Measured per-commit (delta) or per-struct/history (accretion).
- **Agentic / non-agentic** — a cut of the corpus: repos/commits with signs of AI-assistance
  (AI trailers, bot authorship, long messages) versus the rest. The "agentic vs
  non-agentic" contrast is the main way to check whether AI amplifies drift.
- **Boolean accretion** — the addition of boolean fields to a structure over
  time. **A neutral phenomenon, not a verdict**: can be legitimate (independent properties)
  or a smell. The metric `n_bool_field_added` (#135) measures exactly the increase.
- **Boolean blindness** — a particular smell case: several `bool`s encode mutually exclusive
  or correlated states, and an invalid combination is representable in the type (calls for
  `enum`/sum type). The reference example — `LambdaExpressionNode` with `constexpr ⊥ consteval`.
  See [docs/research/boolean_state_accretion_good_vs_smell.md](research/boolean_state_accretion_good_vs_smell.md).
- **Dogfooding** — archcheck must pass its own rules: it's run on its own
  `src/`/`include/`/`tests/`. Any merge that breaks its own rules is inadmissible.

## B. Layers and drift signals

The signals are complementary — they catch different classes, you can't fold them into a single counter.

- **Graph drift** — per-commit changes to the include graph: added/removed edges,
  grown cycles (SCC), new cross-area dependencies. A rare and objective signal.
- **Copypaste / clone layers** — complementary levels of duplication detection: line (#053),
  token (#056, Type-3 with normalization), AST (#052), precision (#059), usage (#054).
  Single source — [docs/duplication_architecture.md](duplication_architecture.md).
- **Boolean-state per-commit** — how many boolean fields a commit added (delta). A quantum for
  correlations; in #135 refined to the net increase into pre-existing structures.
- **Boolean-state per-struct / history** — structures that accreted boolean fields across ≥4
  different commits (axis = structure, not commit). A measure of accumulated decay (#134, #089).
- **Lateral module drift** — the appearance of the first lateral dependency between peer modules
  (rule DRIFT.4).
- **Complexity drift** — growth of a function's cognitive complexity between baseline and the current
  version (rule DRIFT.LOCAL_COMPLEXITY, token-based, no AST).
- **SATD** (Self-Admitted Technical Debt) — added `TODO`/`FIXME`/`HACK` in the diff
  lines (rules SATD.1/SATD.2).

## C. archcheck rules

Each rule = one class = one file in `src/rules/`; registration — the factory in
`rule_set.cpp`. Attribution to a source (Core Guidelines / Lakos / Martin) is mandatory.

- **SF.7** — no `using namespace` in the global scope of a header (Core Guidelines).
- **SF.8** — header self-sufficiency / include guard.
- **SF.9** — no cycles among headers. The only thing that gates in the ordinary check mode.
- **Lakos.GodHeader** — fan-in threshold (default 50): a header included by too
  many = a hub-god.
- **Lakos.ChainLength** — an overly long include chain.
- **DRIFT.1** — a new shortcut edge between files, both already present in baseline (#009). Gates.
- **DRIFT.2** — a new or grown cycle against baseline (#009). Gates. Rare (~0.05% of commits).
- **DRIFT.3** — a new bidirectional module link (area A→B and B→A through different files) that
  wasn't in baseline; not caught by a file cycle. Advisory (#087).
- **DRIFT.4** — the first lateral dependency between peer modules (#118). Graduated:
  - **DRIFT.4.CYCLE** — creates a module cycle with a baseline back-edge; **gates** in drift mode
    (precision ~92% on the corpus).
  - **DRIFT.4.SDP** — violates Martin's Stable Dependencies Principle (I(B) > I(A)+0.10). Advisory.
  - **DRIFT.4.NEW** — just the first lateral pair. Advisory.
- **DRIFT.NEW_CLONE** — copy-paste introduced by a commit (surfacing in `--diff`). Advisory, doesn't gate.
- **DRIFT.LOCAL_COMPLEXITY** — a function that crossed the cognitive complexity threshold. Advisory.
- **SATD.1 / SATD.2** — added self-admitted technical debt. Advisory.
- **TEST (co-evolution)** — prod code changed, tests silent. Advisory.
- **ARG.1** — flag-argument drift: growth in the number of boolean function arguments (#093, scan-level). Advisory.
- **DRIFT.BOOL_FIELD_ACCRETION** — net increase of depth-0 bool fields in a structure that existed in baseline (#090/#135). Net-count (rename/reformat → 0); greenfield doesn't count. Advisory; filters vendored/test/generated via `authored`.
- **Gate vs advisory** — a **gate** (exit 1) is given only by regressions: DRIFT.1, DRIFT.2, DRIFT.4.CYCLE
  in drift mode; SF.9-cycle in the ordinary one. Everything else is **advisory** (reported, exit 0), so that
  legacy debt doesn't fail the first run. See `--baseline`.

## D. Tool architecture

- **Pipeline** — `scan → graph → rules → reporter`. Subsystems: `scan/`, `graph/`, `rules/`,
  `report/`, plus `diff/`, `git/`, `scan/duplication/`.
- **Fast (preprocessor) backend** — the v0.1 default: a fast scan of `#include` without compilation and without
  `compile_commands.json`. Most default rules are include-only — they don't pay the libclang cost.
- **libclang backend** — the `--with-clang` option (fully fledged in v0.2) for semantic rules.
- **Two-backend split** — a deliberate separation: spike #043 showed libclang ×1000+ slower
  on a number of projects → fast stays the default. ADR-003.
- **Zero-config** — running with no arguments gives a useful result. The YAML config is parsed and
  validated, but enforcement of module rules (`layers`/`independence`/`forbidden`) is deferred
  to v0.2 (ADR-001).
- **Baseline** — a snapshot of the current violations/graph, against which regression is measured:
  `--baseline`, `--save-baseline`, `--save-graph-baseline`, `--drift-baseline`. Lets
  legacy projects adopt the tool without rewriting ("not 5000 violations on the first run").
- **Diff mode** — `archcheck --diff <revspec>`: materializes two git states and reports
  structural regressions between them. `--format=json` with a stable schema; `gate: ok|fail`.
- **Exit codes (contract)** — `0` ok · `1` violations · `2` config/parsing error · `3`
  internal error. Change only with versioning.

## E. Metrics

- **CCD / ACD / NCCD** — Cumulative / Average / Normalized Cumulative Component Dependency
  (Lakos): the total connectivity of the component graph and its normalizations.
- **Fan-in** — how many components include a given header; high fan-in → god-header.
- **Chain length** — the length of an include chain.
- **Cognitive complexity** — a function's cognitive complexity (Campbell 2018), computed token-based
  without AST; the basis of DRIFT.LOCAL_COMPLEXITY.
- **Instability I (Martin)** — I = Ce/(Ca+Ce); the basis of SDP (you should depend on something more stable).
- **SDP** (Stable Dependencies Principle) — a dependency should point to a more stable
  module; a violation is the basis of DRIFT.4.SDP.

## F. Corpus and methodology

- **Corpus** — ~1686 local C++ git repos on NVMe (`~/oss_corpus/oss/`); per-commit table
  `experiments/per_commit/results_full.jsonl` (~340k records), worklist `worklist_full.tsv`.
- **Corpus criteria gate** — the repo selection gate (`experiments/CORPUS_CRITERIA.md`): the main one —
  >300 commits since May 2025. Check it before every clone.
- **Agentic gate** — criteria for a repo's "AI density" (AI trailers, bot authorship, message length).
- **Parentless / shallow boundary** — a commit whose `sha^` is unresolvable (root or the boundary
  of a shallow clone); the diff degrades to "the whole file is new" → such commits are skipped.
- **Repo fixed effects** — controlling for the repository: removes between-repo differences, leaving
  the within-repo signal. Many "agentic effects" die under it (see lateral-drift #115).
- **Simpson (Simpson's paradox)** — an aggregate trend that vanishes/reverses within
  groups. The main trap of corpus conclusions; a reason to verify case by case, not by the aggregate.
- **Size confound** — "a big commit → a lot of everything": a spurious correlation from diff size.
  Cured by partial correlation / binning by `added_total`, not by bare Pearson.
- **Eye-check** — a manual case-by-case verification of N findings (input→expected output) by an independent
  method; mandatory before the conclusion "works correctly".
- **TP / FP** — true / false positive. FP classes for bool-drift (#135): greenfield (a new
  file/structure), rename/typo-fix/replace (the bool count doesn't grow), nested depth>0 fields.
- **Velocity (as a signal)** — commit speed as a proxy for agency: **transient and doesn't
  survive repo fixed effects** → not a population AI signal. A classic reversal.

## G. Metric `n_bool_field_added` (#135)

- **n_bool_field_added** — a per-commit quantum: the net increase in the number of depth-0 bool fields in
  structures that existed in the parent (`sha^`). `Σ max(0, count_after − count_before)`.
- **Net-count** — we count the increase in the NUMBER of bools, not the added `+bool` lines.
  Removes the FP from rename/replace/reformat (in those the count doesn't change → 0).
- **Greenfield** — a bool field in a new structure/file (not in `sha^`); NOT drift, not counted.
- **Sidecar** — a separate Python pass (`experiments/boolean_state/bool_field_added_per_commit.py`),
  writes its own JSONL keyed by `(repo, sha)` and is joined into the corpus table; doesn't touch the binary.
