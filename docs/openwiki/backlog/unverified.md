---
title: "openwiki — unverified candidate terms/features"
type: backlog
status: unverified
last_checked_commit: "241b07e"
source_paths:
  - "backlog/new/169_maj_openwiki_agent_context_layer.md"
related_pages:
  - "../index.md"
  - "../sources/external-openwiki-practices.md"
external_sources: []
---

# openwiki — unverified candidate terms/features

## Purpose

The original external spec (`~/Загрузки/task_create_openwiki_for_archcheck.md`,
referenced in `backlog/new/169_maj_openwiki_agent_context_layer.md`'s closing
note) was written for a generic, docs-less repo and names OpenWiki-tool
concepts that are **not** confirmed to exist or apply to archcheck. Parked
here per the "no guessing" rule instead of asserted as real anywhere else in
this wiki.

## Unverified items

- **OpenWiki CLI** (`npm install -g openwiki`, `openwiki --init`,
  `openwiki --update --print`) — never run in this environment; whether it
  exists, works offline, or produces anything sensible for a C++ repo is
  unconfirmed. The retargeted task explicitly defers it and forbids running a
  provider-calling tool without maintainer sign-off
  (`backlog/new/169_maj_openwiki_agent_context_layer.md:131-136`).
- **`.github/workflows/openwiki-update.yml`** (LLM-calling, scheduled/PR-generating
  update workflow) — not present in this repo; `.github/workflows/` contains only
  `ci.yml`, `example_archcheck_pr.yml`, `release.yml` (checked directly). Marked
  out of scope by the retargeted task
  (`backlog/new/169_maj_openwiki_agent_context_layer.md:137-140`).
- **`ingest` / `query` / `lint` as three first-class OpenWiki operations**
  (Karpathy LLM-Wiki framing) — this repo only has the `lint`-equivalent
  (`scripts/check_openwiki.py`, structural + staleness checks); no `ingest` or
  `query` command/tool was found or built.
- **Dedicated `openwiki/{glossary,overview,architecture,verification,workflows,
  decisions}/` subdirectories and `openwiki/README.md`** from the external
  spec's suggested layout — not created under `docs/openwiki/`. The retargeted
  task treats their content as already covered by existing docs
  (`docs/GLOSSARY.md`, `docs/architecture-spec.md`, `docs/decisions/`,
  `docs/ci_usage.md`, `docs/config_format.md`, `docs/baseline_format.md` —
  `backlog/new/169_maj_openwiki_agent_context_layer.md:19-37`), but no
  `docs/openwiki/`-namespaced equivalent exists — noted so a future agent
  doesn't go looking for e.g. `docs/openwiki/glossary/terms.md`.
- **`docs/openwiki/sources/repository-source-inventory.md`** — required by the
  external spec's acceptance criteria (a path-by-path inventory of every
  inspected root doc / entry point / test / CI file); not created in this
  repo and not in this pass's deliverable list; confirmed absent
  (`find docs/openwiki -iname "*inventory*"` → no match).

## Related pages

- [[../index.md]] · [[../sources/external-openwiki-practices.md]]
