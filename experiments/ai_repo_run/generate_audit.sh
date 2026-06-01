#!/usr/bin/env bash
# Аудит dense-корпуса для ГЛАЗНОЙ проверки. Дубли — #056 token-precise (selective
# normalization + token-LCS@0.80, FP-фильтры), cross-file. Циклы — archcheck (свежий).
# Траектория dup% — line #053 trend (грубый тренд роста).
DENSE=/home/localadm/oss/_aidev_dense
P56=/home/localadm/projects/cpparch/experiments/partial_duplication/build/partial_duplication
ARCHCHECK=/home/localadm/projects/cpparch/build/debug/src/archcheck
OUT=/home/localadm/projects/cpparch/experiments/ai_repo_run/DENSE_AUDIT_REPORT.md
DENSEDIR=/home/localadm/projects/cpparch/experiments/ai_repo_run/runs_history/dense

subpath(){ local d="$1" s; for s in src source lib; do
  if [ -d "$d/$s" ] && find "$d/$s" \( -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.h' -o -name '*.hpp' \) 2>/dev/null|grep -q .; then echo "$s"; return; fi
done; echo "."; }

{
echo "# Аудит dense-корпуса: сигналы чекера для ГЛАЗНОЙ проверки"
echo
echo "> **Чекеру (спайки #056/#053, archcheck) на данный момент НЕ доверяем как истине.**"
echo "> Ниже — его *claims* для проверки глазами: \`cd <repo>; git checkout <HEAD-SHA>\`,"
echo "> открыть указанные \`file:line\` — реальная копипаста/цикл или ложняк."
echo ">"
echo "> - **Дубли:** детектор #056 token-precise (selective normalization + token-LCS@0.80,"
echo ">   FP-фильтры) — пары \`[token-LCS=K] file:a-b <-> file:c-d\`, только **cross-file**."
echo ">   \`line<1\` при \`LCS=1\` = diverged-копия (переименовано/правлено)."
echo "> - **Циклы:** archcheck SF.9 — цепочка \`A → … → A\` (include-цикл)."
echo "> - **Траектория dup%:** грубый тренд line-#053 по 25 снапшотам (только направление роста)."
echo "> Дата: 2026-05-31."
echo
for d in "$DENSE"/*/; do
  name=$(basename "$d"); sha=$(git -C "$d" rev-parse --short HEAD 2>/dev/null)
  date=$(git -C "$d" log -1 --format=%cd --date=short 2>/dev/null); sub=$(subpath "$d")
  tsv="$DENSEDIR/$name.tsv"
  echo "## $name"
  echo "- HEAD: \`$sha\` ($date), subpath: \`$sub\`"
  if [ -s "$tsv" ]; then
    traj=$(awk -F'\t' 'NR>1{d=$4;n=$6;c=$8;s=$9; if(f==""){fd=d;fn=n;f=1} ld=d;ln=n;mc=(c>mc?c:mc);ms=(s>ms?s:ms)} END{printf "dup %.1f%%→%.1f%% (Δ%+.1f, line-#053), nodes %d→%d, макс циклов=%d, макс SCC=%d", fd,ld,ld-fd,fn,ln,mc,ms}' "$tsv")
    echo "- Траектория (claim): $traj"
  else
    echo "- Траектория: (drift ещё считается)"
  fi
  echo '- Cross-file дубли #056 token-precise (claim, проверь глазами):'
  echo '```'
  ( cd "$d" && timeout 180 "$P56" "$sub" --partial-precise --top 40 2>/dev/null \
    | awk '/<->/{lab=$1; l=$2; r=$4; ll=l; rr=r; sub(/:.*/,"",ll); sub(/:.*/,"",rr); if(ll!=rr){print lab" "l" <-> "r; n++} } n>=8{exit}' )
  echo '```'
  cyc=$( cd "$d" && timeout 120 "$ARCHCHECK" "$sub" 2>/dev/null | grep -E '\[SF.9\] cycle:' | sed -E 's/.*\[SF.9\] cycle: //' | head -4 )
  [ -n "$cyc" ] && { echo '- Циклы включений (claim):'; echo '```'; echo "$cyc"; echo '```'; }
  echo
done
} > "$OUT"
echo "готово: $OUT ($(wc -l < "$OUT") строк)"
