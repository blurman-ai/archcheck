# [RELEASE] macOS arm64 release binary

**Created:** 2026-07-02
**Status:** new — planned for the post-launch phase (decision 2026-07-02, #163); do NOT
start before the launch announcement ships.
**Module:** RELEASE / CI
**Priority:** major
**Complexity:** M
**Blocks:** local developer adoption on Apple Silicon; Homebrew tap usefulness
**Blocked by:** #163 launch (deliberately sequenced after)
**Related:** #142 (Linux prebuilt binary), #165 (Windows x64), #163 (launch plan,
Homebrew tap item)

## Homebrew tap is prepared and waiting on this task

A fully prepared, sha256-verified Homebrew tap (formula + README + LICENSE) is staged in the
private companion `launch_drafts/homebrew-archcheck-ready/` (deferred 2026-07-02 — Homebrew's
audience is mostly macOS, and today only a linux-x86_64 binary ships). When this task lands a
macOS arm64 binary:
1. add the macOS/arm64 `url` + `sha256` pair (or an `on_macos do ... end` block) to
   `archcheck.rb` — the current formula gates non-Linux with a clear `odie` message;
2. `gh repo create blurman-ai/homebrew-archcheck --public --source=. --push` (the repo MUST be
   named `homebrew-archcheck` for `brew tap blurman-ai/archcheck` to resolve);
3. `brew install blurman-ai/archcheck/archcheck` then works on Apple Silicon.

## Goal

Add a macOS arm64 asset to the tag-triggered release workflow so Apple Silicon
developers (and a future Homebrew tap) get a prebuilt binary.

## Scope

- `release.yml`: a `macos-14` (arm64) job — AppleClang build, Release config,
  `archcheck-<v>-macos-arm64.tar.gz` + `.sha256`, `--version`-matches-tag gate, smoke
  job on a clean runner.
- Static linkage where possible (libc++ is system on macOS; verify no non-system dylib
  deps via `otool -L` in the smoke job). Decide on codesigning/notarization: an unsigned
  CLI tarball triggers Gatekeeper quarantine only via browser download — document
  `xattr -d com.apple.quarantine` or ship `curl | tar` instructions (CI/terminal
  installs are unaffected).
- CI: add an AppleClang job to `ci.yml` first (codebase compiles with clang-18 on Linux,
  so the tail should be small; watch `std::filesystem` and GCC8-COMPAT markers).
- Docs: install snippet in `docs/ci_usage.md` + README Install section.
- Feeds the Homebrew tap item in #163 Phase 1 (the tap formula wants this asset).
- CHANGELOG entry.

## Acceptance

- [ ] Tag push produces a checksummed macOS arm64 tarball; smoke job passes on a clean
      runner.
- [ ] AppleClang job green in `ci.yml`.
- [ ] Install docs updated; ROADMAP "planned platforms" note flipped to shipped.

## Do not do

- Do not gate the initial public launch on this (explicit decision 2026-07-02).
- Do not take on notarization infrastructure unless Gatekeeper actually blocks the
  documented install path — measure first.
