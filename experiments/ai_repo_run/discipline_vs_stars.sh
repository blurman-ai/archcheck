#!/usr/bin/env bash
# discipline_vs_stars.sh — проверка гипотезы "меньше звёзд -> хуже дисциплина".
# Гоним точный line-dup (#053 спайк) whole-tree на КАЖДОЙ репе обоих корпусов,
# тегируем бакетом звёзд, агрегируем ratio/cross-file по бакетам.
#   bucket 100+   = _aidev_run/*            (50 топовых, ≥100★)
#   bucket 0-2..30-100 = _aidev_lowstar/<bucket>/*  (80 низко-звёздных)
set -u
DUP=/tmp/line_dup_build/line_duplication
OUT=/home/localadm/projects/cpparch/experiments/ai_repo_run/discipline.tsv
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/discipline.log
: > "$LOG"
echo -e "bucket\trepo\tsig_loc\tratio\tcross_file" > "$OUT"
log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

run_one(){  # $1=bucket $2=path
  local bucket="$1" d="$2" name
  name=$(basename "$d")
  # пропустить незавершённый клон (идёт прямо сейчас): есть .git/*.lock или нет HEAD
  if ls "$d"/.git/*.lock >/dev/null 2>&1 || ! git -C "$d" rev-parse HEAD >/dev/null 2>&1; then
    log "  SKIP $bucket/$name (клон не завершён / lock)"; return
  fi
  local out ratio cross sig
  out=$("$DUP" "$d" 2>/dev/null)
  ratio=$(echo "$out" | awk '/duplication ratio:/{gsub(/%/,"",$3);print $3}')
  cross=$(echo "$out" | awk '/cross-file blocks:/{print $3}')
  sig=$(echo "$out"   | awk '/significant LOC:/{print $3}')
  [ -z "$ratio" ] && ratio=NA
  [ -z "$cross" ] && cross=NA
  [ -z "$sig" ]   && sig=NA
  echo -e "$bucket\t$name\t$sig\t$ratio\t$cross" >> "$OUT"
  log "  $bucket/$name sig=$sig ratio=$ratio cross=$cross"
}

log "START discipline run"
# 100+ корпус
for d in /home/localadm/oss/_aidev_run/*/; do
  [ -d "$d/.git" ] || continue
  run_one "100+" "$d"
done
# низко-звёздные бакеты
for bdir in /home/localadm/oss/_aidev_lowstar/*/; do
  b=$(basename "$bdir")
  for d in "$bdir"*/; do
    [ -d "$d/.git" ] || continue
    run_one "$b" "$d"
  done
done
log "DONE -> $OUT"

# --- сводка по бакетам (median ratio через sort+awk) ---
echo "" | tee -a "$LOG"
echo "=== СВОДКА по бакетам (только sig_loc>=200) ===" | tee -a "$LOG"
for b in 0-2 2-10 10-30 30-100 100+; do
  vals=$(awk -F'\t' -v B="$b" 'NR>1 && $1==B && $3!="NA" && $3+0>=200 && $4!="NA"{print $4}' "$OUT" | sort -n)
  n=$(echo "$vals" | grep -c . )
  [ "$n" -eq 0 ] && { echo "  $b: нет реп с sig_loc>=200" | tee -a "$LOG"; continue; }
  med=$(echo "$vals" | awk '{a[NR]=$1} END{print (NR%2)? a[(NR+1)/2] : (a[NR/2]+a[NR/2+1])/2}')
  mean=$(echo "$vals" | awk '{s+=$1} END{printf "%.2f", s/NR}')
  echo "  bucket $b: n=$n median_ratio=$med mean_ratio=$mean" | tee -a "$LOG"
done
