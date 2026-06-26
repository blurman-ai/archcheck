# AI smell signals — observable markers of AI-assisted accretion

_Reference on the markers of low-quality AI-assisted code and documentation.
The purpose of this file is not to "detect authorship" but to provide a working
taxonomy of observable smell signals that can later be translated into checks,
review checklists and backlog tasks._

Related to:

- [constraint_decay.md](constraint_decay.md) — why the problem exists at all;
- [ai_code_detection_landscape.md](ai_code_detection_landscape.md) — what external work has measured;
- [ai_drift_cases.md](ai_drift_cases.md) — our real-world drift cases;
- [code_clones.md](code_clones.md) — duplication as a separate signal class.

---

## 1. What counts as an AI smell

By **AI smell**, this document does not mean "the code was definitely written by an AI", but rather:

> an observable artifact of AI-assisted accretion — a pattern in code, tests,
> CLI, config or documentation that statistically appears often in
> agentic/LLM development and increases the risk of structural drift, technical debt
> or a false product contract.

The key idea: **we are not attributing authorship, we are assessing risk.**

The same smell can appear:

- in a human with no AI involved at all;
- in a junior;
- in an experienced developer who was rushing;
- in an agent that optimized for a local task and failed to hold the global
  invariants.

So the right question is not "is this AI code?", but:

> is there a smell here that makes the system less honest, less understandable
> or more coupled than it should be?

---

## 2. What this document does NOT do

### 2.1. It does not determine authorship

The presence of a smell does not prove that the code was written by an AI.

A strong AI-assistance signal for a repository is:

- explicit git markers;
- agent config files;
- self-declared attribution.

This is discussed separately in [ai_code_detection_landscape.md](ai_code_detection_landscape.md).

### 2.2. It does not turn archcheck into a general style linter

Many smells can be seen by eye, but not all of them need to be checked
automatically in `archcheck`.

For `archcheck`, the smells of interest are primarily those that:

- can be expressed as an architectural invariant;
- have an explicit authoritative source;
- are statically checkable;
- help hold CI boundaries rather than just style.

### 2.3. It does not treat prose signals as sufficient

Suspicious documentation, overly-generic wording, identical phrases in
comments and other "textual vibes" are by themselves too weak.

A strong signal is **code + contract + structure**.

---

## 3. Where the taxonomy comes from

The file synthesizes four kinds of sources:

1. **Empirical papers on AI code**
   - EURECOM `Constraint Decay` — structural constraints degrade more
     than purely functional tasks.
   - `AI-Generated Smells` (arXiv:2605.02741) — AI code has distinguishable
     code-level, structural and architectural smells.
   - `Debt Behind the AI Boom` (arXiv:2603.28592) — the bulk of the
     AI-introduced issues found relate to maintainability/code smells.
2. **Vendor/industry measurements**
   - GitClear — growth in duplication and a drop in moved/refactored code.
3. **Review guidance**
   - GitHub Docs advise reviewing AI code through intent, integration,
     edge cases, tests and maintainability, rather than through an attempt to guess the author.
4. **Our own observations from the repository and drift runs**
   - shortcut edges;
   - preview/research leakage;
   - contract drift;
   - placeholder/no-op shipped paths.

---

## 4. Confidence levels

It is useful to separate markers by signal strength.

| Level | What it means | Example |
|---|---|---|
| **H1 strong structural** | the smell is directly visible as a structural or contract violation | new shortcut edge, fan-out spike, parse-but-don't-enforce config |
| **H2 medium local** | the smell is visible locally, but normal explanations are possible | near-clone, wrapper without value, giant orchestration file |
| **H3 weak prose/process** | the smell is noticeable in the narrative/tests/docs, but without code the signal is weaker | stale docs, review-theater tests, generic TODO framing |

For automation in `archcheck`, **H1** is the priority, then part of **H2**.

---

## 5. Map of smells and their applicability to archcheck

| ID | Smell | Type | Confidence | Checkability |
|---|---|---|---|---|
| **AIS.1** | Promise > implementation | contract drift | H1 | review / repo-internal |
| **AIS.2** | Parse-but-don't-enforce | fake-config support | H1 | repo-internal |
| **AIS.3** | Placeholder/no-op in shipped path | dead semantics | H1 | text/AST/review |
| **AIS.4** | Dead surface: flag accepted, output mode unused, schema field inert | false surface | H1 | text/review |
| **AIS.5** | Near-clone families | duplication | H2 | duplication backend |
| **AIS.6** | Parallel branch explosion | local copy-paste schema | H2 | duplication/text |
| **AIS.7** | Shortcut edge across layers | architecture drift | H1 | graph/rules/drift |
| **AIS.8** | Fan-out explosion / god consumer | coupling growth | H1 | graph/rules |
| **AIS.9** | Fan-in hub / god header/file | bottleneck accretion | H1 | graph/rules |
| **AIS.10** | Chain-length / blast-radius growth | hidden propagation cost | H1 | graph/metrics/drift |
| **AIS.11** | Duplicate plumbing / zero-value wrappers | accidental indirection | H2 | text/review |
| **AIS.12** | Giant orchestration / context sink | accumulation hotspot | H2 | metrics/review |
| **AIS.13** | Research leakage into product path | maturity drift | H1 | review/repo-internal |
| **AIS.14** | Source-of-truth fragmentation | doc-code drift | H2 | docs/review |
| **AIS.15** | Test theater / scaffolding tests | false confidence | H2 | review |

`Checkability` in the table means:

- **graph/rules/drift** — a good candidate for an `archcheck` product check;
- **duplication backend** — a candidate for the duplication/research layer;
- **repo-internal/review** — an important smell, but more for dogfood audit than
  for a user-facing default rule.

---

## 6. Detailed markers

### AIS.1. Promise > implementation

**Essence.** The public surface promises a capability that does not exist at runtime.

**What it looks like:**

- help promises an operating mode that the code does not perform;
- spec or README promise a field/semantics that does not exist in the data model;
- the changelog writes "added" even though in fact it is a partial scaffold.

**Why it is typical of AI-assisted work:**

- the model is very good at building the external contract first;
- the external form often appears earlier than honest semantics;
- a person sees a nice CLI/docs layer and underestimates the depth of incompleteness.

**Why it is dangerous:**

- the user trusts a non-existent function;
- tests may pass on a superficial path;
- debt grows not as a TODO but as a false promise.

**False positives:**

- an ordinary unfinished feature by a human;
- a migration period between two contracts.

**For archcheck:** an important smell, but mainly for dogfood and repo audit.
It is not a good default rule for other people's repositories.

### AIS.2. Parse-but-don't-enforce

**Essence.** A config, flag or schema is parsed and validated, but does not affect
real behavior.

**What it looks like:**

- the loader knows the YAML/JSON structure;
- tests check parsing/validation;
- the runtime uses only a small part of the data or none at all.

**Why it is typical:**

- it is easy for an agent to "finish off" the parser and the syntax tests;
- bringing it up to semantic enforcement requires a global understanding of the pipeline.

**Why it is dangerous:**

- this is the most insidious form of feature theater;
- formally "support exists", in fact it does not.

**For archcheck:** a very strong internal smell. It is usually not exported into the user
rule set, but should be regularly caught by dogfood audit.

### AIS.3. Placeholder / no-op in a shipped path

**Essence.** In the production path there is a stub that looks like a feature.

**What it looks like:**

- the function always returns a default/empty result;
- a phase hook exists but does nothing;
- inside the shipped pipeline there is a comment of the "temporary / assume / stub" kind.

**Why it is typical:**

- the agent readily builds out a scaffold to "compiles and looks complete";
- the semantic finishing step is deferred.

**Why it is dangerous:**

- the reviewer underestimates the incompleteness;
- the team starts building new expectations on top of the stub.

**For archcheck:** partially checkable textually, but more often a review smell.

### AIS.4. Dead surface

**Essence.** The surface is extended, but no real behavioral power has been added.

**What it looks like:**

- a flag is accepted but does not affect the result;
- a schema/output mode is declared but not populated;
- a public option exists but the downstream path is not connected.

**Difference from AIS.3:** here the main smell is not in a function stub, but in a false
extension of the interface.

**For archcheck:** useful for internal audits of the CLI and format contracts.

### AIS.5. Near-clone families

**Essence.** Local families of almost identical pieces of code appear, differing
by 1-2 identifiers, literals or a condition.

**What it looks like:**

- repeated blocks of 5+ lines;
- identical branch structure;
- the same calls in a different order or with different names.

**Why it is typical:**

- the agent optimizes for "get working code fast";
- abstraction-after-the-fact almost never happens automatically;
- GitClear precisely records the growth of copy/paste and the drop in moved/refactored code.

**Why it is dangerous:**

- the places that need changing multiply;
- later the agent keeps copying the already-spoiled pattern.

**For archcheck:** a good candidate for the duplication/research layer, but not for
a trusted mandatory gate without precise FP calibration.

### AIS.6. Parallel branch explosion

**Essence.** The same template is multiplied across `if/else`, `switch`, table
initializers and similar blocks.

**What it looks like:**

- 4+ sibling branches with almost identical bodies;
- only the literal, enum, field name or one call changes.

**Why it is typical:**

- the agent prefers a locally understandable expansion instead of a new abstraction;
- especially common in UI bindings, config-fill code, per-sensor/per-field logic.

**For archcheck:** more of a duplication-normalization / smell-classifier than
a basic architectural rule.

### AIS.7. Shortcut edge across layers

**Essence.** For a quick local task, a direct edge is added across an
architectural boundary: a layer begins to depend on something it did not depend on before.

**What it looks like:**

- a new `#include` across a layer;
- the UI directly pulls in preferences/widgets/data-access;
- generic/util begins to depend on a feature namespace.

**Why it is typical:**

- the agent chooses the shortest path to the symbol it needs;
- without an external guardrail, the architectural route almost always loses to local convenience.

**Why it is dangerous:**

- this is the canonical structural drift;
- such edges quickly become the norm and begin to self-replicate.

**For archcheck:** one of the best candidates. It already maps well onto graph rules,
module constraints and DRIFT rules.

### AIS.8. Fan-out explosion / god consumer

**Essence.** A component begins to depend directly on too many neighbors.

**What it looks like:**

- a `.cpp` or header pulls in dozens of project includes;
- an orchestration unit turns into a collector of all subsystems at once;
- one module "knows about everything".

**Why it is typical:**

- the agent adds yet another include/dep without a cost model for coupling;
- refactoring toward an intermediate abstraction layer more often does not happen.

**Why it is dangerous:**

- coupling grows even without cycles;
- the component becomes expensive to change and test.

**For archcheck:** an excellent candidate for a simple graph check:
`Lakos.GodComponentFanOut`.

### AIS.9. Fan-in hub / god header/file

**Essence.** Too much code begins to depend on a single node.

**What it looks like:**

- a header with very high fan-in;
- a utility header gradually becomes a dumping ground.

**Why it is typical:**

- the agent likes to "put it next to something similar";
- once a file has become a convenient point, it begins to attract new responsibilities.

**For archcheck:** already exists as `Lakos.GodHeader`. It is not an AI-specific
rule, but a very useful guardrail against AI-driven accretion.

### AIS.10. Chain-length / blast-radius growth

**Essence.** Even without cycles and explicit violations, depth and radius of influence grow.

**What it looks like:**

- include chains become longer;
- a change in one component potentially touches an ever-larger number of nodes;
- the `edges/nodes` connectivity grows faster than can be justified by new architecture.

**Why it is typical:**

- the agent successfully adds working dependencies, but does not pay the cost for global
  propagation complexity;
- this is "silent drift", which is not visible from a single PR.

**For archcheck:** a very good candidate for report-only metrics and drift gates.

### AIS.11. Duplicate plumbing / zero-value wrappers

**Essence.** Local pipework/helpers/wrappers that add little or zero new semantics
multiply across the repository.

**What it looks like:**

- two almost identical functions for launching a subprocess/file IO/path normalization;
- a thin wrapper just forwards the call downward;
- the same lifecycle code is repeated in several TUs.

**Why it is typical:**

- the agent more often copies an existing local template than searches for a point of consolidation;
- "make one more helper" is cheaper for the local task than re-decomposing the module.

**Why it is dangerous:**

- policy drift between copies;
- small divergences start behaving like bugs.

**For archcheck:** more often a review/audit smell than a user-facing default rule.

### AIS.12. Giant orchestration / context sink

**Essence.** One file or class becomes the place where the agent keeps
piling up new control branches.

**What it looks like:**

- `main.cpp`, `dispatcher.cpp`, `pipeline.cpp`, `manager.cpp` grow quickly;
- phase-style numbering (`phase7`, `phase12`, `step9`) lives too long;
- the file knows about the CLI, and policy, and reporting, and the file system.

**Why it is typical:**

- it pays off for the agent to keep writing where context already exists;
- locally this is the cheap path, globally a future maintenance sink.

**For archcheck:** one can measure size/degree as an aid, but as a default
rule — carefully, so as not to turn the product into a length linter.

### AIS.13. Research leakage into product path

**Essence.** An experimental or noisy layer enters the shipped CLI/build/docs
before the trust bar has been reached.

**What it looks like:**

- a preview/research capability is advertised as an ordinary feature;
- an internal harness sits next to the ordinary tests;
- a placeholder evaluator gets into the core build.

**Why it is typical:**

- both agent and human easily "bring it to a working state" but do not set an explicit
  maturity boundary;
- the external form of the feature appears earlier than a policy about its status.

**For archcheck:** this is more of a repo-governance smell, but for archcheck itself
it is critical.

### AIS.14. Source-of-truth fragmentation

**Essence.** Several different truths about the product live in the repository at once.

**What it looks like:**

- `AGENTS.md`, spec, roadmap and README describe different states;
- naming/style rules diverge;
- backlog and docs reference deleted branches as if they were current.

**Why it is typical:**

- agent-assisted work accelerates the production of artifacts;
- different layers are updated at different speeds;
- nobody pays the cost of aligning the narrative right away.

**For archcheck:** not a rule for the user, but a most important smell for dogfooding.

### AIS.15. Test theater / scaffolding tests

**Essence.** Tests create a sense of reliability, but check things that are not what is really
important for the user contract.

**What it looks like:**

- parsing/creation is tested, not enforcement/behavior;
- the test confirms the presence of an object/flag, not the effect;
- an integration path without fixtures is replaced by local synthetics that do not catch
  the real semantic hole.

**Why it is typical:**

- it is easier for the agent to quickly top up coverage on shallow checks;
- superficial green tests mask contractual holes.

**For archcheck:** this is a review smell; partially cured by mandatory fixtures and
dogfood policy.

---

## 7. What is especially important for archcheck

Not all smells are equally useful for our product. For `archcheck`, high
priority goes to signals that:

- are expressed by the dependency graph;
- are related to structural drift;
- do not require guessing at the author's intent;
- produce few false positives;
- are explainable to the user through Lakos / Core Guidelines / a drift story.

### Priority P1 — can and should be translated into checks

- **AIS.7 Shortcut edge**
- **AIS.8 Fan-out explosion**
- **AIS.9 Fan-in hub**
- **AIS.10 Chain-length / blast-radius growth**

These are good candidates for the `graph/rules/drift` layer.

### Priority P2 — can be translated, but carefully

- **AIS.5 Near-clone families**
- **AIS.6 Parallel branch explosion**
- **AIS.12 Giant orchestration**

Here the risk of FP or blurring the product scope is already higher.

### Priority P3 — important for dogfood and repo audit, but not for user rules

- **AIS.1 Promise > implementation**
- **AIS.2 Parse-but-don't-enforce**
- **AIS.3 Placeholder/no-op**
- **AIS.4 Dead surface**
- **AIS.13 Research leakage**
- **AIS.14 Source-of-truth fragmentation**
- **AIS.15 Test theater**

---

## 8. What should NOT be declared an AI smell

You must not make a smell out of something that is too weakly tied to architectural risk.

Bad candidates:

- just a long comment;
- just generic English names;
- just a large PR;
- just many created files;
- just "the code looks too tidy";
- just the presence of `AGENTS.md` or `CLAUDE.md`;
- just traces of attribution in git.

This may indicate AI-assistance, but says almost nothing about quality.

---

## 9. Practical conclusion

If the goal is to hold the architecture in the era of agentic development, then what we need
to fight is not "AI authorship" but **machine-typical trajectories of degradation**:

- a locally convenient edge across a layer;
- imperceptible growth of coupling;
- copy-paste instead of consolidation;
- a superficial feature surface without real enforcement;
- leakage of an experiment into the product surface.

This is exactly why, for `archcheck`, the most useful AI-smell checks are not
stylometry and not authorship detection, but **graph/drift/coupling guardrails**.

---

## 10. What of this can be turned into simple checks first

If we pick cheap and honest checks with minimal scope creep, the order is
as follows:

1. **`Lakos.GodComponentFanOut`**
   - a simple graph rule;
   - already recorded in the spec as planned;
   - catches the AI-typical "consumer knows too much" very well.
2. **Report-only metrics `edges/nodes` and `max blast radius`**
   - a good early signal of silent drift;
   - can be shown to the user without a hard gate.
3. **A drift gate on blast-radius growth**
   - a strong regression signal, once there is confidence in the thresholds.
4. **After that, the duplication candidates**
   - only after separate FP stabilization.

---

## Sources

- GitHub Docs: [Review AI-generated code](https://docs.github.com/en/enterprise-cloud%40latest/copilot/tutorials/review-ai-generated-code)
- EURECOM / Dente, Satriani, Papotti: [Constraint Decay](https://arxiv.org/abs/2605.06445)
- Zhu, Tsantalis, Rigby: [AI-Generated Smells](https://arxiv.org/abs/2605.02741)
- Aymen et al.: [Debt Behind the AI Boom](https://arxiv.org/abs/2603.28592)
- GitClear: [AI Code Quality 2025](https://www.gitclear.com/ai_assistant_code_quality_2025_research)
- A practical survey field on detection and datasets:
  [ai_code_detection_landscape.md](ai_code_detection_landscape.md)
