#!/usr/bin/env bash
# clone_highstar_rest.sh — добрать оставшиеся ≥100★ C++ репы (взяли 50 из 119).
# Тот же метод: --depth 1 --filter=blob:none, size<500MB через gh api.
# В _aidev_run (тот же корпус). Пропускает уже существующие.
set -u
POOL=/tmp/cpp_pool_shuffled.txt           # full_name<TAB>stars, seed=42
ROOT=/home/localadm/oss/_aidev_run
MAXKB=500000
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/clone_highstar_rest.log
TSV=/home/localadm/projects/cpparch/experiments/ai_repo_run/corpus_highstar_rest.tsv
: > "$LOG"
echo -e "full_name\tstars\tsize_kb" > "$TSV"
log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

added=0
log "START highstar-rest clone"
while IFS=$'\t' read -r full stars; do
  [ -z "$full" ] && continue
  name=$(basename "$full")
  dest="$ROOT/$name"
  [ -d "$dest/.git" ] && { log "SKIP $full (exists)"; continue; }
  size=$(gh api "repos/$full" --jq '.size' 2>/dev/null)
  if ! [[ "$size" =~ ^[0-9]+$ ]]; then log "MISS $full (gone?)"; continue; fi
  if [ "$size" -ge "$MAXKB" ]; then log "BIG  $full (${size}kb)"; continue; fi
  if git clone --depth 1 --filter=blob:none "https://github.com/$full" "$dest" >>"$LOG" 2>&1; then
    real=$(du -sm "$dest" 2>/dev/null | cut -f1)
    added=$((added+1)); log "  OK $full stars=$stars size=${size}kb disk=${real}M (added=$added)"
    echo -e "$full\t$stars\t$size" >> "$TSV"
  else
    log "  FAIL $full"; rm -rf "$dest"
  fi
done < "$POOL"
log "DONE added=$added. corpus _aidev_run size: $(du -sh "$ROOT" 2>/dev/null | cut -f1)"
