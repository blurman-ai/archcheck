---
title: "openwiki — agent entry map"
type: index
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "CHANGELOG.md"
  - "README.md"
  - "src/rules/rule_set.cpp"
  - "docs/GLOSSARY.md"
related_pages:
  - "source-map.md"
  - "cli-contract.md"
  - "schema.md"
external_sources:
  - "https://www.langchain.com/blog/introducing-openwiki-an-open-source-agent-for-repo-documentation"
  - "https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f"
---

# openwiki — agent entry map

## Purpose

Fast, source-backed navigation for coding agents: "I want to change X — which files,
tests, and fixtures do I read first?" This layer is **derived documentation**. When it
disagrees with code, tests, or CHANGELOG, **the code wins** — and you fix the page.

Format and evidence rules live in [schema.md](schema.md). Health is checked by
`scripts/check_openwiki.py` (`/openwiki-check`).

## Coverage

Covered by dedicated pages: the **default rules**, the **drift rules**, the **scan
/ PR-hygiene signals**, **duplication**, the **CLI contract**, and a whole-repo
**source map**. Still thin / see the code + `docs/` for detail: YAML config-rule
**enforcement** (deferred to v0.2 — see [ADR-001](../decisions/001-config-rules-deferred-to-v0.2.md)).
Open items are tracked in [backlog/open-questions.md](backlog/open-questions.md).

## Where to look

| I want to… | Start here |
|---|---|
| Change / add a **rule** | [source-map.md](source-map.md) → "Rule → files"; then the rule's page under `rules/` |
| Know a rule's exact message / threshold / gate status | its `rules/<rule>.md` page |
| Change a **CLI flag, exit code, or output format** | [cli-contract.md](cli-contract.md) (`Risk: high`) |
| Understand **gate vs advisory** (and the `--diff` escalation) | [rules/gate_policy.md](rules/gate_policy.md) |
| Touch **graph metrics** (cycles, CCD/ACD/NCCD, chain depth) | [source-map.md](source-map.md) → graph row; `src/graph/algorithms.cpp` |
| Work on **duplication / clones** | [features/duplication.md](features/duplication.md) → `docs/duplication_architecture.md` (SSOT) |
| Change **config / `.archcheck.yml`** | `docs/config_format.md`; `src/config/`, `include/archcheck/config/config.h` |
| Understand **what actually shipped** | [CHANGELOG.md](../../CHANGELOG.md) (authoritative) |
| Learn a **project term** | [docs/GLOSSARY.md](../GLOSSARY.md) |

## Pages

**Core**: [source-map.md](source-map.md) · [cli-contract.md](cli-contract.md) · [schema.md](schema.md) · [log.md](log.md)

**Default rules** (`makeDefaultRuleSet`):
[SF.7](rules/sf7_using_namespace.md) ·
[SF.8](rules/sf8_include_guard.md) ·
[SF.9](rules/sf9_no_cycles.md) ·
[Lakos god-headers](rules/lakos_god_headers.md) ·
[Lakos chain-length](rules/lakos_chain_length.md)

**Drift rules** (`makeDriftRuleSet`) + gate policy:
[shortcut-edge](rules/drift_no_shortcut_edge.md) ·
[bidirectional-coupling](rules/drift_bidirectional_coupling.md) ·
[lateral-module-dependency](rules/drift_lateral_module_dependency.md) ·
[no-cycle-growth](rules/drift_no_cycle_growth.md) ·
[gate_policy](rules/gate_policy.md)

**Scan / PR-hygiene signals & features**:
[SATD](features/satd.md) ·
[test co-evolution](features/test_co_evolution.md) ·
[bool-field drift](features/bool_field_drift.md) ·
[flag-argument drift](features/flag_argument_drift.md) ·
[new-clone drift](features/new_clone_drift.md) ·
[local complexity](features/local_complexity.md) ·
[history analytics](features/history_analytics.md) ·
[local-include gate](features/local_include_gate.md) ·
[duplication](features/duplication.md)

**Backlog / sources**:
[open-questions](backlog/open-questions.md) ·
[unverified](backlog/unverified.md) ·
[external-openwiki-practices](sources/external-openwiki-practices.md)

## Canonical docs (authoritative — openwiki only links, never restates)

- [CHANGELOG.md](../../CHANGELOG.md) — what shipped (rules, thresholds, modes).
- [docs/GLOSSARY.md](../GLOSSARY.md) — project vocabulary.
- [docs/architecture-spec.md](../architecture-spec.md) · [docs/MVP.md](../MVP.md) · [docs/ROADMAP.md](../ROADMAP.md).
- [docs/decisions/](../decisions/) — ADRs (config→v0.2, SF.21→v0.2, fast backend default).
- [docs/config_format.md](../config_format.md) · [docs/ci_usage.md](../ci_usage.md) · [docs/baseline_format.md](../baseline_format.md) · [docs/duplication_architecture.md](../duplication_architecture.md).

## Related pages

- [[source-map.md]] · [[cli-contract.md]] · [[schema.md]]
