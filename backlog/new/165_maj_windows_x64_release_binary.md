# [RELEASE] Windows x64 release binary

**Created:** 2026-07-02
**Status:** new — planned for the post-launch phase (decision 2026-07-02, #163); do NOT
start before the launch announcement ships.
**Module:** RELEASE / CI
**Priority:** major
**Complexity:** M
**Blocks:** Windows-shop adoption (MSVC C++ CI audience)
**Blocked by:** #163 launch (deliberately sequenced after)
**Related:** #142 (Linux prebuilt binary), #166 (macOS arm64), #163 (launch plan)

## Goal

Add a Windows x64 asset to the tag-triggered release workflow so `archcheck` is
installable on Windows CI runners without building from source.

## Scope

- `release.yml`: a `windows-2022` job — MSVC build (CMake+Ninja or default generator),
  Release config, `archcheck-<v>-windows-x64.zip` + `.sha256`, `--version`-matches-tag
  gate, smoke job on a clean `windows-latest` runner.
- Static linking of the C++ runtime (`/MT`) so the binary runs without a VC++
  redistributable — verify with a dependency walk in the smoke job.
- CI build matrix: add an MSVC compile job to `ci.yml` FIRST (the codebase has never
  been compiled with MSVC — expect a porting tail: `<filesystem>` quirks, popen/fork in
  `src/git/`, path separators). The `git/` subsystem shells out to git — verify it works
  with Git for Windows on the runner.
- Docs: install snippet (PowerShell) in `docs/ci_usage.md` + README Install section.
- CHANGELOG entry.

## Acceptance

- [ ] Tag push produces a checksummed Windows x64 zip; smoke job passes on a clean
      runner (`archcheck --version`, a fixture scan, a `--diff` run on a test repo).
- [ ] MSVC job green in `ci.yml` (tests pass on Windows).
- [ ] Install docs updated; ROADMAP "planned platforms" note flipped to shipped.

## Do not do

- Do not gate the initial public launch on this (explicit decision 2026-07-02).
- Do not add a vcpkg/conan manifest here — distribution channels are #163 Phase-1 scope.
