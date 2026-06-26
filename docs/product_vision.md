# archcheck — Product Vision

_2026-06-02_

## What we are building

`archcheck` is a **CI-first guardrail for architectural drift in C++**.

Not "who wrote the code", but:

- which `PR` added an unwanted dependency;
- which commit created a cycle;
- where coupling grew;
- where the team violated its own module boundaries.

The right framing:

- **Lakos physical design + drift regression in CI**
- **zero-config first, config-rules later**

The wrong framing:

- "AI drift detector"
- "universal quality platform"
- "yet another linter / bug finder"

## What is wrong right now

### 1. The roadmap overweights duplication

Right now `docs/ROADMAP.md` makes the duplication pass the main narrative of `v0.1`.
That is incorrect.

In fact:

- the graph/drift layer already looks like the product core;
- duplication ships as an **advisory reporting capability** (`--duplication`,
  report-only, exit 0) — it is a supported feature, not a preview/experiment;
- but its precision is not sufficient for a trusted CI gate, so it stays
  advisory and **does not block CI**.

So duplication is a supported advisory feature, but not the main blocker of the
product and not a trusted gate.

### 2. Product and the AI research are mixed together

The research has already shown:

- drift is real;
- the tool catches it;
- but the hypothesis "AI causes drift" is not yet proven.

It follows that:

- **the product cannot be positioned as an AI-decay detector**;
- **the AI-vs-human research is a separate track**, not the center of the core roadmap.

### 3. Shipped core and research branches are mixed together

Right now these live side by side:

- the real zero-config graph/drift core;
- an unfinished config surface;
- duplication spikes;
- the AI discovery/verification pipeline.

These are different maturity levels, but in the narrative they are mixed together
in places.

### 4. The next value layer is described poorly

From a product standpoint the next strong step is not simply to
"add more AST/libclang".

The next strong step is:

- **module boundary rules**
- `layers`
- `forbidden`
- `independence`

That is, checking **my architectural boundaries**, not just generic smells.

## What v0.1 should be

`v0.1` should be not a "duplication MVP", but:

## v0.1 — Trusted Drift Core

- zero-config include-graph checks;
- baseline / diff / drift;
- reliable intrinsic rules;
- a clear CLI contract;
- deterministic output;
- a convenient PR/commit workflow.

What belongs in it by meaning:

- `SF.7`, `SF.8`, `SF.9`, Lakos-style intrinsic checks;
- `DRIFT.1`, `DRIFT.2`;
- baseline save/load;
- `--diff`;
- text/json reporting;
- trust in the result.

What should NOT be the center of `v0.1`:

- a duplication gate;
- AI attribution;
- a synthesis loop;
- a "big semantic platform".

Duplication in `v0.1` ships as an **advisory reporting capability**
(`--duplication` — report-only, exit 0, a supported feature).

What it is NOT in `v0.1`:

- a blocking CI gate (it needs gate-grade precision — separate future work);
- the center of the `v0.1` narrative (the center is the trusted drift core).

## What v0.2 should be

## v0.2 — Boundary Enforcement

The main goal of `v0.2`:

- turn `archcheck` from a zero-config guardrail
- into a guardrail + **contractual module rules**.

So the focus of `v0.2`:

- `.archcheck.yml` as a real runtime feature;
- `layers`, `forbidden`, `independence`;
- proper CI integration;
- SARIF if needed;
- selective expansion of graph drift metrics:
  - fan-out growth
  - blast radius growth
  - coupling growth

`libclang` in `v0.2` is acceptable as a supporting backend, but not as the main
product story. It is needed only where it actually unlocks checkable value.

## What to freeze

Until the product core is clarified, do not pull into the center of the roadmap:

- duplication as a mandatory gate;
- AI rule synthesis;
- the "AI loop" as part of the main narrative;
- a plugin API;
- visualization;
- a regex-rule zoo;
- Martin metrics as a headline feature;
- a broad C-support story.

These can be kept:

- in `future/`;
- in `research/`;
- in a preview layer,

but not as the current center of the product.

## How to phrase the product correctly

Short formulation:

> `archcheck` is a C++ CLI for CI that catches architectural regressions in PRs:
> new unwanted dependencies, cycles, and module boundary violations.

Short formulation for the roadmap:

> First the trusted drift core. Then boundary rules. Then selective semantic expansion.

## What to fix in ROADMAP

- Remove the "phase: duplication pass" framing.
- Remove duplication from the role of the main `v0.1` narrative.
- Separate `product core`, `preview`, `research`.
- Explicitly state that AI-specific claims are not yet a product promise.
- Raise config boundary rules above generic semantic expansion.

## Practical priority

The order should be:

1. Finish cleaning up product contracts and alignment.
2. Strengthen the trusted graph/drift core.
3. Bring config-rules to a real runtime.
4. Duplication already ships as an advisory feature (`--duplication`); the next
   step for it is to raise precision to gate-grade, and only then consider it as
   a blocking gate.
5. Keep AI-attribution and synthesis as separate research.
