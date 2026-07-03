# Backlog review — 2026-07-03

Focus of this pass (per request): **duplicate task numbers.** Full classification below.

Totals: active = 37 (new 16 · wip 7 · future 14); completed = 100+; pending = 1 (not reviewed).

---

## ⚠️ Number collisions

`008a–008h` are **legitimate sub-numbering** of task 008 (include scanner), not a collision — excluded.

| # | Files | State | Verdict |
|---|-------|-------|---------|
| ~~167~~ | ~~`new/167_maj_unresolved_local_include_gate-3.md`~~ vs `completed/167_maj_private_journal_split.md` | **RESOLVED** | Renumbered to **168** and closed → `completed/168_maj_unresolved_local_include_gate.md` (implemented in 0e796e5). No collision remains: 167 = private_journal_split, 168 = local_include_gate. |
| **072** | `wip/072_maj_port_056_duplication_into_archcheck.md` vs `completed/072_min_clone_type_classifier.md` | **wip × completed** | Active task shares a number with a done one. Renumber the wip task (next free → 169) or accept as historical. |
| **071** | `future/071_min_clone_detection_opportunities.md` vs `completed/071_fp_classification_rules.md` | **future × completed** | Deferred task collides with a done one. Low urgency (future/), renumber if/when promoted. |
| 117 | `completed/117_min_diff_max_added_lines.md` + `completed/117_min_lateral_cycle_backedge_confirm.md` | completed × completed | Historical. No action — renumbering completed tasks breaks git/commit references. |
| 118 | `completed/118_maj_drift4_lateral_rule.md` + `completed/118_min_diff_max_added_lines.md` | completed × completed | **Already resolved & self-documented**: 118_min is marked "duplicate — implemented under #117 due to an ID clash". Leave as-is. |
| 119 | `completed/119_maj_unified_per_commit_drift_correlation.md` + `completed/119_min_lateral_filesplit_fp.md` | completed × completed | Historical. No action. |
| 136 | `completed/136_crt_cli_docs_contract_after_advisory_wave.md` + `completed/136_maj_struct_fields_literal_brace_depth_bug.md` | completed × completed | Historical. No action. |
| 048 | `completed/048_maj_drift_clean_checkout_methodology.md` + `completed/048_maj_fixture_libresprite_pr581.md` | completed × completed | Historical. No action. |

**Root cause:** parallel agents pick the next number off `completed/` sightlines and collide when two land the same day (117/118/119, 136, 048 are all same-day pairs). 167 has since been renumbered to 168 and closed. Remaining active collisions: **072** (wip) and **071** (future) — renumber if/when they move; low urgency.

---

## Quick wins
| File | Goal | Module |
|------|------|--------|
| new/144_min_diff_unresolved_baseline_exit2 | diff: unresolved-include baseline → exit 2 | CLI/DIFF |
| future/050_min_sf21_anonymous_namespace | SF.21 anon-namespace rule | RULES |
| future/110_min_getter_exposure_drift | getter-exposure drift metric | RULES |

## Medium tasks
| File | Goal | Module | Difficulty |
|------|------|--------|-----------|
| new/057_maj_lakos_fanout_coupling_checks | Lakos fan-out coupling checks | GRAPH/RULES | medium |
| new/077_maj_per_commit_graph_drift_export | per-commit graph drift export | GRAPH/REPORT | medium |
| new/094_maj_param_count_and_accretion | param-count accretion metric | RULES | medium |
| new/095_maj_config_bag_growth | config-bag growth metric | RULES | medium |
| new/125_maj_scan_extensionless_headers | scan extensionless headers | SCAN | medium |
| new/126_maj_sf9_collapse_impl_into_component | SF.9 collapse impl into component | RULES | medium |
| new/152_maj_diff_scope_clone_scan_changed_files | scope clone scan to changed files | DIFF/SCAN | medium |
| new/153_maj_vendor_exclusion_fixtures_and_tails | vendor-exclusion fixtures + tails | FIXTURES/SCAN | medium |
| new/164_maj_drift_scanner_scope_parser_fixpack | drift scanner scope/parser fixpack | SCAN | medium |
| new/165 / 166 | Windows x64 / macOS arm64 release binaries | BUILD | medium |
| wip/093_maj_flag_argument | flag-argument rule (in progress) | RULES | medium |
| wip/129_maj_unify_source_scan_one_pass | unify source scan to one pass | SCAN | medium |

## Hard / blocked
| File | Note |
|------|------|
| new/163_maj_public_launch_plan | umbrella launch plan (Phase 3 announce remains, per git) — hard, gates 165/166 |
| wip/072_maj_port_056_duplication_into_archcheck | port #056 duplication into archcheck — hard; also number-collides (above) |
| wip/054_maj_ai_repo_duplication_run | corpus duplication run — hard, long wall-clock |
| future/042_maj_clang_semantic_backend | libclang backend — v0.2, hard |
| future/052_maj_cross_tu_duplication_detector | cross-TU AST duplication — v0.3+, hard |
| future/005_maj_sarif_reporter_spec | SARIF reporter — deferred |

## Needs research / thin analysis
| File | What's missing |
|------|----------------|
| new/074_maj_ai_repo_discovery_roi_alignment | research-shaped, no concrete acceptance criteria |
| new/078_maj_clone_cochange_harm_signal | signal design, needs corpus validation |
| future/v1_* (4 files) | AI-config synthesis track — protocol/eval still open |

## Duplicates / related (semantic, beyond numbers)
| Files | Proposal |
|-------|----------|
| wip/072 ↔ completed/056 | 072 is explicitly "port 056"; keep, but renumber to clear the 072 clash |
| new/152 ↔ new/153 ↔ wip/129 | all touch scan scope/one-pass — check for overlap before starting |
| new/165 ↔ 166 ↔ 163 | 163 is the launch umbrella; 165/166 are its release-binary children — link `Related:` |

---

**Note:** `completed/071_fp_classification_rules.md` still carries `Status: wip` in its header though it lives in `completed/` — stale header, folder is authoritative. Cosmetic.
