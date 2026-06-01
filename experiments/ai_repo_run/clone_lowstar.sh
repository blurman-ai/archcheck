#!/usr/bin/env bash
# clone_lowstar.sh — клонировать низко-звёздную выборку по бакетам.
# Метод тот же, что у corpus_50: git clone --depth 1 --filter=blob:none,
# size<500MB через gh api. По N реп на бакет. Отдельный корень, чтобы не
# смешивать с ≥100★ и чтобы unshallow_all (он по _aidev_run) их не трогал.
set -u
POOL=/home/localadm/projects/cpparch/experiments/ai_repo_run/lowstar_pool.tsv
ROOT=/home/localadm/oss/_aidev_lowstar
N="${1:-20}"            # реп на бакет
MAXKB=500000            # 500 MB
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/clone_lowstar.log
TSV=/home/localadm/projects/cpparch/experiments/ai_repo_run/corpus_lowstar.tsv
mkdir -p "$ROOT"
: > "$LOG"
echo -e "bucket\tfull_name\tstars\tsize_kb" > "$TSV"
log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

declare -A COUNT
log "START lowstar clone N=$N/bucket root=$ROOT"
# пропускаем заголовок пула
tail -n +2 "$POOL" | while IFS=$'\t' read -r bucket full stars; do
  [ -z "$bucket" ] && continue
  cur=${COUNT[$bucket]:-0}
  [ "$cur" -ge "$N" ] && continue

  name=$(basename "$full")
  dest="$ROOT/$bucket/$name"
  if [ -d "$dest/.git" ]; then
    COUNT[$bucket]=$((cur+1)); log "SKIP $full (exists)"; continue
  fi

  # размер через gh api (kb); 404/ошибка -> пропуск
  size=$(gh api "repos/$full" --jq '.size' 2>/dev/null)
  if ! [[ "$size" =~ ^[0-9]+$ ]]; then
    log "MISS $full (no api/size, gone?)"; continue
  fi
  if [ "$size" -ge "$MAXKB" ]; then
    log "BIG  $full (${size}kb >= ${MAXKB})"; continue
  fi

  mkdir -p "$ROOT/$bucket"
  if git clone --depth 1 --filter=blob:none "https://github.com/$full" "$dest" >>"$LOG" 2>&1; then
    cur=$((cur+1)); COUNT[$bucket]=$cur
    real=$(du -sm "$dest" 2>/dev/null | cut -f1)
    log "  OK $full (${bucket}) stars=$stars size=${size}kb disk=${real}M [$cur/$N]"
    echo -e "$bucket\t$full\t$stars\t$size" >> "$TSV"
  else
    log "  FAIL $full (clone error)"
    rm -rf "$dest"
  fi
done
log "DONE. corpus size: $(du -sh "$ROOT" 2>/dev/null | cut -f1)"
log "per-bucket cloned -> see $TSV"
