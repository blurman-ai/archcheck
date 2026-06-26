# Check coverage matrix

_Which check lives where and in which mode it runs. The source of truth is the code
(`src/rules/rule_set.cpp`, `src/cli/{check,diff,preview}_commands.cpp`,
`src/scan/*`) + CHANGELOG. Update when adding a rule/mode._

Modes:
- **check** — `archcheck <path>` (whole-repo, intrinsic rules; gates on SF.9).
- **drift** — `archcheck --drift-baseline` (whole-repo against a saved baseline).
- **diff** — `archcheck --diff <revspec>` (per-commit; what corpus run #124 ran).
- **history** — `archcheck --history <path>` (history analytics, advisory).
- **duplication** — `archcheck --duplication <path>` (clone snapshot, preview).

## Implemented

| Check | Rule/ID | check | drift | diff (#124) | history | note |
|---|---|:--:|:--:|:--:|:--:|---|
| No include cycles | SF.9 / `Sf9NoCycles` | ✅ gate | ✅ | ✅ `grown_cycles` | — | the only gating signal |
| using namespace in header | SF.7 / `Sf7UsingNamespace` | ✅ adv | — | ❌ intrinsic | — | not a delta → not in diff |
| header self-sufficiency | SF.8 / `Sf8IncludeGuard` | ✅ adv | — | ❌ intrinsic | — | |
| God-header (fan-in) | Lakos.GodHeader | ✅ | ✅ | ✅ `new_god_headers` | — | |
| Include chain length | Lakos.ChainLength | ✅ adv | — | ✅ `chain_length_grown` | — | |
| Shortcut edge | DRIFT.1 | — | ✅ gate | ✅ `added_edges` | — | |
| Cycle growth | DRIFT.2 | — | ✅ gate | ✅ `grown_cycles` | — | |
| Bidir module coupling | DRIFT.3 | — | ✅ adv | ✅ `new_cross_area` | — | |
| Lateral module dep | DRIFT.4 | — | ✅ | ✅ (cross-area) | — | |
| Function cognitive complexity | DRIFT.LOCAL_COMPLEXITY | — | — | ✅ `complexity` 17.2% | — | diff-only (needs a delta) |
| Introduced copy-paste | DRIFT.NEW_CLONE | — | — | ✅ `newclone` 9.8% | — | snapshot — in `--duplication` |
| Self-admitted tech debt | SATD.1/2 | — | — | ✅ `satd` 4.5% | — | diff-only (added lines) |
| Tests don't co-evolve | TEST.1 | — | — | ✅ `test` 22.6% | — | diff-only (numstat) |
| NCCD metric (cohesion) | — | — | — | ✅ `nccd_delta` 9.2% | — | metric delta |
| File LOC growth | SIZE.1 / god_file_growth | — | — | ❌ | ✅ | **history only** |
| Defect magnet (fix-churn) | HIST.1 / defect_attractor | — | — | ❌ | ✅ | **history only** |

## Blind spots of corpus run #124 (`--diff`)
`--diff` does NOT run: **SF.7, SF.8** (intrinsic, not a delta) and **SIZE.1, HIST.1**
(history only). That is, per-commit validation does not cover them; they need a
separate whole-repo / history run over the corpus.

## Documented/planned, but NOT implemented (and why)

| Wishlist item | Status | Reason |
|---|---|---|
| **SF.21** (anonymous namespace in header) + SF.2/5/10/11 | v0.2 | needs a semantic (libclang) backend; text scan → FP by default (ADR-002) |
| **bool fields in structs** | v0.2 | parsing members = semantics (libclang); token backend can't handle it (ADR-003) |
| **Config module rules** (`layers`/`independence`/`forbidden`) | v0.2 | parsed+validated, but enforcement deferred for the sake of zero-config adoption (ADR-001) |
| **#093 flag_argument** (growth of bool params) | post-release | NOT an ADR — deferred by priority; token-feasible, can be pulled in |

Full rule roadmap: [docs/architecture-spec.md](../architecture-spec.md) §Roadmap.
