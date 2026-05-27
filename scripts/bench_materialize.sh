#!/usr/bin/env bash
#
# Benchmark ways to materialize a git ref into a directory, then verify
# our scanner (archcheck --graph) sees the same nodes/edges from each.
#
# Usage:
#   scripts/bench_materialize.sh <repo> <ref> [archcheck-binary]
#
# Methods (in order, each runs once warm + once measured):
#   1. worktree         — git worktree add --detach
#   2. archive          — git archive | tar -x
#   3. checkout-index   — read-tree to temp index + checkout-index --prefix
#   4. restore          — git restore --source --worktree (modern porcelain)
#   5. shared-clone     — git clone --local --no-checkout + checkout

set -u  # no -e: we want the script to continue past one bad method

REPO=${1:?usage: $0 <repo> <ref> [archcheck-binary]}
REF=${2:?usage: $0 <repo> <ref> [archcheck-binary]}
ARCHCHECK=${3:-/home/localadm/projects/cpparch/build/release/src/archcheck}

if [[ ! -d "$REPO/.git" ]]; then
   echo "error: $REPO is not a git repository" >&2
   exit 2
fi
if [[ ! -x "$ARCHCHECK" ]]; then
   echo "error: archcheck binary not found at $ARCHCHECK" >&2
   exit 2
fi

REPO=$(realpath "$REPO")
SHA=$(git -C "$REPO" rev-parse "$REF")
echo "repo:      $REPO"
echo "ref:       $REF  ($SHA)"
echo "archcheck: $ARCHCHECK"
echo

now_ms() { date +%s%3N; }

# materialize <method> <out-dir>
# 0 on success, non-zero with diagnostic on failure
materialize() {
   local method=$1 out=$2 tmp_idx=$3
   case "$method" in
      worktree)
         # `git worktree add` insists on creating the dir itself.
         rmdir "$out" 2>/dev/null
         git -C "$REPO" worktree add --detach --quiet "$out" "$SHA"
         ;;
      archive)
         mkdir -p "$out"
         git -C "$REPO" archive --format=tar "$SHA" | tar -x -C "$out"
         ;;
      checkout-index)
         mkdir -p "$out"
         GIT_INDEX_FILE="$tmp_idx" git -C "$REPO" read-tree "$SHA" \
            && GIT_INDEX_FILE="$tmp_idx" git -C "$REPO" checkout-index --all --prefix="$out/" -f
         ;;
      restore)
         mkdir -p "$out"
         # `git restore --source <ref> --worktree -- .` from inside the target dir
         # treating $out itself as the worktree, but pointing at $REPO/.git.
         (cd "$out" && GIT_WORK_TREE="$out" GIT_DIR="$REPO/.git" \
            git restore --source="$SHA" --worktree --quiet -- ':!.git' .)
         ;;
      shared-clone)
         git clone --local --no-checkout --quiet "$REPO" "$out" \
            && git -C "$out" checkout --quiet --detach "$SHA"
         ;;
      *) echo "unknown method: $method" >&2; return 1 ;;
   esac
}

cleanup() {
   local method=$1 out=$2 tmp=$3
   if [[ "$method" == "worktree" ]]; then
      git -C "$REPO" worktree remove --force "$out" 2>/dev/null
   fi
   rm -rf "$tmp"
}

# run_once <method>  → prints one summary line, or "FAILED" with stderr
run_once() {
   local method=$1
   local tmp out idx
   tmp=$(mktemp -d -t archcheck-bench-XXXXXX)
   out="$tmp/work"
   idx="$tmp/idx"

   local mat_t0 mat_t1 mat_ms
   mat_t0=$(now_ms)
   if ! materialize "$method" "$out" "$idx" 2>"$tmp/err"; then
      echo "FAILED ($method): $(head -1 "$tmp/err")"
      cleanup "$method" "$out" "$tmp"
      return 1
   fi
   mat_t1=$(now_ms)
   mat_ms=$((mat_t1 - mat_t0))

   local files
   files=$(find "$out" -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.c' \
              -o -name '*.cpp' -o -name '*.cxx' -o -name '*.cc' \) | wc -l)

   local scan_t0 scan_t1 scan_ms gout nodes edges
   scan_t0=$(now_ms)
   gout=$("$ARCHCHECK" --graph "$out" 2>/dev/null)
   scan_t1=$(now_ms)
   scan_ms=$((scan_t1 - scan_t0))
   nodes=$(awk '/^nodes:/{print $2}' <<<"$gout")
   edges=$(awk '/^edges:/{print $2}' <<<"$gout")

   cleanup "$method" "$out" "$tmp"

   printf "%-15s  materialize=%6d ms   scan=%5d ms   files=%5d   nodes=%5s   edges=%5s\n" \
      "$method" "$mat_ms" "$scan_ms" "$files" "$nodes" "$edges"
}

# Warm: ensures git object cache is populated, OS filecache likewise.
echo "--- warmup (results discarded) ---"
for m in worktree archive checkout-index restore shared-clone; do
   out=$(run_once "$m")
   echo "warm: $out"
done
echo
echo "--- measured ---"
for m in worktree archive checkout-index restore shared-clone; do
   run_once "$m"
done
