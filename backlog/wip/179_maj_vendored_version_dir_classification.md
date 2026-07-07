# [SCAN][CLASSIFICATION] 179: recognize vendored library dirs with a version+qualifier suffix (net-snmp-*-final-patched)

**Creation date:** 2026-07-07
**Start date:** 2026-07-07
**Status:** wip
**Module:** scan/file_classification
**Priority:** major
**Difficulty:** small (one classifier + a fixture + a test; no new subsystem)
**Blocks:** —
**Blocked by:** —
**Related:** #173b (agentic-weak-review corpus run that surfaced this), #068/#069/#109/#127/#164
(prior vendored-classification extensions — same SSOT), #056 (dedup spike consuming the same classifier)

## Goal

The #173b corpus archcheck run (2026-07-07, `experiments/agentic_weak_review/archcheck_run/`)
showed clone-drift counts inflated by vendored code that `file_classification.h` does **not**
currently recognize. Root cause verified per-path against the full graph-drift JSONL, not the
truncated `.md`:

| Repo | Clone-drift | Vendored/noise share | Gap |
|------|-------------|----------------------|-----|
| `carlkidcrypto/ezsnmp` | 2695 | ~75% | `net-snmp-5.6…5.10-final-patched/` dirs (1124) + giant amalgamated `ezsnmp_netsnmp.cpp`/`ezsnmp_sessionbase.cpp` (894) |
| `mla-c/mla-core` | 4240 | ~41% | `base-lib` embedded at 3 mount points (1733) |

Across all 22 repos in the run: **8284 clone-drift events, ~49.8% classifiable noise**
(net-snmp 13.6%, giant-amalgam 10.8%, base-lib mount 20.9%, cross-mount 4.5%). The raw signal
is not publishable until the classifier closes the clear gap.

## Scope — what this task fixes (and what it does NOT)

**IN scope — the net-snmp-style vendored-version dir (clear, safe, distinctive):**
`net-snmp-5.6-final-patched` normalizes to `netsnmp5.6finalpatched`. It is missed because:
1. `netsnmp` is not in `kVendoredLibDirs`.
2. The version-tail strip in `isVendoredDirName` only strips a **pure** `[0-9.]` tail
   (`zlib-1.3.2` → `zlib`); it fails on the `-final-patched` non-numeric qualifier.

Fix: add `netsnmp` to `kVendoredLibDirs` **and** generalize the version strip so a dir of the
form `<known-vendor-token>-<version>[-<qualifier>...]` is recognized — but **only when the stem
before the first digit-run is already a known vendored token**, so no new over-exclusion surface
is opened (a `libfoo-2d-renderer/` whose `libfoo` is not vendored stays scanned).

**NOT in scope / explicitly rejected:**
- **`base-lib` / `baselib` as a vendored token.** Too generic — many projects' own primary code
  lives under a `base`/`base-lib` dir; banning it would silently hide real findings (the exact
  over-exclusion trap already documented in `file_classification.h` for `units`, SPDX/#081). The
  mla-core case is intra-repo self-embedding, better addressed by cross-mount dedup, not a token.
- **Giant amalgamated files** (`ezsnmp_netsnmp.cpp`, 7900+ lines, normal avg line length so
  `hasMinifiedContent` misses them). Real but a *separate* heuristic decision (line-count or
  SWIG/amalgamation banner detection); do not fold it into this task. Note it in DEBT if not
  spun out.

## Detailed instruction

1. Read `include/archcheck/scan/file_classification.h` §"Vendored code exclusion" and the
   existing version-tail logic in `isVendoredDirName` (currently ~lines 180–196).
2. Add `netsnmp` to `kVendoredLibDirs`.
3. Generalize the version-tail strip: strip a trailing `-<digit>...` segment group (version +
   any alpha qualifier) and re-test the stem against `isVendoredLibDirName`. Keep the existing
   pure-numeric path working. Guard: stem must be a known vendored-lib token — never a bare
   substring/prefix match on arbitrary names.
4. Confirm the fix reaches clone-drift: `new_clone_drift.cpp` sources via
   `SourceSnapshot::authoredSources()`, which applies this classifier — so no wiring change is
   needed, only classification.

## Fixtures are mandatory

- `fixtures/vendored_version_dir/pass/` — an authored dir that must **still** be scanned, e.g.
  `libfoo-2d-renderer/foo.cpp` (stem not a vendored token) and the self-project guard case
  (running on the `net-snmp` repo itself must not vendor-drop its own tree).
- `fixtures/vendored_version_dir/fail_netsnmp/` — `src/net-snmp-5.8-final-patched/snmpwalk.cpp`
  that must be classified vendored (dropped from every signal).

## Control cases (contract)

| Check | Expected |
|-------|----------|
| `isVendoredDirName("net-snmp-5.8-final-patched")` | true |
| `isVendoredDirName("net-snmp-5.10-final-patched")` | true (double-digit minor) |
| `isVendoredDirName("libfoo-2d-renderer")` | false (stem not vendored) |
| self-project guard: scan root named `net-snmp` | own tree NOT vendor-dropped |
| dogfood: archcheck on `src/ include/ tests/` | still 0 violations |

## Definition of done

net-snmp-style version dirs classify as vendored; both fixtures pass; unit test added; dogfood
clean; `lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/` clean. No commit
without explicit `/commit`.

## Do NOT do

- Do NOT add `baselib` or any generic-name token to the vendored lists.
- Do NOT add substring/prefix matching on arbitrary dir names (over-exclusion).
- Do NOT re-run the #173b corpus here; re-running to refresh the clone numbers is a separate
  step once this ships.

## Changed files (planned)

| File | Change |
|------|--------|
| `include/archcheck/scan/file_classification.h` | `netsnmp` token + generalized version-tail strip |
| `fixtures/vendored_version_dir/**` | new pass + fail_netsnmp fixtures |
| `tests/…` | unit test for the new classification cases |

## Progress (2026-07-07)

Implemented. Both code changes landed in `file_classification.h`:
1. `netsnmp` added to `kVendoredLibDirs` (size 15 → 16).
2. `isVendoredDirName` version-tail strip generalized: take the stem before the first
   digit run, require that run to be a **dotted** version (undotted `zlib2` stays a name),
   and require the stem to be a known vendored-lib token (exact match, never a prefix).
   This subsumes the old pure-numeric path (`zlib-1.3.2`, `freetype-2.13.0` still pass)
   and adds the trailing-alpha-qualifier case (`net-snmp-5.6-final-patched`).

**Gates:** 638/638 tests pass (seeds 1/12345/99999); `[vendor]` 119 assertions / 29 cases
green; dogfood `src/ include/ tests/` → 0 violations; lizard clean.

**Tests + fixtures.** On-disk fixtures added per the DoD (user asked for literal dirs),
each a distinct scan root consumed by `tests/integration/graph/vendor_exclude_test.cpp`:
- `fixtures/vendored_version_dir/pass/libfoo-2d-renderer/foo.cpp` — authored lookalike,
  non-vendored stem → stays in the graph.
- `fixtures/vendored_version_dir/pass_self_project/net-snmp/net-snmp-5.8-final-patched/snmpwalk.c`
  — built with root = `net-snmp` so the scan root's own name exempts the same-token
  versioned subtree (self-project guard) → stays.
- `fixtures/vendored_version_dir/fail_netsnmp/src/net-snmp-5.8-final-patched/snmpwalk.cpp`
  → dropped; `src/app.cpp` authored contrast stays (proves drop ≠ empty graph).

Unit path-string cases (`tests/unit/scan/file_classification_test.cpp`) cover the raw
predicate: net-snmp true, double-digit minor, `libfoo-2d-renderer` false, self-project guard.

Global-state note: the self-project e2e sets `selfProjectDir=netsnmp` (root basename) via
`DiskFileSource`; it resets to `{}` at case end so the token collision can't leak into the
net-snmp unit test under Catch2's randomized order (verified across 3 seeds).

**Not done (out of scope, as specified):** giant amalgamated files
(`ezsnmp_netsnmp.cpp`) — separate heuristic; note in DEBT if it won't be spun out.
Corpus re-run to refresh clone numbers is a separate step post-merge.
