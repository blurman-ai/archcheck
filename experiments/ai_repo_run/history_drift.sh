#!/usr/bin/env bash
# history_drift.sh — измеряет архитектурный дрейф ПО ИСТОРИИ одной репы.
#
# Постановка (#054): нас не интересует автор коммита. Интересует коммит как
# СОБЫТИЕ дрейфа — после которого архитектура стала хуже:
#   - в графе:    появился цикл / выросли рёбра (archcheck --diff: grown_cycles, added_edges)
#   - в копипасте: вырос duplication ratio / cross-file блоки (line_duplication spike)
#
# Метод: выбираем N равномерных снапшотов по истории, для каждого интервала
# считаем граф-дельту (memory, без checkout), для каждого снапшота — dup
# (через checkout во временном worktree, master не трогаем). Получаем два
# временных ряда; всплеск = коммит(ы)-виновник(и), затем zoom вручную.
#
# Usage: history_drift.sh <repo> <subpath-rel> <N> <tag>
#   repo        — путь к git-репе (полная история, не shallow!)
#   subpath-rel — поддиректория для скана (относит. repo), отсекает vendored
#   N           — число снапшотов (пилот: 30)
#   tag         — имя прогона → runs_history/<tag>/
set -u

REPO="${1:?repo path}"
SUBREL="${2:?subpath relative to repo}"
N="${3:-30}"
TAG="${4:?run tag}"

ARCHCHECK=~/projects/cpparch/build/debug/src/archcheck
DUP=/tmp/line_dup_build/line_duplication
DUP_EXCLUDES=(--exclude include/imgui --exclude precompile --exclude external)

OUTDIR="~/projects/cpparch/experiments/ai_repo_run/runs_history/$TAG"
mkdir -p "$OUTDIR"
SERIES="$OUTDIR/series.tsv"
GRAPHLOG="$OUTDIR/graph_diff.log"
: > "$GRAPHLOG"

# --- выбрать N равномерных коммитов (oldest→newest) ---
# --first-parent: только магистраль. Без неё rev-list тащит коммиты с боковых
# веток (включая транзишн-обрубки, где в дереве 8-28 файлов) — они дают ложные
# провалы метрик. Магистральные снапшоты репрезентативны. (находка прогона #054)
mapfile -t ALL < <(git -C "$REPO" rev-list --reverse --first-parent HEAD)
TOTAL=${#ALL[@]}
echo "repo=$REPO total_commits=$TOTAL sampling N=$N subpath=$SUBREL"
declare -a SAMP
if [ "$N" -ge "$TOTAL" ]; then
  SAMP=("${ALL[@]}")
else
  for ((i=0; i<N; i++)); do
    idx=$(( i * (TOTAL-1) / (N-1) ))
    SAMP+=("${ALL[$idx]}")
  done
fi
echo "sampled ${#SAMP[@]} commits"

# --- ОДИН worktree на весь прогон; per-snapshot только checkout --detach ---
# (worktree add/remove на каждой итерации переписывает всё дерево — медленно;
#  один worktree + checkout переписывает лишь изменённые файлы.)
WT="/tmp/drift_wt_$TAG"
git -C "$REPO" worktree remove --force "$WT" 2>/dev/null || true
rm -rf "$WT"
git -C "$REPO" worktree add --force --detach "$WT" "${SAMP[0]}" >/dev/null 2>&1

printf "idx\tsha\tdate\tdup_ratio\tcross_file\tnodes\tedges\tsccs_cyclic\tlargest_scc\n" > "$SERIES"

# Обе метрики считаем на checkout-дереве (диск, мгновенно). Граф — АБСОЛЮТНЫЕ
# значения per-снапшот (--graph, не медленный memory --diff); дельта sccs_cyclic
# между снапшотами = где появились циклы. dup — ratio + cross-file.
for ((i=0; i<${#SAMP[@]}; i++)); do
  sha="${SAMP[$i]}"
  short=$(git -C "$REPO" rev-parse --short "$sha")
  date=$(git -C "$REPO" log -1 --format=%as "$sha")

  git -C "$WT" checkout --detach --force "$sha" >/dev/null 2>&1

  ratio="NA"; cross="NA"; nodes="NA"; edges="NA"; cyc="NA"; lscc="NA"
  if [ -d "$WT/$SUBREL" ]; then
    # копипаст-метрика
    dupout=$("$DUP" "$WT/$SUBREL" "${DUP_EXCLUDES[@]}" 2>/dev/null)
    ratio=$(echo "$dupout" | awk '/duplication ratio:/{gsub(/%/,"",$3);print $3}')
    cross=$(echo "$dupout" | awk '/cross-file blocks:/{print $3}')
    # граф-метрика (абсолют на снапшоте)
    g=$("$ARCHCHECK" --graph "$WT/$SUBREL" 2>/dev/null)
    echo "=== $short ($date) ===" >> "$GRAPHLOG"; echo "$g" >> "$GRAPHLOG"
    nodes=$(echo "$g" | awk -F: '/^nodes:/{gsub(/ /,"",$2);print $2}')
    edges=$(echo "$g" | awk -F: '/^edges:/{gsub(/ /,"",$2);print $2}')
    cyc=$(echo "$g"   | awk -F: '/^sccs_cyclic:/{gsub(/ /,"",$2);print $2}')
    lscc=$(echo "$g"  | awk -F: '/^largest_scc:/{gsub(/ /,"",$2);print $2}')
  fi

  printf "%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
    "$i" "$short" "$date" "${ratio:-NA}" "${cross:-NA}" "${nodes:-NA}" "${edges:-NA}" "${cyc:-NA}" "${lscc:-NA}" \
    | tee -a "$SERIES"
done

git -C "$REPO" worktree remove --force "$WT" 2>/dev/null || true

echo ""
echo "series  -> $SERIES"
echo "graphlog-> $GRAPHLOG"
