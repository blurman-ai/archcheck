#!/usr/bin/env bash
# partial_history_drift.sh — per-commit Type-3 (partial copy-paste) drift series.
#
# Question (#054, partial-dup variant): WHICH COMMITS introduce architectural
# drift via copy-paste? For each sampled commit C (parent P) we ask the diff mode
# of the #056 partial_duplication spike: "did C ADD code that is a Type-3
# near-duplicate of code that already existed at P?" — a missing-reuse edge born
# in that commit.
#
# Unlike history_drift.sh (whole-tree ratio per snapshot), this is per-commit and
# diff-scoped: it attributes the copy-paste to the commit that introduced it.
#
# Sampling matches history_drift.sh: N evenly-spaced commits, idx = i*(T-1)/(N-1).
#
# Usage: partial_history_drift.sh <repo> <subpath-rel> <N> <tag>
set -u

REPO="${1:?repo path}"
SUBREL="${2:?subpath relative to repo}"
N="${3:-30}"
TAG="${4:?run tag}"

BIN=/tmp/partial_dup_build/partial_duplication
# precise (token-LCS) confirm, function-scale, relative rare-df, vendored/generated excludes.
COMMON=(--partial-precise --min-tokens 45 --rare-df-pct 8 --threshold 0.85 \
        --exclude include/imgui --exclude precompile --exclude external)

OUTDIR="/home/localadm/projects/cpparch/experiments/ai_repo_run/runs_history/$TAG"
mkdir -p "$OUTDIR"
SERIES="$OUTDIR/series.tsv"
HITLOG="$OUTDIR/hits.log"
: > "$HITLOG"

mapfile -t ALL < <(git -C "$REPO" rev-list --reverse HEAD)
TOTAL=${#ALL[@]}
echo "repo=$REPO total_commits=$TOTAL sampling N=$N subpath=$SUBREL"
echo "covered=$N of $TOTAL commits (sampled, NOT exhaustive); skipped=$((TOTAL-N))"

printf "idx\tsha\tdate\tadded_frags\tpartial_hits\tmax_sim\tmsg\n" > "$SERIES"

for ((i=0; i<N; i++)); do
  if [ "$N" -ge "$TOTAL" ]; then idx=$i; else idx=$(( i * (TOTAL-1) / (N-1) )); fi
  sha=$(git -C "$REPO" rev-parse --short "${ALL[$idx]}")
  date=$(git -C "$REPO" log -1 --format=%as "${ALL[$idx]}")
  msg=$(git -C "$REPO" log -1 --format=%s "${ALL[$idx]}" | tr '\t' ' ' | cut -c1-60)

  out=$("$BIN" --diff "$sha" --repo "$REPO" --subpath "$SUBREL" "${COMMON[@]}" --top 20 2>/dev/null)
  summary=$(echo "$out" | head -1)
  added=$(echo "$summary"  | sed -n 's/.*added_frags=\([0-9]*\).*/\1/p')
  hits=$(echo "$summary"   | sed -n 's/.*partial_hits=\([0-9]*\).*/\1/p')
  maxsim=$(echo "$summary" | sed -n 's/.*max_sim=\([0-9.]*\).*/\1/p')

  printf "%d\t%s\t%s\t%s\t%s\t%s\t%s\n" \
    "$idx" "$sha" "$date" "${added:-0}" "${hits:-0}" "${maxsim:-0}" "$msg" \
    | tee -a "$SERIES"

  # log full hit lists for any commit with >=1 hit (the spot-check material)
  if [ "${hits:-0}" -gt 0 ]; then
    { echo "=== idx=$idx $sha ($date) — $msg ==="; echo "$out"; echo; } >> "$HITLOG"
  fi
done

echo ""
echo "series -> $SERIES"
echo "hits   -> $HITLOG"
