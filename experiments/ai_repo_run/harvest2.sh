#!/usr/bin/env bash
# harvest2: НОВАЯ ось поиска (sort=stars + окно pushed назад + language=c),
# чтобы найти кандидатов, которых не дал harvest1 (sort=updated, pushed>2025-09).
# Дописывает уникальных (не в all_conc, не в текущих кандидатах) в /tmp/harvest_candidates.txt.
set -u
HERE=~/projects/cpparch/experiments/ai_repo_run
CAND=/tmp/harvest_candidates.txt
RAW=/tmp/harvest2.raw
log="$HERE/harvest2.log"
: > "$RAW"; : > "$log"
echo "harvest2 старт $(date '+%F %T')" >> "$log"
q(){ # lang bucket pushed sort
  local n
  n=$(gh search repos --language="$1" --sort="$4" "pushed:$3" "stars:$2" --limit 1000 \
        --json fullName --jq '.[].fullName' 2>>"$log" | tee -a "$RAW" | wc -l)
  echo "  lang:$1 stars:$2 pushed:$3 sort:$4 -> $n" >> "$log"; sleep 8
}
# C++ по числу звёзд (другой срез, чем sort=updated) + назад во времени
for b in '>1000' '300..1000' '100..300' '30..100' '10..30' '3..10'; do
  q cpp "$b" '>2025-05-01' stars
done
# окно «активны были, потом затихли»: pushed 2025-05..2025-10, sort=updated
for b in '50..500' '10..50' '3..10' '1..3'; do
  q cpp "$b" '2025-05-01..2025-10-01' updated
done
# (C-проход убран: language:c = чистый C, не C++ — для исследования C++ это мусор)
sort -u "$RAW" > "$RAW.uniq"
# вычесть известные (all_conc) и уже-кандидатов и склонированных
ls -1 ~/oss/_aidev_dense > /tmp/_cl2.txt 2>/dev/null
awk -F'\t' 'NR==FNR{k[$1]=1;next} !($1 in k){print $1}' "$HERE/all_conc.tsv" "$RAW.uniq" > /tmp/h2_a.txt
awk 'NR==FNR{c[$1]=1;next} !($1 in c){print}' "$CAND" /tmp/h2_a.txt > /tmp/h2_b.txt
awk 'NR==FNR{cl[$1]=1;next} {n=$1; gsub("/","_",n); if(!(n in cl)) print}' /tmp/_cl2.txt /tmp/h2_b.txt > /tmp/h2_new.txt
nnew=$(wc -l < /tmp/h2_new.txt)
cat /tmp/h2_new.txt >> "$CAND"
echo "harvest2 готово $(date '+%F %T'): raw=$(wc -l <"$RAW") uniq=$(wc -l <"$RAW.uniq") НОВЫХ=$nnew (дописаны в $CAND)" >> "$log"
echo "DONE new=$nnew"
