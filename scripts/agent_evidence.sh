#!/usr/bin/env bash
# Consolidated agent evidence block (#183). Runs the EXISTING gates from
# .claude/commands/commit.md steps 3-6 and prints ONE compact before/after
# block. No new checks — reuse only.
#
# Usage: scripts/agent_evidence.sh [label]
#
# EVIDENCE_FAST=1 skips the coverage gate (fast inner-loop use only). A block
# produced with EVIDENCE_FAST=1 is tagged [FAST] and MUST NOT be cited to
# justify ACCEPT — see docs/dev/agent_cells.md (Goodhart law) — only a full
# run closes a cell.
#
# Exit: 0 = all gates PASS/clean, 1 = at least one gate FAILED or a required
# tool is missing (a missing tool is reported as ERROR, never as silent clean).

set -uo pipefail  # not -e: gate commands are expected to fail sometimes; we
                   # capture their exit code and keep going, not abort.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT"

LABEL="${1:-run}"
CF="$HOME/.local/bin/clang-format"
LOG_DIR="$ROOT/build/debug"
mkdir -p "$LOG_DIR"

FAILED=0
TAG=""
[[ "${EVIDENCE_FAST:-0}" == "1" ]] && TAG=" [FAST — not ACCEPT-eligible]"

SHA="$(git rev-parse --short HEAD 2>/dev/null || echo unknown)"
if [[ -n "$(git status --porcelain 2>/dev/null)" ]]; then
  SHA="${SHA}+dirty"
fi

echo "=== archcheck evidence [$LABEL] @ ${SHA}${TAG} ==="

need_tool() {
  if ! command -v "$1" &>/dev/null; then
    echo "$2 ERROR (tool missing: $1)"
    FAILED=1
    return 1
  fi
  return 0
}

# ── build ─────────────────────────────────────────────────────────────────
BUILD_OK=1
if need_tool cmake "build (Debug):   " && need_tool ninja "build (Debug):   "; then
  if cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug \
       > "$LOG_DIR/agent_evidence_cmake.log" 2>&1 \
     && cmake --build build/debug > "$LOG_DIR/agent_evidence_build.log" 2>&1; then
    echo "build (Debug):    OK"
  else
    echo "build (Debug):    FAIL (see $LOG_DIR/agent_evidence_build.log)"
    BUILD_OK=0
    FAILED=1
  fi
else
  BUILD_OK=0
fi

# ── tests + self-check + smoke (need a working binary) ─────────────────────
if [[ "$BUILD_OK" == 1 ]]; then
  ( cd build/debug && ctest --output-on-failure ) > "$LOG_DIR/agent_evidence_tests.log" 2>&1
  SUMMARY_LINE="$(grep -oP '\d+% tests passed, \d+ tests failed out of \d+' "$LOG_DIR/agent_evidence_tests.log" | tail -1)"
  TOTAL="$(echo "$SUMMARY_LINE" | grep -oP '(?<=out of )\d+')"
  NFAILED="$(echo "$SUMMARY_LINE" | grep -oP '\d+(?= tests failed)')"
  if [[ -z "$TOTAL" ]]; then
    echo "tests:            ERROR (no ctest summary — see $LOG_DIR/agent_evidence_tests.log)"
    FAILED=1
  elif [[ "$NFAILED" == 0 ]]; then
    echo "tests:            $TOTAL/$TOTAL PASSED"
  else
    PASSED=$((TOTAL - NFAILED))
    NAMES="$(sed -n '/The following tests FAILED:/,$p' "$LOG_DIR/agent_evidence_tests.log" \
              | grep -oP '(?<=- ).*(?= \(Failed\))' | paste -sd ', ' -)"
    echo "tests:            $PASSED/$TOTAL PASSED | FAILED: $NAMES"
    FAILED=1
  fi

  ./build/debug/src/archcheck --format json src include tests \
    > "$LOG_DIR/agent_evidence_selfcheck.json" 2>&1
  SC_TOTAL="$(grep -oP '"total":\s*\K\d+' "$LOG_DIR/agent_evidence_selfcheck.json")"
  if [[ -z "$SC_TOTAL" ]]; then
    echo "self-check:       ERROR (see $LOG_DIR/agent_evidence_selfcheck.json)"
    FAILED=1
  else
    echo "self-check:       $SC_TOTAL violations (src include tests)"
    [[ "$SC_TOTAL" != 0 ]] && FAILED=1
  fi
else
  echo "tests:            SKIPPED (build failed)"
  echo "self-check:       SKIPPED (build failed)"
fi

# ── format (static — always runs, independent of the build) ────────────────
if need_tool "$CF" "format (18.1.3): "; then
  FMT_OUT="$(find src include tests \( -name '*.h' -o -name '*.cpp' \) -print \
    | xargs -r "$CF" --dry-run --Werror --style=file 2>&1)"
  DIRTY_N="$(echo "$FMT_OUT" | grep -oP '^[^:]+(?=:\d+:\d+: error:)' | sort -u | wc -l)"
  if [[ "$DIRTY_N" == 0 ]]; then
    echo "format (18.1.3):  clean"
  else
    echo "format (18.1.3):  $DIRTY_N files dirty"
    FAILED=1
  fi
fi

# ── cppcheck (static) ────────────────────────────────────────────────────
if need_tool cppcheck "cppcheck:        "; then
  CPPCHECK_OUT="$(cppcheck --enable=warning,performance,portability \
    --inline-suppr --suppress=missingIncludeSystem --quiet \
    -I include src/ include/ 2>&1)"
  CPPCHECK_N="$(echo "$CPPCHECK_OUT" | grep -cE '^\[.*\]: \(')"
  if [[ "$CPPCHECK_N" == 0 ]]; then
    echo "cppcheck:         clean"
  else
    echo "cppcheck:         $CPPCHECK_N warnings"
    FAILED=1
  fi
fi

# ── lizard (static) ─────────────────────────────────────────────────────
if need_tool lizard "lizard:          "; then
  LIZARD_N="$(lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/ 2>&1 \
    | grep -c ': warning:')"
  if [[ "$LIZARD_N" == 0 ]]; then
    echo "lizard:           clean"
  else
    echo "lizard:           $LIZARD_N warnings"
    FAILED=1
  fi
fi

# ── coverage (slowest gate — needs a working Debug build; own build dir) ────
if [[ "${EVIDENCE_FAST:-0}" == "1" ]]; then
  echo "coverage:         SKIPPED (fast mode)"
elif [[ "$BUILD_OK" == 0 ]]; then
  echo "coverage:         SKIPPED (build failed)"
else
  COV_OUT="$(bash scripts/check_coverage.sh 2>&1)"
  LINES_PCT="$(echo "$COV_OUT" | grep -oP 'lines\s+:\s+\K[\d.]+%' )"
  FUNCS_PCT="$(echo "$COV_OUT" | grep -oP 'functions:\s+\K[\d.]+%' )"
  BRANCH_PCT="$(echo "$COV_OUT" | grep -oP 'branches\s+:\s+\K[\d.]+%' )"
  if echo "$COV_OUT" | grep -q 'RESULT: PASS'; then
    echo "coverage:         lines $LINES_PCT funcs $FUNCS_PCT branches $BRANCH_PCT  (gates 90/90/40)"
  elif echo "$COV_OUT" | grep -q 'RESULT: FAIL'; then
    echo "coverage:         lines $LINES_PCT funcs $FUNCS_PCT branches $BRANCH_PCT  (gates 90/90/40, FAIL)"
    FAILED=1
  else
    echo "coverage:         ERROR (script did not report PASS/FAIL)"
    FAILED=1
  fi
fi

# ── smoke (exact commands from commit.md step 5) ────────────────────────────
if [[ "$BUILD_OK" == 1 ]]; then
  S1="FAIL"; S2="FAIL"; S3="FAIL"
  ./build/debug/src/archcheck --version >/dev/null 2>&1 && S1="OK"
  ./build/debug/src/archcheck --help >/dev/null 2>&1 && S2="OK"
  ./build/debug/src/archcheck unknown >/dev/null 2>&1; [[ $? == 2 ]] && S3="OK"
  echo "smoke:            --version $S1, --help $S2, unknown arg -> exit 2 $S3"
  [[ "$S1" == "OK" && "$S2" == "OK" && "$S3" == "OK" ]] || FAILED=1
else
  echo "smoke:            SKIPPED (build failed)"
fi

echo "Unverified:       clang-18/libc++ build (CI-only); Windows/macOS binaries; runtime perf"

# Script's own wall-clock (bash's $SECONDS starts at 0 when the script starts).
# This is the gate-chain run time, NOT the whole cell session (reading, writing
# fixtures, etc.) — that still needs the agent to self-report start/end.
if (( SECONDS >= 60 )); then
  echo "wall time:        $((SECONDS / 60))m $((SECONDS % 60))s"
else
  echo "wall time:        ${SECONDS}s"
fi

exit "$FAILED"
