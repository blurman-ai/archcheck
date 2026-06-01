#!/usr/bin/env bash
# checkout_watchdog.sh — следит за зависшими `git checkout` в /tmp/drift_all_wt.
# Если один и тот же checkout висит дольше MAX сек — убивает его, чтобы harness
# (drift_all.sh) двинулся к следующему снапшоту. Метрика зависшего снапшота
# станет NA, но прогон не застрянет навечно на репе-патологии (ice, BOAZ).
# Самозавершается, когда drift_all.sh завершился.
set -u
MAX=200            # сек на один checkout
prev_pid=""; prev_seen=0
while pgrep -f 'drift_all.sh' >/dev/null 2>&1; do
  pid=$(pgrep -f '/tmp/drift_all_wt checkout' | head -1)
  if [ -n "$pid" ]; then
    if [ "$pid" = "$prev_pid" ]; then
      now=$(date +%s)
      if [ $((now - prev_seen)) -ge "$MAX" ]; then
        echo "$(date +%H:%M:%S) WATCHDOG: убиваю зависший checkout pid=$pid (>${MAX}s)" \
          >> ~/projects/cpparch/experiments/ai_repo_run/watchdog.log
        kill -9 "$pid" 2>/dev/null
        # добить возможный фоновый pack-objects этой репы
        pkill -9 -f 'git pack-objects' 2>/dev/null
        prev_pid=""; prev_seen=0
      fi
    else
      prev_pid="$pid"; prev_seen=$(date +%s)
    fi
  else
    prev_pid=""; prev_seen=0
  fi
  sleep 20
done
echo "$(date +%H:%M:%S) WATCHDOG: drift_all завершён, выхожу" \
  >> ~/projects/cpparch/experiments/ai_repo_run/watchdog.log
