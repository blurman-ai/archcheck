#!/usr/bin/env bash
# Финиш discovery (фаза 2): добор остатка кандидатов с ЖЁСТКИМ таймаутом, затем тир-отбор и клон.
# Гарантирует готовый корпус к утру (не ждём полный прогон 20k+).
set -u
HERE=/home/localadm/projects/cpparch/experiments/ai_repo_run
MEAS="$HERE/new300_measured.tsv"     # уже содержит ~4066 измеренных строк (append-режим)
REMAIN="${1:-/tmp/remaining.txt}"
BUDGET_MB="${2:-5000}"
WINNERS="$HERE/new300_winners.tsv"
CLONELIST="$HERE/new300_clonelist.tsv"
log="$HERE/discover_finish2.log"

echo "measure-remaining старт $(date '+%F %T'): остаток $(wc -l <"$REMAIN"), P=14, timeout 2ч" > "$log"
timeout 7200 bash "$HERE/measure_candidates.sh" "$REMAIN" 300 14 >> "$MEAS"
echo "measure-remaining стоп $(date '+%F %T'): всего строк $(wc -l <"$MEAS")" >> "$log"

# Ретрай CLONEFAIL (крупные репы = дрейф; терять нельзя)
grep -P '\tCLONEFAIL\t' "$MEAS" | cut -f1 | sort -u > /tmp/clonefail.txt
if [ -s /tmp/clonefail.txt ]; then
  echo "ретрай CLONEFAIL: $(wc -l </tmp/clonefail.txt) реп" >> "$log"
  timeout 2400 bash "$HERE/measure_candidates.sh" /tmp/clonefail.txt 300 6 >> "$MEAS" 2>>"$log"
fi

# Тир-отбор: гейт >=300 коммитов; приоритет очереди по AI-плотности/объёму
sel(){ awk -F'\t' -v lo="$1" -v hi="$2" '$2~/^[0-9]+$/&&$2>=300&&$3~/^[0-9]+$/&&$4~/^[0-9]+$/&&$4>=lo&&$4<hi' "$MEAS" | sort -t$'\t' -k4,4 -nr -k2,2 -nr; }
selai(){ awk -F'\t' -v n="$1" '$2~/^[0-9]+$/&&$2>=300&&$3~/^[0-9]+$/&&$4~/^[0-9]+$/&&$3>=n' "$MEAS" | sort -t$'\t' -k4,4 -nr -k2,2 -nr; }
{ sel 50 9999; selai 150; } | awk -F'\t' '!seen[$1]++' > "$WINNERS"
{ cat "$WINNERS"; sel 25 50; sel 5 25; } | awk -F'\t' '!seen[$1]++' > "$CLONELIST"
echo "tier1(conc>=50|ai>=150)=$(wc -l <"$WINNERS")  clonelist(conc>=5)=$(wc -l <"$CLONELIST")" >> "$log"

# Клон в приоритетном порядке под бюджет
cut -f1 "$CLONELIST" > /tmp/winners_list.txt
bash "$HERE/clone_expand.sh" /tmp/winners_list.txt "$BUDGET_MB" 1500 discovery
echo "ALL DONE $(date '+%F %T')" >> "$log"
tail -1 "$HERE/clone_discovery.log" >> "$log"
