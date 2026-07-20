# archcheck vs other tools

Single source of truth for "isn't this the same as X?". Consolidates the
per-dimension comparisons scattered across the repo:

- architecture / dependency graph — [architecture-spec.md §3](architecture-spec.md) (competitor table)
- clone detection — [research/clone_tools_landscape.md](research/clone_tools_landscape.md)
- cognitive complexity — [research/cognitive_complexity_delta_design.md §3](research/cognitive_complexity_delta_design.md)
- boolean-state / flag-argument — [research/tooling_survey.md](research/tooling_survey.md), [backlog task 093](../backlog/wip/093_maj_flag_argument.md)

Shipped-behavior claims are anchored to [CHANGELOG.md](../CHANGELOG.md) (authoritative)
and architecture-spec §"What it does".

---

## The load-bearing claim

> **No free, build-free, CI-first CLI gates a C++ codebase per-commit on the
> *change* in architectural-drift metrics (dependency cycles + Lakos CCD/ACD/NCCD
> levelization + include hygiene) relative to a baseline.**

The word that carries the claim is **drift**: gating on the *delta* between
commits, not on an absolute snapshot. Four qualifiers make the intersection
empty; every competitor fails at least one:

1. **Free / OSS** — kills CppDepend, Structure101, Understand, Sonargraph Architect/Build (all commercial).
2. **Per-commit drift vs a baseline** — kills every free analyzer below; they are snapshot-only (no baseline, no diff, no "got worse" exit code).
3. **CI-first CLI with exit-code gating** — kills DeepEnds (Visual Studio GUI) and Sonargraph Explorer (free, but "no architecture checks").
4. **No build required** (no `compile_commands.json` / no compiler-command capture) — kills Sonargraph (needs `ccspy` capture) and CppDepend (needs a build model).

Do **not** claim "nobody computes CCD" or "nobody detects C++ cycles" — that is
false (`cppdep` computes CCD/ACD/NCCD; `cpp-dependencies` detects cycles) and
loses the argument. The defensible move is to shift the burden of proof:

> *Show me a free CLI that exits non-zero when a PR **adds** a cycle or grows CCD
> relative to the previous commit, without `compile_commands.json`.* There is none.

The free tools prove the metrics are trivial to compute; nobody took the next
step to a **per-commit drift gate**. That step is the product.

---

## 1. Architecture / dependency graph (the core niche)

| Tool | Free | Cycles | Lakos CCD/NCCD | Include hygiene (SF.7/8/9) | Drift vs baseline / per-commit gate | No build needed | Verdict |
|---|---|---|---|---|---|---|---|
| **archcheck** | ✅ | ✅ gate | ✅ (advisory + drift) | ✅ | ✅ `--baseline`, `--diff`, `DRIFT.*` | ✅ fast preprocessor backend | **occupies the niche** |
| **cppdep** (rakhimov) | ✅ OSS | ✗ | ✅ | ✗ | ✗ snapshot report only | ✅ | metrics, no gate |
| **cpp-dependencies** (TomTom) | ✅ Apache | ✅ | ✗ | ✗ | ✗ `.dot` + `--stats` only | ✅ | viz, no gate; dead since 2021 |
| **DeepEnds** | ✅ | ~ | ~ (VS-style) | ✗ | ✗ | ✅ | Visual Studio plugin, not CLI |
| **IWYU** | ✅ | ✗ | ✗ | ✗ (minimizes includes) | ✗ | needs compile flags | adjacent task |
| **Sonargraph Explorer** (free) | ✅ | view | view | ✗ | ✗ "no architecture checks, no scripts" | needs compiler capture | free tier cannot gate |
| **Sonargraph Architect/Build** | ✗ paid | ✅ | ✅ | ~ | ✅ break build vs baseline | ✗ needs `ccspy` capture | paid + needs build |
| **CppDepend** | ✗ paid | ✅ | ✅ | ~ (CQLinq) | ~ | ✗ needs build | paid, GUI, LINQ-first |
| **SonarQube Server** | ✗ (serious tier) | weak | ✗ | ✗ | new-code gate, not Lakos drift | server | quality platform, not a C++ arch CLI |
| **Sotograph** | ✗ | ✅ | ✅ | ~ | ✅ | ✗ | closed, enterprise |

The paradigm is mainstream in other languages (ArchUnit — Java, NetArchTest —
.NET, go-cleanarch — Go, `lakos` — Dart, dependency-constraints — Rust). **C++ is
the empty niche** (architecture-spec §"Why C++ specifically").

## 2. Complexity (cyclomatic / cognitive)

Mature segment, but all snapshot threshold-checkers — none gate on per-commit
*drift* of the metric, and none build a dependency graph.

| Tool | Free / CLI | Metric | Arch graph | Per-commit drift gate |
|---|---|---|---|---|
| **archcheck** | ✅ | cognitive (Sonar CogC spec), **delta/advisory** | ✅ | ✅ (advisory in `--diff`) |
| **lizard** | ✅ | cyclomatic (CCN), tokens, params, + copy-paste | ✗ | ✗ absolute threshold |
| **pmccabe** | ✅ | cyclomatic | ✗ | ✗ |
| **Metrix++** | ✅ | cyclomatic + maintainability | ✗ | ✗ (trend report, no gate) |
| **CCCC** | ✅ | cyclomatic + fan-in/out | partial | ✗ |
| **rust-code-analysis** (Mozilla) | ✅ | cyclomatic **and** cognitive | ✗ | ✗ |

(archcheck is deliberately *not* a cyclomatic counter — that is lizard's job in
its own dogfood gate. Its complexity signal is a delta early-warning, not an
absolute per-function limit.)

## 3. Duplication

Every detector is the same token-based Rabin-Karp class; the product, not the
algorithm, is what differs — full reasoning in
[research/clone_tools_landscape.md](research/clone_tools_landscape.md). PMD CPD,
jscpd, Simian, Duplo, SonarQube (`sonar-cpd`), NiCad, CCFinderX, Deckard are all
standalone clone tools or research tools; archcheck treats a duplicate as **one
sensor** alongside cycles / SF.* / god-headers / levelization, ships as a single
static binary, and gates advisory on a **new** confirmed pair with a baseline.

### NiCad cross-check (measured, not asserted)

NiCad is the precision reference (Type-3, 89–96%). We ran it against archcheck on
its own C example `monit-4.2` — full numbers in
[reports/nicad_vs_archcheck.md](../reports/nicad_vs_archcheck.md) and the
capability grid in [reports/clone_detection_comparison.md](../reports/clone_detection_comparison.md).

| Run (`monit-4.2`, C) | NiCad | archcheck |
|---|---:|---:|
| `functions` — pairs / classes | 27 / 12 | 21 pairs, 8/12 classes covered |
| recall bugs in archcheck | — | **0** |
| archcheck-only real pairs (0 false positives) | — | **9** (small same-file, below NiCad `minsize=10`) |
| `blocks` — genuinely sub-function classes found | 8 | 1 → **6 after #190** |

Honest claim to use — narrower and stronger than "archcheck is better":

> In a NiCad/monit cross-check, archcheck reported 9 additional real copy-paste
> pairs with 0 false positives, mostly small same-file clones below NiCad's
> `minsize=10`.

Not a C++ head-to-head: NiCad has no C++ grammar, and the C-parser workaround
failed to parse 38–94% of files across eight C++ repos, so no C++ recall number
is claimed.

## 4. Boolean-state / flag-argument ("programming on flags")

No mainstream C++ analyzer detects the flag-argument smell as a rule
([research/tooling_survey.md](research/tooling_survey.md) verdict: "novel in the
practical C++ ecosystem").

| Tool | Flag-argument / boolean-state detection |
|---|---|
| **archcheck** | `ARG.1` bool-flag signature, `ARG.2` boolean-literal call, `DRIFT.BOOL_FIELD_ACCRETION` — advisory/delta |
| **clang-tidy** | none; `bugprone-easily-swappable-parameters` **explicitly excludes `bool`** |
| **SonarQube** | `S107` (param count only); boolean smell only indirectly via cognitive complexity |
| **lizard** | `--arguments` counts arity, no type/semantics, no call sites |
| **Cppcheck** | none (bug/UB focus) |
| **Designite** | class/method smells (God Class); no flag-argument detector, thin C++ support |

---

## 5. Capability matrix (archcheck vs SonarQube vs NiCad)

Binary yes/no, no vague wording. Where a capability exists only in a different
sense, it is a note below the table. Anchored to
[CHANGELOG.md](../CHANGELOG.md) for shipped behavior.

| Capability | archcheck | SonarQube Community | SonarQube paid | NiCad |
|---|:---:|:---:|:---:|:---:|
| Free | yes | yes | no | yes |
| C++ analysis | yes | yes | yes | no |
| C analysis | yes | yes | yes | yes |
| Single static CLI binary | yes | no | no | no |
| Runs in a PR | yes | yes | yes | no |
| Can fail a PR | yes | yes | yes | no |
| Baseline for pre-existing violations | yes | yes | yes | no |
| Include graph | yes | no | no | no |
| Include cycles | yes | no | no | no |
| God headers | yes | no | no | no |
| Include chain length | yes | no | no | no |
| Case-mismatch include gate | yes | no | no | no |
| Unresolved local include finding | yes | no | no | no |
| New copy-paste in PR | yes | yes | yes | no |
| Copy-paste density / percent gate | no | yes | yes | no |
| Concrete clone pairs `file:line ↔ file:line` | yes | no | no | yes |
| Boolean-field accretion drift | yes | no | no | no |
| Local complexity drift | yes | yes | yes | no |
| SATD delta in PR | yes | no | no | no |

**Bottom line.** SonarQube is the broad quality platform (C++, PR analysis,
quality gates, duplication density, security/reliability rules); NiCad is the
clone-research tool (strong clone classes, no C++ PR gate, no architecture
checks); archcheck is the C++ CI architecture tool (include graph/cycles, god
headers, chain length, baseline/diff, concrete new-copy-paste findings in PRs).
SonarQube duplication gates on a **density %**; archcheck reports a **concrete new
pair** as an advisory. SonarQube provides none of archcheck's include-architecture
checks.

---

## Sources

- cppdep — <https://github.com/rakhimov/cppdep>
- cpp-dependencies (TomTom) — <https://github.com/tomtom-international/cpp-dependencies>
- Sonargraph editions / Explorer — <https://www.hello2morrow.com/products/sonargraph> · `ccspy` capture: <https://github.com/sonargraph/ccspy>
- DeepEnds — <https://marketplace.visualstudio.com/items?itemName=ZebM.DeepEnds>
- CppDepend — <https://www.cppdepend.com/documentation/cppdepend-analysis>
- lizard — <https://github.com/terryyin/lizard>
- clang-tidy `bugprone-easily-swappable-parameters` — <https://clang.llvm.org/extra/clang-tidy/checks/bugprone/easily-swappable-parameters.html>
- flag-argument smell — <https://martinfowler.com/bliki/FlagArgument.html> · <https://luzkan.github.io/smells/flag-argument/>
- SonarQube Community Build / feature comparison — <https://docs.sonarsource.com/sonarqube-community-build/> · <https://docs.sonarsource.com/sonarqube-community-build/feature-comparison-table>
- NiCad — <https://github.com/CordyJ/Open-NiCad>
- our measured runs — [reports/nicad_vs_archcheck.md](../reports/nicad_vs_archcheck.md) · [reports/clone_detection_comparison.md](../reports/clone_detection_comparison.md)
