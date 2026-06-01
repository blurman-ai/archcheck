#!/usr/bin/env bash
# unshallow_all.sh — докачать ПОЛНУЮ git-историю всех shallow-реп корпуса.
# Корпус качался --depth 1 (snapshot). Для дрейфа-по-истории (#054) нужна
# вся история на каждой репе. Идём по очереди, логируем размер/коммиты до/после.
set -u
ROOT=~/oss/_aidev_run
LOG="$ROOT/_unshallow.log"
: > "$LOG"

log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

log "START unshallow over $ROOT"
total=0; done_n=0; skip_n=0; fail_n=0
for d in "$ROOT"/*/; do
  [ -d "$d/.git" ] || continue
  name=$(basename "$d")
  total=$((total+1))
  shallow=$(git -C "$d" rev-parse --is-shallow-repository 2>/dev/null)
  if [ "$shallow" != "true" ]; then
    n=$(git -C "$d" rev-list --count HEAD 2>/dev/null)
    log "SKIP  $name (already full, commits=$n)"
    skip_n=$((skip_n+1))
    continue
  fi
  before=$(du -sm "$d" 2>/dev/null | cut -f1)
  log "FETCH $name (shallow, ${before}M) ..."
  if git -C "$d" fetch --unshallow >>"$LOG" 2>&1; then
    after=$(du -sm "$d" 2>/dev/null | cut -f1)
    n=$(git -C "$d" rev-list --count HEAD 2>/dev/null)
    log "  OK  $name ${before}M -> ${after}M  commits=$n"
    done_n=$((done_n+1))
  else
    log "  FAIL $name (fetch error, see log)"
    fail_n=$((fail_n+1))
  fi
done
log "DONE total=$total unshallowed=$done_n skipped=$skip_n failed=$fail_n"
log "corpus size now: $(du -sh "$ROOT" 2>/dev/null | cut -f1)"
