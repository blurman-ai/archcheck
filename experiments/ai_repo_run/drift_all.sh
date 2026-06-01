#!/usr/bin/env bash
# drift_all.sh — НОЧНОЙ прогон дрейфа-по-истории по ВСЕМ full-history репам корпуса.
#
# Для каждой репы _aidev_run/* с полной историей (не shallow):
#   - автоопределяем subpath: src/ если есть, иначе корень репы;
#   - сэмплим N снапшотов по first-parent магистрали (адаптивно к длине истории);
#   - на каждом снапшоте: line-dup (ratio, cross-file) + archcheck --graph
#     (nodes/edges/sccs_cyclic/largest_scc);
#   - пишем серию в runs_history/all/<repo>.tsv.
# Затем строим сводку drift_summary.tsv: дельта метрик начало->конец по каждой репе,
# отсортировано по приросту копипаста и по рождению циклов.
#
# Устойчивость: одна репа упала/зависла — логируем и идём дальше (timeout на снапшот).
# Можно прерывать и перезапускать: уже готовые <repo>.tsv пропускаются.
set -u

# ROOT/OUTDIR/GLOB параметризуемы через env (дефолт = _aidev_run, как было).
# GLOB="*/" — репы прямо в ROOT; "*/*/" — на уровень глубже (lowstar/<bucket>/<repo>).
ROOT="${DRIFT_ROOT:-/home/localadm/oss/_aidev_run}"
GLOB="${DRIFT_GLOB:-*/}"
TAGDIR="${DRIFT_TAGDIR:-all}"
ARCHCHECK=/home/localadm/projects/cpparch/build/debug/src/archcheck
DUP=/tmp/line_dup_build/line_duplication
OUTDIR=/home/localadm/projects/cpparch/experiments/ai_repo_run/runs_history/$TAGDIR
SUMMARY=/home/localadm/projects/cpparch/experiments/ai_repo_run/runs_history/drift_summary_${TAGDIR}.tsv
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/drift_all_${TAGDIR}.log
N="${1:-25}"                 # снапшотов на репу (адаптивно ужмётся, если коммитов меньше)
SNAP_TIMEOUT=120             # сек на один снапшот (dup+graph) — анти-зависание
mkdir -p "$OUTDIR"
: > "$LOG"
log(){ echo "$(date +%H:%M:%S) $*" | tee -a "$LOG"; }

# excludes для dup поверх дефолтных (вендоринг, который дефолты не ловят по имени)
DUP_EXCLUDES=(--exclude imgui --exclude precompile --exclude examples --exclude tests --exclude test)

pick_subpath(){  # echo подпуть для скана относительно репы
  # Берём src-подобную папку ТОЛЬКО если в ней реально есть C/C++ исходники
  # (GSL хранит хедеры в include/gsl/ без расширения -> папка есть, файлов нет;
  #  такой subpath даёт ложный 0). Иначе скан с корня — инструмент сам рекурсивно
  # найдёт исходники, а дефолтные excludes отсекут вендоринг.
  local d="$1" s
  for s in src source lib; do
    if [ -d "$d/$s" ] && find "$d/$s" \( -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \
         -o -name '*.h' -o -name '*.hpp' -o -name '*.hh' \) 2>/dev/null | grep -q .; then
      echo "$s"; return
    fi
  done
  echo "."   # корень
}

run_repo(){
  local d="$1" name out_tsv sub
  name=$(basename "$d")
  out_tsv="$OUTDIR/$name.tsv"
  if [ -s "$out_tsv" ]; then log "SKIP $name (уже есть $out_tsv)"; return; fi

  # только полная история
  if git -C "$d" rev-parse --is-shallow-repository 2>/dev/null | grep -q true; then
    log "SKIP $name (shallow, нет истории)"; return
  fi
  local total
  total=$(git -C "$d" rev-list --count --first-parent HEAD 2>/dev/null)
  [ -z "$total" ] && { log "SKIP $name (нет коммитов)"; return; }

  sub=$(pick_subpath "$d")
  local n="$N"; [ "$n" -gt "$total" ] && n="$total"
  [ "$n" -lt 2 ] && { log "SKIP $name (история $total коммит)"; return; }

  mapfile -t ALL < <(git -C "$d" rev-list --reverse --first-parent HEAD)
  local -a SAMP=()
  for ((i=0;i<n;i++)); do SAMP+=("${ALL[$(( i*(total-1)/(n-1) ))]}"); done

  local WT="/tmp/drift_all_wt"
  git -C "$d" worktree remove --force "$WT" 2>/dev/null || true; rm -rf "$WT"
  if ! git -C "$d" worktree add --force --detach "$WT" "${SAMP[0]}" >/dev/null 2>&1; then
    log "FAIL $name (worktree add)"; return
  fi

  printf "idx\tsha\tdate\tdup_ratio\tcross_file\tnodes\tedges\tsccs_cyclic\tlargest_scc\n" > "$out_tsv"
  log "RUN  $name total=$total N=$n subpath=$sub"
  local i
  for ((i=0;i<${#SAMP[@]};i++)); do
    local sha short date ratio cross nodes edges cyc lscc scandir
    sha="${SAMP[$i]}"
    short=$(git -C "$d" rev-parse --short "$sha")
    date=$(git -C "$d" log -1 --format=%as "$sha" 2>/dev/null)
    git -C "$WT" checkout --detach --force "$sha" >/dev/null 2>&1
    ratio=NA; cross=NA; nodes=NA; edges=NA; cyc=NA; lscc=NA
    scandir="$WT/$sub"; [ "$sub" = "." ] && scandir="$WT"
    if [ -d "$scandir" ]; then
      local dupout g
      dupout=$(timeout "$SNAP_TIMEOUT" "$DUP" "$scandir" "${DUP_EXCLUDES[@]}" 2>/dev/null)
      ratio=$(echo "$dupout" | awk '/duplication ratio:/{gsub(/%/,"",$3);print $3}')
      cross=$(echo "$dupout" | awk '/cross-file blocks:/{print $3}')
      g=$(timeout "$SNAP_TIMEOUT" "$ARCHCHECK" --graph "$scandir" 2>/dev/null)
      nodes=$(echo "$g" | awk -F: '/^nodes:/{gsub(/ /,"",$2);print $2}')
      edges=$(echo "$g" | awk -F: '/^edges:/{gsub(/ /,"",$2);print $2}')
      cyc=$(echo "$g"   | awk -F: '/^sccs_cyclic:/{gsub(/ /,"",$2);print $2}')
      lscc=$(echo "$g"  | awk -F: '/^largest_scc:/{gsub(/ /,"",$2);print $2}')
    fi
    printf "%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
      "$i" "$short" "$date" "${ratio:-NA}" "${cross:-NA}" "${nodes:-NA}" "${edges:-NA}" "${cyc:-NA}" "${lscc:-NA}" >> "$out_tsv"
  done
  git -C "$d" worktree remove --force "$WT" 2>/dev/null || true
  log "DONE $name -> $out_tsv"
}

log "START drift_all N=$N over $ROOT"
for d in "$ROOT"/$GLOB; do
  [ -d "$d/.git" ] || continue
  run_repo "$d"
done
log "ALL REPOS DONE. строю сводку..."

# --- сводка: первая и последняя валидная строка каждой серии ---
printf "repo\tspan_first\tspan_last\tdup_first\tdup_last\tdup_delta\tcyc_first\tcyc_last\tlscc_max\tnodes_last\tvalid_snaps\n" > "$SUMMARY"
for tsv in "$OUTDIR"/*.tsv; do
  [ -s "$tsv" ] || continue
  name=$(basename "$tsv" .tsv)
  # baseline берём с первого снапшота, где РЕАЛЬНО есть код (nodes>0 или dup>0).
  # idx 0 часто = первый коммит репы (только README/build, 0 исходников -> dup=0,
  # nodes=0); считать от него = ложный рост 0->X. (находка корпусного прогона)
  awk -F'\t' -v R="$name" '
    function num(x){ return (x=="NA"?0:x+0) }
    NR>1 && $4!="NA" {
      has = (num($6)>0 || num($4)>0)        # есть код на этом снапшоте
      if (has) {
        if (df=="") {df=$4; d1=$3; cf=num($8)}
        dl=$4; d2=$3; cl=num($8); nl=$6
        nv++
        if (num($9)>lmax) lmax=num($9)
      }
    }
    END{
      if (df=="") {
        print R"\tNA\tNA\tNA\tNA\tNA\tNA\tNA\tNA\tNA\t0"
      } else {
        printf "%s\t%s\t%s\t%s\t%s\t%.2f\t%s\t%s\t%s\t%s\t%d\n", R,d1,d2,df,dl,(dl-df),cf,cl,(lmax==""?0:lmax),nl,nv
      }
    }' "$tsv" >> "$SUMMARY"
done
log "СВОДКА -> $SUMMARY"

echo "" | tee -a "$LOG"
echo "=== ТОП-15 по приросту копипаста (dup_delta) ===" | tee -a "$LOG"
{ head -1 "$SUMMARY"; tail -n +2 "$SUMMARY" | awk -F'\t' '$6!="NA"' | sort -t$'\t' -k6 -rn | head -15; } | column -t -s$'\t' | tee -a "$LOG"
echo "" | tee -a "$LOG"
echo "=== РЕПЫ, где РОДИЛИСЬ циклы (cyc_first=0 -> cyc_last>0) ===" | tee -a "$LOG"
{ head -1 "$SUMMARY"; tail -n +2 "$SUMMARY" | awk -F'\t' '$7==0 && $8!="NA" && $8+0>0'; } | column -t -s$'\t' | tee -a "$LOG"
