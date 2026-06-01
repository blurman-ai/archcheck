#!/usr/bin/env bash
DST=/home/localadm/oss/_aidev_dense
for repo in chrxh/alien makr-code/ThemisDB NamecoinGithub/NexusMiner studiocollective/songbird; do
  name=$(echo "$repo" | tr / _); dst="$DST/$name"
  [ -d "$dst/.git" ] && continue
  echo "качаю $repo ..." >> retry_big.log
  if timeout 1200 git clone --quiet "https://github.com/$repo.git" "$dst" 2>/dev/null; then
    echo "OK $repo ($(du -sh "$dst"|cut -f1))" >> retry_big.log
  else
    echo "FAIL2 $repo" >> retry_big.log; rm -rf "$dst"
  fi
done
echo "retry done $(date +%H:%M:%S)" >> retry_big.log
