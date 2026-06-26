# Local complexity growth in a diff: formal measurement and delta-metric design

_2026-06-11. An answer to the owner's question: "adding 50 indented lines is just
volume; adding an extra/nested if is cognitive complexity growth; how do you measure it
FORMALLY?" Two web investigations (the formal literature + a study of implementation sources),
with primary sources opened and cited. This is the design basis for #101/#102, replacing
the prototype's homegrown formula (a review of its defects — [local_complexity_drift_scorer_review.md](local_complexity_drift_scorer_review.md))._

---

## 1. A formal answer exists: Sonar Cognitive Complexity (Campbell, white paper v1.7)

The metric is designed precisely around the thesis "volume ≠ complexity": **increments are charged
only for breaks in linear control flow and for nesting** — linear code of any size yields 0
*by construction*. Source: [the spec (PDF)](https://www.sonarsource.com/docs/CognitiveComplexity.pdf), Appendix B.

Four types of increments:

| Type | What | +1 | nesting surcharge | raises nesting |
|---|---|---|---|---|
| **Structural** | `if`, `?:`, `switch`, `for`/range-for, `while`/`do-while`, `catch` | ✅ | ✅ (+depth) | ✅ |
| **Hybrid** | `else`, `else if` ("the mental cost is already paid when reading the if") | ✅ | ❌ | ✅ |
| **Fundamental** | `goto`; each **series** of same-kind `&&`/`||` (+1 on a change of kind); each method of a recursion cycle | ✅ | ❌ | ❌ |
| **Nesting** | a surcharge to structural constructs by depth inside B2 structures (including lambdas) | — | — | — |

**Ignored** (this is exactly the answer to "50 flat lines"): the method itself; any number of linear
statements; early `return`, ordinary `break`/`continue`; `try`/`finally`; **the number of cases in a
switch** (the whole switch = +1); the number of types in a catch; null-coalescing/shorthand.

The spec's calibration examples: a switch over 4 cases — cyclomatic 4, **CogC 1**; nested loops
with a labeled continue — cyclomatic 4, **CogC 7**. That is, the metric diverges from cyclomatic
in exactly the two places that were noisy in our prototype: flat dispatch gets cheaper, nesting gets
more expensive.

## 2. How well validated this is (honestly)

- **Meta-analysis** by Muñoz Barón, Wyrich, Wagner (ESEM 2020; ~24,000 ratings, 427 snippets,
  Java/C/C++/C#/JS): correlation with **comprehension time r = 0.54** (strong); with
  answer correctness r = −0.13 (weak); composite r = 0.40. The authors call CogC
  "the first validated and solely code-based metric" for understandability.
  [arXiv 2007.12520](https://arxiv.org/abs/2007.12520)
- **Critique** by Lavazza et al. (JSS 2023): CogC correlates with understandability "about as well as
  traditional metrics" (MCC, LOC) — this is not a breakthrough but a careful packaging.
  [DOI 10.1016/j.jss.2022.111561](https://www.sciencedirect.com/science/article/abs/pii/S0164121222002370)
- **Why homegrown formulas are doomed**: Gil & Lalouche (EMSE 2017) — the validity of almost
  any complexity metric is explained by its correlation with size (R² up to 0.97).
  A formula of "control words × nesting + indentation" inevitably measures volume; CogC breaks
  this link for linear code by design. [EMSE 2017](https://link.springer.com/article/10.1007/s10664-017-9513-5)
- **Regular code is easier than additive metrics say**: Jbara & Feitelson
  (ICPC 2014 / CACM 2023) — the giant flat switches of the Linux kernel (MCC up to 620) are understood
  easily; "additive… metrics overestimate the complexity of regular code". An independent
  empirical justification of "switch = +1". [CACM](https://cacm.acm.org/research/from-code-complexity-metrics-to-program-comprehension/)
- **Indentation** (Hindle ICPC 2008) — a valid *proxy for classical metrics*, that is, it
  inherits their correlation with volume. The #102 prototype effectively reinvented this work
  along with its weakness. The place for indentation is diagnostics/file-level fallback (#099), not the score.
- Formally rigorous alternatives (Harrison/Magel 1981, Howatt/Baker 1989 nesting metrics;
  Shao & Wang cognitive weights; DepDegree — data-flow, needs def-use) have not been validated against human
  comprehension at the meta-analysis level. Halstead/ABC grow with any code —
  they don't fit the invariant.

## 3. Implementations (sources opened and studied)

| Tool | Parsing | Counting core | Recursion | `#if` | Note |
|---|---|---|---|---|---|
| [clang-tidy readability-function-cognitive-complexity](https://github.com/llvm/llvm-project/blob/main/clang-tools-extra/clang-tidy/readability/FunctionCognitiveComplexityCheck.cpp) | clang AST | ~160 lines (3 switches B1/B2/B3) | ❌ (FIXME in the code) | ❌ (documented deviation) | default threshold **25**; IgnoreMacros option |
| [sonar-java CognitiveComplexityVisitor](https://github.com/SonarSource/sonar-java/blob/master/java-frontend/src/main/java/org/sonar/java/ast/visitors/CognitiveComplexityVisitor.java) | AST | ~200 lines | separately | n/a | reference for the semantics (the else-if trick, series flattening) |
| [sonar-cxx](https://github.com/SonarOpenCommunity/sonar-cxx/pull/1245) | its own lightweight SSLR (NOT libclang) | comparable | ✅ direct | — | **a precedent for C++ without clang** |
| [gocognit](https://github.com/uudashr/gocognit/blob/master/gocognit.go) | go/ast | ~280 lines | ✅ direct by name | n/a | |
| [rust-code-analysis](https://github.com/mozilla/rust-code-analysis/blob/master/src/metrics/cognitive.rs) | tree-sitter (C++ supported) | ~200–250 lines | TODO | — | tree-sitter = megabytes of parser.c — against "dependencies: minimum" |
| lizard | fuzzy tokens | **CCN only**, no CogC | — | — | a precedent for a token detector of C++ function boundaries |

## 4. Token implementability for archcheck (without AST)

Almost the entire spec maps onto tokens; archcheck already has half the foundation —
`lex()` (`scan/duplication/token_normalizer`) and the body detector `){…}` (`fragmenter`).

| Spec rule | On tokens | How |
|---|---|---|
| `if`/`for`/`while`/`switch`/`catch` +1+nesting | ✅ | keyword + brace stack |
| `else if` = hybrid +1 (not two) | ✅ | adjacent tokens `else` `if` (the form `else { if }` is distinguishable by `{` — the spec deliberately penalizes it more) |
| `do-while` once | heuristic | `do` flag in the brace stack, don't count the trailing `while` |
| `case`/`default` free | ✅ | ignore |
| Series of `&&`/`||`: +1 per series, +1 on change | ✅ | lastOp stack by `(` depth; `!` and entering parens break the series; include `and`/`or` |
| `&&` ≠ rvalue reference | heuristic | logical only if the previous significant token is an identifier/literal/`)`/`]` |
| Lambda: +0, nesting+1 | heuristic | `[` after `=`,`(`,`,`,`return`,`{`,`;`; an error costs only a nesting shift |
| Nesting only from control structures | heuristic (reliable) | a classified brace stack: `{` after a `)` control header = control; after `class/namespace/=`/`return` — no; braceless body = pending until `;` |
| `#if`/`#ifdef`/`#elif` +1 (the spec requires it!) | ✅ | **the token scanner is stronger than clang-tidy** (which is "not accounted for"); for the stack take the first branch (like lizard); an option, default off for comparability |
| Direct recursion +1 | heuristic | name from the signature + `name(` in the body (like gocognit/sonar-cxx); an option, default off |
| Indirect recursion | ❌ | needs a call graph — not in clang-tidy either |
| Macro expansion | ❌ | count "as written" ≈ IgnoreMacros=true; control-flow macros (`Q_FOREACH`, Catch2) — a documented undercount |
| Nesting contribution of ternary branches | ❌ | a rare case, ignore |

**Expected error vs clang-tidy**: on code without control-flow macros — a divergence of
0–2 points per function; on macro-heavy code — a systematic undercount. For a delta this is
acceptable: both sides of the diff are counted by one deterministic scanner, the systematics
cancels under subtraction; what is critical is monotonicity, not absolute calibration.

## 5. Delta-metric design for #101

1. **CogC per the Campbell spec, per function** in the base and head versions of each touched file;
   Δ = CogC_head − CogC_base (a new function: Δ = CogC).
2. **Signals** (in decreasing order of strictness, each with an industry precedent):
   - a function crossed the **absolute threshold 25** (default Sonar C-family AND clang-tidy;
     Campbell confirms — the thresholds are chosen empirically "up to acceptable noise", not derived scientifically);
   - **Δ > 0 in a function already above the threshold** (analog of CodeScene "degradation in hotspot");
   - **Δ ≥ K per single PR** as a soft warning; there is no scientific K ("thresholds remain
     undefined" — the meta-analysis), start with K≈5 and calibrate on the #102 corpus.
3. **Do not normalize by dividing by diff size** — that brings back the correlation with volume
   (Gil & Lalouche). Normalization is built into the metric: linear statements yield 0.
4. PR aggregate = **sum of positive Δ**; report negative Δ separately as an
   improvement (positive reinforcement, the model being CodeScene Delta Analysis).
5. Precedents for delta gates: Sonar "Clean as You Code" (new code conditions),
   [CodeScene Delta Analysis](https://docs.enterprise.codescene.io/versions/4.5.0/guides/delta/automated-delta-analyses.html),
   [Delta Maintainability Model](https://dl.acm.org/doi/10.1109/TechDebt.2019.00030)
   (SIG, TechDebt 2019; in PyDriller, used by GitLab).

**Why this solves both problems of the prototype:** 50 flat lines (including aligned
continuations) → 0; a flat switch-parser over 200 cases → +1; an extra if at depth 3 → +4.

## 6. Minimal core for implementation (estimate ~200–300 lines, like all the ones studied)

First wave: structural increments by keywords + `?`; hybrid `else`/`else if`;
series of logical operators via the lastOp stack; a classified brace stack on top of `lex()`
(reuse of the `extractFragments` logic). Second wave (heuristics): the do-while pairing,
braceless bodies, the lambda pattern, direct recursion (option off), the `#if` increment (option off).
Deliberately not done: indirect recursion, macro expansion, tree-sitter (a dependency against
the "dependencies: minimum" principle; not needed for a delta).

## 7. Attribution for docs/marketing

"Sonar Cognitive Complexity (Campbell 2018/2023), empirical validation — Muñoz Barón
et al., ESEM 2020 (r = 0.54 with comprehension time)" — consistent with the authority
over opinion principle. An honest caveat: Lavazza 2023 showed that CogC does not surpass classical
metrics on correlations; for our task the decisive property is that **insensitivity to
linear volume is built into the spec formally**, not in correlations.
