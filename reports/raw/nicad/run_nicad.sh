#!/bin/bash
# NiCad eval harness: build a C-extension farm for a repo, run nicad6 {functions,blocks} c,
# capture wall time, peak RSS, parse-fail count, extracted count, clone pairs/classes, report size.
# Usage: run_nicad.sh <repo_name> <repo_path> <granularity> <timeout_sec>
set -u
export PATH="$HOME/bin:$PATH"
NICAD=/home/localadm/opt/src/NiCad-6.2
WORK=/home/localadm/nicad_work
CSV=$WORK/metrics_raw.csv

repo="$1"; src="$2"; gran="$3"; tmo="${4:-2700}"
farm="$WORK/farm_${repo}"

# ---- build farm (mirror tree, hardlink, append .c) only once per repo ----
if [ ! -d "$farm" ]; then
  while IFS= read -r f; do
    rel="${f#$src/}"
    d="$farm/$(dirname "$rel")"
    mkdir -p "$d"
    ln "$f" "$farm/${rel}.c" 2>/dev/null || cp "$f" "$farm/${rel}.c"
  done < <(find "$src" -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.h' -o -name '*.hpp' -o -name '*.hxx' -o -name '*.hh' \) )
fi
presented=$(find "$farm" -type f -name '*.c' | wc -l)

# ---- background peak-RSS sampler over txl/nicad process tree ----
peakfile=$(mktemp)
echo 0 > "$peakfile"
( while true; do
    s=$(ps -e -o rss=,comm= 2>/dev/null | awk '/txl|FindClone|ClusterPairs|Extract|Rename|Filter|Abstract|Normalize/{sum+=$1} END{print sum+0}')
    cur=$(cat "$peakfile"); if [ "${s:-0}" -gt "$cur" ]; then echo "$s" > "$peakfile"; fi
    sleep 1
  done ) &
sampler=$!

cd "$NICAD"
log="$WORK/${repo}_${gran}.run.log"
start=$(date +%s)
timeout "$tmo" /usr/bin/time -v ./nicad6 "$gran" c "$farm" > "$log" 2>&1
rc=$?
end=$(date +%s)
kill "$sampler" 2>/dev/null
wall=$((end-start))
treepeak_kb=$(cat "$peakfile"); rm -f "$peakfile"
timrepeak_kb=$(grep -i 'Maximum resident set size' "$log" | grep -oE '[0-9]+' | tail -1)
peak_kb=$(( treepeak_kb > ${timrepeak_kb:-0} ? treepeak_kb : ${timrepeak_kb:-0} ))
peak_mb=$(( peak_kb / 1024 ))

status=ok
[ $rc -eq 124 ] && status=timeout
[ $rc -ne 0 ] && [ $rc -ne 124 ] && status="rc=$rc"

# ---- parse NiCad outputs ----
parsefail=$(grep -oE '[0-9]+ source files failed to parse' "$log" | grep -oE '^[0-9]+' | tail -1); parsefail=${parsefail:-0}
extracted=$(grep -oE 'Extracted [0-9]+' "$log" | grep -oE '[0-9]+' | tail -1); extracted=${extracted:-0}
pairs=$(grep -oE 'Found [0-9]+ clone pairs' "$log" | grep -oE '[0-9]+' | tail -1); pairs=${pairs:-0}
classes=$(grep -oE 'Found [0-9]+ clone classes' "$log" | grep -oE '[0-9]+' | tail -1); classes=${classes:-0}

# clone class XML (for largest-class + report size)
clonexml=$(ls -1 "${farm}_${gran}-blind-clones"/*classes-withsource*.xml 2>/dev/null | head -1)
rep_bytes=0; largest=0
if [ -n "$clonexml" ] && [ -s "$clonexml" ]; then
  rep_bytes=$(du -sb "${farm}_${gran}-blind-clones" 2>/dev/null | cut -f1)
  largest=$(grep -oE 'nclones=\"[0-9]+\"' "$clonexml" | grep -oE '[0-9]+' | sort -nr | head -1); largest=${largest:-0}
  [ "$classes" = "0" ] && classes=$(grep -c '<class ' "$clonexml")
fi

loc=$(cat $(find "$src" -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.h' -o -name '*.hpp' \) ) 2>/dev/null | wc -l)

echo "$repo,$gran,$loc,$presented,$parsefail,$extracted,$pairs,$classes,$largest,$wall,$peak_mb,$rep_bytes,$status" | tee -a "$CSV"
