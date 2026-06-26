# Grand summary: code audit · AI-drift hypothesis · external research · cheap guards · clang backend

_2026-06-11. Method: a multi-agent run (5 code auditors, rule inventory, 2 hypothesis
reviewers, 5 web researchers) + adversarial verification of key claims against
primary sources. Claims from the internet are marked: ✅ — the verifier opened the primary source and
confirmed it; ◻ — a source was found by a researcher, but verification was not run (session
limits); numbers in ✅-claims are reconciled against the source text._

---

## TL;DR — answers to six questions

1. **The code is broadly healthy** — the scan→graph→rules→report layering has not a single back edge,
   the two-backend split didn't leak, the project's own code is dogfood-clean. But: **2 high security
   findings** (abort on malformed YAML instead of exit 2; stack overflow on a deep include chain
   in a default rule), **our own lizard gate is currently red** (5 functions in src/ over thresholds),
   ~95 lines of dead LCS machinery in the shipped library, a placeholder `evaluateAgainstCorpus`
   with a permanent precision=1.0, and systematic fork/exec copy-paste in the git subsystem (~50 lines).
2. **The conclusion "the hypothesis is not confirmed" is unwarranted.** The warranted conclusion is: "the design cannot
   distinguish an effect smaller than ~3.5× on threshold metrics at a horizon <12 months with a contaminated
   control". The 2024–2026 literature says exactly the same: global metrics (duplication,
   graph) are silent in others' work too, the persistent signal lives in local complexity and
   micro-patterns — where our boolean-drift gave 1.6–2.3×. **No direct work on
   boolean/config-bag drift was found — this is, apparently, an original result.**
3. **Currently checked:** 5 default rules (SF.7/8/9 + GodHeader 50 + ChainLength 10),
   3 drift rules (DRIFT.1/2 — gate, DRIFT.3 — advisory), diff mode (6 regression types),
   duplication advisory. The claimed SF.21, compile_commands.json reading, SARIF, and exit 3 — absent.
4. **C++ pain confirmed by numbers:** #1 — dependency management (~82% of ISO 2024
   respondents call it a pain), #2 — build times (~80%); the pain is chronic (−2–3 pp over 3 years).
   Physics: headers = 99.68% of Chromium's compiled lines. Chrome and Figma built **homegrown**
   include-graph checkers in CI (Figma: blocks 50–100 regressions per day) — there is no ready CLI
   product on the market; C++20 modules at 4.1% library adoption won't close the niche.
5. **Cheap guards — two corpora of ideas:** ~15 on the include graph (private headers, entry-point,
   transitive contracts, SDP, include-weight budget…) and ~15 without the include graph (clones in new
   code, change coupling, flag-argument, config-bag…), 8 of which are already turned into
   backlog/new/093–100 tasks. The through-line rule: gate only on "delta against baseline on new
   code", everything probabilistic is advisory.
6. **Clang backend:** the only thing fundamentally unavailable to the include graph is the symbol
   level (which symbols are used from where). Top candidates: symbol-level layering, phantom
   edges, SF.2, the abstractness metric A for Martin's D. The price — 2–3 orders of magnitude more than an include scan
   (LLVM: directive-scan ~3 s vs full parse ~15 min) → strictly opt-in, via compile_commands.json,
   incrementally.

---

## 1. Codebase audit (~14k lines of src/include/tests)

### 1.1 What's good (confirmed, not a compliment)

- **Layering is observed flawlessly**: the full include-dependency map of src/ — not a single
  edge against the scan→graph→rules→report direction, not a single cycle between modules.
- **Two-backend split is clean**: libclang is absent from the tree, nothing leaked into the include-only path.
- **Dogfooding**: the binary builds, a run on the repo itself — the project's own code (src/include/tests)
  yields **0 violations**; all 1067 findings are gitignored sandbox/ (1057) and deliberate fixtures/ (10).
- **git is invoked via fork+execvp without a shell** — there is no classic shell injection.
- **README is almost entirely honest**: all flags exist, the example output reproduces
  byte for byte, thresholds and exit codes match.
- No orphan headers, all CLI flags are dispatched, no commented-out code, no
  premature abstractions found (IRule — 8 implementations, FileSource — 2).

### 1.2 Security (threat model: CI on an untrusted repo)

| # | Severity | Finding | Where |
|---|---|---|---|
| S1 | **high** | A malformed `.archcheck.yml` → ryml with the default handler calls **abort** → SIGABRT (134) instead of the contractual exit 2. In `baseline.cpp` the correct pattern (set_callbacks→exception→restore) already exists, in `config_loader.cpp` — not applied | `src/config/config_loader.cpp:341-358` |
| S2 | **high** | **Stack overflow on a deep include chain**: `computeSccDepths` is a recursive lambda, depth = chain length; the Lakos.ChainLength rule is in the default set → a malicious repo with a chain of tens of thousands of headers crashes an ordinary run | `src/graph/algorithms.cpp:213-230` |
| S3 | medium | The tree walk **follows file symlinks outside the root** (`is_regular_file` dereferences): `evil.h → /etc/passwd` is read and partly leaks into the report | `src/scan/project_files.cpp:39-67` |
| S4 | medium | File and git-blob reads **without a size limit** + **no top-level try/catch in main** → OOM / bad_alloc → std::terminate (134) instead of exit 3 | `src/git/git_object_file_source.cpp:227-268`, `main.cpp:614-619` |
| S5 | medium | `jsonEscape` escapes only `"` `\` `\n` — control characters U+0000..001F from file names/code produce **invalid JSON** (no breakout injection, but downstream consumers crash) | `include/archcheck/report/json_escape.h:11-27` |
| S6 | low | git executes in an untrusted repo **without hardening**: a local `.git/config` (`core.fsmonitor`, `diff.external`, `core.pager`) and hooks (`post-checkout` on worktree add) execute arbitrary commands | `src/git/git_state.cpp:310` |

Recommendations (per finding): S1 — reuse the pattern from baseline.cpp; S2 — iterative
DFS (the condensation is a DAG, a topological pass with memoization); S3 — `symlink_status` +
a `weakly_canonical` check inside root; S4 — an N MB limit + a top-level catch → exit 3;
S5 — `\uXXXX` for everything <0x20 + U+FFFD for invalid UTF-8; S6 — `GIT_CONFIG_NOSYSTEM=1`,
`-c core.hooksPath=/dev/null -c core.fsmonitor= -c diff.external= -c core.pager=cat`, `--`
before ref arguments.

Path traversal via include resolution and shell injection were **not found**.

### 1.3 Dead code (each candidate checked by grep)

The main points:

- **~95 lines of full DP-LCS are dead in the shipped library**: `diffTokens`/`DiffOp` +
  `buildLcsTable`/`backtrackLcs`/`collapseDelInsPairs` are called only by tests; production
  `cloneType` doesn't call them ([clone_classifier.cpp:10-106](../../src/scan/duplication/clone_classifier.cpp#L10-L106)).
- `reverseReachableFrom` and `hasPath` — public graph API that lives only in tests
  ([algorithms.h:15-16](../../include/archcheck/graph/algorithms.h#L15-L16)).
- **`evaluateAgainstCorpus` — a stub, always precision=1.0** ("assume all reported pairs are
  TP"), ground truth ignored — creates a false sense of precision control
  ([fp_corpus_eval.cpp:44-74](../../src/scan/duplication/fp_corpus_eval.cpp#L44-L74)).
- Write-only/never read: `MetricThresholds::chainLengthLimit`, `Pair::sharedRare`,
  `BaselineLoadError::line` (always 0), `Config::version`.
- Dead accessors: `ConfigError::file()/line()/column()`, `Worktree::valid()`,
  `DiskFileSource::root()`.
- `tests/duplication_vmecpp_test.cpp` and `duplication_all_projects_test.cpp` **are never
  compiled** (not included in CMakeLists).
- Deliberate groundwork (not junk, but recorded): `Config::modules/rules` are parsed without
  enforcement; `extraExcludes` of GodHeaders is unreachable from CLI/config.

### 1.4 Quality / compliance with our own standards

- **Our own lizard gate is red**: 5 functions in src/ (3 in duplication_scanner.cpp,
  main.cpp:117, +1) and 13 TEST_CASEs over the CCN15/len30 thresholds → exit 1. The project memo requires
  running lizard before a push — right now it would not pass.
- **Systematic copy-paste against our own ">5 lines" threshold**: ~50 lines of fork/exec
  between `git_state.cpp` and `git_object_file_source.cpp`; **three copies of `toLower`** despite a claimed
  single source of truth; a duplicate of violations JSON serialization + a brittle hand-written substring parser
  for the baseline; a duplicate of the vendor/test filter between graph_builder and project_files; `plainJaccard` —
  a 25-line copy of `weightedJaccard`.
- **Naming is split**: 8 files in snake_case against the required lowerCamelCase, and the central
  interface **`IRule` violates our own ban on the I-prefix** from code_style.md ("just use Rule").
- `duplication_scanner.cpp` is the most problematic file: typos in names (`Loclal`, `filer`),
  **two "phase8" functions**, dead branches, a scattering of unnamed magic thresholds,
  retelling comments (a forbidden pattern).
- No classes >300 lines; clang-format is clean.

### 1.5 Compliance with our own architecture

- **The "static table" registration is actually an imperative factory**: adding a rule requires
  editing `rule_set.cpp` (the OCP from the spec is violated in the strict sense; you don't need to touch
  others' rule files — this mitigates it).
- **NCCD is not computed per Lakos**: `ACD/log2(N+1)` instead of the canonical ratio to the ACD
  of a balanced binary tree — Lakos's calibrated thresholds are inapplicable to our NCCD
  ([algorithms.cpp:275](../../src/graph/algorithms.cpp#L275)).
- **ID collision DRIFT.3**: in the spec it's reserved for public_surface_growth, in the code — for
  bidirectional coupling. One of the two must be renamed before it gets out into the wild.
- CCD/ACD/NCCD are computed but **not output** in either check or `--graph` — only the NCCD delta
  in `--diff` (CLAUDE.md promises "in report").
- **There is no dogfood gate in CI** (only a smoke `--version/--help` + diff mode in the PR workflow),
  although CLAUDE.md calls it mandatory.

### 1.6 Docs vs reality (the main points)

- **CLAUDE.md contains false statements**: reading compile_commands.json (0 occurrences in the code),
  `file:line:column` (no columns), mandatory dogfooding in CI (none), SF.21 in the MVP set
  (no code, no fixtures), "CCD/ACD/NCCD in report" (not output).
- **MVP.md acceptance criteria are not met** (dependency rules not enforced, compile_commands
  not read), while README declares "MVP functionally complete" — a contradiction.
- CHANGELOG: #088/#091/vendored classification not written up; a link to a non-existent repository.
- Backlog: TASK_TRACKER is stale (P0 blockers stand at 8 days while working on research), #079 is finished
  but in wip/, duplicate IDs #071/#072, a zombie file `dup_band_70_80.md` at the root.
- architecture-spec.md has diverged on flags (`--graph-baseline` vs `--drift-baseline`),
  directory structure, and roadmap (DRIFT.1/2 shipped in v0.1, the spec says v0.3).

---

## 2. What archcheck checks right now (inventory)

The full version is in the inventory agent's output; here is a condensed table.

| Rule/mode | What it catches | Source | Threshold | Gate? | Fixtures |
|---|---|---|---|---|---|
| SF.7 | `using namespace` at the global scope of a header | Core Guidelines | — | yes | ✅ |
| SF.8 | no include guard (first 60 non-empty lines; excl.: .inc, ObjC) | Core Guidelines | — | yes | ✅ |
| SF.9 | include-graph cycles (Tarjan, SCC≥2; excl.: inline-split pair #088, all-conditional) | CG/Lakos | — | yes | ✅ |
| Lakos.GodHeader | fan-in > threshold (excl. PCH names) | Lakos | 50 | yes | ✅ |
| Lakos.ChainLength | include-chain depth > threshold | Lakos | 10 | yes | ✅ |
| DRIFT.1 | a new edge between two files that existed in the baseline ("shortcut") | own corpus | — | **gate** | ✅ + real-world |
| DRIFT.2 | a new/grown SCC against the baseline | Lakos | — | **gate** | ✅ |
| DRIFT.3 | new mutual area connectivity A↔B through different files (without a file cycle) | own corpus | — | advisory | ✅ |
| `--diff` | added/removed edges, grown cycles, new cross-area, chain growth, new god-headers (the threshold here is **30**, not 50!), NCCD delta | — | — | exit 1 on regression | integration |
| `--duplication` | token clone detector (normalization, fragmentation, rare-token + winnowing #092, 10 precision filters, EXACT/RENAMED/LITERAL/MIXED/STRUCTURAL classifier) | NiCad class | weighted ≥0.6 | **no, always exit 0** | ❌ no fixtures |
| `--baseline` | suppression by triplet (rule, file, line) | — | — | — | — |

**Claimed but NOT in the code**: SF.21; SARIF; clang_scanner; compile_commands.json;
module rules layers/independence/forbidden (parsed, not enforced — help honestly
warns); exit 3 (`return 3` is absent, there is no catch-all in main); Martin Ce/Ca/I/A/D;
the `rules/core_guidelines|lakos|martin/` grouping.

Threshold mismatch: god-header fan-in **50** in the rule, but **30** in diff mode
(`regression_report.h:52`) — two different defaults for one entity.

---

## 3. The hypothesis "AI increases architectural drift"

### 3.1 What was done and what it showed (reconstruction from docs/research + experiments)

Evolution: (1) detector phase — DRIFT.1/2 on 33 AI PRs; (2) population phase — agentic vs
control by cycles/copy-paste; (3) narrowing #089 — boolean-state; (4) cross-language velocity.

**Worked:**
- **DRIFT.1**: 7 of 33 AI PRs (21%), 12 findings, 4 archetypes (UI→widgets, generic→features,
  system→component, ui-config→core-data); 0 noise on 26 clean PRs, including +9762 LOC. LibreSprite
  #581 verified by a triple git check → hermetic fixture.
- **DRIFT.2 as a gate**: 72 of 135,092 corpus commits (0.05%) — rare, objective.
- **Boolean per-struct drift (the only positive population signal)**: AGENTIC
  9.8% of structures vs NON-AGENTIC 6.0% → **1.6×**; per repo 36% vs 17% → **2.1×**; the activity
  confound is removed (commit medians 1013 vs 940 = 1.08×); after an eye-check of both groups
  (FP share higher in non-agentic) the correction **strengthens** the ratio to **~2.3×** (5.6% vs 2.4%).
- Velocity: event-study 61 vs 31 — commits 1.61× vs 1.00×, lines/commit 190→306 with a flat
  control.

**Didn't work:** cycles (3% vs 6% of repos — 2 repos against 2), authored copy-paste (M-W p=0.144),
AI%↔debt (ρ≈±0.06), the static bool counter (78% config bags), the naming rule (0/1, revert),
file-level history drift (45% FP), velocity-ramp as a proxy for agentness (top-ramp repos AI%=0),
raw cross-area connectivity (~50% false-alarm).

### 3.2 Why "not confirmed" is the wrong conclusion (a methodological review)

The full discussion is a separate reviewer's material; the core:

1. **The control is contaminated by construction**: invisible AI (autocomplete, removed trailers) →
   non-differential misclassification → bias toward zero. With plausible 20% false
   agentic + 30% hidden AI in the control, a true ratio of 2.0 is observed as **1.38** —
   below the design's distinguishability threshold. Worse: in the boolean comparison the "non-agentic" group is taken from
   a corpus that passed the agentic gate (msglen≥100 — one of the OR criteria) — that the signal survived
   on such a diluted contrast is rather an argument **for** the hypothesis.
2. **Level (stock) metrics instead of rate (flow)**: a repo can triple its connectivity without
   crossing a single threshold. Our own case mako: cross-module edges ×3.5 over a year with
   "cycles with no trend" — the signal was in the derivative.
3. **Floor effect + power**: cycles p≈0.06, n=61 vs 31 → MDE ≈ ratio 3.5×; copy-paste →
   MDE ≈ 29 pp. "38% vs 32%" is uninformative — the design couldn't distinguish 32% from 55%. Not a single
   CI/p-value/power calculation is in the docs; boolean per-repo (36% vs 17%, z≈3.2) is nominally
   significant, but without a cluster correction and with labeling blindness = 0.
4. **Survivorship by construction**: the gate ">300 commits since May 2025" selects survivors; if
   drift kills projects — the casualties are excluded by the inclusion criterion; born-agentic (maximal
   dose) are thrown out.
5. **Time-to-effect**: erosion is a process of years (Lehman; Eick et al.); adoption is almost universal at
   H2 2025+, the "after" window is 90 days. Even mature human code (newton-dynamics) holds
   invariants flat for 1.5 years. For threshold metrics the horizon is 2–4 years; for derivatives —
   6–12 months (already available).
6. **A precedent of a "plausible zero"**: grown_cycles was zero corpus-wide due to three pipeline
   bugs — every new null conclusion requires a positive control (injection of a known drift).

**12 alternative designs** (reviewer, in decreasing value/cost order):
1. ⭐ **Commit-level attribution within mixed repos** — AI vs human commits of **the same repo**
   (repo fixed effects kill all confounders at once; 135k per-commit records already collected,
   a join with attribution is needed; ~1 week).
2. PR-level case-control: the same 10 repos, matched human PRs (helper ready; 150–200 PR per arm).
3. Within-repo interrupted time series on the **rate of growth** of connectivity (a slope break at the
   adoption date, mixed-effects).
4. Staggered diff-in-diff (Callaway–Sant'Anna) by adoption dates.
5. Survival analysis of violations ("AI violations are fixed less often/later").
6. A family of micro-accretion metrics modeled on #089 (function parameters, branches, header
   size, config keys; 3–5 days per metric).
7. Abstraction-bypass rate — a generalization of DRIFT.1 without a threshold (the share of new includes into
   impl-headers of another module vs the facade; works on young repos).
8. Per-commit duplication introduction rate (vendored-FP removed automatically).
9. Churn/rework half-life (GitClear-style, pure git).
10. Matched-pair corpus (age/LOC/domain/contributors; CEM/propensity).
11. A full-history reclone of ~100 repos (remove the shallow censoring: 54% of boolean candidates are truncated).
12. Negative-control outcomes + **blind labeling** (without it the "2.3×" won't survive review).

### 3.3 What the literature says (adversarially verified sources)

**The closest thing to our design** — CMU, MSR 2026 (arXiv 2511.04427), ✅ verified against the full
text: diff-in-differences, **806 repos** adopting Cursor (an adoption proxy — the first commit of
`.cursorrules`), against **2418 controls** (1:3 nearest-neighbor propensity matching, same
language, AUC 0.83–0.91), monthly 01.2024–08.2025, ≥6 months before/after, panel GMM. Results (ATT,
log): **velocity is transient** (+281% lines in month 1, disappears by month 3), **cognitive complexity
(SonarQube, project-level) +41.6% ±7.6 — persistent**, static warnings **+30.3% ±6.7 — persistent**,
**duplicated lines density +7.0% ±4.8 — NOT significant**. Languages: TS 366 / Python 118 / JS 60 / Go 36 /
Rust 24 — **C++ practically absent**. Their threats to validity are word-for-word ours: visible adoption,
unknown intensity, control contamination by other AI tools.

The rest by layer:

- **Line level (duplication/churn)**: GitClear 2025 (211M lines): copy/paste 8.3%→12.3% of
  changed lines (2021→2024), duplicated blocks ≥5 lines ×8, refactored/moved 25%→<10%,
  copy/paste exceeds moved for the first time ◻; churn doubling by 2024 ✅. Academic confirmation of the clone
  line: FSE 2025 — Type-1/2 clones in commercial generators up to 7.5% ◻.
- **AI-code field study**: ~19.8k AI files — AI has **less** cross-file duplication (17.2% vs
  24.5%), but "deeper, not broader" call graphs and deeper nesting ◻ (arXiv 2603.27130).
  AI files are maintained less often after merge — the degradation accumulates silently ◻ (arXiv 2605.06464).
- **Architectural level — only synthetic**: LoC↔arch-smells ρ=0.94, prompt specificity
  doesn't matter (p>0.8), "Modular Mirage" — formal modularity without semantic cohesion ◻
  (arXiv 2605.02741); SlopCodeBench: structural erosion in 77% of agent trajectories ◻
  (arXiv 2603.24755); LLMs systematically violate modularity — reach into private members ◻
  (Harvard, LMPL 2025); constraint decay −30 pp at L0→L3, Clean Architecture −9.1 pp ✅
  (Dente et al., arXiv 2605.06445 — our constraint_decay.md).
- **Macro**: DORA 2024 — for +25% AI adoption: throughput −1.5%, stability −7.2% ✅; DORA 2025 —
  throughput turned positive, **the negative on stability persisted**, the framing "AI is an
  amplifier" ✅.
- **The counter-side (honestly)**: GitHub RCT — +13.6% lines without readability errors ◻; Google RCT —
  −21% time ◻; ANZ ~1000 engineers — +42% speed without significant quality degradation ◻;
  METR RCT — experienced OSS maintainers with AI are **19% slower** with a subjective −20% ◻;
  Uplevel — bug rate +41% without throughput growth ◻; Stanford ~100k devs — ~50% of gross gains
  eaten by rework, in brownfield the net effect is 5–10% ◻.
- Related: a practitioner survey — small AI snippets do NOT erode cohesion/coupling, erosion
  begins on large multi-file generations ◻ (arXiv 2506.17833).

### 3.4 Synthesis: what this means for us

1. **Our "null" on global metrics is not an anomaly but a reproduction of the world result.**
   Not a single work showed a signal at the level of global graph metrics of real repos.
   CMU directly got "duplication n.s." with persistent complexity growth — our pair
   "copy-paste p=0.144 / boolean-drift 2.3×" is structurally the same picture.
2. **The signal lives in local concentration**: cognitive complexity (CMU), nesting and depth
   (field study 2603.27130), private-access (Harvard), boolean/config-bag (us). This justifies
   the already-started `local_complexity_drift` experiment (6/6 TP on manual labeling) and task #099
   (indentation-complexity-drift — a proxy for Hindle ICPC 2008: indentation variance correlates
   with McCabe/Halstead ◻).
3. **C++ is a literature gap**: the CMU corpus is almost without C++, the field studies too. Our C++ corpus
   is itself a contribution; "global metrics are flat on C++" may be a language-specific
   finding (older projects, stricter review). Boolean/config-bag drift is a candidate for publication
   (MSR/TechDebt), provided blind labeling and a cluster correction (§3.2 item 12).
4. **Product positioning verbatim from DORA 2025**: the condition for safe AI speed is
   "robust control systems"; archcheck is exactly such a control system for architecture.
   The velocity transience metrics (+281% month 1 → 0 by month 3) are a ready marketing
   argument "speed leaves, complexity stays".

### 3.5 Recommended minimal package of next steps

1. Commit-level attribution on the ready 135k records (design #1) — the cheapest chance to get
   a causally clean result.
2. Blind re-labeling of the boolean samples with a second pass + a cluster-robust test — to make
   the "2.3×" publishable.
3. Local complexity drift (already underway) + the indentation variant as a mass metric without a parser.
4. Repeat the event-study in 6 months on a frozen sample (scripts exist).

---

## 4. C++ developers' pain

### 4.1 Surveys (all numbers reconciled with primary sources)

- **ISO C++ Developer Survey 2024** (Q6 about frustrations, 1261 respondents) ✅: pain #1 —
  "Managing libraries my application depends on": **45.4% major + 36.4% minor pain (~82%)**;
  #2 — **build times: 42.9% + 37.4% (~80%)**; CI from scratch — 30.4% major; CMake — 30.4%.
  Classic memory-safety is noticeably lower (~20% major).
- **Trend 2021→2024** ✅: dependencies −3.0 pp, build times −2.4 pp — the pain eases
  slowly, remaining top-1/top-2. **Chronic.**
- **ISO 2025 (1036) and 2026 (1434 respondents, +38%)** ✅: free-form "magic wand" —
  #1 a standard package manager/build metadata, #2 **"make modules/header replacement
  actually work"** (headers/includes/macros are called painful verbatim), also at the top — reduce
  build time.
- **JetBrains C++ Ecosystem 2023** (2627 C++ developers) ✅: libraries — the main pain;
  **30% don't use code analysis at all**, analysis in CI is a minority.
- Related, about AI (ISO 2025, 937 respondents) ◻: 75.1% encountered incorrect AI output,
  70.8% don't trust it — supports the "control system for AI code" positioning; ISO 2026:
  AI is most often used to understand unfamiliar/legacy code, agents lack
  dependency/context maps.

### 4.2 The physics of the problem and niche precedents

- **Headers = 99.68% of Chromium's compiled lines** ✅: 30,137 compilation steps processed
  3,611.6 million lines of dependencies against 11.86 million lines of sources; a full build is 21.45 CPU-hours;
  O(N²) growth.
- Chromium include graph ◻: 141,248 nodes, 1.31 million edges, **7.8 million simple cycles, 99%
  of which sit in one SCC of 92 files** — an argument to report SCCs, not cycles.
- **Chrome include dashboard** ◻: per-edge cost analysis (`windows.h` = 287 GB of input);
  over weeks 5.13% of compiler input and 6.1% of CPU were removed.
- **Figma** ◻: build +50% over 12 months with +10% of code (transitive includes); a homegrown
  include-graph checker in CI **blocks 50–100 regressions per day**, build sped up twofold.
  This is literally "archcheck as a CI gate", only homegrown — there was no ready product.
- Academia ◻: cyclic dependency — among the three most frequent erosion symptoms in code review
  (arXiv 2201.01184); hub-cycles grow and get more expensive (2306.10599); replacing indirect includes
  with direct ones gives up to 9% of compile time (ACM 2022).

### 4.3 C++20 modules won't close the niche (archcheck's horizon)

Adoption is negligible: **106 of 2587 popular libraries (4.1%)** ship modules (snapshot 11.2025,
arewemodulesyet.org ◻); Ropert (04.2026) doesn't recommend them to most projects ◻; converting
deal.II (800k LOC) — downstream sees "no clear trend" on build ◻ (arXiv 2506.21654); Szalay 2025:
modularizing legacy is impossible without a prior cleanup of the internal architecture ◻ →
**positioning: "archcheck — step 0 before migrating to modules"** (a modules-readiness mode,
§4.4 item 7).

### 4.4 Ideas that directly address the pains found

1. **include-weight budget + ratchet** ("no worse than yesterday" by transitive weight) — Figma/Chrome
   precedents; addresses pain #2 directly, without compilation.
2. **A "top include offenders" report** (cost = fan-in × transitive size) — an answer to the complaint
   from the HN thread about IWYU ("complains about everything, hides the 2–3 root culprits").
3. **An SCC report with a minimum cut** instead of enumerating cycles (Chromium: 7.8M cycles =
   1 SCC × 92 files; arXiv 2306.10599 — suggest an untangling pattern).
4. **A ratchet by CCD/ACD/weight** as a diff/drift mode (preventing regressions is more valuable than a one-off
   cleanup — the Figma lesson).
5. **zero-code TU**: TUs whose ~100% preprocessor input is others' headers (Chromium: 680
   generated files burned 20 CPU-minutes in vain).
6. **forward-declarability hint**: a heuristic on the include scan + tokens; the precise version — in the
   clang backend (§6).
7. **modules-readiness**: clustering the include graph into module candidates + a blocker report
   (cycles between clusters, macro dependencies).
8. **Vendored out of the box** (already present): 56.5–60.6% build dependencies into their own build (ISO
   2025/2026) — a third_party filter is mandatory against "5000 violations on first run".
9. **Marketing numbers in the README**: "dependencies and build times — major pain for 45% and 43%
   (ISO 2024); headers — 99.7% of Chromium's compiled lines; archcheck finds the cause in
   seconds without compilation".
10. **A JSON dependency map for AI agents** (component graph + levels + metrics) — a direct
    request from ISO 2026; expands the audience beyond CI.

---

## 5. Cheap architectural guards (without compilation)

Market context: in every large ecosystem except C++ there is a cheap OSS tool for
architectural rules over the import graph (ArchUnit, import-linter, dependency-cruiser,
eslint-plugin-boundaries, Tach, deptrac). In C++ this is covered only by commercial tools
with full parsing (CppDepend ~120 CQLinq rules, Sonargraph, Lattix, Structure101).
**The market gap is confirmed** by a survey of 87 practitioners (arXiv 2108.01018 ◻): they consider the
link between architecture and code important but use no dedicated tools. A performance precedent:
cpp-dependencies (TomTom) builds an include graph of 1.5 GB of code in ~2.1 s ◻.

Science: reflexion models (Murphy–Notkin, FSE 1995) — convergence/divergence/**absence**;
Arcan (Fontana et al.) — 4 architectural smells detectable on a single dependency graph
(Cyclic, Hub-Like, Unstable Dependency, God Component); Pruijt et al. (SPE 2017) — 10 mature
ACC tools massively miss **indirect** dependencies → honest transitivity is a competitive
advantage.

### 5.1 On the include graph (all the infrastructure already exists)

Ranked by value/cost ratio; the authority in parentheses:

1. **Transitive layers/forbidden/independence** with printing of the full chain (import-linter,
   ArchUnit; addresses the already-parsed but not-enforced module rules of the config!).
2. **no-private / protected headers**: `detail/`, `internal/`, `*_impl.h`, `*-inl.h` may not be
   included from outside the module (boundaries no-private, Lakos insulation). Complexity: low.
3. **entry-point**: another module — only through its public headers (include/ or umbrella);
   including another's src/ header is a violation (boundaries entry-point, Tach, Bazel visibility).
4. **external/vendor isolation**: third-party headers (`<boost/…>`, `<windows.h>`, `<QtCore/…>`)
   — only from declared adapter modules (deptrac, go-arch-lint vendors).
5. **orphan headers**: fan-in 0, not paired, not entry-point → a deletion candidate
   (dependency-cruiser no-orphans). Trivial — fan-in already exists.
6. **no-upward-include**: ban `#include "../…"` upward beyond the module root (ArchUnit
   no-classes-depend-upper). Trivial.
7. **SDP rule**: I = Ce/(Ce+Ca) per component; an edge toward a more unstable one —
   a warning (Martin SDP; precedents of cheap implementation — dependency-cruiser `moreUnstable`,
   Arcan Unstable Dependency, peer-reviewed).
8. **hub-like dependency**: fan-in AND fan-out simultaneously above thresholds — an extension of god-header
   (Arcan, peer-reviewed). Trivial.
9. **include-weight budget** ("Header Hero in CI"): cost = lines × transitive inclusions;
   a diff mode "the PR added N% of compile cost" (motivation: Chromium 0.32% of primary lines).
10. **self-include-first** — a cheap proxy for SF.5: X.cpp includes X.h as the first substantive include
    (BDE verbatim, Google cpplint build/include_order).
11. **include-guard convention** + detection of guard collisions (cpplint, bde_verify).
12. **no-include-of-impl**: ban `#include "x.cpp"` (clang-tidy bugprone-suspicious-include —
    free for us).
13. **SF.12 form**: `""` for one's own, `<>` for external; a flag on a mismatch between resolution and form
    (Core Guidelines SF.12 — nobody covers it fully).
14. **reflexion-absence**: an allowed edge declared in the config that is absent in the code →
    "unused declared dependency" (Murphy–Notkin absence, deptrac debug:unused).
15. **Dovalil's visibility metrics** RV/ARV/GRV per module into the report (ArchUnit Visibility Metrics).

### 5.2 Without the include graph: tokens / git history / struct fields

The evidence base of this layer is in places stronger than classic static analysis:
- **Clones**: Juergens et al., ICSE 2009 ✅ (PDF opened page by page) — 52% of clone groups change
  inconsistently, ~every 2nd–3rd unintended inconsistent edit = a defect (107
  confirmed bugs); Wagner 2016 — 17% of type-3 clones contain defects ◻. But Kapser &
  Godfrey ◻: cloning is often deliberate → gate **only new code** (Sonar way: ≤3%
  duplicates on new code ✅-consensus of the docs).
- **Git process metrics beat code metrics**: Rahman & Devanbu ICSE 2013 ◻; relative churn
  → 89% accuracy for fault-prone (Nagappan & Ball 2005 ◻); FixCache 73–95% (Kim ICSE 2007 ◻);
  20% of files = 83% of defects (Ostrand & Weyuker TSE 2005 ◻); change coupling correlates with
  defects more strongly than complexity (D'Ambros WCRE 2009 ◻); minor contributors → more failures
  (Bird FSE 2011 ◻). The whole Tornhill line is implementable as a CLI over `git log --numstat`
  (code-maat — proof of concept ✅); business validation: Code Red (TechDebt 2022) — unhealthy
  code = 15× defects, +124% time ✅-line; nonlinearity of returns (Borg 2024 ◻).
- **Micro-structural**: Core Guidelines I.4 (flags-enum instead of a set of bools, the anti-example
  `set_settings(true, false, 42)`) and I.23 (<4 parameters) ✅; Fowler FlagArgument ◻; config-bag —
  Xu et al. FSE 2015: Hadoop 17→173 knobs over 7.5 years, most are not touched ◻. **No direct work
  on field/state accretion as a temporal smell exists — archcheck's boolean-drift is the first here.**
- **A vaccine against cargo cult**: Lenarduzzi 2019 ◻ — "clean" and "dirty" classes by SonarQube
  don't differ in fault-proneness → gate only the narrow and high-precision.

Ideas → **tasks already created** (backlog/new/093–100): flag-argument (I.4), param-count+accretion
(I.23), config-bag-growth (Xu 2015), satd-delta (Potdar/Shihab; Wehaibi: SATD ↔ changeability,
not defects → advisory), test-co-evolution (Zaidman), god-file-growth, indentation-complexity-drift
(Hindle 2008), defect-attractor (FixCache/SZZ). Beyond 093–100, the unimplemented ideas remaining:

- **new-clone-gate** — a clone ≥N tokens with ≥1 instance entirely in the diff against the baseline
  (a ready token detector + baseline; probably the most valuable missing gate);
- **inconsistent-clone-drift** — a pair identical on the baseline, an edit touched one
  instance (the strongest signal per Juergens: ~50% of such edits = a bug); advisory high-prio;
- **copy-instead-of-refactor** — a PR adds a clone of existing code and modifies the original;
- **change-coupling drift** — a new pair of co-changing files above a threshold relative to the
  baseline (native drift machinery);
- **hotspot-report** (churn × complexity/indentation) — report only, not a gate;
- **knowledge-risk map** (Bird) — advisory, noise in a solo OSS.

### 5.3 What NOT to gate (per the literature)

The duplicate percentage across the whole repo (Kapser–Godfrey + the "5000 violations" anti-pattern); smell gates
without calibration (Lenarduzzi); hotspots/churn/ownership/defect-attractors as a gate (they punish
work in the right place — prioritization only); SATD (linked to changeability, not defects);
change coupling as an absolute (legitimate header/impl pairs, code+test).

**The through-line pattern: gate only on "delta against baseline on new code" — this is already native
archcheck machinery; everything probabilistic is advisory/report.** (Fully matches the product
rule from docs/codex_archcheck_cheap_drift_tasks.md.)

---

## 6. The second backend (libclang): what to put in and at what cost

### 6.1 What ONLY semantics provides

The only fundamentally new primitive is **knowledge of which symbols are used from where**.
On it rest three classes of checks:
1. **Edge precision**: include-without-use (phantom edge) and
   use-without-include through a transitive header. clangd include-cleaner marks
   everything used, including macro expansions, type deduction, template instantiation ✅ — on
   the preprocessor this is unsolvable (IWYU: ~90% of the code — distinguishing full use vs forward-declare ✅).
2. **Layering by symbols** (analog of Bazel `-fmodules-decluse`/`-Wprivate-header` ◻): module A
   uses a symbol from B through a legal transitive C — the include graph sees A→C and is blind to A→B.
3. **ODR/encapsulation**: SF.2 (misc-definitions-in-headers), non-const globals (I.2),
   cross-module friend, the abstractness metric A.

Validation of the two-backend design from a primary source: clang-tidy `misc-header-include-cycle` itself
works **at the preprocessor level** ✅ — cycles don't need semantics; don't move into the clang backend
anything that already works on the include graph.

### 6.2 Candidates (ranked by value/cost)

1. **Symbol-level enforcement** of existing layers/forbidden contracts (max value).
2. **Phantom-edge detection** — IWYU-lite only for cross-module edges (less noise than IWYU);
   double benefit: advisory "the edge can be removed" + refinement of CCD/ACD/cycles.
3. **SF.2** (misc-definitions-in-headers with architectural attribution + baseline) — cheap,
   already in the roadmap.
4. **Martin A and D=|A+I−1|** per component — I already exists from the graph, A requires AST; a "Zone of Pain"
   report with a Martin citation (authority over opinion).
5. **Namespace↔module consistency** — physical (directory) vs logical (namespace) structure.
6. **Cross-module friend** (English et al.: friend classes = coupling hotspots ◻) — only
   across a module boundary, otherwise noise.
7. **Mutable global state in public headers** (extern non-const = an architectural contract).
8. Concrete-types-in-interface (DIP) — expensive, v0.4+.

**Anti-ideas**: don't build your own ODR-linker detector (gold --detect-odr-violations is weak ✅-line,
the working options are Clang ODR hash only for modules and ASan at runtime); don't duplicate IWYU
in full (it's itself "beta quality", version-locked to clang ✅) — take heuristics via the
clang-include-cleaner library from LLVM upstream.

### 6.3 Cost (numbers from primary sources)

- LLVM (~7k files, 3.8M LoC): directive-scan ~3 s → full preprocessing ~28 s (9×) → full
  build ~15 min ◻ (clang-scan-deps cfe-dev thread).
- Chromium: a full clangd index 2–3 hours on 48 cores/64 GB, 550 MB disk, 2.7 GB RAM ✅
  (2019 numbers — order of magnitude, not the exact cost today).
- Individual clang-tidy checks are pathologically expensive (misc-const-correctness >10× reparse ◻).

→ Architectural consequences: the clang backend is **(a)** strictly opt-in, **(b)** requires
compile_commands.json (like clang-uml/IWYU/clang-tidy — this is the niche norm), **(c)** per-TU
incremental with a cache keyed by the compile-command hash, **(d)** a `--changed-only` mode for PR CI.

---

## 6. Results: Corpus-wide Drift Analysis (2026-06-10..11)

### Setup

To test the hypothesis on a pool of agentic C++ projects, a **corpus of 1000 repos** was assembled by criteria:
- **agentic** (AI trailer, bot authorship ≥10%, median commit length ≥100 chars)
- **> 300 commits since May 2025** (observed window, short-since)
- **≥2 years of history** (before/after baseline)
- **≤50 MB of C++** after cleanup (per-repo)

**Total:** 815 (base) → **+185 new** (process: grow_corpus.py, idempotent, audit in grow_corpus_ledger.tsv).

### Four independent drift layers

Each layer is a separate scan, not reducible to the others:

| # | Layer | Format | Result |
|---|---|---|---|
| **1** | **Per-commit Graph-Drift** (architectural dependencies) | `*_graph_drift.jsonl/.md` | Per-commit additions of include dependencies. The artifact `*_graph_drift.jsonl` recomputed for 43 repos with a stale/missing dataset; total volume — 9700+ per-commit records commit by commit |
| **2** | **Copypaste Clones** (code duplication) | `*_dup.txt` | A HEAD snapshot: EXACT/RENAMED/LITERAL pairs of 3+ lines. 25 examples in EXAMPLES_50.md with clickable links (libvirt 53 lines, fireStormViewer 94, postgresql 150, apache-arrow 39). Top repos: AlchemyViewer (1288 pairs), psryland/rylogic (1296), facebook/rocksdb (759) |
| **3** | **Boolean-Drift Per-Commit** (bool-field increment) | `bool_history_new185.csv` | 5514 commits that added `bool` fields to existing headers (not to new files). **Top repos**: OloEngineBase (207), FastLED (129), llama.cpp (113), Serial-Studio (102), AlchemyViewer (86). Window: shallow-since=2025-05-01 |
| **4** | **Boolean-Drift Per-Struct** (constraint decay) | `perstruct_drift_new185.csv` | 455 structures into which bools flowed across ≥4 distinct commits (the MIN_COMMITS criterion). **Top by span (age)**: llvm/circt LoweringOptions (22 fields, 5 commits, 735 days), qt/qtmultimedia QWasmVideoOutput (11 fields, 6 commits, 733 days), intel/compute-runtime MockDebugSession (18 fields, 4 commits, 732 days) |

### Key findings

1. **Graph-drift + Bool-drift = orthogonal signals.** A repo can leak architecture (the graph degrades, new cross-module dependencies) **and at the same time** filter boolean constraints (structures change quietly). Example: facebook/react-native leaks with 278 graph errors and **at the same time** 0.3% AI commits (an old baseline). The algorithm:
   - If **graph-errors > Δbaseline** → the team is adding dependencies (architectural drift).
   - If **per-struct drift > Δbaseline** → the team is accumulating state in structures (constraint decay).
   - If **both** → a combined signal (control vs agentic).

2. **The window shallow-since=2025-05-01 is universal.** All three layers (graph, boolean per-commit, per-struct) are measured in one window → the data is unquestionably comparable. Per-struct git-blame sees only in-window commits → fields BEFORE May collapse into the boundary commit (a conservative constraint-decay estimate, but legitimate).

3. **OOM on large repos solved by streaming.** The original `bool_history_scan.py` ate the whole `git log -p` into RAM → crashed on FastLED (130 MB, 7369 commits). Rewrote it to line-by-line `Popen` streaming → **11.85 MB peak memory on FastLED, 334 records in 1.5 min** (instead of OOM). The fix has been ported over.

### Result files

- **ai_repo_run/**: `corpus_summary.tsv` (492 rows, merged old+new), `EXAMPLES_50.md` (50 clickable examples), `grow_corpus_ledger.tsv` (KEEP/TOOBIG/GIANT audit trail), `NEW_REPOS_DRIFT_REPORT.md` (top by graph errors)
- **boolean_state/**: `bool_history_new185.csv` (5514), `perstruct_drift_new185.csv` (455), `BOOL_NEW185_DONE.marker` (2026-06-10 18:14:22)

---

## 7. Source registry

### Adversarially verified (✅ — primary source opened, numbers reconciled)

| Source | What's confirmed |
|---|---|
| [arXiv 2511.04427](https://arxiv.org/abs/2511.04427) (MSR 2026, CMU) | 806 repos/2418 controls, `.cursorrules` proxy, SonarQube cognitive complexity +41.64%±7.62 persistent, warnings +30.26%±6.66, duplication +7.03%±4.79 n.s., velocity transient (+281% m1); verified against the full HTML text |
| [GitClear Coding on Copilot](https://www.gitclear.com/coding_on_copilot_data_shows_ais_downward_pressure_on_code_quality) | churn doubling by 2024 — verbatim in the report |
| [DORA 2024](https://dora.dev/research/2024/dora-report/) | +25% AI → throughput −1.5%, stability −7.2% (the report's regression model) |
| [DORA 2025](https://dora.dev/dora-report-2025/) | throughput reversal to positive, stability negative persisted, "AI amplifier" |
| [arXiv 2605.06445](https://arxiv.org/abs/2605.06445) (Dente et al., EURECOM) | constraint decay −30 pp L0→L3 (8 strong configurations), submitted 2026-05-07 |
| [Juergens et al., ICSE 2009](https://teamscale.com/hubfs/Publications/2009-do-code-clones-matter.pdf) | 52% of clones inconsistent, |F|/|UIC|≈0.5, 107 bugs — PDF opened page by page |
| [clangd include-cleaner design](https://clangd.llvm.org/design/include-cleaner) | used-semantics: macro/deduction/instantiation |
| [misc-header-include-cycle](https://clang.llvm.org/extra/clang-tidy/checks/misc/header-include-cycle.html) | "operates at the preprocessor level" |
| [Chromium clangd docs](https://chromium.googlesource.com/chromium/src/+/lkgr/docs/clangd.md) | index 2–3 h/48 cores/64GB; 550MB/2.7GB (2019 data) |
| [IWYU WhyIWYU](https://github.com/include-what-you-use/include-what-you-use/blob/master/docs/WhyIWYU.md), [iwyu.org](https://include-what-you-use.org/) | "~90% of the code for the sake of forward-declare"; beta quality, version-lock |
| [code-maat](https://github.com/adamtornhill/code-maat) | coupling/churn/ownership/age — a standalone CLI from git log |
| [Sonar quality gates](https://docs.sonarsource.com/sonarqube-server/quality-standards-administration/managing-quality-gates/introduction-to-quality-gates) | "Sonar way": duplicates ≤3% on new code only |
| [Core Guidelines I.4/I.23](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#i4-make-interfaces-precisely-and-strongly-typed) | flags-enum vs bool; "fewer than four" |
| [ISO C++ Survey 2024](https://isocpp.org/files/papers/CppDevSurvey-2024-summary.pdf) | dependencies 45.4%+36.4%, build times 42.9%+37.4% (Q6, n=1261) |
| [ISO C++ Survey 2025](https://isocpp.org/files/papers/CppDevSurvey-2025-summary.pdf) / [2026](https://isocpp.org/files/papers/CppDevSurvey-2026-summary.pdf) | magic-wand: package manager #1, "make modules/headers work", build times (n=1036/1434) |
| [moderncppdevops 2024](https://moderncppdevops.com/2024-survey-results/) | trend 2021→2024: −3.0/−2.4 pp |
| [JetBrains C++ 2023](https://blog.jetbrains.com/clion/2024/01/the-cpp-ecosystem-in-2023/) | libraries — pain #1; 30% without code analysis (n=2627) |
| [randomascii: Chromium builds](https://randomascii.wordpress.com/2020/03/30/big-project-build-times-chromium/) | headers 99.68% of compiled lines; 3.6B vs 11.86M; 21.45 CPU-h |

### Found, but without separate verification (◻ — use with verification before citing externally)

AI quality: GitClear 2025 PDF (8.3→12.3%, ×8 blocks, refactoring <10%); arXiv 2603.27130
(19.8k AI files, 17.2% vs 24.5%, deeper-not-broader); 2605.02741 (ρ=0.94, Modular Mirage);
2603.24755 (SlopCodeBench, 77%); 2605.06464 (AI files maintained less often); 2601.18341 (Robbes,
adoption 22–29%); 2510.03029 (+63% smells); Harvard LMPL 2025 (private-access); 2604.04990
(Architecture Without Architects); FSE 2025 (clones 7.5%); the counter-side: GitHub RCT, Google RCT
(2410.12944), ANZ (2402.05636), Peng (2302.06590), METR, Uplevel, Stanford; 2506.17833 (survey).
Cheap guards: Wagner 2016; Kapser–Godfrey EMSE 2008; Code Red (2203.04374); Borg 2024
(2401.13407); Nagappan–Ball 2005; Rahman–Devanbu 2013; Kim FixCache 2007; Ostrand–Weyuker 2005;
D'Ambros 2009; Bird 2011; Xu FSE 2015; Lenarduzzi (1908.11590); Hindle 2008; Potdar–Shihab 2014;
Maldonado 2017; Wehaibi 2016; Zaidman 2011; Murphy–Notkin 1995; Pruijt SPE 2017; Li–Liang
(2103.11392); Fontana/Arcan; cpp-dependencies; randomascii (Chromium 3.6B lines); maskray
(Bazel layering_check); BDE wiki; cpplint; PMD CPD; jscpd; NiCad; SourcererCC.

---

## Appendix: link to the current backlog

- §5.2 ideas 1–8 = tasks **093–100** (created 2026-06-10 from codex_archcheck_cheap_drift_tasks.md).
- §3.4 item 2 = the experiment `local_complexity_drift` (docs/research/local_complexity_drift_examples.md,
  6/6 TP) — the literature justification now exists (CMU complexity, Hindle proxy). **But the prototype's own
  scorer (#102) measures complexity incorrectly** — analysis with a repro and a recommendation to move to
  the Sonar Cognitive Complexity scale: [local_complexity_drift_scorer_review.md](local_complexity_drift_scorer_review.md).
- §1.2 S1–S6, §1.3 (dead code), §1.4 (lizard gate, git/ copy-paste) — candidates for new tasks;
  S1/S2 are worth fixing before any public release.
- §2 "claimed but absent" — a list of edits to CLAUDE.md/README/MVP.md (see also the docs-audit findings).
- §5.1 item 1 (transitive contracts) addresses the already-parsed module rules of the config —
  a natural candidate for the next large feature of v0.2.
