# Coverage constraints — archcheck

## Current thresholds

`scripts/check_coverage.sh` checks three metrics over `src/` only:

| Metric    | Threshold | Actual (2026-05) |
|-----------|-----------|------------------|
| Lines     | 90%       | ~96%             |
| Functions | 90%       | ~95%             |
| Branches  | 40%       | ~63%             |

## Why branches is stuck at 63%, and what you must not do about it

### Cause: gcov + C++ exceptions + lcov 1.13

Astra Linux 1.7 ships **lcov 1.13** (Debian Buster).

In C++ every call to a function that might throw produces two paths in the CFG:
- normal return → "covered" branch
- throw arc → "uncovered" branch

This affects `std::string::push_back`, `operator<<`, `vector::reserve`, etc.
Physically reaching these branches in tests is impossible — OOM cannot be simulated.

**lcov ≥ 1.15** has the `--exclude-unreachable-branches` flag, which removes this noise.  
**lcov 2.x** (current upstream, v2.4) has `--filter exception`.  
Both versions require Perl dependencies (`DateTime`, `Capture::Tiny`) that are not in the
standard Astra Linux 1.7 repository.

### Alternative: gcovr

`gcovr` (Python, `pip3 install gcovr`) has `--exclude-throw-branches`.
Suitable for CI on Ubuntu 22.04+ / Debian Bookworm, but requires rewriting
`check_coverage.sh` for the gcovr format.

### What was checked

- lcov 2.4 was downloaded (Perl scripts from GitHub), `libcapture-tiny-perl` and
  `libdatetime-perl` were installed. Run with `--filter exception` — the flag exists, but
  on data from GCC 8.3 it had no effect: branches stayed at 61–62%
  (gcov 8.x does not mark throw-arcs in a way that lcov 2.4 recognizes).
- Conclusion: without a toolchain upgrade (gcc ≥ 10, lcov ≥ 1.15 or gcovr)
  getting past ~65% branches is not mechanically achievable.

## What can already be raised now

`MIN_LINES=90` and `MIN_FUNCTIONS=90` — already pass, raised.  
`MIN_BRANCHES` stays at `40` until the environment changes.

## Where it runs

| Environment | Tool  | Invocation |
|-------------|-------|------------|
| Local (Astra, lcov 1.13) | lcov | `scripts/check_coverage.sh`, step 6 in `/commit` and `/autocommit` |
| CI (Ubuntu 24.04, gcc-13) | gcovr | job `coverage` in `.github/workflows/ci.yml` (hard gate) |

CI deliberately does NOT call `check_coverage.sh`: it is written for lcov 1.13, while apt on
Ubuntu 24.04 installs lcov 2.x, which fails on throw-arcs and mismatched
`--remove` patterns. gcovr is a single pip package with native `--fail-under-*` and
`--exclude-throw-branches`. **Keep the thresholds in both places identical (90/90/40);
this table is the source of truth.** Branches on gcovr are higher than lcov's ~63% (throw-arcs
cut out by `--exclude-throw-branches`), so the 40% threshold has a wide margin.

## When raising the lcov branch becomes feasible

When the local environment moves to:
- Ubuntu 22.04+ / Debian Bookworm (lcov 1.16+) — add `--exclude-unreachable-branches`
- or `pip3 install gcovr` + rewrite `check_coverage.sh` for gcovr (unify with CI)

## GCC 8 / C++20 stdlib gaps (Astra Linux 1.7)

### The gist of the problem

Astra Linux 1.7 ships **GCC 8.3**. The project is declared as C++20 (`-std=c++20`),
and GCC 8 accepts this flag — but implements far from the whole standard. Some C++20
stdlib features only appeared in GCC 10 (libstdc++ 10).

In CI (Ubuntu 24.04, GCC 13) and on any modern Linux these features work fine.
The problem only arises during a local build on Astra.

### Known missing features

| Feature | Available since | Workaround |
|---------|-----------------|------------|
| `std::string_view::starts_with(x)` | GCC 10 | `s.find(x) == 0` |
| `std::string_view::ends_with(x)` | GCC 10 | `s.rfind(x) == s.size() - x.size()` |
| `std::string::starts_with(x)` | GCC 10 | same |
| `std::string::ends_with(x)` | GCC 10 | same |

Full list of C++20 library features by compiler:
https://en.cppreference.com/w/cpp/compiler_support/20

### Marker for fast lookup when Astra is upgraded

**Rule:** every workaround is accompanied by a tag comment `// GCC8-COMPAT:`.

```cpp
// GCC8-COMPAT: starts_with() requires GCC 10; replace when Astra upgrades
if (path.find(prefix) == 0) // cppcheck-suppress stlIfStrFind
```

When moving to a new Astra (or when Astra upgrades the toolchain to GCC 10+):

```bash
grep -rn "GCC8-COMPAT" src/ include/ tests/
```

Will find all the places that need to be cleaned up.

### Current instances

| File | Line | Workaround | Target code |
|------|------|------------|-------------|
| `src/scan/include_resolver.cpp` | line with `path.find(prefix) == 0` | `find()==0` + cppcheck-suppress | `path.starts_with(prefix)` |

### Special case: `(void)::read()` on GCC 13

This is **not** a GCC 8 problem — the reverse: in CI (GCC 13, Ubuntu 24.04)
the `(void)` cast does not suppress `-Werror=unused-result` for `read()`.
The fix is `[[maybe_unused]] auto n = ::read(...)`.
The `GCC8-COMPAT:` tag is not used, this is CI-specific.

## LCOV_EXCL_* markers already in the code

In `src/git/git_state.cpp` and `src/git/git_object_file_source.cpp`,
`// LCOV_EXCL_START` / `// LCOV_EXCL_STOP` are placed around post-fork child-process code —
this is the only legitimate place for such annotations (the code is physically invisible to gcov
in the parent process). Annotating the other throw branches makes no sense until lcov changes.
