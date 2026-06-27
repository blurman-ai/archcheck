# [PERF][DUPLICATION] diff-scope the new-clone scan to changed files (O(diff), not O(tree))

**Created:** 2026-06-27
**Status:** new
**Module:** SCAN / DUPLICATION / DIFF
**Priority:** major
**Related:** #149 (byte-cap mitigation shipped; this is its "proper fix" follow-up),
#103 (copy-paste precision — the calibration this must not break), #123 (new-clone drift),
#151 (clone-filter FP — adjacent set-dependent-filter territory)

## Why this exists (split out of #149)

#149 shipped a **byte-cap bulk-skip** (`diff_max_clone_scan_bytes`, 40 MiB): past the cap the
new-clone advisory is skipped, the gate still runs. That rescues *size-driven* timeouts (10k+-file
giants) but NOT **pair-explosion** repos (moderate bytes, dense duplication → O(pairs) blowup, e.g.
`bloomberg/blazingmq` under the cap). The real fix is to make the per-commit clone scan cost O(diff)
instead of O(tree). This is a behaviour-affecting change (see below) and needs precision
re-validation, so it is its own task, not part of #149.

## Profile (the thing to beat)

`apache/arrow` HEAD (1747 files, 21 MB): `detectNewClones` = **4145 ms of 4.5 s (92%)**, split evenly
parentPairKeys 2118 ms / emitNewClones 2059 ms. Inside one scan: lex 579 ms, index 104 ms,
**score+filter 1380 ms** over **160 645 candidate pairs**. Lexing the whole tree is unavoidable (a
clone's twin may be in an unchanged file); the cost is **scoring/filtering the candidate pairs**.

## The attempt that was REVERTED (2026-06-27) — and why it is not free

Added `ScannerOptions::restrictToFiles` + a phase-3 skip of candidate pairs touching no changed file,
passed to both the parent and new scans. Speed worked: arrow 5.30 → **2.58 s**; 574 tests + 293 clone
cases green; the touched-but-pre-existing negative held.

**But before/after on 30 real `xiaozhi-esp32` commits (actual `file:line:message`): 17/30 differ — the
scoped scan emits a DIFFERENT, LARGER clone set** (e.g. 2 → 20, 8 → 40). Restricting the candidate set
is not transparent: the index `sharedRare` is identical, so the divergence is **downstream in
`applyCandidateFilters`** — one of the corpus-calibrated FP stages (data-table / boilerplate /
file-local-IDF / dedup / classify) is candidate-**set**-dependent, and a smaller set changes its
decisions. `#103` precision (86–91 %) was measured on the full-scan behaviour. Reverted.

(Self-check trap logged in JOURNEY: the first count-only comparison read "same=30" off the wrong JSON
key — `ruleId` vs `rule` — vacuously 0==0; the divergence only appeared once content was compared.)

## What this task must do

- [ ] **Isolate the set-dependent stage.** Bisect `applyCandidateFilters` (disable each phase, re-run
      the 30-commit before/after) to find which one changes output when the candidate set shrinks.
      Likely a phase that aggregates over `candidates` (not per-pair). Cross-check #151 territory.
- [ ] **Decide the semantics.** Either (a) make scoping transparent — feed the set-dependent stage the
      full candidate statistics while only EMITTING changed-file pairs; or (b) accept the larger set if
      eye-check shows the extra findings are real TPs the full scan wrongly suppressed — then
      **re-validate #103 precision** on the new set before adopting.
- [ ] Rework the parent-guard off the full-parent pair scan (content-existence check) so it is cheap
      AND matches semantics (mind the 3-copy edge: parent A,B unchanged + new copy C).
- [ ] Keep the #123 control set 10/10 and the new_clone_drift unit cases green.
- [ ] Re-measure: how many of the 256 ai-377 / 28 trending blacklisted repos pass after the fix.
- [ ] Unblocks the agentic-stratum bot-test re-run (#146).

## Caveat for the re-run — a SECOND bottleneck: snapshot read (2026-06-27)

Verified live that the clone scan is **not the only** thing that times out the corpus giants. With the
shipped byte-cap at default, `tensorflow` (17 059 C++ files) and `opencv` (3 715) still hit ~115 s and
were killed at **RSS ≈ 35 MB** — i.e. stuck **before** the clone scan, in `readRefSnapshotMemory`
reading thousands of blobs one-by-one via `git cat-file`. These corpus repos are cloned
`--no-checkout` + `--filter=blob:none`, so per-file blob access is slow.

This is an **artifact of the corpus clone strategy, not of real CI** — in CI the repo is checked out,
files are on disk, and the snapshot read is fast (cf. `apache/arrow`, checked out, 21 MB, completes in
~5 s; the byte-cap demo skips its clone scan → ~1 s). Implication for this task's re-run step: drive
the agentic-stratum / blacklist re-measure on **checked-out** working trees (or a batched
`cat-file --batch` blob read), NOT `--diff-mode=memory` over blob:none clones — otherwise the
snapshot-read wall will mask whether the clone-scope fix actually rescued the repo. The byte-cap demo
(arrow, cap < tree size → clone skipped, gate intact, 5.3 s → 1.1 s) confirms the clone-scan path
itself is correctly bounded.

## Acceptance

Per-commit new-clone scan is O(changed) on a large repo (blazingmq completes < 120 s) AND the emitted
clone set is either identical to the current full scan or its precision is re-validated ≥ the #103
baseline. Fixture: a large synthetic tree + small diff → scan time bounded, findings unchanged.
