## What

<!-- One paragraph: what changes and why. -->

## Checklist

- [ ] Tests pass: `cd build/debug && ctest --output-on-failure`
- [ ] Dogfood clean: `./build/debug/src/archcheck src include tests` → 0 violations
- [ ] Rule changes come with `fixtures/<rule>/pass/` and `fail_*/` cases
- [ ] User-visible changes added to `[Unreleased]` in CHANGELOG.md
- [ ] Commit messages follow Conventional Commits
