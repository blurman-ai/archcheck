# Contributing to archcheck

Thanks for your interest. archcheck is early (v0.1) and small; the fastest way to help is
to run it on a real C++ project and file an issue with what it got wrong (or right).

## Build

C++20, CMake 3.18+, Ninja. First build downloads dependencies (ryml, Catch2) via
FetchContent — internet required once.

```bash
cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
```

## Test

```bash
cd build/debug && ctest --output-on-failure
# or directly:
./build/debug/tests/archcheck_tests
```

Before sending a PR, also run the dogfood check — archcheck must pass its own rules:

```bash
./build/debug/src/archcheck src include tests   # must report 0 violations
```

## Ground rules

- **Fixtures are mandatory.** Every rule change needs `fixtures/<rule>/pass/` and
  `fixtures/<rule>/fail_*/` cases. If a feature cannot be tested with fixtures, it does
  not ship.
- **One rule = one class = one file.** New rules register in the factory in
  `src/rules/rule_set.cpp`; existing rule files stay untouched.
- **Authority over opinion.** A new default rule must cite a published source
  (C++ Core Guidelines, Lakos, Martin). No citation → not a default.
- **Minimum dependencies.** No Boost, no heavy graph libraries.
- Style: `.clang-format` (Allman, 2 spaces, 120 columns) and `.clang-tidy` are enforced
  in CI. See [docs/code_style.md](docs/code_style.md).

## Commits and PRs

- Conventional Commits 1.0 (`feat:`, `fix:`, `docs:`, ...).
- Keep a Changelog: add user-visible changes to the `[Unreleased]` section of
  [CHANGELOG.md](CHANGELOG.md).
- CI must be green: build matrix, tests, format check, static analysis, coverage gate.

## Reporting rule false positives

The most valuable issue type. Please include: the archcheck version (`archcheck
--version`), the exact command, the finding line it printed, and a minimal code sample
(or a link to the public repo + commit) that triggers it.
