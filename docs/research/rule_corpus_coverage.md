# Rule → corpus witness coverage

_2026-07-02._ For every check archcheck actually ships, does a real (not synthetic)
corpus witness exist — a concrete `repo@commit:file:line` where the rule fired and a
human confirmed it's a true positive? This doc is the answer, one row per rule. Sources
are existing research docs / showcase entries; nothing here is a new measurement.

Rule IDs and gating status are read from the factory
([src/rules/rule_set.cpp](../../src/rules/rule_set.cpp)) and `archcheck --help` — treat
those as ground truth if this table ever drifts.

## Default rules (`makeDefaultRuleSet`)

| Rule | Mode | Corpus witness | Source |
|---|---|---|---|
| **SF.9** no-cycles | gate | 10+ repos: bpftrace `ast.h↔clone.h`, openvdb `LeafNode↔LeafNodeBool`, AlchemyViewer, Boxedwine64, Doom3, BALL, matiec, hnswlib, Angband, SparkEngine | [EXAMPLES_10_GRAPH_10_CLONES.md](../../experiments/EXAMPLES_10_GRAPH_10_CLONES.md), showcase [005](../../experiments/showcase/005_cycle_jana2_jstreamlog_jlogger.md)/[006](../../experiments/showcase/006_cycle_specfempp_bot_blindspot.md) |
| **SF.7** using-namespace-in-header | advisory | mosh ×4, mumble (`using namespace std`), openvdb ×8, pcl ×2, newsboat, supercollider (QtCollider) ×2 | [goodfirstissue_dogfood/SUMMARY.md](../../experiments/goodfirstissue_dogfood/SUMMARY.md) |
| **SF.8** include-guard | advisory | mumble ×9 (selectivity 9/376 hand-verified), mosh ×1 | same |
| **Lakos.GodHeader** | advisory | SparkEngine `Platform.h` fan-in 439, djeada `component.h` 201, mumble `Global.h` 87 | same; note showcase entry 003 (facebook/wangle) was **pulled** — real fan-in growth, but a logging shim isn't a compelling "bad code" demo |
| **Lakos.ChainLength** | advisory | SparkEngine `SparkEngine.cpp` depth 13 (full 14-file chain hand-traced and re-verified live 2026-07-02, curated as showcase [009](../../experiments/showcase/009_chainlength_sparkengine_depth13.md)); also 58 files >10 in the same repo, bpftrace 46. (Earlier UNIGINE `Unigine.h` depth-33 figure retracted — that repo turned out to be the user's private `gm/` game project, not an OSS corpus entry, so it doesn't belong in a public witness table.) | [EXAMPLES_10_GRAPH_10_CLONES.md](../../experiments/EXAMPLES_10_GRAPH_10_CLONES.md) |

## Drift rules (`makeDriftRuleSet`, require `--drift-baseline`)

| Rule | Mode | Corpus witness | Source |
|---|---|---|---|
| **DRIFT.1** shortcut edge to god-header | gate | **LibreSprite PR #581** (Claude Opus 4.5 commit series): `app/ui/toolbar.cpp -> app/pref/preferences.h` (fan-in ~74, one `bool` accessor). Confirmed introduced by AI commit `0aa57ad`, not inherited from upstream Aseprite (3 independent `git show`/`git log` checks, re-verified live 2026-07-02). Locked into a hermetic regression fixture; curated as showcase [007](../../experiments/showcase/007_shortcut_libresprite_toolbar_preferences.md). | [drift_signal_validation.md](drift_signal_validation.md) §2, [ai_drift_cases.md](ai_drift_cases.md), fixture `fixtures/drift_real_world/libresprite_pr581/` |
| **DRIFT.2** new/grown cycle | gate | 72/135,092 commits (0.05%) corpus-wide; named examples: "Refactor CodeGen to classical header/impl split", "Fix build: add ESP8266WebServer.h guard" (+7 cycle members); showcase JANA2 (4-line commit closed a cycle, author self-admitted "quick-and-dirty"), SPECFEMPP (AI-reviewer-approved PR grew a 7-node cycle, bot blind spot) | [drift_signal_validation.md](drift_signal_validation.md) §1, showcase [005](../../experiments/showcase/005_cycle_jana2_jstreamlog_jlogger.md)/[006](../../experiments/showcase/006_cycle_specfempp_bot_blindspot.md) |
| **DRIFT.3** bidirectional module coupling (aggregate, non-cycle) | advisory | Live, re-run 2026-07-02 via isolated `git worktree`: `danielraffel/pulp@705f86e` — a brand-new `inspect/` module is born already mutually coupled with `core` (`inspect→core` via view/canvas headers, `core→inspect` via `standalone.cpp`), confirmed absent before the commit. Curated as showcase [008](../../experiments/showcase/008_bidirectional_pulp_core_inspect.md). Eyeball of 56 corpus commits: NeoCalculator `hal↔ui/apps/math/display/input`, gizmosql `common↔duckdb`/`common↔sqlite`, Standard-of-Iron `game↔render`, bakabakaband `terrain↔world`, mantra-lang `core↔semantic`, MaximumTrainer `model/persistence↔ui` | [drift_signal_validation.md](drift_signal_validation.md) §3 + "DRIFT.3 — manual review of hits" |
| **DRIFT.4** lateral module dependency (`.CYCLE` gate / `.SDP`, `.NEW` advisory) | mixed | Validated on 479 repos: 153 CYCLE, 58 SDP events; eyeball TP 85% (CYCLE-only 92% after back-edge confirmation fix, #117). Named repos: PantheonChain (genuine same-commit cycle), Astraeus (leaf-utility FP, caught and downgraded), Aztec ×3, GBAStation, ThemisDB, UE5 | [lateral_module_drift_corpus_run.md](lateral_module_drift_corpus_run.md) §8.3 |

## Advisory subsystems (not `IRule`)

| Check | Corpus witness | Source |
|---|---|---|
| Duplication (EXACT/RENAMED/STRUCTURAL clones) | showcase [001](../../experiments/showcase/001_renamed_smatchet_resolveoption.md) (Smatchet, renamed copy-paste), [002](../../experiments/showcase/002_structural_duckdb_webbed_unified_xml_html.md) (duckdb_webbed, up to 82 lines copied); 10 more STRUCTURAL witnesses (Doom3 Lexer/Parser, platform net backends, etc.) | [EXAMPLES_10_GRAPH_10_CLONES.md](../../experiments/EXAMPLES_10_GRAPH_10_CLONES.md) part B |
| Complexity drift | showcase [004](../../experiments/showcase/004_complexity_azure_sdk_c_manifest_parser.md) (Azure SDK for C, manifest parser: 94→106 vs threshold 25) | showcase |

## Bottom line

Every shipped check has at least one hand-verified, real (non-synthetic) corpus
occurrence — no rule ships on faith alone. Depth varies: SF.9/SF.7/DRIFT.2/DRIFT.4 have
double-digit-to-hundreds of witnesses with measured TP rates; DRIFT.1 and DRIFT.3 each
rest on one deeply-verified live case plus a hand-eyeballed sample, because both are
rare/narrow by design (see [drift_signal_validation.md](drift_signal_validation.md) for
why raw cross-area coupling was deliberately *not* shipped as a gate — ~50% false-alarm
rate on the same corpus).

Where a rule's corpus reality turned out to be different from the pitch (Lakos.GodHeader
mostly hitting legitimate cross-cutting headers rather than "bad code"), that's recorded
honestly above and in the showcase README rather than hidden.
