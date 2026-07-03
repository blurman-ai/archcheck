---
title: "CLI contract — modes, flags, exit codes"
type: cli
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/main.cpp"
  - "src/cli/check_command.cpp"
  - "src/cli/diff_command.cpp"
  - "src/cli/preview_commands.cpp"
  - "README.md"
  - "AGENTS.md"
related_pages:
  - "index.md"
  - "source-map.md"
external_sources: []
---

# CLI contract

## Purpose

The stable surface: modes, the main flags, and the exit-code contract. Values here
are the public contract (README §Usage, AGENTS.md "CLI contracts"); the
implementation is `src/main.cpp` + `src/cli/*`. Thresholds and the shipped rule set
are **not** restated — see [CHANGELOG.md](../../CHANGELOG.md) and `--help`.

## Exit codes (contract — don't change without versioning)

| Code | Meaning |
|---|---|
| `0` | OK (no gated finding; advisories may still be printed) |
| `1` | gated violation found |
| `2` | config / parsing error |
| `3` | internal error |

Source: AGENTS.md "CLI contracts", CLAUDE.md "Exit codes". `1` means a **gated**
finding, not merely "something was reported" — advisory findings print and still
exit `0` (README §What it does).

## Modes / flags

Findings print as `file:line: [rule] message` (README:44). Implemented across
`src/cli/`: default check (`check_command`), diff (`diff_command`), previews
(`preview_commands`).

| Invocation | What it does | `src/cli` handler |
|---|---|---|
| `archcheck <paths>` | Scan + default rules (fast preprocessor backend) | `check_command` |
| `archcheck --format json\|md <paths>` | Machine-readable report | `check_command` / `report/json_reporter` |
| `archcheck --diff <revspec> .` | PR workflow: new/grown cycles + new god-headers gate; other signals advisory | `diff_command` |
| `archcheck --save-baseline <f> <paths>` / `--baseline <f> <paths>` | Freeze / apply a violation baseline | `report/violation_baseline` |
| `archcheck --save-graph-baseline <f> <paths>` / `--drift-baseline <f> <paths>` | Pin / compare a graph snapshot (drift) | `graph/baseline`, `diff/` |
| `archcheck --config .archcheck.yml <paths>` | Load config (parsed + validated; enforcement v0.2) | `config/config_loader` |
| `archcheck --duplication <paths>` | Token-based clone report (advisory) | `scan/duplication/` |
| `archcheck --history <paths>` | God-file growth + defect-attractor analytics (report-only) | `git/history_query`, `scan/god_file_growth` |
| `archcheck --scan <paths>` / `--graph <paths>` | Scan / graph preview | `preview_commands` |
| `archcheck --version` / `--help` | Version / usage | `main.cpp` |

> The exact flag grammar and defaults live in `--help` and README §Usage. If this
> table disagrees with `--help`, `--help` wins — update this page.

## For an agent

- Changing an exit code, flag name, or the `file:line: [rule] message` format is
  `Risk: high` (contract change) — needs a version bump; see `docs/dev/git_workflow.md`.
- Mode dispatch lives in `src/main.cpp` → `src/cli/*`. Reporter format lives in
  `src/report/{text_reporter,json_reporter}.cpp`.
- The `--diff` gate/advisory split is decided in `src/rules/gate_policy.cpp` (see the
  README gate table for the authoritative classification).

## Related pages

- [[index.md]]
- [[source-map.md]]
