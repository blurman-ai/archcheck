# Agent-operable cells

Applies the "agent-operable architecture" hypothesis (hand + eye + laws + closure protocol,
per the author's article) to archcheck's own dev workflow. Most of the machinery already
existed unnamed; this doc names four cells, points each at its eye command, and states the
laws a cell may never touch. Closure protocol (the 5 verdicts) is in
[haiku_task_guide.md](haiku_task_guide.md) §2.5 — read both together.

The eye for any cell that touches `src/`/`include/`/`tests/` is one command:

```bash
scripts/agent_evidence.sh <label>
```

It runs the exact gates from [`.claude/commands/commit.md`](../../.claude/commands/commit.md)
steps 3–6 (format, cppcheck, lizard, build, tests, self-check, coverage, smoke) and prints one
≤25-line before/after-comparable block. `EVIDENCE_FAST=1` skips coverage for the inner
iteration loop — see the Goodhart law below before using a fast-mode block to close anything.

## Cell passports

| Cell | Allowed | Forbidden | Eye | Escalate when |
|---|---|---|---|---|
| **new-default-rule** | `src/rules/<rule>.cpp` + `include/archcheck/rules/<rule>.h` (new pair), ONE registration line in `src/rules/rule_set.cpp`, `tests/unit/rules/<rule>_test.cpp`, `fixtures/<rule>/pass/`, `fixtures/<rule>/fail_*/` | `src/graph/`, `src/scan/`, `src/report/`, any other file under `src/rules/`, `include/archcheck/rules/violation.h`, `include/archcheck/rules/i_rule.h`, exit codes, CLI flags | `scripts/agent_evidence.sh new-default-rule`, plus by hand: fixture `pass/` scans clean, every `fail_*/` trips the new rule's id | the rule needs data not exposed by `IRule::check`'s inputs (a graph/scanner API change); no fixture can reproduce the target pattern; the default threshold has no Core Guidelines/Lakos/Martin citation |
| **test-fixture-only** | `tests/**`, `fixtures/**` (including `tests/CMakeLists.txt` to register a new test file — the target list is explicit, not globbed) | `src/**`, `include/**` | `scripts/agent_evidence.sh test-fixture-only` | the test/fixture exposes a real product defect — that fix is a different cell (`new-default-rule` or a bugfix), open it as a separate task instead of expanding scope here |
| **reporter-change** | `src/report/**`, `include/archcheck/report/**`, `tests/unit/report/**` | `src/rules/**`, `src/graph/**`, exit-code dispatch in `src/main.cpp`, `include/archcheck/rules/violation.h` | `scripts/agent_evidence.sh reporter-change`, plus by hand: diff `--format text` and `--format json` output on one fixture before/after | the change needs a new field on `Violation` (crosses into the rules/graph contract) or changes exit-code mapping |
| **research-script** | `experiments/**` (gitignored), companion-repo `archcheck-journal/analysis/**` | `src/**` entirely | no build gate (no `src/` touched) — the self-checking rules in the root [CLAUDE.md](../../CLAUDE.md) §"Self-checking conclusions": spot-check 2–3 cases by hand before any conclusion | a finding implies a product change — that's a `new-default-rule` or `reporter-change` cell, not this one |

A cell boundary is a subset of "allowed", never a superset inferred from convenience — if a
change needs one file outside the list, that alone is an escalation trigger, not a judgment
call to make silently (same rule already in
[haiku_task_guide.md](haiku_task_guide.md) §2.3).

**The write-boundary is not a read-boundary.** `test-fixture-only` forbids *writing* to
`src/`/`include/`, but writing a fixture that actually exercises real behavior still requires
*reading* the production code the fixture targets (#183 pilot: writing four correct
`vendor_exclusion` fixtures needed reading `file_classification.h`, `authored_scope.h`, and
`source_snapshot.h` in full — a guessed fixture would have tested nothing). "Bounded action
surface" bounds what an agent may change, not what it must understand to change it correctly.

## Global laws

| Law | Where enforced |
|---|---|
| Exit-code contract (0 ok / 1 violations / 2 config-err / 3 internal) | `tests/integration/cli/cli_smoke_e2e_test.cpp` (#133); `smoke:` line of the evidence block |
| Coverage ≥ 90% lines / 90% functions / 40% branches | `scripts/check_coverage.sh`, CI job `coverage`; `coverage:` line |
| Complexity: lizard CCN ≤ 15, NLOC ≤ 30, args ≤ 5 | `/commit` step 4; `lizard:` line |
| Formatting: clang-format 18.1.3, clean tree | `/commit` steps 3–4, CI `format-check`; `format:` line |
| Static analysis: cppcheck clean (warning/performance/portability) | `/commit` step 4, CI `static-analysis`; `cppcheck:` line |
| Self-check: 0 violations on `src include tests` | CLAUDE.md dogfooding mandate; `self-check:` line |
| code_quality budgets (≤ 50 new lines/commit, ≤ 2 new files, 0 unrequested abstractions) | [docs/code_quality.md](../code_quality.md) — **manual self-check only, not machine-gated** |

## The Goodhart law (non-negotiable)

An agent must never weaken a fixture, delete or relax a test assertion, regenerate a
baseline, or lower a threshold in order to make a gate pass. Any change to the cell's own
laws — this document, `scripts/`, thresholds, fixtures belonging to a *different* rule,
baselines — is **ESCALATE**, always, regardless of how small it looks. A cell may not modify
the laws that grade it.

**Fast mode is not evidence for closure.** A block produced with `EVIDENCE_FAST=1` (visibly
tagged `[FAST — not ACCEPT-eligible]` in its header) is for the inner iteration loop only —
it never has a coverage line. Citing a fast-mode block to justify `ACCEPT` is exactly the gap
this law exists to close: only a full run (no `EVIDENCE_FAST`) may close a cell.

**Missing tool is not clean.** If a gate's tool isn't found (pinned clang-format, cppcheck,
lizard, cmake), the evidence line reads `ERROR (tool missing: ...)` and the block exits
non-zero — never a silent `clean`.
