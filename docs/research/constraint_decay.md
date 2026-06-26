# Constraint Decay — the source of motivation for archcheck

This document holds the links and an exhaustive summary of two materials that were the
direct trigger for creating archcheck. Read it when you need to remember *why*.

---

## Sources

| Material | Link |
|---|---|
| Preprint (arxiv) | https://arxiv.org/abs/2605.06445 |
| HN discussion | https://news.ycombinator.com/item?id=48256912 |

---

## Paper: "Constraint Decay: The Fragility of LLM Agents in Backend Code Generation"

**Authors:** Francesco Dente, Dario Satriani, Paolo Papotti (EURECOM, University of Basilicata)
**Date:** May 7, 2026, preprint under review.

### The gist in one sentence

LLM agents generate working code well under loosely specified tasks — but degrade as
structural constraints accumulate (architectural pattern, database, ORM). The authors
named this *constraint decay*.

### What was measured

The authors fixed a single API contract (OpenAPI 3.0, 19 endpoints, 291 assertions) and
ran it through four levels of structural constraints:

| Level | Active constraints |
|---|---|
| **L0** | web framework only (baseline) |
| **L1** | + Clean Architecture |
| **L2** | + Clean Architecture + PostgreSQL or SQLite |
| **L3** | + Clean Architecture + PostgreSQL + SQLAlchemy/Sequelize ORM |

In total: 80 tasks (greenfield) + 20 tasks (feature implementation in an existing repo) ×
8 web frameworks (Flask, FastAPI, Django, Express, Fastify, Koa, aiohttp, Hono) ×
2 agents (Mini-SWE-Agent, OpenHands) × 6 models. Roughly 5 billion tokens in all.

Evaluation is twofold: **behavioral** (HTTP tests, independent of the code) +
**structural** (architecture, database, and ORM verifiers).

### The headline result: constraint decay

Eight strong configurations (L0 A% > 50%) lose on average **30 percentage points**
from L0 to L3 — that is 40% of baseline.

Assertion pass rate A% table for the key configurations:

| Agent | Model | L0 | L1 | L2 | L3 | ΔA% |
|---|---|---:|---:|---:|---:|---:|
| Mini-SWE | MiniMax-M2.5 | 88.6 | 92.5 | 66.8 | 58.3 | −30.3 |
| OpenHands | MiniMax-M2.5 | 91.0 | 97.0 | 87.3 | 78.6 | −17.0 |
| Mini-SWE | Kimi-K2.5 | 85.4 | 70.9 | 62.9 | 53.7 | −31.7 |
| Mini-SWE | GPT-5.2 | 78.2 | 49.8 | 27.1 | 48.0 | −30.2 |
| OpenHands | Qwen3-Coder-Next | 73.0 | 51.7 | 42.7 | 27.6 | −45.5 |

Worst case (OpenHands + Qwen3-Coder-Next) — a loss of **45 pp (62% of L0)**.
Most resilient (OpenHands + MiniMax-M2.5) — a loss of 17 pp.

### Which constraint costs the most

Marginal effect of each constraint (matched-pair analysis):

| Constraint | Average penalty ΔA% |
|---|---:|
| PostgreSQL | −19.3 ± 2.5 |
| SQLite | −14.3 ± 2.5 |
| Clean Architecture | −9.1 ± 1.6 |
| SQLAlchemy | −1.5 ± 2.1 |
| Sequelize | −0.6 ± 0.2 |

**The database** is the main killer. The architectural pattern costs a lot, but not
catastrophically. The ORM on its own is almost free (because the complexity is already
baked into the DB constraint).

### Framework sensitivity

Lightweight frameworks with minimal conventions (Express, Koa, Flask) average ~50% A%.
"Convention-heavy" ones (Django, FastAPI) trail by 25–32 pp.

The explanation: Django and FastAPI are built around specific patterns, and the agent is
forced to invert their conventions to fit a foreign architecture — an extra load.

### Failure cause analysis

Logical errors account for ~71% of failures. Within them:

| Subcategory | Qwen3 | MiniMax |
|---|---:|---:|
| Incorrect query logic | 25.5% | 15.0% |
| DB/ORM runtime error | 21.2% | 15.0% |
| Auth misconfiguration | 22.6% | 5.0% |
| Framework idiosyncrasy | 9.5% | 50.0% |
| Business logic defect | 11.7% | 10.0% |

**Data-layer defects** (the first two rows) are the leading cause for both models (~45%
of logical errors), which is exactly why the DB constraint carries the largest penalty.

### Validation on real codebases

Feature implementation tasks — the agent reads an existing RealWorld Conduit repository
(already with PostgreSQL + SQLAlchemy + Clean Architecture), infers the conventions, and
adds functionality. Result: pass@1 stays low (only GPT-5.2 > 50%). Constraint
decay is not an artifact of synthetic setups — it reproduces on real repositories too.

### The authors' conclusion

> For end-users, this dichotomy implies that agents are reliable for rapid prototyping
> but remain unreliable for production-grade backend development. Overcoming this bottleneck
> requires a shift for agent developers: moving beyond purely functional benchmarks to
> actively integrate structural awareness, potentially through retrieval-augmented framework
> documentation, constraint-oriented planning, or targeted pre-training on
> convention-heavy codebases.

---

## HN discussion (285 points, 195 comments, ~May 24, 2026)

Link: https://news.ycombinator.com/item?id=48256912

### Theme 1: "We're moving complexity into markdown"

Top comment (author — a working developer who generates 80%+ of their code with an LLM):

> At some point this starts to look like we're all just moving complexity from the more
> formal and deterministic world of programming languages to the informal and
> non-deterministic world of natural language.

A follow-up reply:

> It's like using a compiler that generates semantically different code every time you run it.
> Basically like compiling a program that's full of UB but "seems to work" most of the time.

This is the sharpest framing of the problem: rules in CLAUDE.md / the system prompt are not
a machine contract. They decay along with the context, you can't diff them, and you can't
rely on them in CI.

### Theme 2: "You need linter rules, not markdown"

> This is why you need to be generating more linter rules instead of just having things
> be in markdown files. I had never written an eslint rule until i started having agents
> pump them out for me and now I've encoded a bunch of important rules as lint rules that
> will fail CI if violated.

This literally describes archcheck's place in the ecosystem: a static check in CI
instead of an undefined prompt.

### Theme 3: "Classic practices work — AI proved it"

> If there is one good thing that the generative AI tools have shown beyond any doubt it's
> that the classic "good programming" practices are still useful and effective. Self-
> documenting code. Modular design. Clearly defined architecture. Incremental development.
> Coding standards. Automated tests. Automated everything.

Lakos, Core Guidelines, Martin — these are not academic artifacts. The AI era has made them
relevant again as the only way to hold structure together under agentic development.

### Theme 4: "Calcification" — the pattern reinforces itself

An observation from practice:

> I have found something I've been calling "calcification", where a pattern starts
> appearing in the codebase and the agent follows the pattern to the point where it
> dominates the context and becomes self-reinforcing.

This is both a strength and a weakness: if the architecture is right — the agent reproduces it.
If it isn't — it reproduces that too.

### Theme 5: "God files" — the agent doesn't abstract on its own initiative

> The models are OK at modularization when given space to "plan" their implementation,
> but rarely decide that abstracting something would be helpful after the fact [...].
> This often leads to "god files" which, when pointed to by the user/architect, causes
> the models to correctly critique the code but often be confused about how to remedy it.

God-headers / god-files are exactly what archcheck detects via the fan-in metric.

### Theme 6: Context window vs. constraints — a zero-sum game

> You can't use all of the context window because at the end, the output would not
> respect the constraints (or guardrails) but to reliably produce production grade code
> you want the model to have expansive awareness which fills up the context window pretty
> quickly.

Constraint decay is, among other things, a manifestation of "constraints drown in the
context." A CI check has none of this problem: it needs no context, it looks at the graph.

### Theme 7: "You can't keep up with fixing invariants"

> The situation is worse. Not only do agents have more difficulty under "structural
> constraints", but structural constraints may need to change, and agents are even worse
> at that. When designing a system or a component we have ideas that form invariants.
> Sometimes the invariant is big, like a certain grand architecture, and sometimes it's
> small, like the selection of a data structure. Except, eventually, you'll want to add
> a feature that clashes with that invariant.

This is exactly what `--baseline` is for: we fix the current level of violations, new ones
break CI, and obsolete rules are explicitly lifted via config.

### Theme 8: "Agents rely on the style of existing code"

A practical takeaway from several independent comments: the best way to convey
architectural expectations to an agent is not rules in markdown, but **examples in the code**.
The agent reads "how it's already done" and reproduces the pattern. archcheck is the
guarantee that these patterns are actually followed and do not "drift."

---

## Connection to archcheck

The paper and the discussion together provide three arguments:

1. **Empirical:** constraint decay is measured — 30 pp. Not intuition, not anecdote.
2. **Mechanistic:** CLAUDE.md / the prompt decay; a CI check does not.
3. **Practical:** the HN community independently arrived at the conclusion "you need linter
   rules, not markdown" — archcheck is exactly that.

A short summary for marketing:
> "LLM agents are reliable for rapid prototyping but remain unreliable for production-grade
> development" (Dente et al., 2026) — archcheck is what makes production reliable.
