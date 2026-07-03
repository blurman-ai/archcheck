# openwiki — build log

Append-only. Newest entry at the bottom. No front matter (exempt in
`scripts/check_openwiki.py`'s `NO_FRONTMATTER`). Entries here are a record of
what was added and why — not sourced wiki claims; see the pages themselves for
those.

## 2026-07-03 — commit `241b07e` — pilot + fan-out (task #169)

- **Pilot** (vertical slice): `index.md`, `source-map.md`, `cli-contract.md`,
  and five default-rule pages under `rules/` (SF.7, SF.8, SF.9, Lakos
  god-headers, Lakos chain-length). Built to A/B-test whether source-backed
  navigation pages actually save an agent tokens/turns before committing to
  the full ~16-page wiki (see `index.md`'s "Pilot scope" note).
- **Fan-out**: extended coverage in parallel to drift rules (starting with
  `rules/drift_no_shortcut_edge.md`), scan / PR-hygiene signals, the
  duplication subsystem (`features/duplication.md` — a thin pointer into the
  existing SSOT, `docs/duplication_architecture.md`, not a re-documentation),
  and scaffolding pages: `schema.md` (promoting `index.md`'s mini-schema into
  a full page), `backlog/open-questions.md`, `backlog/unverified.md`, and
  `sources/external-openwiki-practices.md` (the external OpenWiki method this
  layer adapts).
- Current file set at any point is authoritative in the tree itself
  (`docs/openwiki/`) and in `scripts/check_openwiki.py`'s output — this log
  records intent and sequencing, not a live inventory.
