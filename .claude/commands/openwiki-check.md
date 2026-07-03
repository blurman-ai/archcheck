Check the `docs/openwiki/` agent wiki for structural problems and doc↔code drift.

A manual, deterministic lint (no LLM, no network). Run it whenever you like — e.g.
each evening, or before committing wiki changes.

## Run

```bash
python3 scripts/check_openwiki.py          # warnings don't fail the run
python3 scripts/check_openwiki.py --strict # stale/orphan also fail (exit 1)
```

Exit `0` = clean; `1` = at least one structural error (or a warning under `--strict`).

## What it checks

- **front matter** — every page (except `log.md`, append-only) has `title`, `type`,
  `status`, `last_checked_commit`, `source_paths`; `status ∈ {verified, partial, unverified}`;
  a `verified` page must have non-empty `source_paths`.
- **`[source]`** — every `source_paths` entry exists on disk.
- **`[link]`** — every inline `](x.md)` link resolves.
- **`[orphan]`** — every page is reachable from `index.md` (`log.md`, `backlog/*`,
  `sources/*` are exempt).
- **`[stale]`** — the drift signal: for each page, `git diff <last_checked_commit>..HEAD`
  over its `source_paths`; if the cited code changed since the page was last checked,
  the page is flagged for re-verification.

## Reading the output

- **`FAIL [source]` / `FAIL [link]` / `FAIL [frontmatter]` / `FAIL [status]`** — hard
  errors. Fix the page (correct the path, the link, or the metadata) before committing.
- **`WARN [orphan]`** — link the page from `index.md` (or a page reachable from it),
  or add it to an exempt area if it's intentionally standalone.
- **`WARN [stale]`** — open the page, re-verify each claim against the changed source,
  update anything that drifted, then bump the page's `last_checked_commit` to the
  current `git rev-parse --short HEAD`. The bump is what clears the warning.

## Optional deep re-verification

For a `[stale]` page you can hand the re-check to an agent: point it at the page and
the changed files (`git diff <last_checked_commit>..HEAD -- <source_paths>`) and have
it re-verify each `## Verified facts` bullet, quoting the current literal from code.
Keep the same evidence discipline: no claim without a `path:line`; unverifiable →
move it to `backlog/`, don't invent.

## Notes

- The check is read-only; it never edits the wiki.
- `last_checked_commit` is the contract that makes staleness detectable — always set
  it (and bump it) to a real short SHA when you verify a page.
