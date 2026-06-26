# header-cpp-pairing

- **Category:** P (filesystem scan)
- **Authority:** medium — Bloomberg BDE
- **Source:** [BDE physical code organization](https://github.com/bloomberg/bde/wiki/physical-code-organization) — each component = a `.h`/`.cpp` pair.

## Rule

> "Each `.h` must have a corresponding `.cpp` with the same base name, except for explicitly header-only files."

## Why for archcheck

Indirectly supports SF.5 (`.cpp` must include its own `.h`) and SF.11 (header self-contained). If a `.h`/`.cpp` pair always exists, the user won't drift into "we decided on header-only, but then added the implementation somewhere else anyway." Behind a flag, not a default: the rule does not apply to template-heavy / header-only libraries.

## Detection

Filesystem-scan: collect all base names in each directory. For each `.h`, check for a paired `.cpp`. Exceptions:
- a file is explicitly header-only — determined via the comment marker `// archcheck: header-only` in the first N lines, or via a regex on the name (`*_inl.h`),
- directory categories marked in the config as `header_only: true`,
- if a `.h` contains only a `class declaration` without definitions — allowed (then a `.cpp` may not be needed; checked via AST or heuristically).

The pairing check also covers `.cpp` without `.h` (SF.5 already partially catches this).

## Fixtures

- `pass/` — each `.h` has a `.cpp`.
- `pass_header_only/` — `concepts.h` with the marker `// archcheck: header-only`.
- `fail_orphan_header/` — `foo.h` without `foo.cpp` and without a marker.
- `fail_orphan_cpp/` — `bar.cpp` without `bar.h`.
