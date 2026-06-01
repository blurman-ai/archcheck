#!/usr/bin/env bash
# Финиш discovery: замер кандидатов -> отбор победителей (>=300 post-May & AI-dense) -> клон под бюджет.
#   $1 CAND  — файл кандидатов owner/repo (вывод harvest)
#   $2 BUDGET_MB (по умолч. 5000)
set -u
HERE=~/projects/cpparch/experiments/ai_repo_run
CAND="${1:-/tmp/harvest_candidates.txt}"
BUDGET_MB="${2:-5000}"
MEAS="$HERE/new300_measured.tsv"
WINNERS="$HERE/new300_winners.tsv"
BORDER="$HERE/new300_borderline.tsv"
log="$HERE/discover_finish.log"

echo "measure старт $(date '+%F %T'): $(wc -l <"$CAND") кандидатов" > "$log"
bash "$HERE/measure_candidates.sh" "$CAND" 300 8 > "$MEAS" 2>>"$log"
echo "measure готово $(date '+%F %T'): $(wc -l <"$MEAS") измерено" >> "$log"

# РЕТРАЙ CLONEFAIL: крупные репы (= самый дрейф) могли отвалиться по таймауту/сети.
# Молча терять их нельзя — переизмеряем ещё раз (таймаут уже 300с).
grep -P '\tCLONEFAIL\t' "$MEAS" | cut -f1 | sort -u > /tmp/clonefail.txt
if [ -s /tmp/clonefail.txt ]; then
  echo "ретрай CLONEFAIL: $(wc -l </tmp/clonefail.txt) реп" >> "$log"
  bash "$HERE/measure_candidates.sh" /tmp/clonefail.txt 300 4 >> "$MEAS" 2>>"$log"
fi

# ГЛАВНЫЙ ГЕЙТ = >=300 коммитов после мая (~>=1 коммит/день — сам по себе сигнал:
# характерен для ИИ, а при таком темпе дрейф архитектуры вероятен даже без ИИ).
# ИИ-плотность НЕ условие включения, а лишь ПРИОРИТЕТ в очереди клонирования.
# Тиры (каждый сорт по conc desc, затем по объёму истории):
#   tier1: conc>=50 ИЛИ ai>=150   — ИИ-плотные / большой абсолютный объём ИИ
#   tier2: conc 25..49            — смешанные human+AI
#   tier3: conc 5..24             — слабый ИИ, но высокий темп (твой сигнал: всё равно риск)
# Пол conc>=5: near-zero-AI established (Qt/LLVM) НЕ берём — стабильная архитектура,
# это анти-тезис и слив 5 ГБ. Если останется бюджет — добавим вручную.
sel(){ # conc в диапазоне [lo,hi), сорт по conc desc затем по истории
  awk -F'\t' -v lo="$1" -v hi="$2" '$2~/^[0-9]+$/&&$2>=300&&$3~/^[0-9]+$/&&$4~/^[0-9]+$/&&$4>=lo&&$4<hi' "$MEAS" \
    | sort -t$'\t' -k4,4 -nr -k2,2 -nr
}
selai(){ # ai>=N (большой абсолютный объём ИИ при любой conc)
  awk -F'\t' -v n="$1" '$2~/^[0-9]+$/&&$2>=300&&$3~/^[0-9]+$/&&$4~/^[0-9]+$/&&$3>=n' "$MEAS" \
    | sort -t$'\t' -k4,4 -nr -k2,2 -nr
}
# tier1 = conc>=50  ∪  ai>=150
{ sel 50 9999; selai 150; } | awk -F'\t' '!seen[$1]++' > "$WINNERS"
# clonelist в приоритетном порядке: tier1 → conc[25,50) → conc[5,25); дедуп сохраняет порядок
{ cat "$WINNERS"; sel 25 50; sel 5 25; } | awk -F'\t' '!seen[$1]++' > "$HERE/new300_clonelist.tsv"
cp "$HERE/new300_clonelist.tsv" "$BORDER"
echo "tier1(conc>=50|ai>=150)=$(wc -l <"$WINNERS")  всего к клону(conc>=5)=$(wc -l <"$HERE/new300_clonelist.tsv")" >> "$log"

# Клон в приоритетном порядке под бюджет; per-repo cap 1500M (крупнее — в TOOBIG-лог, не теряется).
cut -f1 "$HERE/new300_clonelist.tsv" > /tmp/winners_list.txt
bash "$HERE/clone_expand.sh" /tmp/winners_list.txt "$BUDGET_MB" 1500 discovery
echo "winners клонированы $(date '+%F %T')" >> "$log"
tail -1 "$HERE/clone_discovery.log" >> "$log"
echo "ГОТОВО $(date '+%F %T')" >> "$log"
