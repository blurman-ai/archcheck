# component-naming-prefix

- **Category:** P (filesystem scan)
- **Authority:** high — Bloomberg BDE
- **Source:** [BDE physical code organization](https://github.com/bloomberg/bde/wiki/physical-code-organization)

## Rule

> "Each component is named `packagename_componentname`. All files of the component share this prefix."

## Why for archcheck

An optional rule (behind a flag, not a default). Gives a firm binding of a file to a module without needing to describe `modules:` in YAML — the module is derived from the file name prefix. Useful for teams ready to adopt the BDE convention; for everyone else — disabled.

## Detection

Filesystem-scan: for each file, extract the prefix up to the first `_`. Group by prefix. If `--bde-naming` is enabled in the project, check that all files in a directory share the same prefix, and that the prefix matches the directory name.

## Fixtures

- `pass/` — `bdlt/bdlt_date.h`, `bdlt/bdlt_date.cpp`, `bdlt/bdlt_calendar.h`.
- `fail_no_prefix/` — `bdlt/date.h` (no prefix).
- `fail_wrong_prefix/` — `bdlt/bsls_assert.h` (prefix not from the current directory).
