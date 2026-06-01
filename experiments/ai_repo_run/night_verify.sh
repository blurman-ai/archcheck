#!/usr/bin/env bash
# Durable ночной запуск верификации (#067) через headless claude. Вызывается crontab в 03:0x.
cd /home/localadm/projects/cpparch || exit 1
LOG=/home/localadm/projects/cpparch/experiments/ai_repo_run/night_verify.log
echo "=== night_verify старт $(date '+%F %T') ===" >> "$LOG"
PROMPT="$(cat /home/localadm/projects/cpparch/experiments/ai_repo_run/night_verify_prompt.txt)"
/home/localadm/.local/bin/claude -p "$PROMPT" --dangerously-skip-permissions >> "$LOG" 2>&1
echo "=== night_verify конец $(date '+%F %T') exit=$? ===" >> "$LOG"
