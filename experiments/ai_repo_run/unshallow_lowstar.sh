#!/usr/bin/env bash
# unshallow_lowstar.sh — докачать ПОЛНУЮ git-историю низко-звёздных реп
# (_aidev_lowstar/<bucket>/*), чтобы прогнать на них дрейф-по-истории.
# Расширяет корпус дрейфа с 85 до ~245 реп (×3). Идём по очереди, логируем.
set -u
ROOT=/home/localadm/oss/_aidev_lowstar
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/unshallow_lowstar.log
: > "$LOG"
log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

log "START unshallow lowstar over $ROOT"
total=0; ok=0; skip=0; fail=0
for d in "$ROOT"/*/*/; do
  [ -d "$d/.git" ] || continue
  name=$(basename "$d")
  total=$((total+1))
  # отключаем авто-gc заранее — он главный тормоз на checkout-heavy прогоне
  git -C "$d" config gc.auto 0 2>/dev/null
  if ! git -C "$d" rev-parse --is-shallow-repository 2>/dev/null | grep -q true; then
    skip=$((skip+1)); log "SKIP  $name (уже полная)"; continue
  fi
  if timeout 300 git -C "$d" fetch --unshallow >>"$LOG" 2>&1; then
    n=$(git -C "$d" rev-list --count HEAD 2>/dev/null)
    ok=$((ok+1)); log "  OK  $name commits=$n"
  else
    fail=$((fail+1)); log "  FAIL $name (timeout/error)"
  fi
done
log "DONE total=$total ok=$ok skip=$skip fail=$fail"
log "lowstar size: $(du -sh "$ROOT" 2>/dev/null | cut -f1)"
