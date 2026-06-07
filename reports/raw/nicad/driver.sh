#!/bin/bash
# Sequential driver: NiCad functions+blocks per repo (timeboxed), then archcheck --duplication.
# Runs sequentially so wall-time / peak-mem metrics are not skewed by contention.
export PATH="$HOME/bin:$PATH"
WORK=/home/localadm/nicad_work
RUN=$WORK/run_nicad.sh
ARCH=/home/localadm/projects/cpparch/build/debug/src/archcheck
RAW=/home/localadm/projects/cpparch/reports/raw
mkdir -p "$RAW"
TMO=2700   # 45 min timebox

# repo_name repo_path  (small/fast first, BambuStudio last)
REPOS=(
  "Catch2 /home/localadm/oss/Catch2"
  "GWToolboxpp /home/localadm/oss/GWToolboxpp"
  "Kartend /home/localadm/oss/Kartend"
  "IrredenEngine /home/localadm/oss/IrredenEngine"
  "LibreSprite /home/localadm/oss/LibreSprite"
  "hyyh619_irrlicht-1.8.3 /home/localadm/oss/hyyh619_irrlicht-1.8.3"
  "AetherSDR /home/localadm/oss/AetherSDR"
  "BambuStudio /home/localadm/oss/BambuStudio"
)

echo "### NiCad runs ###"
for entry in "${REPOS[@]}"; do
  set -- $entry; name=$1; path=$2
  for gran in functions blocks; do
    # skip irrlicht functions (already recorded)
    if [ "$name" = "hyyh619_irrlicht-1.8.3" ] && [ "$gran" = "functions" ]; then continue; fi
    echo ">>> $name $gran  $(date +%T)"
    bash "$RUN" "$name" "$path" "$gran" "$TMO"
  done
done

echo "### archcheck --duplication runs ###"
echo "repo,arch_wall_s,arch_peak_mb,arch_pairs" > $WORK/archcheck_metrics.csv
for entry in "${REPOS[@]}"; do
  set -- $entry; name=$1; path=$2
  echo ">>> archcheck $name $(date +%T)"
  start=$(date +%s)
  /usr/bin/time -v "$ARCH" --duplication "$path" > "$RAW/archcheck_${name}.txt" 2> "$RAW/archcheck_${name}.time"
  end=$(date +%s)
  wall=$((end-start))
  peak_kb=$(grep -i 'Maximum resident set size' "$RAW/archcheck_${name}.time" | grep -oE '[0-9]+' | tail -1)
  peak_mb=$(( ${peak_kb:-0} / 1024 ))
  pairs=$(grep -cE ' <-> | <→ |↔' "$RAW/archcheck_${name}.txt")
  echo "$name,$wall,$peak_mb,$pairs" | tee -a $WORK/archcheck_metrics.csv
done
echo "### DONE $(date +%T) ###"
