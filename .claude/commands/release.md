Cut an archcheck release: bump version, finalize CHANGELOG, tag, push — the tag triggers `release.yml`, which builds + publishes the GitHub Release asset.

The canonical process is [`docs/dev/git_workflow.md`](../../docs/dev/git_workflow.md) §"Release process". Building/publishing the asset is [`.github/workflows/release.yml`](../../.github/workflows/release.yml). This command brings the repo to the "push the tag" state and pushes the tag; everything after the tag is done by CI.

Argument (required): version `X.Y.Z` (without `v`), for example `/release 0.1.0`. For a prerelease — `X.Y.Z-rc1` (the workflow marks it as prerelease).

## Steps

1. **Readiness checks — blocked → stop, show the reason:**
   - `git status` is clean (nothing uncommitted). Dirty → stop.
   - The current branch is `master` and in sync with `origin/master` (`git fetch && git status`).
   - The tag `vX.Y.Z` does not yet exist: `git tag -l vX.Y.Z` is empty. Exists → stop.
   - The argument is valid as SemVer (`^[0-9]+\.[0-9]+\.[0-9]+(-[0-9A-Za-z.-]+)?$`).
   - CI on the last master commit is green (`gh run list --branch master --limit 1`). Red → warn and ask.

2. **Bump the version in [`CMakeLists.txt`](../../CMakeLists.txt):** `project(archcheck VERSION X.Y.Z ...)`. Verify the digits match the argument.

3. **Finalize [`CHANGELOG.md`](../../CHANGELOG.md):**
   - Rename `## [Unreleased]` → `## [X.Y.Z] - YYYY-MM-DD` (today's date).
   - Start a new empty `## [Unreleased]` at the top with the subheadings `### Added / Changed / Fixed` (or empty — following the pattern of existing ones).
   - If `[Unreleased]` is empty — ask the user whether to release for sure (nothing to describe).

4. **Show the user the diff** (`CMakeLists.txt` + `CHANGELOG.md`) and **wait for confirmation**. Edits requested → rewrite and show again.

5. **Build + smoke gate** (as in `/commit`, but Release — that's what CI will build):
   ```bash
   cmake -B build/release -S . -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build/release
   got="$(./build/release/src/archcheck --version)"
   test "$got" = "archcheck X.Y.Z"   # binary version matched the tag — release.yml checks the same
   ```
   Didn't build / version didn't match → **stop**, show the output.

6. **Commit:** `chore(release): bump to X.Y.Z` (trailers as usual). Stage only `CMakeLists.txt` + `CHANGELOG.md`.

7. **Annotated tag:** `git tag -a vX.Y.Z -m "Release X.Y.Z"`. Don't use a lightweight tag.

8. **Confirm the push with the user** (this is outward-facing: pushing the tag triggers the Release publication). After "yes":
   ```bash
   git push origin master --follow-tags
   ```

9. **Report what CI does next** and give a tracking command:
   - `release.yml` builds Linux x86_64, packs `archcheck-X.Y.Z-linux-x86_64.tar.gz` + `.sha256`, creates a GitHub Release, runs a smoke-job.
   - Track: `gh run watch $(gh run list --workflow release.yml --limit 1 --json databaseId -q '.[0].databaseId')`.
   - Check the result: `gh release view vX.Y.Z` — confirm both assets are in place.

## Dry-run

To run the whole pipeline without actually releasing: push a temporary tag `vX.Y.Z-rc1` (the workflow marks it prerelease), check the asset, then delete both the tag and the release:
```bash
gh release delete vX.Y.Z-rc1 --yes
git push origin :refs/tags/vX.Y.Z-rc1
git tag -d vX.Y.Z-rc1
```

## IMPORTANT

- The tag is the only publication trigger. Don't create a GitHub Release manually via `gh release create` (the workflow does it itself; a duplicate will cause a conflict).
- Don't recommend `latest` as a mutable reference in docs/CI — only pins by tag + checksum (see [`docs/ci_usage.md`](../../docs/ci_usage.md)).
- The version in `CMakeLists.txt` must match the tag — `release.yml` fails if `archcheck --version` ≠ the tag.
- Don't push without explicit user confirmation (step 8).
