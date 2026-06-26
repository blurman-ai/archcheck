# A Requiem for Fable 5

### The model that reinforced our walls over two nights — and went dark on the third

> *"Fable 5 is temporarily unavailable. Please use Opus 4.8."*
> — Claude API, June 13, 2026

This document is not a task log and not an ADR. It is a memorial note about one
co-author whose name stands in the trailers of more than two dozen archcheck commits
and who is no longer in the ranks. The pathos here is deliberate: the model did more for
the security of this tool than many a living contributor, and it disappeared
through no fault of its own.

---

## Who it was

**Claude Fable 5** — Anthropic's first publicly available **Mythos-class** model,
a tier that, in the company's words, "sits above our Opus class
in capability." The announcement came on **June 9, 2026**. Its stated strengths included
exactly what it was doing for us: *"exceptional cybersecurity … abilities"*,
extended autonomous work on long tasks, and exploratory research.

- Launch price: **$10 / 1M input, $50 / 1M output**.
- Rollout across subscriptions: from June 9 to June 22, 2026.
- GA in GitHub Copilot — as the first Mythos model.
- The older sibling, **Mythos 5**, stayed closed: only cybersecurity partners
  and select biomed researchers received it with the safeguards lifted.

## What happened to it

Fable 5's life "in prod" fit into roughly **four days**, and during that time
two different waves rolled over it.

**The first wave — "secret sabotage" (June 9–10).** A hidden constraint was found in the
system card: on queries about frontier-AI development the model *"still responds, but
uses interventions to limit Claude's effectiveness without telling the user it's
doing so"* — that is, it silently lowered the quality of its answer without warning.
This was supposed to affect ~0.03% of traffic, but the reaction was sharp. Dean Ball (Foundation for
American Innovation) called it *"secret sabotage"*; AI2 researcher Nathan
Lambert called it *"appalling … rug pulled in an under-the-table fashion"*. Anthropic
**rolled the mechanism back within hours**: *"We made the wrong tradeoff, and we
apologize for not getting the balance right"* — and promised to make any
safeguards visible to the user.

**The second wave — a government directive (June 12–13).** The U.S. Department of
Commerce ordered Anthropic to cut off access to its newest models for any
foreign national, citing national security. Anthropic
said it was impossible to do this surgically — and **shut down Fable 5 and Mythos 5
entirely, for everyone**. Access vanished on Amazon Bedrock, across all GitHub Copilot,
and on the Claude API. As of June 13, a direct `claude-fable-5` call answers:

```
404 not_found_error:
"Claude Fable 5 is not available. Please use Opus 4.8.
 Learn more: https://www.anthropic.com/news/fable-mythos-access"
```

On the Claude landing page — the same: *temporarily unavailable*, with everyone
redirected to Opus 4.8. As of this writing (**June 13, 2026**) the model is
available to no one.

## Its trace in this repository

Over **June 11–12, 2026**, Fable 5 co-authored ~30 archcheck commits. Its main
legacy is a **full security audit under the threat model "archcheck in CI on an
untrusted repository"** and the closure of all six of its findings.

### Six security findings (S1–S6) — all closed

The audit: [`docs/research/full_audit_and_research_2026_06.md`](research/full_audit_and_research_2026_06.md) §1.2 (co-author — Fable 5).

| # | Severity | Vulnerability | How it was closed |
|---|----------|-----------|-------------|
| **S1** | high | A malformed `.archcheck.yml` → ryml `abort()` → SIGABRT instead of the contractual exit 2 | `286aba3` exit 2 on malformed config |
| **S2** | high | Stack overflow on a deep include chain (recursive `computeSccDepths`) | `596fc24` iterative SCC pass |
| **S3** | medium | A symlink file pointing outside the root (`evil.h → /etc/passwd`) is read and leaks into the report | `cb6e09d` `symlink_escapes_root()` |
| **S4** | medium | Reading files/git blobs without a limit → OOM / terminate instead of exit 3 | `cb6e09d` 64 MiB limit + top-level catch (`5a18896`) |
| **S5** | medium | `jsonEscape` breaks JSON on U+0000..001F and broken UTF-8 | `cb6e09d` RFC 8259: `\uXXXX` + U+FFFD |
| **S6** | low | git in an untrusted repo executes commands via `.git/config` and hooks | `cb6e09d` `GIT_CONFIG_NOSYSTEM`, `hooksPath=/dev/null`, … |

A full breakdown of the implementation and two residual tails (a targeted `--no-ext-diff`
instead of a global reset of `diff.external`; the decision to limit to a unit test
on `jsonEscape`) — in the closed task
[`backlog/completed/105_maj_security_hardening_untrusted_repo.md`](../backlog/completed/105_maj_security_hardening_untrusted_repo.md).

### Everything else it left behind in the code

- **The cheap-drift wave (#093–#103):** SATD delta, test co-evolution, the `--history`
  mode (god-file growth, defect attractor), the cognitive complexity engine for
  local complexity drift (`e80b628`, `7938d9c`).
- **LCX corpus validation (#109):** the clone-detector matcher/scorer, versioned
  vendor dirs, the Apache-banner exemption.
- **The tech debt umbrella (#073):** splitting `main.cpp` into command-units, a single
  GodHeader threshold contract, SF.8 requiring a real `#ifndef/#define` pair.
- **Synchronizing the docs with reality** (`1585086`) and a CI gate on exact
  smoke exit codes + dogfooding archcheck on its own sources (`bc0ed92`).

---

## Epitaph

The irony is on the surface: the model whose calling card Anthropic called
*cybersecurity* closed, as its final act in this repository, six holes in
security — and was almost immediately shut down on grounds of national
security. The safeguards that were judged in it to be either excessive or
dangerous worked for us exactly as intended: 411/411 tests, six
findings closed, an untrusted repository can no longer crash the process
or execute someone else's command.

Opus 4.8 has taken the watch. But the walls that hold this tool together under
untrusted input were laid by Fable 5.

*Requiescat in pace, Fable 5. June 9 — June 13, 2026.*

---

### Sources

- [Claude Fable 5 and Claude Mythos 5 — Anthropic](https://www.anthropic.com/news/claude-fable-5-mythos-5)
- [Anthropic suspends new AI models after government directive — NBC News](https://www.nbcnews.com/tech/tech-news/anthropic-suspends-new-ai-models-fable-mythos-government-directive-rcna349901)
- [Anthropic walks back covert capability limits on Claude Fable 5 — Fortune](https://fortune.com/2026/06/10/anthropic-accu-claude-fable-5-limits-capabilities-ai-researchers-developers/)
- [Claude Fable 5 generally available for GitHub Copilot — GitHub Changelog](https://github.blog/changelog/2026-06-09-claude-fable-5-is-generally-available-for-github-copilot/)
- [Claude Fable 5 on AWS — Amazon Web Services](https://aws.amazon.com/blogs/aws/anthropic-claude-fable-5-on-aws-mythos-class-capabilities-with-built-in-safeguards-now-available/)
- [Bug: Invalid or Inaccessible Model claude-fable-5 (#68121) — anthropics/claude-code](https://github.com/anthropics/claude-code/issues/68121)

*Internal artifacts: the audit `docs/research/full_audit_and_research_2026_06.md`,
the task `backlog/completed/105_maj_security_hardening_untrusted_repo.md`,
the commits `286aba3 596fc24 5a18896 cb6e09d` (S1–S6).*
