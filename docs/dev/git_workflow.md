# Git workflow — archcheck

OSS process standards. Each block carries a link to its source, so that no external contributor has to wonder "why this way".

## Branching: GitHub Flow

Source: [GitHub Flow](https://docs.github.com/en/get-started/quickstart/github-flow).

- `master` is always green.
- Feature branch: `<type>/<NNN>-<short-slug>` — for example `feat/004-project-skeleton`, `docs/006-spec-refactor`, `chore/007-workflow-setup`.
- `<type>` ∈ `feat / fix / docs / refactor / chore / test / build / perf / ci` — the same as in Conventional Commits.
- `<NNN>` — task ID from `backlog/`. No task — create one first.
- After merge the branch is deleted automatically (`Settings → General → Pull Requests → Automatically delete head branches`).

### Direct push for admin

The repo admin is added to the master ruleset's *Bypass list* (Settings → Rules). `git push origin master` directly is allowed. A PR remains an option for risky changes, not a mandatory step.

Force-push to master is blocked for everyone, including admin — deliberately, as protection against accidental history rewriting.

## Commits: Conventional Commits 1.0

Source: [conventionalcommits.org/v1.0.0](https://www.conventionalcommits.org/en/v1.0.0/).

Format:

```
<type>(<scope>): <description>

[optional body]

[optional trailers]
```

**Types:**

| Type | When |
|---|---|
| `feat` | new product functionality |
| `fix` | bugfix in product code |
| `docs` | documentation only (README, docs/, spec) |
| `refactor` | code change without behavior change |
| `test` | tests or fixtures |
| `build` | build system (CMake, dependencies) |
| `ci` | CI configs (GitHub Actions) |
| `perf` | optimization |
| `chore` | routine: repo infrastructure, skills, renames, configs |

**Scopes** for archcheck (by subsystem):

| Scope | Subsystem |
|---|---|
| `config` | YAML loader, Config struct |
| `graph` | component graph, cycles, metrics |
| `scan` | include / clang scanners |
| `rules/sf` | Core Guidelines SF.* |
| `rules/lakos` | cycles, god-headers, CCD/ACD/NCCD |
| `rules/martin` | I/A/D metrics |
| `rules/custom` | user pattern rules |
| `report` | text / json / sarif reporters |
| `cli` | main, arguments, exit codes |
| `fixtures` | `fixtures/` |
| `build` | CMake, packaging |
| `docs` | general documents |
| `spec` | architecture specification |
| `claude` | `.claude/` (settings, skills) |
| `tasks` | `backlog/` |
| `process` | git workflow, CHANGELOG, release process |

If changes span several scopes — specify the most representative one or omit the scope.

**Subject:** ≤ 72 characters, lowercase, imperative (`add`, not `added`), no trailing period. May reference a task: `(#NNN)`.

**Trailers** (on top of Conventional Commits — ignored by parsers, needed for AI audit):

```
AI-Assisted: Claude
Verified: <how it was checked — autotest / manual / build / nothing>
Risk: low|med|high (reason)
Co-Authored-By: Claude <noreply@anthropic.com>
```

## Versioning: SemVer 2.0

Source: [semver.org/spec/v2.0.0](https://semver.org/spec/v2.0.0.html).

- Pre-1.0 (current phase): `0.x.y`. SemVer §4 permits breaking changes in MINOR; we avoid them but do not forbid them.
- v1.0 — when CLI flags, exit codes, the JSON report schema, the YAML config format, and the baseline format are stabilized.
- What counts as a breaking change — see [`docs/architecture-spec.md`](../architecture-spec.md) section "Stability contract".

## Tags: annotated `vX.Y.Z`

```bash
git tag -a v0.1.0 -m "Release 0.1.0"
git push origin master --follow-tags
```

Do not use lightweight tags (`git tag v0.1.0` without `-a`) — metadata (author, date, message) is lost.

## Changelog: Keep a Changelog 1.1

Source: [keepachangelog.com/v1.1.0](https://keepachangelog.com/en/1.1.0/).

[`CHANGELOG.md`](../../CHANGELOG.md) at the repo root. Sections: `Added / Changed / Deprecated / Removed / Fixed / Security`. `[Unreleased]` is always on top; at release it is frozen as `[X.Y.Z] - YYYY-MM-DD`, and a new empty `[Unreleased]` is started. Written by hand, not auto-generated — a curated changelog reads better for humans.

## Known lint-gate limitations

### clang-format: divergence between local and CI builds

CI uses `clang-format-18` from Ubuntu apt. The local environment (AstraLinux) contains `clang-format-18.1.8 (9.astra6)` — a different build of the same major version. Occasionally they format complex constructs (multiline ternaries, long `<<` chains) differently.

**Accepted decision:** accept it as a risk. If CI fails with `-Wclang-format-violations` on a file that the lint-gate accepted locally — run `clang-format -i` and push a separate `fix(ci)` commit.

Full elimination would require a single binary (Docker with Ubuntu), which is too heavy for local development.

## Release process (draft until v1.0)

Automated by the [`/release`](../../.claude/commands/release.md) command; manually — the same steps:

1. All release commits are merged into master.
2. `CHANGELOG.md`: rename `[Unreleased]` → `[X.Y.Z] - YYYY-MM-DD`, add an empty `[Unreleased]`.
3. Bump the version in `CMakeLists.txt` (`project(... VERSION X.Y.Z)`).
4. Commit: `chore(release): bump to X.Y.Z`.
5. Tag: `git tag -a vX.Y.Z -m "Release X.Y.Z"`.
6. Push: `git push origin master --follow-tags`.
7. **The GitHub Release is created automatically** — the [`release.yml`](../../.github/workflows/release.yml) workflow
   catches the push of a `vX.Y.Z` tag, builds the Linux x86_64 binary, packs
   `archcheck-X.Y.Z-linux-x86_64.tar.gz` + `.sha256`, publishes the Release and
   runs a smoke job on a clean runner. No manual touch needed.

Dry-run: push a temporary tag (`vX.Y.Z-rc1` → marked prerelease),
check the asset, then delete the tag and release.
