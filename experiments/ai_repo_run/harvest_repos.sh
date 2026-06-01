#!/usr/bin/env bash
# Сбор кандидатов: активные C++ репы через gh search repos по star-бакетам.
# Цель funnel — репы, способные иметь >=300 коммитов после мая (значит активные).
# Вывод: уникальные owner/repo в $OUT, за вычетом уже известных (all_conc) и склонированных.
#   $1 OUT — файл результата
set -u
OUT="${1:-/tmp/harvest_candidates.txt}"
RAW="$OUT.raw"
HERE=/home/localadm/projects/cpparch/experiments/ai_repo_run
log="$HERE/harvest.log"
: > "$RAW"; : > "$log"
echo "harvest старт $(date '+%F %T')" >> "$log"

q(){ # bucket  pushed
  local b="$1" p="$2" n
  n=$(gh search repos --language=cpp --sort=updated "pushed:$p" "stars:$b" --limit 1000 \
        --json fullName --jq '.[].fullName' 2>>"$log" | tee -a "$RAW" | wc -l)
  echo "  stars:$b pushed:$p -> $n" >> "$log"
  sleep 8
}

# Высокие/средние бакеты — активность ловится sort=updated, одиночный запрос, окно >2025-09-01
for b in '>2000' '800..2000' '300..800' '120..300' '50..120' '20..50' '8..20' '3..8'; do
  q "$b" '>2025-09-01'
done
# Плотные низкозвёздные бакеты (vibe-репы) — НЕпересекающиеся месячные диапазоны, обход 1000-cap
RANGES='2026-05-01..2026-06-30 2026-04-01..2026-05-01 2026-03-01..2026-04-01 2026-02-01..2026-03-01 2026-01-01..2026-02-01 2025-12-01..2026-01-01 2025-11-01..2025-12-01 2025-10-01..2025-11-01 2025-09-01..2025-10-01'
for b in '1..3' '0..1'; do
  for r in $RANGES; do
    q "$b" "$r"
  done
done

sort -u "$RAW" > "$OUT.all"
# вычесть уже известные (all_conc col1)
awk -F'\t' 'NR==FNR{known[$1]=1; next} !($1 in known){print $1}' all_conc.tsv "$OUT.all" > "$OUT.tmp1"
# вычесть уже склонированные (по dir-name owner_repo)
ls -1 /home/localadm/oss/_aidev_dense > /tmp/_cloned_now.txt 2>/dev/null
awk 'NR==FNR{cl[$1]=1; next} {name=$1; gsub("/","_",name); if(!(name in cl)) print $1}' /tmp/_cloned_now.txt "$OUT.tmp1" > "$OUT"
echo "harvest готово $(date '+%F %T'): raw=$(wc -l <"$RAW") uniq=$(wc -l <"$OUT.all") new_candidates=$(wc -l <"$OUT")" >> "$log"
echo "DONE new=$(wc -l <"$OUT")"
