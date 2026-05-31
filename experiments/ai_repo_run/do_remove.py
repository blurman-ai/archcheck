#!/usr/bin/env python3
"""Dry-run / удаление клонов по _remove_plan.tsv.

  python3 do_remove.py            # DRY RUN: считает пути+размеры, пишет REMOVED.md
  python3 do_remove.py --apply    # реально удаляет директории с диска

Источник плана — _remove_plan.tsv (repo, corpus, tag, reason) из classify_remove.py.
Пути: high-star = /home/localadm/oss/_aidev_run/<repo>;
      low-star  = /home/localadm/oss/_aidev_lowstar/<bucket>/<repo>.
"""
import csv, os, sys, shutil, subprocess

BASE = os.path.dirname(os.path.abspath(__file__))
OSS  = "/home/localadm/oss"
RUN  = os.path.join(OSS, "_aidev_run")
LOW  = os.path.join(OSS, "_aidev_lowstar")
APPLY = "--apply" in sys.argv

def du_mb(path):
    try:
        out = subprocess.run(["du", "-sm", path], capture_output=True, text=True).stdout
        return int(out.split()[0])
    except Exception:
        return 0

def resolve(repo, corpus):
    if corpus == "_aidev_run":
        p = os.path.join(RUN, repo)
        return p if os.path.isdir(p) else None
    for b in ("0-2", "2-10", "10-30", "30-100"):
        p = os.path.join(LOW, b, repo)
        if os.path.isdir(p):
            return p
    return None

rows = list(csv.DictReader(open(os.path.join(BASE, "_remove_plan.tsv"), encoding="utf-8"), delimiter="\t"))
found, missing, total_mb = [], [], 0
for r in rows:
    p = resolve(r["repo"], r["corpus"])
    if p:
        mb = du_mb(p); total_mb += mb
        found.append((r["repo"], r["corpus"], r["tag"], r["reason"], p, mb))
    else:
        missing.append((r["repo"], r["corpus"]))

# REMOVED.md — постоянный след (что и почему убрано)
lines = ["# Убранные из корпуса репозитории (#054)", "",
         "Клоны, на которых archcheck (дубли + граф) **ничего не нашёл** или нашёл",
         "**только ложное** (вендоринг / fork-in-repo / data-blob / мета-агрегатор /",
         "codegen / header-only-фантом). Удалены с диска для экономии места; данные",
         "(сводки `runs_history/*.tsv`) сохранены — список воспроизводим скриптами",
         "`classify_remove.py` → `do_remove.py`.", "",
         "**Теги причин:** `false` — ложное срабатывание/артефакт; `no-signal` —",
         "дубли плоско, циклов нет, связность не растёт (нет срабатывания).", "",
         f"Итого убрано: **{len(found)}** реп, освобождено **~{total_mb} МБ**.", ""]
for corpus, title in (("_aidev_run", "High-star (`_aidev_run`)"),
                      ("_aidev_lowstar", "Low-star (`_aidev_lowstar`)")):
    sub = [x for x in found if x[1] == corpus]
    if not sub: continue
    lines += [f"## {title} — {len(sub)} реп", "", "| репа | тег | МБ | причина |", "|---|---|---:|---|"]
    for repo, _c, tag, reason, _p, mb in sorted(sub, key=lambda x: (x[2], x[0])):
        lines.append(f"| {repo} | {tag} | {mb} | {reason} |")
    lines.append("")
if missing:
    lines += ["## Не найдено на диске (уже отсутствуют) ",
              "", ", ".join(f"{r}({c})" for r, c in missing), ""]
open(os.path.join(BASE, "REMOVED.md"), "w", encoding="utf-8").write("\n".join(lines) + "\n")

# короткий ASCII-итог (терминал не искажает)
print(f"FOUND={len(found)}")
print(f"MISSING={len(missing)}")
print(f"TOTAL_MB={total_mb}")
print(f"APPLY={APPLY}")

if APPLY:
    removed = 0
    for repo, corpus, tag, reason, p, mb in found:
        shutil.rmtree(p, ignore_errors=True)
        removed += 0 if os.path.isdir(p) else 1
    print(f"REMOVED={removed}")
