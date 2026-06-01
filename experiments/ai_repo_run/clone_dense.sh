#!/usr/bin/env bash
# Полные клоны прошедших порог реп в oss/_aidev_dense (для drift+dup стадии 2).
DST=~/oss/_aidev_dense
LIST="$1"   # TSV: repo\ttotal\tai\tconc (с заголовком)
mkdir -p "$DST"
log=~/projects/cpparch/experiments/ai_repo_run/clone_dense.log
echo "старт клонирования $(date +%H:%M:%S) из $LIST" > "$log"
tail -n +2 "$LIST" | cut -f1 | xargs -P 3 -I{} bash -c '
  repo="{}"; name=$(echo "$repo" | tr / _); dst="'"$DST"'/$name"
  [ -d "$dst/.git" ] && { echo "SKIP $repo (есть)" >> "'"$log"'"; exit 0; }
  if timeout 300 git clone --quiet "https://github.com/$repo.git" "$dst" 2>/dev/null; then
    echo "OK   $repo" >> "'"$log"'"
  else
    echo "FAIL $repo" >> "'"$log"'"; rm -rf "$dst"
  fi
'
echo "ГОТОВО $(date +%H:%M:%S): $(ls "$DST" | wc -l) реп в $DST" >> "$log"
