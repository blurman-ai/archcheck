# Two-day digest: agentic review, drift, and copy-paste

_Prepared 2026-07-07 for article drafting. This is a compact, paste-ready summary
of the 2026-07-06..2026-07-07 research pass._

## Short thesis

The honest finding is not "AI reviewers miss all architectural problems". It is narrower
and stronger:

> AI review is useful at line-level correctness and sometimes catches DRY/duplication issues,
> but it does not systematically operate on the include-graph / complexity / history plane.
> archcheck is complementary to AI review: bots review the diff; archcheck checks the graph
> and main-history drift.

## What was tested

Two research directions were joined:

1. **Gate moderator (#174):** does a weak human gate amplify agent-authored architectural drift?
2. **Agentic weak-review corpus (#173/#173b):** do agent-authored PRs reviewed by AI bots carry
   archcheck drift, especially copy-paste / complexity / graph regressions?

## Main numbers

- Agentic weak-review corpus after enrichment: **16,982 usable PRs** across **1,636 repos**.
- Seed corpus: **485 PRs / 29 repos**, all authored by `app/copilot-swe-agent`.
- Enriched seed: **108 / 485** PRs have an AI reviewer present.
- Repo-selection table: **49 repos** with agent-authored PRs touching real C/C++ files.
- Clone-and-run sample: **36 repos** selected; **34/36** cloned; **25/36** produced complete
  per-commit data.
- Closed-loop search result: **0 confirmed cases** of
  `agent-authored commit + growth/clone/complexity signal + AI-reviewer present`
  in the completed per-commit sample.
- Among **104 agent-reviewed PRs** in repos with drift replay:
  **0 grown cycles**, **0 new cross-module/layer dependencies**.

## Gate moderator result

The hypothesis "agent-authored commits drift more when they pass through a weak gate" was **not
supported**.

The raw table looked suggestive: agent drift appeared more weakly gated than human drift. But the
denominator killed that interpretation. Agent commits route through weak/non-blocking gates around
90% of the time regardless of drift. Agent drift was not routed through a weaker gate than the
agent baseline. This is composition, not interaction.

In short:

- agent workflow often has a weak/non-blocking gate by default;
- drift is not specially slipping through a weaker gate;
- weak gate is a property of the workflow, not an observed amplifier of drift in this corpus.

## AI review vs archcheck plane

The useful distinction:

- AI review sees **line-level / local diff** problems:
  build breaks, ODR hazards, missing/unused includes, memory leaks, wrong fields, deadlocks,
  suspicious scope creep.
- archcheck sees **graph / complexity / history** problems:
  include cycles, new edges, cross-area dependencies, local-complexity growth, clone growth,
  and direct-to-main drift that never went through a PR.

This produced the "disjoint planes" result: AI review can be competent and still miss the
structural metric archcheck reports.

## Best agentic-corpus examples

### m1m1r.kit #5

`ListScreen::onRender` local complexity grew **27 -> 50**. Copilot reviewed the PR and made real
comments about sprite rendering bugs, unchecked `createSprite()`, and an unused variable. It did
not mention the complexity jump.

This is the cleanest example of "the bot read the file, but not the metric".

### m1m1r.kit #3

The PR added `GaldrCore::_cmdLcd` at complexity **32**, plus new include edges and clones.
Copilot left several useful bug comments: wrong field name, removed config key still referenced,
busy-wait/deadlock, underflow-style delay issue. It did not name the dependency fan-out,
complexity, or clone signal.

### NetDiagnostics #8

Copilot noticed scope creep in the exact Android file where archcheck later saw local-complexity
growth. But it framed the issue as review/scope mismatch, not as complexity or maintainability
regression.

## Copy-paste finding

Copy-paste exists in the agentic corpus, but it is weaker evidence than cycles/cross-area drift.

Why:

1. AI reviewers sometimes catch DRY/duplication directly. In `m1m1r.kit` #1/#4, Copilot flagged
   duplicated layout/theme constants and hardcoded colors.
2. Some clone signals are benign or mechanical: RX/TX mirrors, platform ports, protocol boilerplate,
   build-fix headers.
3. There was real noise from path-renames / vendored-like structures. In `mla-core`, a large part of
   the apparent drift was the same base-lib being moved through `framework -> base-lib -> lib/base-lib`.
   That produced repeated "new" edges and motivated #179.
4. The agentic corpus did not produce a strong same-PR case of:
   "AI authored PR, AI reviewer reviewed it, PR introduced a grown cycle or new cross-module edge,
   reviewer missed it."

So the fair copy-paste line is:

> The agentic corpus contains copy-paste and complexity growth, and AI reviewers do not consistently
> name those metrics. But duplication is mixed evidence because bots sometimes catch DRY issues and
> some clone hits are mechanical. The stronger product evidence remains include cycles and cross-area
> drift from the trending corpus.

## Stronger evidence from the trending corpus

The trending-corpus result remains the load-bearing evidence:

- Start from archcheck-found drift events, not from PRs.
- Among **38 AI-reviewed PRs** that introduced a new/grown include cycle or new cross-area
  dependency, bots structurally engaged in only **2**.
- The rest mostly received line-level comments while the architectural drift merged.
- **0** were blocked by the AI reviewer.

The best showcase is `PrincetonUniversity/SPECFEMPP#943`: the PR was framed as cutting circular
dependencies, the AI review accepted that framing, but archcheck found a new 7-node include cycle.

## Article-safe wording

Use this:

> AI review bots are useful reviewers of the textual diff. They catch real bugs, sometimes even
> duplicated constants or include hygiene. But they do not reliably build the dependency graph,
> track complexity drift, or see direct-to-main history. In our corpus, copy-paste appears in
> agentic code, but the cleaner evidence for an architectural blind spot is cycles and cross-module
> drift: the bot reviews the lines, archcheck checks the structure.

Avoid saying:

- "AI reviewers never catch duplication" — false; they sometimes do.
- "Weak review causes agent drift" — not supported by #174.
- "The agentic corpus proves bots let cycles through" — it did not; the 104 reviewed PRs had
  zero grown cycles / cross-module dependencies.
- "Copy-paste is the strongest evidence" — it is present, but mixed; cycles/cross-area drift are
  stronger.

