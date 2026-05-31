#!/usr/bin/env bash
# Export each corpus repo's include graph and run the cheap "touchable" probe.
# Usage: graph_probe_all.sh [corpus_root]   (default: ~/oss/_aidev_run)
# Output: runs_history/probe/<repo>.txt (full per-repo findings)
#         runs_history/probe/summary.tsv (one header line per repo)
set -u
ROOT="${1:-~/oss/_aidev_run}"
HERE="$(cd "$(dirname "$0")" && pwd)"
AC=~/projects/cpparch/build/debug/src/archcheck
PROBE="$HERE/graph_probe.py"
OUT="$HERE/runs_history/probe"
mkdir -p "$OUT"
SUM="$OUT/summary.tsv"
printf "repo\tnodes\tedges\ten\tcyclic\tlargest\tmutual\tselfloop\n" > "$SUM"

for d in "$ROOT"/*/; do
  repo="$(basename "$d")"
  gf="/tmp/probe_$repo.graph"
  "$AC" --save-graph-baseline "$gf" "$d" >/dev/null 2>&1 || { echo "SKIP $repo (export failed)"; continue; }
  python3 "$PROBE" "$gf" "$repo" > "$OUT/$repo.txt" 2>&1 || { echo "SKIP $repo (probe failed)"; continue; }
  # parse the header metric line into the TSV
  hdr="$(sed -n '2p' "$OUT/$repo.txt")"
  n=$(sed -E 's/.*nodes=([0-9]+).*/\1/' <<<"$hdr")
  e=$(sed -E 's/.*edges=([0-9]+).*/\1/' <<<"$hdr")
  en=$(sed -E 's/.*edges\/nodes=([0-9.]+).*/\1/' <<<"$hdr")
  cy=$(sed -E 's/.*cyclic_sccs=([0-9]+).*/\1/' <<<"$hdr")
  lg=$(sed -E 's/.*largest_scc=([0-9]+).*/\1/' <<<"$hdr")
  mu=$(sed -E 's/.*mutual_pairs=([0-9]+).*/\1/' <<<"$hdr")
  sl=$(sed -E 's/.*selfloops=([0-9]+).*/\1/' <<<"$hdr")
  printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" "$repo" "$n" "$e" "$en" "$cy" "$lg" "$mu" "$sl" >> "$SUM"
  rm -f "$gf"
  echo "ok $repo  (n=$n e/n=$en cyc=$cy largest=$lg mutual=$mu self=$sl)"
done
echo "=== summary: $SUM ==="